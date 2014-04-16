// PBKDF2verify.cpp : Defines the entry point for the console application.
//

#include "targetver.h"
#include <iostream>
#include<iomanip>
#include "../cryptopp/pch.h"
#include "../cryptopp/stdcpp.h"
#include "../cryptopp/hmac.h"
#include "../cryptopp/pwdbased.h"
#include "../cryptopp/sha.h"
#include "../cryptopp/hex.h"
#include "../cryptopp/files.h"

using namespace std;
using namespace CryptoPP;

struct PBKDF_TestTuple
{
	byte purpose;
	unsigned int iterations;
	const char *hexPassword, *hexSalt, *hexDerivedKey;
};

bool TestPBKDF(PasswordBasedKeyDerivationFunction &pbkdf, const PBKDF_TestTuple *testSet, unsigned int testSetSize)
{
	bool pass = true;

	for (unsigned int i = 0; i<testSetSize; i++)
	{
		const PBKDF_TestTuple &tuple = testSet[i];

		string password, salt, derivedKey;
		StringSource(tuple.hexPassword, true, new HexDecoder(new StringSink(password)));
		StringSource(tuple.hexSalt, true, new HexDecoder(new StringSink(salt)));
		StringSource(tuple.hexDerivedKey, true, new HexDecoder(new StringSink(derivedKey)));

		SecByteBlock derived(derivedKey.size());
		pbkdf.DeriveKey(derived, derived.size(), tuple.purpose, (byte *)password.data(), password.size(), (byte *)salt.data(), salt.size(), tuple.iterations);
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
int main(int argc, char * argv[])
{
	bool pass = true;
	// from draft-ietf-smime-password-03.txt, at http://www.imc.org/draft-ietf-smime-password
	PBKDF_TestTuple testSet[] =
	{
		{ 0, 5, "70617373776f7264", "1234567878563412", "D1DAA78615F287E6" },
		{ 0, 500, "416C6C206E2D656E746974696573206D75737420636F6D6D756E69636174652077697468206F74686572206E2d656E74697469657320766961206E2D3120656E746974656568656568656573", "1234567878563412", "6A8970BF68C92CAEA84A8DF28510858607126380CC47AB2D" },
		// Draft PKCS #5 PBKDF2 Test Vectors http://tools.ietf.org/html/draft-josefsson-pbkdf2-test-vectors-06
		// "password" (8 octets) S = "salt" (4 octets)	c = 1 DK = 0c60c80f961f0e71f3a9b524af6012062fe037a6
		{ 0, 1, "70617373776F7264", "73616C74", "0c60c80f961f0e71f3a9b524af6012062fe037a6" },
		// "password" (8 octets) S = "salt" (4 octets)	c = 2 DK = ea6c014dc72d6f8ccd1ed92ace1d41f0d8de8957 
		{ 0, 2, "70617373776F7264", "73616C74", "ea6c014dc72d6f8ccd1ed92ace1d41f0d8de8957" },
		// "password" (8 octets) S = "salt" (4 octets)	c = 4096 DK = 4b007901b765489abead49d926f721d065a429c1
		{ 0, 4096, "70617373776F7264", "73616C74", "4b007901b765489abead49d926f721d065a429c1" },
		// "password" (8 octets) S = "salt" (4 octets)	c = 16777216 DK = eefe3d61cd4da4e4e9945b3d6ba2158c2634e984
		// { 0, 16777216, "70617373776F7264", "73616C74", "eefe3d61cd4da4e4e9945b3d6ba2158c2634e984" },
		// "passwordPASSWORDpassword" (24 octets) S = "saltSALTsaltSALTsaltSALTsaltSALTsalt" (36 octets) c = 4096 DK = 3d2eec4fe41c849b80c8d83662c0e44a8b291a964cf2f07038
		{ 0, 4096, "70617373776F726450415353574F524470617373776F7264", "73616C7453414C5473616C7453414C5473616C7453414C5473616C7453414C5473616C74",
		"3d2eec4fe41c849b80c8d83662c0e44a8b291a964cf2f07038" },
		// "pass\0word" (9 octets) S = "sa\0lt" (5 octets)	c = 4096 DK = 56fa6aa75548099dcc37d7f03425e0c3
		{ 0, 4096, "7061737300776F7264", "7361006C74", "56fa6aa75548099dcc37d7f03425e0c3" },
	};

	CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA1> pbkdf;

	cout << "\nPKCS #5 PBKDF2 validation suite running...\n\n";
	pass = TestPBKDF(pbkdf, testSet, sizeof(testSet) / sizeof(testSet[0])) && pass;
}

//#pragma warning(pop)
