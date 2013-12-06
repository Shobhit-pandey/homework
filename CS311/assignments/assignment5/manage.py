#! /usr/bin/env python

"""
Manage

Server to manage compute processes that find perfect numbers.
"""

from select import select
from socket import socket, AF_INET, SOCK_STREAM, SOL_SOCKET, SO_REUSEADDR
from Queue import Queue, Empty

"""
"""


class Connection(object):
    """
    Represents a connection from accepting a socket
    """
    def __init__(self, conn, addr):
        self.conn = conn
        self.addr = addr


class Manage(object):
    """
    Manage connection from clients

    Initial design used from: http://pymotw.com/2/select/
    """
    seqNum = 1
    inputs = []
    outputs = []
    mq = {}

    def init(self, HOST, PORT, BACKLOG):
        s = socket(AF_INET, SOCK_STREAM)
        s.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
        s.setblocking(0)
        s.bind((HOST, PORT))
        s.listen(BACKLOG)
        # Add socket to list of available inputs
        self.server = s
        self.inputs.append(s)
        print("Bound socket to port {0}".format(PORT))

    def run(self):
        while self.inputs:
            readable, writeable, exceptions = select(self.inputs,
                                                     self.outputs,
                                                     self.inputs)
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
        fn = s.fileno()

        data = s.recv(1024)
        if data:
            # Socket with available data
            num = data[:-2]
            if num.isdigit():
                self.mq[fn].put(str(self.seqNum) + '\n')
                self.seqNum += int(num)
            if s not in self.outputs:
                self.outputs.append(s)
        else:
            # Socket without data, client is done or dead
            #print("Closing connection from:"
            #      "{0}".format(addr))
            self.remove(s)

    def accept(self, s):
        """
        Accept an incoming socket connection
        """
        conn, addr = s.accept()
        conn.setblocking(0)

        self.inputs.append(conn)
        print("New connection from: {0}".format(addr))
        self.mq[conn.fileno()] = Queue()

    def send(self, s):
        """
        Send messages from socket queue to socket.
        """
        try:
            next_msg = self.mq[s.fileno()].get_nowait()
        except Empty:
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
        del self.mq[s.fileno()]
        s.close()
            
if __name__ == "__main__":
    m = Manage()
    m.init('', 50000, 5)

    m.run()
    m.close()
