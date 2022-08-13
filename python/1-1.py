#!/usr/bin/env python3
# Correctly decode these bytes from hex and encode them to base64

from utils import *

hex_input = ''
correct_b64 = ''
with open('../inputs/1.txt') as f:
    lines = f.readlines()
    hex_input = lines[0].rstrip()
    correct_b64 = lines[1].rstrip()

b64 = hex_to_b64(hex_input)

print('correct' if b64 == correct_b64 else 'incorrect')
