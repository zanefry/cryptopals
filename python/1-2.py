#!/usr/bin/env python3
# Xor these equal-length buffers together

from utils import *

h0:          str
h1:          str
correct_out: str

with open('../inputs/2.txt') as f:
    h0, h1, correct_out = [l.rstrip() for l in f.readlines()]

b0 = bytes.fromhex(h0)
b1 = bytes.fromhex(h1)

xord = bytes([b0[i] ^ b1[i] for i in range(len(b0))]).hex()

print('correct' if xord == correct_out else 'incorrect')
