#ifndef AES
#define AES

#include "hex_base64.h"

u8 gmul(u8 a, u8 b);
u8 ginv(u8 a);
u8 gdiv(u8 a, u8 b);

u8 pad_to_block(u8* plain, u16 num_bytes);

void ecb_enc(u8* plaintext, u8 num_blocks, u8* key);
void ecb_dec(u8* ciphertext, u8 num_blocks, u8* key);
void cbc_enc(u8* plaintext, u8 num_blocks, u8* key, u8* iv);
void cbc_dec(u8* plaintext, u8 num_blocks, u8* key, u8* iv);

#endif
