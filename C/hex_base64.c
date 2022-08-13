#include "hex_base64.h"

//// Format checks //// 

bstring make_bstring(size_t size)
{
    bstring b = { .buf = malloc(size), .size = size };
    return b;
}

u8 is_hex(char* string)
{
    int i = 0;
    while (string[i] != '\0' && string[i] != '\n') {
	char c = string[i];
    
	if ('0' <= c && c <= '9')
	    i++;
	else if ('a' <= c && c <= 'f')
	    i++;
	else if ('A' <= c && c <= 'F') {
	    string[i] = 'a' + (c - 'A');
	    i++;
	} else
	    return 0;
    }

    return 1;
}

u8 is_base64(char* string)
{
    int i = 0;
    while (string[i] != '\0' && string[i] != '\n') {
	char c = string[i];

	if ('A' <= c && c <= 'Z')
	    i++;
	else if ('a' <= c && c <= 'z')
	    i++;
	else if ('0' <= c && c <= '9')
	    i++;
	else if (c == '+' || c == '/' || c == '=')
	    i++;
	else
	    return 0;
    }

    return 1;
}

u8 is_binary(char* string)
{
  int i = 0;
  while (string[i] != '\0') {
      if (string[i] == '0' || string[i] == '1')
	  i++;
      else
	  return 0;
  }

  return 1;
}

//// Decode //// 

bstring hex_to_bytes(char* hex)
{
    if (is_hex (hex) != 1) {
	perror ("hex_to_bytes called on a non-hex string\n");
	exit (-1);
    }

    size_t num_hex = strlen (hex);
    size_t num_bytes = (num_hex + 1) / 2;

    char* buf = calloc(num_bytes, sizeof (char));
    bstring bytes = { .buf = buf, .size = num_bytes };

    for (int i = 0; i < num_hex; i++) {
	char value;

	if (hex[i] <= '9')
	    value = hex[i] - '0';
	else
	    value = hex[i] - 'a' + 10;

	if (i % 2 == 0)
	    bytes.buf[i/2] += value << 4;
	else
	    bytes.buf[i/2] += value;
    }

    return bytes;
}

bstring base64_to_bytes(char* base64)
{
    if (is_base64 (base64) != 1) {
	perror ("base64_to_bytes called on a non-base64 string\n");
	exit (-1);
    }

    size_t num_base64 = strlen (base64);
    size_t num_bytes = (num_base64 % 4 == 0) ? 3*num_base64 / 4 : 3*(num_base64/4 + 1);
    size_t num_blocks = num_bytes / 3;

    char* buf = calloc (num_bytes, sizeof (char));
    bstring bytes = { .buf = buf, .size = num_bytes };

    for (int i = 0; base64[i] != '=' && base64[i] != '\n' && i < num_base64; i++) {
	char c = base64[i];
	char value;

	if ('A' <= c && c <= 'Z')
	    value = c - 'A';
	if ('a' <= c && c <= 'z')
	    value = c - 'a' + 26;
	if ('0' <= c && c <= '9')
	    value = c - '0' + 52;
	if (c == '+')
	    value = 62;
	if (c == '/')
	    value = 63;

	unsigned int block = i / 4;

	switch (i % 4) {
	case 0:
	    bytes.buf[block*3] = value << 2;
	    break;
	case 1:
	    bytes.buf[block*3]    += (value & 0x30) >> 4;
	    bytes.buf[block*3 + 1] = (value & 0x0f) << 4;
	    break;
	case 2:
	    bytes.buf[block*3 + 1] += (value & 0x3c) >> 2;
	    bytes.buf[block*3 + 2]  = (value & 0x03) << 6;
	    break;
	case 3:
	    bytes.buf[block*3 + 2] += value;
	}
    }

    return bytes;
}

bstring binary_to_bytes(char* binary)
{
    if (is_binary (binary) != 1) {
	perror ("binary_to_bytes called on a non-binary string");
	exit (-1);
    }

    size_t num_bits = strlen (binary);
    size_t num_bytes = (num_bits % 8 == 0) ? num_bits / 8 : num_bits / 8 + 1;

    char* buf = calloc (num_bytes, sizeof (char));
    bstring bytes = { .buf = buf, .size = num_bytes };

    for (int i = 0; i < num_bits; i++) {
	char bit = binary[i];
	char value = (bit == '1') ? 1 << (7 - (i % 8)) : 0;

	bytes.buf[i / 8] += value;
    }

    return bytes;
}

//// Encode ////

char nibble_to_hex(char nibble)
{
    nibble &= 0b00001111;

    if (nibble < 10)
	return '0' + nibble;
    else
	return 'a' + nibble - 10;
}

char* bytes_to_hex(bstring bytes)
{
    size_t num_hex = bytes.size * 2;
    char* hex = calloc (num_hex + 1, sizeof(char));

    for (int i = 0; i < num_hex; i++) {
	if (i % 2 == 0)
	    hex[i] = nibble_to_hex (bytes.buf[i/2] >> 4);
	else
	    hex[i] = nibble_to_hex (bytes.buf[i/2]);
    }

    return hex;
}

char sextet_to_base64(char sextet)
{
    sextet &= 0b00111111;

    if (sextet < 26)
	return 'A' + sextet;
    if (26 <= sextet && sextet < 52)
	return 'a' + sextet - 26;
    if (52 <= sextet && sextet < 62)
	return '0' + sextet - 52;
    if (sextet == 62)
	return '+';
    else
	return '/';
}

char* bytes_to_base64(bstring bytes)
{
    size_t num_blocks = (bytes.size % 3 == 0) ? bytes.size / 3 : bytes.size / 3 + 1;
    size_t num_base64 = num_blocks * 4;
    size_t num_nonzero_base64 = num_base64;

    char* base64 = calloc(num_base64 + 1, sizeof(char));
  
    switch (bytes.size % 3) {
    case 0:
	break;
    case 1:
	base64[--num_nonzero_base64] = '=';
	base64[--num_nonzero_base64] = '=';
	break;
    case 2:
	base64[--num_nonzero_base64] = '=';
    }

    for (int i = 0; i < num_nonzero_base64; i++) {
	unsigned int block = i / 4;

	switch (i % 4) {
	case 0:
	    base64[i] = sextet_to_base64 (bytes.buf[3*block] >> 2);
	    break;
	case 1:
	    base64[i] = sextet_to_base64 (((bytes.buf[3*block] & 0x03) << 4) + \
					  ((bytes.buf[3*block + 1] & 0xf0) >> 4));
	    break;
	case 2:
	    base64[i] = sextet_to_base64 (((bytes.buf[3*block + 1] & 0x0f) << 2) + \
					  ((bytes.buf[3*block + 2] & 0xc0) >> 6));
	    break;
	case 3:
	    base64[i] = sextet_to_base64 (bytes.buf[3*block + 2]);
	    break;
	}
    }

    return base64;
}

char* bytes_to_binary (bstring bytes)
{
    size_t num_bits = bytes.size * 8;
    char* binary = calloc (num_bits + 1, sizeof (char));

    for (int i = 0; i < num_bits; i++) {
	int block = i / 8;
	int bit = 7 - i % 8;

	if ((bytes.buf[block] & (1 << bit)) != 0)
	    binary[i] = '1';
	else
	    binary[i] = '0';
    }

    return binary;
}

//// Translate ////

char* hex_to_base64 (char* hex)
{
    bstring bytes = hex_to_bytes (hex);
    return bytes_to_base64 (bytes);
}

char* base64_to_hex (char* base64)
{
    bstring bytes = base64_to_bytes (base64);
    return bytes_to_hex (bytes);
}

char* binary_to_hex (char* binary)
{
    bstring bytes = binary_to_bytes (binary);
    return bytes_to_hex (bytes);
}

char* hex_to_binary (char* hex)
{
    bstring bytes = hex_to_bytes (hex);
    return bytes_to_binary (bytes);
}

char* base64_to_binary (char* base64)
{
    bstring bytes = base64_to_bytes (base64);
    return bytes_to_binary (bytes);
}

char* binary_to_base64 (char* binary)
{
    bstring bytes = binary_to_bytes (binary);
    return bytes_to_base64 (bytes);
}

// doesn't work for single char strings
bstring string_to_bytes (char* string)
{
    size_t length = strlen (string);
    char* buf = calloc (length + 1, sizeof (char));

    strcpy (buf, string);
    return (bstring) { .buf = buf, .size = length };
}

char* bytes_to_string (bstring bytes)
{
    char* string = realloc (bytes.buf, bytes.size + 1);
    string[bytes.size] = '\0';

    return string;
}

char equal (bstring a, bstring b)
{
    if (a.size != b.size)
	return 0;
    else
	for (int i = 0; i < a.size; i++)
	    if (a.buf[i] != b.buf[i])
		return 0;

    return 1;
}
