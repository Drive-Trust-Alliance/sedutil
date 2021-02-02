/* C:B**************************************************************************
This software is Copyright Fidelity Height LLC 2021 <fidelityheight.com>

This file is NOT part of sedutil.

sedutil is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

sedutil is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sedutil.  If not, see <http://www.gnu.org/licenses/>.

* C:E********************************************************************** */


//Basic read and write from Windows Registry. Be careful ! You modify the registry at your own risk !
//Przemyslaw Zaworski, 2017

#pragma warning(disable: 4224) //C2224: conversion from int to char , possible loss of data
#pragma warning(disable: 4244) //C4244: 'argument' : conversion from 'uint16_t' to 'uint8_t', possible loss of data
#pragma comment(lib, "rpcrt4.lib")  // UuidCreate - Minimum supported OS Win 2000

#include <windows.h>
#include <iostream>
#include "DtaHashPwd.h"

//#disable _CRT_SECURE_NO_WARNINGS
#pragma warning(disable: 4996)

//On a Windows 64 - bit system the Registry is actually divided into two parts.One section is used by 64 - bit processes, and one part by 32 - bit processes.

//For example, if a 32 - bit application programatically writes to what it believes is HKLM\SOFTWARE\Company\Application, it's actually redirected by the WoW64-layer to HKLM\SOFTWARE\Wow6432Node\Company\Application.
class regMgr {
private:
	//LPCTSTR key_value = new TCHAR[256] ; // 
	TCHAR key_value[64] = { "key_value" };
	BOOL loggingEnable = FALSE;
	// seed[8] = { L'F', L'.', L'D', L'y', L'e', L'T', L'$', L'i' }; 
	// somehow if size is 8, we can not initiate 8 elemlent, we need a larger array to hold the initial value

	TCHAR seed[9] = { 'F', '.', 'D', 'y', 'e', 'T', '$', 'i' };
	TCHAR value[255];
	DWORD value_length = 255;
	vector <uint8_t> hash; // salt has been assign for 8 byte
	BOOL os64; 
public:

	regMgr(){
		//std::wcout << green << L"$$$$$ constructor: seed=" << seed << L", key_value=" << key_value << std::endl; //  << std::flush;  
		os64 = IsX64win();
	}
	~regMgr() {  }; // delete key_value; }   // { if (key_value) free(key_value) };
	int registry_readex(HKEY hk, LPCTSTR subkey, LPCTSTR name, DWORD type);
	int registry_newex(HKEY hk, LPCTSTR subkey, LPCTSTR name, DWORD type, LPCTSTR value);
	//void regMgr::registry_writeex(HKEY hk, LPCTSTR subkey, LPCTSTR name, DWORD type, const char* value);
	int regMgr::registry_writeex(HKEY hk, LPCTSTR subkey, LPCTSTR name, DWORD type, LPCTSTR value);
	int registry_deleteex(HKEY hk, LPCTSTR subkey, LPCTSTR name, DWORD type);
	int registry_Delnode(HKEY hk, LPCTSTR subkey);
	BOOL get_logging() { return loggingEnable; };
	void set_logging(BOOL flag) { loggingEnable = flag;  }
	void hashkey(TCHAR *t); 
	BOOL IsX64win();
	BOOL isOS64() { return os64; }
};

