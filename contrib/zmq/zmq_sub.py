#!/usr/bin/env python
# Copyright (c) 2023-2024 The Hellar Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

import binascii
import zmq
import struct

port = 28332

zmqContext = zmq.Context()
zmqSubSocket = zmqContext.socket(zmq.SUB)
zmqSubSocket.setsockopt(zmq.SUBSCRIBE, b"hashblock")
zmqSubSocket.setsockopt(zmq.SUBSCRIBE, b"hashtx")
zmqSubSocket.setsockopt(zmq.SUBSCRIBE, b"hashtxlock")
zmqSubSocket.setsockopt(zmq.SUBSCRIBE, b"hashgovernancevote")
zmqSubSocket.setsockopt(zmq.SUBSCRIBE, b"hashgovernanceobject")
zmqSubSocket.setsockopt(zmq.SUBSCRIBE, b"hashinstantsenddoublespend")
zmqSubSocket.setsockopt(zmq.SUBSCRIBE, b"rawblock")
zmqSubSocket.setsockopt(zmq.SUBSCRIBE, b"rawtx")
zmqSubSocket.setsockopt(zmq.SUBSCRIBE, b"rawtxlock")
zmqSubSocket.setsockopt(zmq.SUBSCRIBE, b"rawgovernancevote")
zmqSubSocket.setsockopt(zmq.SUBSCRIBE, b"rawgovernanceobject")
zmqSubSocket.setsockopt(zmq.SUBSCRIBE, b"rawinstantsenddoublespend")
zmqSubSocket.connect("tcp://127.0.0.1:%i" % port)

try:
    while True:
        msg = zmqSubSocket.recv_multipart()
        topic = str(msg[0].decode("utf-8"))
        body = msg[1]
        sequence = "Unknown"

        if len(msg[-1]) == 4:
          msgSequence = struct.unpack('<I', msg[-1])[-1]
          sequence = str(msgSequence)

        if topic == "hashblock":
            print('- HASH BLOCK ('+sequence+') -')
            print(binascii.hexlify(body).decode("utf-8"))
        elif topic == "hashtx":
            print ('- HASH TX ('+sequence+') -')
            print(binascii.hexlify(body).decode("utf-8"))
        elif topic == "hashtxlock":
            print('- HASH TX LOCK ('+sequence+') -')
            print(binascii.hexlify(body).decode("utf-8"))
        elif topic == "rawblock":
            print('- RAW BLOCK HEADER ('+sequence+') -')
            print(binascii.hexlify(body[:80]).decode("utf-8"))
        elif topic == "rawtx":
            print('- RAW TX ('+sequence+') -')
            print(binascii.hexlify(body).decode("utf-8"))
        elif topic == "rawtxlock":
            print('- RAW TX LOCK ('+sequence+') -')
            print(binascii.hexlify(body).decode("utf-8"))
        elif topic == "rawinstantsenddoublespend":
            print('- RAW IS DOUBLE SPEND ('+sequence+') -')
            print(binascii.hexlify(body).decode("utf-8"))
        elif topic == "hashgovernancevote":
            print('- HASH GOVERNANCE VOTE ('+sequence+') -')
            print(binascii.hexlify(body).decode("utf-8"))
        elif topic == "hashgovernanceobject":
            print('- HASH GOVERNANCE OBJECT ('+sequence+') -')
            print(binascii.hexlify(body).decode("utf-8"))
        elif topic == "rawgovernancevote":
            print('- RAW GOVERNANCE VOTE ('+sequence+') -')
            print(binascii.hexlify(body).decode("utf-8"))
        elif topic == "rawgovernanceobject":
            print('- RAW GOVERNANCE OBJECT ('+sequence+') -')
            print(binascii.hexlify(body).decode("utf-8"))
        elif topic == "hashinstantsenddoublespend":
            print('- HASH IS DOUBLE SPEND ('+sequence+') -')
            print(binascii.hexlify(body).decode("utf-8"))

except KeyboardInterrupt:
    zmqContext.destroy()
