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

#include "pyexthash.h"
#ifdef PYEXTHASH
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include "..\License\include.h"
#else
// python.h is included in includ.h and must be include python.h first, it is python bug
#include "../License/include.h" 
#endif
#endif

#include "os.h"
#include <iostream>
#include <iomanip>
#include "pyexthash.h"
#ifndef PYEXTHASH
#include "DtaHashPwd.h"
#endif
#include "DtaLexicon.h"
#include "DtaOptions.h"
#include "DtaDev.h"
#include "log.h"

#if 1 // defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__)
extern "C" {
#include "pbkdf2.h"
#include "sha1.h"
}
#endif


#include <stdio.h>


#ifdef PYEXTHASH
#include "hash.h"

hashpwd::hashpwd()
{
}

hashpwd::~hashpwd()
{
}

//vector<uint8_t> 
void hashpwd::DtaHashPassword(vector<uint8_t> &hash, char * password, vector<uint8_t> salt,
	unsigned int iter, uint8_t hashsize) {
#else
void DtaHashPassword(vector<uint8_t> &hash, char * password, vector<uint8_t> salt,
	unsigned int iter, uint8_t hashsize) {
#endif
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
	case 'a':
	case 'A': return 0xa;
	case 'b':
	case 'B': return 0xb;
	case 'c':
	case 'C': return 0xc;
	case 'd':
	case 'D': return 0xd;
	case 'e':
	case 'E': return 0xe;
	case 'f':
	case 'F': return 0xf;
	default: return 0xff;  // throw std::invalid_argument();
	}
}

vector<uint8_t> hex2data(char * password)
{
	vector<uint8_t> h;
	h.clear();
	if ((false))
		printf("strlen(password)=%d\n", (int)strlen(password));
	if (strlen(password) != 64)
	{
		//LOG(D) << "Hashed Password length isn't 64-byte, no translation";
		h.clear();
		for (uint16_t i = 0; i < (uint16_t)strnlen(password, 32); i++)
			h.push_back(password[i]);
		return h;
	}

	//printf("GUI hashed password=");
	for (uint16_t i=0; i < (uint16_t)strlen(password); i+=2)
	{
		h.push_back((uint8_t)(((hex_digit_to_nybble(password[i]) << 4) & 0xf0)
                             |((hex_digit_to_nybble(password[i + 1]) ) & 0x0f)));
	}
	//for (uint16_t i = 0; i < (uint16_t)h.size(); i++)
	//	fprintf(Output2FILE::Stream(), "%02x", h[i]);
	//fprintf(Output2FILE::Stream(), "\n");
	return h;
}

// hex data to asci Upper case 
inline unsigned char hex_nibble_to_ascii(uint8_t ch)
{
	switch (ch)
	{
	case 0: return '0';
	case 1: return '1';
	case 2: return '2';
	case 3: return '3';
	case 4: return '4';
	case 5: return '5';
	case 6: return '6';
	case 7: return '7';
	case 8: return '8';
	case 9: return '9';
	case 0xA: return 'A';
	case 0xB: return 'B';
	case 0xC: return 'C';
	case 0xD: return 'D';
	case 0xE: return 'E';
	case 0xF: return 'F';
	default: return '?';  // throw std::invalid_argument();
	}
}

void data2ascii(vector<uint8_t> &h , vector<uint8_t> &password)
{
	// 32-byte hash hex to 64 byte ascii 
	for (uint16_t i = 0; i < h.size(); i += 1)
	{
		password.push_back(hex_nibble_to_ascii((h[i] >> 4) & 0x0f));
		password.push_back(hex_nibble_to_ascii(h[i] & 0x0f));
		//password[i * 2] = hex_nibble_to_ascii((h[i]>>4) & 0x0f); // high nibble
		//password[i * 2 + 1] = hex_nibble_to_ascii(h[i] & 0x0f); // low nibble
	}
#if 0
	printf("32-byte hex to 64-byte ascii : ");
	for (uint16_t i = 0; i < password.size(); i++)
        fprintf(Output2FILE::Stream(), "%02X", password[i]);
    fprintf(Output2FILE::Stream(), "\n");
#endif

}
#ifndef PYEXTHASH
//void DtaHashPwd(vector<uint8_t> &hash, char * password, DtaDev * d, unsigned int iter){}
void DtaHashPwd(vector<uint8_t> &hash, char * password, DtaDev * d, unsigned int iter)
{

    LOG(D1) << " Entered DtaHashPwd";

	//d->no_hash_passwords = true; // force no hashing for debug purpose
	IFLOG(D4) fprintf(Output2FILE::Stream(), "d->translate_req = %d\n", d->translate_req);
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

#define USE_PASSWORD_SALT
#if defined( USE_PASSWORD_SALT )
    vector<uint8_t> salt(d->getPasswordSalt());
#else // ! defined( USE_PASSWORD_SALT )
    char serialNum[21];
    bzero(serialNum, sizeof(serialNum));
    strncpy(serialNum, d->getSerialNum(),sizeof(serialNum));
    vector<uint8_t> salt(serialNum,serialNum+sizeof(serialNum)-1);
#endif // defined( USE_PASSWORD_SALT )

    //	vector<uint8_t> salt(DEFAULTSALT);
	if (iter == 75000) {
		DtaHashPassword(hash, password, salt);
	} else {
		DtaHashPassword(hash, password, salt, iter);
	}

    // non-printable char cause screen error
//    IFLOG(D4) fprintf(Output2FILE::Stream(), "password as string =%s", password);
//    IFLOG(D4) fprintf(Output2FILE::Stream(), "\n");
    IFLOG(D4) fprintf(Output2FILE::Stream(), "password:\n");
    for (size_t i = 0; i < strlen(password); i++) IFLOG(D4) fprintf(Output2FILE::Stream(), "%02X", password[i]);
    IFLOG(D4) fprintf(Output2FILE::Stream(), "\n");
    IFLOG(D4) fprintf(Output2FILE::Stream(), "salt:\n");
	for (size_t i = 0; i < salt.size(); i++) IFLOG(D4) fprintf(Output2FILE::Stream(), "%02X", salt[i]);
	IFLOG(D4) fprintf(Output2FILE::Stream(), "\n");
	IFLOG(D4) fprintf(Output2FILE::Stream(), "salt as string =%s", salt.data());
    IFLOG(D4) fprintf(Output2FILE::Stream(), "\n");
    
    IFLOG(D4) fprintf(Output2FILE::Stream(), "Hashed password size = %lu",hash.size());
    IFLOG(D4) fprintf(Output2FILE::Stream(), "\n");
    IFLOG(D4) fprintf(Output2FILE::Stream(), "hashed password:\n");
	for (size_t i = 0; i < hash.size(); i++) IFLOG(D4) fprintf(Output2FILE::Stream(), "%02X", hash[i]);
	IFLOG(D4) fprintf(Output2FILE::Stream(), "\n");

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
#endif

#ifdef PYEXTHASH
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#pragma warning(disable: 4996)
#endif

PyObject* hash_function1(PyObject* self, PyObject* args)
{
	vector<uint8_t>  hashstr; // returned hash
	char * password;
	char * saltstr;
	vector<uint8_t> salt;
	uint32_t iter;
	uint8_t sz;
	uint8_t hp[128]; // converted ascii from hashstr
	hashpwd hh;

	memset(hp, 0, 128);
	//void hashpwd::DtaHashPassword(vector<uint8_t> &hash, char * password, vector<uint8_t> salt,
	//unsigned int iter, uint8_t hashsize)
	if (!PyArg_ParseTuple(args, "ssib", &password, &saltstr,&iter, &sz)) // str:password,str:salt,int(iteration,short i:hash size
	{
		//goto error; // why use go to 
		return 0;
	};
	salt.clear();
	for (int jj = 0; jj < strnlen(saltstr, 255); jj++) salt.push_back(saltstr[jj]);
	printf("%s %s %d %d %d %d\n",password, saltstr,iter, sz, strnlen(saltstr, 255), salt.size());
 	hh.DtaHashPassword(hashstr, password, salt, iter, sz);
	
	for (int ii = 0; ii < (int)(hashstr.size() - 2); ii += 1) { // first 2 byte of hash vector is header
		snprintf((char *)hp + (ii * 2), 4, "%02x", hashstr.at(ii + 2));
	}
	return PyString_FromStringAndSize((char *)hp,sz*2);
}


PyMethodDef hashMethods[] =
{
	{ "hashpwd",(PyCFunction)hash_function1,METH_VARARGS,0 },
	{ 0,0,0,0 }  // leave it empty for now
};


PyMODINIT_FUNC
initPyExtHash(void)
{
	PyObject *m;

	m = Py_InitModule("PyExtHash", hashMethods); // array of exported function 
	if (m == NULL)
		return;
}
#endif
