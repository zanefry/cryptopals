#!/usr/bin/env python3
# Break repeating key xor
# 1) Determine key size
#   * For each keysize compute the distance between the first and second keysize blocks
#   * Scale the distance by the keysize
#   * Compare scaled distances: the smallest is likely from the actual keysize
# 2) Determine key
#   * Partition ciphertxt into {block0[0], block1[0], ...}, {block0[1], block1[1], ...} etc
#   * Assume each slice has been xored by the same byte and solve like problem 3
#   * Recreate full key from bytes

from utils import *
from freq import score

ciphertxt: bytes
with open('../inputs/6.txt') as f:
    ciphertxt = base64.b64decode(f.read())

keysize_scores = []
for keysize in range(2, 40):
    block0 = ciphertxt[:keysize]

    blocks = []
    i = 1
    while (i + 1)*keysize < len(ciphertxt):
        blocks.append(ciphertxt[i*keysize:(i + 1)*keysize])
        i += 1

    total_dist = sum([hamming_dist(block0, blocks) for block in blocks])
    avg_dist = total_dist / (len(blocks) - 1)
    scaled_dist = avg_dist / keysize

    keysize_scores.append((scaled_dist, keysize))

min_dist_keysize = sorted(keysize_scores)[0][1]
print(f'{min_dist_keysize=}')

slices = [[] for _ in range(min_dist_keysize)]
for i, byte in enumerate(ciphertxt):
    slices[i % min_dist_keysize].append(byte)

candidates = []
for slice in slices:
    scores = []
    for b in range(256):
        xor = bytes([b ^ c for i, c in enumerate(slice)])
        scores.append((score(xor), b))

    scores.sort()
    candidates.append([b for score, b in scores[:3]])

for l in candidates: print(l)













