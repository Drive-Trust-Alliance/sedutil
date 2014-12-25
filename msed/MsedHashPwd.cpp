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
#include <iostream>
#include <iomanip>
#include "MsedHashPwd.h"
#include "MsedLexicon.h"
#include "MsedBaseDev.h"
#include "log.h"
#include "../cryptopp/pch.h"
#include "../cryptopp/stdcpp.h"
#include "../cryptopp/hmac.h"
#include "../cryptopp/pwdbased.h"
#include "../cryptopp/sha.h"
#include "../cryptopp/hex.h"
#include "../cryptopp/files.h"

using namespace std;
using namespace CryptoPP;

void MsedHashPwd(vector<uint8_t> &hash, char * password, MsedBaseDev * d)
{
    LOG(D1) << " Entered MsedHashPwd";
    uint8_t serNum[20];
    d->getSerialNum(serNum);
    vector<uint8_t> salt(serNum, serNum + 20);
    //	vector<uint8_t> salt(DEFAULTSALT);
    MsedHashPassword(hash, password, salt);
    LOG(D1) << " Exit MsedHashPwd"; // log for hash timing

}

void MsedHashPassword(vector<uint8_t> &hash, char * password, vector<uint8_t> salt,
                      unsigned int iter, uint8_t hashsize)
{
    LOG(D1) << " Entered MsedHashPassword";
    // if the hashsize can be > 255 the token overhead logic needs to be fixed
    assert(1 == sizeof (hashsize));
    if (253 < hashsize) LOG(E) << "Hashsize > 253 incorrect token generated";

    hash.clear();
    hash.reserve(hashsize + 2); // hope this will prevent reallocation
    for (uint16_t i = 0; i < hashsize; i++) {
        hash.push_back(' ');
    }

    PKCS5_PBKDF2_HMAC<SHA1> pbkdf2;
    pbkdf2.DeriveKey(hash.data(), hash.size(), 0, (byte *) password, strnlen(password, 256),
                     salt.data(), salt.size(), iter);
    // add the token overhead
    hash.insert(hash.begin(), (uint8_t)hash.size());
    hash.insert(hash.begin(), 0xd0);
}
// weirdness with c4505 in misc won't allow a pop ?????
//#ifdef _MSC_VER
//#pragma warning(pop)
//#endif

struct PBKDF_TestTuple
{
    uint8_t purpose;
    unsigned int iterations;
    const char *hexPassword, *hexSalt, *hexDerivedKey;
};

bool TestMsed(const PBKDF_TestTuple *testSet, unsigned int testSetSize)
{
    bool pass = true;
    std::vector<uint8_t> hash, seaSalt;

    for (unsigned int i = 0; i < testSetSize; i++) {
        const PBKDF_TestTuple &tuple = testSet[i];

        string derivedKey;
        StringSource(tuple.hexDerivedKey, true, new HexDecoder(new StringSink(derivedKey)));

        SecByteBlock derived(derivedKey.size());
        hash.clear();
        seaSalt.clear();
        for (uint16_t i = 0; i < strnlen(tuple.hexSalt, 255); i++) {
            seaSalt.push_back(tuple.hexSalt[i]);
        }
        MsedHashPassword(hash, (char *) tuple.hexPassword, seaSalt,
                         tuple.iterations, (uint8_t)derivedKey.size());
        bool fail = memcmp(hash.data() + 2, derivedKey.data(), derived.size()) != 0;
        pass = pass && !fail;

        HexEncoder enc(new FileSink(cout));
        cout << (fail ? "FAILED   " : "passed   ");
        enc.Put(tuple.purpose);
        cout << " " << tuple.iterations;
        cout << " " << tuple.hexPassword << " " << tuple.hexSalt << " ";
        enc.Put(hash.data() + 2, hash.size() - 2, true);
        cout << endl;
    }

    return pass;
}

bool TestPBKDF(PasswordBasedKeyDerivationFunction &pbkdf, const PBKDF_TestTuple *testSet, unsigned int testSetSize)
{
    bool pass = true;

    for (unsigned int i = 0; i < testSetSize; i++) {
        const PBKDF_TestTuple &tuple = testSet[i];

        string password, salt, derivedKey;
        StringSource(tuple.hexPassword, true, new HexDecoder(new StringSink(password)));
        StringSource(tuple.hexSalt, true, new HexDecoder(new StringSink(salt)));
        StringSource(tuple.hexDerivedKey, true, new HexDecoder(new StringSink(derivedKey)));

        SecByteBlock derived(derivedKey.size());
        pbkdf.DeriveKey(derived, derived.size(), tuple.purpose, (byte *) password.data(), password.size(), (byte *) salt.data(), salt.size(), tuple.iterations);
        bool fail = memcmp(derived, derivedKey.data(), derived.size()) != 0;
        pass = pass && !fail;

        HexEncoder enc(new FileSink(cout));
        cout << (fail ? "FAILED   " : "passed   ");
        enc.Put(tuple.purpose);
        cout << " " << tuple.iterations;
        cout << " " << tuple.hexPassword << " " << tuple.hexSalt << " ";
        enc.Put(derived, derived.size());
        cout << endl;
    }

    return pass;
}

int MsedTestPBDKF2()
{
    bool pass = true;
    // from draft-ietf-smime-password-03.txt, at http://www.imc.org/draft-ietf-smime-password
    {
        PBKDF_TestTuple testSet[] = {
            { 0, 5, "70617373776f7264", "1234567878563412", "D1DAA78615F287E6"},
            { 0, 500, "416C6C206E2D656E746974696573206D75737420636F6D6D756E69636174652077697468206F74686572206E2d656E74697469657320766961206E2D3120656E746974656568656568656573", "1234567878563412", "6A8970BF68C92CAEA84A8DF28510858607126380CC47AB2D"},
            // Draft PKCS #5 PBKDF2 Test Vectors http://tools.ietf.org/html/draft-josefsson-pbkdf2-test-vectors-06
            // "password" (8 octets) S = "salt" (4 octets)	c = 1 DK = 0c60c80f961f0e71f3a9b524af6012062fe037a6
            { 0, 1, "70617373776F7264", "73616C74", "0c60c80f961f0e71f3a9b524af6012062fe037a6"},
            // "password" (8 octets) S = "salt" (4 octets)	c = 2 DK = ea6c014dc72d6f8ccd1ed92ace1d41f0d8de8957
            { 0, 2, "70617373776F7264", "73616C74", "ea6c014dc72d6f8ccd1ed92ace1d41f0d8de8957"},
            // "password" (8 octets) S = "salt" (4 octets)	c = 4096 DK = 4b007901b765489abead49d926f721d065a429c1
            { 0, 4096, "70617373776F7264", "73616C74", "4b007901b765489abead49d926f721d065a429c1"},
            // "password" (8 octets) S = "salt" (4 octets)	c = 16777216 DK = eefe3d61cd4da4e4e9945b3d6ba2158c2634e984
            // { 0, 16777216, "70617373776F7264", "73616C74", "eefe3d61cd4da4e4e9945b3d6ba2158c2634e984" },
            // "passwordPASSWORDpassword" (24 octets) S = "saltSALTsaltSALTsaltSALTsaltSALTsalt" (36 octets) c = 4096 DK = 3d2eec4fe41c849b80c8d83662c0e44a8b291a964cf2f07038
            { 0, 4096, "70617373776F726450415353574F524470617373776F7264", "73616C7453414C5473616C7453414C5473616C7453414C5473616C7453414C5473616C74",
                "3d2eec4fe41c849b80c8d83662c0e44a8b291a964cf2f07038"},
            // "pass\0word" (9 octets) S = "sa\0lt" (5 octets)	c = 4096 DK = 56fa6aa75548099dcc37d7f03425e0c3
            { 0, 4096, "7061737300776F7264", "7361006C74", "56fa6aa75548099dcc37d7f03425e0c3"},
        };

        PKCS5_PBKDF2_HMAC<SHA1> pbkdf;

        cout << "\nPKCS #5 PBKDF2 validation suite running on Template ... \n\n";
        pass = TestPBKDF(pbkdf, testSet, sizeof (testSet) / sizeof (testSet[0])) && pass;
        cout << "\nPKCS #5 PBKDF2 validation suite on Template ... ";
        if (pass)
            cout << "passed\n";
        else
            cout << "**FAILED**\n";

    }
    {
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
    }
    return 0;
}

