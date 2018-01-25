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
 
    names = ['PhysicalDrive[0-9]', '/dev/sd[a-z]', '/dev/nvme[0-9]',  '/dev/disk[0-9]']
    idx = 0
    ui.devs_list = []
    ui.locked_list = []
    ui.setup_list = []
    ui.unlocked_list = []
    ui.nonsetup_list = []
    ui.tcg_list = []
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
                            else:
                                ui.opal_ver_list.append(md.group(1))
                        ui.vendor_list.append(md.group(2))
                        ui.series_list.append(md.group(3))
                        ui.salt_list.append(md.group(4).ljust(20))
                        ui.sn_list.append(md.group(4).replace(' ',''))
                        
    if ui.devs_list != []:
        for i in range(len(ui.devs_list)):
            queryText = os.popen(ui.prefix + 'sedutil-cli --query ' + ui.devs_list[i]).read()
            
            txt_msid = os.popen(ui.prefix + "sedutil-cli --printDefaultPassword " + ui.devs_list[i] ).read()
            
            
            msid = 'N/A'
            if txt_msid != '' :
                regex_msid = 'MSID:\s*([A-z0-9]*)'
                mm = re.search(regex_msid, txt_msid)
                if mm:
                    msid = mm.group(1)
            
            ui.msid_list.append(msid)
            
            ui.pba_list.append("N/A")
            
            txt_TCG = "Locked = "
            txt_L = "Locked = Y"
            txt_S = "LockingEnabled = Y"
            msidText = os.popen(ui.prefix + 'sedutil-cli -n -t --getmbrsize ' + msid + ' ' + ui.devs_list[i]).read()
            m = re.search('Shadow', msidText)
            isTCG = re.search(txt_TCG, queryText)
            isLocked = re.search(txt_L, queryText)
            isSetup = (re.search(txt_S, queryText) != None) & (not m)
            if isTCG:
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
            else:
                ui.lockstatus_list.append("N/A")
                ui.setupstatus_list.append("N/A")
        print ("devs_list: ",  ui.devs_list)
        print ("vendor_list: ", ui.vendor_list)
        print ("opal_ver_list: ", ui.opal_ver_list)
        print ("sn_list: ", ui.sn_list)	

def run_setupFull(button, ui, mode):
    index = ui.dev_select.get_active()
    ui.devname = ui.devs_list[ui.nonsetup_list[index]]
    msid = ui.msid_list[ui.nonsetup_list[index]]
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
        message1 = gtk.MessageDialog(type=gtk.MESSAGE_WARNING, buttons=gtk.BUTTONS_OK_CANCEL)
        message1.set_markup("Warning: If you lose your password, all data will be lost. Do you want to proceed?")
        res1 = message1.run()
        if res1 == gtk.RESPONSE_OK:
            message2 = gtk.MessageDialog(type=gtk.MESSAGE_WARNING, buttons=gtk.BUTTONS_OK_CANCEL)
            message2.set_markup("Final Warning: If you lose your password, all data will be lost. Are you sure you want to proceed?")
            res2 = message2.run()
            if res2 == gtk.RESPONSE_OK:
                message2.destroy()
                message1.destroy()
                password = lockhash.hash_pass(ui.new_pass_entry.get_text(), ui.salt_list[ui.nonsetup_list[index]], ui.dev_msid.get_text())
                ui.start_spin()
                ui.wait_instr.show()
                
                def t1_run():
                    queryText = os.popen(ui.prefix + 'sedutil-cli --query ' + ui.devname).read()
                    txt_LE = "LockingEnabled = N"
                    txt_ME = "MBREnabled = N"
                    unlocked = re.search(txt_LE, queryText)
                    activated = re.search(txt_ME, queryText)
                    status1 = -1
                    if unlocked:
                        status1 =  os.system(ui.prefix + "sedutil-cli -n -t --initialSetup " + password + " " + ui.devname )
                    
                        timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                        timeStr = timeStr[2:]
                        statusAW3 = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 01" + timeStr + " " + password + " Admin1 " + ui.devname)
                    
                        timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                        timeStr = timeStr[2:]
                        statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 10" + timeStr + " " + password + " Admin1 " + ui.devname)
                        timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                        timeStr = timeStr[2:]
                        statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 11" + timeStr + " " + password + " Admin1 " + ui.devname)
                    elif activated:
                        s1 = os.system(ui.prefix + "sedutil-cli -n -t --setSIDPassword " + msid + " " + password + " " + ui.devname)
                        s2 = os.system(ui.prefix + "sedutil-cli -n -t --setAdmin1Pwd " + msid + " " + password + " " + ui.devname)
                        timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                        timeStr = timeStr[2:]
                        statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 10" + timeStr + " " + password + " Admin1 " + ui.devname)
                        timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                        timeStr = timeStr[2:]
                        statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 11" + timeStr + " " + password + " Admin1 " + ui.devname)
                        status1 = (s1 | s2)
                    
                    status2 = ''
                    status3 = ''
                    status2 =  os.system(ui.prefix + "sedutil-cli -n -t --enableLockingRange " + ui.LKRNG + " " + password + " " + ui.devname )
                    status3 =  os.system(ui.prefix + "sedutil-cli -n -t --setMBRdone on " + password + " " + ui.devname )
                    gobject.idle_add(cleanup, status1 | status2 | status3)
                
                def cleanup(status):
                    ui.stop_spin()
                    t1.join()
                    if status !=0 :
                        ui.msg_err("Error: Setup of " + ui.devname + " failed. Try again.")
                        ui.op_instr.show()
                        ui.box_newpass.show()
                        ui.box_newpass_confirm.show()
                        ui.check_box_pass.show()
                        ui.setup_next.show()
                        ui.go_button_cancel.show()
                    else : 
                        ui.setupstatus_list[ui.nonsetup_list[index]] = "Yes"
                        if ui.VERSION == 3 and ui.pass_sav.get_active():
                            passSaveUSB(ui)
                        if mode == 1:
                            status5 =  os.system(ui.prefix + "sedutil-cli -n -t --enableLockingRange " + ui.LKRNG + " " + password + " " + ui.devname )
                            status6 =  os.system(ui.prefix + "sedutil-cli -n -t --setLockingrange " + ui.LKRNG + " LK " + password + " " + ui.devname )
                            status7 =  os.system(ui.prefix + "sedutil-cli -n -t --setMBRDone on " + password + " " + ui.devname )
                        ui.query(1)
                        ui.msg_ok("Password for " + ui.devname + " set up successfully.")
                        if mode == 0:
                            ui.setup_prompt2()
                        else:
                            ui.lockstatus_list[ui.nonsetup_list[index]] = "Locked"
                            ui.updateDevs(ui.nonsetup_list[index],[1,2])
                            ui.returnToMain()
                            
                t1 = threading.Thread(target=t1_run, args=())
                t1.start()
                start_time = time.time()
                t2 = threading.Thread(target=timeout_track, args=(ui, 10.0, start_time, t1))
                t2.start()

def run_pbaWrite(button, ui, mode):
    status = -1
    password = ""
    index = ui.dev_select.get_active()
    if mode == 0: #update, add usb check
        ui.devname = ui.devs_list[ui.setup_list[index]]
        if ui.VERSION == 3 and ui.check_pass_rd.get_active():
            password = passReadUSB(ui, ui.dev_vendor.get_text(), ui.dev_sn.get_text())
            if password == None or password == 'x':
                ui.msg_err('No password found for the drive.')
                return
        else:
            password = lockhash.hash_pass(ui.pass_entry.get_text(), ui.salt_list[ui.setup_list[index]], ui.dev_msid.get_text())
    else:
        ui.devname = ui.devs_list[ui.nonsetup_list[index]]
        password = lockhash.hash_pass(ui.new_pass_entry.get_text(), ui.salt_list[ui.nonsetup_list[index]], ui.dev_msid.get_text())
    ui.start_spin()
    ui.pba_wait_instr.show()
    
    def t1_run():
        if password != None:
            status = os.system( ui.prefix + "sedutil-cli -n -t --loadpbaimage " + password + " n " + ui.devname )
            gobject.idle_add(cleanup, status)
        else:
            gobject.idle_add(cleanup, 1)
        
        
    def cleanup(status):
        ui.stop_spin()
        t1.join()
        if status != 0 :
            ui.msg_err("Error: Writing PBA image to " + ui.devname + " failed.")
            
            ui.op_instr.show()
            if mode == 1:
                ui.setupLockOnly.show()
                ui.setupLockPBA.show()
            else:
                ui.box_pass.show()
                ui.check_box_pass.show()
                ui.updatePBA_button.show()
                ui.go_button_cancel.show()
        else :
            status2 = ''
            p = ''
            status2 =  os.system(ui.prefix + "sedutil-cli -n -t --setMBREnable on " + password + " " + ui.devname )
            p = os.popen(ui.prefix + "sedutil-cli -n -t --pbaValid " +  password + " " + ui.devname).read()
            pba_regex = 'PBA image version\s*:\s*(.+)'
            m1 = re.search(pba_regex, p)
            if m1:
                pba_ver = m1.group(1)
                ui.msg_ok("PBA image " + pba_ver + " written to " + ui.devname + " successfully.")
                if mode == 0:
                    ui.pba_list[ui.setup_list[index]] = pba_ver
                else:
                    ui.pba_list[ui.nonsetup_list[index]] = pba_ver
            if mode == 1:
                ui.setup_finish()
            else:
                ui.returnToMain()
    
    t1 = threading.Thread(target=t1_run, args=())
    t1.start()
    start_time = time.time()
    t2 = threading.Thread(target=timeout_track, args=(ui, 300.0, start_time, t1))
    t2.start()

def run_changePW(button, ui):
    index = ui.dev_select.get_active()
    old_hash = ""
    if ui.VERSION == 3 and ui.check_pass_rd.get_active():
        old_hash = passReadUSB(ui, ui.dev_vendor.get_text(), ui.dev_sn.get_text())
        if old_hash == None or old_hash == 'x':
            ui.msg_err('No password found for the drive.')
            return
    else:
        old_pass = ui.pass_entry.get_text()
        old_trim = re.sub('\s', '', old_pass)
        old_hash = lockhash.hash_pass(old_trim, ui.salt_list[ui.setup_list[index]], ui.dev_msid.get_text())
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
        index = ui.dev_select.get_active()
        ui.devname = ui.devs_list[ui.setup_list[index]]
        new_hash = lockhash.hash_pass(ui.new_pass_entry.get_text(), ui.salt_list[ui.setup_list[index]], ui.dev_msid.get_text())
        ui.start_spin()
        ui.wait_instr.show()
        level = ui.auth_menu.get_active()
        def t1_run():
            status = -1
            if ui.VERSION == 3 and level == 1:
                status = os.system(ui.prefix + "sedutil-cli -n -t -u --setpassword " + old_hash + " User1 " + new_hash + " " + ui.devname)
            else:
                status1 = os.system(ui.prefix + "sedutil-cli -n -t --setSIDPassword " + old_hash + " " + new_hash + " " + ui.devname)
                status2 = os.system(ui.prefix + "sedutil-cli -n -t --setAdmin1Pwd " + old_hash + " " + new_hash + " " + ui.devname)
                if status1 == ui.AUTHORITY_LOCKED_OUT or status2 == ui.AUTHORITY_LOCKED_OUT:
                    status = ui.AUTHORITY_LOCKED_OUT
                elif status1 == ui.NOT_AUTHORIZED or status2 == ui.NOT_AUTHORIZED:
                    status = ui.NOT_AUTHORIZED
                else:
                    status = status1 | status2
            gobject.idle_add(cleanup, status)
            
        def cleanup(status):
            if status != 0:
                if status == ui.AUTHORITY_LOCKED_OUT:
                    ui.msg_err("Error: You have been locked out of " + ui.devname + " due to multiple failed authentication attempts.  Please reboot and try again.")
                elif status == ui.NOT_AUTHORIZED:
                    ui.msg_err("Error: Incorrect password, please try again.")
                else:
                    ui.msg_err("Error: Something went wrong. Error code: " + str(status))
            else :
                if ui.VERSION == 3 and ui.pass_sav.get_active():
                    passSaveUSB(ui)
                t1.join()
                ui.stop_spin()
                if ui.VERSION == 3 and level == 1:
                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                    timeStr = timeStr[2:]
                    statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 10" + timeStr + " " + new_hash + " User1 " + ui.devname)
                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                    timeStr = timeStr[2:]
                    statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 11" + timeStr + " " + new_hash + " User1 " + ui.devname)
                elif ui.VERSION != 1:
                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                    timeStr = timeStr[2:]
                    statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 10" + timeStr + " " + new_hash + " Admin1 " + ui.devname)
                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                    timeStr = timeStr[2:]
                    statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 11" + timeStr + " " + new_hash + " Admin1 " + ui.devname)
                ui.msg_ok("Password for " + ui.devname + " changed successfully.") 
                
                if ui.auth_menu.get_active() != 1 and ui.pba_list[ui.setup_list[index]] == 'N/A':
                    p = ''
                    p = os.popen(ui.prefix + "sedutil-cli -n -t --pbaValid " + new_hash + " " + ui.devname).read()
                    pba_regex = 'PBA image version\s*:\s*(.+)'
                    m1 = re.search(pba_regex, p)
                    if m1:
                        pba_ver = m1.group(1)
                        ui.pba_list[ui.setup_list[index]] = pba_ver
                
                ui.returnToMain()
            
        t1 = threading.Thread(target=t1_run, args=())
        t1.start()
        start_time = time.time()
        t2 = threading.Thread(target=timeout_track, args=(ui, 10.0, start_time, t1))
        t2.start()

def run_revertUser(button, ui):
    if ui.eraseData_check.get_active() == True:
        message = gtk.MessageDialog(type=gtk.MESSAGE_WARNING, buttons=gtk.BUTTONS_OK_CANCEL)
        message.set_markup("Warning : Revert with password erase all data. Do you want to proceed?")
        res = message.run()
        print message.get_widget_for_response(gtk.RESPONSE_OK)
        print gtk.RESPONSE_OK
        if res == gtk.RESPONSE_OK :
            messageA = gtk.MessageDialog(type=gtk.MESSAGE_WARNING, buttons=gtk.BUTTONS_OK_CANCEL)
            messageA.set_markup("Warning Warning Warning : Revert with password erase all data. Do you really want to proceed?")
            resA = messageA.run()

            if resA == gtk.RESPONSE_OK : 
                messageA.destroy()
                message.destroy()
                ui.start_spin()
                ui.wait_instr.show()
                password = ""
                index = ui.dev_select.get_active()
                if ui.VERSION == 3 and ui.check_pass_rd.get_active():
                    password = passReadUSB(ui, ui.dev_vendor.get_text(), ui.dev_sn.get_text())
                    if password == None or password == 'x':
                        ui.msg_err('No password found for the drive.')
                        return
                else:
                    password = lockhash.hash_pass(ui.pass_entry.get_text(), ui.salt_list[ui.setup_list[index]], ui.dev_msid.get_text())
                def t1_run():
                    if password == None or password == 'x':
                        gobject.idle_add(cleanup, 0, 1)
                    else:
                        
                        ui.devname = ui.devs_list[ui.setup_list[index]]
                        timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                        timeStr = timeStr[2:]
                        if ui.VERSION != 1:
                            statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 15" + timeStr + " " + password + " Admin1 " + ui.devname)
                        status = os.system(ui.prefix + "sedutil-cli -n -t --revertTPer " + password + " " + ui.devname)
                        gobject.idle_add(cleanup, status)
                    
                def cleanup(status):
                    t1.join()
                    ui.stop_spin()
                    if status != 0 :
                        if status == ui.NOT_AUTHORIZED:
                            ui.msg_err("Error: Invalid password, try again.")
                        elif status == ui.AUTHORITY_LOCKED_OUT:
                            ui.msg_err("Error: Locked out due to multiple failed attempts.  Please reboot and try again.")
                        else:
                            ui.msg_err("Error: Something went wrong. Error code: " + str(status))
                    else :
                        index = ui.dev_select.get_active()
                        dev_msid = ui.msid_list[ui.setup_list[index]]
                        
                        status = os.system(ui.prefix + "sedutil-cli -n -t --activate " + dev_msid + " " + ui.devname)
                        
                        timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                        timeStr = timeStr[2:]
                        statusAE = os.system(ui.prefix + "sedutil-cli -n -t --auditerase " + dev_msid + " Admin1 " + ui.devname)
                        
                        if ui.VERSION != 1:
                            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                            timeStr = timeStr[2:]
                            statusAW1 = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 05" + timeStr + " " + dev_msid + " Admin1 " + ui.devname)
                            
                            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                            timeStr = timeStr[2:]
                            statusAW2 = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 08" + timeStr + " " + dev_msid + " Admin1 " + ui.devname)
                            
                            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                            timeStr = timeStr[2:]
                            statusAW3 = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 01" + timeStr + " " + dev_msid + " Admin1 " + ui.devname)
                        ui.msg_ok("Device " + ui.devname + " successfully reverted with password.")
                        index = ui.dev_select.get_active()
                        ui.query(1)
                        ui.lockstatus_list[ui.setup_list[index]] = "Unlocked"
                        ui.setupstatus_list[ui.setup_list[index]] = "No"
                        ui.updateDevs(ui.setup_list[index],[4])
                        ui.returnToMain()
                t1 = threading.Thread(target=t1_run, args=())
                t1.start()
                start_time = time.time()
                t2 = threading.Thread(target=timeout_track, args=(ui, 10.0, start_time, t1))
                t2.start()
        
    else:
        index = ui.dev_select.get_active()
        ui.devname = ui.devs_list[ui.setup_list[index]]
        ui.start_spin()
        ui.wait_instr.show()
        password = ''
        if ui.VERSION == 3 and ui.check_pass_rd.get_active():
            password = passReadUSB(ui, ui.dev_vendor.get_text(), ui.dev_sn.get_text())
            if password == None or password == 'x':
                ui.msg_err('No password found for the drive.')
                return
        else:
            password = lockhash.hash_pass(ui.pass_entry.get_text(), ui.salt_list[ui.setup_list[index]], ui.dev_msid.get_text())
        
        def t1_run():
            if password == None or password == 'x':
                gobject.idle_add(cleanup, 1, 0)
            
            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
            timeStr = timeStr[2:]
            statusAW = -1
            if ui.VERSION != 1:
                statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 13" + timeStr + " " + password + " Admin1 " + ui.devname)
            if statusAW == 0 or ui.VERSION == 1:
                status1 =  os.system(ui.prefix + "sedutil-cli -n -t --setMBRdone on " + password + " " + ui.devname )
                status2 =  os.system(ui.prefix + "sedutil-cli -n -t --setLockingRange " + ui.LKRNG + " " 
                        + ui.LKATTR + " " + password + " " + ui.devname)
                status = os.system(ui.prefix + "sedutil-cli -n -t --revertnoerase " + password + " " + ui.devname)
                gobject.idle_add(cleanup, status)
            else:
                gobject.idle_add(cleanup, statusAW)
            
        def cleanup(status):
            ui.stop_spin()
            t1.join()
            if status == ui.NOT_AUTHORIZED :
                ui.msg_err("Error: Invalid password, try again.")
            elif status == ui.AUTHORITY_LOCKED_OUT :
                ui.msg_err("Error: Locked out due to multiple failed attempts.  Please reboot and try again.")
            else :
                p0 = os.popen(ui.prefix + "sedutil-cli --query " + ui.devname).read()
                txtLE = "LockingEnabled = N"
                le_check = re.search(txtLE, p0)
                if le_check:
                    index = ui.dev_select.get_active()
                    dev_msid = ui.msid_list[ui.setup_list[index]]
                    p1 = os.system(ui.prefix + "sedutil-cli -n -t --revertTPer " + password + " " + ui.devname)
                    status = os.system(ui.prefix + "sedutil-cli -n -t --activate " + dev_msid + " " + ui.devname)
                    if ui.VERSION != 1:
                        timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                        timeStr = timeStr[2:]
                        statusAW1 = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 04" + timeStr + " " + dev_msid + " Admin1 " + ui.devname)
                        
                        timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                        timeStr = timeStr[2:]
                        statusAW2 = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 01" + timeStr + " " + dev_msid + " Admin1 " + ui.devname)
                    ui.msg_ok("Device " + ui.devname + " successfully reverted with password.")
                    index = ui.dev_select.get_active()
                    ui.query(1)
                    ui.lockstatus_list[ui.setup_list[index]] = "Unlocked"
                    ui.setupstatus_list[ui.setup_list[index]] = "No"
                    ui.updateDevs(ui.setup_list[index],[4])
                    ui.returnToMain()
                else:
                    ui.msg_err("Error: Revert failed.")
        t1 = threading.Thread(target=t1_run, args=())
        t1.start()
        start_time = time.time()
        t2 = threading.Thread(target=timeout_track, args=(ui, 15.0, start_time, t1))
        t2.start()

def run_revertPSID(button, ui):
    message = gtk.MessageDialog(type=gtk.MESSAGE_WARNING, buttons=gtk.BUTTONS_OK_CANCEL)
    message.set_markup("Warning : Revert with PSID erase all data. Do you want to proceed?")
    res = message.run()
    print message.get_widget_for_response(gtk.RESPONSE_OK)
    print gtk.RESPONSE_OK
    if res == gtk.RESPONSE_OK :
        messageA = gtk.MessageDialog(type=gtk.MESSAGE_WARNING, buttons=gtk.BUTTONS_OK_CANCEL)
        messageA.set_markup("Warning Warning Warning : Revert with PSID erase all data. Do you really want to proceed?")
        resA = messageA.run()

        if resA == gtk.RESPONSE_OK :
            messageA.destroy()
            message.destroy()
            ui.start_spin()
            ui.wait_instr.show()
            index = ui.dev_select.get_active()
            
            def t1_run():            
                psid = ui.revert_psid_entry.get_text()
                
                ui.devname = ui.devs_list[ui.tcg_list[index]]
                
                status =  os.system(ui.prefix + "sedutil-cli -n -t --yesIreallywanttoERASEALLmydatausingthePSID " + psid + " " + ui.devname )
                gobject.idle_add(cleanup, status)
            
            def cleanup(status):
                ui.stop_spin()
                t1.join()
                if status != 0 :
                    ui.msg_err("Error: Incorrect PSID, please try again." )
                else :
                    index = ui.dev_select.get_active()
                    dev_msid = ui.msid_list[ui.tcg_list[index]]
                    status = os.system(ui.prefix + "sedutil-cli -n -t --activate " + dev_msid + " " + ui.devname)
                    if ui.VERSION != 1:
                        timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                        timeStr = timeStr[2:]
                        statusAW1 = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 06" + timeStr + " " + dev_msid + " Admin1 " + ui.devname)
                        
                        timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                        timeStr = timeStr[2:]
                        statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 08" + timeStr + " " + dev_msid + " Admin1 " + ui.devname)
                        
                        timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                        timeStr = timeStr[2:]
                        statusAW2 = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 01" + timeStr + " " + dev_msid + " Admin1 " + ui.devname)
                    ui.msg_ok("Device " + ui.devname + " successfully reverted with PSID.")
                    index = ui.dev_select.get_active()
                    ui.query(1)
                    ui.lockstatus_list[ui.tcg_list[index]] = "Unlocked"
                    ui.setupstatus_list[ui.tcg_list[index]] = "No"
                    ui.updateDevs(ui.tcg_list[index],[4])
                    ui.returnToMain()
            
            t1 = threading.Thread(target=t1_run, args=())
            t1.start()
            start_time = time.time()
            t2 = threading.Thread(target=timeout_track, args=(ui, 10.0, start_time, t1))
            t2.start()

def run_lockset(button, ui):
    index = ui.dev_select.get_active()
    ui.devname = ui.devs_list[ui.unlocked_list[index]]
    ui.start_spin()
    ui.wait_instr.show()
    password = ""
    if ui.VERSION == 3 and ui.check_pass_rd.get_active():
        password = passReadUSB(ui, ui.dev_vendor.get_text(), ui.dev_sn.get_text())
        if password == None or password == 'x':
            ui.msg_err('No password found for the drive.')
            return
    else:
        password = lockhash.hash_pass(ui.pass_entry.get_text(), ui.salt_list[ui.unlocked_list[index]], ui.dev_msid.get_text())
    def t1_run():
        
        if password == None or password == 'x':
            gobject.idle_add(cleanup, 1)
        else:
            status1 =  os.system(ui.prefix + "sedutil-cli -n -t --enableLockingRange " + ui.LKRNG + " " 
                    + password + " " + ui.devname )
            status2 =  os.system(ui.prefix + "sedutil-cli -n -t --setMBRDone on " + password + " " + ui.devname )
            status3 =  os.system(ui.prefix + "sedutil-cli -n -t --setMBREnable on " + password + " " + ui.devname )
            if ui.devname != '\\\\.\\PhysicalDrive0' and ui.devname != '/dev/sda' and ui.devname != '/nvme':
                status4 =  os.system(ui.prefix + "sedutil-cli -n -t --setLockingrange " + ui.LKRNG + " LK "
                        + password + " " + ui.devname )
                status = status1 | status2 | status3 | status4
                gobject.idle_add(cleanup, status, 1)
            else:
                status = status1 | status2 | status3
                gobject.idle_add(cleanup, status, 0)
        
    def cleanup(status, mode):
        ui.stop_spin()
        t1.join()
        if (status) != 0 :
            ui.msg_err("Error while attempting to lock " + ui.devname + '.')
        else :
            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
            timeStr = timeStr[2:]
            statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 02" + timeStr + " " + password + " Admin1 " + ui.devname)
            if mode == 1:
                ui.msg_ok("Drive " + ui.devname + "locked successfully.") 
                ui.lockstatus_list[ui.unlocked_list[index]] = "Locked"
            else:
                ui.msg_ok("Locking enabled on drive " + ui.devname + ".") 
            
            if ui.pba_list[ui.unlocked_list[index]] == 'N/A':
                p = os.popen(ui.prefix + "sedutil-cli -n -t --pbaValid " + password + " " + ui.devname).read()
                pba_regex = 'PBA image version\s*:\s*(.+)'
                m1 = re.search(pba_regex, p)
                if m1:
                    pba_ver = m1.group(1)
                    ui.pba_list[ui.unlocked_list[index]] = pba_ver
            
            ui.query(1)
            
            ui.updateDevs(ui.unlocked_list[index],[1,2])
            ui.returnToMain()

    t1 = threading.Thread(target=t1_run, args=())
    t1.start()
    start_time = time.time()
    t2 = threading.Thread(target=timeout_track, args=(ui, 10.0, start_time, t1))
    t2.start()

def run_unlockPBA(button, ui, reboot):
    index = ui.dev_select.get_active()
    ui.devname = ui.devs_list[ui.locked_list[index]]
    ui.LKATTR = "RW"
    password = ''
    if ui.VERSION == 3 and ui.check_pass_rd.get_active():
        password = passReadUSB(ui, ui.dev_vendor.get_text(), ui.dev_sn.get_text())
        if password == None or password == 'x':
            ui.msg_err('No password found for the drive.')
            return
    else:
        password = lockhash.hash_pass(ui.pass_entry.get_text(), ui.salt_list[ui.locked_list[index]], ui.dev_msid.get_text())
    ui.start_spin()
    ui.wait_instr.show()
    def t1_run():
        if ui.VERSION == 3 and ui.auth_menu.get_active() == 1:
            status1 = os.system(ui.prefix + "sedutil-cli -n -t -u --setMBRDone on " + password + " " + ui.devname)
            status2 = os.system(ui.prefix + "sedutil-cli -n -t -u --setLockingRange 1 " + ui.LKATTR + " " + password + " " + devname)
            gobject.idle_add(cleanup, status1 | status2)
        else:
            status1 =  os.system(ui.prefix + "sedutil-cli -n -t --setMBRdone on " + password + " " + ui.devname )
            
            status2 =  os.system(ui.prefix + "sedutil-cli -n -t --setLockingRange " + ui.LKRNG + " " 
                    + ui.LKATTR + " " + password + " " + ui.devname)
            gobject.idle_add(cleanup, status1 | status2)
        
    def cleanup(status):
        ui.stop_spin()
        t1.join()
        if status != 0 :
            ui.msg_err("Error: Preboot unlocking of " + ui.devname + " failed.")
            ui.unlock_prompt(ui)
        else :
            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
            timeStr = timeStr[2:]
            if ui.VERSION == 3 and ui.auth_menu.get_active() == 1:
                statusAW = os.system(ui.prefix + "sedutil-cli -n -t -u --auditwrite 02" + timeStr + " " + password + " User1 " + ui.devname)
            elif ui.VERSION != 1:
                statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 02" + timeStr + " " + password + " Admin1 " + ui.devname)

            if reboot:
                ui.reboot()
            else:
                ui.msg_ok(ui.devname + " preboot unlocked successfully.")
                if ui.auth_menu.get_active() != 1 and ui.pba_list[ui.locked_list[index]] == 'N/A':
                    p = os.popen(ui.prefix + "sedutil-cli -n -t --pbaValid " + password + " " + ui.devname).read()
                    pba_regex = 'PBA image version\s*:\s*(.+)'
                    m1 = re.search(pba_regex, p)
                    if m1:
                        pba_ver = m1.group(1)
                        ui.pba_list[ui.locked_list[index]] = pba_ver
                
                ui.query(1)
                ui.lockstatus_list[ui.locked_list[index]] = "Unlocked"
                
                ui.updateDevs(ui.locked_list[index],[2,3])
                ui.returnToMain()
                
    t1 = threading.Thread(target=t1_run, args=())
    t1.start()
    start_time = time.time()
    t2 = threading.Thread(target=timeout_track, args=(ui, 10.0, start_time, t1))
    t2.start()

def run_unlockFull(button, ui):
    index = ui.dev_select.get_active()
    ui.devname = ui.devs_list[ui.setup_list[index]]
    password = ""
    if ui.VERSION == 3 and ui.check_pass_rd.get_active():
        password = passReadUSB(ui, ui.dev_vendor.get_text(), ui.dev_sn.get_text())
        if password == None or password == 'x':
            ui.msg_err('No password found for the drive.')
            return
    else:
        password = lockhash.hash_pass(ui.pass_entry.get_text(), ui.salt_list[ui.setup_list[index]], ui.dev_msid.get_text())
    ui.start_spin()
    ui.wait_instr.show()
    
    def t1_run():
        
        if password == None or password == 'x':
            gobject.idle_add(cleanup, 1)
        else:
            index = ui.dev_select.get_active()
            msid = ui.msid_list[ui.setup_list[index]]
            status1 =  os.system(ui.prefix + "sedutil-cli -n -t --disableLockingRange " + ui.LKRNG + " " + password + " " + ui.devname )
            status2 =  os.system(ui.prefix + "sedutil-cli -n -t --setMBREnable off " + password + " " + ui.devname )
            status3 = os.system(ui.prefix + "sedutil-cli -n -t --setSIDPassword " + password + " " + msid + " " + ui.devname)
            status4 = os.system(ui.prefix + "sedutil-cli -n -t --setAdmin1Pwd " + password + " " + msid + " " + ui.devname)
            gobject.idle_add(cleanup, status1 | status2 | status3 | status4)
            
    
    def cleanup(status):
        ui.stop_spin()
        t1.join()
        if status != 0 :
            ui.msg_err("TCG Unlock unsuccess")
            ui.op_instr.show()
            ui.box_pass.show()
            ui.check_box_pass.show()
            ui.unlockFull_button.show()
            ui.go_button_cancel.show()
        else :
            msid = ui.msid_list[ui.setup_list[index]]
            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
            timeStr = timeStr[2:]
            statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 02" + timeStr + " " + msid + " Admin1 " + ui.devname)
            ui.msg_ok("TCG Unlock success")
            
            if ui.pba_list[ui.setup_list[index]] == 'N/A':
                p = os.popen(ui.prefix + "sedutil-cli -n -t --pbaValid " + msid + " " + ui.devname).read()
                pba_regex = 'PBA image version\s*:\s*(.+)'
                m1 = re.search(pba_regex, p)
                if m1:
                    pba_ver = m1.group(1)
                    ui.pba_list[ui.setup_list[index]] = pba_ver
            
            ui.query(1)
            ui.lockstatus_list[ui.setup_list[index]] = "Unlocked"
            ui.setupstatus_list[ui.setup_list[index]] = "No"
            ui.updateDevs(ui.setup_list[index],[4])
            ui.returnToMain()
    t1 = threading.Thread(target=t1_run, args=())
    t1.start()
    start_time = time.time()
    t2 = threading.Thread(target=timeout_track, args=(ui, 10.0, start_time, t1))
    t2.start()

def run_unlockPartial(button, ui):
    index = ui.dev_select.get_active()
    ui.devname = ui.devs_list[ui.setup_list[index]]
    password = ""
    if ui.VERSION == 3 and ui.check_pass_rd.get_active():
        password = passReadUSB(ui, ui.dev_vendor.get_text(), ui.dev_sn.get_text())
        if password == None or password == 'x':
            ui.msg_err('No password found for the drive.')
            return
    else:
        password = lockhash.hash_pass(ui.pass_entry.get_text(), ui.salt_list[ui.setup_list[index]], ui.dev_msid.get_text())
    ui.start_spin()
    ui.wait_instr.show()
    
    def t1_run():
        if password == None or password == 'x':
            gobject.idle_add(cleanup, 1)
        else:
            status1 =  os.system(ui.prefix + "sedutil-cli -n -t --disableLockingRange " + ui.LKRNG + " " + password + " " + ui.devname )
            status2 =  os.system(ui.prefix + "sedutil-cli -n -t --setMBREnable off " + password + " " + ui.devname )
            gobject.idle_add(cleanup, status1 | status2)
    
    def cleanup(status):
        ui.stop_spin()
        t1.join()
        if status != 0 :
            ui.msg_err("Error: Partial unlock failed")
            ui.op_instr.show()
            ui.box_pass.show()
            ui.check_box_pass.show()
            ui.unlockPartial_button.show()
            ui.go_button_cancel.show()
        else :
            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
            timeStr = timeStr[2:]
            statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 02" + timeStr + " " + password + " Admin1 " + ui.devname)
            ui.msg_ok("Partial unlock completed")
            
            if ui.pba_list[ui.setup_list[index]] == 'N/A':
                p = os.popen(ui.prefix + "sedutil-cli -n -t --pbaValid " + password + " " + ui.devname).read()
                pba_regex = 'PBA image version\s*:\s*(.+)'
                m1 = re.search(pba_regex, p)
                if m1:
                    pba_ver = m1.group(1)
                    ui.pba_list[ui.setup_list[index]] = pba_ver
            
            ui.query(1)
            if ui.setup_list[index] in ui.locked_list:
                ui.lockstatus_list[ui.setup_list[index]] = "Unlocked"
                ui.updateDevs(ui.setup_list[index],[2,3])
            ui.returnToMain()
            
    t1 = threading.Thread(target=t1_run, args=())
    t1.start()
    start_time = time.time()
    t2 = threading.Thread(target=timeout_track, args=(ui, 10.0, start_time, t1))
    t2.start()

def run_unlockUSB(button, ui):
    folder_list = []
    dev_os = platform.system()
    if dev_os == 'Windows':
        for drive in ascii_uppercase:
            if drive != 'C' and os.path.isdir('%s:\\' % drive):
                if os.path.isdir('%s:\\FidelityLock' % drive):
                    folder_list.append(drive)
    elif dev_os == 'Linux':
        txt = os.popen(ui.prefix + 'mount -l').read()
        dev_regex = '/dev/sd[b-z][1-9]?\son\s(/[A-z/]*)\stype'
        drive_list = re.findall(dev_regex, txt)
        for d in drive_list:
            if os.path.isdir('%s/FidelityLock' % d):
                folder_list.append(d)
    if len(folder_list) == 0:
        msg_err('No password files found, check to make sure the USB is properly inserted.')
    else:
        ui.start_spin()
        ui.wait_instr.show()
        def t1_run():
            dev_unlocked = []
            for i in ui.locked_list:
                pw = passReadUSB(ui, ui.vendor_list[i], ui.sn_list[i])
                if pw == None:
                    ui.msg_err('No password found.')
                    break
                elif pw != 'x':
                    ui.LKATTR = "RW"
                    ui.LKRNG = "0"
                    status1 = ''
                    status2 = ''
                    if ui.auth_menu.get_active() == 0:
                        status1 =  os.system(ui.prefix + "sedutil-cli -n -t --setMBRdone on " + pw + " " + ui.devs_list[i] )
                        status2 =  os.system(ui.prefix + "sedutil-cli -n -t --setLockingRange " + ui.LKRNG + " " 
                                + ui.LKATTR + " " + pw + " " + ui.devs_list[i])
                    else:
                        status1 =  os.system(ui.prefix + "sedutil-cli -n -t -u --setMBRdone on " + pw + " " + ui.devs_list[i] )
                        status2 =  os.system(ui.prefix + "sedutil-cli -n -t -u --setLockingRange 1 " 
                                + ui.LKATTR + " " + pw + " " + ui.devs_list[i])
                    if (status1 | status2) == 0 :
                        timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                        timeStr = timeStr[2:]
                        if ui.VERSION != 1:
                            statusAW = os.system(ui.prefix + "sedutil-cli -n -t --auditwrite 02" + timeStr + " " + pw + " Admin1 " + ui.devs_list[i])
                        dev_unlocked.append(i)
                        
                        if ui.auth_menu.get_active() != 1 and ui.pba_list[i] == 'N/A':
                            p = os.popen(ui.prefix + "sedutil-cli -n -t --pbaValid " + pw + " " + ui.devname).read()
                            pba_regex = 'PBA image version\s*:\s*(.+)'
                            m1 = re.search(pba_regex, p)
                            if m1:
                                pba_ver = m1.group(1)
                                ui.pba_list[i] = pba_ver
                        
            gobject.idle_add(cleanup, dev_unlocked)
            
        def cleanup(dev_unlocked):
            if len(dev_unlocked) == 0:
                ui.msg_err('No drives were unlocked.')
            else:
                txt = 'The following drives were unlocked: '
                for j in range(len(dev_unlocked) - 1):
                    txt = txt + ui.devs_list[dev_unlocked[j]] + ','
                txt = txt + dev_unlocked[len(dev_unlocked) - 1]
                ui.msg_ok(txt)
                for i in dev_unlocked:
                    ui.lockstatus_list[i] = "Unlocked"
                    ui.updateDevs(i,[2,3])
                ui.returnToMain()
                
        t1 = threading.Thread(target=t1_run, args=())
        t1.start()
        start_time = time.time()
        t2 = threading.Thread(target=timeout_track, args=(ui, 30.0, start_time, t1))
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
        txt = os.popen(ui.prefix + 'mount -l').read()
        dev_regex = '/dev/sd[b-z][1-9]?\son\s(/[A-z/]*)\stype'
        drive_list = re.findall(dev_regex, txt)
        for d in drive_list:
            if os.path.isdir('%s/FidelityLock' % d):
                folder_list.append(d)
    if len(folder_list) == 0:
        msg_err('No password files found, check to make sure the USB is properly inserted.')
        return None
    else:
        latest_pw = 'x'
        for i in range(len(folder_list)):
            filename = model + '_' + sn + '.psw'
            if dev_os == 'Windows':
                filepath = folder_list[i] + ":\\FidelityLock\\" + filename
            elif dev_os == 'Linux':
                filepath = folder_list[i] + "/FidelityLock/" + filename
            if os.path.isfile(filepath):
                f = open(filepath, 'r')
                txt = f.read()
                pswReg = ''
                if ui.auth_menu.get_active() == 0:
                    pswReg = 'Timestamp: ([0-9]{14})\nAdmin: ([a-z0-9]{64})'
                else:
                    pswReg = 'Timestamp: ([0-9]{14})\nUser: ([a-z0-9]{64})'
                list = re.findall(pswReg, txt)
                for e in list:
                    ts = e[0]
                    if ts > latest_ts:
                        latest_ts = ts
                        latest_pw = e[1]
                f.close()   
        return latest_pw

def passSaveUSB(ui, *args):
    dev_os = platform.system()
    timestamp = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
    drive = ui.drive_menu.get_active_text()
    salt = ''
    index = ui.dev_select.get_active()
    if ui.view_state == 4:
        salt = ui.salt_list[ui.nonsetup_list[index]]
    elif ui.view_state == 2:
        salt = ui.salt_list[ui.setup_list[index]]
    f = None
    path = ''
    if dev_os == 'Windows':
        if not os.path.isdir('%s\\FidelityLock' % drive):
            os.makedirs('%s\\FidelityLock' % drive)
        path = '' + drive + '\\FidelityLock\\' + ui.dev_vendor.get_text() + '_' + ui.dev_sn.get_text() + '.psw'
    elif dev_os == 'Linux':
        if not os.path.isdir('%s/FidelityLock' % drive):
            os.makedirs('%s/FidelityLock' % drive)
        path = '' + drive + '/FidelityLock/' + ui.dev_vendor.get_text() + '_' + ui.dev_sn.get_text() + '.psw'
    if path != '':
        if os.path.isfile(path):
            f = open(path, 'a')
            #txt = f.read()
            #otherReg = ''
            #if ui.auth_menu.get_active() == 1:
            #    otherReg = 'Timestamp: ([0-9]{14})\nAdmin: ([a-z0-9]{64})'
            #else:
            #    otherReg = 'Timestamp: ([0-9]{14})\nUser: ([a-z0-9]{64})'
            #m = re.search(otherReg, txt)
            #if m:
            #
            #else:
            #    
            if ui.auth_menu.get_active() == 0:
                f.write('\nTimestamp: ' + timestamp + '\nAdmin: ' + lockhash.hash_pass(ui.new_pass_entry.get_text(), salt, ui.dev_msid.get_text()))
            else:
                f.write('\nTimestamp: ' + timestamp + '\nUser: ' + lockhash.hash_pass(ui.new_pass_entry.get_text(), salt, ui.dev_msid.get_text()))
            f.close()
        else:
            f = open(path, 'w')
            f.write('Model Number: ' + ui.dev_vendor.get_text() + '\nSerial Number: ' + ui.dev_sn.get_text() + '\n')
            if ui.auth_menu.get_active() == 0:
                f.write('\nTimestamp: ' + timestamp + '\nAdmin: ' + lockhash.hash_pass(ui.new_pass_entry.get_text(), salt, ui.dev_msid.get_text()))
            else:
                f.write('\nTimestamp: ' + timestamp + '\nUser: ' + lockhash.hash_pass(ui.new_pass_entry.get_text(), salt, ui.dev_msid.get_text()))
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
        if ui.VERSION == 3 and ui.check_pass_rd.get_active():
            password_a = passReadUSB(ui, ui.dev_vendor.get_text(), ui.dev_sn.get_text())
            if password_a == None or password_a == 'x':
                ui.msg_err('No password found for the drive.')
                return
        else:
            password_a = lockhash.hash_pass(ui.pass_entry.get_text(), ui.salt_list[ui.setup_list[index]], ui.dev_msid.get_text())
        password_u = lockhash.hash_pass(ui.new_pass_entry.get_text(), ui.salt_list[ui.setup_list[index]], ui.dev_msid.get_text())
        
        ui.start_spin()
        ui.wait_instr.show()
        
        def t1_run():
            s0 = os.system(ui.prefix + "sedutil-cli -n -t --enableLockingRange 1 " + password_a + " " + ui.devname)
            
            s2 = os.system(ui.prefix + "sedutil-cli -n -t --enableuser ON " + password_a + " User1 " + ui.devname)
            s3 = os.system(ui.prefix + "sedutil-cli -n -t --enableuserread ON " + password_a + " User1 " + ui.devname)
            s1 = os.system(ui.prefix + "sedutil-cli -n -t --setpassword " + password_a + " User1 " + password_u + " " + ui.devname)
            
            gobject.idle_add(cleanup, s0|s1|s2|s3)
        
        def cleanup(status):
            ui.stop_spin()
            t1.join()
            if status !=0 :
                ui.msg_err("Error: User setup for " + ui.devname + " failed. Try again.")
                ui.op_instr.show()
                ui.box_pass.show()
                ui.box_newpass.show()
                ui.box_newpass_confirm.show()
                ui.check_box_pass.show()
                ui.go_button_cancel.show()
            else : 
                if ui.VERSION == 3 and ui.pass_sav.get_active():
                    passSaveUSB(ui)
                ui.query(1)
                ui.msg_ok("User Password for " + ui.devname + " set up successfully.")
                ui.returnToMain()
                    
        t1 = threading.Thread(target=t1_run, args=())
        t1.start()
        start_time = time.time()
        t2 = threading.Thread(target=timeout_track, args=(ui, 10.0, start_time, t1))
        t2.start()
        
def run_setupUSB(button, ui, *args):
    dev1 = ui.dev_select.get_active_text()
    index2 = ui.usb_menu.get_active()
    
    ui.start_spin()
    ui.wait_instr.show()
    
    def t1_run():
        dev_os = platform.system()
        if dev_os == 'Windows':
            status1 =  os.system(ui.prefix + "sedutil-cli --createUSB UEFI " + dev1 + " \\\\.\\PhysicalDrive" + ui.usb_list[index2][0])
            gobject.idle_add(cleanup, status1)
        elif dev_os == 'Linux':
            status1 =  os.system(ui.prefix + "sedutil-cli --createUSB UEFI " + dev1 + " " + ui.usb_list[index2][0])
            gobject.idle_add(cleanup, status1)
    def cleanup(status):
        ui.stop_spin()
        t1.join()
        if status != 0 :
            ui.msg_err("Error: Setup USB failed")
            ui.op_instr.show()
            ui.setupUSB_button.show()
            ui.go_button_cancel.show()
        else :
            ui.msg_ok("Partial unlock completed")
            
            ui.returnToMain()
            
    t1 = threading.Thread(target=t1_run, args=())
    t1.start()
    start_time = time.time()
    t2 = threading.Thread(target=timeout_track, args=(ui, 120.0, start_time, t1))
    t2.start()
        

def openLog(button, ui, *args):
    columns = ["Level", "Date and Time", "Event ID", "Event Description"]
    ui.auditEntries = []
    ui.errorEntries = []
    ui.warnerrEntries = []

    logWin = gtk.Window()
    logWin.set_border_width(10)
    logWin.set_default_size(500, 500)
    logWin.set_title("Audit Log")
    if os.path.isfile('icon.jpg'):
        logWin.set_icon_from_file('icon.jpg')
    vbox = gtk.VBox()
    logWin.add(vbox)
    
    ui.listStore = gtk.ListStore(str, str, int, str)
    
    index = ui.dev_select.get_active()
    ui.devname = ui.devs_list[index]
    password = ""
    if ui.VERSION == 3 and ui.check_pass_rd.get_active():
        password = passReadUSB(ui, ui.dev_vendor.get_text(), ui.dev_sn.get_text())
        if password == None or password == 'x':
            ui.msg_err('No password found for the drive.')
            return
    else:
        password = lockhash.hash_pass(ui.pass_entry.get_text(), ui.salt_list[ui.tcg_list[index]], ui.dev_msid.get_text())
    txt = ""
    if ui.VERSION == 3:
        auth_level = ui.auth_menu.get_active()
        if auth_level == 0:
            txt = os.popen(ui.prefix + "sedutil-cli -n -t -u --auditread " + password + " Admin1 " + ui.devname ).read()
        else:
            txt = os.popen(ui.prefix + "sedutil-cli -n -t -u --auditread " + password + " User1 " + ui.devname ).read()
    else:
        txt = os.popen(ui.prefix + "sedutil-cli -n -t --auditread " + password + " Admin1 " + ui.devname ).read()
    print txt
    auditFullRegex = 'Total Number of Audit Entries\s*:\s*([0-9]+)\n((?:.+\n?)+)'
    a = re.search(auditFullRegex, txt)
    if txt == "Invalid Audit Signature or No Audit Entry log\n" or not a:
        ui.msg_err("Invalid Audit Signature or No Audit Entry Log or Read Error")
    else:
        numEntries = int(a.group(1))
        logList = a.group(2).split('\n')
        auditRegex = "([0-9]+/[0-9]+/[0-9]+\s+[0-9]+:[0-9]+:[0-9]+)\s+([0-9]+)"
        pattern = re.compile(auditRegex)
        
        for i in range(numEntries):
            m = pattern.match(logList[i])
            dateTime = m.group(1)
            eventID = int(m.group(2))
            eventDes = ui.eventDescriptions[eventID]
            eventLevel = "Information"
            if eventID == 9 or eventID == 14 or eventID == 16 or eventID == 18:
                eventLevel = "Error"
                ui.errorEntries.append((eventLevel, dateTime, eventID, eventDes))
                ui.warnerrEntries.append((eventLevel, dateTime, eventID, eventDes))
            elif eventID == 13 or eventID == 15 or eventID == 17:
                eventLevel = "Warning"
                ui.warnerrEntries.append((eventLevel, dateTime, eventID, eventDes))
            ui.auditEntries.append((eventLevel, dateTime, eventID, eventDes))
        for i in range(len(ui.auditEntries)):
            ui.listStore.append(ui.auditEntries[i])
    
    treeView = gtk.TreeView(model=ui.listStore)
    
    for i in range(len(columns)):
        cell = gtk.CellRendererText()
        col = gtk.TreeViewColumn(columns[i], cell, text=i)
        if i < 3:
            col.set_sort_column_id(gtk.SORT_DESCENDING)
            col.set_sort_indicator(True)
        treeView.append_column(col)
    
    vbox.pack_start(treeView)
    
    halign = gtk.Alignment(1,0,0,0)
    filter_box = gtk.HBox(False, 0)
    
    ui.viewAll_button = gtk.Button('View all entries')
    ui.viewAll_button.connect("clicked", filterLog, ui, ui.auditEntries, 0)
    ui.viewAll_button.set_sensitive(False)
    filter_box.pack_start(ui.viewAll_button, False, False, 5)
    
    ui.viewWarnErr_button = gtk.Button('View Warnings & Errors')
    ui.viewWarnErr_button.connect("clicked", filterLog, ui, ui.warnerrEntries, 1)
    filter_box.pack_start(ui.viewWarnErr_button, False, False, 5)
    
    ui.viewErr_button = gtk.Button('View Errors')
    ui.viewErr_button.connect("clicked", filterLog, ui, ui.errorEntries, 2)
    filter_box.pack_start(ui.viewErr_button, False, False, 5)
    
    #ui.eraseLog_button = gtk.Button('Erase the device\'s entire log')
    #ui.eraseLog_button.connect("clicked", eraseLog, ui)
    #filter_box.pack_start(ui.eraseLog_button, False, False, 5)
    
    halign.add(filter_box)
    vbox.pack_start(halign, False, False, 0)
    
    logWin.show_all()
    
def filterLog(button, ui, entries, mode):
    ui.listStore.clear()
    for i in range(len(entries)):
        ui.listStore.append(entries[i])
    ui.viewAll_button.set_sensitive(mode != 0)
    ui.viewWarnErr_button.set_sensitive(mode != 1)
    ui.viewErr_button.set_sensitive(mode != 2)
    
#def eraseLog(button, ui, *args):
#    password = lockhash.hash_pass(ui.pass_entry.get_text(), ui.dev_sn.get_text(), ui.dev_msid.get_text())
#    status = os.system( ui.prefix + "sedutil-cli -n -t --auditerase " + password + " " + ui.devname )
#    if status != 0:
#        ui.msg_err('Error while attempting to erase the audit log')
#    else:
#        ui.listStore.clear()
#        ui.auditEntries = []
#        ui.errorEntries = []
#        ui.warnerrEntries = []
#        ui.msg_ok('Audit Log erased successfully')

def timeout_track(ui, max_time, start_time, op_thread):
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
    
    gobject.idle_add(timeout_cleanup, ui, max_time, start_time, op_thread)

def timeout_cleanup(ui, max_time, start_time, op_thread):
    if op_thread.isAlive():
        curr_time = time.time()
        elapsed_time = curr_time - start_time
        if elapsed_time >= max_time:
            op_thread.join(0.0)
            ui.msg_err("Operation timed out.")
    
    ui.stop_spin()
