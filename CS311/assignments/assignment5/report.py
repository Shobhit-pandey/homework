#! /usr/bin/env python
"""
Basic client code used from: http://pymotw.com/2/select/
"""
import socket
import sys

messages = ['results',
            'quit']
server_address = ('localhost', 50000)

# Create a TCP/IP socket
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening
print >>sys.stderr, 'Connecting to %s port %s' % server_address
s.connect(server_address)

for message in messages:
    # Send messages on both sockets
    print >>sys.stderr, '%s: sending "%s"' % (s.getsockname(), message)
    s.send(message)

    # Read responses on both sockets
    data = s.recv(1024)
    print >>sys.stderr, '%s: received "%s"' % (s.getsockname(), data)
    if not data:
        print >>sys.stderr, 'closing socket', s.getsockname()
        s.close()
