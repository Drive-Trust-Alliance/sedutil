'''
Author : jerry hwang
date : 6/27/2019
owner : Fidelity Height LLC
Description : 
    When python 2.x present in the system and no window Pro to support 'group policy editor'
    which turn on power shell script to allow VS to run version script. 
    this python script will delete Version.h, run 'git describe' to generate 'Version.h'
'''
import os

v = os.popen("git describe").read()
v = v.strip()

os.remove("Version.h")
f = open("Version.h" ,"a")
L = ['#define GIT_VERSION "' , v , '"']
s = ''.join(L)
print ("s = ", s)
f.write(s)
f.close()



