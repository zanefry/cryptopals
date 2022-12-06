#!/usr/bin/env python3
# Xor this text with the repeating key 'ICE'

from utils import *

hex_input: str
correct_output: str
with open('../inputs/5.txt') as f:
    hex_input, correct_output = f.read().split('\n\n')
    correct_output = correct_output.replace('\n', '')

result = repeating_xor(hex_input.encode('ascii'), b'ICE')

print('correct' if result.hex() == correct_output else 'incorrect')
