/* C:B**************************************************************************
This software is Copyright 2014, 2022 Bright Plaza Inc. <drivetrust@drivetrust.com>

This file is part of sedutil.

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


#include "os.h"
#include <iostream>
#include <iomanip>
#include "DtaHashPassword.h"
#include "DtaLexicon.h"
#include "DtaOptions.h"
#include "DtaDev.h"
#include "log.h"

extern "C" {
#include "pbkdf2.h"
#include "sha1.h"
}


#include <stdio.h>


void DtaHashPassword(vector<uint8_t> &hash, char * password, vector<uint8_t> salt,
	unsigned int iter, uint8_t hashsize) {
	LOG(D1) << " Entered DtaHashPassword";
	// if the hashsize can be > 255 the token overhead logic needs to be fixed
	assert(1 == sizeof(hashsize));
	if (253 < hashsize) { LOG(E) << "Hashsize > 253 incorrect token generated"; }
	
	hash.clear();
	// don't hash the default OPAL password ''
	if (0 == strnlen(password, 32)) {
		goto exit;
	}
	hash.reserve(hashsize + 2); // hope this will prevent reallocation
	for (uint8_t i = 0; i < hashsize; i++) {
		hash.push_back(' ');
	}
#if 1 // defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__)
	cf_pbkdf2_hmac((uint8_t *)password, strnlen(password, 256),
		salt.data(), salt.size(),
		iter,
		hash.data(), hash.size(),
		&cf_sha1);
#endif
//	gc_pbkdf2_sha1(password, strnlen(password, 256), (const char *)salt.data(), salt.size(), iter,
//		(char *)hash.data(), hash.size());
exit:	// add the token overhead
	hash.insert(hash.begin(), (uint8_t)hash.size());
	hash.insert(hash.begin(), 0xd0);
	LOG(D1) << " Exited DtaHashPassword";
}

// hashing for logging ON OFF command

    void DtaHashPasswordLogging(vector<uint8_t> &hash, char * password, vector<uint8_t> salt,
        unsigned int iter, uint8_t hashsize) {

	hash.clear();
	hash.reserve(hashsize); // hope this will prevent reallocation
	for (uint8_t i = 0; i < hashsize; i++) {
		hash.push_back(' ');
	}
#if 1 // defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__)
	cf_pbkdf2_hmac((uint8_t *)password, strnlen(password, 256),
		salt.data(), salt.size(),
		iter,
		hash.data(), hash.size(),
		&cf_sha1);
#endif
	//	gc_pbkdf2_sha1(password, strnlen(password, 256), (const char *)salt.data(), salt.size(), iter,
	//		(char *)hash.data(), hash.size());
	//exit:	// add the token overhead
#if 0
	hash.insert(hash.begin(), (uint8_t)hash.size());
	hash.insert(hash.begin(), 0xd0);
#endif
	//LOG(D1) << " Exited DtaHashPassword";
}
