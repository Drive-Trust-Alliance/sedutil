
import os
import re
import sys


'''
Existing Power Schemes (* Active)
-----------------------------------
power scheme
Power Scheme GUID: 381b4222-f694-41f0-9685-ff5bb260df2e  (Balanced) *
Power Scheme GUID: 8c5e7fda-e8bf-4a96-9a85-a6e23a8c635c  (High performance)
Power Scheme GUID: a1841308-3541-4fab-bc81-f71556f20b4a  (Power saver)
Power Setting GUID: a7066653-8d6c-40a8-910e-a1f54b84c7e5  (Start menu power button)
Subgroup GUID: 4f971e89-eebd-4455-a8de-9e59040e7347  (Power buttons and lid)
    GUID Alias: SUB_BUTTONS
Power Setting GUID: 5ca83367-6e45-459f-a27b-476b1d01c936  (Lid close action)
Power Setting GUID: 96996bc0-ad50-47ec-923b-6f41874dd9eb  (Sleep button action)
Power Setting GUID: 7648efa3-dd9c-4e3e-b566-50f929386280  (Power button action)
'''
    
def get_scheme():
    scheme = ''
    Bal = "381b4222-f694-41f0-9685-ff5bb260df2e"
    High = "8c5e7fda-e8bf-4a96-9a85-a6e23a8c635c"
    Saver = "a1841308-3541-4fab-bc81-f71556f20b4a"
    
    s = os.popen('powercfg -GETACTIVESCHEME').read()
    
    s1 = re.search(Bal, s)
    s2 = re.search(High, s)
    s3 = re.search(Saver, s)

    if s1:
        scheme = s1.group()
    if s2:
        scheme = s2.group()
    if s3:
        scheme = s3.group()
        
    return scheme
    
def verify_power():
    Bal = "381b4222-f694-41f0-9685-ff5bb260df2e"
    High = "8c5e7fda-e8bf-4a96-9a85-a6e23a8c635c"
    Saver = "a1841308-3541-4fab-bc81-f71556f20b4a"
    Lid = "5ca83367-6e45-459f-a27b-476b1d01c936"
    Slp = "96996bc0-ad50-47ec-923b-6f41874dd9eb"
    Pwr = "7648efa3-dd9c-4e3e-b566-50f929386280"
    Subgp = "4f971e89-eebd-4455-a8de-9e59040e7347"
    
    scheme = get_scheme()
    
    regex_Lid = 'Power Setting GUID:\s*' + Lid + '\s*\(Lid close action\)(?:\n.+){9}\n\s*Current AC Power Setting Index: 0x00000002\n\s*Current DC Power Setting Index: 0x00000002'
    regex_Slp = 'Power Setting GUID:\s*' + Slp + '\s*\(Sleep button action\)(?:\n.+){11}\n\s*Current AC Power Setting Index: 0x00000002\n\s*Current DC Power Setting Index: 0x00000002'
    regex_Pwr = 'Power Setting GUID:\s*' + Pwr + '\s*\(Power button action\)(?:\n.+){11}\n\s*Current AC Power Setting Index: 0x00000002\n\s*Current DC Power Setting Index: 0x00000002'
    
    s = os.popen('powercfg -QUERY ' + scheme + ' ' + Subgp).read()
    
    s1 = re.search(regex_Lid, s)
    s2 = re.search(regex_Slp, s)
    s3 = re.search(regex_Pwr, s)
    return ((s1 != None) & (s2 != None) & (s3 != None))
    
    
def set_power():
    Lid = "5ca83367-6e45-459f-a27b-476b1d01c936"
    Slp = "96996bc0-ad50-47ec-923b-6f41874dd9eb"
    Pwr = "7648efa3-dd9c-4e3e-b566-50f929386280"
    Subgp = "4f971e89-eebd-4455-a8de-9e59040e7347"
    
    scheme = get_scheme()
	
    s = os.popen("powercfg -hibernate on")
    
    s = os.popen("powercfg -SETACVALUEINDEX " + scheme + " " + Subgp + " " + Lid +  " 2").read() 
    s = os.popen("powercfg -SETACVALUEINDEX " + scheme + " " + Subgp + " " + Pwr +  " 2").read() 
    s = os.popen("powercfg -SETACVALUEINDEX " + scheme + " " + Subgp + " " + Slp +  " 2").read() 

    s = os.popen("powercfg -SETDCVALUEINDEX " + scheme + " " + Subgp + " " + Lid +  " 2").read() 
    s = os.popen("powercfg -SETDCVALUEINDEX " + scheme + " " + Subgp + " " + Pwr +  " 2").read() 
    s = os.popen("powercfg -SETDCVALUEINDEX " + scheme + " " + Subgp + " " + Slp +  " 2").read()

    s1 = os.popen("powercfg -x standby-timeout-ac 10")
    s2 = os.popen("powercfg -x standby-timeout-dc 10")
    s3 = os.popen("powercfg -x hibernate-timeout-ac 15")
    s4 = os.popen("powercfg -x hibernate-timeout-dc 15")