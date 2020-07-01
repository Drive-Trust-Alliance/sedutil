import datetime
import os
import re
import platform
import subprocess
import time

def findUSB(auth, dev, model, sn):
    folder_list = []
    drive_list = []
    dev_os = platform.system()
    err_list = ''
    present = [False]*len(dev)
    if dev_os == 'Windows':
        #print 'findusb before wmic'
        txt = os.popen('wmic logicaldisk where "drivetype=2" get caption,filesystem').read()
        #print 'findusb after wmic'
        txt_regex = '([D-Z]:)\s+FAT'
        drive_list = re.findall(txt_regex,txt)
        for d in drive_list:
            if os.path.isdir('%s\\OpalLock' % d):
                folder_list.append(d)
                for i in range(len(dev)):
                    if os.path.isfile(d + '\\OpalLock\\' + model[i] + '_' + sn[i] + '.psw'):
                        rgx_a = auth
                        try:
                            f = open(d + '\\OpalLock\\' + model[i] + '_' + sn[i] + '.psw', 'r')
                            t = f.read()
                            f.close()
                            m = re.search(rgx_a, t)
                            if m:
                                present[i] = True
                        except IOError:
                            pass
    elif dev_os == 'Linux':
        txt = os.popen("for DLIST in `dmesg  | grep \"Attached SCSI removable disk\" | cut -d\" \" -f 3  | sed -e 's/\[//' -e 's/\]//'` ; do\necho $DLIST\ndone ").read()
        txt_regex = 'sd[a-z]'
        list_u = re.findall(txt_regex,txt)
        for u in list_u:
            txt1 = os.popen('sudo mount').read()
            m = re.search(u,txt1)
            if not m:
                txt2 = os.popen('sudo blkid').read()
                rgx = '(' + u + '1?).+'
                m1 = re.search(rgx,txt2)
                if m1:
                    r2 = '\s+TYPE="([a-z]+)"'
                    txt3 = m1.group(0)
                    m2 = re.search(r2,txt3)
                    type_a = m2.group(1)
                    s = os.system('sudo mount -t ' + type_a + ' /dev/' + m1.group(1))
        txt = os.popen('sudo mount').read()
        dev_regex = '/dev/sd[a-z][1-9]?\s*on\s*(\S+)\s*type'
        drive_list = re.findall(dev_regex, txt)
        for d in drive_list:
            if os.path.isdir('%s/OpalLock' % d):
                folder_list.append(d)
                for i in range(len(dev)):
                    if os.path.isfile(d + '/OpalLock/' + model[i] + '_' + sn[i] + '.psw'):
                        rgx_a = auth
                        try:
                            f = open(d + '/OpalLock/' + model[i] + '_' + sn[i] + '.psw', 'r')
                            t = f.read()
                            f.close()
                            m = re.search(rgx_a, t)
                            if m:
                                present[i] = True
                        except IOError:
                            pass
    err_start = True
    for j in range(len(present)):
        if not present[j]:
            if err_start:
                err_start = False
            else:
                err_list = err_list + ', '
            err_list = err_list + dev[j]
    return (folder_list,drive_list, err_list)

def passReadUSB(auth, dev, model, sn):
    pass_usb = ''
    latest_pw = 'x'
    latest_ts = 0
    latest_pu = ''
    dev_os = platform.system()
    nl = []
    d_list = findUSB(auth, [dev], [model], [sn])
    folder_list = d_list[0]
    drive_list = d_list[1]
    if len(drive_list) == 0:
        return (None, pass_usb)
    elif len(folder_list) == 0:
        return (latest_pw, pass_usb)
    else:
        for i in range(len(folder_list)):
            filename = model + '_' + sn + '.psw'
            if dev_os == 'Windows':
                filepath = folder_list[i] + "\\OpalLock\\" + filename
            elif dev_os == 'Linux':
                filepath = folder_list[i] + "/OpalLock/" + filename
            if os.path.isfile(filepath):
                pass_usb = folder_list[i]
                try:
                    f = open(filepath, 'r')
                    txt = f.read()
                    f.close()
                    pswReg = None
                    if auth == 'Admin':
                        pswReg = 'Timestamp: ([0-9]{14})\r?\nAdmin: (\S+)'
                    else:
                        pswReg = 'Timestamp: ([0-9]{14})\r?\nUser: (\S+)'
                    entry = re.search(pswReg, txt)
                    if entry:
                        ts = int(entry.group(1))
                        pw = entry.group(2).strip('\0')
                        if ts > latest_ts:
                            latest_ts = ts
                            latest_pw = pw
                            latest_pu = pass_usb
                except IOError:
                    pass
        return (latest_pw, latest_pu)

def passSaveUSB(hashed_pwd, drive, mnum, snum, pass_usb, auth):
    hashed_pwd = hashed_pwd.strip('\0')
    dev_os = platform.system()
    timestamp = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
    f = None
    path = ''
    if dev_os == 'Windows':
        if pass_usb != '':
            drive = pass_usb
        if os.path.isdir(drive):
            try:
                if not os.path.isdir('%s\\OpalLock' % drive):
                    os.makedirs('%s\\OpalLock' % drive)
                path = '' + drive + '\\OpalLock\\' + mnum + '_' + snum + '.psw'
            except WindowsError:
                pass
    elif dev_os == 'Linux':
        if pass_usb != '':
            drive = pass_usb
        if os.path.isdir(drive):
            try:
                if not os.path.isdir('%s/OpalLock' % drive):
                    os.makedirs('%s/OpalLock' % drive)
                path = '' + drive + '/OpalLock/' + mnum + '_' + snum + '.psw'
            except WindowsError:
                pass
    if path != '':
        if os.path.isfile(path):
            try:
                f = open(path, 'r+')
                txt = f.read()
                f.seek(0)
                f.write('Model Number: ' + mnum + '\nSerial Number: ' + snum + '\n')
                entryReg = '(Timestamp: [0-9]{14}\r?\n([A-z0-9]+): \S+)'
                entries = re.findall(entryReg, txt)
                for e in entries:
                    if auth != e[1]:
                        f.write('\n' + e[0])
                f.write('\n\nTimestamp: ' + timestamp + '\n' + auth + ': ' + hashed_pwd)
                
                f.truncate()
                f.close()
            except IOError:
                return 1
        else:
            try:
                f = open(path, 'w')
                f.write('Model Number: ' + mnum + '\nSerial Number: ' + snum + '\n')
                f.write('\n\nTimestamp: ' + timestamp + '\n' + auth + ': ' + hashed_pwd)
                
                f.close()
            except IOError:
                return 1
        return 0
    else:
        return 1
        
def passSaveAppData(hashed_pwd, mnum, snum, auth):
    dev_os = platform.system()
    if dev_os == 'Windows':
        hashed_pwd = hashed_pwd.strip('\0')
        timestamp = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
        f = None
        adDir = 'C:\\Users\\' + os.getenv('username') + '\\AppData\\Local\\OpalLock'
        if not os.path.isdir(adDir):
            os.makedirs(adDir)
        adFile = adDir + '\\' + mnum + '_' + snum + '.psw'
        if os.path.isfile(adFile):
            try:
                f = open(adFile, 'r+')
                txt = f.read()
                f.seek(0)
                f.write('Model Number: ' + mnum + '\nSerial Number: ' + snum + '\n')
                entryReg = '(Timestamp: [0-9]{14}\r?\n([A-z0-9]+): \S+)'
                entries = re.findall(entryReg, txt)
                for e in entries:
                    if auth != e[1]:
                        f.write('\n' + e[0])
                f.write('\n\nTimestamp: ' + timestamp + '\n' + auth + ': ' + hashed_pwd)
                
                f.truncate()
                f.close()
            except IOError:
                pass
        else:
            try:
                f = open(adFile, 'w')
                f.write('Model Number: ' + mnum + '\nSerial Number: ' + snum + '\n')
                f.write('\n\nTimestamp: ' + timestamp + '\n' + auth + ': ' + hashed_pwd)
                
                f.close()
            except IOError:
                pass
    return
        
def removeUserUSB(prefix, model, sn, *args):
    folder_list = []
    latest_pw = ""
    latest_ts = "0"
    dev_os = platform.system()
    if dev_os == 'Windows':
        txt = os.popen('wmic logicaldisk where "drivetype=2" get caption,filesystem').read()
        txt_regex = '([D-Z]:)\s+FAT'
        drive_list = re.findall(txt_regex,txt)
        for d in drive_list:
            if os.path.isdir('%s\\OpalLock' % d):
                folder_list.append(d)
    elif dev_os == 'Linux':
        txt = os.popen(prefix + 'mount').read()
        dev_regex = '/dev/sd[a-z][1-9]?\s*on\s*(\S+)\s*type'
        drive_list = re.findall(dev_regex, txt)
        txt2 = os.popen(prefix + 'blkid').read()
        dev_regex2 = '(/dev/sd[a-z][1-9]?.+)'
        all_list = re.findall(dev_regex2, txt2)
        r1 = '/dev/sd[a-z][1-9]?'
        r2 = '\s+TYPE="([a-z]+)"'
        for a in all_list:
            m1 = re.search(r1,a)
            m2 = re.search(r2,a)
            dev_a = m1.group(0)
            type_a = m2.group(1)
            if dev_a not in drive_list:
                s = os.system(prefix + 'mount -t ' + type_a + ' ' + dev_a)
        txt = os.popen(prefix + 'mount').read()
        dev_regex = '/dev/sd[a-z][1-9]?\s*on\s*(\S+)\s*type'
        drive_list = re.findall(dev_regex, txt)
        for d in drive_list:
            if os.path.isdir('%s/OpalLock' % d):
                folder_list.append(d)
    if len(folder_list) == 0:
        return
    else:
        pw = 'x'
        for i in range(len(folder_list)):
            filename = model + '_' + sn + '.psw'
            if dev_os == 'Windows':
                filepath = folder_list[i] + "\\OpalLock\\" + filename
            elif dev_os == 'Linux':
                filepath = folder_list[i] + "/OpalLock/" + filename
            if os.path.isfile(filepath):
                try:
                    f = open(filepath, 'r+')
                    txt = f.read()
                    pswReg = 'Timestamp: [0-9]{14}\r?\nUser: (\S+)'
                    entry = re.search(pswReg, txt)
                    entryReg = '(Timestamp: [0-9]{14}\r?\n([A-z0-9]+): \S+)'
                    entries = re.findall(entryReg, txt)
                    if entry:
                        if len(entries) == 1:
                            f.close()
                            os.remove(filepath)
                        else:
                            f.seek(0)
                            f.write('Model Number: ' + model + '\nSerial Number: ' + sn + '\n')
                            for e in entries:
                                if e[1] != 'User':
                                    f.write('\n' + e[0])
                            
                            f.truncate()
                            f.close()
                    else:
                        f.close()
                except IOError:
                    pass
        return
        
#def removeUserAppData(mnum, snum):
#    adDir = 'C:\\Users\\' + os.getenv('username') + '\\AppData\\Local\\OpalLock'
#    if dev_os == 'Windows' and os.path.isdir(adDir):
        

def rp_setupFull(e, i, result_list, status_list, trylimit_list, count, rc_list, password, status_usb, usb_dir, dev, prefix, msid, model, sn, usb, mbr_sup, admin, user, version, preserved_files, au_pwd):
    usb_failed = False
    status_final = -1
    rc = -1
    save_status = -1
    valid = False
    with open(os.devnull, 'w') as pipe:
        if not e.is_set():
            queryText = os.popen(prefix + 'sedutil-cli --query ' + dev).read()
            if not e.is_set():
                txt_LE = "LockingEnabled = N"
                unlocked = re.search(txt_LE, queryText)
                if unlocked:
                    subprocess.call([prefix + 'sedutil-cli', '-n', '-t', '--activate', msid, dev], stdout=pipe)#stderr=log)
                    if count == 0:
                        valid = True
                if count == 0 and not e.is_set():
                    hash_pwd = ''
                    if not valid:
                        statusTest = subprocess.call([prefix + 'sedutil-cli', '-n', '-t', '--setAdmin1Pwd', msid, msid, dev], stdout=pipe)
                        if statusTest == 0:
                            valid = True
                    if valid and version != 4:
                        status_tmp = subprocess.call([prefix + 'sedutil-cli', '--createUSB', 'UEFI', dev, '\\\\.\\PhysicalDrive' + usb], stdout=pipe)#stderr=log)
                        if status_tmp == 0:
                            if usb_dir == '':
                                usb_dev = usb
                                p = subprocess.Popen(["diskpart"], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
                                res1 = p.stdin.write('select disk ' + usb_dev + b'\n')
                                res1 = p.stdin.write('detail disk\n')
                                res1 = p.stdin.write('exit\n')

                                output = p.communicate()[0]
                                
                                vol_re = 'Volume [0-9]+\s+([A-Z])\s+'

                                list_v = re.findall(vol_re, output)
                                if list_v != []:
                                    usb_dir = list_v[0] + ':'
                            if usb_dir != '':
                                if len(preserved_files) > 0:
                                    try:
                                        os.makedirs('%s/OpalLock' % usb_dir)
                                    except WindowsError:
                                        pass
                                for fp in preserved_files:
                                    try:
                                        f = open(fp[0], 'w')
                                        f.write(fp[1])
                                        f.close()
                                    except IOError:
                                        pass
                                save_status = passSaveUSB(password, usb_dir, model, sn, '', "Admin")
                                if save_status == 0:
                                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                                    timeStr = timeStr[2:]
                                    statusAW = subprocess.call([prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '23' + timeStr, msid, 'Admin1', dev], stdout=pipe)#stderr=log)
                        status_usb.value = status_tmp
                    elif version != 4:
                        status_final = 1
                        status_usb.value = -2
                    else:
                        status_usb.value = 0
                elif count != 0 and not e.is_set() and version != 4:
                    while status_usb.value == -1:
                        time.sleep(1)
                    if status_usb.value == 0:
                        if usb_dir == '':
                            
                            if dev_os == 'Windows':
                                usb_dev = usb
                                p = subprocess.Popen(["diskpart"], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
                                res1 = p.stdin.write('select disk ' + usb_dev + b'\n')
                                res1 = p.stdin.write('detail disk\n')
                                res1 = p.stdin.write('exit\n')

                                output = p.communicate()[0]
                                
                                vol_re = 'Volume [0-9]+\s+([A-Z])\s+'

                                list_v = re.findall(vol_re, output)
                                if list_v != []:
                                    usb_dir = list_v[0] + ':'
                        if usb_dir != '':
                            pass_usb = ''
                            save_status = passSaveUSB(password, usb_dir, model, sn, pass_usb, "Admin")
                            #if ds_sup == 'Supported' and save_status == 0:
                            if save_status == 0:
                                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                                timeStr = timeStr[2:]
                                statusAW = subprocess.call([prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '23' + timeStr, msid, 'Admin1', dev], stdout=pipe)#stderr=log)
                if status_usb.value == 0 and not e.is_set(): 
                    if version != 4 and mbr_sup != 'Not Supported':
                        rc = subprocess.call([prefix + 'sedutil-cli', '-n', '-t', '--loadpbaimage', msid, 'n', dev], stdout=pipe)#stderr=log)
                    
                    
                    #if not supported or written successfully
                    if rc == 0 or mbr_sup == 'Not Supported' or version == 4:
                        s1 = subprocess.call([prefix + 'sedutil-cli', '-n', '-t', '--setSIDPassword', msid, password, dev], stdout=pipe)#stderr=log)
                        if e.is_set():
                            if not s1:
                                s1 = subprocess.call([prefix + 'sedutil-cli', '-n', '-t', '--setSIDPassword', password, msid, dev], stdout=pipe)#stderr=log)
                        elif s1 == 0:
                            s2 = subprocess.call([prefix + 'sedutil-cli', '-n', '-t', '--setAdmin1Pwd', msid, password, dev], stdout=pipe)#stderr=log)
                            if e.is_set():
                                if not s2:
                                    s1 = subprocess.call([prefix + 'sedutil-cli', '-n', '-t', '--setSIDPassword', password, msid, dev], stdout=pipe)#stderr=log)
                                    s2 = subprocess.call([prefix + 'sedutil-cli', '-n', '-t', '--setAdmin1Pwd', password, msid, dev], stdout=pipe)#stderr=log)
                            else:
                                status_is = subprocess.call([prefix + 'sedutil-cli', '-n', '-t', '-a', '--initialsetup', password, dev], stdout=pipe)#stderr=log)
                                if not e.is_set():
                                    status2 =  subprocess.call([prefix + 'sedutil-cli', '-n', '-t', '--enableLockingRange', '0', password, dev], stdout=pipe)#stderr=log)
                                    status_final = (s1 | s2)
                        
                        if s1 == 0 and not e.is_set():
                            #if ds_sup == 'Supported':
                            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                            timeStr = timeStr[2:]
                            statusAW = subprocess.call([prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '02' + timeStr, password, 'Admin1', dev], stdout=pipe)#stderr=log)
                            statusAW = subprocess.call([prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '13' + timeStr, password, 'Admin1', dev], stdout=pipe)#stderr=log)
                        else:
                            status_final = s1
                            s1 = subprocess.call([prefix + 'sedutil-cli', '-n', '-t', '--revertnoerase', msid, dev], stdout=pipe)#stderr=log)
                            s1 = subprocess.call([prefix + 'sedutil-cli', '-n', '-t', '--activate', msid, dev], stdout=pipe)#stderr=log)
                    
                    
                    
                    if status_final == 0:
                        num_admins = int(admin)
                        num_users = int(user)
                        
                        txt_tl = '05'
                        txt = os.popen(prefix + 'sedutil-cli -n -t --getmbrsize "' + password + '" ' + dev).read()
                        regex_tl = 'SID TryLimit = ([0-9]+)'
                        m_tl = re.search(regex_tl, txt)
                        if m_tl:
                            int_tl = int(m_tl.group(1))
                            trylimit_list[count] = int_tl
                            if int_tl < 10:
                                txt_tl = '0' + str(int_tl)
                            else:
                                txt_tl = str(int_tl)
                        
                        str_i = 'OpalLock Datastore Info 2'
                        for n in range(num_admins - 1):
                            str_i = str_i + '1'
                        for j in range(8 - num_admins):
                            str_i = str_i + '0'
                        for k in range(num_users - 1):
                            str_i = str_i + '1'
                        str_i = str_i + '2'
                        for l in range(64 - num_users):
                            str_i = str_i + '0'
                        str_i = str_i + txt_tl + 'N/A                             '
                        f = open('datawrite' + sn + '.txt','w+')
                        f.write(str_i)
                        f.close()
                        s = subprocess.call([prefix + 'sedutil-cli', '-n', '-t', '--datastorewrite', password, 'Admin1', 'datawrite' + sn + '.txt', '0', '130000', '130', dev], stdout=pipe)#stderr=log)
                        #os.remove('datawrite' + ui.sn_list[i] + '.txt')
                        passSaveAppData(password, model, sn, 'Admin')
                        
                        if version == 4 and usb_dir != '':
                            pass_usb = ''
                            save_status = passSaveUSB(password, usb_dir, model, sn, pass_usb, 'Admin')
                            if save_status == 0:
                                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                                timeStr = timeStr[2:]
                                statusAW = subprocess.call([prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '23' + timeStr, password, 'Admin1', dev], stdout=pipe)#stderr=log)
                    
                                
        if os.path.isfile('datawrite' + sn + '.txt'):
            try:
                os.remove('datawrite' + sn + '.txt')
            except WindowsError:
                print 'Error: datawrite' + sn + '.txt is somehow not closed'
        status_list[count] = status_final
        rc_list[count] = rc
        #print 'rc_list[' + str(count) + '] set to ' + str(rc) + ' confirm ' + str(rc_list[count])
        if e.is_set():
            result_list[count] = 2
        
        elif status_final != 0:
            result_list[count] = 1
        else :
            
            m1 = None
            if mbr_sup != 'Not Supported':
                p = os.popen(prefix + 'sedutil-cli -n -t --pbaValid "' +  password + '" ' + dev).read()
                pba_regex = 'PBA image version\s*:\s*([A-z0-9\.\-]+)$'
                m1 = re.search(pba_regex, p)
            if m1:
                pba_ver = m1.group(1)
                pba_ver = pba_ver.strip('\0')
                #print pba_ver
                pba_ver = pba_ver.ljust(32)
                #ui.pba_list[i] = pba_ver
                #if ds_sup == 'Supported':
                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                timeStr = timeStr[2:]
                statusAW = subprocess.call([prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '10' + timeStr, password, 'Admin1', dev], stdout=pipe)#stderr=log)
                f = open('datawrite' + sn + '.txt','w+')
                f.write(pba_ver)
                f.close()

                s = subprocess.call([prefix + 'sedutil-cli', '-n', '-t', '--datastorewrite', password, 'Admin1', 'datawrite' + sn + '.txt', '0', '130098', '32', dev], stdout=pipe)#stderr=log)
                
            if save_status <= 0:
                result_list[count] = 0
            else:
                result_list[count] = 3
                    
                    
                    
