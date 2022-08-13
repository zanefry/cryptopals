#include "aes.h"
#include "time.h"

struct profile {
    char email[100];
    u8 uid;
    u8 is_admin;
};

struct profile* profile_for(char* email)
{
    static u8 num_users = 0;
    struct profile* p = malloc(sizeof(struct profile));

    strcpy(p->email, email);
    p->uid = num_users++;
    p->is_admin = 0;
}

char* encode_profile(struct profile p)
{
    char* string = malloc(200);

    sprintf(string, "email=%s&uid=%u&role=", p.email, p.uid);

    if (p.is_admin)
	strcat(string, "admin");
    else
	strcat(string, "user");

    return string;
}

void main()
{
    // seed and get key
    u8* key = malloc(16);
    srand(time(NULL));

    for (u8 i = 0; i < 16; i++)
	key[i] = rand() % 256;

    // set cut profile
    struct profile* p1 = profile_for("AAAAAAAAAAadmin\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c@mail.com");

    printf("CUT PROFILE:\n");
    printf("\t{\n\t\temail = \"%s\",\n\t\tuid = %d,\n\t\trole = ", p1->email, p1->uid);
    if (p1->is_admin)
	printf("admin\n");
    else
	printf("user\n");
    printf("\t}\n\n");

    // encode and print cut profile
    u8* data = encode_profile(*p1);
    u8 num_blocks = pad_to_block(data, strlen(data));

    printf("ENCODED CUT PROFILE:\n\t");
    for (u8 block = 0; block < num_blocks; block++) {
	printf("%d : ", block);
	for (u8 i = 0; i < 16; i++)
	    printf("%c", data[16*block + i]);
	printf("\n\t");
    } printf("\n");

    // encrypt and print cut profile
    ecb_enc(data, num_blocks, key);

    printf("ENCRYPTED:\n\t");

    u8* cut = malloc(16);
    for (u8 block = 0; block < num_blocks; block++) {
	printf("%d : ", block);
	for (u8 i = 0; i < 16; i++) {
	    printf("%02x", data[16*block + i]);
	    if (block == 1)
		cut[i] = data[16*block + i];
	} printf("\n\t");
    } printf("\n");

    // cut desired block
    printf("CUT enc(\"admin\\x0c\\x0c\\x0c\\x0c\\x0c\\x0c\\x0c\\x0c\\x0c\\x0c\\x0c\\x0c\"):\n\t");
    for (u8 i = 0; i < 16; i++)
	printf("%02x", cut[i]);
    printf("\n\n");

    struct profile* p2 = profile_for("hack@gmail.com");

    printf("PASTE PROFILE:\n");
    printf("\t{\n\t\temail = \"%s\",\n\t\tuid = %d,\n\t\trole = ", p2->email, p2->uid);
    if (p2->is_admin)
	printf("admin\n");
    else
	printf("user\n");
    printf("\t}\n\n");

    data = encode_profile(*p2);
    num_blocks = pad_to_block(data, strlen(data));

    printf("ENCODED PASTE PROFILE:\n\t");
    for (u8 block = 0; block < num_blocks; block++) {
	printf("%d : ", block);
	for (u8 i = 0; i < 16; i++)
	    printf("%c", data[16*block + i]);
	printf("\n\t");
    } printf("\n");

    // encrypt and print paste profile
    ecb_enc(data, num_blocks, key);

    printf("ENCRYPTED:\n\t");
    for (u8 block = 0; block < num_blocks; block++) {
	printf("%d : ", block);
	for (u8 i = 0; i < 16; i++)
	    printf("%02x", data[16*block + i]);
	printf("\n\t");
    } printf("\n");

    // paste
    for (u8 i = 0; i < 16; i++)
	data[32 + i] = cut[i];

    printf("PASTED:\n\t");
    for (u8 block = 0; block < num_blocks; block++) {
	printf("%d : ", block);
	for (u8 i = 0; i < 16; i++)
	    printf("%02x", data[16*block + i]);
	printf("\n\t");
    } printf("\n");

    ecb_dec(data, num_blocks, key);

    printf("DECRYPTED:\n\t");
    for (u8 block = 0; block < num_blocks; block++) {
	printf("%d : ", block);
	for (u8 i = 0; i < 16; i++)
	    printf("%c", data[16*block + i]);
	printf("\n\t");
    } printf("\n");
}
