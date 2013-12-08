#! /usr/bin/env python
"""
Basic client code used from: http://pymotw.com/2/select/
"""
import socket
import sys

messages = ['results', 'die']
reponse = [
    ('<?xml version="1.0" encoding="UTF-8"?>'
     '<perfect><number>1</number><number>6</number></perfect>'
     '<tested><min>1</min><max>35234234</max></tested>'
     '<clients>'
     '<client><ip>127.0.0.1</ip><port>53234</port><perf>53234</perf></client>'
     '<client><ip>127.0.0.1</ip><port>53524</port><perf>23334</perf></client>'
     '</clients>')
]
HOST = 'localhost'
PORT = 50000
server_address = (HOST, PORT)

# Create a TCP/IP socket
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening
print >>sys.stderr, 'Connecting to %s port %s' % server_address
try:
    s.connect(server_address)
except socket.error:
    print >>sys.stderr, 'Connection refused'
    exit(1)

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
