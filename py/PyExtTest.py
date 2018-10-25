import PyExtLic
import PyExtOb
import platform

if platform.system() == 'Linux' :
    print "Linux does not have license implementation"

if platform.system() == 'Windows':
    print ("Get windows Valid License Level : ", PyExtLic.get_lic())

for L in [1, 2, 4, 16, 32, 64]:
    print PyExtOb.get_str(L)




