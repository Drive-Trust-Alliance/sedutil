import hashlib
import pbkdf2
import re


def hash_pbkdf2(plaintext, salt, iterations = 75000, bytes = 32):
    pwhash = pbkdf2.PBKDF2(plaintext, salt, iterations).hexread(bytes)
    return pwhash

def hash_pass(plaintext, salt, msid):
    if plaintext == msid: #don't hash MSID
        return plaintext
    pw_trim = re.sub('\s', '', plaintext)
    pw = hash_pbkdf2(pw_trim, salt)
    return 'd020' + pw

def testsedutil(testSet):
    passed = 1;

    for i in range(len(testSet)):
        tuple = testSet[i]
        actual = hash_pbkdf2(testSet[i][2], testSet[i][3], testSet[i][1], testSet[i][0])
        expected = testSet[i][4]
        if actual != expected:
            passed = 0

    return passed

def testPBKDF2():
    passed = 1
    testSet = [
        ( 20, 1, "password", "salt", "0c60c80f961f0e71f3a9b524af6012062fe037a6"),
        ( 20, 2, "password", "salt", "ea6c014dc72d6f8ccd1ed92ace1d41f0d8de8957"),
        ( 20, 4096, "password", "salt", "4b007901b765489abead49d926f721d065a429c1"),
        ( 25, 4096, "passwordPASSWORDpassword", "saltSALTsaltSALTsaltSALTsaltSALTsalt",
            "3d2eec4fe41c849b80c8d83662c0e44a8b291a964cf2f07038")]

    #print "\nPKCS #5 PBKDF2 validation suite running ... \n\n"
    passed = testsedutil(testSet) and passed
    #print "\nPKCS #5 PBKDF2 validation suite ... "
    if (passed):
        print "passed\n"
        return 0
    else:
        print "**FAILED**\n"
        return 1