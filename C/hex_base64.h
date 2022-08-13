// For the Matasano crypto challenges. Used for reading and printing bytes.

#ifndef HEX_BASE64
#define HEX_BASE64

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint8_t   u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef struct bstring {
  u8* buf;
  size_t size;
} bstring;

// Constructor
bstring make_bstring(size_t size);

// Encoding and Decoding
bstring     hex_to_bytes(u8* hex);
u8*         bytes_to_hex(bstring bytes);
bstring     base64_to_bytes(u8* base64);
u8*         bytes_to_base64(bstring bytes);
bstring     binary_to_bytes(u8* binary);

// Translating
u8* bytes_to_binary(bstring bytes);
u8* hex_to_base64(u8* hex);
u8* base64_to_hex(u8* base64);
u8* binary_to_hex(u8* binary);
u8* hex_to_binary(u8* hex);
u8* base64_to_binary(u8* base64);
u8* binary_to_base64(u8* binary);

bstring string_to_bytes(u8* string);
u8*     bytes_to_string(bstring bytes);
u8      equal(bstring a, bstring b);

#endif
