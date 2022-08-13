#!/usr/bin/env python3
# Xor this text with the repeating key 'ICE'

from utils import *

input: str
correct_output: str
with open('../inputs/5.txt') as f:
    input, correct_output = f.read().split('\n\n')
    correct_output = correct_output.replace('\n', '')

if repeating_xor(input.encode('ascii'), b'ICE').hex() == correct_output:
    print('correct')
else:
    print('incorrect')
