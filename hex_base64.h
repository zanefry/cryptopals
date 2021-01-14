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
bstring hex_to_bytes(char* hex);
char*      bytes_to_hex(bstring bytes);
bstring base64_to_bytes(char* base64);
char*      bytes_to_base64(bstring bytes);
bstring binary_to_bytes(char* binary);

// Translating
char* bytes_to_binary(bstring bytes);
char* hex_to_base64(char* hex);
char* base64_to_hex(char* base64);
char* binary_to_hex(char* binary);
char* hex_to_binary(char* hex);
char* base64_to_binary(char* base64);
char* binary_to_base64(char* binary);

bstring string_to_bytes(char* string);
char*      bytes_to_string(bstring bytes);
char equal (bstring a, bstring b);

#endif
