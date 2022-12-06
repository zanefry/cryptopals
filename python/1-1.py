#!/usr/bin/env python3
# Correctly decode these bytes from hex and encode them to base64

from utils import *

hex_input: str
correct_b64: str
with open('../inputs/1.txt') as f:
    hex_input, correct_b64 = [l.rstrip() for l in f.readlines()]

b64 = hex_to_b64(hex_input)

print('correct' if b64 == correct_b64 else 'incorrect')
