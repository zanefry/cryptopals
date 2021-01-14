#include "hex_base64.h"

void f(char* c)
{
    c = realloc(c, 1);
}

void main()
{
    char* c = malloc(1);

    f(c);
    c[0] = 'A';

    printf("test\n");
}
