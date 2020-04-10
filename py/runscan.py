import ctypes
import datetime
import dialogs
import gobject
import gtk
import lockhash
import os
import platform
import powerset
import random
import re
import runop
import string
from string import ascii_uppercase
if platform.system() == 'Windows':
    import subprocess
import threading
import verify

def run_scan(button, ui, fullscan):
    verified = verify.licCheck(ui)
    if verified:
        ui.scan_ip = True
        timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
        #print 'start scan ' + timeStr
        ui.start_spin()
        ui.load_instr.show()
        ui.disable_menu()
        
        if not ui.firstscan and fullscan:
            model = ui.dev_select.get_model()
        
            iter = gtk.TreeIter
            for row in model:
                model.remove(row.iter)
                
            ui.dev_vendor.set_text('')
            ui.dev_sn.set_text('')
            ui.dev_msid.set_text('')
            ui.dev_series.set_text('')
            ui.dev_pbaVer.set_text('')
            
            ui.dev_opal_ver.set_text('')
            ui.dev_status.set_text('')
            ui.dev_setup.set_text('')
            ui.dev_enc.set_text('')
            ui.dev_blockSID.set_text('')
            ui.dev_userSetup.set_text('')

            #if len(ui.vendor_list) > 0:
            #    ui.opal_ver_list = []
            #    ui.series_list = []
            #    ui.lockstatus_list = []
            #    ui.setupstatus_list = []
        
        if ui.firstscan and ui.DEV_OS == 'Windows':
            if os.path.isfile('mountvol.txt'):
                f = open('mountvol.txt', 'r')
                txt = f.read()
                #print txt
                f.close()
                regex = '([a-z0-9]{8}-[a-z0-9]{4}-[a-z0-9]{4}-[a-z0-9]{4}-[a-z0-9]{12}) ([A-Z]:) ([A-Z0-9]+)'
                usb_regex = '([a-z0-9]{8}-[a-z0-9]{4}-[a-z0-9]{4}-[a-z0-9]{4}-[a-z0-9]{12}) ([A-Z]:)\n'
                mv_tuples = re.findall(regex,txt)
                for t in mv_tuples:
                    duplicate = False
                    for e in ui.mv_list:
                        if not duplicate:
                            if t[0] == e[0] and t[2] == e[2]:
                                duplicate = True
                    if not duplicate:
                        entry = [t[0],t[1],t[2]]
                        #print entry
                        ui.mv_list.append(entry)
                #print ui.mv_list
                usb_tuples = re.findall(usb_regex,txt)
                for u in usb_tuples:
                    duplicate = False
                    for e in ui.usb_mv_list:
                        if not duplicate:
                            if u[0] == e[0]:
                                duplicate = True
                    if not duplicate:
                        entry = [u[0],u[1]]
                        ui.usb_mv_list.append(entry)
                #print '\n'
                #print ui.usb_mv_list
        
        #if fullscan:
        finddev(ui, fullscan)

def finddev(ui, fullscan):
    if not fullscan:
        ui.firstscan = False
    
    if fullscan:
        #ui.full_devs_list = []
        #ui.full_sn_list = []
        #ui.full_salt_list = []
        #ui.full_devs_map = []
        
        #ui.devs_list = []
        ui.locked_list = []
        ui.setup_list = []
        ui.nonsetup_list = []
        ui.tcg_list = []
        
        ui.mbr_list = []
        ui.mbr_setup_list = []
        
        ui.usetup_list = []
        ui.ulocked_list = []
        
        #ui.lockstatus_list = []
        #ui.setupstatus_list = []
        
        #ui.full_isSetup_list = []
        #ui.full_isLocked_list = []
        
        #ui.full_dsSup_list = []
        
        vendor_old = ui.vendor_list
        salt_old = ui.salt_list
        msid_old = ui.msid_list
        pba_old = ui.pba_list
        
        admin_aol_old = ui.admin_aol_list
        user_aol_old = ui.user_aol_list
        psid_aol_old = ui.psid_aol_list
        
        #ui.vendor_list = []
        #ui.salt_list = []
        #ui.sn_list = []
        #ui.series_list = []
        
        #ui.msid_list = []
        #ui.pba_list = []
        #ui.user_list = []
        #ui.full_user_list = []
        #ui.blockSID_list = []
        
        #ui.label_list = []
        ui.partition_list = []
        #ui.encsup_list = []
        
        #ui.setupuser_list = []
        #ui.datastore_list = []
        
        #ui.opal_ver_list = []
        #ui.full_ver_list = []
        
        #ui.tcg_usb_list = []
        
        
    def scan_t1():
        
        
        locked_new = []
        setup_new = []
        TCG_new = []
        not_sure = []
        
        fdm_new = []
        opal_ver_new = []
        full_ver_new = []
        encsup_new = []
        devs_new = []
        vendor_new = []
        series_new = []
        salt_new = []
        sn_new = []
        full_devs_new = []
        full_sn_new = []
        full_salt_new = []
        pba_new = []
        msid_new = []
        
        mbr_new = []
        
        #label_new = []
        
        lockstatus_new = []
        setupstatus_new = []
        blockSID_new = []
        retrylimit_new = []
        user_new = []
        admin_new = []
        setupuser_new = []
        datastore_new = []
        full_user_new = []
        full_dsSup_new = []
        admin_aol_new = []
        user_aol_new = []
        psid_aol_new = []
        full_isLocked_new = []
        full_isSetup_new = []
        
        mbr_new = []
        mbr_setup_new = []
        
        tcg_usb_new = []
        
        tcg_count = 0
        
        if fullscan:
            rescan_needed = False
            #print ui.vendor_list
            #print ui.series_list
            #print ui.sn_list
            for x in range(len(ui.devs_list)):
                runop.prelock(x)
                txt_q = os.popen(ui.prefix + 'sedutil-cli --query ' + ui.devs_list[x]).read()
                runop.postlock(x)
                regex_d = ui.vendor_list[x] + '\s*:\s*' + ui.series_list[x] + '\s*' + ui.sn_list[x]
                regex_l = 'Locked = ([YN])'
                m1 = re.search(regex_d, txt_q)
                if m1:
                    m2 = re.search(regex_l, txt_q)
                    if m2:
                        if (m2.group(1) == 'N' and x in ui.locked_list) or (m2.group(1) == 'Y' and x not in ui.locked_list):
                            rescan_needed = True
                else:
                    rescan_needed = True
            #print 'rescan needed?'
            #print rescan_needed
            if not ui.firstscan:
                for z in range(len(ui.devs_list)):
                    runop.prelock(z)
            if ui.DEV_OS == 'Windows' and (rescan_needed or ui.firstscan):
                with open(os.devnull, 'w') as pipe:
                    f = open('rescan.txt', 'w')
                    f.write('rescan')
                    f.close()
                    subprocess.call(['diskpart', '/s', 'rescan.txt'], stdout=pipe)#stderr=log)
                    os.remove('rescan.txt')
            txt = os.popen(ui.prefix + 'sedutil-cli --scan n').read()
            if not ui.firstscan:
                for z in range(len(ui.devs_list)):
                    runop.postlock(z)
            
            #print txt
         
            names = ['PhysicalDrive[0-9]', '/dev/sd[a-z]', '/dev/nvme[0-9]',  '/dev/disk[0-9]']
            idx = 0
            
            
            
            
            dev_count = 0
            
            
            mn_tmp = []
            s_tmp = []
            tcg_tmp = []
            
            bridge_map = []
            
            
            for index in range(len(names)): #index=0(window) 1(Linux) 2(OSX)
            
                m = re.search(names[index] + ".*", txt)
                
                if m:
                    
                    txt11 = names[index] + "\s+.[A-z0-9]+\s+.*"
                    m1 = re.findall(txt11, txt)
                    if m1:
                        
                        for tt in m1:
                            m2 = re.match(names[index],tt)
                            if (index == 0) : 
                                ui.devname = "\\\\.\\" + m2.group()
                            else:
                                ui.devname =  m2.group()
                            
                            rgx = '(?:PhysicalDrive[0-9]+|/dev/sd[a-z][0-9]?|/dev/nvme[0-9])\s+([12ELP]+|No)\s+(\S+(?:\s\S+)*)\s*:\s*([^:]+)\s*:(.+)'
                            md = re.match(rgx,tt)
                            if md:
                                mn_tmp.append(md.group(2))
                                s_tmp.append(md.group(4).ljust(20))
                                tcg_tmp.append(md.group(1))
                                
                                
            for j in range(len(mn_tmp)):
                matched = False
                for k in range(len(ui.devs_list)):
                    if vendor_old[k] == mn_tmp[j] and salt_old[k] == s_tmp[j] and tcg_tmp[j] != 'No':
                        matched = True
                if matched:
                    bridge_map.append(1)
                else:
                    bridge_map.append(0)
            
            
            #was here
            
            
            full_opalver = []
            full_encsup = []
            full_vendor = []
            full_series = []
            
            for index in range(len(names)): #index=0(window) 1(Linux) 2(OSX)
            
                m = re.search(names[index] + ".*", txt)
                
                if m:
                    
                    if (index == 0 ):
                       ui.prefix = ""
                    else:
                       ui.prefix = "sudo "
                    
                    txt11 = names[index] + "\s+.[A-z0-9]+\s+.*"
                    m1 = re.findall(txt11, txt)
                    if m1:
                        bm_idx = 0
                        
                        for tt in m1:
                            m2 = re.match(names[index],tt)
                            if (index == 0) : 
                                ui.devname = "\\\\.\\" + m2.group()
                            else:
                                ui.devname =  m2.group()
                                
                            
                            
                            rgx = '(?:PhysicalDrive[0-9]+|/dev/sd[a-z][0-9]?|/dev/nvme[0-9])\s+([12ELP]+|No)\s+(\S+(?:\s\S+)*)\s*:\s*([^:]+)\s*:(.+)'
                            md = re.match(rgx,tt)
                            
                            
                            if md:
                                full_devs_new.append(ui.devname)
                                full_sn_new.append(md.group(4).replace(' ', ''))
                                full_salt_new.append(md.group(4).ljust(20))
                                full_vendor.append(md.group(2))
                                full_series.append(md.group(3))
                                if md.group(1) == 'No':
                                    full_opalver.append('None')
                                    full_ver_new.append(False)
                                    full_encsup.append('N/A')
                                else:
                                    tcg_count = tcg_count + 1
                                    full_ver_new.append(True)
                                    if md.group(1) == 'P':
                                        full_opalver.append("Pyrite")
                                        full_encsup.append("Not Supported")
                                    else:
                                        full_encsup.append("Supported")
                                        if md.group(1) == '1' or md.group(1) == '2':
                                            full_opalver.append("Opal " + md.group(1) + ".0")
                                        elif md.group(1) == '12':
                                            full_opalver.append("Opal 1.0/2.0")
                                        elif md.group(1) == 'E':
                                            full_opalver.append("Enterprise")
                                        elif md.group(1) == 'L':
                                            full_opalver.append("Opallite")
                                        elif md.group(1) == 'R':
                                            full_opalver.append("Ruby")
                                        else:
                                            full_opalver.append(md.group(1))
            if tcg_count > ui.MAX_DEV:
                selection_map = [0] * len(full_devs_new)
                tcg_track = 0
                if ui.firstscan and ui.VERSION == 1:
                    for i in range(len(full_devs_new)):
                        if not full_ver_new[i]:
                            selection_map[i] = 1
                        elif tcg_track < ui.MAX_DEV:
                            for j in range(256):
                                runop.prelock(j)
                            txt_q = os.popen(ui.prefix + 'sedutil-cli --query ' + full_devs_new[i]).read()
                            for j in range(256):
                                runop.postlock(j)
                            rgx_q = 'Locked = Y'
                            mq = re.match(rgx_q,txt_q)
                            if mq:
                                selection_map[i] = 1
                                tcg_track = tcg_track + 1
                elif not ui.firstscan:
                    for i in range(len(full_devs_new)):
                        if not full_ver_new[i]:
                            selection_map[i] = 1
                        elif bridge_map[i]:
                            selection_map[i] = 1
                            tcg_track = tcg_track + 1
                if tcg_track < ui.MAX_DEV:
                    i = 0
                    while tcg_track < ui.MAX_DEV:
                        if not selection_map[i]:
                            selection_map[i] = 1
                            if full_ver_new[i]:
                                tcg_track = tcg_track + 1
                        i = i + 1
                k = 0
                for j in range(len(selection_map)):
                    if selection_map[j]:
                        devs_new.append(full_devs_new[j])
                        sn_new.append(full_sn_new[j])
                        vendor_new.append(full_vendor[j])
                        series_new.append(full_series[j])
                        salt_new.append(full_salt_new[j])
                        opal_ver_new.append(full_opalver[j])
                        encsup_new.append(full_encsup[j])
                        fdm_new.append(k)
                        k = k + 1
                    else:
                        fdm_new.append(-1)
            else:
                devs_new = full_devs_new
                sn_new = full_sn_new
                vendor_new = full_vendor
                series_new = full_series
                salt_new = full_salt_new
                opal_ver_new = full_opalver
                encsup_new = full_encsup
                for k in range(len(devs_new)):
                    fdm_new.append(k)
        drive_dict = {}
        sedutil_res = None
        hash_res = None
        
        mv_edit = []
        mv_newdir = []
        mv_rm = []
        usb_edit = []
        if devs_new != [] or not fullscan:
            if fullscan:
                lockstatus_new = [None] * len(devs_new)
                setupstatus_new = [None] * len(devs_new)
                blockSID_new = [None] * len(devs_new)
                msid_new = [None] * len(devs_new)
                pba_new = [None] * len(devs_new)
                retrylimit_new = [None] * len(devs_new)
                user_new = [None] * len(devs_new)
                admin_new = [None] * len(devs_new)
                setupuser_new = [None] * len(devs_new)
                datastore_new = [None] * len(devs_new)
                
                full_user_new = [None] * len(full_devs_new)
                full_dsSup_new = [None] * len(full_devs_new)
                
                admin_aol_new = [0] * len(devs_new)
                user_aol_new = [0] * len(devs_new)
                psid_aol_new = [0] * len(devs_new)
                
                locked_new = [None] * len(devs_new)
                setup_new = [None] * len(devs_new)
                TCG_new = [None] * len(devs_new)
                not_sure = [None] * len(full_devs_new)
                
                full_isSetup_new = [None] * len(full_devs_new)
                full_isLocked_new = [None] * len(full_devs_new)
                
                tcg_usb_new = [None] * len(devs_new)
                
            
            if ui.firstscan and fullscan:
                pwd_test = ''.join(random.SystemRandom().choice(ascii_uppercase + string.digits) for _ in range(16))
                salt_test = salt_new[0]
                dev_test = devs_new[0]
                sedutil_txt = os.popen(ui.prefix + 'sedutil-cli --hashvalidation ' + pwd_test + ' ' + dev_test).read()
                sedutil_regex = 'hashed password\s*:\s*([A-F0-9]{64})'
                sedutil_match = re.search(sedutil_regex, sedutil_txt)
                sedutil_res = sedutil_match.group(1).lower()
                hash_res = lockhash.hash_pbkdf2(pwd_test, salt_test)
            elif not fullscan:
                ui.firstscan = False
                
            user1setup = [None] * len(devs_new)
            pbaver = [None] * len(devs_new)
            retrylimit = [None] * len(devs_new)
            m = [None] * len(full_devs_new)
            msid = [None] * len(devs_new)
            
            status_dsr = [None] * len(full_devs_new)
            
            #for i in range(len(ui.devs_list)):
            def t_run(i, vendor_old, salt_old, pba_old, msid_old, locked_new, setup_new, TCG_new, not_sure, lockstatus_new, setupstatus_new, blockSID_new, msid_new, pba_new, retrylimit_new, user_new, admin_new, setupuser_new, datastore_new, full_user_new, full_dsSup_new, admin_aol_new, user_aol_new, psid_aol_new, full_isSetup_new, full_isLocked_new, tcg_usb_new):
                for j in range(256):
                    runop.prelock(j)
                queryText = os.popen(ui.prefix + 'sedutil-cli --query ' + full_devs_new[i]).read()
                for j in range(256):
                    runop.postlock(j)
                
                dl_idx = fdm_new[i]
                
                txt_TCG = "Locked = "
                isTCG = re.search(txt_TCG, queryText)
                
                txt_MBRSup = "MBR shadowing Not Supported = N"
                isSup = re.search(txt_MBRSup, queryText)
                
                msid_i = 'N/A'
                if fullscan:
                    if dl_idx != -1 and opal_ver_new[dl_idx] == 'Pyrite':
                        p1 = 'PYRITE 1\.0 function'
                        p2 = 'PYRITE 2\.0 function'
                        m1 = re.search(p1, queryText)
                        m2 = re.search(p2, queryText)
                        if m1:
                            opal_ver_new[dl_idx] = 'Pyrite 1.0'
                        elif m2:
                            opal_ver_new[dl_idx] = 'Pyrite 2.0'
                    
                    old_idx = -1
                    if dl_idx != -1:
                        for j in range(len(vendor_old)):
                            if vendor_new[dl_idx] == vendor_old[j] and salt_new[dl_idx] == salt_old[j]:
                                old_idx = j
                            
                    
                    
                    
                    if old_idx >= 0:
                        admin_aol_new[dl_idx] = admin_aol_old[old_idx]
                        user_aol_new[dl_idx] = user_aol_old[old_idx]
                        psid_aol_new[dl_idx] = psid_aol_old[old_idx]
                    if isTCG:
                        m_user = None
                        #if old_idx >= 0:
                        #    ui.msid_list[i] = msid_old[old_idx]
                        
                        regex_datastore = 'DataStore function \(0x0202\)'
                        regex_users = "Locking Users = ([0-9]+)"
                        regex_admins = "Locking Admins = ([0-9]+)"
                        
                        m_datastore = re.search(regex_datastore, queryText)
                        if dl_idx != -1:
                            isUSB = '(?:PhysicalDrive[0-9]+|/dev/sd[a-z][0-9]?|/dev/nvme[0-9])\s*USB'
                            m_USB = re.search(isUSB, queryText)
                            if m_USB:
                                tcg_usb_new[dl_idx] = True
                            else:
                                tcg_usb_new[dl_idx] = False
                        
                            #if m_datastore:
                            datastore_new[dl_idx] = 'Supported'
                            full_dsSup_new[i] = True
                            #else:
                            #    ui.datastore_list[dl_idx] = 'Not Supported'
                            #    ui.full_dsSup_list[i] = False
                        
                            m_user = re.search(regex_users, queryText)
                            m_admin = re.search(regex_admins, queryText)
                            
                            if m_user:
                                user_new[dl_idx] = m_user.group(1)
                                full_user_new[i] = m_user.group(1)
                                admin_new[dl_idx] = m_admin.group(1)
                            else:
                                user_new[dl_idx] = '0'
                                full_user_new[i] = '0'
                        else:
                            if m_datastore:
                                full_dsSup_new[i] = True
                            else:
                                full_dsSup_new[i] = False
                            m_user = re.search(regex_users, queryText)
                            if m_user:
                                full_user_new[i] = m_user.group(1)
                        #txt_MBRSup = "MBR shadowing Not Supported = N"
                        txt_BSID = "BlockSID"
                        txt_BSID_enabled = "BlockSIDState = Y"
                        
                        #isSup = re.search(txt_MBRSup, queryText)
                        hasBlockSID = re.search(txt_BSID, queryText)
                        isBlockSID = re.search(txt_BSID_enabled, queryText)
                        
                        
                        txt_L = "Locked = Y"
                        txt_NS = "LockingEnabled = N"

                        
                        isLocked = re.search(txt_L, queryText)
                        isNotSetup = re.search(txt_NS, queryText)
                    
                        if isLocked:
                            if dl_idx != -1:
                                locked_new[dl_idx] = True
                                setup_new[dl_idx] = 1
                                TCG_new[dl_idx] = True
                                lockstatus_new[dl_idx] = 'Locked'
                                setupstatus_new[dl_idx] = 'Yes'
                            not_sure[i] = False
                            full_isLocked_new[i] = True
                            full_isSetup_new[i] = True
                        elif isNotSetup:
                            if dl_idx != -1:
                                locked_new[dl_idx] = False
                                setup_new[dl_idx] = 0
                                TCG_new[dl_idx] = True
                                lockstatus_new[dl_idx] = 'Unlocked'
                                setupstatus_new[dl_idx] = 'No'
                                pba_new[dl_idx] = 'N/A'
                                setupuser_new[dl_idx] = 'N/A'
                            not_sure[i] = False
                            full_isLocked_new[i] = False
                            full_isSetup_new[i] = False
                        else:
                            if dl_idx != -1:
                                locked_new[dl_idx] = False
                                TCG_new[dl_idx] = True
                                lockstatus_new[dl_idx] = 'Unlocked'
                            not_sure[i] = True
                            full_isLocked_new[i] = False
                            
                        
                        if dl_idx != -1:
                            if not isSup:
                                pba_new[dl_idx] = "Not Supported"
                                
                            if hasBlockSID and isBlockSID:
                                blockSID_new[dl_idx] = "Enabled"
                            elif hasBlockSID:
                                blockSID_new[dl_idx] = "Disabled"
                            else:
                                blockSID_new[dl_idx] = "Not Supported"
                            
                    elif dl_idx != -1:
                        msid_new[dl_idx] = 'N/A'
                        blockSID_new[dl_idx] = 'N/A'
                        pba_new[dl_idx] = 'N/A'
                        user_new[dl_idx] = '0'
                        lockstatus_new[dl_idx] = 'N/A'
                        setupstatus_new[dl_idx] = 'N/A'
                        datastore_new[dl_idx] = 'N/A'
                        
                        locked_new[dl_idx] = False
                        setup_new[dl_idx] = 0
                        TCG_new[dl_idx] = False
                        not_sure[dl_idx] = False
                        tcg_usb_new[dl_idx] = False
                        
                        
                pwd = lockhash.get_val() + full_salt_new[i]
                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')

                hash_pwd = ''
                if isTCG:
                    hash_pwd = lockhash.hash_pbkdf2(pwd, full_salt_new[i])
                
                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                
                #print 'after hashing audit user pwd ' + ui.devs_list[i] + ' ' + timeStr

                #user1setup = None
                #pbaver = None
                #m0 = None
                if isTCG and full_dsSup_new[i] == True:
                    lock_idx = -1
                    if ui.firstscan and fdm_new[i] != -1:
                        lock_idx = fdm_new[i]
                    elif not ui.firstscan and full_salt_new[i] in ui.salt_list:
                        try:
                            lock_idx = ui.salt_list.index(full_salt_new[i])
                        except ValueError:
                            pass
                    #if lock_idx != -1:
                    for j in range(256):
                        runop.prelock(j)
                    s = -1
                    if ui.DEV_OS == 'Windows':
                        with open(os.devnull, 'w') as pipe:
                            s = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--datastoreread', hash_pwd, 'User' + full_user_new[i], 'dataread' + full_sn_new[i] + '.txt', '0', '130000', '130', full_devs_new[i]], stdout=pipe)#stderr=log)
                    else:
                        s = os.system(ui.prefix + 'sedutil-cli -n -t -u --datastoreread "' + hash_pwd + '" User' + full_user_new[i] + ' dataread' + full_sn_new[i] + '.txt 0 130000 130 ' + full_devs_new[i])
                    #if lock_idx != -1:
                    for j in range(256):
                        runop.postlock(j)
                    
                    status_dsr[i] = s
                    
                    if s == 0:
                        f = open('dataread' + full_sn_new[i] + '.txt', 'r')
                        f_txt = f.read()
                        #print str(i) + ' ' + f_txt
                        f.close()
                        
                        hdr = 'OpalLock Datastore Info'
                        m0 = re.search(hdr, f_txt)
                        #print str(i) + ' regex match result'
                        #print m0
                        m[i] = m0
                        if dl_idx != -1:
                            if m0:
                                #setup_new[dl_idx] = True
                                #setupstatus_new[dl_idx] = 'Yes'
                                user1setup[dl_idx] = f_txt[32]
                                if isSup:
                                    re_pba = '^[A-z0-9\.\-]+'
                                    m_pba = re.search(re_pba, f_txt[98:])
                                    if m_pba:
                                        pbaver[dl_idx] = m_pba.group(0)
                                retrylimit[dl_idx] = int(f_txt[96:98])
                                #else:
                                #    pbaver.append(None)
                            #else:
                            #    setup_new[dl_idx] = False
                            #    setupstatus_new[dl_idx] = 'No'
                            #    pbaver.append(None)
                            #    user1setup.append(None)
                    elif dl_idx != -1:
                        retrylimit[dl_idx] = 5
                    #    m.append(None)
                    #    if dl_idx != -1:
                    #        user1setup.append(None)
                    #        pbaver.append(None)
                elif isTCG:
                    #m.append(None)
                    if dl_idx != -1:
                        if user_new[dl_idx] == '0':
                            user1setup[dl_idx] = '0'
                        elif setupuser_new[dl_idx] == None:
                            user1setup[dl_idx] = '1'
                        #else:
                        #    user1setup.append(None)
                        if pba_new[dl_idx] != None:
                            pbaver[dl_idx] = pba_new[dl_idx]
                        #else:
                        #    pbaver.append(None)
                        retrylimit[dl_idx] = 5
                elif dl_idx != -1:
                    retrylimit[dl_idx] = 0
                #    m.append(None)
                #    if dl_idx != -1:
                #        user1setup.append(None)
                #        pbaver.append(None)
                if TCG_new[dl_idx]:# and ui.msid_list[i] == None:
                    for j in range(256):
                        runop.prelock(j)
                    txt_msid = os.popen(ui.prefix + "sedutil-cli --printDefaultPassword " + full_devs_new[i] ).read()
                    for j in range(256):
                        runop.postlock(j)
                    if txt_msid != '' :
                        regex_msid = 'MSID:\s*(\S+)'
                        mm = re.search(regex_msid, txt_msid)
                        if mm:
                            msid_i = mm.group(1)
                            #print str(dl_idx) + ' ' + msid_i
                if dl_idx != -1:
                    msid[dl_idx] = msid_i
                gobject.idle_add(t_cleanup, i, m, user1setup, pbaver, msid, dl_idx, retrylimit, setup_new, setupstatus_new, pba_new, status_dsr)
                    
            def t_cleanup(i, m, usetup, pbaver, msid, dl_idx, retrylimit, setup_new, setupstatus_new, pba_new, status_dsr):
                if os.path.isfile('dataread' + full_sn_new[i] + '.txt'):
                    try:
                        os.remove('dataread' + full_sn_new[i] + '.txt')
                    except WindowsError:
                        print 'Error: dataread' + full_sn_new[i] + '.txt is somehow not closed'
                #if sedutil_res == hash_res:
                #for i in range(len(ui.devs_list)):
                    #print ui.full_devs_map
                #fdl_idx = ui.full_devs_map.index(i)
                if fullscan:
                    if dl_idx != -1:
                        retrylimit_new[dl_idx] = retrylimit[dl_idx]
                        #print ui.retrylimit_list
                        if opal_ver_new[dl_idx] != 'None' and (setupstatus_new[dl_idx] == None or pba_new[dl_idx] == None or setupuser_new[dl_idx] == None):
                            if datastore_new[dl_idx] != 'Not Supported':
                                if m[i]:
                                    #print str(i) + ' is setup'
                                    setupstatus_new[dl_idx] = 'Yes'
                                    setup_new[dl_idx] = 1
                                    #if not setup_new[dl_idx]:# dl_idx in ui.nonsetup_list:
                                    #    ui.nonsetup_list.remove(dl_idx)
                                    if usetup[dl_idx] == '2':
                                        setupuser_new[dl_idx] = 'Yes'
                                    elif usetup[dl_idx] == '1':
                                        setupuser_new[dl_idx] = 'No'
                                    else:
                                        setupuser_new[dl_idx] = 'Not Supported'
                                    if pbaver[dl_idx] != None:
                                        pv = pbaver[dl_idx].strip('\0')
                                        pba_new[dl_idx] = pv
                                    else:
                                        pba_new[dl_idx] = 'N/A'
                                elif status_dsr[i] == 0:
                                    setupstatus_new[dl_idx] = 'Yes'
                                    setupuser_new[dl_idx] = 'Unknown'
                                    setup_new[dl_idx] = 1
                                    pba_new[dl_idx] = 'Unknown'
                                elif status_dsr[i] != ui.NOT_AUTHORIZED and status_dsr[i] != ui.AUTHORITY_LOCKED_OUT and status_dsr[i] != ui.INVALID_PARAMETER:
                                    #print str(i) + ' may or may not be setup'
                                    setupstatus_new[dl_idx] = 'Unknown'
                                    setupuser_new[dl_idx] = 'Unknown'
                                    setup_new[dl_idx] = 2
                                    pba_new[dl_idx] = 'Unknown'
                                else:
                                    #print str(i) + ' is not setup'
                                    setupstatus_new[dl_idx] = 'No'
                                    setupuser_new[dl_idx] = 'N/A'
                                    setup_new[dl_idx] = 0
                                    if pba_new[dl_idx] == None:
                                        pba_new[dl_idx] = 'N/A'
                            else:
                                setupstatus_new[dl_idx] = 'No'
                                if usetup[dl_idx] == '0':
                                    setupuser_new[dl_idx] = 'Not Supported'
                                else:
                                    setupuser_new[dl_idx] = 'N/A'
                                if pbaver[dl_idx] != None:
                                    pba_new[dl_idx] = pbaver[dl_idx].strip('\0')
                                else:
                                    pba_new[dl_idx] = 'N/A'
                        elif opal_ver_new[dl_idx] == 'None':
                            setupstatus_new[dl_idx] = 'N/A'
                            setupuser_new[dl_idx] = 'N/A'
                            pba_new[dl_idx] = 'N/A'
                        if msid_new[dl_idx] == None:
                            msid_new[dl_idx] = msid[dl_idx]
                            #print msid_new
                    #for j in range(len(ui.full_devs_list)):
                    if full_isSetup_new[i] == None:
                        if m[i]:
                            full_isSetup_new[i] = True
                        else:
                            full_isSetup_new[i] = False
                    #gobject.idle_add(cleanup1, sedutil_res, hash_res)
                                
                #def cleanup1(sedutil_res, hash_res):
                    #if sedutil_res == hash_res:
                    
                    if dl_idx == 0:
                        #ui.dev_setup.set_text(setupstatus_new[dl_idx])
                        #ui.dev_pbaVer.set_text(pba_new[dl_idx])
                        #ui.dev_userSetup.set_text(setupuser_new[dl_idx])
                        if ui.toggleSingle_radio.get_active():
                            if ui.view_state == 4 and setupstatus_new[dl_idx] == 'Yes':
                                ui.op_instr.hide()
                                ui.na_instr.set_text('This drive has already been set up.')
                                ui.na_instr.show()
                                if setupuser_new[dl_idx] != 'Yes':
                                    ui.box_auth.hide()
                                    ui.auth_menu.set_active(0)
                                ui.disable_entries_buttons()
                            elif ui.view_state == 2 and setupstatus_new[dl_idx] == 'No':
                                ui.op_instr.hide()
                                ui.na_instr.set_text('This drive has not been set up.')
                                ui.na_instr.show()
                                ui.box_auth.hide()
                                ui.auth_menu.set_active(0)
                                ui.disable_entries_buttons()
                            elif ui.view_state == 1 and setupuser_new[dl_idx] != 'Yes':
                                ui.box_auth.hide()
                                ui.auth_menu.set_active(0)
                        if ui.toggleMulti_radio.get_active():
                            if (ui.view_state == 2 and setupstatus_new[dl_idx] == 'No') or (ui.view_state == 4 and setupstatus_new[dl_idx] == 'Yes'):
                                ui.mode_toggled(None)
                        #ui.dev_msid.set_text(msid_new[dl_idx])
                else:
                    act_idx = ui.dev_select.get_active()
                    if act_idx == dl_idx:
                        #ui.dev_setup.set_text(ui.setupstatus_list[act_idx])
                        #ui.dev_pbaVer.set_text(ui.pba_list[act_idx])
                        #ui.dev_userSetup.set_text(ui.setupuser_list[act_idx])
                        if ui.toggleSingle_radio.get_active():
                            if ui.view_state == 4 and ui.setupstatus_list[act_idx] == 'Yes':
                                ui.op_instr.hide()
                                ui.na_instr.set_text('This drive has already been set up.')
                                ui.na_instr.show()
                                if ui.setupuser_list[act_idx] != 'Yes':
                                    ui.box_auth.hide()
                                    ui.auth_menu.set_active(0)
                                ui.disable_entries_buttons()
                            elif ui.view_state == 2 and ui.setupstatus_list[act_idx] == 'No':
                                ui.op_instr.hide()
                                ui.na_instr.set_text('This drive has not been set up.')
                                ui.na_instr.show()
                                ui.box_auth.hide()
                                ui.auth_menu.set_active(0)
                                ui.disable_entries_buttons()
                            elif ui.view_state == 1 and ui.setupuser_list[act_idx] != 'Yes':
                                ui.box_auth.hide()
                                ui.auth_menu.set_active(0)
                        if ui.toggleMulti_radio.get_active():
                            if (ui.view_state == 2 and ui.setupstatus_list[act_idx] == 'No') or (ui.view_state == 4 and ui.setupstatus_list[act_idx] == 'Yes'):
                                ui.mode_toggled(None)
                        #ui.dev_msid.set_text(ui.msid_list[act_idx])

            if sedutil_res == hash_res:
                thread_list = [None] * len(full_devs_new)
                
                count = 0
                for d in full_devs_new:
                    thread_list[count] = threading.Thread(target=t_run, args=(count, vendor_old, salt_old, pba_old, msid_old, locked_new, setup_new, TCG_new, not_sure, lockstatus_new, setupstatus_new, blockSID_new, msid_new, pba_new, retrylimit_new, user_new, admin_new, setupuser_new, datastore_new, full_user_new, full_dsSup_new, admin_aol_new, user_aol_new, psid_aol_new, full_isSetup_new, full_isLocked_new, tcg_usb_new))
                    thread_list[count].start()
                    count = count + 1
                    
                    
                for i in range(len(thread_list)):
                    thread_list[i].join()
            
                
                
                
                if ui.DEV_OS == 'Windows':
                    for x in range(256):
                        runop.prelock(x)
                    i = 0
                    #if not fullscan:
                    #    ui.label_list = []
                    full_label_list = []
                    fdl = []
                    fsl = []
                    dl = []
                    ovl = []
                    fdm = []
                    lsl = []
                    if fullscan:
                        fdl = full_devs_new
                        fsl = full_sn_new
                        dl = devs_new
                        ovl = opal_ver_new
                        fdm = fdm_new
                        lsl = lockstatus_new
                    else:
                        fdl = ui.full_devs_list
                        fsl = ui.full_sn_list
                        dl = ui.devs_list
                        ovl = ui.opal_ver_list
                        fdm = ui.full_devs_map
                        lsl = ui.lockstatus_list
                    #print 'fdl'
                    #print fdl
                    for dev in fdl:
                        #print dev
                        num_re = '[0-9]+'
                        m = re.search(num_re, dev)
                        if m:
                            p = subprocess.Popen(["diskpart"], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
                            res1 = p.stdin.write('select disk ' + m.group(0) + '\n')
                            res1 = p.stdin.write('detail disk\n')
                            res1 = p.stdin.write('exit\n')
                            

                            output = p.communicate()[0]
                            #print output
                            vol_re = 'Volume [0-9]+\s+([A-Z])\s+'

                            list_v = re.findall(vol_re, output)
                            #print list_v
                            
                            list_l = ''
                            start = True
                            
                            if fullscan:
                                if len(list_v) == 0:
                                    is_unlocked = False
                                    if dev in dl and (ovl[fdm[i]] == 'None' or lsl[fdm[i]] == 'Unlocked'):
                                        is_unlocked = True
                                    elif dev not in dl or lsl[fdm[i]] == None:
                                        queryText = os.popen(ui.prefix + 'sedutil-cli --query ' + dev).read()
                                        txt_L = "Locked = Y"
                                        m_l = re.search(txt_L, queryText)
                                        if not m_l:
                                            is_unlocked = True
                                    if is_unlocked:
                                    #if ui.lockstatus_list[i] != 'Locked':
                                        txt = os.popen('mountvol').read() #also check whether the UUID is already mounted, in other spots verify whether the UUID is valid, remove if the drive is present and unlocked but the UUID isn't present
                                        mv_idx = 0
                                        #print txt
                                        for v in ui.mv_list:
                                            if v[2] == fsl[i]:
                                                n = re.search(v[0],txt)
                                                if n:
                                                    regex_mounted = v[0] + '}\\\s*\n\s*([A-Z]):'
                                                    v_mounted = re.search(regex_mounted,txt)
                                                    if not v_mounted:
                                                        if not os.path.isdir(v[1]):
                                                            #print 'mountvol ' + v[1] + ' \\\\?\\Volume{' + v[0] + '}\\'
                                                            with open(os.devnull, 'w') as pipe:
                                                                subprocess.call(['mountvol', v[1], '\\\\?\\Volume{' + v[0] + '}\\'], stdout=pipe)#stderr=log)
                                                            
                                                        else:
                                                            dir_idx = 3
                                                            done = False
                                                            while not done:
                                                                if dir_idx == 26:
                                                                    done = True
                                                                dir = ascii_uppercase[dir_idx] + ':'
                                                                if not os.path.isdir(dir):
                                                                    #print 'mountvol ' + dir + ' \\\\?\\Volume{' + v[0] + '}\\'
                                                                    with open(os.devnull, 'w') as pipe:
                                                                        subprocess.call(['mountvol', dir, '\\\\?\\Volume{' + v[0] + '}\\'], stdout=pipe)#stderr=log)
                                                                    
                                                                    mv_edit.append(mv_idx)
                                                                    #v[1] = dir
                                                                    mv_newdir.append(dir)
                                                                    done = True
                                                                if not done:
                                                                    dir_idx = dir_idx + 1
                                                    else:
                                                        if v_mounted.group(1) not in list_v:
                                                            mv_rm.append(mv_idx)
                                                            #print 'marked for removal because this partition is already mounted but is not listed for the drive'
                                                            #print ui.mv_list[mv_idx]
                                                #else:
                                                #    mv_rm.append(mv_idx)
                                                #    print 'marked for removal because UUID is not present even though drive is present'
                                                #    print ui.mv_list[mv_idx]
                                            mv_idx = mv_idx + 1
                                
                                p = subprocess.Popen(["diskpart"], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
                                res1 = p.stdin.write('select disk ' + m.group(0) + '\n')
                                res1 = p.stdin.write('detail disk\n')
                                res1 = p.stdin.write('exit\n')
                                
                                output = p.communicate()[0]
                                
                                
                                list_v = re.findall(vol_re, output)
                            #if dev in ui.devs_list:
                            if len(list_v) > 0:
                                list_l = '('
                            kernel32 = ctypes.windll.kernel32
                            
                            
                            for x in list_v:
                                ui.partition_list.append(x)
                                if not start:
                                    list_l = list_l + ', '
                                else:
                                    start = False
                                buf_name = ctypes.create_unicode_buffer(1024)
                                buf_fsn = ctypes.create_unicode_buffer(1024)
                                sn = None
                                mcl = None
                                fsf = None
                                rc = kernel32.GetVolumeInformationW(ctypes.c_wchar_p(x + ":"), buf_name, ctypes.sizeof(buf_name), sn, mcl, fsf, buf_fsn, ctypes.sizeof(buf_fsn))
                                
                                list_l = list_l + buf_name.value + '(' + x + ':)'
                                full_label_list.append(x + ':')
                           
                            if len(list_v) > 0:
                                list_l = list_l + ')'
                            #print list_l
                            if dev in dl:
                                #print list_l
                                drive_dict[dev] = list_l
                            
                        i = i + 1
                    txt = os.popen('mountvol').read()
                    v_regex = '([a-z0-9]{8}-[a-z0-9]{4}-[a-z0-9]{4}-[a-z0-9]{4}-[a-z0-9]{12})}\\\s*\n\s*\*'
                    mv_regex = '([a-z0-9]{8}-[a-z0-9]{4}-[a-z0-9]{4}-[a-z0-9]{4}-[a-z0-9]{12})}\\\s*\n\s*([A-Z]:)'
                    v_list = re.findall(v_regex, txt)
                    p_list = re.findall(mv_regex, txt)
                    
                    for v in v_list:
                        for e in ui.usb_mv_list:
                            if e[0] == v:
                                if not os.path.isdir(e[1]):
                                    with open(os.devnull, 'w') as pipe:
                                        subprocess.call(['mountvol', e[1], '\\\\?\\Volume{' + e[0] + '}\\'], stdout=pipe)#stderr=log)
                                    
                                else:
                                    dir_idx = 3
                                    done = False
                                    while not done:
                                        if dir_idx == 26:
                                            done = True
                                        dir = ascii_uppercase[dir_idx] + ':'
                                        if not os.path.isdir(dir):
                                            with open(os.devnull, 'w') as pipe:
                                                subprocess.call(['mountvol', dir, '\\\\?\\Volume{' + e[0] + '}\\'], stdout=pipe)#stderr=log)
                                            
                                            usb_edit.append((e[0], dir))
                                            done = True
                    for p in p_list:
                        if p[1] not in full_label_list:
                            usb_edit.append([p[0], p[1]])
                    for x in range(256):
                        runop.postlock(x)

        #else:
        #    ui.setupstatus_list = []
        #    ui.lockstatus_list = []
        #    ui.blockSID_list = []
        #    ui.sn_list = []
        #    ui.salt_list = []
        #    ui.user_list = []
        #    ui.msid_list = []
        #    ui.vendor_list = []
        #    ui.series_list = []
        #    ui.pba_list = []
        #    ui.encsup_list = []
        #    ui.msg_err('No drives detected.')
        
        
        #print drive_dict
        #print 'end of scan_t1'
        gobject.idle_add(scan_cleanup, sedutil_res, hash_res, drive_dict, mv_edit, mv_newdir, mv_rm, usb_edit, devs_new, lockstatus_new, setupstatus_new, blockSID_new, msid_new, pba_new, retrylimit_new, user_new, admin_new, setupuser_new, datastore_new, full_user_new, full_dsSup_new, admin_aol_new, user_aol_new, psid_aol_new, full_isSetup_new, full_isLocked_new, locked_new, setup_new, TCG_new, mbr_new, fdm_new, full_devs_new, opal_ver_new, full_ver_new, encsup_new, vendor_new, series_new, salt_new, sn_new, full_sn_new, full_salt_new, tcg_count, tcg_usb_new)
    
    def scan_cleanup(sedutil_res, hash_res, drive_dict, mv_edit, mv_dir, mv_rm, usb_edit, devs_new, lockstatus_new, setupstatus_new, blockSID_new, msid_new, pba_new, retrylimit_new, user_new, admin_new, setupuser_new, datastore_new, full_user_new, full_dsSup_new, admin_aol_new, user_aol_new, psid_aol_new, full_isSetup_new, full_isLocked_new, locked_new, setup_new, TCG_new, mbr_new, fdm_new, full_devs_new, opal_ver_new, full_ver_new, encsup_new, vendor_new, series_new, salt_new, sn_new, full_sn_new, full_salt_new, tcg_count, tcg_usb_new):
        if sedutil_res != hash_res:
            ui.msg_err('Hash validation failed.')
            gtk.main_quit()
        
        
        
        if fullscan:
            ui.devs_list = devs_new
            ui.lockstatus_list = lockstatus_new
            ui.setupstatus_list = setupstatus_new
            ui.blockSID_list = blockSID_new
            ui.msid_list = msid_new
            ui.pba_list = pba_new
            ui.retrylimit_list = retrylimit_new
            ui.user_list = user_new
            ui.admin_list = admin_new
            ui.setupuser_list = setupuser_new
            ui.datastore_list = datastore_new
            
            ui.full_user_list = full_user_new
            ui.full_dsSup_list = full_dsSup_new
            
            ui.admin_aol_list = admin_aol_new
            ui.user_aol_list = user_aol_new
            ui.psid_aol_list = psid_aol_new
            
            ui.full_isSetup_list = full_isSetup_new
            ui.full_isLocked_list = full_isLocked_new
            
            ui.mbr_list = mbr_new
            ui.full_devs_map = fdm_new
            ui.full_devs_list = full_devs_new
            
            ui.opal_ver_list = opal_ver_new
            ui.full_ver_list = full_ver_new
            ui.encsup_list = encsup_new
            
            ui.vendor_list = vendor_new
            ui.series_list = series_new
            ui.salt_list = salt_new
            ui.sn_list = sn_new
            
            ui.full_sn_list = full_sn_new
            ui.full_salt_list = full_salt_new
            
            ui.tcg_usb_list = tcg_usb_new
            
            ui.mbr_setup_list = []
            ui.usetup_list = []
            ui.ulocked_list = []
            for i in range(len(ui.devs_list)):
                if TCG_new[i] and ui.pba_list[i] != 'Not Supported':
                    ui.mbr_list.append(i)
                if locked_new[i]:
                    ui.locked_list.append(i)
                    ui.setup_list.append(i)
                    ui.tcg_list.append(i)
                    if ui.setupuser_list[i] == 'Yes':
                        ui.ulocked_list.append(i)
                        ui.usetup_list.append(i)
                    if TCG_new[i] and ui.pba_list[i] != 'Not Supported':
                        ui.mbr_setup_list.append(i)
                elif setup_new[i] > 0:
                    ui.setup_list.append(i)
                    if setup_new[i] == 2:
                        ui.nonsetup_list.append(i)
                    ui.tcg_list.append(i)
                    if ui.setupuser_list[i] == 'Yes' or ui.setupuser_list[i] == 'Unknown':
                        ui.usetup_list.append(i)
                    if TCG_new[i] and ui.pba_list[i] != 'Not Supported':
                        ui.mbr_setup_list.append(i)
                elif TCG_new[i] and ui.blockSID_list[i] != 'Enabled':# and not not_sure[i]:
                    ui.nonsetup_list.append(i)
                    ui.tcg_list.append(i)
                elif TCG_new[i]:
                    ui.tcg_list.append(i)
                #elif TCG_new[i]:
                #    ui.tcg_list.append(i)
                #    ui.setup_list.append(i)
                #    ui.nonsetup_list.append(i)
                
            if ui.VERSION != 1 and tcg_count > ui.MAX_DEV and not ui.posthibern:
                ui.msg_err('' + str(tcg_count) + ' TCG drives were detected but your license only supports ' + str(ui.MAX_DEV) + ' TCG drives.  Only ' + str(ui.MAX_DEV) + ' TCG drives are listed.')

            
            if ui.devs_list == []:
                ui.msg_err('No drives detected, try running this application with Administrator.')
                
        
        label_new = []
        if ui.DEV_OS == 'Windows':
            for x in range(len(mv_edit)):
                ui.mv_list[mv_edit[x]][1] = mv_dir[x]
            for y in range(len(usb_edit)):
                matched = False
                for z in range(len(ui.usb_mv_list)):
                    if usb_edit[y][0] == ui.usb_mv_list[z][0]:
                        matched = True
                        if usb_edit[y][1] != ui.usb_mv_list[z][1]:
                            ui.usb_mv_list[z][1] = usb_edit[y][1]
                if not matched:
                    ui.usb_mv_list.append(usb_edit[y])
            #print 'loading label_list'
            label_new = [''] * len(ui.devs_list)
            #print ui.label_list
            label_i = 0
            for dev in ui.devs_list:
                if drive_dict.has_key(dev):
                    label_new[label_i] = drive_dict[dev]
                else:
                    label_new[label_i] = ''
                #print ui.label_list
                label_i = label_i + 1
            #print ui.label_list
            i_rm = len(mv_rm) - 1
            while i_rm >= 0:
                ui.mv_list.pop(mv_rm[i_rm])
                i_rm = i_rm - 1
            #print ui.mv_list
            for i in range(len(ui.full_devs_list)):
                dl_idx = ui.full_devs_map[i]
                is_unlocked = False
                if ui.full_ver_list[i] == False:# or ui.full_isLocked_list[i] == False:
                    is_unlocked = True
                #elif ui.full_isLocked_list[i] == False:
                else:
                    for j in range(256):
                        runop.prelock(j)
                    queryText = os.popen(ui.prefix + 'sedutil-cli --query ' + ui.full_devs_list[i]).read()
                    for j in range(256):
                        runop.postlock(j)
                    txt_UL = "Locked = N"
                    m_ul = re.search(txt_UL, queryText)
                    if m_ul:
                        is_unlocked = True
                if is_unlocked:
                    vol_list = []
                    if dl_idx != -1:
                        #print ui.label_list
                        #print dl_idx
                        #print ui.label_list[dl_idx]
                        if label_new[dl_idx] != '':
                            regex = '[A-Z]:'
                            vol_list = re.findall(regex, label_new[dl_idx])
                    #print vol_list
                    for v in vol_list:
                        if v != 'C:':
                            txt_m = os.popen('mountvol ' + v + ' /l').read()
                            regex_m = '[a-z0-9]{8}-[a-z0-9]{4}-[a-z0-9]{4}-[a-z0-9]{4}-[a-z0-9]{12}'
                            m = re.search(regex_m, txt_m)

                            if m != None:
                                exists = False
                                diffdir = -1
                                idx = 0
                                for e in ui.mv_list:
                                    #if e[0] == m.group(0) and e[1] == v and e[2] == ui.full_sn_list[i]:
                                    #    exists = True
                                    #el
                                    if e[0] == m.group(0) and e[2] == ui.full_sn_list[i]:
                                        #diffdir = idx
                                        exists = True
                                    idx = idx + 1
                                #if diffdir >= 0:
                                    #print 'pop outdated entry'
                                    #print ui.mv_list[diffdir]
                                #    ui.mv_list.pop(diffdir)
                                if not exists:
                                    entry = [m.group(0),v,ui.full_sn_list[i]]
                                    print 'new entry'
                                    print entry
                                    ui.mv_list.append(entry)
                                    #print 'new entry'
                                    #print entry
                elif dl_idx != -1:
                    #print str(dl_idx) + ' setting to none'
                    label_new[dl_idx] = ''
            #print ui.mv_list
            #print ui.label_list
        elif ui.DEV_OS == 'Linux':
            label_new = [''] * len(ui.devs_list)
        ui.label_list = label_new
        old_idx = 0
        if not fullscan:
            old_idx = ui.dev_select.get_active()
            
        model = ui.dev_select.get_model()
        #print ui.label_list
        iter = gtk.TreeIter
        if model != None:
            for row in model:
                model.remove(row.iter)
        for i in range(len(ui.devs_list)):
            if ui.label_list[i] != None:
                ui.dev_select.append_text(ui.devs_list[i] + ' ' + ui.label_list[i])
            else:
                ui.label_list[i] = ''
                ui.dev_select.append_text(ui.devs_list[i])
                
        ui.dev_select.set_active(old_idx)
    
        length = len(ui.devs_list)
        if length > 0:
            ui.dev_vendor.set_text(ui.vendor_list[old_idx])
            ui.dev_sn.set_text(ui.sn_list[old_idx])
            ui.dev_series.set_text(ui.series_list[old_idx])
            if ui.msid_list[old_idx] != None:
                ui.dev_msid.set_text(ui.msid_list[old_idx])
            else:
                ui.dev_msid.set_text('Loading...')
            if ui.pba_list[old_idx] != None:
                ui.dev_pbaVer.set_text(ui.pba_list[old_idx])
            else:
                ui.dev_pbaVer.set_text('Loading...')
            ui.dev_opal_ver.set_text(ui.opal_ver_list[old_idx])
            ui.dev_status.set_text(ui.lockstatus_list[old_idx])
            if ui.setupstatus_list[old_idx] != None:
                ui.dev_setup.set_text(ui.setupstatus_list[old_idx])
            else:
                ui.dev_setup.set_text('Loading...')
            ui.dev_enc.set_text(ui.encsup_list[old_idx])
            if ui.setupuser_list[old_idx] != None:
                ui.dev_userSetup.set_text(ui.setupuser_list[old_idx])
            else:
                ui.dev_userSetup.set_text('Loading...')
            
        ui.stop_spin()
        if not ui.op_inprogress:
            ui.load_instr.hide()
            ui.enable_menu()
        
            if ui.firstscan:
                numTCG = len(ui.tcg_list)
                if numTCG == 0:
                    ui.noTCG_instr.show()
                    ui.main_instr.hide()
                else:
                    ui.noTCG_instr.hide()
                    ui.main_instr.show()
                if ui.VERSION == 1:
                    if len(ui.locked_list) > 0:
                        ui.dev_select.set_active(ui.locked_list[0])
                    ui.unlock_prompt()
            else:
                if fullscan:
                    ui.msg_ok('Rescan complete')
                #print ui.devs_list
                #print ui.locked_list
                #print ui.setup_list
                #print ui.nonsetup_list
                #print ui.tcg_list
                #print ui.msid_list
                if ui.op_prompt == 0:
                    numTCG = len(ui.tcg_list)
                    if numTCG == 0:
                        ui.noTCG_instr.show()
                        ui.main_instr.hide()
                    else:
                        ui.noTCG_instr.hide()
                        ui.main_instr.show()
                if ui.op_prompt == 1:
                    ui.openLog_prompt()
                elif ui.op_prompt == 2:
                    ui.setup_prompt1()
                elif ui.op_prompt == 3:
                    ui.updatePBA_prompt()
                elif ui.op_prompt == 4:
                    ui.changePW_prompt()
                elif ui.op_prompt == 5:
                    ui.setupUSB_prompt()
                elif ui.op_prompt == 6:
                    ui.setupUser_prompt()
                elif ui.op_prompt == 7:
                    ui.unlock_prompt()
                elif ui.op_prompt == 9:
                    ui.revert_keep_prompt()
                elif ui.op_prompt == 10:
                    ui.revert_erase_prompt()
                elif ui.op_prompt == 11:
                    ui.revert_psid_prompt()
                elif ui.op_prompt == 12:
                    ui.removeUser_prompt()
        if ui.firstscan:
            if ui.VERSION == 1 and ui.DEV_OS == 'Linux':
                if ui.VERSION == 1:
                    if len(ui.locked_list) > 0:
                        ui.dev_select.set_active(ui.locked_list[0])
                    ui.unlock_prompt()
                present = False
                if ui.pba_devname != None and not ui.invalid_pba:
                    folder_list = []
                    
                    txt = os.popen("for DLIST in `dmesg  | grep \"Attached SCSI removable disk\" | cut -d\" \" -f 3  | sed -e 's/\[//' -e 's/\]//'` ; do\necho $DLIST\ndone ").read()
                    #print txt
                    txt_regex = 'sd[a-z]'
                    list_u = re.findall(txt_regex,txt)
                    #print list_u
                    for u in list_u:
                        txt1 = os.popen(ui.prefix + 'mount').read()
                        m = re.search(u,txt1)
                        if not m:
                            txt2 = os.popen(ui.prefix + 'blkid').read()
                            rgx = '(' + u + '1?).+'
                            m1 = re.search(rgx,txt2)
                            if m1:
                                r2 = '\s+TYPE="([a-z]+)"'
                                txt3 = m1.group(0)
                                m2 = re.search(r2,txt3)
                                type_a = m2.group(1)
                                if type_a != 'ntfs' and type_a != 'exfat':
                                    s = os.system(ui.prefix + 'mount -t ' + type_a + ' /dev/' + m1.group(1))
                    
                    txt = os.popen(ui.prefix + 'mount').read()
                    dev_regex = '/dev/sd[a-z][1-9]?\s*on\s*(\S+)\s*type'
                    drive_list = re.findall(dev_regex, txt)
                    for d in drive_list:
                        if os.path.isdir('%s/OpalLock' % d):
                            folder_list.append(d)
                    
                    if folder_list != []:
                        
                        for i0 in folder_list:
                            for i in ui.locked_list:
                                fp = i0 + '/OpalLock/' + ui.vendor_list[i] + '_' + ui.sn_list[i] + '.psw'
                                if os.path.isfile(fp):
                                    present = True
                

                if ui.invalid_pba:
                    if len(ui.locked_list) == 0:
                        ui.msg_err('No locked drives, cannot verify the preboot image.')
                    else:
                        ui.msg_err('Invalid preboot image.')
                    ui.PBA_VERSION = 0
                elif present and ui.PBA_VERSION > 1:
                    message = gtk.MessageDialog(type=gtk.MESSAGE_QUESTION, buttons=gtk.BUTTONS_CANCEL, parent = ui)
                    msg = 'USB detected, Proceeding to unlock with USB in 5 seconds.'
                    message.set_markup(msg)
                    timer = gobject.timeout_add(5000, runop.run_unlockPBA, None, ui, True, True, message)
                    
                    res = message.run()
                    if res == gtk.RESPONSE_CANCEL :
                        gobject.source_remove(timer)
                    message.destroy()
                    timer = 0
                            
            
            if ui.VERSION == 0 or ui.PBA_VERSION == 0:
                ui.buttonBox.set_sensitive(False)
                demo_msg = gtk.MessageDialog(type=gtk.MESSAGE_INFO, buttons=gtk.BUTTONS_OK, parent = ui)
                msg = 'You are running the demo version, with most capabilities disabled.  To unlock additional capabilities, upgrade your license.'
                demo_msg.set_markup(msg)
                
                demo_msg.run()
                demo_msg.destroy()
            
            if not ui.up_to_date:
                message = gtk.MessageDialog(type=gtk.MESSAGE_QUESTION, buttons=gtk.BUTTONS_YES_NO, parent = ui)
                message.set_markup("A new version of Opal Lock is available, would you like to update now?")
                res = message.run()
                message.destroy()
                if res == gtk.RESPONSE_YES:
                    os.system("start \"\" https://fidelityheight.com/download/OpalLock_setup.exe")
                    #gtk.main_quit()
                else:
                    ui.update_link.show()
            
            if ui.DEV_OS == 'Windows':
                verified = powerset.verify_power()
                if len(ui.tcg_list) > 0 and not verified:
                    dialogs.mngPower_prompt(None, ui, 0)
            ui.firstscan = False
        ui.driveCount.set_text("Total Drives : " + str(len(ui.devs_list)))
        ui.tcgCount.set_text("Total TCG Drives : " + str(len(ui.tcg_list)))
        ui.externalCount.set_text("Total External Drives: " + str(len(ui.tcg_usb_list)))
        ui.scan_ip = False
        
        ui.posthibern = False
    
    t_scan = threading.Thread(target=scan_t1, args=())
    t_scan.start()