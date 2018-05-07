#!/usr/bin/env python3

import socket
import gzip
import requests

# TCP_IP = 'localhost'
# TCP_PORT = 12345
# BUFFER_SIZE = 1024

# s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# s.connect((TCP_IP, TCP_PORT))

while 1:
    msg = input()
    if msg != "get":
        #data = gzip.compress(msg.encode())
        data = msg.encode()
        r = requests.post("http://localhost:12345/esw/myserver/data", data)
        print(r.status_code)
    else:
        r = requests.get("http://localhost:12345/esw/myserver/count")
        print(r.text)

# s.close()