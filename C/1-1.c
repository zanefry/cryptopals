#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* hex_to_bytes(char* hex_string);
char* bytes_to_hex(char* bytes, int num_bytes);
char* bytes_to_base64(char* bytes, int num_bytes);

int main()
{
    char hex[400];
    int num_bytes;
    char* bytes;

    scanf("%s", hex);
    bytes = hex_to_bytes(hex);

    return 0;
}

////////////////////// hex //////////////////////////////////
// Takes a hex string and returns the string of bytes they encode
char* hex_to_bytes(char* hex_string)
{
    size_t hex_length = strlen(hex_string);
    size_t num_bytes;
    char* bytes;

    if(hex_length % 2 == 0)
        num_bytes = hex_length / 2;
    else
        num_bytes = (hex_length + 1) / 2;

    bytes = calloc(num_bytes + 1, sizeof(char));

    for(int i = 0; i < num_bytes; i++)
        bytes[i] = hex_to_byte(hex_string[i * 2], hex_string[i * 2 + 1]);

    return bytes;
}

// Takes two hex chars, returns byte
char hex_to_byte(char hex1, char hex2)
{
    char byte;

    if(hex1 > '9')
        byte = (hex1 - 'a' + 10) << 4;
    else
        byte = (hex1 - '0') << 4;

    if(hex2 > '9')
        byte += hex2 - 'a' + 10;
    else
        byte += hex2 - '0';

    return byte;
}

// Takes byte, returns two hex chars
char* byte_to_hex(char byte)
{
    char* hex = calloc(3, sizeof(char));

    if((byte >> 4) > 9)
        hex[0] = 'a' + (byte >> 4) - 9;
    else
        hex[0] = '0' + (byte >> 4);

    if((byte << 4) >> 4 > 9)
        hex[0] = 'a' + ((byte << 4) >> 4) - 9;
    else
        hex[0] = '0' + (byte << 4) >> 4;

    return hex;
}

// Takes byte string and length and returns hex encoded string
char* bytes_to_hex(char* bytes, int num_bytes)
{
    char* hex = calloc(2 * num_bytes + 1, sizeof(char));

    char enc;
    for(int i = 0; i < num_bytes; i++) {
        enc = byte_to_hex(bytes[i]);
        hex[i * 2] = enc[0];
        hex[i * 2 + 1] = enc[1];
    }

    return hex;
}

/////////////// base64 ///////////////////////////

char byte_to_base64(char byte)
{
    char base64;

    if(byte > 25)
        base64 = 'a' + byte - 26;
    else
        base64 = 'A' + byte;

    return base64;
}

char* block_to_base64(char byte1, char byte2, char byte3, bool final)
{
    char base64[4];

    base64[0] = byte_to_base64(byte1 >> 2);
    base64[1] = byte_to_base64(((byte1 << 6) >> 2) + (byte2 >> 4));

    if(final && byte3 == 0) {
        if(byte2 == 0)
            base64[2] = '=';
        else
            base64[2] = byte_to_base64(((byte2 << 4) >> 2) + (byte3 >> 6));
        base64[3] = '=';
    } else {
        base64[2] = byte_to_base64(((byte2 << 4) >> 2) + (byte3 >> 6));
        base64[3] = byte_to_base64((byte3 << 2) >> 2);
    }
}

char* bytes_to_base64(char* bytes, int num_bytes) {

}















