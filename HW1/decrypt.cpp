#include <iostream>
#include <string>
#include <string.h>
#include <algorithm>
#include <math.h>
using namespace std;

string Caesar(string cipher, int key);
string Playfair(string cipher, string key);
string Vernam(string cipher, string key);
string Railfence(string cipher, int key);
string RowTransposition(string cipher, string key);
int str2int(string str);

int main(int argc, char* argv[]) {
	//-m method, -i input, -k key

	string method, input, key;

	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-m")) {
			method = argv[i + 1];
		}
		if (!strcmp(argv[i], "-i")) {
			input = argv[i + 1];
			input.erase(remove(input.begin(), input.end(), ' '), input.end());
		}
		if (!strcmp(argv[i], "-k")) {
			key = argv[i + 1];
			key.erase(remove(key.begin(), key.end(), ' '), key.end());
		}
	}

	if (method == "caesar") {
		int k = str2int(key);
		cout << Caesar(input, k);
	}
	else if(method == "playfair"){
		cout << Playfair(input, key);
	}
	else if (method == "vernam") {
		cout << Vernam(input, key);
	}
	else if (method == "railfence") {
		int k = str2int(key);
		cout << Railfence(input, k);
	}
	else if (method == "row") {
		cout << RowTransposition(input, key);
	}
}

string Caesar(string cipher, int key) {
	string plain;
	for (int j = 0; j < cipher.length(); j++) {
		char c = tolower(cipher[j]) - key;
		plain += c > 'a' ? c : c + 26; //大寫字母和小寫字母中間有6個符號
	}
	return plain;
}

string Playfair(string cipher, string key) {
	string table;
	string used = "abcdefghiklmnopqrstuvwxyz";
	string plain;

	for (int i = 0; i < key.length(); i++) {
		if (key[i] == 'j') key[i] = 'i'; //把j視為i
		if (used.find(key[i]) + 1) table += key[i]; //防止false所以+1
		used.erase(remove(used.begin(), used.end(), key[i]), used.end());
	}

	for (int i = 0; i < used.length(); i++) {
		table += used[i];
	}

	//for (int i = 0; i < table.length(); i++) {
	//	cout << table[i] << " ";
	//	if (i != 0 && i % 5 == 4) cout << endl;
	//}

	for (int i = 0; i < cipher.length(); i += 2) {
		char first = tolower(cipher[i]);
		char second = tolower(cipher[i + 1]);
		
		int fpos = table.find(first);
		int spos = table.find(second);

		if(fpos % 5 == spos % 5) { //same column
			fpos < 5 ? plain += table[fpos + 20] : plain += table[fpos - 5];
			spos < 5 ? plain += table[spos + 20] : plain += table[spos - 5];
			continue;
		} 
		if(abs(fpos - spos) < 5 && (fpos / 5 == spos / 5)) { //same row
			fpos % 5 == 0 ? plain += table[fpos + 4] : plain += table[fpos - 1];
			spos % 5 == 0 ? plain += table[spos + 4] : plain += table[spos - 1];
			continue;
		}
		int relate = (fpos % 5) - (spos % 5); //相對位置
		plain += table[fpos - relate];
		plain += table[spos + relate];

		plain.erase(remove(plain.begin(), plain.end(), 'x'), plain.end()); //去掉x
	}

	return plain;
	//cout << table << endl;
}

string Vernam(string cipher, string key) {
	string plain;

	for (int i = 0; i < cipher.length(); i++) {
		char c = tolower(cipher[i]) - 'a';
		char k = key[i] - 'a';
		plain += (c ^ k) + 'a';
	}

	return plain;
}

string Railfence(string cipher, int key) {
	string plain;

	int* table = new int[cipher.length()];
	int* count = new int[key]; //去計算數列中的出現數字，用來分行
	for (int i = 0; i < key; i++) { //初始化count
		count[i] = 0;
	}

	int index = 1;
	bool goDown = true;
	for (int i = 0; i < cipher.length(); i++) {
		cipher[i] = tolower(cipher[i]);

		table[i] = index;
		count[index - 1] += 1;
		goDown ? index++ : index--;
		if (index == key) goDown = false;
		if (index == 1) goDown = true;
	}

	string* plainTable = new string[key];
	int pos = 0;
	for (int i = 0; i < key; i++) {
		plainTable[i] = cipher.substr(pos, count[i]);
		pos += count[i];
		//cout << plainTable[i] << endl;
	}

	goDown = true;
	int now = 0;
	int* rate = new int[key];
	for (int i = 0; i < key; i++) rate[i] = 0;
	for (int i = 0; i < cipher.length(); i++) {
		plain += plainTable[now][rate[now]];
		rate[now]++;
		goDown ? now++ : now--;
		if (now == key - 1) goDown = false;
		if (now == 0) goDown = true;
	}

	return plain;
}

string RowTransposition(string cipher, string key) {
	string plain;
	int keyLen = key.length();
	int rows = cipher.length() / keyLen;
	string* plainTable = new string[keyLen];

	for (int i = 0; i < keyLen; i++) {
		plainTable[i] = cipher.substr(rows * (key[i] - '1'), rows);
	}

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < keyLen; j++) {
			plain += tolower(plainTable[j][i]);
		}
	}

	return plain;
}

int str2int(string str) {
	int re = 0;
	for (int i = 0; i < str.length(); i++) {
		re * 10;
		re += str[i] - '0';
	}
	return re;
}
