#pragma once
//#pragma comment(lib, "rpcrt4.lib")  // UuidCreate - Minimum supported OS Win 2000

//#include "uuid.h"
#include <windows.h>
#include <iostream>
#include <vector>

using namespace std;

vector <uint8_t>  ugenv(UUID &uuid, uint8_t * strfat);

inline unsigned char hex_digit_to_nybble(char ch)
{
	switch (ch)
	{
	case '0': return 0x0;
	case '1': return 0x1;
	case '2': return 0x2;
	case '3': return 0x3;
	case '4': return 0x4;
	case '5': return 0x5;
	case '6': return 0x6;
	case '7': return 0x7;
	case '8': return 0x8;
	case '9': return 0x9;
	case 'a': return 0xa;
	case 'A': return 0xa;
	case 'b': return 0xb;
	case 'B': return 0xb;
	case 'c': return 0xc;
	case 'C': return 0xc;
	case 'd': return 0xd;
	case 'D': return 0xd;
	case 'e': return 0xe;
	case 'E': return 0xe;
	case 'f': return 0xf;
	case 'F': return 0xf;
	default: return 0xff;  // throw std::invalid_argument();
	}
}


vector <uint8_t>  ugenv(UUID &uuid, uint8_t * strfat)
{
	UuidCreate(&uuid);
	char *str;
	vector <uint8_t> u;

	u.clear();
	UuidToStringA(&uuid, (RPC_CSTR*)&str);
	cout << str << endl;

	for (int i = 0; i < 4; i++) {
		u.push_back ( 0xff & ((hex_digit_to_nybble(str[i * 2]) << 4) & 0xf0) + (hex_digit_to_nybble(str[(i * 2) + 1]) & 0xf) );
	} 
	int j = 0;
	for (int i = 0; i < 12; i++) {
		if (str[i] == '-') { j++ ; continue; }
		strfat[i - j] = toupper(str[i]);
	}
	cout << "ascii uuid ";
	for (int i = 0; i < 11; i++) {
		printf("%02X", strfat[i]);
	}
	cout << endl << "Hex uuid : ";
	for (int i = 0; i < 4; i++) {
		printf("%02X", u[i]);
	}
	cout << endl;
	RpcStringFreeA((RPC_CSTR*)&str);
	return u;
}

/*
int main()
{
	UUID uuid;
	uint8_t struuid[20];
	ugenv(uuid, struuid);
	for (int i = 0; i < 4; i++) { printf("%02X", struuid[i]); }
	cout << endl;

}
*/


//int main()
//{
//	RPC_S_OK;

	/*
	UUID uuid;
	if (UuidCreate(&uuid) == 0) {
		unsigned char* wszUuid = NULL;
		UuidToStringA(&uuid, &wszUuid);
		if (wszUuid != NULL) {
			string* res = new string((char*)wszUuid);
			qDebug(res->c_str());
			return res;
		}
		else {
			return new string("");
		}
	}
	*/


	//UUID uuid;
	/*
	if (UuidCreate(&uuid) == 0) {
		unsigned char* wszUuid = NULL;
		UuidToString(&uuid, &wszUuid);
		if (wszUuid != NULL) {
			string* res = new string((char*)wszUuid);
			qDebug(res->c_str());
			return res;
		}
		else {
			return new string("");
		}
	} */

	/*
	QUuid uuid = QUuid::createUuid();
	QString suuid = uuid.toString();

	string res = suuid.toStdString();

	// removes the { } characters
	res = res.substr(1, res.size() - 2);
	return new string(res);
	*/


//}