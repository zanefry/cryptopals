#!/usr/bin/env python3
# One of these lines has been xored with a single byte

from freq import score

lines: list
with open('../inputs/4.txt') as f:
    lines = [bytes.fromhex(l.rstrip()) for l in f.readlines()]

all_scores = []
for raw in lines:
    for b in range(256):
        xor = bytes([b ^ c for c in raw])
        all_scores.append((xor, score(xor)))

top_sorted = sorted(all_scores, key=lambda pair: pair[1])[:5]

for buf, score in top_sorted:
    if all(b <= 127 for b in buf):
        print(f"{score}:\t{buf.decode('ascii')}")

