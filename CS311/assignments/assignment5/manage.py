#! /usr/bin/env python

"""
Manage

Server to manage compute processes that find perfect numbers.
"""

from signal import signal, SIGINT
from select import select, error as SelectError
from socket import socket, AF_INET, SOCK_STREAM, SOL_SOCKET, SO_REUSEADDR
from os import kill, getpid

import logging
import logging.handlers
from Queue import Queue, Empty as QueueEmpty


LOG_FILE = "manage.log"

logger = logging.getLogger('manage')
logger.setLevel(logging.DEBUG)

log_handler = logging.handlers.RotatingFileHandler(LOG_FILE, mode='w')
log_formatter = logging.Formatter('[%(asctime)s] %(message)s')

log_handler.setFormatter(log_formatter)
logger.addHandler(log_handler)


def handler(signum, frame):
    """
    Signal handler for controlling manage
    """
    m.signal()


class Connection(object):
    """
    Represents a connection from accepting a socket
    """
    def __init__(self, conn, addr):
        self.conn = conn
        self.addr = addr

    def preformance(self, perf):
        """
        Sets the performance data of the connection
        """
        self.perf = perf

    def __repr__(self):
        return str(self.addr)


class Manage(object):
    """
    Manage connection from clients

    Initial design used from: http://pymotw.com/2/select/
    """
    log = logger.debug

    def __init__(self):
        self.seqNum = 1
        self.conns = {}
        self.inputs = []
        self.outputs = []
        self.curWorker = None
        self.mq = {}

    def init(self, HOST, PORT, BACKLOG):
        """
        Setup an non-blocking IPv4 stream socket. Reusing the socket if
        available, and listening for BACKLOG number of connections.
        """
        s = socket(AF_INET, SOCK_STREAM)
        s.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
        s.setblocking(0)
        s.bind((HOST, PORT))
        s.listen(BACKLOG)
        # Add socket to list of available inputs
        self.server = s
        self.inputs.append(s)
        self.log("Bound socket to port: %s", PORT)
        self.log("Sequence initialized to: %s", self.seqNum)

    def run(self):
        while self.inputs:
            try:
                readable, writeable, exceptions = select(self.inputs,
                                                         self.outputs,
                                                         self.inputs)
            except (SelectError) as err:
                self.log("Select died: %s", err)
                break

            # Readable
            for s in readable:
                if s is self.server:
                    # New connection incoming
                    self.accept(s)
                else:
                    self.recieve(s)

            # Writeable
            for s in writeable:
                self.send(s)

            # Exceptions
            for s in exceptions:
                self.remove(s)

    def recieve(self, s):
        """
        Recieve and handle input for socket s
        """
        data = s.recv(1024)
        if data:
            # Socket with available data, remove newline, and any
            # trailing whitespace
            data = data.rstrip()
            self.parse(data, s)
        else:
            # Socket without data, client is done or dead
            self.remove(s)

    def parse(self, data, s):
        """
        Parse incoming data for socket s
        """
        fn = s.fileno()

        if data.isdigit():
            self.log("Recieved range: %s", data)
            self.mq[fn].put(str(self.seqNum) + '\n')
            self.seqNum += int(data)
            self.curWorker = str(self.conns[s])
            self.log("Sequence now: %s", self.seqNum)
        elif data == "quit":
            self.log("Server shutting down.")
            self.remove(s)
            # Goto self.signal
            kill(getpid(), SIGINT)
        elif data == "current":
            worker = self.curWorker
            if not worker:
                worker = "None"
            self.mq[fn].put(str(worker) + '\n')

        if s not in self.outputs:
            self.outputs.append(s)

    def accept(self, s):
        """
        Accept an incoming socket connection
        """
        conn, addr = s.accept()
        conn.setblocking(0)

        self.conns[conn] = Connection(conn, addr)
        self.inputs.append(conn)
        self.log("New connection from: %s", addr)
        self.mq[conn.fileno()] = Queue()

    def send(self, s):
        """
        Send messages from socket queue to socket.
        """
        try:
            next_msg = self.mq[s.fileno()].get_nowait()
        except QueueEmpty:
            #print >>sys.stderr, 'output queue for', s.getpeername(), 'is empty'
            # No messages waiting so stop checking for writability.
            self.outputs.remove(s)
        else:
            #print >>sys.stderr, 'sending "%s" to %s' % (next_msg, s.getpeername())
            s.send(next_msg)

    def remove(self, s):
        """
        Close the socket 's'
          - remove message queue
          - remove from outputs, inputs
        """
        if s in self.outputs:
            self.outputs.remove(s)
        self.inputs.remove(s)
        s.close()

    def flush(self, s):
        """
        Flush a sockets output queue
        """
        self.log("Flushing: %s", self.conns[s])

        while not self.mq[s.fileno()].empty():
            self.send(s)

    def flushall(self):
        """
        Flush all connected sockets output queues
        """
        for s in self.conns:
            self.flush(s)

    def signal(self):
        """
        Signal handling for Manage object

        Reads in rest of data from clients, sends 'kill' message to all
        clients and exits
        """
        self.log("Closing connections.")

        # Flush all output buffer
        self.flushall()

        # There should be no more outputs
        if self.outputs:
            self.log("Error: outputs not all flushed")

        # Clear input buffer and tell client to die
        for s in self.inputs:
            if s is not self.server:
                self.mq[s.fileno()].put_nowait("die\n")
                if s not in self.outputs:
                    self.outputs.append(s)
                self.inputs.remove(s)

        self.flushall()
        exit(0)
            
# Create a manage global object
m = Manage()
            
if __name__ == "__main__":
    signal(SIGINT, handler)

    m.init('', 50000, 5)
    m.run()
