
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
    
def set_power(ui):
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
    
    m_idx = ui.screen_menu.get_active()
    h_idx = ui.hib_menu.get_active()
    
    if m_idx == 0:
        m_val = '1'
    elif m_idx == 1:
        m_val = '2'
    elif m_idx == 2:
        m_val = '3'
    elif m_idx == 3:
        m_val = '5'
    elif m_idx == 4:
        m_val = '10'
    elif m_idx == 5:
        m_val = '15'
    elif m_idx == 6:
        m_val = '20'
    elif m_idx == 7:
        m_val = '25'
    elif m_idx == 8:
        m_val = '30'
    elif m_idx == 9:
        m_val = '45'
    elif m_idx == 10:
        m_val = '60'
    elif m_idx == 11:
        m_val = '120'
    elif m_idx == 12:
        m_val = '180'
    elif m_idx == 13:
        m_val = '240'
    elif m_idx == 14:
        m_val = '300'
    if h_idx == 0:
        h_val = '5'
    elif h_idx == 1:
        h_val = '10'
    elif h_idx == 2:
        h_val = '15'
    elif h_idx == 3:
        h_val = '20'
    elif h_idx == 4:
        h_val = '25'
    elif h_idx == 5:
        h_val = '30'
    elif h_idx == 6:
        h_val = '45'
    elif h_idx == 7:
        h_val = '60'
    elif h_idx == 8:
        h_val = '120'
    elif h_idx == 9:
        h_val = '180'
    elif h_idx == 10:
        h_val = '240'
    elif h_idx == 11:
        h_val = '300'
    
    s = os.popen('powercfg -x monitor-timeout-ac ' + m_val)
    s = os.popen('powercfg -x monitor-timeout-dc ' + m_val)

    s = os.popen("powercfg -x standby-timeout-ac 0")
    s = os.popen("powercfg -x standby-timeout-dc 0")
    
    s = os.popen("powercfg -x hibernate-timeout-ac " + h_val)
    s = os.popen("powercfg -x hibernate-timeout-dc " + h_val)
    
def get_power():
    scheme = get_scheme()
    sub_sleep = '238c9fa8-0aad-41ed-83f4-97be242c8f20'
    sub_video = '7516b95f-f776-4464-8c53-06167f40cc99'
    
    s_screen = os.popen('powercfg -QUERY ' + scheme + ' ' + sub_video).read()
    #print s_screen
    s_hibern = os.popen('powercfg -QUERY ' + scheme + ' ' + sub_sleep).read()
    #print s_hibern
    
    screen_regex = 'Power Setting GUID: 3c0bc021-c8a8-4e07-a973-6b14cbcb2b7e\s*\(Turn off display after\).*\n.+\n.+\n.+\n.+\n.+\n\s+Current AC Power Setting Index: (0x\S+)'
    hibern_regex = 'Power Setting GUID: 9d7815a6-7ee4-497e-8888-515a05f02364\s*\(Hibernate after\).*\n.+\n.+\n.+\n.+\n.+\n\s+Current AC Power Setting Index: (0x\S+)'
    
    m_screen = re.search(screen_regex, s_screen)
    m_hibern = re.search(hibern_regex, s_hibern)
    
    hex_screen = m_screen.group(1)
    hex_hibern = m_hibern.group(1)
    
    dec_screen = int(hex_screen, 0)/60
    dec_hibern = int(hex_hibern, 0)/60
    
    str_screen = str(dec_screen)
    str_hibern = str(dec_hibern)
    
    screen_idx = -1
    hibern_idx = -1
    
    if str_screen == '1':
        screen_idx = 0
    elif str_screen == '2':
        screen_idx = 1
    elif str_screen == '3':
        screen_idx = 2
    elif str_screen == '5':
        screen_idx = 3
    elif str_screen == '10':
        screen_idx = 4
    elif str_screen == '15':
        screen_idx = 5
    elif str_screen == '20':
        screen_idx = 6
    elif str_screen == '25':
        screen_idx = 7
    elif str_screen == '30':
        screen_idx = 8
    elif str_screen == '45':
        screen_idx = 9
    elif str_screen == '60':
        screen_idx = 10
    elif str_screen == '120':
        screen_idx = 11
    elif str_screen == '180':
        screen_idx = 12
    elif str_screen == '240':
        screen_idx = 13
    elif str_screen == '300':
        screen_idx = 14
    
    if str_hibern == '5':
        hibern_idx = 0
    elif str_hibern == '10':
        hibern_idx = 1
    elif str_hibern == '15':
        hibern_idx = 2
    elif str_hibern == '20':
        hibern_idx = 3
    elif str_hibern == '25':
        hibern_idx = 4
    elif str_hibern == '30':
        hibern_idx = 5
    elif str_hibern == '45':
        hibern_idx = 6
    elif str_hibern == '60':
        hibern_idx = 7
    elif str_hibern == '120':
        hibern_idx = 8
    elif str_hibern == '180':
        hibern_idx = 9
    elif str_hibern == '240':
        hibern_idx = 10
    elif str_hibern == '300':
        hibern_idx = 11
        
    return (screen_idx, hibern_idx)