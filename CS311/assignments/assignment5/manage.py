#! /usr/bin/env python

"""
Manage

Server to manage compute processes that find perfect numbers.
"""

from signal import signal, SIGINT
from select import select
from socket import (socket, AF_INET, SOCK_STREAM, SOL_SOCKET,
                    SO_REUSEADDR, error as SocketError)
from os import kill, getpid

import logging
import logging.handlers
from Queue import Queue, Empty as QueueEmpty
import re

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
        self.mq = {}
        self.perfect_numbers = []
        self.curWorker = None
        self.accepting = True

    def init(self, HOST, PORT, BACKLOG):
        """
        Setup an non-blocking IPv4 stream socket. Reusing the socket if
        available, and listening for BACKLOG number of connections.
        """
        s = socket(AF_INET, SOCK_STREAM)
        s.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
        #s.setblocking(0)
        s.bind((HOST, PORT))
        s.listen(BACKLOG)
        # Add socket to list of available inputs
        self.server = s
        self.inputs.append(s)
        self.log("Bound socket to port: %s", PORT)
        self.log("Sequence initialized to: %s", self.seqNum)

    def run(self):
        """
        Run Manage

        Select sockets, create new connections, and send/recv data.

        For reading:
            If the socket is the server, check for and recieve new
            connections.
            Otherwise, if the socket has data, parse it, else remove it.
        For writing:
            If the socket is in outputs, send queued data.
        For errors:
            Just close the socket and remove it from all lists
        """
        while self.inputs:
            readable, writeable, exceptions = select(self.inputs,
                                                     self.outputs,
                                                     self.inputs)
            for s in readable:
                if s is self.server and self.accepting:
                    self.accept(s)
                else:
                    data = s.recv(1024)
                    if data:
                        self.parse(data.rstrip(), s)
                    else:
                        self.remove(s)

            # Writeable
            for s in writeable:
                self.send(s)

            # Exceptions
            for s in exceptions:
                self.remove(s)

    def parse(self, data, s):
        """
        Parse incoming data for socket s
        """
        fn = s.fileno()

        perf = re.compile("<xml><perf>(\d+)</perf></xml>")
        numbers = re.compile("<xml>(?:<number>(\d+)</number>)+</xml>")

        if not self.accepting:
            self.mq[fn].put_nowait("die\n")
            self.inputs.remove(s)
        else:
            self.log("Recieved %s from %s", repr(data),
                    self.conns[s].addr)

            has_perf = perf.match(data)
            has_nums = numbers.match(data)

            if data == "quit":
                # Goto self.signal
                kill(getpid(), SIGINT)
            elif data == "current":
                worker = self.curWorker
                if not worker:
                    worker = "None"
                self.mq[fn].put(str(worker) + '\n')
            elif has_perf:
                self.mq[fn].put("<xml><range>"+str(self.seqNum)+"</range></xml>")
                self.seqNum += int(has_perf.group(1))
                self.curWorker = str(self.conns[s])
                self.log("Sequence now: %s", self.seqNum)
            elif has_nums:
                self.perfect_numbers.append(list(has_nums.groups()))
                self.log("Found perfect numbers: %s",
                         str(list(has_nums.groups())))

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
        except SocketError:
            self.remove(s)
        else:
            self.log("Sending '%s' to %s", repr(next_msg), self.conns[s].addr)
            s.send(next_msg)

    def remove(self, s):
        """
        Close the socket 's'
          - remove it from message queues
          - remove it from inputs and outputs list
        """
        if s in self.outputs:
            self.outputs.remove(s)
        self.inputs.remove(s)
        del self.conns[s]
        s.close()

    def signal(self):
        """
        Signal handling for Manage object

        Reads in rest of data from clients, sends 'kill' message to all
        clients and exits
        """
        self.log("Server shutting down.")
        self.accepting = False
        
        for s in self.inputs:
            if s is self.server:
                self.inputs.remove(s)
            
# Create a manage global object
m = Manage()
            
if __name__ == "__main__":
    signal(SIGINT, handler)

    m.init('', 50000, 5)
    m.run()
