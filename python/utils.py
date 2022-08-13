#!/usr/bin/env python3
import base64

def hex_to_b64(hex: str) -> str:
    raw = bytes.fromhex(hex)
    return base64.b64encode(raw).decode('ascii')

def b64_to_hex(b64: str) -> str:
    raw = base64.b64decode(b64)
    return raw.hex()

def score(buf: bytes) -> int:
    freq_list = 'eothasinrdluymwfgcbpkvjqxz'

    score = 0
    for b in buf:
        if b <= 127 and chr(b).isalpha():
            score += 26 - freq_list.index(chr(b).lower())

    return score

def repeating_xor(buf: bytes, key: bytes) -> bytes:
    return bytes([b ^ key[i % len(key)] for i, b in enumerate(buf)])
