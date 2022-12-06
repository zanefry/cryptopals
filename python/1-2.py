#!/usr/bin/env python3
# Xor these equal-length buffers together

from utils import *

h0: str
h1: str
correct_out: str

with open('../inputs/2.txt') as f:
    h0, h1, correct_out = [l.rstrip() for l in f.readlines()]

b0 = bytes.fromhex(h0)
b1 = bytes.fromhex(h1)

num_bytes = len(b0)

result = bytes([x ^ y for x, y in zip(b0, b1)])

print('correct' if result.hex() == correct_out else 'incorrect')
