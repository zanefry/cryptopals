#include "aes.h"
#include <time.h>

bstring key;

bstring oracle(bstring plaintext)
{
    u8 junk_size = rand() % 100;
    bstring secret = base64_to_bytes("Um9sbGluJyBpbiBteSA1LjAKV2l0aCBteSByYWctdG9wIGRvd24gc28gbXkgaGFpciBjYW4gYmxvdwpUaGUgZ2lybGllcyBvbiBzdGFuZGJ5IHdhdmluZyBqdXN0IHRvIHNheSBoaQpEaWQgeW91IHN0b3A/IE5vLCBJIGp1c3QgZHJvdmUgYnkK");

    bstring added = make_bstring(junk_size + plaintext.size + secret.size);

    for (u8 i = 0; i < junk_size; i++)
	added.buf[i] = rand() % 256;

    for (u16 i = junk_size; i < added.size; i++) {
	if (i - junk_size < plaintext.size)
	    added.buf[i] = plaintext.buf[i - junk_size];
	else
	    added.buf[i] = secret.buf[i - plaintext.size - junk_size];
    }

    u8 num_blocks = pad_to_block(added.buf, added.size);
    added.size = 16*num_blocks;

    ecb_enc(added.buf, num_blocks, key.buf);

    return added;
}

bstring find_block_image(bstring block)
{
    bstring input = make_bstring(32);
    memcpy(input.buf, block.buf, 16);
    memcpy(input.buf + 16, block.buf, 16);

    bstring b1 = { NULL, 16 };
    bstring b2 = { NULL, 16 };

    u8 found = 0;
    while (!found) {
	bstring output = oracle(input);

	for (u8 block = 0; block < (output.size / 16) - 1; block++) {
	    b1.buf = output.buf + 16*block;
	    b2.buf = output.buf + 16*(block + 1);

	    if (equal(b1, b2)) {
		found = 1;
		break;
	    }
	}
    }

    return b1;
}

void main()
{
    // seed
    srand(time(NULL));

    // make random key
    key = make_bstring(16);
    for (u8 i = 0; i < 16; i++)
	key.buf[i] = rand() % 256;

    // recover block of secret
    bstring secret_block = make_bstring(16);

    bstring input = make_bstring(32);
    input.size = 16;
    memset(input.buf, 'A', input.size);

    bstring as_image = find_block_image(input);
    printf("A block image found\n");

    // find possible block images
    bstring possible_block_images[256];

    for (u16 c = 0; c < 256; c++) {
	input.buf[15] = c;
	possible_block_images[c] = find_block_image(input);
    } printf("possible block images found\n");

    input.size = 31;
    memset(input.buf, 'A', 31);

    u8 found = 0;
    while (!found) {
	bstring output = oracle(input);

	printf("A block image:\t");
	for (u8 i = 0; i < 16; i++)
	    printf("%02x", as_image.buf[i]);
	printf("\n");

	printf("output:\n");
	for (u8 i = 0; i < output.size / 16; i++) {
	    printf("\t");
	    for (u8 j = 0; j < 16; j++)
		printf("%02x", output.buf[16*i + j]);
	    printf("\n");
	} printf("\n");

	// locate target block
	
	u8 target_block_idx = 0;
	bstring target_block = { NULL, 16 };

	for (u8 i = 0; i < output.size - 1; i++) {
	    target_block.buf = output.buf + 16*target_block_idx++;

	    if (equal(target_block, as_image)) {
		printf("target block index: %d\n", target_block_idx);
		break;
	    }
	}

	// check target for match

	target_block.buf = output.buf + 16*target_block_idx++;

	for (u16 c = 0; c < 256; c++)
	    if (equal(target_block, possible_block_images[c])) {
		printf("first char of secret: %c\n", c);
		found = 1;
	    }
    }

    // until found
    //      look for A block image in output
    //      check next block for match with possible block
    // print char of match
}
