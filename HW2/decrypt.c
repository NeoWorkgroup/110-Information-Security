#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <endian.h>
#include <getopt.h>
#include <string.h>

typedef struct
{
   uint32_t ks[32];
} DesContext;

//Rotate left operation
#define ROL28(a, n) ((((a) << (n)) | ((a) >> (28 - (n)))) & 0x0FFFFFFF)
#define ROR32(a, n) (((a) >> (n)) | ((a) << (32 - (n))))
#define ROL32(a, n) (((a) << (n)) | ((a) >> (32 - (n))))
 
//Initial permutation
#define DES_IP(left, right) \
{ \
   temp = ((left >> 4) ^ right) & 0x0F0F0F0F; \
   right ^= temp; \
   left ^= temp << 4; \
   temp = ((left >> 16) ^ right) & 0x0000FFFF; \
   right ^= temp; \
   left ^= temp << 16; \
   temp = ((right >> 2) ^ left) & 0x33333333; \
   left ^= temp; \
   right ^= temp << 2; \
   temp = ((right >> 8) ^ left) & 0x00FF00FF; \
   left ^= temp; \
   right ^= temp << 8; \
   temp = ((left >> 1) ^ right) & 0x55555555; \
   right ^= temp; \
   left ^= temp << 1; \
   left = ROL32(left, 1); \
   right = ROL32(right, 1); \
}
 
//Final permutation
#define DES_FP(left, right) \
{ \
   left = ROR32(left, 1); \
   right = ROR32(right, 1); \
   temp = ((left >> 1) ^ right) & 0x55555555; \
   right ^= temp; \
   left ^= temp << 1; \
   temp = ((right >> 8) ^ left) & 0x00FF00FF; \
   left ^= temp; \
   right ^= temp << 8; \
   temp = ((right >> 2) ^ left) & 0x33333333; \
   left ^= temp; \
   right ^= temp << 2; \
   temp = ((left >> 16) ^ right) & 0x0000FFFF; \
   right ^= temp; \
   left ^= temp << 16; \
   temp = ((left >> 4) ^ right) & 0x0F0F0F0F; \
   right ^= temp; \
   left ^= temp << 4; \
}
 
//DES round
#define DES_ROUND(left, right, ks) \
{ \
   temp = right ^ *(ks); \
   left ^= sp2[(temp >> 24) & 0x3F]; \
   left ^= sp4[(temp >> 16) & 0x3F]; \
   left ^= sp6[(temp >> 8) & 0x3F]; \
   left ^= sp8[temp & 0x3F]; \
   temp = ROR32(right, 4) ^ *(ks + 1); \
   left ^= sp1[(temp >> 24) & 0x3F]; \
   left ^= sp3[(temp >> 16) & 0x3F]; \
   left ^= sp5[(temp >> 8) & 0x3F]; \
   left ^= sp7[temp & 0x3F]; \
   temp = right; \
   right = left; \
   left = temp; \
}
 
//Permuted choice 1
#define DES_PC1(left, right) \
{ \
   uint32_t temp; \
   temp = ((left >> 4) ^ right) & 0x0F0F0F0F; \
   right ^= temp; \
   left ^= (temp << 4); \
   temp = ((right >> 16) ^ left) & 0x0000FFFF; \
   left ^= temp; \
   right ^= (temp << 16); \
   temp = ((left >> 2) ^ right) & 0x33333333; \
   right ^= temp; \
   left ^= (temp << 2); \
   temp = ((right >> 16) ^ left) & 0x0000FFFF; \
   left ^= temp; \
   right ^= (temp << 16); \
   temp = ((left >> 1) ^ right) & 0x55555555; \
   right ^= temp; \
   left ^= (temp << 1); \
   temp = ((right >> 8) ^ left) & 0x00FF00FF; \
   left ^= temp; \
   right ^= (temp << 8); \
   temp = ((left >> 1) ^ right) & 0x55555555; \
   right ^= temp; \
   left ^= (temp << 1); \
   temp = (left << 8) | ((right >> 20) & 0x000000F0); \
   left = ((right << 20) & 0x0FF00000); \
   left |= ((right << 4) & 0x000FF000); \
   left |= ((right >> 12) & 0x00000FF0); \
   left |= ((right >> 28) & 0x0000000F); \
   right = temp >> 4; \
}
 
//Selection function 1
static const uint32_t sp1[64] =
{
   0x01010400, 0x00000000, 0x00010000, 0x01010404, 0x01010004, 0x00010404, 0x00000004, 0x00010000,
   0x00000400, 0x01010400, 0x01010404, 0x00000400, 0x01000404, 0x01010004, 0x01000000, 0x00000004,
   0x00000404, 0x01000400, 0x01000400, 0x00010400, 0x00010400, 0x01010000, 0x01010000, 0x01000404,
   0x00010004, 0x01000004, 0x01000004, 0x00010004, 0x00000000, 0x00000404, 0x00010404, 0x01000000,
   0x00010000, 0x01010404, 0x00000004, 0x01010000, 0x01010400, 0x01000000, 0x01000000, 0x00000400,
   0x01010004, 0x00010000, 0x00010400, 0x01000004, 0x00000400, 0x00000004, 0x01000404, 0x00010404,
   0x01010404, 0x00010004, 0x01010000, 0x01000404, 0x01000004, 0x00000404, 0x00010404, 0x01010400,
   0x00000404, 0x01000400, 0x01000400, 0x00000000, 0x00010004, 0x00010400, 0x00000000, 0x01010004
};
 
//Selection function 2
static const uint32_t sp2[64] =
{
   0x80108020, 0x80008000, 0x00008000, 0x00108020, 0x00100000, 0x00000020, 0x80100020, 0x80008020,
   0x80000020, 0x80108020, 0x80108000, 0x80000000, 0x80008000, 0x00100000, 0x00000020, 0x80100020,
   0x00108000, 0x00100020, 0x80008020, 0x00000000, 0x80000000, 0x00008000, 0x00108020, 0x80100000,
   0x00100020, 0x80000020, 0x00000000, 0x00108000, 0x00008020, 0x80108000, 0x80100000, 0x00008020,
   0x00000000, 0x00108020, 0x80100020, 0x00100000, 0x80008020, 0x80100000, 0x80108000, 0x00008000,
   0x80100000, 0x80008000, 0x00000020, 0x80108020, 0x00108020, 0x00000020, 0x00008000, 0x80000000,
   0x00008020, 0x80108000, 0x00100000, 0x80000020, 0x00100020, 0x80008020, 0x80000020, 0x00100020,
   0x00108000, 0x00000000, 0x80008000, 0x00008020, 0x80000000, 0x80100020, 0x80108020, 0x00108000
};
 
//Selection function 3
static const uint32_t sp3[64] =
{
   0x00000208, 0x08020200, 0x00000000, 0x08020008, 0x08000200, 0x00000000, 0x00020208, 0x08000200,
   0x00020008, 0x08000008, 0x08000008, 0x00020000, 0x08020208, 0x00020008, 0x08020000, 0x00000208,
   0x08000000, 0x00000008, 0x08020200, 0x00000200, 0x00020200, 0x08020000, 0x08020008, 0x00020208,
   0x08000208, 0x00020200, 0x00020000, 0x08000208, 0x00000008, 0x08020208, 0x00000200, 0x08000000,
   0x08020200, 0x08000000, 0x00020008, 0x00000208, 0x00020000, 0x08020200, 0x08000200, 0x00000000,
   0x00000200, 0x00020008, 0x08020208, 0x08000200, 0x08000008, 0x00000200, 0x00000000, 0x08020008,
   0x08000208, 0x00020000, 0x08000000, 0x08020208, 0x00000008, 0x00020208, 0x00020200, 0x08000008,
   0x08020000, 0x08000208, 0x00000208, 0x08020000, 0x00020208, 0x00000008, 0x08020008, 0x00020200
};
 
//Selection function 4
static const uint32_t sp4[64] =
{
   0x00802001, 0x00002081, 0x00002081, 0x00000080, 0x00802080, 0x00800081, 0x00800001, 0x00002001,
   0x00000000, 0x00802000, 0x00802000, 0x00802081, 0x00000081, 0x00000000, 0x00800080, 0x00800001,
   0x00000001, 0x00002000, 0x00800000, 0x00802001, 0x00000080, 0x00800000, 0x00002001, 0x00002080,
   0x00800081, 0x00000001, 0x00002080, 0x00800080, 0x00002000, 0x00802080, 0x00802081, 0x00000081,
   0x00800080, 0x00800001, 0x00802000, 0x00802081, 0x00000081, 0x00000000, 0x00000000, 0x00802000,
   0x00002080, 0x00800080, 0x00800081, 0x00000001, 0x00802001, 0x00002081, 0x00002081, 0x00000080,
   0x00802081, 0x00000081, 0x00000001, 0x00002000, 0x00800001, 0x00002001, 0x00802080, 0x00800081,
   0x00002001, 0x00002080, 0x00800000, 0x00802001, 0x00000080, 0x00800000, 0x00002000, 0x00802080
};
 
//Selection function 5
static const uint32_t sp5[64] =
{
   0x00000100, 0x02080100, 0x02080000, 0x42000100, 0x00080000, 0x00000100, 0x40000000, 0x02080000,
   0x40080100, 0x00080000, 0x02000100, 0x40080100, 0x42000100, 0x42080000, 0x00080100, 0x40000000,
   0x02000000, 0x40080000, 0x40080000, 0x00000000, 0x40000100, 0x42080100, 0x42080100, 0x02000100,
   0x42080000, 0x40000100, 0x00000000, 0x42000000, 0x02080100, 0x02000000, 0x42000000, 0x00080100,
   0x00080000, 0x42000100, 0x00000100, 0x02000000, 0x40000000, 0x02080000, 0x42000100, 0x40080100,
   0x02000100, 0x40000000, 0x42080000, 0x02080100, 0x40080100, 0x00000100, 0x02000000, 0x42080000,
   0x42080100, 0x00080100, 0x42000000, 0x42080100, 0x02080000, 0x00000000, 0x40080000, 0x42000000,
   0x00080100, 0x02000100, 0x40000100, 0x00080000, 0x00000000, 0x40080000, 0x02080100, 0x40000100
};
 
//Selection function 6
static const uint32_t sp6[64] =
{
   0x20000010, 0x20400000, 0x00004000, 0x20404010, 0x20400000, 0x00000010, 0x20404010, 0x00400000,
   0x20004000, 0x00404010, 0x00400000, 0x20000010, 0x00400010, 0x20004000, 0x20000000, 0x00004010,
   0x00000000, 0x00400010, 0x20004010, 0x00004000, 0x00404000, 0x20004010, 0x00000010, 0x20400010,
   0x20400010, 0x00000000, 0x00404010, 0x20404000, 0x00004010, 0x00404000, 0x20404000, 0x20000000,
   0x20004000, 0x00000010, 0x20400010, 0x00404000, 0x20404010, 0x00400000, 0x00004010, 0x20000010,
   0x00400000, 0x20004000, 0x20000000, 0x00004010, 0x20000010, 0x20404010, 0x00404000, 0x20400000,
   0x00404010, 0x20404000, 0x00000000, 0x20400010, 0x00000010, 0x00004000, 0x20400000, 0x00404010,
   0x00004000, 0x00400010, 0x20004010, 0x00000000, 0x20404000, 0x20000000, 0x00400010, 0x20004010
};
 
//Selection function 7
static const uint32_t sp7[64] =
{
   0x00200000, 0x04200002, 0x04000802, 0x00000000, 0x00000800, 0x04000802, 0x00200802, 0x04200800,
   0x04200802, 0x00200000, 0x00000000, 0x04000002, 0x00000002, 0x04000000, 0x04200002, 0x00000802,
   0x04000800, 0x00200802, 0x00200002, 0x04000800, 0x04000002, 0x04200000, 0x04200800, 0x00200002,
   0x04200000, 0x00000800, 0x00000802, 0x04200802, 0x00200800, 0x00000002, 0x04000000, 0x00200800,
   0x04000000, 0x00200800, 0x00200000, 0x04000802, 0x04000802, 0x04200002, 0x04200002, 0x00000002,
   0x00200002, 0x04000000, 0x04000800, 0x00200000, 0x04200800, 0x00000802, 0x00200802, 0x04200800,
   0x00000802, 0x04000002, 0x04200802, 0x04200000, 0x00200800, 0x00000000, 0x00000002, 0x04200802,
   0x00000000, 0x00200802, 0x04200000, 0x00000800, 0x04000002, 0x04000800, 0x00000800, 0x00200002
};
 
//Selection function 8
static const uint32_t sp8[64] =
{
   0x10001040, 0x00001000, 0x00040000, 0x10041040, 0x10000000, 0x10001040, 0x00000040, 0x10000000,
   0x00040040, 0x10040000, 0x10041040, 0x00041000, 0x10041000, 0x00041040, 0x00001000, 0x00000040,
   0x10040000, 0x10000040, 0x10001000, 0x00001040, 0x00041000, 0x00040040, 0x10040040, 0x10041000,
   0x00001040, 0x00000000, 0x00000000, 0x10040040, 0x10000040, 0x10001000, 0x00041040, 0x00040000,
   0x00041040, 0x00040000, 0x10041000, 0x00001000, 0x00000040, 0x10040040, 0x00001000, 0x00041040,
   0x10001000, 0x00000040, 0x10000040, 0x10040000, 0x10040040, 0x10000000, 0x00040000, 0x10001040,
   0x00000000, 0x10041040, 0x00040040, 0x10000040, 0x10040000, 0x10001000, 0x10001040, 0x00000000,
   0x10041040, 0x00041000, 0x00041000, 0x00001040, 0x00001040, 0x00040040, 0x10000000, 0x10041000
};
 
int desInit(DesContext *context, const uint8_t *key, size_t keyLen)
{
   uint16_t i;
   uint32_t c;
   uint32_t d;
 
   //Check parameters
   if(context == NULL || key == NULL)
      return -1;
 
   //Invalid key length?
   if(keyLen != 8)
      return -1;
 
   //Copy the key
   memcpy(&c, key + 0, 4);
   memcpy(&d, key + 4, 4);

   c = htobe32(c);
   d = htobe32(d);
 
   //Permuted choice 1
   DES_PC1(c, d);
 
   //Generate the key schedule
   for(i = 0; i < 16; i++)
   {
      //Individual blocks are shifted left
      if(i == 0 || i == 1 || i == 8 || i == 15)
      {
         c = ROL28(c, 1);
         d = ROL28(d, 1);
      }
      else
      {
         c = ROL28(c, 2);
         d = ROL28(d, 2);
      }
 
      //Permuted choice 2
      context->ks[2 * i] =
         ((c << 4)  & 0x24000000) | ((c << 28) & 0x10000000) |
         ((c << 14) & 0x08000000) | ((c << 18) & 0x02080000) |
         ((c << 6)  & 0x01000000) | ((c << 9)  & 0x00200000) |
         ((c >> 1)  & 0x00100000) | ((c << 10) & 0x00040000) |
         ((c << 2)  & 0x00020000) | ((c >> 10) & 0x00010000) |
         ((d >> 13) & 0x00002000) | ((d >> 4)  & 0x00001000) |
         ((d << 6)  & 0x00000800) | ((d >> 1)  & 0x00000400) |
         ((d >> 14) & 0x00000200) | ((d)       & 0x00000100) |
         ((d >> 5)  & 0x00000020) | ((d >> 10) & 0x00000010) |
         ((d >> 3)  & 0x00000008) | ((d >> 18) & 0x00000004) |
         ((d >> 26) & 0x00000002) | ((d >> 24) & 0x00000001);
 
      context->ks[2 * i + 1] =
         ((c << 15) & 0x20000000) | ((c << 17) & 0x10000000) |
         ((c << 10) & 0x08000000) | ((c << 22) & 0x04000000) |
         ((c >> 2)  & 0x02000000) | ((c << 1)  & 0x01000000) |
         ((c << 16) & 0x00200000) | ((c << 11) & 0x00100000) |
         ((c << 3)  & 0x00080000) | ((c >> 6)  & 0x00040000) |
         ((c << 15) & 0x00020000) | ((c >> 4)  & 0x00010000) |
         ((d >> 2)  & 0x00002000) | ((d << 8)  & 0x00001000) |
         ((d >> 14) & 0x00000808) | ((d >> 9)  & 0x00000400) |
         ((d)       & 0x00000200) | ((d << 7)  & 0x00000100) |
         ((d >> 7)  & 0x00000020) | ((d >> 3)  & 0x00000011) |
         ((d << 2)  & 0x00000004) | ((d >> 21) & 0x00000002);
   }
 
   //No error to report
   return 1;
}
 
void desDecryptBlock(DesContext *context, const uint8_t *input, uint8_t *output)
{
   uint16_t i;
   uint32_t left;
   uint32_t right;
   uint32_t temp;
 
   //Keys in the key schedule must be applied in reverse order
   uint32_t *ks = context->ks + 30;
 
   //Copy the ciphertext from the input buffer
   memcpy(&left, input + 0, 4);
   memcpy(&right, input + 4, 4);

   left = htobe32(left);
   right = htobe32(right);
 
   //Initial permutation
   DES_IP(left, right);
 
   //16 rounds of computation are needed
   for(i = 0; i < 16; i++, ks -= 2)
   {
      DES_ROUND(left, right, ks);
   }
 
   //Inverse IP permutation
   DES_FP(right, left);
 
   right = htobe32(right);
   left = htobe32(left);
   //Copy the resulting plaintext
   memcpy(output + 0, &right, sizeof(right));
   memcpy(output + 4, &left, sizeof(left));
}

int main(int argc, char **argv)
{
	int opt;
	DesContext s;
	uint64_t input = 0;
	uint64_t key = 0;
	uint64_t output = 0;

	while((opt = getopt(argc, argv, "hi:k:")) != -1)
	{
		switch(opt)
		{
			case 'i':	/* Input */
				sscanf(optarg, "%llx", &input);
				input = htobe64(input);
				break;
			case 'k':	/* Key from argument */
				sscanf(optarg, "%llx", &key);
				key = htobe64(key);
				break;
			case 'h': /* Help */
			default:
				fprintf(stderr, "Usage: %s –i [input] –k [key]\n", argv[0]);
				exit(0);
				break;
		}
	}

	desInit(&s, &key, 8);
	desDecryptBlock(&s, &input, &output);
	output = htobe64(output);

	//printf("input = 0x%016llX, key = 0x%016llX\n", input, key);
	printf("0x%016llX\n", output);
}
