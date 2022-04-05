#include <iostream>
#include <string.h>
#include <string>
using namespace std;

string hex2bin(string);
string bin2hex(string);
string int2bin(int);
string int2hex(int);
string myXOR(string, string);
string leftShift(string);
int bin2int(string);

/*************************************************************/
/*TABLES*/
/*************************************************************/

int Expansion[48] = {32, 1 , 2 , 3 , 4 , 5,
					 4 , 5 , 6 , 7 , 8 , 9,
					 8 , 9 , 10, 11, 12, 13,
					 12, 13, 14, 15, 16, 17,
					 16, 17, 18, 19, 20, 21,
					 20, 21, 22, 23, 24, 25,
					 24, 25, 26, 27, 28, 29,
					 28, 29, 30, 31, 32, 1};

int P[32] = {	16,	7,	20,	21,	29,	12,	28,	17,	1,	15,	23,	26,	5,	18,	31,	10,
				2,	8,	24,	14,	32,	27,	3,	9,	19,	13,	30,	6,	22,	11,	4,	25 };

int IP[64] = {  58,	50,	42,	34,	26,	18,	10,	2,
				60,	52,	44,	36,	28,	20,	12,	4,
				62,	54,	46,	38,	30,	22,	14,	6,
				64,	56,	48,	40,	32,	24,	16,	8,
				57,	49,	41,	33,	25,	17,	9,	1,
				59,	51,	43,	35,	27,	19,	11,	3,
				61,	53,	45,	37,	29,	21,	13,	5,
				63,	55,	47,	39,	31,	23,	15,	7 };

int FP[64] = {	40,	8,	48,	16,	56,	24,	64,	32,
				39,	7,	47,	15,	55,	23,	63,	31,
				38,	6,	46,	14,	54,	22,	62,	30,
				37,	5,	45,	13,	53,	21,	61,	29,
				36,	4,	44,	12,	52,	20,	60,	28,
				35,	3,	43,	11,	51,	19,	59,	27,
				34,	2,	42,	10,	50,	18,	58,	26,
				33,	1,	41,	9,	49,	17,	57,	25 };

int S_one[4][16] = { {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
				  	 {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
					 {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
					 {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13} };

int S_two[4][16] = { {15,	1,	8,	14,	6,	11,	3,	4,	9,	7,	2,	13,	12,	0,	5,	10},
				 	 {3,	13,	4,	7,	15,	2,	8,	14,	12,	0,	1,	10,	6,	9,	11,	5},
					 {0,	14,	7,	11,	10,	4,	13,	1,	5,	8,	12,	6,	9,	3,	2,	15},
					 {13,	8,	10,	1,	3,	15,	4,	2,	11,	6,	7,	12,	0,	5,	14,	9} };

int S_three[4][16] = { {10,	0,	9,	14,	6,	3,	15,	5,	1,	13,	12,	7,	11,	4,	2,	8},
					   {13,	7,	0,	9,	3,	4,	6,	10,	2,	8,	5,	14,	12,	11,	15,	1},
					   {13,	6,	4,	9,	8,	15,	3,	0,	11,	1,	2,	12,	5,	10,	14,	7},
					   {1,	10,	13,	0,	6,	9,	8,	7,	4,	15,	14,	3,	11,	5,	2,	12} };

int S_four[4][16] = { {7,	13,	14,	3,	0,	6,	9,	10,	1,	2,	8,	5,	11,	12,	4,	15},
					  {13,	8,	11,	5,	6,	15,	0,	3,	4,	7,	2,	12,	1,	10,	14,	9},
					  {10,	6,	9,	0,	12,	11,	7,	13,	15,	1,	3,	14,	5,	2,	8,	4},
					  {3,	15,	0,	6,	10,	1,	13,	8,	9,	4,	5,	11,	12,	7,	2,	14} };

int S_five[4][16] = { {2,	12,	4,	1,	7,	10,	11,	6,	8,	5,	3,	15,	13,	0,	14,	9},
					  {14,	11,	2,	12,	4,	7,	13,	1,	5,	0,	15,	10,	3,	9,	8,	6},
					  {4,	2,	1,	11,	10,	13,	7,	8,	15,	9,	12,	5,	6,	3,	0,	14},
					  {11,	8,	12,	7,	1,	14,	2,	13,	6,	15,	0,	9,	10,	4,	5,	3} };

int S_six[4][16] = { {12,	1,	10,	15,	9,	2,	6,	8,	0,	13,	3,	4,	14,	7,	5,	11},
					 {10,	15,	4,	2,	7,	12,	9,	5,	6,	1,	13,	14,	0,	11,	3,	8},
					 {9,	14,	15,	5,	2,	8,	12,	3,	7,	0,	4,	10,	1,	13,	11,	6},
					 {4,	3,	2,	12,	9,	5,	15,	10,	11,	14,	1,	7,	6,	0,	8,	13} };

int S_seven[4][16] = { {4,	11,	2,	14,	15,	0,	8,	13,	3,	12,	9,	7,	5,	10,	6,	1},
					   {13,	0,	11,	7,	4,	9,	1,	10,	14,	3,	5,	12,	2,	15,	8,	6},
					   {1,	4,	11,	13,	12,	3,	7,	14,	10,	15,	6,	8,	0,	5,	9,	2},
					   {6,	11,	13,	8,	1,	4,	10,	7,	9,	5,	0,	15,	14,	2,	3,	12} };

int S_eight[4][16] = { {13,	2,	8,	4,	6,	15,	11,	1,	10,	9,	3,	14,	5,	0,	12,	7},
					   {1,	15,	13,	8,	10,	3,	7,	4,	12,	5,	6,	11,	0,	14,	9,	2},
					   {7,	11,	4,	1,	9,	12,	14,	2,	0,	6,	10,	13,	15,	3,	5,	8},
					   {2,	1,	14,	7,	4,	10,	8,	13,	15,	12,	9,	0,	3,	5,	6,	11} };

int PC_1[56] = {    57,	49,	41,	33,	25,	17,	9,	1,	58,	50,	42,	34,	26,	18,
					10,	2,	59,	51,	43,	35,	27,	19,	11,	3,	60,	52,	44,	36,
					63,	55,	47,	39,	31,	23,	15,	7,	62,	54,	46,	38,	30,	22,
					14,	6,	61,	53,	45,	37,	29,	21,	13,	5,	28,	20,	12,	4 };

int PC_2[48] = {    14,	17,	11,	24,	1,	5,	3,	28,	15,	6,	21,	10,
					23,	19,	12,	4,	26,	8,	16,	7,	27,	20,	13,	2,
					41,	52,	31,	37,	47,	55,	30,	40,	51,	45,	33,	48,
					44,	49,	39,	56,	34,	53,	46,	42,	50,	36,	29,	32 };

/*************************************************************/
/*FUNCTION*/
/*************************************************************/

string hex2bin(string hex) {
	string bin;
	for (int i = 0; i < hex.length(); i++) {
		int c = hex[i] - '0';
		if (c > 10) c -= 7;
		bin += int2bin(c);
	}
	return bin;
}

string bin2hex(string bin) {
	string hex;
	for (int i = 0; i < bin.length(); i += 4) {
		string subbin = bin.substr(i, 4);
		int a = bin2int(subbin);

		if (a < 10) hex += a + '0';
		if (a >= 10) hex += a + 'A' - 10;
	}
	return hex;
}

string int2bin(int z) {
	string bin;
	for (int i = 0; i < 4; i++) {
		int j = z % 2;
		bin = char(j + '0') + bin;
		z = z / 2;
	}
	return bin;
}

string myXOR(string a, string b) {
	string re;
	for (int i = 0; i < a.length(); i++) {
		if (a[i] == b[i]) re += '0';
		if (a[i] != b[i]) re += '1';
	}
	return re;
}

string leftShift(string str) {
	str = str + str[0]; // + '0';
	str.erase(str.begin());
	return str;
}

int bin2int(string str) {
	int i = 0;
	for (int j = 0; j < str.length(); j++) {
		i *= 2;
		i += (str[j] - '0');
	}
	return i;
}

/*************************************************************/
/*MAIN FUNCTION*/
/*************************************************************/

int main(int argc, char* argv[]) {
	char* in;
	char* k;
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-i")) {
			in = argv[i + 1];
			i++;
		}
		if (!strcmp(argv[i], "-k")) {
			k = argv[i + 1];
			i++;
		}
	}

	string input(in), key(k);
	//去掉0x
	input.erase(input.find("0x"), 2);	key.erase(key.find("0x"), 2);
	//補0
	while (input.length() < 16) {
		input = '0' + input;
	}
	while (key.length() < 16) {
		key = '0' + key;
	}
	//cout << input << " " << key;


	//將16進位轉成2進位
	string input_bin = hex2bin(input);
	string key_bin = hex2bin(key);

	//初始置換
	string IP_bin;
	for (int i = 0; i < 64; i++) {
		IP_bin = IP_bin + input_bin[IP[i] - 1];
	}

	string L = IP_bin.substr(0, 32);
	string R = IP_bin.substr(32, 32);

	//PC-1
	string PCm1;
	for (int i = 0; i < 56; i++) {
		PCm1 += key_bin[PC_1[i] - 1];
	}

	string keyL = PCm1.substr(0, 28);
	string keyR = PCm1.substr(28, 28);

	for (int i = 0; i < 16; i++) {
		keyL = leftShift(keyL);
		keyR = leftShift(keyR);
		if (i != 0 && i != 1 && i != 8 && i != 15) {
			keyL = leftShift(keyL);
			keyR = leftShift(keyR);
		}

		string roundKey;
		for (int j = 0; j < 48; j++) {
			if (j < 24) {
				roundKey = roundKey + keyL[PC_2[j] - 1];
			}
			else {
				roundKey = roundKey + keyR[PC_2[j] - 29];
			}
		}

		string expand_R;
		for (int j = 0; j < 48; j++) {
			expand_R = expand_R + R[Expansion[j] - 1];
		}

		//cout << roundKey << endl << expand_R << endl;

		string XOR_R = myXOR(roundKey, expand_R);

		//cout << XOR_R << endl << endl;

		string shorten_R;
		for (int j = 0; j < 8; j++) {
			int S;
			string str;
			switch (j) {
			case 0:
				str = XOR_R.substr(0, 1) + XOR_R.substr(5, 1);
				S = S_one[bin2int(str)][bin2int(XOR_R.substr(1, 4))];
				break;
			case 1:
				str = XOR_R.substr(6, 1) + XOR_R.substr(11, 1);
				S = S_two[bin2int(str)][bin2int(XOR_R.substr(7, 4))];
				break;
			case 2:
				str = XOR_R.substr(12, 1) + XOR_R.substr(17, 1);
				S = S_three[bin2int(str)][bin2int(XOR_R.substr(13, 4))];
				break;
			case 3:
				str = XOR_R.substr(18, 1) + XOR_R.substr(23, 1);
				S = S_four[bin2int(str)][bin2int(XOR_R.substr(19, 4))];
				break;
			case 4:
				str = XOR_R.substr(24, 1) + XOR_R.substr(29, 1);
				S = S_five[bin2int(str)][bin2int(XOR_R.substr(25, 4))];
				break;
			case 5:
				str = XOR_R.substr(30, 1) + XOR_R.substr(35, 1);
				S = S_six[bin2int(str)][bin2int(XOR_R.substr(31, 4))];
				break;
			case 6:
				str = XOR_R.substr(36, 1) + XOR_R.substr(41, 1);
				S = S_seven[bin2int(str)][bin2int(XOR_R.substr(37, 4))];
				break;
			case 7:
				str = XOR_R.substr(42, 1) + XOR_R.substr(47, 1);
				S = S_eight[bin2int(str)][bin2int(XOR_R.substr(43, 4))];
				break;
			}
			shorten_R = shorten_R + int2bin(S);
		}
		//cout << shorten_R << endl << endl;

		//P置換
		string newR;
		for (int j = 0; j < 32; j++) {
			newR = newR + shorten_R[P[j] - 1];
		}
		//cout << shorten_R << endl << newR << endl;

		//cout << R << endl << myXOR(L, newR) << endl;

		string buffer = L;
		L = R;
		R = myXOR(buffer, newR);
		//cout << L << R;
	}

	//cout << endl;
	
	string output_bin = R + L;
	string FP_bin;
	for (int i = 0; i < 64; i++) {
		FP_bin += output_bin[FP[i] - 1];
	}
	
	string output = bin2hex(FP_bin);
	//cout << output_bin << endl;
	//cout << FP_bin << endl;
	cout << "0x" << output;
}