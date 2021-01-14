#include "hex_base64.h"
#include "freq_alt.h"

bytestring xor (bytestring plain, bytestring key)
{
  bytestring cipher = { .buf = malloc (plain.length), .length = plain.length };

  for (int i = 0; i < plain.length; i++)
    cipher.buf[i] = plain.buf[i] ^ key.buf[i % key.length];

  return cipher;
}

size_t distance (bytestring A, bytestring B)
{
  bytestring diff = xor (A, B);
  size_t distance = 0;

  for (int i = 0; i < diff.length; i++)
    for (char b = 1; b != 0; b <<= 1)
      if (diff.buf[i] & b)
	distance++;

  return distance;
}

void main ()
{
  char* input = malloc (1000);
  size_t input_length;
  size_t keysize = 29;

  getline (&input, &input_length, stdin);
  bytestring input_bytes = base64_to_bytes (input);

  bytestring* blocks = malloc (keysize * sizeof (bytestring));

  // allocate blocks
  for (int i = 0; i < keysize; i++)
    {
      // add necessary last bytes if input_bytes.length not a multiple of keysize
      size_t size = input_bytes.length / keysize;
      if (i > 0 && input_bytes.length % keysize >= i)
	size++;

      blocks[i] = (bytestring) { .buf = malloc (size), .length = size };
    }

  // populate blocks
  for (int i = 0; i < input_bytes.length; i++)
    blocks[i % keysize].buf[i / keysize] = input_bytes.buf[i];

  // find candidates

  bytestring key = { .buf = malloc (1), .length = 1 };
  bytestring result;
  float r;
  
  char candidates[29][200];
  size_t counts[29] = {0};

  for (int i = 0; i < keysize; i++)
    do {
      result = xor (blocks[i], key);
      r = rank (result);

      if (r > 5)
	{
	  candidates[i][counts[i]] = key.buf[0];
	  counts[i]++;
	}

      key.buf[0]++;
    } while (key.buf[0] != 0);

  for (int i = 0; i < keysize; i++)
      for (int j = 0; j < counts[i]; j++)
	{
	  key.buf[0] = candidates[i][j];
	  r = rank (xor (blocks[i], key));

	  printf ("%2.3f : %c\n", r, key.buf[0]);
	}
    
  return;
}
