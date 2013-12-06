#! /usr/bin/env python

"""
Manage

Server to manage compute processes that find perfect numbers.
"""

from socket import socket, AF_INET, SOCK_STREAM, SOL_SOCKET, SO_REUSEADDR


class Manage(object):
    """
    Manage connection from clients
    """
    s = None
    conn = None
    clients = []
    seqNum = 1

    def init(self, HOST, PORT):
        self.s = socket(AF_INET, SOCK_STREAM)
        self.s.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
        self.s.bind((HOST, PORT))
        self.s.listen(1)
        print("Bound socket to port {0}".format(PORT))

    def run(self):
        conn, addr = self.s.accept()
        self.conn = conn
        print("Conn: {0}".format(addr))
        self.clients.append(addr)
        while True:
            data = conn.recv(1024)
            if not data:
                break
            num = data[:-2]
            if num.isdigit():
                conn.sendall(str(self.seqNum)+'\n')
                self.seqNum += int(num)

    def close(self):
        self.conn.close()

if __name__ == "__main__":
    m = Manage()
    m.init('', 50000)

    m.run()
    m.close()
