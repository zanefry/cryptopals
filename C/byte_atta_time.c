#include "aes.h"
#include <time.h>

bstring key;
bstring secret;

void oracle(bstring input, bstring* result)
{
	if (result->size != input.size + secret.size) {
		result->size = input.size + secret.size;
		result->buf = realloc(result->buf, result->size);
	}

	memcpy(result->buf, input.buf, input.size);
	memcpy(result->buf + input.size, secret.buf, secret.size);
    
	u8 num_blocks = pad_to_block(result->buf, result->size);
	result->size = 16*num_blocks;

	//ecb_enc(result->buf, num_blocks, key.buf);
}

void main()
{
	srand(time(NULL));

	////////////////////////////
	//// attacker does not have access to this stuff
	secret = base64_to_bytes("Um9sbGluJyBpbiBteSA1LjAKV2l0aCBteSByYWctdG9wIGRvd24gc28gbXkgaGFpciBjYW4gYmxvdwpUaGUgZ2lybGllcyBvbiBzdGFuZGJ5IHdhdmluZyBqdXN0IHRvIHNheSBoaQpEaWQgeW91IHN0b3A/IE5vLCBJIGp1c3QgZHJvdmUgYnkK");

	key = make_bstring(16);
	for (u8 i = 0; i < 16; i++)
		key.buf[i] = rand() % 256;
	////////////////////////////

	bstring found;
	bstring input = make_bstring(16);
	bstring result = make_bstring(1); // for realloc by oracle

	input.size = 0; // to find size of secret alone
	oracle(input, &result);

	found = make_bstring(result.size);

	u8 candidate_images[256 * 16]; // one block for each candidate

	memset(input.buf, 'A', 15);

	for (u8 block = 0; block < 9; block++) {
		for (u8 i = 0; i < 16; i++) {
			input.size = 16;

			// prepare input
			if (block == 0)
				memcpy(input.buf + 15 - i, found.buf, i);
			else
				memcpy(input.buf, found.buf + 16*(block-1) + i + 1, 15);

			// store candidate images
			for (u16 c = 0; c < 256; c++) {
				input.buf[input.size - 1] = c;

				oracle(input, &result);
				memcpy(candidate_images + 16*c, result.buf, 16);
			}

			// get actual image
			input.size = 15 - i;
			oracle(input, &result);

			for (u8 i = 0; i < 16; i++)
				result.buf[16*8 + i];

			// find match
			u16 match;
			for (match = 0; match < 256 && memcmp(result.buf + 16*block, candidate_images + match*16, 16); match++);
			      
			found.buf[16*block + i] = match;
		}
	}

	for (u8 i = 0; i < found.size; i++)
		printf("%c", found.buf[i]);
	printf("\n");
}
