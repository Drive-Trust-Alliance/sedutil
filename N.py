import os
import sys
sys.path.insert(1, '..\py')
import subprocess
import time

if __name__ == "__main__":

    list = ['background.py', 'lockhash.py',   'runupdate.py', 'verify.py']
    listm = ['runprocess.py', 'runscan.py', 'background.py', 'runsetup.py', 'runthread.py', 'runprocess.py']
    list = list + listm
    print ('list = ', list)
    optmp = '--plugin-enable=multiprocessing'
   
    for f in list:
        print f 
        os.system('\Python27\Scripts\Nuitka.bat --module --recurse-all ' + f )
        print ("Nuitika complete compilation of ", f )

    list = ['OpalLock.py']
    for f in list:
        print f     
        #subprocess.call(['\Python27\Scripts\Nuitka.bat', '--standalone', optmp, f ])
        os.system('\Python27\Scripts\Nuitka.bat --standalone ' + optmp + ' ' + f)
        print ("Nuitika complete compilation of ", f )