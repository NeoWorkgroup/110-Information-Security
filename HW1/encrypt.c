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

#define strip_j(x)	\
	((x) == 'J' ? 'I' : (x))

// 0 ~ 24
#define noj_toi(x)	\
	((x) > 'I' ? ((x) - 'B') : ((x) - 'A'))

#define ito_noj(x)	\
	((x) + 'A' > 'I' ? (x) + 'B' : (x) + 'A')

void playfair_encrypt(char *input, char *key, char *output)
{
	int8_t occur[25];
	int8_t s[25];
	for(int i = 0; i < 25; i++)
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
		s[i++] = a;
		s_end = i;
		occur[a] += 1;
	}

	for(int i = 0; i < 25; i++)
	{
		printf("%3hhd ", occur[i]);
		if(i % 5 == 4)
			putchar('\n');
	}

	// fill the rest
	for(int i = 0; i < 25; i++)
	{
		int j_end;
		for(int j = 0; j < 25; j_end = j++)
		{
			if(s[j] == i)
				break;
		}
		if(j_end >= 24)
			s[s_end++] = i;
	}

	puts("sbox:");
	for(int i = 0; i < 25; i++)
	{
		printf("%2c ", ito_noj(s[i]));
		if(i % 5 == 4)
			putchar('\n');
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
	size_t len = strlen(input);

	for(int j = 0; j < k; j++)
	{
		for(int i = 0, c; (c = (i * k + j)) < len; i++)
		{
			*output++ = input[c];
		}
	}
}

void row_encrypt(char *input, char *key, char *output)
{
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

	output = strdup(input);

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
