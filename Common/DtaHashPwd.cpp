/* C:B**************************************************************************
This software is Copyright 2014-2016 Bright Plaza Inc. <drivetrust@drivetrust.com>

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
#include "DtaHashPwd.h"
#include "DtaLexicon.h"
#include "DtaDev.h"
#include "log.h"

#if 1 // defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__)
extern "C" {
#include "pbkdf2.h"
#include "sha1.h"
}
#endif

using namespace std;

void DtaHashPassword(vector<uint8_t> &hash, char * password, vector<uint8_t> salt,
	unsigned int iter, uint8_t hashsize)
{
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
}

// credit
// https://www.codeproject.com/articles/99547/hex-strings-to-raw-data-and-back
//

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

vector<uint8_t> hex2data(char * password)
{
	vector<uint8_t> h;
	h.clear();
	if (false)
		printf("strlen(password)=%d\n", (int)strlen(password));
	if (strlen(password) != 64)
	{
		LOG(E) << "Hashed Password length isn't 64-byte, no translation";
		h.clear();
		for (uint16_t i = 0; i < (uint16_t)strnlen(password, 32); i++)
			h.push_back(password[i]);
		return h;
	}

	//printf("GUI hashed password=");
	for (uint16_t i=0; i < (uint16_t)strlen(password); i+=2)
	{
		h.push_back(
		(hex_digit_to_nybble(password[i]) << 4) |  // high 4-bit
			(hex_digit_to_nybble(password[i + 1]) & 0x0f)); // lo 4-bit
	}
	//for (uint16_t i = 0; i < (uint16_t)h.size(); i++)
	//	printf("%02x", h[i]);
	//printf("\n");
	return h;
}


void DtaHashPwd(vector<uint8_t> &hash, char * password, DtaDev * d, unsigned int iter)
{
    LOG(D1) << " Entered DtaHashPwd";
    char *serNum;

	//d->no_hash_passwords = true; // force no hashing for debug purpose
	IFLOG(D4) printf("d->translate_req = %d\n", d->translate_req); 
	if (d->no_hash_passwords) {
		if (d->translate_req) { // host-hashed password, convert 64-byte ascii into 32-byte data ???????
			hash = hex2data(password); 
		}
		else {
			hash.clear();
			for (uint16_t i = 0; i < strnlen(password, 32); i++)
				hash.push_back(password[i]);
		}
		// add the token overhead
		hash.insert(hash.begin(), (uint8_t)hash.size());
		hash.insert(hash.begin(), 0xd0);
		LOG(D1) << " Exit DtaHashPwd";
		return;
    }
    serNum = d->getSerialNum();
    vector<uint8_t> salt(serNum, serNum + 20);
    //	vector<uint8_t> salt(DEFAULTSALT);
	if (iter==75000)
		DtaHashPassword(hash, password, salt);
	else
		DtaHashPassword(hash, password, salt,iter);
#if false
	printf("serNum=%s\n", serNum);
	printf("serNum as data =");
	for (int i = 0; i < strnlen(serNum,20); i++) printf("%02X", serNum[i]);
	printf("\n");
	printf("salt size = %d ; salt =",salt.size());
	for (int i = 0; i < salt.size(); i++) printf("%02X", salt[i]);
	printf("\n");
	printf("salt as string =%s\n", salt.data());
	//printf("password : %s\n",password); // non-printable char cause screen error 
	printf("Hashed password size = %d ; hashed password =",hash.size());
	for (int i = 0; i < hash.size(); i++)
		printf("%02x", hash[i]);
	printf("\n");
#endif	
    LOG(D1) << " Exit DtaHashPwd"; // log for hash timing
}

struct PBKDF_TestTuple
{
    uint8_t hashlen;
    unsigned int iterations;
    const char *Password, *Salt, *hexDerivedKey;
};

int testresult(std::vector<uint8_t> &result, const char * expected, size_t len) {
	char work[50];
	if (len > 50) return 1;
	int p = 0;
	printf("Expected Result: %s\nActual Result  : ", expected);
	for (uint32_t i = 0; i < len; i++) { printf("%02x", result[i + 2]); }; printf("\n");
	for (uint32_t i = 0; i < len * 2; i += 2) {
		work[p] = expected[i] & 0x40 ? 16 * ((expected[i] & 0xf) + 9) : 16 * (expected[i] & 0xf);
		work[p] += expected[i + 1] & 0x40 ? (expected[i + 1] & 0xf) + 9 : expected[i + 1] & 0xf;
		p++;
	}
	return memcmp(result.data()+2, work, len);
}

int Testsedutil(const PBKDF_TestTuple *testSet, unsigned int testSetSize)
{
    int pass = 1;
    std::vector<uint8_t> hash, seaSalt;

    for (unsigned int i = 0; i < testSetSize; i++) {
        const PBKDF_TestTuple &tuple = testSet[i];
        hash.clear();
        seaSalt.clear();
        for (uint16_t j = 0; j < strnlen(tuple.Salt, 255); j++) {
            seaSalt.push_back(tuple.Salt[j]);
        }
		printf("Password %s Salt %s Iterations %i Length %i\n", (char *)tuple.Password,
			(char *) tuple.Salt, tuple.iterations, tuple.hashlen);
		DtaHashPassword(hash, (char *) tuple.Password, seaSalt, tuple.iterations, tuple.hashlen);
		int fail = (testresult(hash, tuple.hexDerivedKey, tuple.hashlen) == 0);
        pass = pass & fail;
    }

    return pass;
}
int TestPBKDF2()
{
    int pass = 1;
    // from draft-ietf-smime-password-03.txt, at http://www.imc.org/draft-ietf-smime-password
    PBKDF_TestTuple testSet[] = {
        // Draft PKCS #5 PBKDF2 Test Vectors http://tools.ietf.org/html/draft-josefsson-pbkdf2-test-vectors-06
        // "password" (8 octets) S = "salt" (4 octets)	c = 1 DK = 0c60c80f961f0e71f3a9b524af6012062fe037a6
        { 20, 1, "password", "salt", "0c60c80f961f0e71f3a9b524af6012062fe037a6"},
        // "password" (8 octets) S = "salt" (4 octets)	c = 2 DK = ea6c014dc72d6f8ccd1ed92ace1d41f0d8de8957
        { 20, 2, "password", "salt", "ea6c014dc72d6f8ccd1ed92ace1d41f0d8de8957"},
        // "password" (8 octets) S = "salt" (4 octets)	c = 4096 DK = 4b007901b765489abead49d926f721d065a429c1
        { 20, 4096, "password", "salt", "4b007901b765489abead49d926f721d065a429c1"},
        // "password" (8 octets) S = "salt" (4 octets)	c = 16777216 DK = eefe3d61cd4da4e4e9945b3d6ba2158c2634e984
        //{ 20, 16777216, "password", "salt", "eefe3d61cd4da4e4e9945b3d6ba2158c2634e984" },
        // "passwordPASSWORDpassword" (24 octets) S = "saltSALTsaltSALTsaltSALTsaltSALTsalt" (36 octets) c = 4096 DK = 3d2eec4fe41c849b80c8d83662c0e44a8b291a964cf2f07038
        { 25, 4096, "passwordPASSWORDpassword", "saltSALTsaltSALTsaltSALTsaltSALTsalt",
            "3d2eec4fe41c849b80c8d83662c0e44a8b291a964cf2f07038"},
        // "pass\0word" (9 octets) S = "sa\0lt" (5 octets)	c = 4096 DK = 56fa6aa75548099dcc37d7f03425e0c3
        //{ 16, 4096, "pass\0word", "sa\0lt", "56fa6aa75548099dcc37d7f03425e0c3" },
        // program receives char * from OS so this test would fail but is not possible IRL
    };

    cout << "\nPKCS #5 PBKDF2 validation suite running ... \n\n";
    pass = Testsedutil(testSet, sizeof (testSet) / sizeof (testSet[0])) && pass;
    cout << "\nPKCS #5 PBKDF2 validation suite ... ";
    if (pass)
        cout << "passed\n";
    else
        cout << "**FAILED**\n";
    return 0;
}

