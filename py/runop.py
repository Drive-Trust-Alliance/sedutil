import datetime
import gobject
import gtk
import lockhash
import os
import platform
import powerset
import re
from string import ascii_uppercase
import subprocess
import sys
import threading
import time

def findos(ui):
    if sys.platform == "linux" or sys.platform == "linux2":
        ui.ostype = 1
        ui.prefix = "sudo "
    elif sys.platform == "darwin":
        ui.ostype = 2
        ui.prefix = "sudo "
    elif sys.platform == "win32":
        ui.ostype = 0
        ui.prefix = ""
    
def finddev(ui):
    txt = os.popen(ui.prefix + 'sedutil-cli --scan n').read()
    print txt
 
    names = ['PhysicalDrive[0-9]', '/dev/sd[a-z]', '/dev/nvme[0-9]',  '/dev/disk[0-9]']
    idx = 0
    ui.devs_list = []
    ui.locked_list = []
    ui.setup_list = []
    ui.unlocked_list = []
    ui.nonsetup_list = []
    ui.tcg_list = []
    
    vendor_new = []
    salt_new = []
    sn_new = []
    msid_new = []
    pba_new = []
    
    tcg_count = 0
    
    for index in range(len(names)): #index=0(window) 1(Linux) 2(OSX)
    
        m = re.search(names[index] + ".*", txt)
        
        if m:
            
            if (index == 0 ):
               ui.prefix = ""
            else:
               ui.prefix = "sudo "
            
            txt11 = names[index] + " .[A-z0-9]+ *[A-z0-9].*"
            m1 = re.findall(txt11, txt)
            if m1:
                for tt in m1:
                    m2 = re.match(names[index],tt)
                    if (index == 0) : 
                        ui.devname = "\\\\.\\" + m2.group()
                    else:
                        ui.devname =  m2.group()
                    ui.devs_list.append(ui.devname)
                    rgx = '\S+\s*([12ELP]+|No)\s*(\S+(?:\s\S+)*)\s*:\s*([^:]+)\s*:(.+)'
                    md = re.match(rgx,tt)
                    if md:
                        if md.group(1) == 'No':
                            ui.opal_ver_list.append("None")
                        else:
                            tcg_count = tcg_count + 1
                            if tcg_count <= ui.MAX_DEV:
                                if md.group(1) == '1' or md.group(1) == '2':
                                    ui.opal_ver_list.append("Opal " + md.group(1) + ".0")
                                elif md.group(1) == '12':
                                    ui.opal_ver_list.append("Opal 1.0/2.0")
                                elif md.group(1) == 'E':
                                    ui.opal_ver_list.append("Enterprise")
                                elif md.group(1) == 'L':
                                    ui.opal_ver_list.append("Opallite")
                                elif md.group(1) == 'P':
                                    ui.opal_ver_list.append("Pyrrite")
                                elif md.group(1) == 'R':
                                    ui.opal_ver_list.append("Ruby")
                                else:
                                    ui.opal_ver_list.append(md.group(1))
                        vendor_new.append(md.group(2))
                        ui.series_list.append(md.group(3))
                        salt_new.append(md.group(4).ljust(20))
                        sn_new.append(md.group(4).replace(' ',''))
                    else:
                        ui.opal_ver_list.append("None")
                        vendor_new.append('N/A')
                        ui.series_list.append('N/A')
                        salt_new.append('N/A')
                        sn_new.append('N/A')
                        
    if ui.devs_list != []:
        for i in range(len(ui.devs_list)):
            queryText = os.popen(ui.prefix + 'sedutil-cli --query ' + ui.devs_list[i]).read()
            
            old_idx = -1
            for j in range(len(ui.vendor_list)):
                if ui.vendor_list[j] == vendor_new[i] and ui.sn_list[j] == sn_new[i] and ui.salt_list[j] == salt_new[i]:
                    old_idx = j
                    
            msid = 'N/A'
            
            txt_TCG = "Locked = "
            isTCG = re.search(txt_TCG, queryText)
            if isTCG:
            
                if old_idx >= 0:
                    msid = ui.msid_list[old_idx]
                    msid_new.append(ui.msid_list[old_idx])
                else:
                    txt_msid = os.popen(ui.prefix + "sedutil-cli --printDefaultPassword " + ui.devs_list[i] ).read()
                    
                    if txt_msid != '' :
                        regex_msid = 'MSID:\s*([A-z0-9]*)'
                        mm = re.search(regex_msid, txt_msid)
                        if mm:
                            msid = mm.group(1)
                    
                    msid_new.append(msid)
                
                regex_users = "Locking Users = ([0-9]+)"
                m_user = re.search(regex_users, queryText)
                ui.user_list.append(m_user.group(1))
                
                txt_MBRSup = "MBR shadowing Not Supported = N"
                txt_BSID = "BlockSID"
                txt_BSID_enabled = "BlockSID_BlockSIDState = 0x001"
                
                isSup = re.search(txt_MBRSup, queryText)
                hasBlockSID = re.search(txt_BSID, queryText)
                isBlockSID = re.search(txt_BSID_enabled, queryText)
                
                
                txt_L = "Locked = Y"
                txt_S = "LockingEnabled = Y"
                #msidText = os.popen(ui.prefix + 'sedutil-cli -n -t --getmbrsize ' + msid + ' ' + ui.devs_list[i]).read()
                #m = re.search('Shadow', msidText)
                pwd = 'F0iD2eli81Ty' + salt_new[i]
                
                hash_pwd = lockhash.hash_pass(pwd, salt_new[i], msid_new[i])
                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                timeStr = timeStr[2:]
                auditText = os.popen(ui.prefix + 'sedutil-cli -n -t -u --auditread ' + hash_pwd + ' User' + m_user.group(1) + ' ' + ui.devs_list[i]).read()
                m = re.search('Fidelity Audit Log', auditText)
                
                isLocked = re.search(txt_L, queryText)
                isSetup = (re.search(txt_S, queryText) != None) and m
            
                if isLocked:
                    ui.locked_list.append(i)
                    ui.setup_list.append(i)
                    ui.tcg_list.append(i)
                    ui.lockstatus_list.append("Locked")
                    ui.setupstatus_list.append("Yes")
                elif isSetup:
                    ui.setup_list.append(i)
                    ui.unlocked_list.append(i)
                    ui.tcg_list.append(i)
                    ui.lockstatus_list.append("Unlocked")
                    ui.setupstatus_list.append("Yes")
                else:
                    ui.nonsetup_list.append(i)
                    ui.tcg_list.append(i)
                    ui.lockstatus_list.append("Unlocked")
                    ui.setupstatus_list.append("No")
                    
                if isSup and old_idx < 0:
                    pba_new.append("N/A")
                elif old_idx >= 0:
                    pba_new.append(ui.pba_list[old_idx])
                else:
                    pba_new.append("Not Supported")
                    
                if hasBlockSID and isBlockSID:
                    ui.blockSID_list.append("Enabled")
                elif hasBlockSID:
                    ui.blockSID_list.append("Disabled")
                else:
                    ui.blockSID_list.append("Not Supported")
            else:
                msid_new.append("N/A")
                ui.user_list.append("N/A")
                ui.lockstatus_list.append("N/A")
                ui.setupstatus_list.append("N/A")
                pba_new.append("N/A")
                ui.blockSID_list.append("N/A")
                ui.user_list.append('')
        
        
    ui.msid_list = msid_new
    ui.vendor_list = vendor_new
    ui.salt_list = salt_new
    ui.sn_list = sn_new
    ui.pba_list = pba_new
    print ("devs_list: ",  ui.devs_list)
    print ("vendor_list: ", ui.vendor_list)
    print ("opal_ver_list: ", ui.opal_ver_list)
    print ("salt_list: ", ui.salt_list)	
    
    if tcg_count > ui.MAX_DEV:
        ui.msg_err('' + str(tcg_count) + ' TCG drives were detected but your license only supports ' + str(ui.MAX_DEV) + ' TCG drives.  Only the first ' + str(ui.MAX_DEV) + ' TCG drives are listed.')

def run_setupFull(button, ui):
    if ui.warned and ui.orig != ui.new_pass_entry.get_text():
        ui.orig = ''
        ui.warned = False
        
        ui.msg_err('The passwords entered do not match. Try again.')
        ui.op_instr.set_text('Setting up a drive includes setting a password which you can use to unlock the drive.\nEnter the new password for the drive and click \'Continue\'.')
        return
    
    pw = ui.new_pass_entry.get_text()
    pw_confirm = ui.confirm_pass_entry.get_text()
    pw_trim = re.sub('\s', '', pw)
    pw_trim_confirm = re.sub(r'\s+', '', pw_confirm)
    if len(pw_trim) < 8:
        ui.msg_err("This password is too short.  Please enter a password at least 8 characters long excluding whitespace.")
    elif ui.bad_pw.has_key(pw_trim):
        ui.msg_err("This password is on the blacklist of bad passwords, please enter a stronger password.")
    elif pw_trim != pw_trim_confirm:
        ui.msg_err("The entered passwords do not match.")
    else:
        if not ui.warned:
            message = gtk.MessageDialog(type=gtk.MESSAGE_WARNING, buttons=gtk.BUTTONS_YES_NO, parent = ui)
            message.set_markup("Warning: If you lose your password, all data will be lost. Do you want to proceed?")
            res = message.run()
            if res == gtk.RESPONSE_YES:
                message.destroy()
                ui.warned = True
                ui.orig = ui.new_pass_entry.get_text()
                ui.new_pass_entry.get_buffer().delete_text(0,-1)
                ui.confirm_pass_entry.get_buffer().delete_text(0,-1)
                ui.op_instr.set_text('Re-enter and re-confirm the password to verify.')
                ui.dev_single.show()
                ui.label_dev2.show()
                ui.dev_select.hide()
                ui.label_dev.hide()
            else:
                message.destroy()
            return
                
        else :
            ui.warned = False
        selected_list = []
        if ui.toggleSingle_radio.get_active():
            index = ui.dev_select.get_active()
            selected_list = [index]
        else:
            index = 0
            selected_list = []
            iter = ui.liststore.get_iter_first()
            while iter != None:
                selected = ui.liststore.get_value(iter, 0)
                if selected:
                    selected_list.append(index)
                iter = ui.liststore.iter_next(iter)
                index = index + 1

        del pw_trim
        del pw_trim_confirm
        message2 = gtk.MessageDialog(type=gtk.MESSAGE_WARNING, buttons=gtk.BUTTONS_OK_CANCEL, parent = ui)
        message2.set_markup("Final Warning: If you lose your password, all data will be lost. Are you sure you want to proceed?")
        res2 = message2.run()
        if res2 == gtk.RESPONSE_OK:
            message2.destroy()
            ui.start_spin()
            ui.wait_instr.show()
            
            def t1_run(e):
                fail_list = []
                success_list = []
                notify_TO = False
                for i in selected_list:
                    if e.isSet():
                        fail_list.append(i)
                        notify_TO = True
                    else:
                        ui.devname = ui.devs_list[ui.nonsetup_list[i]]
                        password = lockhash.hash_pass(ui.new_pass_entry.get_text(), ui.salt_list[ui.nonsetup_list[i]], ui.msid_list[ui.nonsetup_list[i]])
                        if e.isSet():
                            fail_list.append(i)
                            notify_TO = True
                            continue
                        queryText = os.popen(ui.prefix + 'sedutil-cli --query ' + ui.devname).read()
                        if e.isSet():
                            fail_list.append(i)
                            notify_TO = True
                            continue
                        txt_LE = "LockingEnabled = N"
                        txt_ME = "MBREnabled = N"
                        unlocked = re.search(txt_LE, queryText)
                        activated = re.search(txt_ME, queryText)
                        status1 = -1
                        status2 = -1
                        if unlocked:
                            status1 =  os.system(ui.prefix + "sedutil-cli -n -t --initialSetup " + password + " " + ui.devname )
                            if e.isSet():
                                if not status1:
                                    s1 =  os.system(ui.prefix + "sedutil-cli -n -t --revertnoerase " + password + " " + ui.devname)
                                    s1 =  os.system(ui.prefix + "sedutil-cli -n -t --setsidpassword " + password + " " + ui.msid_list[ui.nonsetup_list[i]] + " " + ui.devname)
                                    s1 =  os.system(ui.prefix + "sedutil-cli -n -t --activate " + ui.msid_list[ui.nonsetup_list[i]] + " " + ui.devname)
                                fail_list.append(i)
                                notify_TO = True
                                continue
                            if status1 == 0:
                                status2 =  os.system(ui.prefix + "sedutil-cli -n -t --enableLockingRange " + ui.LKRNG + " " + password + " " + ui.devname )
                                
                                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                                timeStr = timeStr[2:]
                                statusAW3 = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 02" + timeStr + " " + password + " Admin1 " + ui.devname)
                            
                                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                                timeStr = timeStr[2:]
                                statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 11" + timeStr + " " + password + " Admin1 " + ui.devname)
                                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                                timeStr = timeStr[2:]
                                statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 12" + timeStr + " " + password + " Admin1 " + ui.devname)
                        elif activated:
                            s1 = os.system(ui.prefix + "sedutil-cli -n -t --setSIDPassword " + ui.msid_list[ui.nonsetup_list[i]] + " " + password + " " + ui.devname)
                            if e.isSet():
                                if not s1:
                                    s1 = os.system(ui.prefix + "sedutil-cli -n -t --setSIDPassword " + password + " " + ui.msid_list[ui.nonsetup_list[i]] + " " + ui.devname)
                                fail_list.append(i)
                                notify_TO = True
                                continue
                            if s1 == 0:
                                s2 = os.system(ui.prefix + "sedutil-cli -n -t --setAdmin1Pwd " + ui.msid_list[ui.nonsetup_list[i]] + " " + password + " " + ui.devname)
                                if e.isSet():
                                    if not s2:
                                        s1 = os.system(ui.prefix + "sedutil-cli -n -t --setSIDPassword " + password + " " + ui.msid_list[ui.nonsetup_list[i]] + " " + ui.devname)
                                        s2 = os.system(ui.prefix + "sedutil-cli -n -t --setAdmin1Pwd " + password + " " + ui.msid_list[ui.nonsetup_list[i]] + " " + ui.devname)
                                    fail_list.append(i)
                                    notify_TO = True
                                    continue
                                status1 = (s1 | s2)
                                status2 =  os.system(ui.prefix + "sedutil-cli -n -t --enableLockingRange " + ui.LKRNG + " " + password + " " + ui.devname )
                                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                                timeStr = timeStr[2:]
                                statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 11" + timeStr + " " + password + " Admin1 " + ui.devname)
                                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                                timeStr = timeStr[2:]
                                statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 12" + timeStr + " " + password + " Admin1 " + ui.devname)
                            
                        
                        #status3 =  os.system(ui.prefix + "sedutil-cli -n -t --setMBREnable on " + password + " " + ui.devname )
                        #status4 =  os.system(ui.prefix + "sedutil-cli -n -t --setMBRdone on " + password + " " + ui.devname )
                        if (status1 | status2):
                            fail_list.append(i)
                        else:
                            success_list.append(i)
                ui.confirm_pass_entry.get_buffer().delete_text(0,-1)
                gobject.idle_add(cleanup, success_list, fail_list, password, notify_TO)
            
            def cleanup(list_s, list_f, password, op_to):
                ui.stop_spin()
                t1.join()
                if len(list_f) > 0 :
                    if op_to:
                        ui.msg_err("Operation timed out")
                    for i in list_s:
                        ui.setupstatus_list[ui.nonsetup_list[i]] = "Yes"
                    liststr = ''
                    start = True
                    for j in list_f:
                        if start:
                            liststr = liststr + ', '
                            start = False
                        liststr = liststr + ui.devs_list[ui.nonsetup_list[j]]
                    ui.msg_err("Error: Setup of " + liststr + " failed.")
                    ui.op_instr.show()
                    ui.box_newpass.show()
                    ui.box_newpass_confirm.show()
                    ui.check_box_pass.show()
                    ui.setup_next.show()
                    ui.cancel_button.show()
                else :
                    liststr = ''
                    start = True
                    for i in list_s:
                        ui.devname = ui.devs_list[ui.nonsetup_list[i]]
                        ui.setupstatus_list[ui.nonsetup_list[i]] = "Yes"
                        if start:
                            liststr = liststr + ', '
                            start = False
                        liststr = liststr + ui.devname
                        if ui.VERSION == 3 and ui.pass_sav.get_active():
                            print "setupFull passSaveUSB " + password + " " + ui.auth_menu.get_active_text()
                            passSaveUSB(ui, password, ui.drive_menu.get_active_text(), ui.vendor_list[ui.nonsetup_list[i]], ui.sn_list[ui.nonsetup_list[i]])
                            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                            timeStr = timeStr[2:]
                            statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 18" + timeStr + " " + password + " Admin1 " + ui.devname)
                        #status5 =  os.system(ui.prefix + "sedutil-cli -n -t --enableLockingRange " + ui.LKRNG + " " + password + " " + ui.devname )
                        #status7 =  os.system(ui.prefix + "sedutil-cli -n -t --setMBRDone on " + password + " " + ui.devname )
                        ui.query(None,1)
                    ui.msg_ok("Password set up successfully for " + liststr + ".")
                    
                    ui.setup_prompt2(list_s)
                        
            e = threading.Event()
            t1 = threading.Thread(target=t1_run, args=(e,))
            t1.start()
            start_time = time.time()
            t2 = threading.Thread(target=timeout_track, args=(ui, 30.0 * len(selected_list), start_time, t1, e))
            t2.start()
            
        else:
            message2.destroy()
            
def run_setupPBA(button, ui):
    if ui.mbr_radio.get_active():
        run_pbaWrite(None, ui, 1)
    elif ui.usb_radio.get_active():
        run_setupUSB(None, ui)
    elif ui.skip_radio.get_active():
        message1 = gtk.MessageDialog(type=gtk.MESSAGE_WARNING, buttons=gtk.BUTTONS_OK_CANCEL, parent = ui)
        if ui.dev_pbaVer.get_text() == 'N/A':
            message1.set_markup("Warning: If you do not have the Preboot Image set up or another drive that can unlock this drive, you will not be able to unlock the drive.  Are you sure you want to proceed?")
        else:
            message1.set_markup("Warning: If you do not have a bootable USB set up to unlock the drive then you will not be able to unlock the drive.  Are you sure you want to proceed?")
        res1 = message1.run()
        message1.destroy()
        if res1 == gtk.RESPONSE_OK:
            ui.setup_finish()

def run_pbaWrite(button, ui, mode):
    status = -1
    password = ""
    index = ui.dev_select.get_active()
    dev_idx = -1
    if mode == 0:
        dev_idx = ui.setup_list[index]
        ui.devname = ui.devs_list[dev_idx]
        if ui.VERSION == 3 and ui.check_pass_rd.get_active():
            password = passReadUSB(ui, ui.dev_vendor.get_text(), ui.dev_sn.get_text())
            if password == None or password == 'x':
                ui.msg_err('No password found for the drive.')
                return
        else:
            password = lockhash.hash_pass(ui.pass_entry.get_text(), ui.salt_list[dev_idx], ui.dev_msid.get_text())
            ui.pass_entry.get_buffer().delete_text(0,-1)
    else:
        dev_idx = ui.nonsetup_list[ui.sel_list[index]]
        ui.devname = ui.dev_select.get_active_text()
        password = lockhash.hash_pass(ui.new_pass_entry.get_text(), ui.salt_list[dev_idx], ui.msid_list[dev_idx])
    ui.start_spin()
    ui.pba_wait_instr.show()
    
    def t1_run():
        if password != None:
            ui.process = subprocess.Popen( [ui.prefix + "sedutil-cli", "-n", "-t", "--loadpbaimage", password, "n", ui.devname] )
            ui.process.communicate()
            rc = ui.process.returncode
            gobject.idle_add(cleanup, rc)
        else:
            gobject.idle_add(cleanup, 1)
        
        
    def cleanup(status):
        ui.stop_spin()
        t1.join()
        index = ui.dev_select.get_active()
        if mode == 0:
            dev_idx = ui.setup_list[index]
        else:
            dev_idx = ui.nonsetup_list[ui.sel_list[index]]
            print dev_idx
        if status != 0 :
            dev_idx = -1
            
            if status == ui.NOT_AUTHORIZED or status == ui.AUTHORITY_LOCKED_OUT:
                pwd = 'F0iD2eli81Ty' + ui.salt_list[dev_idx]
                hash_pwd = lockhash.hash_pass(pwd, ui.salt_list[dev_idx], ui.msid_list[dev_idx])
                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                timeStr = timeStr[2:]
                statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 04" + timeStr + " " + hash_pwd + " User" + ui.user_list[dev_idx] + " " + ui.devname)
                if status == ui.AUTHORITY_LOCKED_OUT:
                    statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 10" + timeStr + " " + hash_pwd + " User" + ui.user_list[dev_idx] + " " + ui.devname)
                    ui.msg_err("Error: Writing PBA image to " + ui.devname + " failed. Retry limit reached, please power cycle your drive to try again.")
                else:
                    ui.msg_err("Error: Writing PBA image to " + ui.devname + " failed. Invalid password.")
            else:
                ui.msg_err("Error: Writing PBA image to " + ui.devname + " failed.")
            
            ui.op_instr.show()
            if mode == 1:
                ui.setupSelect.show()
            else:
                ui.box_pass.show()
                ui.check_box_pass.show()
                ui.updatePBA_button.show()
                ui.cancel_button.show()
        else :
            status2 =  os.system(ui.prefix + "sedutil-cli -n -t --setMBREnable on " + password + " " + ui.devname )
            status1 =  os.system(ui.prefix + "sedutil-cli -n -t --setMBRDone on " + password + " " + ui.devname )
            p = os.popen(ui.prefix + "sedutil-cli -n -t --pbaValid " +  password + " " + ui.devname).read()
            pba_regex = 'PBA image version\s*:\s*(.+)'
            m1 = re.search(pba_regex, p)
            if m1:
                if ui.VERSION % 2 == 1 and mode == 0 and ui.pass_sav.get_active():
                    passSaveUSB(ui, password, ui.drive_menu.get_active_text(), ui.dev_vendor.get_text(), ui.dev_sn.get_text())
                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                    timeStr = timeStr[2:]
                    statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 18" + timeStr + " " + password + " Admin1 " + ui.devname)
                pba_ver = m1.group(1)
                ui.msg_ok("PBA image " + pba_ver + " written to " + ui.devname + " successfully.")
                if mode == 0:
                    ui.pba_list[dev_idx] = pba_ver
                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                    timeStr = timeStr[2:]
                    statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 03" + timeStr + " " + password + " Admin1 " + ui.devname)
                    statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 08" + timeStr + " " + password + " Admin1 " + ui.devname)
                else:
                    ui.pba_list[dev_idx] = pba_ver
            if mode == 1:
                ui.setup_finish()
            else:
                ui.returnToMain()
    
    e = threading.Event()
    t1 = threading.Thread(target=t1_run, args=())
    t1.start()
    start_time = time.time()
    t2 = threading.Thread(target=timeout_track, args=(ui, 450.0, start_time, t1, e))
    t2.start()

def run_changePW(button, ui):
    selected_list = []
    if ui.toggleSingle_radio.get_active():
        index = ui.dev_select.get_active()
        selected_list = [index]
    else:
        index = 0
        selected_list = []
        iter = ui.liststore.get_iter_first()
        while iter != None:
            selected = ui.liststore.get_value(iter, 0)
            if selected:
                selected_list.append(index)
            iter = ui.liststore.iter_next(iter)
            index = index + 1

    old_hash = ""
    new_pass = ui.new_pass_entry.get_text()
    new_pass_confirm = ui.confirm_pass_entry.get_text()
    
    pw_trim = re.sub('\s', '', new_pass)
    pw_trim_confirm = re.sub(r'\s+', '', new_pass_confirm)
    if len(pw_trim) < 8:
        ui.msg_err("The new password is too short.  Please enter a password at least 8 characters long excluding whitespace.")
    elif ui.bad_pw.has_key(pw_trim):
        ui.msg_err("The new password is on the blacklist of weak passwords, please enter a stronger password.")
    elif pw_trim != pw_trim_confirm:
        ui.msg_err("The new entered passwords do not match.")
    else:
        ui.start_spin()
        ui.wait_instr.show()
        level = ui.auth_menu.get_active()
        def t1_run(e):
            list_s = []
            list_f = []
            notify_TO = False
            for index in selected_list:
                if e.isSet():
                    list_f.append(index)
                    notify_TO = True
                    continue
                
                ui.devname = ui.devs_list[ui.setup_list[index]]
                if ui.VERSION % 2 == 1 and ui.check_pass_rd.get_active():
                    old_hash = passReadUSB(ui, ui.vendor_list[ui.setup_list[index]], ui.sn_list[ui.setup_list[index]])
                    if old_hash == None or old_hash == 'x':
                        list_f.append(index)
                else:
                    old_pass = ui.pass_entry.get_text()
                    #ui.pass_entry.get_buffer().delete_text(0,-1)
                    old_trim = re.sub('\s', '', old_pass)
                    del old_pass
                    old_hash = lockhash.hash_pass(old_trim, ui.salt_list[ui.setup_list[index]], ui.msid_list[ui.setup_list[index]])
                    del old_trim
                if e.isSet():
                    list_f.append(index)
                    notify_TO = True
                    continue
                if old_hash != None and old_hash != 'x':
                    new_hash = lockhash.hash_pass(ui.new_pass_entry.get_text(), ui.salt_list[ui.setup_list[index]], ui.msid_list[ui.setup_list[index]])
                    if e.isSet():
                        list_f.append(index)
                        notify_TO = True
                        continue
                    status = -1
                    if ui.VERSION % 2 == 1 and level == 1:
                        status = os.system(ui.prefix + "sedutil-cli -n -t -u --setpassword " + old_hash + " User1 " + new_hash + " " + ui.devname)
                        if e.isSet():
                            if status == 0:
                                status = os.system(ui.prefix + "sedutil-cli -n -t -u --setpassword " + new_hash + " User1 " + old_hash + " " + ui.devname)
                            list_f.append(index)
                            notify_TO = True
                            continue
                    else:
                        status1 = os.system(ui.prefix + "sedutil-cli -n -t --setSIDPassword " + old_hash + " " + new_hash + " " + ui.devname)
                        if e.isSet():
                            if status == 0:
                                status1 = os.system(ui.prefix + "sedutil-cli -n -t --setSIDPassword " + new_hash + " " + old_hash + " " + ui.devname)
                            list_f.append(index)
                            notify_TO = True
                            continue
                        if status1 == 0:
                            status2 = os.system(ui.prefix + "sedutil-cli -n -t --setAdmin1Pwd " + old_hash + " " + new_hash + " " + ui.devname)
                            if e.isSet():
                                if status2 == 0:
                                    status2 = os.system(ui.prefix + "sedutil-cli -n -t --setAdmin1Pwd " + new_hash + " " + old_hash + " " + ui.devname)
                                    status1 = os.system(ui.prefix + "sedutil-cli -n -t --setSIDPassword " + new_hash + " " + old_hash + " " + ui.devname)
                                list_f.append(index)
                                notify_TO = True
                                continue
                            if status2 == ui.AUTHORITY_LOCKED_OUT or status2 == ui.NOT_AUTHORIZED:
                                status1 = os.system(ui.prefix + "sedutil-cli -n -t --setSIDPassword " + new_hash + " " + old_hash + " " + ui.devname)
                                list_f.append(index)
                            else:
                                list_s.append(index)
                                if ui.VERSION % 2 == 1 and ui.pass_sav.get_active():
                                    passSaveUSB(ui, new_hash, ui.drive_menu.get_active_text(), ui.vendor_list[ui.setup_list[index]], ui.sn_list[ui.setup_list[index]])
                                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                                    timeStr = timeStr[2:]
                                    if ui.VERSION % 2 == 1 and level == 1:
                                        statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 18" + timeStr + " " + new_hash + " User1 " + ui.devs_list[ui.setup_list[index]])
                                    else:
                                        statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 18" + timeStr + " " + new_hash + " Admin1 " + ui.devs_list[ui.setup_list[index]])
                                if ui.auth_menu.get_active() != 1 and ui.pba_list[ui.setup_list[index]] == 'N/A':
                                    p = ''
                                    p = os.popen(ui.prefix + "sedutil-cli -n -t --pbaValid " + new_hash + " " + ui.devname).read()
                                    pba_regex = 'PBA image version\s*:\s*(.+)'
                                    m1 = re.search(pba_regex, p)
                                    if m1:
                                        pba_ver = m1.group(1)
                                        ui.pba_list[ui.setup_list[index]] = pba_ver
                                if ui.VERSION % 2 == 1 and level == 1:
                                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                                    timeStr = timeStr[2:]
                                    statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 05" + timeStr + " " + new_hash + " User1 " + ui.devs_list[ui.setup_list[index]])
                                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                                    timeStr = timeStr[2:]
                                    statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 13" + timeStr + " " + new_hash + " User1 " + ui.devs_list[ui.setup_list[index]])
                                else:
                                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                                    timeStr = timeStr[2:]
                                    statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 03" + timeStr + " " + new_hash + " Admin1 " + ui.devs_list[ui.setup_list[index]])
                                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                                    timeStr = timeStr[2:]
                                    statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 11" + timeStr + " " + new_hash + " Admin1 " + ui.devs_list[ui.setup_list[index]])
                                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                                    timeStr = timeStr[2:]
                                    statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 12" + timeStr + " " + new_hash + " Admin1 " + ui.devs_list[ui.setup_list[index]])
                        else:
                            list_f.append(index)
            gobject.idle_add(cleanup, list_s, list_f, notify_TO)
            
        def cleanup(list_s, list_f, op_to):
            t1.join()
            ui.stop_spin()
            dialog_str = ''
            
            if len(list_f) > 0:
                if op_to:
                    ui.msg_err('Operation timed out.')
                dialog_str = 'Change password failed for the following drives: '
                start = True
                for i in list_f:
                    if not start:
                        dialog_str = dialog_str + ', '
                    else:
                        start = False
                    dialog_str = dialog_str + ui.devs_list[ui.setup_list[i]]
                    pwd = 'F0iD2eli81Ty' + ui.salt_list[ui.setup_list[i]]
                    hash_pwd = lockhash.hash_pass(pwd, ui.salt_list[ui.setup_list[i]], ui.msid_list[ui.setup_list[i]])
                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                    timeStr = timeStr[2:]
                    if ui.auth_menu.get_active() == 0:
                        statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 04" + timeStr + " " + hash_pwd + " User" + ui.user_list[ui.setup_list[i]] + " " + ui.devs_list[ui.setup_list[i]])
                    else:
                        statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 06" + timeStr + " " + hash_pwd + " User" + ui.user_list[ui.setup_list[i]] + " " + ui.devs_list[ui.setup_list[i]])
                if len(list_s) > 0:
                    dialog_str = dialog_str + '\nPassword changed successfully for the following drives: '
                    start = True
                    for j in list_s:
                        if not start:
                            dialog_str = dialog_str + ', '
                        else:
                            start = False
                        dialog_str = dialog_str + ui.devs_list[ui.setup_list[j]]
                ui.msg_err(dialog_str)
                ui.changePW_prompt()
            else:
                dialog_str = dialog_str + '\nPassword changed successfully for the following drives: '
                start = True
                for i in list_s:
                    if not start:
                        dialog_str = dialog_str + ', '
                    else:
                        start = False
                    dialog_str = dialog_str + ui.devs_list[ui.setup_list[i]]
                ui.msg_ok(dialog_str)
                ui.returnToMain()
            
        e = threading.Event()
        t1 = threading.Thread(target=t1_run, args=(e,))
        t1.start()
        start_time = time.time()
        t2 = threading.Thread(target=timeout_track, args=(ui, 30.0*len(selected_list), start_time, t1, e))
        t2.start()

def run_revertErase(button, ui):
    text_a = ui.revert_agree_entry.get_text()
    if text_a.lower() != 'i agree':
        ui.msg_err("Type 'I agree' into the entry box")
        return
    if not ui.warned:
        message = gtk.MessageDialog(type=gtk.MESSAGE_WARNING, buttons=gtk.BUTTONS_YES_NO, parent = ui)
        message.set_markup("Warning: Are you sure you want to revert and erase all of your data?\nYou will not be able to recover your data after it is deleted.")
        res = message.run()
        if res == gtk.RESPONSE_YES:
            message.destroy()
            ui.warned = True
            ui.orig = ui.pass_entry.get_text()
            ui.pass_entry.get_buffer().delete_text(0,-1)
            ui.revert_agree_entry.get_buffer().delete_text(0,-1)
            ui.op_instr.set_text('Re-enter ' + ui.devname + '\'s password to verify that you want to revert the drive.')
            ui.dev_single.show()
            ui.label_dev2.show()
            ui.dev_select.hide()
            ui.label_dev.hide()
        else:
            message.destroy()
            
    else :
        ui.warned = False
        if ui.orig != ui.pass_entry.get_text():
            ui.orig = ''
            ui.msg_err('The passwords entered do not match')
            return
        ui.orig = ''
        messageA = gtk.MessageDialog(type=gtk.MESSAGE_WARNING, buttons=gtk.BUTTONS_YES_NO, parent = ui)
        messageA.set_markup("Final Warning: Are you absolutely sure you want to proceed with reverting " + ui.devname + " and erasing all of its data?")
        resA = messageA.run()

        if resA == gtk.RESPONSE_YES : 
            messageA.destroy()
            
            selected_list = []
            if ui.toggleSingle_radio.get_active():
                index = ui.dev_select.get_active()
                selected_list = [index]
            else:
                index = 0
                selected_list = []
                iter = ui.liststore.get_iter_first()
                while iter != None:
                    selected = ui.liststore.get_value(iter, 0)
                    if selected:
                        selected_list.append(index)
                    iter = ui.liststore.iter_next(iter)
                    index = index + 1
                
            ui.start_spin()
            ui.wait_instr.show()
            
            password = ""
            
            
            
            def t1_run(e):
                list_s = []
                list_f = []
                notify_TO = False
                for index in selected_list:
                    if e.isSet():
                        list_f.append(index)
                        continue
                    if ui.VERSION % 2 == 1 and ui.check_pass_rd.get_active():
                        password = passReadUSB(ui, ui.vendor_list[ui.setup_list[index]], ui.sn_list[ui.setup_list[index]])
                        if password == None or password == 'x':
                            list_f.append(index)
                            continue
                    else:
                        password = lockhash.hash_pass(ui.pass_entry.get_text(), ui.salt_list[ui.setup_list[index]], ui.msid_list[ui.setup_list[index]])
                    if e.isSet():
                        notify_TO = True
                        list_f.append(index)
                        continue
                    #if password == None or password == 'x':
                    #    gobject.idle_add(cleanup, 0, 1)
                    #else:
                    ui.devname = ui.devs_list[ui.setup_list[index]]
                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                    timeStr = timeStr[2:]
                    statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 03" + timeStr + " " + password + " Admin1 " + ui.devs_list[ui.setup_list[index]])
                    if statusAW != 0:
                        list_f.append(index)
                        continue
                    statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 21" + timeStr + " " + password + " Admin1 " + ui.devs_list[ui.setup_list[index]])
                    status = os.system(ui.prefix + "sedutil-cli -n -t --revertTPer " + password + " " + ui.devs_list[ui.setup_list[index]])
                    if status != 0:
                        list_f.append(index)
                        continue
                    #else:
                    list_s.append(index)
                    if ui.pass_usb != '':
                        dev_os = platform.system()
                        if dev_os == 'Windows':
                            filepath = ui.pass_usb + ':\\FidelityLock\\' + ui.vendor_list[ui.setup_list[index]] + '_' + ui.sn_list[ui.setup_list[index]]
                            os.remove(filepath)
                        elif dev_os == 'Linux':
                            filepath = ui.pass_usb + '/FidelityLock/' + ui.vendor_list[ui.setup_list[index]] + '_' + ui.sn_list[ui.setup_list[index]]
                            os.remove(filepath)
                    dev_msid = ui.msid_list[ui.setup_list[index]]
                    status = os.system(ui.prefix + "sedutil-cli -n -t --activate " + dev_msid + " " + ui.devs_list[ui.setup_list[index]])
                    statusAE = os.system(ui.prefix + "sedutil-cli -n -t --auditerase " + dev_msid + " Admin1 " + ui.devs_list[ui.setup_list[index]])
                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                    timeStr = timeStr[2:]
                    statusAW1 = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 24" + timeStr + " " + dev_msid + " Admin1 " + ui.devs_list[ui.setup_list[index]])
                    statusAW2 = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 26" + timeStr + " " + dev_msid + " Admin1 " + ui.devs_list[ui.setup_list[index]])
                    statusAW3 = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 01" + timeStr + " " + dev_msid + " Admin1 " + ui.devs_list[ui.setup_list[index]])
                gobject.idle_add(cleanup, list_s, list_f)
                
            def cleanup(list_s, list_f):
                t1.join()
                ui.stop_spin()
                if len(list_f) > 0 :
                    dialog_str = 'Revert and Erase failed for the following drives: '
                    start = True
                    for i in list_f:
                        if not start:
                            dialog_str = dialog_str + ', '
                        else:
                            start = False
                        dialog_str = dialog_str + ui.devs_list[ui.setup_list[i]]
                        timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                        timeStr = timeStr[2:]
                        pwd = 'F0iD2eli81Ty' + ui.salt_list[ui.setup_list[index]]
                        hash_pwd = lockhash.hash_pass(pwd, ui.salt_list[ui.setup_list[index]], ui.msid_list[ui.setup_list[index]])
                        statusAW1 = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 21" + timeStr + " " + hash_pwd + " User" + ui.user_list[ui.setup_list[index]] + " " + ui.devname)
                        statusAW1 = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 28" + timeStr + " " + hash_pwd + " User" + ui.user_list[ui.setup_list[index]] + " " + ui.devname)
                    if len(list_s) > 0:
                        dialog_str = dialog_str + '\nThe following drives were reverted successfully: '
                        start = True
                        for j in list_s:
                            if not start:
                                dialog_str = dialog_str + ', '
                            else:
                                start = False
                            dialog_str = dialog_str + ui.devs_list[ui.setup_list[j]]
                            ui.lockstatus_list[ui.setup_list[j]] = "Unlocked"
                            ui.setupstatus_list[ui.setup_list[j]] = "No"
                            ui.pba_list[ui.setup_list[j]] = 'N/A'
                            ui.updateDevs(ui.setup_list[j],[4])
                    ui.msg_err(dialog_str)
                    ui.revert_erase_prompt()
                else :
                    dialog_str = 'The following drives were reverted successfully: '
                    start = True
                    for i in list_s:
                        if not start:
                            dialog_str = dialog_str + ', '
                        else:
                            start = False
                        dialog_str = dialog_str + ui.devs_list[ui.setup_list[i]]
                        ui.lockstatus_list[ui.setup_list[i]] = "Unlocked"
                        ui.setupstatus_list[ui.setup_list[i]] = "No"
                        ui.pba_list[ui.setup_list[i]] = 'N/A'
                        ui.updateDevs(ui.setup_list[i],[4])
                    ui.msg_ok(dialog_str)
                    ui.returnToMain()
                    
            e = threading.Event()
            t1 = threading.Thread(target=t1_run, args=(e,))
            t1.start()
            start_time = time.time()
            t2 = threading.Thread(target=timeout_track, args=(ui, 30.0, start_time, t1, e))
            t2.start()
        else:
            messageA.destroy()
        

def run_revertPSID(button, ui):
    text_a = ui.revert_agree_entry.get_text()
    if text_a.lower() != 'i agree':
        ui.msg_err("Type 'I agree' into the entry box")
        return
    index = ui.dev_select.get_active()
    ui.devname = ui.devs_list[ui.tcg_list[index]]
    if not ui.warned:
        message = gtk.MessageDialog(type=gtk.MESSAGE_WARNING, buttons=gtk.BUTTONS_YES_NO, parent = ui)
        message.set_markup("Warning: Are you sure you want to revert " + ui.devname + " and erase all of its data?\nYou will not be able to recover your data after it is deleted.")
        res = message.run()
        if res == gtk.RESPONSE_YES:
            message.destroy()
            ui.warned = True
            ui.orig = ui.revert_psid_entry.get_text()
            ui.revert_psid_entry.get_buffer().delete_text(0,-1)
            ui.revert_agree_entry.get_buffer().delete_text(0,-1)
            ui.op_instr.set_text('Re-enter ' + ui.devname + '\'s PSID to verify that you want to revert the drive.')
            ui.dev_single.show()
            ui.label_dev2.show()
            ui.dev_select.hide()
            ui.label_dev.hide()
        else:
            message.destroy()
    else:
        ui.warned = False
        if ui.orig != ui.revert_psid_entry.get_text():
            ui.orig = ''
            ui.msg_err('The PSIDs entered do not match')
            return
        ui.orig = ''
        message = gtk.MessageDialog(type=gtk.MESSAGE_WARNING, buttons=gtk.BUTTONS_YES_NO, parent = ui)
        message.set_markup("Final Warning: Are you absolutely sure you want to proceed with reverting " + ui.devname + " and erasing all of its data?")
        res = message.run()
        if res == gtk.RESPONSE_YES :
            message.destroy()
                
            ui.start_spin()
            ui.wait_instr.show()
            
            
            def t1_run():            
                psid = ui.revert_psid_entry.get_text()
                
                status =  os.system(ui.prefix + "sedutil-cli -n -t --yesIreallywanttoERASEALLmydatausingthePSID " + psid + " " + ui.devname )
                gobject.idle_add(cleanup, status)
            
            def cleanup(status):
                index = ui.dev_select.get_active()
                ui.stop_spin()
                t1.join()
                if status != 0 :
                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                    timeStr = timeStr[2:]
                    pwd = 'F0iD2eli81Ty' + ui.salt_list[ui.tcg_list[index]]
                    hash_pwd = lockhash.hash_pass(pwd, ui.salt_list[ui.tcg_list[index]], ui.msid_list[ui.tcg_list[index]])
                    statusAW1 = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 22" + timeStr + " " + hash_pwd + " User" + ui.user_list[ui.tcg_list[index]] + " " + ui.devname)
                    statusAW1 = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 29" + timeStr + " " + hash_pwd + " User" + ui.user_list[ui.tcg_list[index]] + " " + ui.devname)
                    ui.msg_err("Error: Incorrect PSID, please try again." )
                else :
                    index = ui.dev_select.get_active()
                    dev_msid = ui.msid_list[ui.tcg_list[index]]
                    status = os.system(ui.prefix + "sedutil-cli -n -t --activate " + dev_msid + " " + ui.devname)
                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                    timeStr = timeStr[2:]
                    statusAW1 = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 22" + timeStr + " " + dev_msid + " Admin1 " + ui.devname)
                    statusAW1 = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 25" + timeStr + " " + dev_msid + " Admin1 " + ui.devname)
                    statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 26" + timeStr + " " + dev_msid + " Admin1 " + ui.devname)
                    statusAW2 = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 01" + timeStr + " " + dev_msid + " Admin1 " + ui.devname)
                    ui.msg_ok("Device " + ui.devname + " successfully reverted with PSID.")
                    index = ui.dev_select.get_active()
                    ui.query(None,1)
                    ui.lockstatus_list[ui.tcg_list[index]] = "Unlocked"
                    ui.setupstatus_list[ui.tcg_list[index]] = "No"
                    ui.pba_list[ui.tcg_list[index]] = 'N/A'
                    ui.updateDevs(ui.tcg_list[index],[4])
                    ui.returnToMain()
            
            e = threading.Event()
            t1 = threading.Thread(target=t1_run, args=())
            t1.start()
            start_time = time.time()
            t2 = threading.Thread(target=timeout_track, args=(ui, 30.0, start_time, t1, e))
            t2.start()
        else:
            message.destroy()
    
def run_lockDrive(button, ui):
    index = ui.dev_select.get_active()
    ui.devname = ui.devs_list[ui.unlocked_list[index]]
    print 'Selected Drive: ' + ui.devname
    ui.start_spin()
    ui.wait_instr.show()
    password = ""
    notify_TO = False
    if ui.VERSION == 3 and ui.check_pass_rd.get_active():
        password = passReadUSB(ui, ui.dev_vendor.get_text(), ui.dev_sn.get_text())
        print 'USB password: ' + password
        if password == None or password == 'x':
            ui.msg_err('No password found for the drive.')
            return
    else:
        password = lockhash.hash_pass(ui.pass_entry.get_text(), ui.salt_list[ui.unlocked_list[index]], ui.dev_msid.get_text())
        print 'Typed password: ' + password
        #ui.pass_entry.get_buffer().delete_text(0,-1)
    def t1_run(e):
        print ui.prefix + "sedutil-cli -n -t --setMBRDone on " + password + " " + ui.devname
        status1 =  os.system(ui.prefix + "sedutil-cli -n -t --setMBRDone on " + password + " " + ui.devname )
        if e.isSet():
            notify_TO = True
            gobject.idle_add(cleanup, -1, 1, notify_TO)
        elif status1 != 0:
            gobject.idle_add(cleanup, status1, 0, notify_TO)
        else:
            status2 =  os.system(ui.prefix + "sedutil-cli -n -t --setMBREnable on " + password + " " + ui.devname )
            if e.isSet():
                notify_TO = True
                gobject.idle_add(cleanup, -1, 1, notify_TO)
            else:
                if ui.devname != '\\\\.\\PhysicalDrive0' and ui.devname != '/dev/sda' and ui.devname != '/nvme':
                    status3 =  os.system(ui.prefix + "sedutil-cli -n -t --setLockingrange " + ui.LKRNG + " LK "
                            + password + " " + ui.devname )
                    status = status1 | status2 | status3
                    gobject.idle_add(cleanup, status, 1, notify_TO)
                else:
                    status = status1 | status2
                    gobject.idle_add(cleanup, status, 0, notify_TO)
        
    def cleanup(status, mode, op_to):
        ui.stop_spin()
        t1.join()
        if status != 0 :
            if op_to:
                ui.msg_err('Operation timed out.')
            if status == ui.NOT_AUTHORIZED or status == ui.AUTHORITY_LOCKED_OUT:
                pwd = 'F0iD2eli81Ty' + ui.salt_list[ui.unlocked_list[index]]
                hash_pwd = lockhash.hash_pass(pwd, ui.salt_list[ui.unlocked_list[index]], ui.msid_list[ui.unlocked_list[index]])
                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                timeStr = timeStr[2:]
                statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 04" + timeStr + " " + hash_pwd + " User" + ui.user_list[ui.unlocked_list[index]] + " " + ui.devname)
                if status == ui.AUTHORITY_LOCKED_OUT:
                    statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 10" + timeStr + " " + hash_pwd + " User" + ui.user_list[ui.unlocked_list[index]] + " " + ui.devname)
                    ui.msg_err("Error: Retry limit reached for " + ui.devname + '.  Please power cycle the drive to try again.')
                else:
                    ui.msg_err("Error: Invalid password for " + ui.devname + '.')
            else:
                ui.msg_err("Error while attempting to lock " + ui.devname + '.')
        else :
            if ui.VERSION % 2 == 1 and ui.pass_sav.get_active():
                print "lockDrive passSaveUSB " + password + " " + ui.auth_menu.get_active_text()
                passSaveUSB(ui, password, ui.drive_menu.get_active_text(), ui.dev_vendor.get_text(), ui.dev_sn.get_text())
                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                timeStr = timeStr[2:]
                statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 18" + timeStr + " " + password + " Admin1 " + ui.devname)
            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
            timeStr = timeStr[2:]
            statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 03" + timeStr + " " + password + " Admin1 " + ui.devname)
            
            if mode == 1:
                statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 17" + timeStr + " " + password + " Admin1 " + ui.devname)
                ui.msg_ok("Drive " + ui.devname + " locked successfully.") 
                ui.lockstatus_list[ui.unlocked_list[index]] = "Locked"
            else:
                ui.msg_ok("Locking enabled on drive " + ui.devname + " but not locked. Power cycle the drive to lock the drive.") 
            
            if ui.pba_list[ui.unlocked_list[index]] == 'N/A':
                p = os.popen(ui.prefix + "sedutil-cli -n -t --pbaValid " + password + " " + ui.devname).read()
                pba_regex = 'PBA image version\s*:\s*(.+)'
                m1 = re.search(pba_regex, p)
                if m1:
                    pba_ver = m1.group(1)
                    ui.pba_list[ui.unlocked_list[index]] = pba_ver
            
            ui.query(None,1)
            if mode == 1:
                ui.updateDevs(ui.unlocked_list[index],[1,2])
            ui.returnToMain()

    e = threading.Event()
    t1 = threading.Thread(target=t1_run, args=(e,))
    t1.start()
    start_time = time.time()
    t2 = threading.Thread(target=timeout_track, args=(ui, 30.0, start_time, t1, e))
    t2.start()

def run_unlockPBA(button, ui, reboot):
    selected_list = []
    if ui.toggleSingle_radio.get_active():
        index = ui.dev_select.get_active()
        selected_list = [ui.locked_list[index]]
    else:
        index = 0
        selected_list = []
        iter = ui.liststore.get_iter_first()
        while iter != None:
            selected = ui.liststore.get_value(iter, 0)
            if selected:
                selected_list.append(ui.locked_list[index])
            iter = ui.liststore.iter_next(iter)
            index = index + 1
    ui.LKATTR = "RW"
    ui.start_spin()
    ui.wait_instr.show()
    def t1_run(e):
        list_s = []
        list_f = []
        notify_TO = False
        for i in selected_list:
            if e.isSet():
                list_f.append(i)
                notify_TO = True
                continue
            ui.devname = ui.devs_list[i]
            password = ''
            if ui.VERSION % 2 == 1 and ui.check_pass_rd.get_active():
                password = passReadUSB(ui, ui.vendor_list[i], ui.sn_list[i])
                if password == None or password == 'x':
                    list_f.append(i)
                    continue
            else:
                password = lockhash.hash_pass(ui.pass_entry.get_text(), ui.salt_list[i], ui.msid_list[i])
            if e.isSet():
                notify_TO = True
                list_f.append(i)
                continue
            elif password == None or password == 'x':
                list_f.append(i)
                continue
            if ui.VERSION % 2 == 1 and ui.auth_menu.get_active() == 1:
                status1 = os.system(ui.prefix + "sedutil-cli -n -t -u --setMBRDone on " + password + " " + ui.devname)
                if status1 != 0:
                    list_f.append(i)
                    pwd = 'F0iD2eli81Ty' + ui.salt_list[i]
                    hash_pwd = lockhash.hash_pass(pwd, ui.salt_list[i], ui.msid_list[i])
                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                    timeStr = timeStr[2:]
                    if ui.VERSION % 2 == 1 and ui.auth_menu.get_active() == 1:
                        statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 06" + timeStr + " " + hash_pwd + " User" + ui.user_list[i] + " " + ui.devname)
                    else:
                        statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 04" + timeStr + " " + hash_pwd + " User" + ui.user_list[i] + " " + ui.devname)
                    continue
                elif e.isSet():
                    status1 = os.system(ui.prefix + "sedutil-cli -n -t -u --setMBRDone off " + password + " " + ui.devname)
                    notify_TO = True
                    list_f.append(i)
                    continue
                status2 = os.system(ui.prefix + "sedutil-cli -n -t -u --setLockingRange 0 " + ui.LKATTR + " " + password + " " + ui.devname)
                list_s.append(i)
                if ui.VERSION % 2 == 1 and ui.pass_sav.get_active():
                    print "unlockPBA passSaveUSB " + password
                    passSaveUSB(ui, password, ui.drive_menu.get_active_text(), ui.vendor_list[i], ui.sn_list[i])
                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                    timeStr = timeStr[2:]
                    if ui.VERSION % 2 == 1 and ui.auth_menu.get_active() == 1:
                        statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 18" + timeStr + " " + password + " User1 " + ui.devname)
                    else:
                        statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 18" + timeStr + " " + password + " Admin1 " + ui.devname)
                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                timeStr = timeStr[2:]
                if ui.VERSION % 2 == 1 and ui.auth_menu.get_active() == 1:
                    statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 05" + timeStr + " " + password + " User1 " + ui.devname)
                    statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 14" + timeStr + " " + password + " User1 " + ui.devname)
                    if ui.usb_boot:
                        statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 16" + timeStr + " " + password + " User1 " + ui.devname)
                    else:
                        statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 15" + timeStr + " " + password + " User1 " + ui.devname)
                else:
                    statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 03" + timeStr + " " + password + " Admin1 " + ui.devname)
                    statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 14" + timeStr + " " + password + " Admin1 " + ui.devname)
                    if ui.usb_boot:
                        statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 16" + timeStr + " " + password + " Admin1 " + ui.devname)
                    else:
                        statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 15" + timeStr + " " + password + " Admin1 " + ui.devname)
            else:
                status1 =  os.system(ui.prefix + "sedutil-cli -n -t --setMBRdone on " + password + " " + ui.devname )
                if status1 != 0:
                    list_f.append(i)
                    pwd = 'F0iD2eli81Ty' + ui.salt_list[i]
                    hash_pwd = lockhash.hash_pass(pwd, ui.salt_list[i], ui.msid_list[i])
                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                    timeStr = timeStr[2:]
                    if ui.VERSION % 2 == 1 and ui.auth_menu.get_active() == 1:
                        statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 06" + timeStr + " " + hash_pwd + " User" + ui.user_list[i] + " " + ui.devname)
                    else:
                        statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 04" + timeStr + " " + hash_pwd + " User" + ui.user_list[i] + " " + ui.devname)
                    continue
                elif e.isSet():
                    status1 = os.system(ui.prefix + "sedutil-cli -n -t --setMBRDone off " + password + " " + ui.devname)
                    notify_TO = True
                    list_f.append(i)
                    continue
                status2 =  os.system(ui.prefix + "sedutil-cli -n -t --setLockingRange " + ui.LKRNG + " " 
                        + ui.LKATTR + " " + password + " " + ui.devname)
                list_s.append(i)
                if ui.VERSION % 2 == 1 and ui.pass_sav.get_active():
                    print "unlockPBA passSaveUSB " + password
                    passSaveUSB(ui, password, ui.drive_menu.get_active_text(), ui.vendor_list[i], ui.sn_list[i])
                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                    timeStr = timeStr[2:]
                    if ui.auth_menu.get_active() == 1:
                        statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 18" + timeStr + " " + password + " User1 " + ui.devname)
                    else:
                        statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 18" + timeStr + " " + password + " Admin1 " + ui.devname)
                if ui.auth_menu.get_active() != 1 and ui.pba_list[i] == 'N/A':
                    p = os.popen(ui.prefix + "sedutil-cli -n -t --pbaValid " + password + " " + ui.devname).read()
                    pba_regex = 'PBA image version\s*:\s*(.+)'
                    m1 = re.search(pba_regex, p)
                    if m1:
                        pba_ver = m1.group(1)
                        ui.pba_list[i] = pba_ver
                        
                ui.lockstatus_list[i] = "Unlocked"
                
                ui.updateDevs(i,[2,3])
                
                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                timeStr = timeStr[2:]
                if ui.VERSION % 2 == 1 and ui.auth_menu.get_active() == 1:
                    statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 05" + timeStr + " " + password + " User1 " + ui.devname)
                else:
                    statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 03" + timeStr + " " + password + " Admin1 " + ui.devname)
        ui.pass_entry.get_buffer().delete_text(0,-1)
        gobject.idle_add(cleanup, list_s, list_f, notify_TO)
        
    def cleanup(list_s, list_f, op_to):
        ui.stop_spin()
        t1.join()
        if len(list_f) > 0 :
            if op_to:
                ui.msg_err('Operation timed out.')
            txt = 'The following drives were not successfully unlocked: '
            start = True
            for i in list_f:
                if not start:
                    txt = txt + ', '
                else:
                    start = False
                txt = txt + ui.devs_list[i]
            
            if len(list_s) > 0 :
                start = True
                txt = txt + 'The following drives were unlocked: '
                for j in list_s:
                    if not start:
                        txt = txt + ', '
                    else:
                        start = False
                    txt = txt + ui.devs_list[j]
            ui.msg_err(txt)
            ui.unlock_prompt(ui)
        else :
            if reboot:
                ui.reboot()
            else:
                txt = ''
                start = True
                for i in list_s:
                    if not start:
                        txt = txt + ', '
                    else:
                        start = False
                    txt = txt + ui.devs_list[i]
                    
                    #ui.query(None, 1)
                    
                ui.msg_ok(txt + " unlocked successfully.")
                
                ui.returnToMain()
                
    e = threading.Event()
    t1 = threading.Thread(target=t1_run, args=(e,))
    t1.start()
    start_time = time.time()
    t2 = threading.Thread(target=timeout_track, args=(ui, 30.0*len(selected_list), start_time, t1, e))
    t2.start()

def run_revertKeep(button, ui):
    selected_list = []
    if ui.toggleSingle_radio.get_active():
        index = ui.dev_select.get_active()
        selected_list = [index]
    else:
        index = 0
        selected_list = []
        iter = ui.liststore.get_iter_first()
        while iter != None:
            selected = ui.liststore.get_value(iter, 0)
            if selected:
                selected_list.append(index)
            iter = ui.liststore.iter_next(iter)
            index = index + 1
    ui.start_spin()
    ui.wait_instr.show()
    password = ''
    
    def t1_run(e):
        list_s = []
        list_f = []
        notify_TO = False
        for index in selected_list:
            if e.isSet():
                notify_TO = True
                list_f.append(index)
                continue
            ui.devname = ui.devs_list[ui.setup_list[index]]
            dev_msid = ui.msid_list[ui.setup_list[index]]
            if ui.VERSION % 2 == 1 and ui.check_pass_rd.get_active():
                password = passReadUSB(ui, ui.vendor_list[ui.setup_list[index]], ui.sn_list[ui.setup_list[index]])
                if password == None or password == 'x':
                    list_f.append(index)
            else:
                password = lockhash.hash_pass(ui.pass_entry.get_text(), ui.salt_list[ui.setup_list[index]], dev_msid)
            if e.isSet():
                notify_TO = True
                list_f.append(index)
                continue
            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
            timeStr = timeStr[2:]
            p0 = os.popen(ui.prefix + "sedutil-cli --query " + ui.devname).read()
            txtL = "Locked = Y"
            is_Locked = re.search(txtL, p0)
            is_Locked = ui.lockstatus_list[ui.setup_list[index]]
            if is_Locked:
                status1 =  os.system(ui.prefix + "sedutil-cli -n -t --setMBRdone on " + password + " " + ui.devname )
                if status1 != 0:
                    if e.isSet():
                        notify_TO = True
                    list_f.append(index)
                    continue
                elif e.isSet():
                    notify_TO = True
                    list_f.append(index)
                    continue
                status2 =  os.system(ui.prefix + "sedutil-cli -n -t --setLockingRange " + ui.LKRNG + " " + ui.LKATTR + " " + password + " " + ui.devname)
                if e.isSet():
                    notify_TO = True
                    list_f.append(index)
                    continue
            
            statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 20" + timeStr + " " + password + " Admin1 " + ui.devname)
            if statusAW == 0:
                statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 03" + timeStr + " " + password + " Admin1 " + ui.devname)
                
                status = os.system(ui.prefix + "sedutil-cli -n -t --revertnoerase " + password + " " + ui.devname)
                p0 = os.popen(ui.prefix + "sedutil-cli --query " + ui.devname).read()
                txtLE = "LockingEnabled = N"
                le_check = re.search(txtLE, p0)
                if le_check:
                    status = os.system(ui.prefix + "sedutil-cli -n -t --setSIDPassword " + password + " " + dev_msid + " " + ui.devname)
                    list_s.append(index)
                    if status == 0:
                        status = os.system(ui.prefix + "sedutil-cli -n -t --activate " + dev_msid + " " + ui.devname)
                else: #need to figure out if there's any additional action needed if revertnoerase fails
                    list_f.append(index)
            else:
                list_f.append(index)
        gobject.idle_add(cleanup, list_s, list_f, notify_TO)
        
    def cleanup(list_s, list_f, op_to):
        ui.stop_spin()
        t1.join()
        for index in list_s:
            dev_msid = ui.msid_list[ui.setup_list[index]]
            ui.devname = ui.devs_list[ui.setup_list[index]]
            if ui.pass_usb != '':
                dev_os = platform.system()
                if dev_os == 'Windows':
                    filepath = ui.pass_usb + ':\\FidelityLock\\' + ui.vendor_list[ui.setup_list[index]] + '_' + ui.sn_list[ui.setup_list[index]] + '.psw'
                    os.remove(filepath)
                elif dev_os == 'Linux':
                    filepath = ui.pass_usb + '/FidelityLock/' + ui.vendor_list[ui.setup_list[index]] + '_' + ui.sn_list[ui.setup_list[index]] + '.psw'
                    os.remove(filepath)
                    
                    
            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
            timeStr = timeStr[2:]
            
            statusAW1 = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 23" + timeStr + " " + dev_msid + " Admin1 " + ui.devname)
            statusAW2 = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 01" + timeStr + " " + dev_msid + " Admin1 " + ui.devname)
        if len(list_f) > 0:
            if op_to:
                ui.msg_err('Operation timed out.')
            dialog_str = 'Revert and Erase failed for the following drives: '
            start = True
            for i in list_f:
                if not start:
                    dialog_str = dialog_str + ', '
                else:
                    start = False
                dialog_str = dialog_str + ui.devs_list[ui.setup_list[i]]
                pwd = 'F0iD2eli81Ty' + ui.salt_list[ui.setup_list[i]]
                hash_pwd = lockhash.hash_pass(pwd, ui.salt_list[ui.setup_list[i]], ui.msid_list[ui.setup_list[i]])
                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                timeStr = timeStr[2:]
                statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 04" + timeStr + " " + hash_pwd + " User" + ui.user_list[ui.setup_list[i]] + " " + ui.devname)
                statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 20" + timeStr + " " + hash_pwd + " User" + ui.user_list[ui.setup_list[i]] + " " + ui.devname)
                statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 27" + timeStr + " " + hash_pwd + " User" + ui.user_list[ui.setup_list[i]] + " " + ui.devname)
            if len(list_s) > 0:
                dialog_str = dialog_str + '\nThe following drives were reverted successfully: '
                start = True
                for j0 in list_s:
                    list_j.append(ui.setup_list[j0])
                
                for j in list_j:
                    if not start:
                        dialog_str = dialog_str + ', '
                    else:
                        start = False
                    dialog_str = dialog_str + ui.devs_list[j]
                    ui.lockstatus_list[j] = "Unlocked"
                    ui.setupstatus_list[j] = "No"
                    ui.pba_list[j] = 'N/A'
                    ui.updateDevs(j,[4])
            ui.msg_err(dialog_str)
            ui.revert_keep_prompt()
        else:
            dialog_str = 'The following drives were reverted successfully: '
            start = True
            list_i = []
            for i0 in list_s:
                list_i.append(ui.setup_list[i0])
            
            for i in list_i:
                if not start:
                    dialog_str = dialog_str + ', '
                else:
                    start = False
                dialog_str = dialog_str + ui.devs_list[i]
                ui.lockstatus_list[i] = "Unlocked"
                ui.setupstatus_list[i] = "No"
                ui.pba_list[i] = 'N/A'
                ui.updateDevs(i,[4])
            ui.msg_ok(dialog_str)
            ui.returnToMain()

    e = threading.Event()
    t1 = threading.Thread(target=t1_run, args=(e,))
    t1.start()
    start_time = time.time()
    t2 = threading.Thread(target=timeout_track, args=(ui, 30.0, start_time, t1, e))
    t2.start()

def run_unlockUSB(button, ui, mode, msg):
    if msg:
        msg.destroy()
    folder_list = []
    dev_os = platform.system()
    if dev_os == 'Windows':
        for drive in ascii_uppercase:
            if drive != 'C' and os.path.isdir('%s:\\' % drive):
                if os.path.isdir('%s:\\FidelityLock' % drive):
                    folder_list.append(drive)
    elif dev_os == 'Linux':
        txt = os.popen(ui.prefix + 'mount').read()
        dev_regex = '/dev/sd[a-z][1-9]?\s*on\s*(\S+)\s*type'
        drive_list = re.findall(dev_regex, txt)
        txt2 = os.popen(ui.prefix + 'blkid').read()
        dev_regex2 = '(/dev/sd[a-z][1-9]?.+)'
        all_list = re.findall(dev_regex2, txt2)
        r1 = '/dev/sd[a-z][1-9]?'
        r2 = 'TYPE="([a-z]+)"'
        for a in all_list:
            m1 = re.search(r1,a)
            m2 = re.search(r2,a)
            dev_a = m1.group(0)
            type_a = m2.group(1)
            if dev_a not in drive_list:
                s = os.system(ui.prefix + 'mount -t ' + type_a + ' ' + dev_a)
        txt = os.popen(ui.prefix + 'mount').read()
        dev_regex = '/dev/sd[a-z][1-9]?\s*on\s*(\S+)\s*type'
        drive_list = re.findall(dev_regex, txt)
        for d in drive_list:
            if os.path.isdir('%s/FidelityLock' % d):
                folder_list.append(d)
    if len(folder_list) == 0:
        ui.msg_err('No password files found, check to make sure the USB is mounted.')
    else:
        ui.start_spin()
        ui.wait_instr.show()
        def t1_run(e):
            dev_unlocked = []
            dev_failed = []
            for i in ui.locked_list:
                pw = passReadUSB(ui, ui.vendor_list[i], ui.sn_list[i])
                if pw == None or pw == 'x':
                    ui.auth_menu.set_active(1)
                    pw = passReadUSB(ui, ui.vendor_list[i], ui.sn_list[i])
                if pw != 'x' and pw != None:
                    if e.isSet():
                        dev_failed.append(i)
                        continue
                    ui.LKATTR = "RW"
                    ui.LKRNG = "0"
                    status1 = ''
                    status2 = ''
                    if ui.auth_menu.get_active() == 0:
                        status1 =  os.system(ui.prefix + "sedutil-cli -n -t --setMBRdone on " + pw + " " + ui.devs_list[i] )
                        
                        if status != 0:
                            dev_failed.append(i)
                            pwd = 'F0iD2eli81Ty' + ui.salt_list[i]
                            hash_pwd = lockhash.hash_pass(pwd, ui.salt_list[i], ui.msid_list[i])
                            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                            timeStr = timeStr[2:]
                            statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 04" + timeStr + " " + hash_pwd + " User" + ui.user_list[i] + " " + ui.devname)
                            dev_failed.append(i)
                            continue
                        if e.isSet():
                            status1 =  os.system(ui.prefix + "sedutil-cli -n -t --setMBRdone off " + pw + " " + ui.devs_list[i] )
                            dev_failed.append(i)
                            continue
                        status2 =  os.system(ui.prefix + "sedutil-cli -n -t --setLockingRange " + ui.LKRNG + " " 
                                + ui.LKATTR + " " + pw + " " + ui.devs_list[i])
                    else:
                        status1 =  os.system(ui.prefix + "sedutil-cli -n -t -u --setMBRdone on " + pw + " " + ui.devs_list[i] )
                        if status != 0:
                            dev_failed.append(i)
                            pwd = 'F0iD2eli81Ty' + ui.salt_list[i]
                            hash_pwd = lockhash.hash_pass(pwd, ui.salt_list[i], ui.msid_list[i])
                            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                            timeStr = timeStr[2:]
                            statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 06" + timeStr + " " + hash_pwd + " User" + ui.user_list[i] + " " + ui.devname)
                            dev_failed.append(i)
                            continue
                        if e.isSet():
                            status1 =  os.system(ui.prefix + "sedutil-cli -n -t -u --setMBRdone off " + pw + " " + ui.devs_list[i] )
                            dev_failed.append(i)
                            continue
                        status2 =  os.system(ui.prefix + "sedutil-cli -n -t -u --setLockingRange 0 " 
                                + ui.LKATTR + " " + pw + " " + ui.devs_list[i])
                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                    timeStr = timeStr[2:]
                    if ui.auth_menu.get_active() == 0:
                        statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 03" + timeStr + " " + pw + " Admin1 " + ui.devs_list[i])
                    else:
                        statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 05" + timeStr + " " + pw + " User1 " + ui.devs_list[i])
                    dev_unlocked.append(i)
                    
                    if ui.auth_menu.get_active() != 1 and ui.pba_list[i] == 'N/A':
                        p = os.popen(ui.prefix + "sedutil-cli -n -t --pbaValid " + pw + " " + ui.devname).read()
                        pba_regex = 'PBA image version\s*:\s*(.+)'
                        m1 = re.search(pba_regex, p)
                        if m1:
                            pba_ver = m1.group(1)
                            ui.pba_list[i] = pba_ver
                ui.auth_menu.set_active(0)
            gobject.idle_add(cleanup, dev_unlocked, dev_failed, mode)
            
        def cleanup(dev_unlocked, dev_failed, mode):
            if len(dev_unlocked) == 0:
                ui.msg_err('No drives were unlocked.')
                return 0
            elif mode == 0:
                if len(dev_failed) == 0:
                    txt = 'The following drives were unlocked: '
                    for j in range(len(dev_unlocked) - 1):
                        txt = txt + ui.devs_list[dev_unlocked[j]] + ', '
                    txt = txt + ui.devs_list[dev_unlocked[len(dev_unlocked) - 1]]
                    ui.msg_ok(txt)
                    for i in dev_unlocked:
                        ui.lockstatus_list[i] = "Unlocked"
                        ui.updateDevs(i,[2,3])
                    ui.returnToMain()
                else:
                    txt = 'The following drives were not successfully unlocked: '
                    for i in range(len(dev_failed) - 1):
                        txt = txt + ui.devs_list[dev_failed[i]] + ', '
                    txt = txt + ui.devs_list[dev_failed[len(dev_failed) - 1]] + '\n'
                    txt = txt + 'The following drives were unlocked: '
                    for j in range(len(dev_unlocked) - 1):
                        txt = txt + ui.devs_list[dev_unlocked[j]] + ', '
                    txt = txt + ui.devs_list[dev_unlocked[len(dev_unlocked) - 1]]
                    ui.msg_err(txt)
                    for i in dev_unlocked:
                        ui.lockstatus_list[i] = "Unlocked"
                        ui.updateDevs(i,[2,3])
            else:
                ui.reboot()
        
        e = threading.Event()
        t1 = threading.Thread(target=t1_run, args=(e,))
        t1.start()
        start_time = time.time()
        t2 = threading.Thread(target=timeout_track, args=(ui, 30.0, start_time, t1, e))
        t2.start()

def passReadUSB(ui, model, sn, *args):
    folder_list = []
    latest_pw = ""
    latest_ts = "0"
    dev_os = platform.system()
    if dev_os == 'Windows':
        for drive in ascii_uppercase:
            if drive != 'C' and os.path.isdir('%s:\\' % drive):
                if os.path.isdir('%s:\\FidelityLock' % drive):
                    folder_list.append(drive)
    elif dev_os == 'Linux':
        txt = os.popen(ui.prefix + 'mount').read()
        dev_regex = '/dev/sd[a-z][1-9]?\s*on\s*(\S+)\s*type'
        drive_list = re.findall(dev_regex, txt)
        txt2 = os.popen(ui.prefix + 'blkid').read()
        dev_regex2 = '(/dev/sd[a-z][1-9]?.+)'
        all_list = re.findall(dev_regex2, txt2)
        r1 = '/dev/sd[a-z][1-9]?'
        r2 = 'TYPE="([a-z]+)"'
        for a in all_list:
            m1 = re.search(r1,a)
            m2 = re.search(r2,a)
            dev_a = m1.group(0)
            type_a = m2.group(1)
            if dev_a not in drive_list:
                s = os.system(ui.prefix + 'mount -t ' + type_a + ' ' + dev_a)
        txt = os.popen(ui.prefix + 'mount').read()
        dev_regex = '/dev/sd[a-z][1-9]?\s*on\s*(\S+)\s*type'
        drive_list = re.findall(dev_regex, txt)
        for d in drive_list:
            if os.path.isdir('%s/FidelityLock' % d):
                folder_list.append(d)
    if len(folder_list) == 0:
        #ui.msg_err('No password files found, check to make sure the USB is properly inserted.')
        return None
    else:
        pw = 'x'
        for i in range(len(folder_list)):
            filename = model + '_' + sn + '.psw'
            if dev_os == 'Windows':
                filepath = folder_list[i] + ":\\FidelityLock\\" + filename
            elif dev_os == 'Linux':
                filepath = folder_list[i] + "/FidelityLock/" + filename
            if os.path.isfile(filepath):
                ui.pass_usb = folder_list[i]
                f = open(filepath, 'r')
                txt = f.read()
                pswReg = 'Timestamp: [0-9]{14}\r?\n' + ui.auth_menu.get_active_text() + ': ([a-z0-9]{64})'
                entry = re.search(pswReg, txt)
                if entry:
                    pw = entry.group(1)
                f.close()
        print pw
        return pw

def passSaveUSB(ui, hashed_pwd, drive, mnum, snum, *args):
    dev_os = platform.system()
    timestamp = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
    f = None
    path = ''
    
    if dev_os == 'Windows':
        if ui.pass_usb != '':
            drive = ui.pass_usb + ":"
        if not os.path.isdir('%s\\FidelityLock' % drive):
            os.makedirs('%s\\FidelityLock' % drive)
        path = '' + drive + '\\FidelityLock\\' + mnum + '_' + snum + '.psw'
    elif dev_os == 'Linux':
        if ui.pass_usb != '':
            drive = ui.pass_usb
        if not os.path.isdir('%s/FidelityLock' % drive):
            os.makedirs('%s/FidelityLock' % drive)
        path = '' + drive + '/FidelityLock/' + mnum + '_' + snum + '.psw'
        print path
    if path != '':
        if os.path.isfile(path):
            f = open(path, 'r+')
            txt = f.read()
            f.seek(0)
            f.write('Model Number: ' + mnum + '\nSerial Number: ' + snum + '\n')
            entryReg = '(Timestamp: [0-9]{14}\r?\n([A-z0-9]+): [a-z0-9]{64})'
            entries = re.findall(entryReg, txt)
            for e in entries:
                if ui.auth_menu.get_active_text() != e[1]:
                    f.write('\n' + e[0])
            f.write('\n\nTimestamp: ' + timestamp + '\n' + ui.auth_menu.get_active_text() + ': ' + hashed_pwd)
            #ui.new_pass_entry.get_buffer().delete_text(0,-1)
            ui.confirm_pass_entry.get_buffer().delete_text(0,-1)
            f.truncate()
            f.close()
        else:
            f = open(path, 'w')
            f.write('Model Number: ' + mnum + '\nSerial Number: ' + snum + '\n')
            f.write('\n\nTimestamp: ' + timestamp + '\n' + ui.auth_menu.get_active_text() + ': ' + hashed_pwd)
            #ui.new_pass_entry.get_buffer().delete_text(0,-1)
            ui.confirm_pass_entry.get_buffer().delete_text(0,-1)
            f.close()

def run_setupUser(button, ui):
    index = ui.dev_select.get_active()
    ui.devname = ui.devs_list[ui.setup_list[index]]
    msid = ui.msid_list[ui.setup_list[index]]
    pw_u = ui.new_pass_entry.get_text()
    pw_u_confirm = ui.confirm_pass_entry.get_text()
    pw_u_trim = re.sub('\s', '', pw_u)
    pw_u_trim_confirm = re.sub(r'\s+', '', pw_u_confirm)
    if len(pw_u_trim) < 8:
        ui.msg_err("This password is too short.  Please enter a password at least 8 characters long excluding whitespace.")
    elif ui.bad_pw.has_key(pw_u_trim):
        ui.msg_err("This password is on the blacklist of bad passwords, please enter a stronger password.")
    elif pw_u_trim != pw_u_trim_confirm:
        ui.msg_err("The entered passwords do not match.")
    else:
        password_a = ''
        
        
        ui.start_spin()
        ui.wait_instr.show()
        
        def t1_run(e):
            notify_TO = False
            noPW = False
            if ui.VERSION == 3 and ui.check_pass_rd.get_active():
                ui.auth_menu.set_active(0)
                password_a = passReadUSB(ui, ui.dev_vendor.get_text(), ui.dev_sn.get_text())
                if password_a == None or password_a == 'x':
                    noPW = True
            else:
                password_a = lockhash.hash_pass(ui.pass_entry.get_text(), ui.salt_list[ui.setup_list[index]], ui.dev_msid.get_text())
            if e.isSet():
                notify_TO = True
                gobject.idle_add(cleanup, -1, notify_TO, noPW)
            elif not noPW:
                password_u = lockhash.hash_pass(ui.new_pass_entry.get_text(), ui.salt_list[ui.setup_list[index]], ui.dev_msid.get_text())
                if e.isSet():
                    notify_TO = True
                    gobject.idle_add(cleanup, -1, notify_TO, noPW)
                else:
                    s1 = -1
                    s2 = -1
                    s3 = -1
                    s1 = os.system(ui.prefix + "sedutil-cli -n -t --enableuser ON " + password_a + " User1 " + ui.devname)
                    
                    if s1 == 0:
                        if e.isSet():
                            ui.msg_err('Operation timed out.')
                            s1 = os.system(ui.prefix + "sedutil-cli -n -t --enableuser OFF " + password_a + " User1 " + ui.devname)
                        else:
                            s2 = os.system(ui.prefix + "sedutil-cli -n -t --enableuserread ON " + password_a + " User1 " + ui.devname)
                            if e.isSet():
                                ui.msg_err('Operation timed out.')
                                s1 = os.system(ui.prefix + "sedutil-cli -n -t --enableuser OFF " + password_a + " User1 " + ui.devname)
                            else:
                                s3 = os.system(ui.prefix + "sedutil-cli -n -t --setpassword " + password_a + " User1 " + password_u + " " + ui.devname)
                                if ui.pass_sav.get_active():
                                    ui.auth_menu.set_active(1)
                                    print "setupUser passSaveUSB " + password_u
                                    passSaveUSB(ui, password_u, ui.drive_menu.get_active_text(), ui.dev_vendor.get_text(), ui.dev_sn.get_text())
                                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                                    timeStr = timeStr[2:]
                                    statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 18" + timeStr + " " + password_a + " Admin1 " + ui.devname)
                                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                                timeStr = timeStr[2:]
                                statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 03" + timeStr + " " + password_a + " Admin1 " + ui.devname)
                                statusAW1 = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 09" + timeStr + " " + password_a + " Admin1 " + ui.devname)
                                statusAW1 = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 13" + timeStr + " " + password_a + " Admin1 " + ui.devname)
                    
                    gobject.idle_add(cleanup, s1|s2|s3, notify_TO, noPW)
            else:
                gobject.idle_add(cleanup, -1, notify_TO, noPW)
        def cleanup(status, op_to, no_pw):
            ui.stop_spin()
            t1.join()
            if no_pw:
                ui.msg_err('Password not found.')
            if status !=0 :
                if op_to:
                    ui.msg_err('Operation timed out.')
                if status == ui.NOT_AUTHORIZED or status == ui.AUTHORITY_LOCKED_OUT:
                    pwd = 'F0iD2eli81Ty' + ui.salt_list[ui.setup_list[index]]
                    hash_pwd = lockhash.hash_pass(pwd, ui.salt_list[ui.setup_list[index]], ui.msid_list[ui.setup_list[index]])
                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                    timeStr = timeStr[2:]
                    statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 04" + timeStr + " " + hash_pwd + " User" + ui.user_list[ui.setup_list[index]] + " " + ui.devname)
                    if status == ui.AUTHORITY_LOCKED_OUT:
                        statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 10" + timeStr + " " + hash_pwd + " User" + ui.user_list[ui.setup_list[index]] + " " + ui.devname)
                        ui.msg_err("Error: User setup for " + ui.devname + " failed. Retry limit has been reached, power cycle the drive to try again.")
                    else:
                        ui.msg_err("Error: User setup for " + ui.devname + " failed. Invalid Admin password.")
                else:
                    ui.msg_err("Error: User setup for " + ui.devname + " failed.")
                ui.op_instr.show()
                ui.box_pass.show()
                ui.box_newpass.show()
                ui.box_newpass_confirm.show()
                ui.check_box_pass.show()
                ui.cancel_button.show()
            else:
                ui.query(None,1)
                ui.msg_ok("User Password for " + ui.devname + " set up successfully.")
                ui.returnToMain()
                    
        e = threading.Event()
        t1 = threading.Thread(target=t1_run, args=(e,))
        t1.start()
        start_time = time.time()
        t2 = threading.Thread(target=timeout_track, args=(ui, 30.0, start_time, t1, e))
        t2.start()
        
def run_removeUser(button, ui):
    index = ui.dev_select.get_active()
    ui.devname = ui.devs_list[ui.setup_list[index]]
    msid = ui.msid_list[ui.setup_list[index]]
    pw = ui.pass_entry.get_text()
    pw_trim = re.sub('\s', '', pw)
    password_a = ''
    
    ui.start_spin()
    ui.wait_instr.show()
    
    def t1_run(e):
        no_pw = False
        if ui.VERSION == 3 and ui.check_pass_rd.get_active():
            ui.auth_menu.set_active(0)
            password_a = passReadUSB(ui, ui.dev_vendor.get_text(), ui.dev_sn.get_text())
            if password_a == None or password_a == 'x':
                noPW = True
        else:
            password_a = lockhash.hash_pass(pw_trim, ui.salt_list[ui.setup_list[index]], ui.dev_msid.get_text())
        rc = -1
        notify_TO = False
        if e.isSet():
            notify_TO = True
        #s1 = os.system(ui.prefix + "sedutil-cli -n -t --enableuserread OFF " + password_a + " User1 " + ui.devname)
        elif not noPW:
            f1 = os.popen(ui.prefix + "sedutil-cli -n -t --getmbrsize " + password_a + " " + ui.devname)
            f_txt = f1.read()
            rc = f1.close()
            userRegex = 'User1\s*=\s*1'
            m = re.search(userRegex,f_txt)
            if e.isSet():
                notify_TO = True
                rc = -1
            elif m:
                s2 = os.system(ui.prefix + "sedutil-cli -n -t --enableuser OFF " + password_a + " User1 " + ui.devname)
                if ui.pass_sav.get_active():
                    ui.auth_menu.set_active(0)
                    print "removeUser passSaveUSB " + password_a
                    passSaveUSB(ui, password_a, ui.drive_menu.get_active_text(), ui.dev_vendor.get_text(), ui.dev_sn.get_text())
                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                    timeStr = timeStr[2:]
                    statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 18" + timeStr + " " + password_a + " Admin1 " + ui.devname)
                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                timeStr = timeStr[2:]
                statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 03" + timeStr + " " + password_a + " Admin1 " + ui.devname)
                statusAW1 = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 10" + timeStr + " " + password_a + " Admin1 " + ui.devname)
                rc = 0
        gobject.idle_add(cleanup, rc, notify_TO, noPW)
    
    def cleanup(status, op_to, no_pw):
        ui.stop_spin()
        t1.join()
        if status == None:
            ui.msg_err('User is not enabled on this drive')
        elif no_pw:
            ui.msg_err('Password not found.')
        elif status !=0 :
            pwd = 'F0iD2eli81Ty' + ui.salt_list[ui.setup_list[index]]
            hash_pwd = lockhash.hash_pass(pwd, ui.salt_list[ui.setup_list[index]], ui.msid_list[ui.setup_list[index]])
            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
            timeStr = timeStr[2:]
            statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 04" + timeStr + " " + hash_pwd + " User" + ui.user_list[ui.setup_list[index]] + " " + ui.devname)
            if status == ui.NOT_AUTHORIZED:
                ui.msg_err("Error: Removing user for " + ui.devname + " failed. Invalid password.")
            elif status == ui.AUTHORITY_LOCKED_OUT:
                ui.msg_err("Error: Removing user for " + ui.devname + " failed. Retry limit reached, please power cycle the drive to try again.")
            else:
                ui.msg_err("Error: Removing user for " + ui.devname + " failed.")
            ui.op_instr.show()
            ui.box_pass.show()
            ui.box_newpass.show()
            ui.box_newpass_confirm.show()
            ui.check_box_pass.show()
            ui.cancel_button.show()
        else :
            ui.query(None,1)
            ui.msg_ok("User removed on " + ui.devname + " set up successfully.")
            ui.returnToMain()
                
    e = threading.Event()
    t1 = threading.Thread(target=t1_run, args=(e,))
    t1.start()
    start_time = time.time()
    t2 = threading.Thread(target=timeout_track, args=(ui, 30.0, start_time, t1, e))
    t2.start()
        
def run_setupUSB(button, ui, *args):
    dev1 = ui.dev_select.get_active_text()
    index = ui.dev_select.get_active()
    index2 = ui.usb_menu.get_active()
    
    ui.start_spin()
    ui.wait_instr.show()
    
    def t1_run():
        dev_os = platform.system()
        if dev_os == 'Windows':
            ui.process =  subprocess.Popen([ui.prefix + "sedutil-cli", "--createUSB", "UEFI", dev1, " \\\\.\\PhysicalDrive" + ui.usb_list[index2][0]])
            ui.process.communicate()
            status1 = ui.process.returncode
            gobject.idle_add(cleanup, status1)
        elif dev_os == 'Linux':
            ui.process =  subprocess.Popen([ui.prefix + "sedutil-cli", "--createUSB", "UEFI", dev1, ui.usb_list[index2][0]])
            ui.process.communicate()
            status1 = ui.process.returncode
            gobject.idle_add(cleanup, status1)
    def cleanup(status):
        ui.stop_spin()
        t1.join()
        if status != 0 :
            ui.msg_err("Error: Setup USB failed")
            ui.op_instr.show()
            ui.setupUSB_button.show()
            ui.cancel_button.show()
        else :
            if ui.pass_sav.get_active():
                dev_os = platform.system()
                hash_pwd = ''
                if ui.check_pass_rd.get_active():
                    ui.auth_menu.set_active(0)
                    hash_pwd = passReadUSB(ui, ui.dev_vendor.get_text(), ui.dev_sn.get_text())
                    if hash_pwd == None or password_a == 'x':
                        ui.msg_err('No password found for the drive.')
                        return
                    ui.pass_usb = ''
                else:
                    pw_trim = re.sub('\s', '', ui.pass_entry.get_text())
                    hash_pwd = lockhash.hash_pass(pw_trim, ui.salt_list[ui.setup_list[index]], ui.dev_msid.get_text())
                ui.auth_menu.set_active(0)
                print "setupUSB passSaveUSB " + hash_pwd
                #Windows usb_list: disk number, disk name
                #search through drive letters to find the matching disk number
                if dev_os == 'Windows':
                    usb_drive = ''
                    for drive in ascii_uppercase:
                        if usb_drive == '' and drive != 'C' and os.path.isdir('%s:\\' % drive):
                            txt = os.popen("powershell -NoProfile -NoExit -Command Get-Disk (Get-Partition -DriveLetter '" + drive + "').DiskNumber").read()
                            disknum_regex = '\n' + ui.usb_list[index2][0]
                            print disknum_regex
                            m = re.search(disknum_regex, txt)
                            if m:
                                usb_drive = drive + ':'
                    if usb_drive != '':
                        passSaveUSB(ui, hash_pwd, usb_drive, ui.dev_vendor.get_text(), ui.dev_sn.get_text())
                    else:
                        msg_err('Partition could not be found to save password.')
                if dev_os == 'Linux':
                    passSaveUSB(ui, hash_pwd, ui.usb_list[index2][1], ui.dev_vendor.get_text(), ui.dev_sn.get_text())
                
            ui.msg_ok("PBA USB Setup Complete")
            
            ui.returnToMain()
            
    e = threading.Event()
    t1 = threading.Thread(target=t1_run, args=())
    t1.start()
    start_time = time.time()
    t2 = threading.Thread(target=timeout_track, args=(ui, 60.0, start_time, t1, e))
    t2.start()
    
def filterLog(button, ui, entries, mode):
    ui.listStore.clear()
    for i in range(len(entries)):
        ui.listStore.append(entries[i])
    ui.viewAll_button.set_sensitive(mode != 0)
    ui.viewWarnErr_button.set_sensitive(mode != 1)
    ui.viewErr_button.set_sensitive(mode != 2)

def timeout_track(ui, max_time, start_time, op_thread, e):
    status = 0
    done = False
    while not done:
        if op_thread.isAlive():
            curr_time = time.time()
            elapsed_time = curr_time - start_time
            if elapsed_time >= max_time:
                done = True
        else:
            done = True
    
    gobject.idle_add(timeout_cleanup, ui, max_time, start_time, op_thread, e)

def timeout_cleanup(ui, max_time, start_time, op_thread, e):
    if op_thread.isAlive():
        curr_time = time.time()
        elapsed_time = curr_time - start_time
        if elapsed_time >= max_time:
            if ui.process != None:
                ui.process.terminate()
                ui.process = None
            op_thread.join(0.0)
            e.set()
            #ui.msg_err("Operation timed out.")
    
    ui.stop_spin()
