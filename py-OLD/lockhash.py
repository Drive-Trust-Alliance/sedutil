import hashlib
import pbkdf2
import re

def convert(num):
    if num:
        return chr(num % 256) + convert(num // 256)
    else:
        return ''
        
def get_val():
    return convert(37549468393802268844761886790)

def hash_pbkdf2(plaintext, salt, iterations = 75000, bytes = 32):
    pwhash = pbkdf2.PBKDF2(plaintext, salt, iterations).hexread(bytes)
    return pwhash

def hash_pass(plaintext, salt, msid):
    if plaintext == msid: #don't hash MSID
        return plaintext
    #pw_trim = re.sub('\s', '', plaintext)
    #del plaintext
    pw = hash_pbkdf2(plaintext, salt)
    return pw
    
def hash_sig(plaintext, salt):
    sig = hash_pbkdf2(plaintext, salt, 1000)
    return sig

def testsedutil(testSet):
    failed = 0;
    out = ''
    for i in range(len(testSet)):
        tuple = testSet[i]
        actual = hash_pbkdf2(testSet[i][2], testSet[i][3], testSet[i][1], testSet[i][0])
        expected = testSet[i][4]
        out = out + 'Password ' + testSet[i][2] + ' Salt ' + testSet[i][3] + ' Iterations ' + str(testSet[i][1]) + ' Length ' + str(testSet[i][0]) + '\n'
        out = out + 'Expected Result: ' + expected + '\nActual Result  : ' + actual + '\n'
        if actual != expected:
            failed = 1
    res = [failed, out]
    return res

def testPBKDF2():
    testSet = [
        ( 20, 1, "password", "salt", "0c60c80f961f0e71f3a9b524af6012062fe037a6"),
        ( 20, 2, "password", "salt", "ea6c014dc72d6f8ccd1ed92ace1d41f0d8de8957"),
        ( 20, 4096, "password", "salt", "4b007901b765489abead49d926f721d065a429c1"),
        ( 25, 4096, "passwordPASSWORDpassword", "saltSALTsaltSALTsaltSALTsaltSALTsalt",
            "3d2eec4fe41c849b80c8d83662c0e44a8b291a964cf2f07038")]

    result = testsedutil(testSet)
    return result