import ctypes
import os
import platform
import re
if platform.system() == 'Windows':
    import subprocess
    import _winreg


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
    guid = '[a-f0-9]{8}-[a-f0-9]{4}-[a-f0-9]{4}-[a-f0-9]{4}-[a-f0-9]{12}'
    
    s = os.popen('powercfg -GETACTIVESCHEME').read()
    
    s1 = re.search(guid, s)

    if s1:
        scheme = s1.group()
        
    return scheme
    
def verify_power():
    Lid = "5ca83367-6e45-459f-a27b-476b1d01c936"
    Slp = "96996bc0-ad50-47ec-923b-6f41874dd9eb"
    Pwr = "7648efa3-dd9c-4e3e-b566-50f929386280"
    Subgp = "4f971e89-eebd-4455-a8de-9e59040e7347"
    
    SlpAfter = '29f6c1db-86da-48c5-9fdb-f2b67b1f44da'
    Slpgp = '238c9fa8-0aad-41ed-83f4-97be242c8f20'
    
    scheme = get_scheme()
    
    regex_Lid = 'Power Setting GUID:\s*' + Lid + '\s*\(Lid close action\)(?:\n.+){9,11}\n\s*Current AC Power Setting Index: 0x0000000[^1]\n\s*Current DC Power Setting Index: 0x0000000[^1]'
    regex_Slp = 'Power Setting GUID:\s*' + Slp + '\s*\(Sleep button action\)(?:\n.+){9,11}\n\s*Current AC Power Setting Index: 0x0000000[^1]\n\s*Current DC Power Setting Index: 0x0000000[^1]'
    regex_Pwr = 'Power Setting GUID:\s*' + Pwr + '\s*\(Power button action\)(?:\n.+){9,11}\n\s*Current AC Power Setting Index: 0x0000000[^1]\n\s*Current DC Power Setting Index: 0x0000000[^1]'
    
    regex_SlpAfter = 'Power Setting GUID:\s*' + SlpAfter + '\s*\(Sleep after\)(?:\n.+){5}\n\s*Current AC Power Setting Index: 0x00000000\n\s*Current DC Power Setting Index: 0x00000000'
    
    s = os.popen('powercfg -QUERY ' + scheme + ' ' + Subgp).read()
    t = os.popen('powercfg -QUERY ' + scheme + ' ' + Slpgp).read()
    
    s1 = re.search(regex_Lid, s)
    s2 = re.search(regex_Slp, s)
    s3 = re.search(regex_Pwr, s)
    
    t1 = re.search(regex_SlpAfter, t)
    
    reg_path = r'SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\FlyoutMenuSettings'
    key = None
    
    sleep_val = None
    hibernate_val = None
    
    is64bit = ctypes.c_bool()
    handle = ctypes.windll.kernel32.GetCurrentProcess()
    success = ctypes.windll.kernel32.IsWow64Process(handle, ctypes.byref(is64bit))
    if (success and is64bit).value:
        try:
            key = _winreg.OpenKey(_winreg.HKEY_LOCAL_MACHINE, reg_path, 0, _winreg.KEY_READ | _winreg.KEY_WOW64_64KEY)
        except WindowsError:
            pass
    else:
        try:
            key = _winreg.OpenKey(_winreg.HKEY_LOCAL_MACHINE, reg_path, 0, _winreg.KEY_READ)
        except:
            pass
    
    if key != None:
        sleep_val, _ = _winreg.QueryValueEx(key, 'ShowSleepOption')
        hibernate_val, _ = _winreg.QueryValueEx(key, 'ShowHibernateOption')
        _winreg.CloseKey(key)
    
    return ((s1 != None) & (s2 != None) & (s3 != None) & (t1 != None) & (sleep_val == 0) & (hibernate_val == 1))
    
def set_power(ui):
    Lid = "5ca83367-6e45-459f-a27b-476b1d01c936"
    Slp = "96996bc0-ad50-47ec-923b-6f41874dd9eb"
    Pwr = "7648efa3-dd9c-4e3e-b566-50f929386280"
    Subgp = "4f971e89-eebd-4455-a8de-9e59040e7347"
    
    scheme = get_scheme()
	
    s = os.popen("powercfg -hibernate on")
    
    p_idx = ui.pb_menu.get_active()
    s_idx = ui.sb_menu.get_active()
    l_idx = ui.lid_menu.get_active()
    
    if p_idx >= 1:
        p_idx = p_idx + 1
    if s_idx >= 1: 
        s_idx = s_idx + 1
    if l_idx >= 1:
        l_idx = l_idx + 1
    
    s = os.popen("powercfg -SETACVALUEINDEX " + scheme + " " + Subgp + " " + Lid +  " " + str(l_idx))
    s = os.popen("powercfg -SETACVALUEINDEX " + scheme + " " + Subgp + " " + Pwr +  " " + str(p_idx)) 
    s = os.popen("powercfg -SETACVALUEINDEX " + scheme + " " + Subgp + " " + Slp +  " " + str(s_idx))

    s = os.popen("powercfg -SETDCVALUEINDEX " + scheme + " " + Subgp + " " + Lid +  " " + str(l_idx))
    s = os.popen("powercfg -SETDCVALUEINDEX " + scheme + " " + Subgp + " " + Pwr +  " " + str(p_idx))
    s = os.popen("powercfg -SETDCVALUEINDEX " + scheme + " " + Subgp + " " + Slp +  " " + str(s_idx))
    
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
    elif m_idx == 15:
        m_val = '0'
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
    elif h_idx == 12:
        h_val = '0'
    
    s01 = -1
    s02 = -1
    s11 = -1
    s12 = -1
    s21 = -1
    s22 = -1
    
    with open(os.devnull, 'w') as pipe:
        s01 = subprocess.call(['powercfg', '-x', 'monitor-timeout-ac', m_val], stdout=pipe)#stderr=log)
        s02 = subprocess.call(['powercfg', '-x', 'monitor-timeout-dc', m_val], stdout=pipe)#stderr=log)

        s11 = subprocess.call(['powercfg', '-x', 'standby-timeout-ac', '0'], stdout=pipe)#stderr=log)
        s12 = subprocess.call(['powercfg', '-x', 'standby-timeout-dc', '0'], stdout=pipe)#stderr=log)
        
        s21 = subprocess.call(['powercfg', '-x', 'hibernate-timeout-ac', h_val], stdout=pipe)#stderr=log)
        s22 = subprocess.call(['powercfg', '-x', 'hibernate-timeout-dc', h_val], stdout=pipe)#stderr=log)
    
    reg_path = r'SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\FlyoutMenuSettings'
    key = None
    
    is64bit = ctypes.c_bool()
    handle = ctypes.windll.kernel32.GetCurrentProcess()
    success = ctypes.windll.kernel32.IsWow64Process(handle, ctypes.byref(is64bit))
    if (success and is64bit).value:
        try:
            key = _winreg.CreateKeyEx(_winreg.HKEY_LOCAL_MACHINE, reg_path, 0, _winreg.KEY_WRITE | _winreg.KEY_WOW64_64KEY)
        except WindowsError:
            pass
    else:
        try:
            key = _winreg.CreateKeyEx(_winreg.HKEY_LOCAL_MACHINE, reg_path, 0, _winreg.KEY_WRITE)
        except:
            pass
    
    if key != None:
        _winreg.SetValueEx(key, 'ShowSleepOption', 0, _winreg.REG_DWORD, 0)
        _winreg.SetValueEx(key, 'ShowHibernateOption', 0, _winreg.REG_DWORD, 1)
        _winreg.CloseKey(key)
    
    return s01 | s02 | s11 | s12 | s21 | s22
    
def get_power():
    scheme = get_scheme()
    sub_sleep = '238c9fa8-0aad-41ed-83f4-97be242c8f20'
    sub_video = '7516b95f-f776-4464-8c53-06167f40cc99'
    sub_pbl = '4f971e89-eebd-4455-a8de-9e59040e7347'
    
    s_screen = os.popen('powercfg -QUERY ' + scheme + ' ' + sub_video).read()
    ##print s_screen
    s_hibern = os.popen('powercfg -QUERY ' + scheme + ' ' + sub_sleep).read()
    ##print s_hibern
    s_pbl = os.popen('powercfg -QUERY ' + scheme + ' ' + sub_pbl).read()
    
    screen_regex = 'Power Setting GUID: 3c0bc021-c8a8-4e07-a973-6b14cbcb2b7e\s*\(Turn off display after\).*\n.+\n.+\n.+\n.+\n.+\n\s+Current AC Power Setting Index: (0x\S+)'
    hibern_regex = 'Power Setting GUID: 9d7815a6-7ee4-497e-8888-515a05f02364\s*\(Hibernate after\).*\n.+\n.+\n.+\n.+\n.+\n\s+Current AC Power Setting Index: (0x\S+)'
    power_regex = 'Power Setting GUID: 7648efa3-dd9c-4e3e-b566-50f929386280\s*\(Power button action\).*\n.+\n.+\n.+\n.+\n.+\n.+\n.+\n.+\n.+\n.+\n.+\n\s+Current AC Power Setting Index: 0x0000000([0-4])'
    sleep_regex = 'Power Setting GUID: 96996bc0-ad50-47ec-923b-6f41874dd9eb\s*\(Sleep button action\).*\n.+\n.+\n.+\n.+\n.+\n.+\n.+\n.+\n.+\n.+\n.+\n\s+Current AC Power Setting Index: 0x0000000([0-4])'
    lid_regex = 'Power Setting GUID: 5ca83367-6e45-459f-a27b-476b1d01c936\s*\(Lid close action\).*\n.+\n.+\n.+\n.+\n.+\n.+\n.+\n.+\n.+\n\s+Current AC Power Setting Index: 0x0000000([0-3])'
    
    m_screen = re.search(screen_regex, s_screen)
    m_hibern = re.search(hibern_regex, s_hibern)
    
    m_power = re.search(power_regex, s_pbl)
    m_sleep = re.search(sleep_regex, s_pbl)
    m_lid = re.search(lid_regex, s_pbl)
    
    power_idx = -1
    sleep_idx = -1
    lid_idx = -1
    
    if m_power:
        power_idx = int(m_power.group(1))
        if power_idx > 1:
            power_idx = power_idx - 1
    else:
        power_idx = 1
    if m_sleep:
        sleep_idx = int(m_sleep.group(1))
        if sleep_idx > 1:
            sleep_idx = sleep_idx - 1
    else:
        sleep_idx = 1
    if m_lid:
        lid_idx = int(m_lid.group(1))
        if lid_idx > 1:
            lid_idx = lid_idx - 1
    else:
        lid_idx = 1
    
    hex_screen = None
    hex_hibern = None
    
    str_screen = None
    str_hibern = None
    
    if m_screen:
        hex_screen = m_screen.group(1)
    if m_hibern:
        hex_hibern = m_hibern.group(1)
    
    if hex_screen != None:
        dec_screen = int(hex_screen, 0)/60
        str_screen = str(dec_screen)
    if m_hibern != None:
        dec_hibern = int(hex_hibern, 0)/60
        str_hibern = str(dec_hibern)
    
    screen_idx = -1
    hibern_idx = -1
    
    if str_screen != None:
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
        elif str_screen == '0':
            screen_idx = 15
    
    if str_hibern != None:
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
        elif str_hibern == '0':
            hibern_idx = 12
        
    if screen_idx == -1:
        screen_idx = 15
    if hibern_idx == -1:
        hibern_idx = 12
        
    return (screen_idx, hibern_idx, power_idx, sleep_idx, lid_idx)