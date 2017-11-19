/* C:B**************************************************************************
This software is Copyright 2014-2017 Bright Plaza Inc. <drivetrust@drivetrust.com>

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
#include "DtaOptions.h"
#include "DtaDev.h"
#include "log.h"

extern "C" {
#include "pbkdf2.h"
#include "sha1.h"
}
using namespace std;

void DtaHashPassword(vector<uint8_t> &hash, const  vector<uint8_t>& password,
    const vector<uint8_t>& salt, unsigned int iter, uint8_t hashsize)
{
	LOG(D1) << " Entered DtaHashPassword";
	// if the hashsize can be > 255 the token overhead logic needs to be fixed
	assert(1 == sizeof(hashsize));
	if (253 < hashsize) { LOG(E) << "Hashsize > 253 incorrect token generated"; }
	
	hash.clear();
	// don't hash the devault OPAL password ''
	if (0 == password.size()) {
		goto exit;
	}
	hash.reserve(hashsize + 2); // hope this will prevent reallocation
	for (uint16_t i = 0; i < hashsize; i++) {
		hash.push_back(' ');
	}
	
	cf_pbkdf2_hmac(&password[0], password.size(),
		salt.data(), salt.size(),
		iter,
		hash.data(), hash.size(),
		&cf_sha1);

//	gc_pbkdf2_sha1(password, strnlen(password, 256), (const char *)salt.data(), salt.size(), iter,
//		(char *)hash.data(), hash.size());
exit:	// add the token overhead
	hash.insert(hash.begin(), (uint8_t)hash.size());
	hash.insert(hash.begin(), 0xd0);
}

void DtaHashPwd(vector<uint8_t> &hash, char * password, DtaDev * d)
{
    LOG(D1) << " Entered DtaHashPwd";
    char *serNum;
    vector<uint8_t> decoded_password;
    if (d->hex_passwords)
    {
        for (char* p=password; *p; ++p)
        {
            uint8_t num1 = (uint8_t)(*p & 0x40 ? (*p & 0xf) + 9 : *p & 0xf);
            ++p;
            if (*p == 0)
                break;
            uint8_t num2 = (uint8_t)(*p & 0x40 ? (*p & 0xf) + 9 : *p & 0xf);
            decoded_password.push_back(num1 * 16 + num2);
        }
    }
    else
    {
        decoded_password.assign(password, password + strlen(password));
    }

    if (d->no_hash_passwords) {
        if (decoded_password.size() > 32)
            decoded_password.resize(32);
        hash = decoded_password;
        // add the token overhead
        hash.insert(hash.begin(), (uint8_t)hash.size());
        hash.insert(hash.begin(), 0xd0);
        LOG(D1) << " Exit DtaHashPwd";
        return;
    }
    serNum = d->getSerialNum();
    vector<uint8_t> salt(serNum, serNum + 20);
    //	vector<uint8_t> salt(DEFAULTSALT);
    DtaHashPassword(hash, decoded_password, salt);
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
    std::vector<uint8_t> hash, seaSalt, password;

    for (unsigned int i = 0; i < testSetSize; i++) {
        const PBKDF_TestTuple &tuple = testSet[i];
        hash.clear();
        seaSalt.clear();
        for (uint16_t j = 0; j < strnlen(tuple.Salt, 255); j++) {
            seaSalt.push_back(tuple.Salt[j]);
        }
		printf("Password %s Salt %s Iterations %i Length %i\n", (char *)tuple.Password,
			(char *) tuple.Salt, tuple.iterations, tuple.hashlen);
        password.assign(tuple.Password, tuple.Password+strlen(tuple.Password));
		DtaHashPassword(hash, password, seaSalt, tuple.iterations, tuple.hashlen);
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

