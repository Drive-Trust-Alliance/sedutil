'''
Fidelity Height LLC Copyright 2020 

Optane finder 

'''
import os, re, platform
'''
C:\Users\Optane>Intelmas show -intelssd \\.\physicaldrive0

- Intel SSD Optane_0000 -

DevicePath : \\.\PHYSICALDRIVE0
DeviceStatus : Unknown
Firmware : 17.2
FirmwareUpdateAvailable : The selected drive contains current firmware as of this tool release.
Index : 0
ModelNumber : Optane+932GBHDD
ProductFamily : Intel SSD
SerialNumber : Optane_0000


Intelmas
'Intelmas' is not recognized as an internal or external command,
operable program or batch file.


'''

# return 'Optane' if find intel Optane drive. otherwise '' 
def findoptane() :
    devname="\\\\.\\PhysicalDrive"
    DEV_OS = platform.system()
    if DEV_OS == 'Windows':
        e_list = [] 
        txt = os.popen('Intelmas show -intelssd \\\\.\\PhysicalDrive0' ).read()
        txt_regex = 'is not recognized as an internal or external command, able program or batch file'
        d_list = re.findall(txt_regex,txt)
        if d_list != '' or d_list != [] : # Intel Optane tool is not installed
            print('e_list=', e_list)
            print('Intel Optane tool is installed, continue')
        else :
            print('Intel Optane tool is not installed quit')
            return e_list

        list_optane = [] 
        for d in range (26) :
            txt = os.popen('Intelmas show -intelssd \\\\.\\PhysicalDrive' + str(d) ).read()
            #print(txt)
            #txt1 = txt
            #txt2=txt1.split(':',2)
            #for i in txt2 :
            #    print(i)
             
            txt_regex = 'ModelNumber : Optane[+][0-9]*GBHDD'
            model = re.findall(txt_regex,txt)
            ##print(model)
            
            txt_regex = 'SerialNumber : Optane_[0-9]*'
            serial = re.findall(txt_regex,txt)
            ##print(serial)
            
            txt_regex = 'Optane[+][0-9]*GBHDD'
            d_list = re.findall(txt_regex,txt)
            ##print(d_list)
            
            
            if d_list != [] : 
                print("dlist=",d_list)
                print('Detect ' + devname + str(d) + ' is intel Opatane Drive. Warn user ')
                list_optane.append(str(d)) 
            #else :
                #print("d_list is empty, No Intel Optane memory with HDD ")
                #print('Detect ' + devname + str(d) + ' is NOT intel Opatane Drive. Warn user ')
        print('list_optane=', list_optane)        
        return list_optane
        
        
        
        
r = findoptane()
if r == [] :
    print("No Optane drive founded")
else :
    print("Found Optane drive")
    