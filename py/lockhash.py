import hashlib
import pbkdf2
import re


def hash_pbkdf2(plaintext, salt, iterations = 75000):
    pw = pbkdf2.crypt(plaintext, salt, iterations)
    hash_object = hashlib.sha256(pw)
    pwhash = hash_object.hexdigest()
    return pwhash

def hash_pass(plaintext, salt, msid):
    if plaintext == msid: #don't hash MSID
        return plaintext
    pw_trim = re.sub('\s', '', plaintext)
    #pw = pbkdf2.crypt(pw_trim, salt, 75000)
    pw = hash_pbkdf2(pw_trim, salt)
    return pw

def testsedutil(testSet):
    passed = 1;

    for i in range(len(testSet)):
        tuple = testSet[i]
		#DtaHashPassword(hash, (char *) tuple.Password, seaSalt, tuple.iterations, tuple.hashlen)
        actual = hash_pbkdf2(testSet[i][1], testSet[i][2], testSet[i][0])
        expected = testSet[i][3]
        if actual != expected:
            passed = 0

    return passed

def testPBKDF2():
    passed = 1
    testSet = [
        ( 1, "password", "salt", "1f3115d3b96dd72804bbe03f3a7baf907f7ffef86a412b9f64d2a5cbf3c4681c"),
        ( 2, "password", "salt", "53cb94baa9f6838790359dda3a94153b09cae24503bb375f44251baaebd7758b"),
        ( 4096, "password", "salt", "5afcc968768516540db66e6241294945fd6c3a4e6920ed7be36e7bfd8d4ae760"),
        ( 4096, "passwordPASSWORDpassword", "saltSALTsaltSALTsaltSALTsaltSALTsalt",
            "9e9899425fd7a80053feb1f06057fc3444ba286d892e9187d4ca02c3ec596871")]

    #print "\nPKCS #5 PBKDF2 validation suite running ... \n\n"
    passed = testsedutil(testSet) and passed
    #print "\nPKCS #5 PBKDF2 validation suite ... "
    if (passed):
        print "passed\n"
        return 0
    else:
        print "**FAILED**\n"
        return 1