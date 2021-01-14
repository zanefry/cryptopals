#include "hex_base64.h"
#include "freq_alt.h"

bytestring xor (bytestring bytes, char c)
{
  bytestring result = { .buf = calloc (bytes.length + 1, sizeof (char)), .length = bytes.length };

  for (int i = 0; i < bytes.length; i++)
    result.buf[i] = bytes.buf[i] ^ c;

  return result;
}

void main ()
{
  char* line = NULL;
  size_t size;
  int lc = 0;
  bytestring result;
  float r;
  
  while (1)
    {
      if (getline (&line, &size, stdin) == -1)
	break;
      else {
	lc++;

	char c = 0;
	do {
	  result = xor (hex_to_bytes (line), c);
	  r = rank (result);

	  if (r > 1)
	    printf ("%2.2f : %s\n", r, result.buf);

	  c++;
	} while (c != 0x00);
      }
    }

  return;
}
