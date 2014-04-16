/* C:B**************************************************************************
This software is Copyright 2014 Michael Romeo <r0m30@r0m30.com>

This file is part of msed.

msed is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

msed is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with msed.  If not, see <http://www.gnu.org/licenses/>.

* C:E********************************************************************** */
// set diagnostics to /W3 for /W4 messages not handled in CryptoPP
#ifdef _MSC_VER
#pragma warning(push,3)
#endif
#include "os.h"
#include "MsedHashPwd.h"
#include "../cryptopp/pch.h"
#include "../cryptopp/stdcpp.h"
#include "../cryptopp/hmac.h"
#include "../cryptopp/pwdbased.h"
#include "../cryptopp/sha.h"

using namespace std;
using namespace CryptoPP;

void MsedHashPwd(vector<uint8_t> &hash, char * password, vector<uint8_t> salt,
	unsigned int iter, uint8_t hashsize)
{
	// if the hashsize can be > 255 the token overhead logic needs to be fixed
	assert(1 == sizeof(hashsize)); 
	hash.clear();
	hash.reserve(hashsize + 2); // hope this will prevent reallocation
	//  hash is <hashsize> bytes
	for (uint16_t i = 0; i < hashsize; i++) {
		hash.push_back(' ');
	}
	
	PKCS5_PBKDF2_HMAC<SHA1> pbkdf2;
	pbkdf2.DeriveKey(hash.data(), hash.size(), 0, (byte *)password, strnlen(password, 256),
		salt.data(), salt.size(), iter);
// add the token overhead
	hash.insert(hash.begin(), hash.size());
	hash.insert(hash.begin(), 0xd0);
}
// weirdness with c4505 in misc won't allow a pop ?????
//#ifdef _MSC_VER
//#pragma warning(pop)
//#endif


