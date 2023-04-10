#!/opt/local/bin/python
#
# Name : PyExtTestHash.py
# Fidelity Height LLC Copyright 2018 - 2019
# Author : Jerry Hwang
# Date : 2/13/2019
# Update History :
# 3/26/2019 print hash result and expected result. 

# command line 
# python -c "import PyExtHash; print dir();  print PyExtHash.hashpwd(\"password123\",\"salt\",70000,32)"
# 
# Please replace salt with drive's series number in 20-byte (blank fill to 20-byte)
#

import PyExtHash
import time
print (dir())
print ("***** Start PBKDF2 test *****")
print ("actual : ", PyExtHash.hashpwd("password","salt",1,20))
print ("expected:    0c60c80f961f0e71f3a9b524af6012062fe037a6")
print ("***** end of PBKDF2 test ******")


print ('Before call PyExtHash ', time.strftime("%H:%M:%S"))
print ('actual   : ', PyExtHash.hashpwd("password123","1847E1D8EFCB        ",75000,32))
p = PyExtHash.hashpwd("password123","S3YHNX0K421690J     ",75000,32)
print ('actual   : ', p ) # PyExtHash.hashpwd("password123","S3YHNX0K421690J     ",75000,32))
print ('expected : 6C6E26918F7E899A0F6E0B6D2D81CF2BB27A0BFEB577CF57A9C09F6EC1749B7D')

print ('After call PyExtHash ', time.strftime("%H:%M:%S"))
# ser number 20-byte "1847E1D8EFCB        "
# windowsTestResult = '''
# C:\tmp18D2\util\windows\CLI\Release>python test.py
# ['PyExtHash', '__builtins__', '__doc__', '__file__', '__name__', '__package__', 'time']
# Before call PyExtHash  11:25:13
# password123 1847E1D8EFCB         75000 32
# - 11:25:13.000 DBG1:  Entered DtaHashPassword
# - 11:25:13.156 DBG1:  Exited DtaHashPassword
# ea25afea31ca0477279e274f4fe362fcfc4334d1eddea3d1885b350516281f3c
# After call PyExtHash  11:25:13
# '''

linuxTestResult = '''
[centospny@localhost hash]$ python PyExtTestHash.py 
['PyExtHash', '__builtins__', '__doc__', '__file__', '__name__', '__package__', 'time']
***** Start PBKDF2 test *****
password salt 1 20 4 4
- 15:00:22.984 DBG1:  Entered DtaHashPassword
- 15:00:22.985 DBG1:  Exited DtaHashPassword
('actual : ', '0c60c80f961f0e71f3a9b524af6012062fe037a6')
expected:    0c60c80f961f0e71f3a9b524af6012062fe037a6
***** end of PBKDF2 test ******
Before call PyExtHash  15:00:22
password123 1847E1D8EFCB         75000 32 20 20
- 15:00:22.985 DBG1:  Entered DtaHashPassword
- 15:00:23.703 DBG1:  Exited DtaHashPassword
('actual   : ', 'ea25afea31ca0477279e274f4fe362fcfc4334d1eddea3d1885b350516281f3c')
password123 S3YHNX0K421690J      75000 32 20 20
- 15:00:23.703 DBG1:  Entered DtaHashPassword
- 15:00:24.422 DBG1:  Exited DtaHashPassword
('actual   : ', '6c6e26918f7e899a0f6e0b6d2d81cf2bb27a0bfeb577cf57a9c09f6ec1749b7d')
expected : 6C6E26918F7E899A0F6E0B6D2D81CF2BB27A0BFEB577CF57A9C09F6EC1749B7D
After call PyExtHash  15:00:24
***** Start PBKDF2 test *****
password salt 1 20 4 4
- 15:00:24.422 DBG1:  Entered DtaHashPassword
- 15:00:24.422 DBG1:  Exited DtaHashPassword
('actual : ', '0c60c80f961f0e71f3a9b524af6012062fe037a6')
expected:    0c60c80f961f0e71f3a9b524af6012062fe037a6
password salt 2 20 4 4
- 15:00:24.422 DBG1:  Entered DtaHashPassword
- 15:00:24.422 DBG1:  Exited DtaHashPassword
('actual : ', 'ea6c014dc72d6f8ccd1ed92ace1d41f0d8de8957')
expected:    ea6c014dc72d6f8ccd1ed92ace1d41f0d8de8957
password salt 4096 20 4 4
- 15:00:24.423 DBG1:  Entered DtaHashPassword
- 15:00:24.443 DBG1:  Exited DtaHashPassword
('actual : ', '4b007901b765489abead49d926f721d065a429c1')
expected:    4b007901b765489abead49d926f721d065a429c1
passwordPASSWORDpassword saltSALTsaltSALTsaltSALTsaltSALTsalt 4096 25 36 36
- 15:00:24.443 DBG1:  Entered DtaHashPassword
- 15:00:24.484 DBG1:  Exited DtaHashPassword
('actual : ', '3d2eec4fe41c849b80c8d83662c0e44a8b291a964cf2f07038')
expected:    3d2eec4fe41c849b80c8d83662c0e44a8b291a964cf2f07038
[centospny@localhost hash]$ 

'''


macosTestResult = '''
password salt 1 20 4 4
- 15:43:05.349 DBG1:  Entered DtaHashPassword
- 15:43:05.349 DBG1:  Exited DtaHashPassword
actual :  b'0c60c80f961f0e71f3a9b524af6012062fe037a6'
expected:    0c60c80f961f0e71f3a9b524af6012062fe037a6
***** end of PBKDF2 test ******
Before call PyExtHash  15:43:05
password123 1847E1D8EFCB         75000 32 20 20
- 15:43:05.349 DBG1:  Entered DtaHashPassword
- 15:43:05.444 DBG1:  Exited DtaHashPassword
actual   :  b'ea25afea31ca0477279e274f4fe362fcfc4334d1eddea3d1885b350516281f3c'
password123 S3YHNX0K421690J      75000 32 20 20
- 15:43:05.444 DBG1:  Entered DtaHashPassword
- 15:43:05.502 DBG1:  Exited DtaHashPassword
actual   :  b'6c6e26918f7e899a0f6e0b6d2d81cf2bb27a0bfeb577cf57a9c09f6ec1749b7d'
expected : 6C6E26918F7E899A0F6E0B6D2D81CF2BB27A0BFEB577CF57A9C09F6EC1749B7D
After call PyExtHash  15:43:05
***** Start PBKDF2 test *****
password salt 1 20 4 4
- 15:43:05.502 DBG1:  Entered DtaHashPassword
- 15:43:05.503 DBG1:  Exited DtaHashPassword
actual :  b'0c60c80f961f0e71f3a9b524af6012062fe037a6'
expected:    0c60c80f961f0e71f3a9b524af6012062fe037a6
password salt 2 20 4 4
- 15:43:05.503 DBG1:  Entered DtaHashPassword
- 15:43:05.503 DBG1:  Exited DtaHashPassword
actual :  b'ea6c014dc72d6f8ccd1ed92ace1d41f0d8de8957'
expected:    ea6c014dc72d6f8ccd1ed92ace1d41f0d8de8957
password salt 4096 20 4 4
- 15:43:05.503 DBG1:  Entered DtaHashPassword
- 15:43:05.504 DBG1:  Exited DtaHashPassword
actual :  b'4b007901b765489abead49d926f721d065a429c1'
expected:    4b007901b765489abead49d926f721d065a429c1
passwordPASSWORDpassword saltSALTsaltSALTsaltSALTsaltSALTsalt 4096 25 36 36
- 15:43:05.504 DBG1:  Entered DtaHashPassword
- 15:43:05.507 DBG1:  Exited DtaHashPassword
actual :  b'3d2eec4fe41c849b80c8d83662c0e44a8b291a964cf2f07038'
expected:    3d2eec4fe41c849b80c8d83662c0e44a8b291a964cf2f07038
'''

print ("***** Start PBKDF2 test *****")
print ("actual : ", PyExtHash.hashpwd("password","salt",1,20))
print ("expected:    0c60c80f961f0e71f3a9b524af6012062fe037a6")
print ("actual : ", PyExtHash.hashpwd("password","salt",2,20))
print ("expected:    ea6c014dc72d6f8ccd1ed92ace1d41f0d8de8957")
print ("actual : ", PyExtHash.hashpwd("password","salt",4096,20))
print ("expected:    4b007901b765489abead49d926f721d065a429c1")
print ("actual : ", PyExtHash.hashpwd("passwordPASSWORDpassword", "saltSALTsaltSALTsaltSALTsaltSALTsalt",4096,25))
print ("expected:    3d2eec4fe41c849b80c8d83662c0e44a8b291a964cf2f07038")




