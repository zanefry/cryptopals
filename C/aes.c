// * ECB of a 128-bit block
// ** Prior to first round
// *** generate round keys with key schedule
// *** add zeroth round key to block
// ** Main rounds
// *** SubBytes
// *** ShiftRows
// *** MixColumns
// *** AddRoundKey
// ** Final round
// *** SubBytes
// *** ShiftRows
// *** AddRoundKey

#include "aes.h"

u8 ltable[256], i_ltable[256], Sbox[256], i_Sbox[256];

void init_tables();
void init_Sbox();

/* --- Galois -- */

u8 gmul(u8 a, u8 b)
{
    if (!(a && b)) return 0;

    init_tables();
    return i_ltable[(ltable[a] + ltable[b]) % 0xff];
}

u8 ginv(u8 a)
{
    if (!a) printf("called ginv(0)\n"), exit(-1);

    init_tables();
    return i_ltable[0xff - ltable[a]];
}

u8 gdiv(u8 a, u8 b)
{
    init_tables();
    return gmul(a, ginv(b));
}

void init_tables()
{
    static u8 initialized;
    if (initialized) return;
    else initialized = 1;

    u8 a = 1, p = 0;

    // 3 is used as the generator
    for (u16 i = 0; i <= 0xff; i++) {
        i_ltable[i] = a;
        ltable[a] = i;

        a ^= (a << 1) ^ ((a & 0x80) ? 0x1b : 0);
    }

    i_ltable[0xff] = i_ltable[0];
    ltable[0] = 0;

    init_Sbox();

}

void init_Sbox()
{
    for (u16 i = 1; i <= 0xff; i++) {}
}


u32 rot_word(u32 word)
{
    return (word << 24) | (word >> 8);
}

u32 sub_word(u32 word)
{
    u8 b1 = (0xff000000 & word) >> 24;
    u8 b2 = (0x00ff0000 & word) >> 16;
    u8 b3 = (0x0000ff00 & word) >> 8;
    u8 b4 = 0x000000ff & word;

    return (Sbox[b1] << 24) | (Sbox[b2] << 16) | (Sbox[b3] << 8) | Sbox[b4];
}

u32* expand_key(u32* key)
{
    size_t num_rounds = 11;
    u32* rkeys = malloc(num_rounds * 16);

    u32 rcons[10];
    rcons[0] = 1;

    for (int i = 1; i < 10; i++)
        rcons[i] = gmul(rcons[i - 1], 2);

    for (int i = 0; i < 4 * num_rounds; i++) {
        if (i < 4)
            rkeys[i] = key[i];
        else if (i % 4 == 0)
            rkeys[i] = rkeys[i - 4] ^ sub_word(rot_word(rkeys[i - 1])) ^ rcons[i/4 - 1];
        else
            rkeys[i] = rkeys[i - 4] ^ rkeys[i - 1];
    }

    return rkeys;
}

/* --- Core --- */

void shift_rows(u8* block)
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < i; j++) {
            u8 temp = block[i];
            block[i] = block[4 + i];
            block[4 + i] = block[8 + i];
            block[8 + i] = block[12 + i];
            block[12 + i] = temp;
        }
}

void i_shift_rows(u8* block)
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < i; j++) {
            u8 temp = block[12 + i];
            block[12 + i] = block[8 + i];
            block[8 + i] = block[4 + i];
            block[4 + i] = block[i];
            block[i] = temp;
        }
}

void mix_columns(u8* block)
{
    for (u8 i = 0; i < 4; i++) {
        u8* col = block + 4*i;
        u8 a[4], b[4];

        for (u8 j = 0; j < 4; j++) {
            a[j] = col[j];
            b[j] = gmul(col[j], 2);
        }

        col[0] = b[0] ^ a[3] ^ a[2] ^ b[1] ^ a[1];
        col[1] = b[1] ^ a[0] ^ a[3] ^ b[2] ^ a[2];
        col[2] = b[2] ^ a[1] ^ a[0] ^ b[3] ^ a[3];
        col[3] = b[3] ^ a[2] ^ a[1] ^ b[0] ^ a[0];
    }
}

void i_mix_columns(u8* block)
{
    for (u8 i = 0; i < 4; i++) {
        u8* col = block + 4*i;
        u8 a[4];

        for (u8 i = 0; i < 4; i++)
            a[i] = col[i];

        col[0] = gmul(a[0],14) ^ gmul(a[2],13) ^ gmul(a[1],11) ^ gmul(a[3],9);
        col[1] = gmul(a[1],14) ^ gmul(a[3],13) ^ gmul(a[2],11) ^ gmul(a[0],9);
        col[2] = gmul(a[2],14) ^ gmul(a[0],13) ^ gmul(a[3],11) ^ gmul(a[1],9);
        col[3] = gmul(a[3],14) ^ gmul(a[0],11) ^ gmul(a[1],13) ^ gmul(a[2],9);
    }
}

void enc_block(u8* block, u8* key)
{
    u8* rkeys = (u8*)expand_key((u32*)key);

    // initial AddRoundKey
    for (u8 i = 0; i < 16; i++)
        block[i] ^= rkeys[i];

    // 10 core rounds (final round without MixColumns)
    for (u8 round = 1; round < 11; round++) {
        // SubBytes
        for (u8 i = 0; i < 16; i++)
            block[i] = Sbox[block[i]];

        // ShiftRows
        shift_rows(block);

        // MixColumns
        if (round != 10) mix_columns(block);

        // AddRoundKey
        for (u8 i = 0; i < 16; i++)
            block[i] ^= rkeys[16*round + i];
    }
}

void dec_block(u8* block, u8* key)
{
    u8* rkeys = (u8*)expand_key((u32*)key);

    // 10 core rounds (final round without MixColumns)
    for (u8 round = 10; round > 0; round--) {
        // AddRoundKey
        for (u8 i = 0; i < 16; i++)
            block[i] ^= rkeys[16*round + i];

        // MixColumns
        if (round != 10) i_mix_columns(block);

        // ShiftRows
        i_shift_rows(block);

        // SubBytes
        for (u8 i = 0; i < 16; i++)
            block[i] = i_Sbox[block[i]];
    }

    // initial AddRoundKey
    for (u8 i = 0; i < 16; i++)
        block[i] ^= rkeys[i];
}

u8 pad_to_block(u8* plain, u16 num_bytes)
{
    u8 num_blocks = num_bytes / 16 + ((num_bytes % 16) ? 1 : 0);
    u8 num_padding = 16*num_blocks - num_bytes;

    plain = realloc(plain, 16*num_blocks);

    for (u16 i = num_bytes; i < 16*num_blocks; i++)
        plain[i] = num_padding;

    return num_blocks;
}

/* --- Cipher Modes --- */

void ecb_enc(u8* plaintext, u8 num_blocks, u8* key)
{
    for (u8 i = 0; i < num_blocks; i++)
        enc_block(plaintext + 16*i, key);
}

void ecb_dec(u8* ciphertext, u8 num_blocks, u8* key)
{
    for (u8 i = 0; i < num_blocks; i++)
        dec_block(ciphertext + 16*i, key);
}

void cbc_enc(u8* plaintext, u8 num_blocks, u8* key, u8* iv)
{
    for (u8 block = 0; block < num_blocks; block++) {
        if (!block)
            for (u8 i = 0; i < 16; i++)
                plaintext[i] ^= iv[i];
        else
            for (u8 i = 0; i < 16; i++)
                plaintext[16*block + i] ^= plaintext[16*(block - 1) + i];

        enc_block(plaintext + 16*block, key);
    }
}

void cbc_dec(u8* ciphertext, u8 num_blocks, u8* key, u8* iv)
{
    for (u8 block = num_blocks - 1; block > 0; block--) {
        dec_block(ciphertext + 16*block, key);

        for (u8 i = 0; i < 16; i++)
            ciphertext[16*block + i] ^= ciphertext[16*(block - 1) + i];
    }

    dec_block(ciphertext, key);

    for (u8 i = 0; i < 16; i++)
        ciphertext[i] ^= iv[i];
}
