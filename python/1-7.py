#!/usr/bin/env python3

import base64
from aes import ecb_dec

ciphertxt: bytes
with open('../inputs/7.txt') as f:
    ciphertxt = base64.b64decode(''.join([l.rstrip() for l in f.readlines()]))

print(ecb_dec(ciphertxt, b'YELLOW SUBMARINE'))
