import os

#PATHSIGN='sign-32bit\\'
def vfysig():
    pydlist = ['PyExtLic.pyd', 'PyExtOb.pyd', 'PyExtHash.pyd','sedutil-cli.exe','OpalLock.exe'  ,'QLMWizardFidelity\QlmLicenseWizard.exe']
    #print pydlist
    rr = 0 
    for f in pydlist: 
        #print f 
        #print ('signtool verify /pa ' + f + ' >NUL 2>NUL 1>NUL')
        r = os.system('sign-32bit\signtool verify /pa ' + f + ' >NUL 2>NUL 1>NUL')
        #r = os.system('sign-32bit\signtool verify /pa ' + f )
        #print r
        if r == 0 :
            print (f + ' signature authenticate OK')
        else :
            print (f + ' signature authenticate NG')
            rr = rr + r
    return rr
    
    
if vfysig() == 0:
    print 'All signature authenicate successfully'
else :
    print 'One of the signature authenticate incorrectly'


            
    
    
