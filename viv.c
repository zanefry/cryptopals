#include "aes.h"

void main()
{
    char* base64 = malloc(300);
    scanf("%s", base64);

    bstring raw = base64_to_bytes(base64);

    ecb_dec(raw.buf, raw.size / 16, "YELLOW SUBMARINE");

    printf("%s\n", bytes_to_string(raw));
}
