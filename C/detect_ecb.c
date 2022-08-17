#include "hex_base64.h"

void main ()
{
    char** lines = calloc(500, sizeof (char*));
    size_t linecount = 0;
    size_t size;

    while (1)
    {
        if (getline(&lines[linecount], &size, stdin) == -1)
            break;
        else
        {
            lines[linecount][320] = '\0';
            linecount++;
        }
    }

    for (int line = 0; line < linecount; line++)
    {
        bytestring line_bytes = hex_to_bytes(lines[line]);
        char match = 0;

        for (int block1 = 0; block1 < 9; block1++)
            for (int block2 = block1 + 1; block2 < 10; block2++)
            {
                bytestring a = { .buf = line_bytes.buf + block1*16, .length = 16 };
                bytestring b = { .buf = line_bytes.buf + block2*16, .length = 16 };

                if (equal(a, b))
                    match++;
            }

        if (match)
            printf("%d : %d\n", line, match);
    }

    return;
}
