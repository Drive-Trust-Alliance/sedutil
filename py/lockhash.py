import hashlib
import pbkdf2
import re

def convert(num):
    if num:
        return chr(num % 256) + convert(num // 256)
    else:
        return ''
        
def get_val(input):
    if input == 0:
        return convert(37549468393802268844761886790)
    else:
        return [convert(134778767330889818343107709416847468870),
            convert(133516696771073529497692148323911493958),
            convert(42702100953497539105770872562006649158),
            convert(42702102616360496875857926820255263046),
            convert(42702426738463857721441676939831306566),
            convert(145433241952439177690100336922345105734)]

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