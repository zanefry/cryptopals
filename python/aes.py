#!/usr/bin/env python3

log_lut =   [0] * 256
exp_lut =   [0] * 256
Sbox =      [0] * 256
inv_Sbox =  [0] * 256

tables_initialized = False

def init_tables(): # using 3 as a generator for GF(2^8)
    global tables_initialized
    if tables_initialized: return
    else: tables_initialized = True

    a = 1
    for i in range(256):
        exp_lut[i] = a # 3^i = a
        log_lut[a] = i # log3(a) = i

        a ^= (a << 1) ^ (0x1b if (a & 0x80) else 0) # a *= 3
        a &= 0xff

    init_Sbox()

def init_Sbox():
    def rotl_byte(a: int, n: int) -> int:
        for _ in range(n):
            hi_bit = a & 0x80
            a = (a << 1) & 0xff
            a |= hi_bit >> 7

        return a

    for n in range(1, 256):
        inv = ginv(n)
        Sbox[n] =   inv ^ \
                    rotl_byte(inv, 1) ^ \
                    rotl_byte(inv, 2) ^ \
                    rotl_byte(inv, 3) ^ \
                    rotl_byte(inv, 4) ^ \
                    0x63

        inv_Sbox[Sbox[n]] = n

    Sbox[0] = 0x63
    inv_Sbox[0x63] = 0

### Field Ops ###

def gmul(a: int, b: int) -> int:
    init_tables()

    if not a or not b: return 0
    return exp_lut[(log_lut[a] + log_lut[b]) % 0xff]

def ginv(a: int) -> int:
    if not a:
        raise ValueError('0 has no multiplicative inverse')

    init_tables()
    return exp_lut[0xff - log_lut[a]]

def gdiv(a: int, b: int) -> int:
    if not b:
        raise ValueError('division by 0')

    init_tables()
    return exp_lut[(log_lut[a] - log_lut[b]) % 0xff]

### Transformations ###

def subBytes(block: bytearray):
    init_tables()

    for i, b in enumerate(block):
        block[i] = Sbox[b]

def isubBytes(block: bytearray):
    init_tables()

    for i, b in enumerate(block):
        block[i] = inv_Sbox[b]

def shiftRows(block: bytearray):
    for r in range(4):
        row = [block[4*c + r] for c in range(4)]
        row = row[r:] + row[:r]

        for c in range(4):
            block[4*c + r] = row[c]

def ishiftRows(block: bytearray):
    for r in range(4):
        row = [block[4*c + r] for c in range(4)]
        row = row[-r:] + row[:-r]

        for c in range(4):
            block[4*c + r] = row[c]

def mixColumns(block: bytearray):
    for i in range(4):
        col = block[4*i:4*i + 4]
        xformed = 4 * [0]

        xformed[0] = gmul(col[0], 2) ^ gmul(col[1], 3) ^ gmul(col[2], 1) ^ gmul(col[3], 1)
        xformed[1] = gmul(col[0], 1) ^ gmul(col[1], 2) ^ gmul(col[2], 3) ^ gmul(col[3], 1)
        xformed[2] = gmul(col[0], 1) ^ gmul(col[1], 1) ^ gmul(col[2], 2) ^ gmul(col[3], 3)
        xformed[3] = gmul(col[0], 3) ^ gmul(col[1], 1) ^ gmul(col[2], 1) ^ gmul(col[3], 2)

        block[4*i:4*i + 4] = xformed

def imixColumns(block: bytearray):
    for i in range(4):
        col = block[4*i:4*i + 4]
        xformed = 4 * [0]

        xformed[0] = gmul(col[0], 14) ^ gmul(col[1], 11) ^ gmul(col[2], 13) ^ gmul(col[3],  9)
        xformed[1] = gmul(col[0],  9) ^ gmul(col[1], 14) ^ gmul(col[2], 11) ^ gmul(col[3], 13)
        xformed[2] = gmul(col[0], 13) ^ gmul(col[1],  9) ^ gmul(col[2], 14) ^ gmul(col[3], 11)
        xformed[3] = gmul(col[0], 11) ^ gmul(col[1], 13) ^ gmul(col[2],  9) ^ gmul(col[3], 14)

        block[4*i:4*i + 4] = xformed

### Core ###

def expand_key(key: bytes) -> bytes:
    num_rounds = 10
    rcons = [bytearray(4 * [0]) for _ in range(num_rounds)]

    rcons[0][0] = 1
    for i in range(1, num_rounds):
        rcons[i][0] = gmul(rcons[i-1][0], 2)
        rcons[i] = bytes(rcons[i])

    def S_word(word):
        return bytes([Sbox[b] for b in word])

    def rot(word):
        return word[1:] + word[:1]

    def xor(w0, w1):
        return bytes([x ^ y for x, y in zip(w0, w1)])

    key = [key[:4], key[4:8], key[8:12], key[12:]]
    rkeys = []
    for i in range(4 * (num_rounds + 1)):
        if i < 4:
            rkeys.append(key[i])
        elif i % 4 == 0:
            rc = rcons[i // 4 - 1]
            rkeys.append(xor(S_word(rot(rkeys[i-1])), xor(rkeys[i-4],  rc)))
        else:
            rkeys.append(xor(rkeys[i-1], rkeys[i-4]))

    return b''.join(rkeys)

def enc_block(block: bytearray, key: bytes):
    rkeys = expand_key(key)

    for i in range(16):
        block[i] ^= rkeys[i]

    for rnd in range(1, 11):
        subBytes(block)
        shiftRows(block)
        if rnd != 10: mixColumns(block)

        for i in range(16):
            block[i] ^= rkeys[16*rnd + i]

def dec_block(block: bytearray, key: bytes):
    rkeys = expand_key(key)

    for rnd in range(10, 0, -1):
        for i in range(16):
            block[i] ^= rkeys[16*rnd + i]

        if rnd != 10: imixColumns(block)
        ishiftRows(block)
        isubBytes(block)

    for i in range(16):
        block[i] ^= rkeys[i]

### Modes ###

def add_padding(b: bytes) -> bytes:
    len_padding = 16 - len(b) % 16
    return b + len_padding * len_padding.to_bytes(1, 'little')

def ecb_enc(plaintxt: bytes, key: bytes) -> bytes:
    pt = bytearray(add_padding(plaintxt))

    for i in range(len(pt) // 16):
        block = pt[16*i:16*(i+1)]
        enc_block(block, key)
        pt[16*i:16*(i+1)] = block

    return bytes(pt)

def ecb_dec(ciphertxt: bytes, key: bytes) -> bytes:
    if len(ciphertxt) % 16:
        raise ValueError('incorrect padding')

    ct = bytearray(ciphertxt)

    for i in range(len(ct) // 16):
        block = ct[16*i:16*(i+1)]
        dec_block(block, key)
        ct[16*i:16*(i+1)] = block

    return bytes(ct)
