/* Use POSIX C Source */
#ifndef _POSIX_C_SOURCE
#  define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>
#include <ctype.h>

void panic(char *msg)
{
	fputs(msg, stderr);
	putc('\n', stderr);
	exit(1);
}

void caesar_encrypt(char *input, char *key, char *output)
{
	int k = atoi(key);

	for(int i = 0; input[i] != '\0'; i++)
	{
		char c = toupper(input[i]);
		*output++ = (c - 'A' + k) % 26 + 'A';
	}
	*output = '\0';
}

// No 'J'
#define strip_j(x)	\
	((x) == 'J' ? 'I' : (x))

// 'A' ~ 'Z' to  0 ~ 24
#define noj_toi(x)	\
	((x) > 'I' ? ((x) - 'B') : ((x) - 'A'))

// 0 ~ 24 to 'A' ~ 'Z'
#define ito_noj(x)	\
	((x) + 'A' > 'I' ? (x) + 'B' : (x) + 'A')

#define xy2t(x, y)	\
	((x) * 5 + (y))

#define t2x(t)		\
	((t) / 5)

#define t2y(t)		\
	((t) % 5)

int cindex(char *s, char p)
{
	for(int i = 0; i < strlen(s); i++)
	{
		if(s[i] == p)
			return i;
	}
	return -1;
}

void playfair_encrypt(char *input, char *key, char *output)
{
	int8_t occur[25];
	char s[26]; // include space for NUL
	for(int i = 0; i < 26; i++)
	{
		occur[i] = 0;
		s[i] = -1;
	}

	int s_end;
	for(int i = 0; *key != '\0'; key++)
	{
		int a = noj_toi(strip_j(toupper(*key)));
		if(occur[a])
			continue;
		s[i++] = ito_noj(a);
		s_end = i;
		occur[a] += 1;
	}

	// fill the rest
	for(int i = 0; i < 25; i++)
	{
		int j_end = -1;
		for(int j = 0; j < 25; j_end = j++)
		{
			if(s[j] == ito_noj(i))
				break;
		}
		if(j_end >= 24)
			s[s_end++] = ito_noj(i);
	}

	puts("sbox:");
	for(int i = 0; i < 25; i++)
	{
		printf("%2c ", s[i]);
		if(i % 5 == 4)
			putchar('\n');
	}

	*output = '\0';

	for(int i = 0; *input != '\0'; i++)
	{
		int ax, ay, bx, by;
		int ai, bi;

		ai = cindex(s, toupper(*input));
		if(toupper(*input) == toupper(*(input + 1)) || *(input + 1) == '\0')
		{
			bi = cindex(s, 'X');
			input += 1;
		}
		else
		{
			bi = cindex(s, toupper(*(input + 1)));
			input += 2;
		}

		printf("A = %c(%d), B = %c(%d)\n", ito_noj(ai), ai, ito_noj(bi), ai);

		ax = t2x(ai);
		ay = t2y(ai);

		bx = t2x(bi);
		by = t2y(bi);

		if(ax == bx)
		{
			ay++;
			ay %= 5;
			by++;
			by %= 5;
		}
		else if(ay == by)
		{
			ax++;
			ax %= 5;
			bx++;
			bx %= 5;
		}
		else
		{
			int t = ax;
			ax = bx;
			bx = t;
		}

		printf("AXY=(%d, %d)\n", ax, ay);
		printf("BXY=(%d, %d)\n", bx, by);

		ai = xy2t(ax, ay);
		bi = xy2t(bx, by);

		//printf("After: A = %c(%d), B = %c(%d)\n", ito_noj(ai), ai, ito_noj(bi), bi);

		*output++ = ito_noj(ai);
		*output++ = ito_noj(bi);
	}
}

void vernam_encrypt(char *input, char *key, char *output)
{
	for(; *input != '\0' && *key != '\0'; input++, key++)
	{
		*output++ = ((toupper(*input) - 'A') ^ (toupper(*key) - 'A')) + 'A';
	}
	*output = '\0';
}

void railfence_encrypt(char *input, char *key, char *output)
{
	int k = atoi(key);
	int len = strlen(input);
	char rows[k][len];

	for(int i = 0; i < k; i++)
	{
		rows[i][0] = '\0';
	}

	int dir = +1;
	int row = 0;
	for(int j = 0; j < len; j++)
	{
		output[j] = toupper(input[j]);
		strncat(rows[row], output + j, 1);
		row += dir;
		if(row == k -1 || row == 0)
			dir = -dir;
	}

	*output = '\0';
	for(int i = 0; i < k; i++)
	{
		strcat(output, rows[i]);
	}
}

void row_encrypt(char *input, char *key, char *output)
{
	int keylen = strlen(key);
	int len = strlen(input);
	int colsize;
	char rows[keylen][colsize = len / keylen + (len % keylen == 0 ? 0 : 1) + 1];

	for(int i = 0; i < keylen; i++)
		for(int j = 0; j < colsize; j++)
			rows[i][j] = '\0';

	for(int i = 0; i < len; i++)
	{
		printf("rows[%d][%d] = [%d]%c\n", i % keylen, i / keylen, i, input[i]);
		rows[i % keylen][i / keylen] = toupper(input[i]);
	}

	*output = '\0';
	for(int i = 1; i < (keylen + 1) && i <= 9; i++)
	{
		char *at = strchr(key, '0' + i);
		if(at == NULL)
			break;
		else
		{
			int index = at - key;
			printf("cat -> [%d] \"%s\"\n", index, rows[index]);
			strcat(output, rows[index]);
		}
	}
}

int main(int argc, char **argv)
{
	char *method;
	char *input;
	char *key;
	char *output;

	int opt;

	while((opt = getopt(argc, argv, "hm:i:k:")) != -1)
	{
		switch(opt)
		{
			case 'm':
				method = strdup(optarg);
				assert(method != NULL);
				break;
			case 'i':	/* Input */
				input = strdup(optarg);
				assert(input != NULL);
				break;
			case 'k':	/* Key from argument */
				key = strdup(optarg);
				assert(key != NULL);
				break;
			case 'h': /* Help */
			default:
				fprintf(stderr, "Usage: %s –m [method] –i [input] –k [key]\n", argv[0]);
				exit(0);
				break;
		}
	}

	output = calloc(strlen(input) + 1, '\0');

	if(strcmp(method, "caesar") == 0)
		caesar_encrypt(input, key, output);
	else if(strcmp(method, "playfair") == 0)
		playfair_encrypt(input, key, output);
	else if(strcmp(method, "vernam") == 0)
		vernam_encrypt(input, key, output);
	else if(strcmp(method, "railfence") == 0)
		railfence_encrypt(input, key, output);
	else if(strcmp(method, "row") == 0)
		row_encrypt(input, key, output);
	else
		panic("Invalid method");

	puts(output);
}
