/* C:B**************************************************************************
This software is Copyright 2021 Fidelity Height LLC 2021 <fidelityheight.com>

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
// regtry.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "regtry.h"


BOOL regMgr::IsX64win()
{
	UINT x64test = GetSystemWow64DirectoryA(NULL, 0);
	if (GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)  return FALSE;
	else return TRUE;
}

void regMgr::hashkey(TCHAR *t)
{
	//DtaHashPassword(hash, password, salt, iter);
	int iter;
	char password[64], buf_date[32] = { 0 }; // 8->32
	vector <uint8_t> salt(seed, seed + 8);

	SYSTEMTIME st, lt;
	GetSystemTime(&st);
	GetLocalTime(&lt);
	sprintf_s(buf_date, "%04d%02d%02d", lt.wYear, lt.wMonth, lt.wDay);
	strcpy_s(password, t); // copy to password directroy
   //std::cout << "password=" << password << std::endl;

	strcat_s(password, buf_date);
	//std::cout << "password + buf_date=" << password << std::endl;
	//strcpy_s(password, lstrlen(key_value), (const char *)key_value);
	
	DtaHashPasswordLogging(hash, password, salt, iter = 123); // generate 32-byte hex data in vector hash
}

//  take hk for registry read

int regMgr::registry_readex(HKEY hk, LPCTSTR subkey, LPCTSTR name, DWORD type)
{
	// input hk can be HKEY_CLASSES_ROOT 
	//HKEY_CURRENT_CONFIG 
	//HKEY_CURRENT_USER 
	//KEY_LOCAL_MACHINE 
	//HKEY_USERS
	//std::cout << "Enter regMgr::registry_readex" << std::endl;
	HKEY key;
	LSTATUS ls = ERROR_ACCESS_DENIED;

	ls = RegOpenKey(hk, subkey, &key);
	if (ls) {
		//std::cout << "RegOpenKey() error status = " << ls << std::endl; //else std::cout << "RegOpenKey() OK status = " << ls << std::endl;
		return ls;
	}
	ls = RegQueryValueEx(key, name, NULL, &type, (LPBYTE)&value, &value_length);
	if (ls) {
		//std::cout << "RegQueryValueEx() error status = " << ls << std::endl;
		return ls;
	}
	//else std::wcout << "RegQueryValueEx() OK status = " << ls << "value=" << value <<   std::endl;
	// value contain registry key of 64-byte ascii
	vector <uint8_t> h1, h2;
	for (int i = 0; i < 64; i++) h1.push_back(value[i]);
#if 0
	printf("read register hashed 64-byte data : ");
	for (int i = 0; i < h1.size(); i++) { printf("%C", h1[i]); }
	printf("\n");
#endif

	loggingEnable = FALSE;
	hashkey("ON");
	data2ascii(hash, h2);
	if (h1 == h2) {
		//std::cout << "hash match , logging is on" << std::endl;
		loggingEnable = TRUE;
	}
	else {
		//std::cout << "hash NOT match , logging is OFF" << std::endl;
		loggingEnable = FALSE;
	}

	ls = RegCloseKey(key);
	if (ls) {
		std::cout << "RegCloseKey() error status = " << ls << std::endl;
		//else std::cout << "RegCloseKey() OK status = " << ls << std::endl;
		return 902;
	}

	//std::wcout << value << std::endl;
	//std::cout << "Exit regMgr::registry_readex" << std::endl;
	return 0;
}


//void regMgr::registry_writeex(HKEY hk, LPCTSTR subkey, LPCTSTR name, DWORD type, const char* value)
int regMgr::registry_writeex(HKEY hk, LPCTSTR subkey, LPCTSTR name, DWORD type, LPCTSTR value)
{
	LSTATUS ls = ERROR_ACCESS_DENIED;
	std::wcout.flush();
	std::wcout << "entering regMgr::registry_writeex" << std::endl << std::flush;
	HKEY key;
	RegOpenKey(hk, subkey, &key);
#if 0
	std::wcout << L"reg write value=" << value << L" len=" << lstrlen(value) << L" sizeof(char)=" << sizeof(char) << std::endl << std::flush;
#endif	// hash ON OFF before 
	hashkey((TCHAR *)value); // get hashed value 32-byte hex data in vector<uint8_t> hash, 
							 // convert vector hash to TCHAR 
	vector<uint8_t> v;
	data2ascii(hash, v); // convert 32-byte hex data in hash vector to 64-byte ascii in vector v , v will be wrtten to
	TCHAR t[128];
	//std::cout << "hash.size()=" << hash.size() << "v.size()=" << v.size() <<  std::endl;
	for (int i = 0; i < hash.size() * 2; i++)
	{
		t[i] = v[i];
	}
	ls = RegSetValueEx(key, name, 0, type, (LPBYTE)t, v.size()); // hash.size()=3 *2=64 but in registry, it only write 34 byte of ascii

	ls |= RegCloseKey(key);
	std::wcout << "exiting regMgr::registry_writeex" << std::endl << std::flush;
	return 0;
}

// create a new key

//void registry_newex(HKEY hk, LPCTSTR subkey, LPCTSTR name, DWORD type, const char* value)
int regMgr::registry_newex(HKEY hk, LPCTSTR subkey, LPCTSTR name, DWORD type, LPCTSTR value)
{
	LSTATUS ls = ERROR_ACCESS_DENIED;
	std::cout << "Enter regMgr::registry_new" << std::endl;
	HKEY key;

	ls = RegCreateKey(hk, subkey, &key);
	if (ls) std::cout << "RegCreateKey() error status = " << ls << std::endl; //else std::cout << "RegCreateKey() OK status = " << ls << std::endl;
	ls = RegSetValueEx(key, name, 0, type, (LPBYTE)value, strlen(value) * sizeof(TCHAR));
	if (ls) std::cout << "RegSetValueEx() error status = " << ls << std::endl; //else std::cout << "RegSetValueExKey() OK status = " << ls << std::endl;
	ls = RegCloseKey(key);
	if (ls) {
		std::cout << "RegCloseKey() error status = " << ls << std::endl; //else std::cout << "RegCloseKey() OK status = " << ls << std::endl;
		return 901;
	}
	std::cout << "Exit regMgr::registry_new" << std::endl;
	return 0;
}

int regMgr::registry_deleteex(HKEY hk, LPCTSTR subkey, LPCTSTR name, DWORD type)
{
	LSTATUS ls;
	ls = RegDeleteKey(hk, subkey);
	if (ls) {
		std::cout << "RegDeleteKey() error status = " << ls << std::endl; //else std::cout << "RegDeleteKey() OK status = " << ls << std::endl;
		return 904;
	}
	else return 0;
}

//#include <windows.h>
//#include <stdio.h>
#include <strsafe.h>

//*************************************************************
//
//  RegDelnodeRecurse()
//
//  Purpose:    Deletes a registry key and all its subkeys / values.
//
//  Parameters: hKeyRoot    -   Root key
//              lpSubKey    -   SubKey to delete
//
//  Return:     TRUE if successful.
//              FALSE if an error occurs.
//
//*************************************************************

BOOL RegDelnodeRecurse(HKEY hKeyRoot, LPTSTR lpSubKey)
{
	LPTSTR lpEnd;
	LONG lResult;
	DWORD dwSize;
	TCHAR szName[MAX_PATH];
	HKEY hKey;
	FILETIME ftWrite;

	// First, see if we can delete the key without having
	// to recurse.

	lResult = RegDeleteKey(hKeyRoot, lpSubKey);

	if (lResult == ERROR_SUCCESS)
		return 0;

	lResult = RegOpenKeyEx(hKeyRoot, lpSubKey, 0, KEY_READ, &hKey);

	if (lResult != ERROR_SUCCESS)
	{
		if (lResult == ERROR_FILE_NOT_FOUND) {
			printf("Key not found.\n");
			return 907;
		}
		else {
			printf("Error opening key.\n");
			return 908;
		}
	}

	// Check for an ending slash and add one if it is missing.

	lpEnd = lpSubKey + lstrlen(lpSubKey);

	if (*(lpEnd - 1) != TEXT('\\'))
	{
		*lpEnd = TEXT('\\');
		lpEnd++;
		*lpEnd = TEXT('\0');
	}

	// Enumerate the keys

	dwSize = MAX_PATH;
	lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL,
		NULL, NULL, &ftWrite);

	if (lResult == ERROR_SUCCESS)
	{
		do {

			*lpEnd = TEXT('\0');
			StringCchCat(lpSubKey, MAX_PATH * 2, szName);

			if (!RegDelnodeRecurse(hKeyRoot, lpSubKey)) {
				break;
			}

			dwSize = MAX_PATH;

			lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL,
				NULL, NULL, &ftWrite);

		} while (lResult == ERROR_SUCCESS);
	}

	lpEnd--;
	*lpEnd = TEXT('\0');

	RegCloseKey(hKey);

	// Try again to delete the key.

	lResult = RegDeleteKey(hKeyRoot, lpSubKey);

	if (lResult == ERROR_SUCCESS)
		return 0;

	return 909;
}

//*************************************************************
//
//  RegDelnode()
//
//  Purpose:    Deletes a registry key and all its subkeys / values.
//
//  Parameters: hKeyRoot    -   Root key
//              lpSubKey    -   SubKey to delete
//
//  Return:     TRUE if successful.
//              FALSE if an error occurs.
//
//*************************************************************


BOOL regMgr::registry_Delnode(HKEY hKeyRoot, LPCTSTR lpSubKey)
{
	TCHAR szDelKey[MAX_PATH * 2];
	StringCchCopy(szDelKey, MAX_PATH * 2, lpSubKey);
	return RegDelnodeRecurse(hKeyRoot, szDelKey);
}






int logging(int argc, char * argv[] )
{
	// input hk can be 
	//HKEY_CLASSES_ROOT HKEY_CURRENT_CONFIG HKEY_CURRENT_USER 
	//HKEY_LOCAL_MACHINE HKEY_USERS
	LSTATUS ls;
	BOOL b;
	HKEY key;
	regMgr m;

	const char psw[22] = { 'f','I','d','E','l','I','t','y','!','h','E','I','g','h','t','$','9','5','0','3','5' };
	//fIdElIty!hEIght$95035
	// 21 char but require 22 byte array size 
	// fIdElIty!hEIght$95035
	// std::cout << psw << std::endl;
	if (argc == 1) {
		std::cout << "Enter the argument for argv[0]" << std::endl;
		return 0;
	}
	if (argc == 2) {
		if (strlen(argv[1]) == 0) {
			std::cout << "Please enter a correct argument" << std::endl;
			return 0;
		}

		for (int i = 0; i < strlen(argv[1]); i++) {
			argv[1][i]= ::tolower(argv[1][i]);
		}
	
		if (strcmp(argv[1], "off") == 0)
		{
			b = m.registry_writeex(HKEY_LOCAL_MACHINE, "SOFTWARE\\Fidelity Height LLC\\SEDUTIL LOG", "LOGGING", REG_SZ, "OFF");
			if (b) {
				std::cout << "registry_writeex OFF error" << std::endl;
				return 0;
			}
		}
		if (strcmp(argv[1], "read") == 0)
		{
			ls = m.registry_readex(HKEY_LOCAL_MACHINE, "SOFTWARE\\Fidelity Height LLC\\SEDUTIL LOG", "LOGGING", REG_SZ);
			if (ls) {
				std::cout << "registry_readex error" << std::endl;
				return 0;
			}
		}
		if (strcmp(argv[1], "create") == 0)
		{
			ls = m.registry_newex(HKEY_LOCAL_MACHINE, "SOFTWARE\\Fidelity Height LLC\\SEDUTIL LOG", "LOGGING", REG_SZ, "OFF");
			
			if (ls) {
				std::cout << "registry_newex error" << std::endl;
				return 0;
			}
		}
		if (strcmp(argv[1], "delete") == 0)
		{
			ls = m.registry_Delnode(HKEY_LOCAL_MACHINE, "SOFTWARE\\Wow6432Node\\Fidelity Height LLC\\SEDUTIL LOG");
			if (ls) {
				std::cout << "registry_delnode error" << std::endl;
				return 0;
			}
		}
		else {
			std::cout << "Unknown parameter" << std::endl;
			return 0;
		}

	}
	else if (argc == 3 )
	{
		if (strcmp(argv[2], psw) != 0) // 21 char
		{
			std::cout << "Please enter correct credential" << std::endl;
			return 950;
		}
		for (int i = 0; i < strlen(argv[1]); i++) {
			argv[1][i] = ::tolower(argv[1][i]);
		}
		if (strcmp(argv[1], "on") == 0)
		{
			b = m.registry_writeex(HKEY_LOCAL_MACHINE, "SOFTWARE\\Fidelity Height LLC\\SEDUTIL LOG", "LOGGING", REG_SZ, "ON");
			if (b) {
				std::cout << "registry_writeex ON error" << std::endl;
				return 0;
			}
		}
	}
	else {
		std::cout << "Please enter a correct argument" << std::endl;
		return 0;
	}

	return 0;
	
	if (m.isOS64()) // if OS is 64-bit, key is under Wow6432Node
		ls = RegOpenKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Wow6432Node\\Fidelity Height LLC\\SEDUTIL LOG", &key);
	else // if OS is 32-bit, key is under SOFTWARE
		ls = RegOpenKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Fidelity Height LLC\\SEDUTIL LOG", &key);
	if (ls) { // if ready key fail, create it
		//m.registry_newex(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Wow6432Node\\Fidelity Height LLC\\SEDUTIL LOG", L"LOGGING", REG_SZ, L"OFF");
		// create key is auto under Wow6432Node, no need to add Wow6432Node in subkey
		m.registry_newex(HKEY_LOCAL_MACHINE, "SOFTWARE\\Fidelity Height LLC\\SEDUTIL LOG", "LOGGING", REG_SZ, "OFF");
	}


	m.registry_writeex(HKEY_LOCAL_MACHINE, "SOFTWARE\\Fidelity Height LLC\\SEDUTIL LOG", "LOGGING", REG_SZ, "ON");
	
	m.registry_readex(HKEY_LOCAL_MACHINE, "SOFTWARE\\Fidelity Height LLC\\SEDUTIL LOG", "LOGGING", REG_SZ);
	if (m.get_logging())
		std::cout << "logging is TRUE" << std::endl;
	else
		std::cout << "logging is FALSE" << std::endl << std::endl << std::endl ;
	system("pause");

	m.registry_writeex(HKEY_LOCAL_MACHINE, "SOFTWARE\\Fidelity Height LLC\\SEDUTIL LOG", "LOGGING", REG_SZ, "OFF");
	m.registry_readex(HKEY_LOCAL_MACHINE, "SOFTWARE\\Fidelity Height LLC\\SEDUTIL LOG", "LOGGING", REG_SZ);
	if (m.get_logging())
		std::cout << "logging is TRUE" << std::endl;
	else
		std::cout << "logging is FALSE" << std::endl;

	// NO DELETE temperary
#if 0
	//bSuccess = RegDelnode(HKEY_CURRENT_USER, TEXT("Software\\TestDir"));
	//bSuccess = RegDelnode(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Wow6432Node\\Fidelity Height LLC\\SEDUTIL LOG\\LOGGING"));
	BOOL bSuccess;
	bSuccess = m.registry_Delnode(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Wow6432Node\\Fidelity Height LLC\\SEDUTIL LOG");
	if (bSuccess)
		printf("delete SOFTWARE\\Wow6432Node\\Fidelity Height LLC\\SEDUTIL LOG\\LOGGING Success!\n");
	else printf("delete SOFTWARE\\Wow6432Node\\Fidelity Height LLC\\SEDUTIL LOG\\LOGGING Failure.\n");
#endif
	// NG registry_deleteex(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Wow6432Node\\Fidelity Height LLC\\SEDUTIL LOG");
	
	return 0;


	m.registry_readex(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", "\\Device\\BthModem0", REG_SZ);
	//printf("*****1055 HARDWARE\\DEVICEMAP\\SERIALCOMM ***** \n");

	m.registry_readex(HKEY_LOCAL_MACHINE, "SOFTWARE\\Wow6432Node\\ADOBE\\Acrobat Reader\\DC\\Installer\\IOD", "Library", REG_SZ);
	//printf("*****106 SOFTWARE\\Wow6432Node\\ADOBE\\Acrobat Reader\\DC\\Installer\\IOD *****\n");


	return 0;




}

