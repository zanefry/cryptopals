#include "hex_base64.h"
#include "freq.h"

bstring xor(bstring bytes, char c)
{
    bstring result = { .buf = calloc(bytes.size + 1, sizeof(char)), .size = bytes.size };

    for (int i = 0; i < bytes.size; i++)
        result.buf[i] = bytes.buf[i] ^ c;

    return result;
}

void main()
{
    char* line = NULL;
    size_t size;

    while (1)
    {
        if (getline(&line, &size, stdin) == -1)
            break;
        else {
            char c = 0;
            do {
                bstring result = xor(hex_to_bytes (line), c);
                float r = rank(result);

                if (r > 1)
                    printf ("%2.2f : %s\n", r, result.buf);

                c++;

            } while (c != 0x00);
        }
    }

    return;
}
