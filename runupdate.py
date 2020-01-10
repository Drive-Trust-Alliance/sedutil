import datetime
#from multiprocessing import Event, Array, Value
import platform
import os
import re
import subprocess

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
        

def rp_pbaWrite(e_to, i, result_list, status_list, count, password, au_pwd, dev, prefix, model, sn, ds_sup, user, version, admin_counter, pass_sav, sel_drive):
    pass_usb = ''
    rc = -1
    pass_rd = False
    with open(os.devnull, 'w') as pipe:
        if password == None:
            (password, pass_usb) = passReadUSB("Admin", dev, model, sn)
            pass_rd = True
        if password != None and password != 'x':
            rc = subprocess.call([prefix + 'sedutil-cli', '-n', '-t', '--loadpbaimage', password, 'n', dev], stdout=pipe)#stderr=log)
            
        dev_idx = i
        status = rc
        status_list[count] = status
        NOT_AUTHORIZED = 1
        AUTHORITY_LOCKED_OUT = 18
        SP_BUSY = 3

        if e_to.is_set():
            result_list[count] = 2
        elif status != 0 :
            if (status == NOT_AUTHORIZED or status == AUTHORITY_LOCKED_OUT) and ds_sup == 'Supported':
                
                
                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                timeStr = timeStr[2:]
                if status == NOT_AUTHORIZED and admin_counter < 5:
                    statusAW = subprocess.call([prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '04' + timeStr, au_pwd, 'User' + user, dev], stdout=pipe)#stderr=log)
                elif status == AUTHORITY_LOCKED_OUT or admin_counter >= 5:
                    statusAW = subprocess.call([prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '05' + timeStr, au_pwd, 'User' + user, dev], stdout=pipe)#stderr=log)
                    statusAW = subprocess.call([prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '09' + timeStr, au_pwd, 'User' + user, dev], stdout=pipe)#stderr=log)
                subprocess.call([prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '28' + timeStr, au_pwd, 'User' + user, dev], stdout=pipe)#stderr=log)
            
            if status == NOT_AUTHORIZED or status == AUTHORITY_LOCKED_OUT:
                result_list[count] = 1
            elif status == SP_BUSY:
                result_list[count] = 4
            else:
                result_list[count] = 3
        else :
            admin_counter = 0
            
            p = os.popen(prefix + 'sedutil-cli -n -t --pbaValid "' +  password + '" ' + dev).read()
            
            pba_regex = 'PBA image version\s*:\s*([A-z0-9\.\-]+)$'
            m1 = re.search(pba_regex, p)
            if m1:
                pba_ver = m1.group(1)
                pba_ver = pba_ver.strip('\0')
                pba_ver = pba_ver.ljust(32)
                f = open('datawrite' + sn + '.txt','w+')
                f.write(pba_ver)
                f.close()
                s = subprocess.call([prefix + 'sedutil-cli', '-n', '-t', '--datastorewrite', password, 'Admin1', 'datawrite' + sn + '.txt', '0', '130098', '32', dev], stdout=pipe)#stderr=log)
            save_status = -1
            if version % 2 == 1 and pass_rd:
                if ds_sup == 'Supported':
                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                    timeStr = timeStr[2:]
                    statusAW = subprocess.call([prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '25' + timeStr, password, 'Admin1', dev], stdout=pipe)#stderr=log)
            if version % 2 == 1 and pass_sav:
                save_status = passSaveUSB(password, sel_drive, model, sn, pass_usb, 'Admin')
                if ds_sup == 'Supported' and save_status == 0:
                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                    timeStr = timeStr[2:]
                    statusAW = subprocess.call([prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '23' + timeStr, password, 'Admin1', dev], stdout=pipe)#stderr=log)
            if save_status <= 0:
                result_list[count] = 0
            else:
                result_list[count] = 5
            if ds_sup == 'Supported':
                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                timeStr = timeStr[2:]
                statusAW = subprocess.call([prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '03' + timeStr, password, 'Admin1', dev], stdout=pipe)#stderr=log)
                statusAW = subprocess.call([prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '10' + timeStr, password, 'Admin1', dev], stdout=pipe)#stderr=log)