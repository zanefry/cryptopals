#!/usr/bin/env python3
# This line was xored with a single byte

from utils import *
from freq import score

ciphertxt: bytes
with open('../inputs/3.txt') as f:
    ciphertxt = bytes.fromhex(f.read().rstrip())

scores = {}
for b in range(256):
    xor = bytes([b ^ c for i, c in enumerate(ciphertxt)])

    if all(32 <= c <= 126 for c in xor):
        decoded = xor.decode('ascii')
        scores[decoded] = score(xor)

for text, score in sorted(scores.items(), key=lambda item: item[1]):
    print(f'{score}:\t{text}')
