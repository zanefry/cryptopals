#!/usr/bin/env python3
import sys, base64

def hex_to_b64(hex: str) -> str:
    raw = bytes.fromhex(hex)
    return base64.b64encode(raw).decode('ascii')

def b64_to_hex(b64: str) -> str:
    raw = base64.b64decode(b64)
    return raw.hex()

def repeating_xor(buf: bytes, key: bytes) -> bytes:
    return bytes([b ^ key[i % len(key)] for i, b in enumerate(buf)])

def hamming_dist(b0: bytes, b1: bytes) -> int:
    if len(b0) != len(b1):
        sys.exit('hamming_dist called on buffers of unequal length')

    sum = 0
    for i in range(len(b0)):
        sum += (b0[i] ^ b1[i]).bit_count()

    return sum
