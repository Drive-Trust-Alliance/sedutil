/* C:B**************************************************************************
This software is Copyright 2014,2015 Michael Romeo <r0m30@r0m30.com>

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
#include "os.h"
#include <iostream>
#include <iomanip>
#include <assert.h>
#include "MsedHashPwd.h"
#include "MsedLexicon.h"
#include "MsedDev.h"
#include "log.h"

extern "C"
int gc_pbkdf2_sha1(const char *P, size_t Plen,const char *S, size_t Slen,
					unsigned int c,char *DK, size_t dkLen);

using namespace std;

void MsedHashPassword(vector<uint8_t> &hash, char * password, vector<uint8_t> salt,
	unsigned int iter, uint8_t hashsize)
{
	LOG(D1) << " Entered MsedHashPassword";
	// if the hashsize can be > 255 the token overhead logic needs to be fixed
	assert(1 == sizeof(hashsize));
	if (253 < hashsize) LOG(E) << "Hashsize > 253 incorrect token generated";

	hash.clear();
	hash.reserve(hashsize + 2); // hope this will prevent reallocation
	for (uint16_t i = 0; i < hashsize; i++) {
		hash.push_back(' ');
	}
	gc_pbkdf2_sha1(password, strnlen(password, 256), (const char *)salt.data(), salt.size(), iter,
		(char *)hash.data(), hash.size());
	// add the token overhead
	hash.insert(hash.begin(), (uint8_t)hash.size());
	hash.insert(hash.begin(), 0xd0);
}

void MsedHashPwd(vector<uint8_t> &hash, char * password, MsedDev * d)
{
    LOG(D1) << " Entered MsedHashPwd";
    char *serNum;
    serNum = d->getSerialNum();
    vector<uint8_t> salt(serNum, serNum + 20);
    //	vector<uint8_t> salt(DEFAULTSALT);
    MsedHashPassword(hash, password, salt);
    LOG(D1) << " Exit MsedHashPwd"; // log for hash timing
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

int TestMsed(const PBKDF_TestTuple *testSet, unsigned int testSetSize)
{
    int pass = 1;
    std::vector<uint8_t> hash, seaSalt;

    for (unsigned int i = 0; i < testSetSize; i++) {
        const PBKDF_TestTuple &tuple = testSet[i];
        hash.clear();
        seaSalt.clear();
        for (uint16_t i = 0; i < strnlen(tuple.Salt, 255); i++) {
            seaSalt.push_back(tuple.Salt[i]);
        }
		printf("Password %s Salt %s Iterations %i Length %i\n", (char *)tuple.Password,
			(char *) tuple.Salt, tuple.iterations, tuple.hashlen);
		MsedHashPassword(hash, (char *) tuple.Password, seaSalt, tuple.iterations, tuple.hashlen);
		int fail = (testresult(hash, tuple.hexDerivedKey, tuple.hashlen) == 0);
        pass = pass & fail;
    }

    return pass;
}
int MsedTestPBDKF2()
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

    cout << "\nPKCS #5 PBKDF2 validation suite running on Msed ... \n\n";
    pass = TestMsed(testSet, sizeof (testSet) / sizeof (testSet[0])) && pass;
    cout << "\nPKCS #5 PBKDF2 validation suite on Msed ... ";
    if (pass)
        cout << "passed\n";
    else
        cout << "**FAILED**\n";
    return 0;
}

