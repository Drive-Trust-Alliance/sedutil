import cleanop
import datetime
import dialogs
import gobject
import lockhash
import os
import platform
import re
import runop
import runprocess
if platform.system() == 'Windows':
    import subprocess
import threading
import time

from multiprocessing import Array

def rt_unlockPBA(ui, selected_list, reboot, autounlock):
    rescan_needed = False
    scan_check = False
    
    txt_s = ''
    rgx = '(PhysicalDrive[0-9]+|/dev/sd[a-z][0-9]?|/dev/nvme[0-9])\s+(?:[12ELP]+|No)\s+([^\:\s]+(?:\s[^\:\s]+)*)\s*:\s*[^:]+\s*:\s*(\S+)'
    
    list_d = []
    actual_d = []
    
    for index in selected_list:
        if ui.tcg_usb_list[index]:
            scan_check = True
    if scan_check:
        for x in range(len(ui.devs_list)):
            runop.prelock(x)
        txt_s = os.popen(ui.prefix + 'sedutil-cli --scan n').read()
        for x in range(len(ui.devs_list)):
            runop.postlock(x)
        list_d = re.findall(rgx, txt_s)
            
    for index in selected_list:
        if ui.tcg_usb_list[index]:
            new_devname = None
            matched = False
            present = False
            
            for entry in list_d:
                if not present or not matched:
                    test_dev = entry[0]
                    if ui.DEV_OS == 'Windows':
                        test_dev = '\\\\.\\' + test_dev
                    if test_dev == ui.devs_list[index] and entry[1] == ui.vendor_list[index] and entry[2] == ui.sn_list[index]:
                        present = True
                        matched = True
                    elif entry[1] == ui.vendor_list[index] and entry[2] == ui.sn_list[index]:
                        present = True
                        new_devname = test_dev
            if present and not matched:
                actual_d.append(new_devname)
            elif matched:
                actual_d.append(ui.devs_list[index])
            else:
                actual_d.append(None)
            if not present or not matched:
                rescan_needed = True
        else:
            actual_d.append(ui.devs_list[index])

    def t1_run(e, dev, i, thread_list, result_list, status_list, count, password, hash_au, autounlock, reboot):
        noPW = False
        status_final = -1
        pass_usb = ''
        admin_found = False
        user_found = False
        password_u = None
        auto_auth = -1
        
        save_status = -1
        
        if not autounlock:
            if password == None:
                (password, pass_usb) = runprocess.passReadUSB(ui.auth_menu.get_active_text(), ui.devs_list[i], ui.vendor_list[i], ui.sn_list[i])
            if password == None or password == 'x':
                noPW = True
        else:
            (password, pass_usb) = runprocess.passReadUSB("Admin", ui.devs_list[i], ui.vendor_list[i], ui.sn_list[i])
            if password == None or password == 'x':
                (password, pass_usb) = runprocess.passReadUSB("User", ui.devs_list[i], ui.vendor_list[i], ui.sn_list[i])
                if password == None or password == 'x':
                    noPW = True
                else:
                    user_found = True
                    auto_auth = 1
            else:
                admin_found = True
                (password_u, pass_usb_u) = runprocess.passReadUSB("User", ui.devs_list[i], ui.vendor_list[i], ui.sn_list[i])
                if password_u != None and password_u != 'x':
                    user_found = True
                else:
                    auto_auth = 0
        runop.prelock(i)
        if not e.is_set() and not noPW:
            if not autounlock:
                if ui.DEV_OS == 'Windows':
                    with open(os.devnull, 'w') as pipe:
                        if (ui.VERSION % 3 == 0 or (ui.VERSION == 1 and ui.PBA_VERSION != 1)) and ui.auth_menu.get_active() == 1:
                            status1 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--setLockingRange', '0', ui.LKATTR, password, dev], stdout=pipe)#stderr=log)
                            if status1 != 0:
                                status_final = status1
                            elif e.is_set():
                                status2 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--setLockingRange', '0', 'LK', password, dev], stdout=pipe)#stderr=log)
                            elif i in ui.mbr_list:
                                status2 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--setMBRDone', 'on', password, dev], stdout=pipe)#stderr=log)
                                status_final = status2
                            else:
                                status_final = 0
                                if e.is_set():
                                    if i in ui.mbr_list:
                                        status2 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--setMBRDone', 'off', password, dev], stdout=pipe)#stderr=log)
                                    status2 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--setLockingRange', '0', 'LK', password, dev], stdout=pipe)#stderr=log)
                        else:
                            status1 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--setLockingRange', ui.LKRNG, ui.LKATTR, password, dev], stdout=pipe)#stderr=log)
                            status_final = status1
                            if e.is_set():
                                status2 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--setLockingRange', ui.LKRNG, 'LK', password, dev], stdout=pipe)#stderr=log)
                            elif status1 == 0 and i in ui.mbr_list:
                                status2 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--setMBRDone', 'on', password, dev], stdout=pipe)#stderr=log)
                                if e.is_set():
                                    if i in ui.mbr_list:
                                        status2 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--setMBRDone', 'off', password, dev], stdout=pipe)#stderr=log)
                                    status2 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--setLockingRange', ui.LKRNG, 'LK', password, dev], stdout=pipe)#stderr=log)
                else:
                    if (ui.VERSION % 3 == 0 or (ui.VERSION == 1 and ui.PBA_VERSION != 1)) and ui.auth_menu.get_active() == 1:
                        status1 = os.system(ui.prefix + 'sedutil-cli -n -t -u --setLockingRange 0 ' + ui.LKATTR + ' "' + password + '" ' + dev)
                        if status1 != 0:
                            status_final = status1
                        elif e.is_set():
                            status2 = os.system(ui.prefix + 'sedutil-cli -n -t -u --setLockingRange 0 LK "' + password + '" ' + dev)
                        elif i in ui.mbr_list:
                            status2 = os.system(ui.prefix + 'sedutil-cli -n -t -u --setMBRDone on "' + password + '" ' + dev)
                            status_final = status2
                        else:
                            status_final = 0
                            if e.is_set():
                                if i in ui.mbr_list:
                                    status1 = os.system(ui.prefix + "sedutil-cli -n -t -u --setMBRDone off " + password + " " + dev)
                                status2 = os.system(ui.prefix + "sedutil-cli -n -t -u --setLockingRange 0 LK " + password + " " + dev)
                    else:
                        status1 =  os.system(ui.prefix + 'sedutil-cli -n -t --setLockingRange ' + ui.LKRNG + ' ' + ui.LKATTR + ' "' + password + '" ' + dev)
                        status_final = status1
                        if e.is_set():
                            status2 = os.system(ui.prefix + 'sedutil-cli -n -t --setLockingRange ' + ui.LKRNG + ' LK "' + password + '" ' + dev)
                        elif status1 == 0 and i in ui.mbr_list:
                            status2 =  os.system(ui.prefix + 'sedutil-cli -n -t --setMBRdone on "' + password + '" ' + dev )
                            if e.is_set():
                                if i in ui.mbr_list:
                                    status1 = os.system(ui.prefix + "sedutil-cli -n -t --setMBRDone off " + password + " " + dev)
                                status2 = os.system(ui.prefix + "sedutil-cli -n -t --setLockingRange " + ui.LKRNG + " LK " + password + " " + dev)
            else:
                if (ui.VERSION % 3 == 0 or (ui.VERSION == 1 and ui.PBA_VERSION != 1)) and user_found and not admin_found:
                    status1 = os.system(ui.prefix + 'sedutil-cli -n -t -u --setLockingRange 0 ' + ui.LKATTR + ' "' + password + '" ' + dev)
                    if status1 != 0:
                        status_final = status1
                    elif e.is_set():
                        status2 = os.system(ui.prefix + 'sedutil-cli -n -t -u --setLockingRange 0 LK "' + password + '" ' + dev)
                    elif i in ui.mbr_list:
                        status2 = os.system(ui.prefix + 'sedutil-cli -n -t -u --setMBRDone on "' + password + '" ' + dev)
                        status_final = status2
                        if e.is_set():
                            if i in ui.mbr_list:
                                status1 = os.system(ui.prefix + "sedutil-cli -n -t -u --setMBRDone off " + password + " " + dev)
                            status2 = os.system(ui.prefix + "sedutil-cli -n -t -u --setLockingRange 0 LK " + password + " " + dev)
                    else:
                        status_final = 0
                elif admin_found:
                    status1 =  os.system(ui.prefix + 'sedutil-cli -n -t --setLockingRange ' + ui.LKRNG + ' ' + ui.LKATTR + ' "' + password + '" ' + dev)
                    status_final = status1
                    if e.is_set():
                        status2 = os.system(ui.prefix + 'sedutil-cli -n -t --setLockingRange ' + ui.LKRNG + ' LK "' + password + '" ' + dev)
                    elif status1 == 0:
                        auto_auth = 0
                        if i in ui.mbr_list:
                            status2 =  os.system(ui.prefix + 'sedutil-cli -n -t --setMBRdone on "' + password + '" ' + dev )
                            if e.is_set():
                                if i in ui.mbr_list:
                                    status1 = os.system(ui.prefix + 'sedutil-cli -n -t --setMBRDone off "' + password + '" ' + dev)
                                status2 = os.system(ui.prefix + 'sedutil-cli -n -t --setLockingRange ' + ui.LKRNG + ' LK "' + password + '" ' + dev)
                    elif user_found:
                        status1 = os.system(ui.prefix + 'sedutil-cli -n -t -u --setLockingRange 0 ' + ui.LKATTR + ' "' + password_u + '" ' + dev)
                        status_final = status1
                        if e.is_set():
                            status2 = os.system(ui.prefix + 'sedutil-cli -n -t -u --setLockingRange 0 LK "' + password_u + '" ' + dev)
                        elif status1 == 0:
                            auto_auth = 1
                            password = password_u
                            if i in ui.mbr_list:
                                status2 = os.system(ui.prefix + 'sedutil-cli -n -t -u --setMBRDone on "' + password_u + '" ' + dev)
                                if e.is_set():
                                    if i in ui.mbr_list:
                                        status1 = os.system(ui.prefix + "sedutil-cli -n -t -u --setMBRDone off " + password_u + " " + dev)
                                    status2 = os.system(ui.prefix + "sedutil-cli -n -t -u --setLockingRange 0 LK " + password_u + " " + dev)
        if not e.is_set():
            if status_final == 0:
                runprocess.passSaveAppData(password, ui.vendor_list[i], ui.sn_list[i], ui.auth_menu.get_active_text())
                if ui.pass_sav.get_active():
                    save_status = runprocess.passSaveUSB(password, ui.drive_menu.get_active_text(), ui.vendor_list[i], ui.sn_list[i], pass_usb, ui.auth_menu.get_active_text())
            def aw_run(i, save_status, auth, dev, password):
                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                timeStr = timeStr[2:]
                if ui.DEV_OS == 'Windows':
                    with open(os.devnull, 'w') as pipe:
                        if status_final != 0 or noPW:
                            if (not autounlock and auth == 0) or (autounlock and auto_auth == 0):
                                if status_final == ui.NOT_AUTHORIZED and ui.admin_aol_list[i] < ui.retrylimit_list[i]:
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '04' + timeStr, hash_au, 'User' + ui.user_list[i], dev], stdout=pipe)#stderr=log)
                                elif status_final == ui.AUTHORITY_LOCKED_OUT or ui.admin_aol_list[i] >= ui.retrylimit_list[i]:
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '05' + timeStr, hash_au, 'User' + ui.user_list[i], dev], stdout=pipe)#stderr=log)
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '09' + timeStr, hash_au, 'User' + ui.user_list[i], dev], stdout=pipe)#stderr=log)
                                if ui.usb_boot:
                                    subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '35' + timeStr, hash_au, 'User' + ui.user_list[i], dev], stdout=pipe)#stderr=log)
                                elif ui.VERSION == 1:
                                    subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '34' + timeStr, hash_au, 'User' + ui.user_list[i], dev], stdout=pipe)#stderr=log)
                                subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '33' + timeStr, hash_au, 'User' + ui.user_list[i], dev], stdout=pipe)#stderr=log)
                            else:
                                if status_final == ui.NOT_AUTHORIZED and ui.user_aol_list[i] < ui.retrylimit_list[i]:
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '07' + timeStr, hash_au, 'User' + ui.user_list[i], dev], stdout=pipe)#stderr=log)
                                elif status_final == ui.AUTHORITY_LOCKED_OUT or ui.user_aol_list[i] >= ui.retrylimit_list[i]:
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '08' + timeStr, hash_au, 'User' + ui.user_list[i], dev], stdout=pipe)#stderr=log)
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '09' + timeStr, hash_au, 'User' + ui.user_list[i], dev], stdout=pipe)#stderr=log)
                                if ui.usb_boot:
                                    subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '35' + timeStr, hash_au, 'User' + ui.user_list[i], dev], stdout=pipe)#stderr=log)
                                elif ui.VERSION == 1:
                                    subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '34' + timeStr, hash_au, 'User' + ui.user_list[i], dev], stdout=pipe)#stderr=log)
                                subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '33' + timeStr, hash_au, 'User' + ui.user_list[i], dev], stdout=pipe)#stderr=log)
                        else:
                            if (not autounlock and auth == 1) or (autounlock and auto_auth == 1):
                                if ui.check_pass_rd.get_active():
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '26' + timeStr, password, 'User1', dev], stdout=pipe)#stderr=log)
                                statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '06' + timeStr, password, 'User1', dev], stdout=pipe)#stderr=log)
                                statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '15' + timeStr, password, 'User1', dev], stdout=pipe)#stderr=log)
                                if ui.usb_boot:
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '17' + timeStr, password, 'User1', dev], stdout=pipe)#stderr=log)
                                elif ui.VERSION == 1:
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '16' + timeStr, password, 'User1', dev], stdout=pipe)#stderr=log)
                            else:
                                if ui.check_pass_rd.get_active():
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '25' + timeStr, password, 'Admin1', dev], stdout=pipe)#stderr=log)
                                statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '03' + timeStr, password, 'Admin1', dev], stdout=pipe)#stderr=log)
                                statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '15' + timeStr, password, 'Admin1', dev], stdout=pipe)#stderr=log)
                                if ui.usb_boot:
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '17' + timeStr, password, 'Admin1', dev], stdout=pipe)#stderr=log)
                                elif ui.VERSION == 1:
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '16' + timeStr, password, 'Admin1', dev], stdout=pipe)#stderr=log)
                            if save_status == 0:
                                if not autounlock and auth == 1:
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '24' + timeStr, password, 'User1', dev], stdout=pipe)#stderr=log)
                                elif auth == 0:
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '23' + timeStr, password, 'Admin1', dev], stdout=pipe)#stderr=log)
                else:
                    if status_final != 0 or noPW:
                        if (not autounlock and auth == 0) or (autounlock and auto_auth == 0):
                            if status_final == ui.NOT_AUTHORIZED and ui.admin_aol_list[i] < ui.retrylimit_list[i]:
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 04' + timeStr + ' "' + hash_au + '" User' + ui.user_list[i] + ' ' + dev)
                            elif status_final == ui.AUTHORITY_LOCKED_OUT or ui.admin_aol_list[i] >= ui.retrylimit_list[i]:
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 05' + timeStr + ' "' + hash_au + '" User' + ui.user_list[i] + ' ' + dev)
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 09' + timeStr + ' "' + hash_au + '" User' + ui.user_list[i] + ' ' + dev)
                            if ui.usb_boot:
                                os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 35' + timeStr + ' "' + hash_au + '" User' + ui.user_list[i] + ' ' + dev)
                            elif ui.VERSION == 1:
                                os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 34' + timeStr + ' "' + hash_au + '" User' + ui.user_list[i] + ' ' + dev)
                            os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 33' + timeStr + ' "' + hash_au + '" User' + ui.user_list[i] + ' ' + dev)
                        else:
                            if status_final == ui.NOT_AUTHORIZED and ui.user_aol_list[i] < ui.retrylimit_list[i]:
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 07' + timeStr + ' "' + hash_au + '" User' + ui.user_list[i] + ' ' + dev)
                            elif status_final == ui.AUTHORITY_LOCKED_OUT or ui.user_aol_list[i] >= ui.retrylimit_list[i]:
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 08' + timeStr + ' "' + hash_au + '" User' + ui.user_list[i] + ' ' + dev)
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 09' + timeStr + ' "' + hash_au + '" User' + ui.user_list[i] + ' ' + dev)
                            if ui.usb_boot:
                                os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 35' + timeStr + ' "' + hash_au + '" User' + ui.user_list[i] + ' ' + dev)
                            elif ui.VERSION == 1:
                                os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 34' + timeStr + ' "' + hash_au + '" User' + ui.user_list[i] + ' ' + dev)
                            os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 33' + timeStr + ' "' + hash_au + '" User' + ui.user_list[i] + ' ' + dev)
                    else:
                        if (not autounlock and auth == 1) or (autounlock and auto_auth == 1):
                            if ui.check_pass_rd.get_active():
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 26' + timeStr + ' "' + password + '" User1 ' + dev)
                            statusAW = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 06' + timeStr + ' "' + password + '" User1 ' + dev)
                            statusAW = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 15' + timeStr + ' "' + password + '" User1 ' + dev)
                            if ui.usb_boot:
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 17' + timeStr + ' "' + password + '" User1 ' + dev)
                            elif ui.VERSION == 1:
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 16' + timeStr + ' "' + password + '" User1 ' + dev)
                        else:
                            if ui.check_pass_rd.get_active():
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 25' + timeStr + ' "' + password + '" Admin1 ' + dev)
                            statusAW = os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 03' + timeStr + ' "' + password + '" Admin1 ' + dev)
                            statusAW = os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 15' + timeStr + ' "' + password + '" Admin1 ' + dev)
                            if ui.usb_boot:
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 17' + timeStr + ' "' + password + '" Admin1 ' + dev)
                            elif ui.VERSION == 1:
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 16' + timeStr + ' "' + password + '" Admin1 ' + dev)
                        if save_status == 0:
                            if not autounlock and auth == 1:
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 24' + timeStr + ' "' + password + '" Admin1 ' + dev)
                            elif auth == 0:
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 23' + timeStr + ' "' + password + '" Admin1 ' + dev)
                runop.postlock(i)
            if ui.datastore_list[i] == 'Supported':
                t_aw = threading.Thread(target=aw_run, args=(i,save_status, ui.auth_menu.get_active(), dev, password))
                t_aw.start()
                if reboot:
                    t_aw.join()
                    time.sleep(2)
                else:
                    time.sleep(0.5)
            else:
                runop.postlock(i)
        else:
            runop.postlock(i)
        #if autounlock:
        #    while ui.scan_ip:
        #        time.sleep(1)
        gobject.idle_add(cleanup1, dev, i, status_final, thread_list, result_list, status_list, count, noPW, e, password, pass_usb, autounlock, auto_auth, hash_au, save_status)
    
    def cleanup1(dev, i, status, thread_list, result_list, status_list, count, no_pw, e, password, pass_usb, autounlock, auto_auth, hash_au, save_status):
        #dev = ui.devs_list[i]
        if e.is_set():
            result_list[count] = 2
        elif status != 0 or no_pw:
            result_list[count] = 1
            if (not autounlock and ui.auth_menu.get_active() == 0) or (autounlock and auto_auth == 0):
                if status == ui.NOT_AUTHORIZED and ui.admin_aol_list[i] < ui.retrylimit_list[i]:
                    ui.admin_aol_list[i] = ui.admin_aol_list[i] + 1
                elif status == ui.AUTHORITY_LOCKED_OUT or ui.admin_aol_list[i] >= ui.retrylimit_list[i]:
                    ui.admin_aol_list[i] = ui.retrylimit_list[i] + 1
                elif status == ui.SP_BUSY:
                    result_list[count] = 4
                else:
                    result_list[count] = 3
            else:
                
                if status == ui.NOT_AUTHORIZED and ui.user_aol_list[i] < ui.retrylimit_list[i]:
                    ui.user_aol_list[i] = ui.user_aol_list[i] + 1
                elif status == ui.AUTHORITY_LOCKED_OUT or ui.user_aol_list[i] >= ui.retrylimit_list[i]:
                    ui.user_aol_list[i] = ui.retrylimit_list[i] + 1
                elif status == ui.SP_BUSY:
                    result_list[count] = 4
                else:
                    result_list[count] = 3
        else :
            if (not autounlock and ui.auth_menu.get_active() == 0) or (autounlock and auto_auth == 0):
                ui.admin_aol_list[i] = 0
            else:
                ui.user_aol_list[i] = 0
            if ui.pass_sav.get_active():
                
                if save_status <= 0:
                    result_list[count] = 0
                    
                else:
                    result_list[count] = 5
            else:
                result_list[count] = 0
            
            
        status_list[count] = status
        
                
    e_to = threading.Event()
    start_time = time.time()
    
    
    #thread_list = [None] * len(selected_list)
    proc_list = [None] * len(selected_list)
    #result_list = [None] * len(selected_list)
    result_list = Array('i', len(selected_list))
    #status_list = [None] * len(selected_list)
    status_list = Array('i', len(selected_list))
    count = 0
    for idx in range(len(selected_list)):
        result_list[idx] = -1
        status_list[idx] = -1
        if actual_d[idx] == None:
            result_list[idx] = 6
            status_list[idx] = -2
        
    for i in selected_list:
        if actual_d[count] != None:
            if not autounlock:
                if ui.check_pass_rd.get_active():
                    password = None
                    #password = runprocess.passReadUSB(ui, ui.vendor_list[i], ui.sn_list[i], ui.auth_menu.get_active())
                else:
                    pw_trim = re.sub('\s','',ui.pass_entry.get_text())
                    
                    password = lockhash.hash_pass(pw_trim, i, ui)#ui.salt_list[i], ui.msid_list[i])
            else:
                password = None
                #password = runprocess.passReadUSB(ui, ui.vendor_list[i], ui.sn_list[i], ui.auth_menu.get_active())
                
            #au = '"' + lockhash.get_val() + ui.salt_list[i] + '"'
            au = lockhash.get_val() + ui.salt_list[i]
            hash_au = lockhash.hash_pass(au, i, ui)#ui.salt_list[i], ui.msid_list[i])
            proc_list[count] = threading.Thread(target=t1_run, args=(e_to, actual_d[count], i, proc_list, result_list, status_list, count, password, hash_au, autounlock, reboot))
            proc_list[count].start()
        count = count + 1
    
    t2 = threading.Thread(target=timeout_track, args=(ui, 60.0*len(selected_list), start_time, proc_list, result_list, e_to, selected_list, status_list, cleanop.unlockPBA_cleanup, reboot, rescan_needed, -1, -1))
    t2.start()

def rt_revertKeep(ui, selected_list):
    rescan_needed = False
    scan_check = False
    
    txt_s = ''
    rgx = '(PhysicalDrive[0-9]+|/dev/sd[a-z][0-9]?|/dev/nvme[0-9])\s+(?:[12ELP]+|No)\s+([^\:\s]+(?:\s[^\:\s]+)*)\s*:\s*[^:]+\s*:\s*(\S+)'
    
    list_d = []
    actual_d = []
    
    for index in selected_list:
        if ui.tcg_usb_list[index]:
            scan_check = True
    if scan_check:
        for x in range(len(ui.devs_list)):
            runop.prelock(x)
        txt_s = os.popen(ui.prefix + 'sedutil-cli --scan n').read()
        for x in range(len(ui.devs_list)):
            runop.postlock(x)
        list_d = re.findall(rgx, txt_s)
            
    for index in selected_list:
        if ui.tcg_usb_list[index]:
            new_devname = None
            matched = False
            present = False
            
            for entry in list_d:
                if not present or not matched:
                    test_dev = entry[0]
                    if ui.DEV_OS == 'Windows':
                        test_dev = '\\\\.\\' + test_dev
                    if test_dev == ui.devs_list[index] and entry[1] == ui.vendor_list[index] and entry[2] == ui.sn_list[index]:
                        present = True
                        matched = True
                    elif entry[1] == ui.vendor_list[index] and entry[2] == ui.sn_list[index]:
                        present = True
                        new_devname = test_dev
            if present and not matched:
                actual_d.append(new_devname)
            elif matched:
                actual_d.append(ui.devs_list[index])
            else:
                actual_d.append(None)
            if not present or not matched:
                rescan_needed = True
        else:
            actual_d.append(ui.devs_list[index])

    def t1_run(e, dev, index, thread_list, result_list, status_list, count, password, hash_au):
        #dev = ui.devs_list[index]
        #print dev
        noPW = False
        status_final = -1
        dev_msid = ui.msid_list[index]
        pass_usb = ''
        #print dev_msid
        if password == None:
            (password, pass_usb) = runprocess.passReadUSB('Admin', dev, ui.vendor_list[index], ui.sn_list[index])
        if password == None or password == 'x':
            noPW = True
        if not e.is_set() and not noPW:
            runop.prelock(index)
            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
            timeStr = timeStr[2:]
            p0 = os.popen(ui.prefix + "sedutil-cli --query " + dev).read()
            txtL = "Locked = Y"
            is_Locked = re.search(txtL, p0)
            status1 = -1
            if ui.DEV_OS == 'Windows':
                with open(os.devnull, 'w') as pipe:
                    if is_Locked:
                        status1 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--setLockingRange', ui.LKRNG, ui.LKATTR, password, dev], stdout=pipe)#stderr=log)
                        if status1 == 0 and not e.is_set() and index in ui.mbr_list:
                            status1 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--setMBRdone', 'on', password, dev], stdout=pipe)#stderr=log)
                    else:
                        status1 = 0
                    if status1 == 0 and not e.is_set():
                        statusAW = 0
                        if ui.datastore_list[index] == 'Supported':
                            statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '03' + timeStr, password, 'Admin1', dev], stdout=pipe)#stderr=log)
                        if statusAW == 0:
                            #--query to find DataStore max table size, proceed if feature code found, set max entries to proper value
                            p = subprocess.Popen([ui.prefix + 'sedutil-cli', '-n', '-t', '--query', dev], stdout=subprocess.PIPE)
                            output = p.communicate()[0]
                            mtsRegex = 'Max Size Tables = ([0-9]+)'
                            m = re.search(mtsRegex, output)
                            rewrite = False
                            if m:
                                rewrite = True
                                mts = int(m.group(1))
                                max_entries = 1000
                                if mts < 10485760:
                                    max_entries = 100
                                len = 50 + (8 * max_entries)
                                #--datastoreread to grab all information and store in a temp file
                                s1 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--datastoreread', password, 'Admin1', 'dataread' + ui.sn_list[index] + '.txt', '0', '0', str(len), dev], stdout=pipe)#stderr=log)
                            status = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--setSIDPassword', password, dev_msid, dev], stdout=pipe)#stderr=log)
                            if status == 0:
                                status = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--revertnoerase', password, dev], stdout=pipe)#stderr=log)
                                if status == 0:
                                    p0 = os.popen(ui.prefix + "sedutil-cli --query " + dev).read()
                                    txtLE = "LockingEnabled = N"
                                    le_check = re.search(txtLE, p0)
                                    if le_check:
                                        status = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--activate', dev_msid, dev], stdout=pipe)#stderr=log)
                                        if pass_usb != '':
                                            if ui.DEV_OS == 'Windows':
                                                filepath = pass_usb + '\\OpalLock\\' + ui.vendor_list[index] + '_' + ui.sn_list[index] + '.psw'
                                                if os.path.isfile(filepath):
                                                    os.remove(filepath)
                                                #also look for AppData password file and erase if exists
                                            elif ui.DEV_OS == 'Linux':
                                                filepath = pass_usb + '/OpalLock/' + ui.vendor_list[index] + '_' + ui.sn_list[index] + '.psw'
                                                if os.path.isfile(filepath):
                                                    os.remove(filepath)
                                        status_final = status
                                        #--datastorewrite to take contents of temp file and rewrite back to DataStore
                                        if rewrite:
                                            s1 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--datastorewrite', dev_msid, 'Admin1', 'dataread' + ui.sn_list[index] + '.txt', '0', '0', str(len), dev], stdout=pipe)#stderr=log)
                                            os.remove('dataread' + ui.sn_list[index] + '.txt')
                                    else:
                                        status_final = status
                                else:
                                    subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--setSIDPassword', dev_msid, password, dev], stdout=pipe)#stderr=log)
                                    status_final = status
                            else:
                                status_final = status
                        else:
                            status_final = statusAW
                    elif not e.is_set():
                        status_final = status1
            else:
                if is_Locked:
                    status1 =  os.system(ui.prefix + 'sedutil-cli -n -t --setLockingRange ' + ui.LKRNG + ' ' + ui.LKATTR + ' "' + password + '" ' + dev)
                    if status1 == 0 and not e.is_set() and index in ui.mbr_list:
                        status1 =  os.system(ui.prefix + 'sedutil-cli -n -t --setMBRdone on "' + password + '" ' + dev )
                else:
                    status1 = 0
                if status1 == 0 and not e.is_set():
                    statusAW = 0
                    if ui.datastore_list[index] == 'Supported':
                        statusAW = os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 03' + timeStr + ' "' + password + '" Admin1 ' + dev)
                    if statusAW == 0:
                        p0 = os.popen(ui.prefix + "sedutil-cli --query " + dev).read()
                        mtsRegex = 'Max Size Tables = ([0-9]+)'
                        m = re.search(mtsRegex, p0)
                        rewrite = False
                        if m:
                            rewrite = True
                            mts = int(m.group(1))
                            max_entries = 1000
                            if mts < 10485760:
                                max_entries = 100
                            len = 50 + (8 * max_entries)
                            #--datastoreread to grab all information and store in a temp file
                            s1 = os.system(ui.prefix + 'sedutil-cli -n -t --datastoreread "' + password + '" Admin1 dataread' + ui.sn_list[index] + '.txt 0 0 ' + str(len) + ' ' + dev)
                        status = os.system(ui.prefix + 'sedutil-cli -n -t --setSIDPassword "' + password + '" "' + dev_msid + '" ' + dev)
                        if status == 0:
                            status = os.system(ui.prefix + 'sedutil-cli -n -t --revertnoerase "' + password + '" ' + dev)
                            if status == 0:
                                txtLE = "LockingEnabled = N"
                                le_check = re.search(txtLE, p0)
                                if le_check:
                                    status = os.system(ui.prefix + 'sedutil-cli -n -t --activate "' + dev_msid + '" ' + dev)
                                    if pass_usb != '':
                                        if ui.DEV_OS == 'Windows':
                                            filepath = pass_usb + '\\OpalLock\\' + ui.vendor_list[index] + '_' + ui.sn_list[index] + '.psw'
                                            if os.path.isfile(filepath):
                                                os.remove(filepath)
                                        elif ui.DEV_OS == 'Linux':
                                            filepath = pass_usb + '/OpalLock/' + ui.vendor_list[index] + '_' + ui.sn_list[index] + '.psw'
                                            if os.path.isfile(filepath):
                                                os.remove(filepath)
                                    if rewrite:
                                        s1 = os.system(ui.prefix + 'sedutil-cli -n -t --datastorewrite "' + dev_msid + '" Admin1 dataread' + ui.sn_list[index] + '.txt 0 0 ' + str(len) + ' ' + dev)
                                        os.remove('dataread' + ui.sn_list[index] + '.txt')
                                    status_final = status
                                else:
                                    status_final = status
                            else:
                                os.system(ui.prefix + 'sedutil-cli -n -t --setSIDPassword "' + dev_msid + '" "' + password + '" ' + dev)
                                status_final = status
                        else:
                            status_final = status
                    else:
                        status_final = statusAW
                elif not e.is_set():
                    status_final = status1
            def aw_run(status, i, dev):
                if ui.DEV_OS == 'Windows':
                    with open(os.devnull, 'w') as pipe:
                        if status != 0 and ui.datastore_list[i] == 'Supported':
                            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                            timeStr = timeStr[2:]
                            time.sleep(2)
                            if status == ui.NOT_AUTHORIZED and ui.admin_aol_list[i] < ui.retrylimit_list[i]:
                                statusAW1 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '04' + timeStr, hash_au, 'User' + ui.user_list[i], dev], stdout=pipe)#stderr=log)
                            elif status == ui.AUTHORITY_LOCKED_OUT or ui.admin_aol_list[i] >= ui.retrylimit_list[i]:
                                statusAW1 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '05' + timeStr, hash_au, 'User' + ui.user_list[i], dev], stdout=pipe)#stderr=log)
                                statusAW1 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '09' + timeStr, hash_au, 'User' + ui.user_list[i], dev], stdout=pipe)#stderr=log)
                            statusAW1 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '36' + timeStr, hash_au, 'User' + ui.user_list[i], dev], stdout=pipe)#stderr=log)
                        elif ui.datastore_list[i] == 'Supported':
                            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                            timeStr = timeStr[2:]
                            time.sleep(2)
                            statusAW1 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '18' + timeStr, dev_msid, 'Admin1', dev], stdout=pipe)#stderr=log)
                            statusAW1 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '01' + timeStr, dev_msid, 'Admin1', dev], stdout=pipe)#stderr=log)
                else:
                    if status != 0 and ui.datastore_list[i] == 'Supported':
                        timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                        timeStr = timeStr[2:]
                        time.sleep(2)
                        if status == ui.NOT_AUTHORIZED and ui.admin_aol_list[i] < ui.retrylimit_list[i]:
                            statusAW1 = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 04' + timeStr + ' "' + hash_au + '" User' + ui.user_list[i] + ' ' + dev)
                        elif status == ui.AUTHORITY_LOCKED_OUT or ui.admin_aol_list[i] >= ui.retrylimit_list[i]:
                            statusAW1 = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 05' + timeStr + ' "' + hash_au + '" User' + ui.user_list[i] + ' ' + dev)
                            statusAW1 = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 09' + timeStr + ' "' + hash_au + '" User' + ui.user_list[i] + ' ' + dev)
                        statusAW2 = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 36' + timeStr + ' "' + hash_au + '" User' + ui.user_list[i] + ' ' + dev)
                    elif ui.datastore_list[i] == 'Supported':
                        timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                        timeStr = timeStr[2:]
                        time.sleep(2)
                        statusAW1 = os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 18' + timeStr + ' "' + dev_msid + '" Admin1 ' + dev)
                        statusAW2 = os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 01' + timeStr + ' "' + dev_msid + '" Admin1 ' + dev)
                runop.postlock(i)
                
            t_aw = threading.Thread(target=aw_run, args=(status_final, index, dev))
            t_aw.start()
            time.sleep(0.5)
        gobject.idle_add(cleanup1, dev, index, status_final, thread_list, result_list, status_list, count, noPW, e)
    
    def cleanup1(dev, i, status, thread_list, result_list, status_list, count, no_pw, e):
        #dev = ui.devs_list[i]
        if e.is_set():
            result_list[count] = 2
        elif status != 0 or no_pw:
            if status == ui.NOT_AUTHORIZED and ui.admin_aol_list[i] < ui.retrylimit_list[i]:
                result_list[count] = 1
                ui.admin_aol_list[i] = ui.admin_aol_list[i] + 1
            elif status == ui.AUTHORITY_LOCKED_OUT or ui.admin_aol_list[i] >= ui.retrylimit_list[i]:
                result_list[count] = 1
                ui.admin_aol_list[i] = ui.retrylimit_list[i] + 1
            elif status == ui.SP_BUSY:
                result_list[count] = 4
            else:
                result_list[count] = 3
        else :
            result_list[count] = 0
            ui.admin_aol_list[i] = 0
            ui.user_aol_list[i] = 0
            if i in ui.usetup_list:
                ui.usetup_list.remove(i)
            if i in ui.ulocked_list:
                ui.ulocked_list.remove(i)
        status_list[count] = status
    
                
    e_to = threading.Event()
    start_time = time.time()
    #thread_list = [None] * len(selected_list)
    proc_list = [None] * len(selected_list)
    #result_list = [None] * len(selected_list)
    result_list = Array('i', len(selected_list))
    #status_list = [None] * len(selected_list)
    status_list = Array('i', len(selected_list))
    count = 0
    for idx in range(len(selected_list)):
        result_list[idx] = -1
        status_list[idx] = -1
        if actual_d[idx] == None:
            result_list[idx] = 6
            status_list[idx] = -2
    for i in selected_list:
        if actual_d[count] != None:
            password = None
            if ui.check_pass_rd.get_active():
                password = None
                #password = runprocess.passReadUSB(ui, ui.vendor_list[i], ui.sn_list[i], 0)
            else:
                pw_trim = re.sub('\s','',ui.pass_entry.get_text())
                password = lockhash.hash_pass(pw_trim, i, ui)#ui.salt_list[i], ui.msid_list[i])
            #au = '"' + lockhash.get_val() + ui.salt_list[i] + '"'
            au = lockhash.get_val() + ui.salt_list[i]
            hash_au = lockhash.hash_pass(au, i, ui)#ui.salt_list[i], ui.msid_list[i])
            proc_list[count] = threading.Thread(target=t1_run, args=(e_to, actual_d[count], i, proc_list, result_list, status_list, count, password, hash_au))
            proc_list[count].start()
        count = count + 1
        
    t2 = threading.Thread(target=timeout_track, args=(ui, 60.0*len(selected_list), start_time, proc_list, result_list, e_to, selected_list, status_list, cleanop.revertKeep_cleanup, rescan_needed, -1, -1, -1))
    t2.start()

def rt_revertErase(ui, selected_list):
    rescan_needed = False
    scan_check = False
    
    txt_s = ''
    rgx = '(PhysicalDrive[0-9]+|/dev/sd[a-z][0-9]?|/dev/nvme[0-9])\s+(?:[12ELP]+|No)\s+([^\:\s]+(?:\s[^\:\s]+)*)\s*:\s*[^:]+\s*:\s*(\S+)'
    
    list_d = []
    actual_d = []
    
    for index in selected_list:
        if ui.tcg_usb_list[index]:
            scan_check = True
    if scan_check:
        for x in range(len(ui.devs_list)):
            runop.prelock(x)
        txt_s = os.popen(ui.prefix + 'sedutil-cli --scan n').read()
        for x in range(len(ui.devs_list)):
            runop.postlock(x)
        list_d = re.findall(rgx, txt_s)
            
    for index in selected_list:
        if ui.tcg_usb_list[index]:
            new_devname = None
            matched = False
            present = False
            
            for entry in list_d:
                if not present or not matched:
                    test_dev = entry[0]
                    if ui.DEV_OS == 'Windows':
                        test_dev = '\\\\.\\' + test_dev
                    if test_dev == ui.devs_list[index] and entry[1] == ui.vendor_list[index] and entry[2] == ui.sn_list[index]:
                        present = True
                        matched = True
                    elif entry[1] == ui.vendor_list[index] and entry[2] == ui.sn_list[index]:
                        present = True
                        new_devname = test_dev
            if present and not matched:
                actual_d.append(new_devname)
            elif matched:
                actual_d.append(ui.devs_list[index])
            else:
                actual_d.append(None)
            if not present or not matched:
                rescan_needed = True
        else:
            actual_d.append(ui.devs_list[index])

    def t1_run(e, dev, index, thread_list, result_list, status_list, count, password, hash_au):
        noPW = False
        status_final = -1
        dev_msid = ui.msid_list[index]
        pass_usb = ''
        if password == None:
            (password, pass_usb) = runprocess.passReadUSB('Admin', dev, ui.vendor_list[index], ui.sn_list[index])
        if password == None or password == 'x':
            noPW = True
        
        if not e.is_set() and not noPW:
            runop.prelock(index)
            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
            timeStr = timeStr[2:]
            status = 0
            if ui.DEV_OS == 'Windows':
                with open(os.devnull, 'w') as pipe:
                    if index in ui.locked_list:
                        status = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--setlockingrange', ui.LKRNG, ui.LKATTR, password, dev], stdout=pipe)#stderr=log)
                        status_final = status
                    rewrite = False
                    len = 0
                    if status == 0:
                        p = subprocess.Popen([ui.prefix + 'sedutil-cli', '-n', '-t', '--query', dev], stdout=subprocess.PIPE)
                        output = p.communicate()[0]
                        mtsRegex = 'Max Size Tables = ([0-9]+)'
                        m = re.search(mtsRegex, output)
                        
                        if m:
                            rewrite = True
                            mts = int(m.group(1))
                            max_entries = 1000
                            if mts < 10485760:
                                max_entries = 100
                            len = 50 + (8 * max_entries)
                            #--datastoreread to grab all information and store in a temp file
                            s1 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--datastoreread', password, 'Admin1', 'dataread' + ui.sn_list[index] + '.txt', '0', '0', str(len), dev], stdout=pipe)#stderr=log)
                        status = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--revertTPer', password, dev], stdout=pipe)#stderr=log)
                        status_final = status
                    if status == 0:
                        if pass_usb != '':
                            if ui.DEV_OS == 'Windows':
                                filepath = pass_usb + '\\OpalLock\\' + ui.vendor_list[index] + '_' + ui.sn_list[index] + '.psw'
                                if os.path.isfile(filepath):
                                    os.remove(filepath)
                            elif ui.DEV_OS == 'Linux':
                                filepath = pass_usb + '/OpalLock/' + ui.vendor_list[index] + '_' + ui.sn_list[index] + '.psw'
                                if os.path.isfile(filepath):
                                    os.remove(filepath)
                        status = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--activate', dev_msid, dev], stdout=pipe)#stderr=log)
                        if rewrite:
                            s1 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--datastorewrite', dev_msid, 'Admin1', 'dataread' + ui.sn_list[index] + '.txt', '0', '0', str(len), dev], stdout=pipe)#stderr=log)
                            os.remove('dataread' + ui.sn_list[index] + '.txt')
            else:
                if index in ui.locked_list:
                    status = os.system(ui.prefix + 'sedutil-cli -n -t --setlockingrange ' + ui.LKRNG + ' ' + ui.LKATTR + ' "' + password + '" ' + dev)
                    status_final = status
                rewrite = False
                len = 0
                if status == 0:
                    p0 = os.popen(ui.prefix + "sedutil-cli --query " + dev).read()
                    mtsRegex = 'Max Size Tables = ([0-9]+)'
                    m = re.search(mtsRegex, p0)
                    if m:
                        rewrite = True
                        mts = int(m.group(1))
                        max_entries = 1000
                        if mts < 10485760:
                            max_entries = 100
                        len = 50 + (8 * max_entries)
                        #--datastoreread to grab all information and store in a temp file
                        s1 = os.system(ui.prefix + 'sedutil-cli -n -t --datastoreread "' + password + '" Admin1 dataread' + ui.sn_list[index] + '.txt 0 0 ' + str(len) + ' ' + dev)
                    status = os.system(ui.prefix + 'sedutil-cli -n -t --revertTPer "' + password + '" ' + dev)
                    status_final = status
                if status == 0:
                    if pass_usb != '':
                        if ui.DEV_OS == 'Windows':
                            filepath = pass_usb + '\\OpalLock\\' + ui.vendor_list[index] + '_' + ui.sn_list[index] + '.psw'
                            if os.path.isfile(filepath):
                                os.remove(filepath)
                        elif ui.DEV_OS == 'Linux':
                            filepath = pass_usb + '/OpalLock/' + ui.vendor_list[index] + '_' + ui.sn_list[index] + '.psw'
                            if os.path.isfile(filepath):
                                os.remove(filepath)
                    status = os.system(ui.prefix + 'sedutil-cli -n -t --activate "' + dev_msid + '" ' + dev)
                    if rewrite:
                        s1 = os.system(ui.prefix + 'sedutil-cli -n -t --datastorewrite "' + dev_msid + '" Admin1 dataread' + ui.sn_list[index] + '.txt 0 0 ' + str(len) + ' ' + dev)
                        os.remove('dataread' + ui.sn_list[index] + '.txt')
                
            def aw_run(status, index, dev):
                if ui.datastore_list[index] == 'Supported':
                    if ui.DEV_OS == 'Windows':
                        with open(os.devnull, 'w') as pipe:
                            if status != 0 or noPW:
                                result_list[count] = 1
                                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                                timeStr = timeStr[2:]
                                time.sleep(2)
                                #prelock(index)
                                if status == ui.NOT_AUTHORIZED and ui.admin_aol_list[index] < ui.retrylimit_list[index]:
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '04' + timeStr, hash_au, 'User' + ui.user_list[index], dev], stdout=pipe)#stderr=log)
                                elif status == ui.AUTHORITY_LOCKED_OUT or ui.admin_aol_list[index] >= ui.retrylimit_list[index]:
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '05' + timeStr, hash_au, 'User' + ui.user_list[index], dev], stdout=pipe)#stderr=log)
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '09' + timeStr, hash_au, 'User' + ui.user_list[index], dev], stdout=pipe)#stderr=log)
                                statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '37' + timeStr, hash_au, 'User' + ui.user_list[index], dev], stdout=pipe)#stderr=log)
                                #postlock(index)
                            else:
                                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                                timeStr = timeStr[2:]
                                time.sleep(2)
                                #prelock(index)
                                statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '19' + timeStr, dev_msid, 'Admin1', dev], stdout=pipe)#stderr=log)
                                statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '01' + timeStr, dev_msid, 'Admin1', dev], stdout=pipe)#stderr=log)
                    else:
                        if status != 0 or noPW:
                            result_list[count] = 1
                            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                            timeStr = timeStr[2:]
                            time.sleep(2)
                            #prelock(index)
                            if status == ui.NOT_AUTHORIZED and ui.admin_aol_list[index] < ui.retrylimit_list[index]:
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 04' + timeStr + ' "' + hash_au + '" User' + ui.user_list[index] + ' ' + dev)
                            elif status == ui.AUTHORITY_LOCKED_OUT or ui.admin_aol_list[index] >= ui.retrylimit_list[index]:
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 05' + timeStr + ' "' + hash_au + '" User' + ui.user_list[index] + ' ' + dev)
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 09' + timeStr + ' "' + hash_au + '" User' + ui.user_list[index] + ' ' + dev)
                            statusAW1 = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 37' + timeStr + ' "' + hash_au + '" User' + ui.user_list[index] + ' ' + dev)
                            #postlock(index)
                        else:
                            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                            timeStr = timeStr[2:]
                            time.sleep(2)
                            #prelock(index)
                            statusAW2 = os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 19' + timeStr + ' "' + dev_msid + '" Admin1 ' + dev)
                            statusAW3 = os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 01' + timeStr + ' "' + dev_msid + '" Admin1 ' + dev)
                runop.postlock(index)
            t_aw = threading.Thread(target=aw_run, args=(status_final, index, dev))
            t_aw.start()
            time.sleep(0.5)
        gobject.idle_add(cleanup1, dev, index, status_final, thread_list, result_list, status_list, count, noPW, e)
        
    
    def cleanup1(dev, i, status, thread_list, result_list, status_list, count, no_pw, e):
        if e.is_set():
            result_list[count] = 2
        elif status != 0 or no_pw:
            result_list[count] = 1
            if status == ui.NOT_AUTHORIZED and ui.admin_aol_list[i] < ui.retrylimit_list[i]:
                ui.admin_aol_list[i] = ui.admin_aol_list[i] + 1
            elif status == ui.AUTHORITY_LOCKED_OUT or ui.admin_aol_list[i] >= ui.retrylimit_list[i]:
                ui.admin_aol_list[i] = ui.retrylimit_list[i] + 1
            elif status == ui.SP_BUSY:
                result_list[count] = 4
            else:
                result_list[count] = 3
        else :
            result_list[count] = 0
            
            ui.admin_aol_list[i] = 0
            ui.user_aol_list[i] = 0
            if i in ui.usetup_list:
                ui.usetup_list.remove(i)
            if i in ui.ulocked_list:
                ui.ulocked_list.remove(i)
        status_list[count] = status

        
    e_to = threading.Event()
    start_time = time.time()
    
    
    proc_list = [None] * len(selected_list)
    result_list = Array('i', len(selected_list))
    status_list = Array('i', len(selected_list))
    count = 0
    
    
    for idx in range(len(selected_list)):
        result_list[idx] = -1
        status_list[idx] = -1
        if actual_d[idx] == None:
            result_list[idx] = 6
            status_list[idx] = -2
    for i in selected_list:
        if actual_d[count] != None:
            password = None
            if ui.check_pass_rd.get_active():
                password = None
            else:
                pw_trim = re.sub('\s','',ui.pass_entry.get_text())
                password = lockhash.hash_pass(pw_trim, i, ui)
            au_pwd = lockhash.get_val() + ui.salt_list[i]
            au_hash = lockhash.hash_pass(au_pwd, i, ui)
            proc_list[count] = threading.Thread(target=t1_run, args=(e_to, actual_d[count], i, proc_list, result_list, status_list, count, password, au_hash))
            proc_list[count].start()
        count = count + 1
        
    t2 = threading.Thread(target=timeout_track, args=(ui, 60.0*len(selected_list), start_time, proc_list, result_list, e_to, selected_list, status_list, cleanop.revertErase_cleanup, rescan_needed, -1, -1, -1))
    t2.start()

def rt_revertPSID(ui, psid):
    index = ui.dev_select.get_active()
    new_devname = None
    
    rescan_needed = False
    present = True
    status = -1
            
    if ui.tcg_usb_list[index]:
        matched = False
        present = False
        for x in range(len(ui.devs_list)):
            runop.prelock(x)
        txt_s = os.popen(ui.prefix + 'sedutil-cli --scan n').read()
        for x in range(len(ui.devs_list)):
            runop.postlock(x)
        rgx = '(PhysicalDrive[0-9]+|/dev/sd[a-z][0-9]?|/dev/nvme[0-9])\s+(?:[12ELP]+|No)\s+([^\:\s]+(?:\s[^\:\s]+)*)\s*:\s*[^:]+\s*:\s*(\S+)'
        list_d = re.findall(rgx, txt_s)
        for entry in list_d:
            if not present or not matched:
                test_dev = entry[0]
                if ui.DEV_OS == 'Windows':
                    test_dev = '\\\\.\\' + test_dev
                if test_dev == ui.devname and entry[1] == ui.vendor_list[index] and entry[2] == ui.sn_list[index]:
                    present = True
                    matched = True
                elif entry[1] == ui.vendor_list[index] and entry[2] == ui.sn_list[index]:
                    present = True
                    new_devname = test_dev
        if present and not matched:
            ui.devname = new_devname
        if not present or not matched:
            rescan_needed = True
            
    if present:
        runop.prelock(index)
        if ui.DEV_OS == 'Windows':
            with open(os.devnull, 'w') as pipe:
                rewrite = False
                len = 0
                p = subprocess.Popen([ui.prefix + 'sedutil-cli', '-n', '-t', '--query', dev], stdout=subprocess.PIPE)
                output = p.communicate()[0]
                mtsRegex = 'Max Size Tables = ([0-9]+)'
                m = re.search(mtsRegex, output)
                
                if m:
                    mts = int(m.group(1))
                    max_entries = 1000
                    if mts < 10485760:
                        max_entries = 100
                    len = 50 + (8 * max_entries)
                    #--datastoreread to grab all information and store in a temp file
                    pwd = lockhash.get_val() + ui.salt_list[index]
                    hash_pwd = lockhash.hash_pass(pwd, index, ui)
                    s1 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--datastoreread', hash_pwd, 'User' + ui.user_list[index], 'dataread' + ui.sn_list[index] + '.txt', '0', '0', str(len), dev], stdout=pipe)#stderr=log)
                    if s1 == 0:
                        rewrite = True
                status = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--yesIreallywanttoERASEALLmydatausingthePSID', psid, ui.devname], stdout=pipe)#stderr=log)
                if status == 0:
                    dev_msid = ui.msid_list[index]
                    subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--activate', dev_msid, ui.devname], stdout=pipe)#stderr=log)
                    if rewrite:
                        s1 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--datastorewrite', dev_msid, 'Admin1', 'dataread' + ui.sn_list[index] + '.txt', '0', '0', str(len), dev], stdout=pipe)#stderr=log)
                        os.remove('dataread' + ui.sn_list[index] + '.txt')
        else:
            rewrite = False
            len = 0
            p0 = os.popen(ui.prefix + "sedutil-cli --query " + dev).read()
            mtsRegex = 'Max Size Tables = ([0-9]+)'
            m = re.search(mtsRegex, p0)
            if m:
                mts = int(m.group(1))
                max_entries = 1000
                if mts < 10485760:
                    max_entries = 100
                len = 50 + (8 * max_entries)
                #--datastoreread to grab all information and store in a temp file
                pwd = lockhash.get_val() + ui.salt_list[index]
                hash_pwd = lockhash.hash_pass(pwd, index, ui)
                s1 = os.system(ui.prefix + 'sedutil-cli -n -t --datastoreread "' + hash_pwd + '" User' + ui.user_list[index] + ' dataread' + ui.sn_list[index] + '.txt 0 0 ' + str(len) + ' ' + dev)
                if s1 == 0:
                    rewrite = True
            status =  os.system(ui.prefix + "sedutil-cli -n -t --yesIreallywanttoERASEALLmydatausingthePSID " + psid + " " + ui.devname )
            if status == 0:
                dev_msid = ui.msid_list[index]
                os.system(ui.prefix + 'sedutil-cli -n -t --activate "' + dev_msid + '" ' + ui.devname)
                if rewrite:
                    s1 = os.system(ui.prefix + 'sedutil-cli -n -t --datastorewrite "' + dev_msid + '" Admin1 dataread' + ui.sn_list[index] + '.txt 0 0 ' + str(len) + ' ' + dev)
                    os.remove('dataread' + ui.sn_list[index] + '.txt')
        def aw_run(status, index, dev):
            if ui.datastore_list[index] == 'Supported':
                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                timeStr = timeStr[2:]
                time.sleep(2)
                if ui.DEV_OS == 'Windows':
                    with open(os.devnull, 'w') as pipe:
                        if status != 0:
                            pwd = lockhash.get_val() + ui.salt_list[index]
                            hash_pwd = lockhash.hash_pass(pwd, index, ui)
                            statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '38' + timeStr, hash_pwd, 'User' + ui.user_list[index], dev], stdout=pipe)#stderr=log)
                        else:
                            index = ui.dev_select.get_active()
                            dev_msid = ui.msid_list[index]
                            statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '20' + timeStr, dev_msid, 'Admin1', dev], stdout=pipe)#stderr=log)
                            statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '27' + timeStr, dev_msid, 'Admin1', dev], stdout=pipe)#stderr=log)
                            statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '01' + timeStr, dev_msid, 'Admin1', dev], stdout=pipe)#stderr=log)
                else:
                    if status != 0:
                        pwd = lockhash.get_val() + ui.salt_list[index]
                        hash_pwd = lockhash.hash_pass(pwd, index, ui)
                        statusAW1 = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 38' + timeStr + ' "' + hash_pwd + '" User' + ui.user_list[index] + ' ' + dev)
                    else:
                        index = ui.dev_select.get_active()
                        dev_msid = ui.msid_list[index]
                        statusAW1 = os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 20' + timeStr + ' "' + dev_msid + '" Admin1 ' + dev)
                        statusAW = os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 27' + timeStr + ' "' + dev_msid + '" Admin1 ' + dev)
                        statusAW2 = os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 01' + timeStr + ' "' + dev_msid + '" Admin1 ' + dev)
            runop.postlock(index)
        t_aw = threading.Thread(target=aw_run, args=(status,index,ui.devname))
        t_aw.start()
        time.sleep(0.5)
    gobject.idle_add(cleanop.revertPSID_cleanup, ui, status, present, rescan_needed)

def rt_queryAuth(self, parent, index, salt, user):
    failed = False
    devpass = None
    
    no_pass = False
    no_usb = False
    add_str = ''
    rc = -1
    
    statusAW = -1
    
    if self.passReadQ.get_active():
        model = parent.vendor_list[index]
        sn = parent.sn_list[index]
        #devpass = runop.passReadUSB(parent, model, sn)
        devpass = None
        latest_pw = None
        latest_ts = 0
        
        #nl = []
        #d_list = runprocess.findUSB(parent, nl)
        sl = [parent.devs_list[index]]
        mn = [parent.vendor_list[index]]
        sp = [parent.sn_list[index]]
        d_list = runprocess.findUSB('Admin', sl, mn, sp)
        folder_list = d_list[0]
        drive_list = d_list[1]
        if len(folder_list) > 0:
            for i in range(len(folder_list)):
                filename = model + '_' + sn + '.psw'
                if parent.DEV_OS == 'Windows':
                    filepath = folder_list[i] + "\\OpalLock\\" + filename
                elif parent.DEV_OS == 'Linux':
                    filepath = folder_list[i] + "/OpalLock/" + filename
                if os.path.isfile(filepath):
                    #pass_usb = folder_list[i]
                    try:
                        f = open(filepath, 'r')
                        txt = f.read()
                        auth = 'Admin'
                        #if parent.VERSION == 3 and parent.setupuser_list[index] == 'Yes' or parent.setupuser_list[index] == None:
                        #    auth = self.authQuery.get_active_text()
                        
                        pswReg = 'Timestamp: ([0-9]{14})\r?\n' + auth + ': (\S+)'
                        entry = re.search(pswReg, txt)
                        if entry:
                            ts = int(entry.group(1))
                            pw = entry.group(2)
                            if ts > latest_ts:
                                latest_ts = ts
                                latest_pw = pw
                        f.close()
                    except IOError:
                        print "file missing"
            if latest_pw == None:
                latest_pw = 'x'
        elif len(drive_list) > 0:
            latest_pw = 'x'
        devpass = latest_pw
        if devpass == None or devpass == 'x':
            failed = True
            if devpass == None:
                no_usb = True
            else:
                no_pass = True
    else:
        devpass = lockhash.hash_pass(self.queryPass.get_text(), index, parent)#salt, parent.dev_msid.get_text())
    audit_pwd = lockhash.get_val() + salt
    hash_pwd = lockhash.hash_pass(audit_pwd, index, parent)
    if not failed:
        level = 0
        #if parent.VERSION % 3 == 0  and parent.setupuser_list[index] == 'Yes':
        #    level = self.authQuery.get_active()
        new_devname = None
        
        rescan_needed = False
        
        if parent.tcg_usb_list[index]:
            matched = False
            present = False
            dl_len = len(parent.devs_list)
            for x in range(dl_len):
                runop.prelock(x)
            txt_s = os.popen(parent.prefix + 'sedutil-cli --scan n').read()
            for x in range(dl_len):
                runop.postlock(x)
            rgx = '(PhysicalDrive[0-9]+|/dev/sd[a-z][0-9]?|/dev/nvme[0-9])\s+(?:[12ELP]+|No)\s+([^\:\s]+(?:\s[^\:\s]+)*)\s*:\s*[^:]+\s*:\s*(\S+)'
            list_d = re.findall(rgx, txt_s)
            for entry in list_d:
                if not present or not matched:
                    test_dev = entry[0]
                    if parent.DEV_OS == 'Windows':
                        test_dev = '\\\\.\\' + test_dev
                    if test_dev == parent.devname and entry[1] == parent.vendor_list[index] and entry[2] == parent.sn_list[index]:
                        present = True
                        matched = True
                    elif entry[1] == parent.vendor_list[index] and entry[2] == parent.sn_list[index]:
                        present = True
                        new_devname = test_dev
            if present and not matched:
                parent.devname = new_devname
            if not present or not matched:
                rescan_needed = True
        
        runop.prelock(index)
        m3 = ''
        if level == 0:
            f0 = os.popen(parent.prefix + 'sedutil-cli -n -t --getmbrsize "' + devpass + '" ' + parent.devname)
            p0 = f0.read()
            rc = f0.close()
            #print 'getmbrsize return code: ' + str(rc)
            if rc == None:
                rc = 0
            out_regex = None
            #if platform.system() == 'Linux':
            out_regex = 'Shadow.+(?:\n.+)+'
            #elif platform.system() == 'Windows':
            #    out_regex = re.compile(ur'Shadow.+(?:\n.+)+', re.UNICODE)
            m3 = re.search(out_regex, p0)
            if not m3:
                if rc == parent.NOT_AUTHORIZED or rc == parent.AUTHORITY_LOCKED_OUT:
                    #pwd = lockhash.get_val() + salt
                    #hash_pwd = lockhash.hash_pass(pwd, index, parent)#salt, parent.dev_msid.get_text())
                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                    timeStr = timeStr[2:]
                    if rc == parent.NOT_AUTHORIZED and parent.admin_aol_list[index] < parent.retrylimit_list[index]:
                        if parent.DEV_OS == 'Windows':
                            with open(os.devnull, 'w') as pipe:
                                statusAW = subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '04' + timeStr, hash_pwd, 'User' + user, parent.devname], stdout=pipe)#stderr=log)
                        else:
                            statusAW = os.system(parent.prefix + 'sedutil-cli -n -t -u --auditwrite 04' + timeStr + ' "' + hash_pwd + '" User' + user + ' ' + parent.devname)
                    else:
                        if parent.DEV_OS == 'Windows':
                            with open(os.devnull, 'w') as pipe:
                                statusAW = subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '05' + timeStr, hash_pwd, 'User' + user, parent.devname], stdout=pipe)#stderr=log)
                        else:
                            statusAW = os.system(parent.prefix + 'sedutil-cli -n -t -u --auditwrite 05' + timeStr + ' "' + hash_pwd + '" User' + user + ' ' + parent.devname)
                    if parent.DEV_OS == 'Windows':
                        with open(os.devnull, 'w') as pipe:
                            subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '39' + timeStr, hash_pwd, 'User' + user, parent.devname], stdout=pipe)#stderr=log)
                    else:
                        os.system(parent.prefix + 'sedutil-cli -n -t -u --auditwrite 39' + timeStr + ' "' + hash_pwd + '" User' + user + ' ' + parent.devname)
                failed = True
        if not failed:
            p1 = ''
            p2 = ''
            #if parent.VERSION != 1:
            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
            timeStr = timeStr[2:]
            if parent.DEV_OS == 'Windows':
                with open(os.devnull, 'w') as pipe:
                    statusAW = subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '03' + timeStr, devpass, 'Admin1', parent.devname], stdout=pipe)#stderr=log)
            else:
                statusAW = os.system(parent.prefix + 'sedutil-cli -n -t -u --auditwrite 03' + timeStr + ' "' + devpass + '" Admin1 ' + parent.devname)
            if statusAW == parent.NOT_AUTHORIZED or statusAW == parent.AUTHORITY_LOCKED_OUT:
                #pwd = lockhash.get_val() + parent.salt_list[index]
                #hash_pwd = lockhash.hash_pass(pwd, index, parent)#parent.salt_list[index], parent.msid_list[index])
                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                timeStr = timeStr[2:]
                if statusAW == parent.NOT_AUTHORIZED and parent.admin_aol_list[index] < parent.retrylimit_list[index]:
                    if parent.DEV_OS == 'Windows':
                        with open(os.devnull, 'w') as pipe:
                            status = subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '04' + timeStr, hash_pwd, 'User' + user, parent.devname], stdout=pipe)#stderr=log)
                    else:
                        status = os.system(parent.prefix + 'sedutil-cli -n -t -u --auditwrite 04' + timeStr + ' "' + hash_pwd + '" User' + user + ' ' + parent.devname)
                else:
                    if parent.DEV_OS == 'Windows':
                        with open(os.devnull, 'w') as pipe:
                            statusAW = subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '05' + timeStr, hash_pwd, 'User' + user, parent.devname], stdout=pipe)#stderr=log)
                    else:
                        statusAW = os.system(parent.prefix + 'sedutil-cli -n -t -u --auditwrite 05' + timeStr + ' "' + hash_pwd + '" User' + user + ' ' + parent.devname)
                if parent.DEV_OS == 'Windows':
                    with open(os.devnull, 'w') as pipe:
                        subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '39' + timeStr, hash_pwd, 'User' + user, parent.devname], stdout=pipe)#stderr=log)
                else:
                    os.system(parent.prefix + 'sedutil-cli -n -t -u --auditwrite 39' + timeStr + ' "' + hash_pwd + '" User' + user + ' ' + parent.devname)
                failed = True
            elif statusAW != 0:
                failed = True
            else:
                if self.passReadQ.get_active():
                    if parent.DEV_OS == 'Windows':
                        with open(os.devnull, 'w') as pipe:
                            subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '25' + timeStr, devpass, 'Admin1', parent.devname], stdout=pipe)#stderr=log)
                    else:
                        os.system(parent.prefix + 'sedutil-cli -n -t -u --auditwrite 25' + timeStr + ' "' + devpass + '" Admin1 ' + parent.devname)
                if parent.DEV_OS == 'Windows':
                    with open(os.devnull, 'w') as pipe:
                        subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '21' + timeStr, devpass, 'Admin1', parent.devname], stdout=pipe)#stderr=log)
                else:
                    os.system(parent.prefix + 'sedutil-cli -n -t -u --auditwrite 21' + timeStr + ' "' + devpass + '" Admin1 ' + parent.devname)
                p2 = os.popen(parent.prefix + 'sedutil-cli -n -t -u --auditread "' + devpass + '" Admin1 ' + parent.devname).read()
            if parent.VERSION != 1 and not failed and index in parent.mbr_list:
                p1 = os.popen(parent.prefix + 'sedutil-cli -n -t --pbaValid "' + devpass + '" ' + parent.devname).read()
                r = None
                
                #if platform.system() == 'Windows':
                #    r = re.compile(ur'PBA image version\s*:', re.UNICODE)
                #elif platform.system() == 'Linux':
                r = 'PBA image version\s*:'
                t = re.search(r, p1)
                if not t:
                    failed = True
            if not failed:
                pba_regex = None
                #if platform.system() == 'Windows':
                #    pba_regex = re.compile(ur'PBA image version\s*:\s*(\S+)', re.UNICODE)
                #    audit_regex = re.compile(ur'Fidelity Audit Log Version\s*([0-9]+\.[0-9]+)\s*:', re.UNICODE)
                #elif platform.system() == 'Linux':
                
                audit_regex = 'Fidelity Audit Log Version\s*([0-9]+\.[0-9]+)\s*:'
                m2 = re.search(audit_regex, p2)
                audit_ver = ''
                if index in parent.mbr_list:
                    pba_regex = 'PBA image version\s*:\s*([A-z0-9\.\-]+)$'
                    m1 = re.search(pba_regex, p1)
                    
                    pba_ver = ''
                    
                    if m1:
                        pba_ver = m1.group(1)
                        pba_ver = pba_ver.strip('\0')
                    else:
                        pba_ver = 'N/A'
                    if m2:
                        audit_ver = m2.group(1)
                        audit_ver = audit_ver.strip('\0')
                    else:
                        audit_ver = 'N/A'
                    if parent.VERSION != 1:
                        if level == 0:
                            add_str = "\n" + m3.group(0) + "\n\nPreboot Image Version: " + pba_ver + "\nAudit Log Version: " + audit_ver
                        else:
                            add_str = "\nAudit Log Version: " + audit_ver
                    else:
                        add_str = "\n" + m3.group(0) + "\nAudit Log Version: " + audit_ver
                else:
                    if m2:
                        audit_ver = m2.group(1)
                        audit_ver = audit_ver.strip('\0')
                    else:
                        audit_ver = 'N/A'
                    if parent.VERSION != 1:
                        add_str = "\n" + m3.group(0) + "\n\nPreboot Image Version: Not Supported\nAudit Log Version: " + audit_ver
                    else:
                        add_str = "\n" + m3.group(0) + "\nAudit Log Version: " + audit_ver
                #print add_str
        runop.postlock(index)
    gobject.idle_add(cleanop.queryAuth_cleanup, self, index, failed, parent, add_str, rc, no_pass, no_usb)
    
def rt_openLog(parent, index):
    audit_pwd = ''
    hash_ap = ''
    pass_usb = ''
    drive = ''
    rd_usb = False
    no_pw = False
    no_USB = False
    not_detected = False
    not_setup = False
    not_active = False
    password = ''
    txt = ''
    new_devname = None
    save_status = -1
    
    rescan_needed = False
    present = True
    #runop.prelock(index)
    dl_len = len(parent.devs_list)
    for i in range(dl_len):
        runop.prelock(i)
    if parent.tcg_usb_list[index]:
        matched = False
        present = False
        txt_s = os.popen(parent.prefix + 'sedutil-cli --scan n').read()
        rgx = '(PhysicalDrive[0-9]+|/dev/sd[a-z][0-9]?|/dev/nvme[0-9])\s+(?:[12ELP]+|No)\s+([^\:\s]+(?:\s[^\:\s]+)*)\s*:\s*[^:]+\s*:\s*(\S+)'
        list_d = re.findall(rgx, txt_s)
        for entry in list_d:
            if not present or not matched:
                test_dev = entry[0]
                if parent.DEV_OS == 'Windows':
                    test_dev = '\\\\.\\' + test_dev
                if test_dev == parent.devname and entry[1] == parent.vendor_list[index] and entry[2] == parent.sn_list[index]:
                    present = True
                    matched = True
                elif entry[1] == parent.vendor_list[index] and entry[2] == parent.sn_list[index]:
                    present = True
                    new_devname = test_dev
        if present and not matched:
            parent.devname = new_devname
        if not present or not matched:
            rescan_needed = True
    for i in range(dl_len):
        if i != index:
            runop.postlock(i)
    statusAW = -1
    auth_level = parent.auth_menu.get_active()
    if present:
        
        if parent.check_pass_rd.get_active():
            (password, pass_usb) = runprocess.passReadUSB(parent.auth_menu.get_active_text(), parent.devs_list[index], parent.dev_vendor.get_text(), parent.dev_sn.get_text())
            rd_usb = True
            if password == None or password == 'x':
                no_pw = True
            #    parent.msg_err('No USB detected.')
            #    return
            #elif password == 'x':
            #    parent.msg_err('No password found for the drive.')
            #    return
        else:
            pw = re.sub('\s', '', parent.pass_entry.get_text())
            password = lockhash.hash_pass(pw, index, parent)#parent.salt_list[index], parent.dev_msid.get_text())
            parent.pass_entry.get_buffer().delete_text(0,-1)
        if not no_pw and parent.pass_sav.get_active():
            if parent.drive_menu.get_active() < 0:
                #parent.msg_err('No USB selected.')
                #return
                no_USB = True
            drive = parent.drive_menu.get_active_text()
            
            if parent.DEV_OS == 'Windows':
                drive = drive + '\\'
            if not os.path.isdir(drive):
                #parent.msg_err('Selected USB not detected')
                #return
                not_detected = True
        if not no_pw and not no_USB and not not_detected and parent.setupstatus_list[index] == 'No':
            qtxt = os.popen(parent.prefix + 'sedutil-cli --query ' + parent.devname).read()
            is_act = 'LockingEnabled\s*=\s*Y'
            m = re.search(is_act,qtxt)
            if m and password != parent.msid_list[index]:
                #parent.msg_err('This drive has not been set up, use the MSID to access the audit log.')
                #return
                not_setup = True
            elif not m:
                #parent.msg_err('This drive has not been activated. The audit log for this drive is not accessible.')
                #return
                not_active = True
                
        
                
        if not no_pw and not no_USB and not not_detected and not not_setup and not not_active:
            txt = ""
            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
            timeStr = timeStr[2:]
            
            pwd = lockhash.get_val() + parent.salt_list[index]
            hash_pwd = lockhash.hash_pass(pwd, index, parent)
            
            count = ''
            
            #print statusAW
            audit_pwd = lockhash.get_val() + parent.salt_list[index]
            hash_ap = lockhash.hash_pass(audit_pwd, index, parent)
            
            if parent.DEV_OS == 'Windows':
                with open(os.devnull, 'w') as pipe:
                    if parent.datastore_list[index] == 'Supported':
                        if auth_level == 0:
                            statusAW = subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '03' + timeStr, password, 'Admin1', parent.devname], stdout=pipe)#stderr=log)
                            if statusAW == 0:
                                if rd_usb:
                                    subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '25' + timeStr, password, 'Admin1', parent.devname], stdout=pipe)#stderr=log)
                                subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '22' + timeStr, password, 'Admin1', parent.devname], stdout=pipe)#stderr=log)
                                txt = os.popen(parent.prefix + 'sedutil-cli -n -t --auditread "' + password + '" Admin1 ' + parent.devname ).read()
                                parent.admin_aol_list[index] = 0
                                if parent.pass_sav.get_active():
                                    save_status = runprocess.passSaveUSB(password, parent.drive_menu.get_active_text(), parent.dev_vendor.get_text(), parent.dev_sn.get_text(), pass_usb, parent.auth_menu.get_active_text())
                                    if save_status == 0:
                                        statusAW = subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '23' + timeStr, password, 'Admin1', parent.devname], stdout=pipe)#stderr=log)
                            elif statusAW == parent.NOT_AUTHORIZED and parent.admin_aol_list[index] < parent.retrylimit_list[index]:
                                parent.admin_aol_list[index] = parent.admin_aol_list[index] + 1
                                subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '04' + timeStr, hash_pwd, 'User' + parent.user_list[index], parent.devname], stdout=pipe)#stderr=log)
                                subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '40' + timeStr, hash_pwd, 'User' + parent.user_list[index], parent.devname], stdout=pipe)#stderr=log)
                            elif statusAW == parent.AUTHORITY_LOCKED_OUT or parent.admin_aol_list[index] >= parent.retrylimit_list[index]:
                                parent.admin_aol_list[index] = parent.retrylimit_list[index] + 1
                                subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '05' + timeStr, hash_pwd, 'User' + parent.user_list[index], parent.devname], stdout=pipe)#stderr=log)
                                subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '40' + timeStr, hash_pwd, 'User' + parent.user_list[index], parent.devname], stdout=pipe)#stderr=log)
                                subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '09' + timeStr, hash_pwd, 'User' + parent.user_list[index], parent.devname], stdout=pipe)#stderr=log)
                        else:
                            statusAW = subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '06' + timeStr, password, 'User1', parent.devname], stdout=pipe)#stderr=log)
                            if statusAW == 0:
                                if rd_usb:
                                    subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '26' + timeStr, password, 'User1', parent.devname], stdout=pipe)#stderr=log)
                                subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '22' + timeStr, password, 'User1', parent.devname], stdout=pipe)#stderr=log)
                                txt = os.popen(parent.prefix + 'sedutil-cli -n -t -u --auditread "' + password + '" User1 ' + parent.devname ).read()
                                parent.user_aol_list[index] = 0
                                if parent.pass_sav.get_active():
                                    save_status = runprocess.passSaveUSB(password, parent.drive_menu.get_active_text(), parent.dev_vendor.get_text(), parent.dev_sn.get_text(), pass_usb, parent.auth_menu.get_active_text())
                                    if save_status == 0:
                                        statusAW = subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '24' + timeStr, password, 'User1', parent.devname], stdout=pipe)#stderr=log)
                            elif statusAW == parent.NOT_AUTHORIZED and parent.user_aol_list[index] < parent.retrylimit_list[index]:
                                parent.user_aol_list[index] = parent.user_aol_list[index] + 1
                                subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '07' + timeStr, hash_pwd, 'User' + parent.user_list[index], parent.devname], stdout=pipe)#stderr=log)
                                subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '40' + timeStr, hash_pwd, 'User' + parent.user_list[index], parent.devname], stdout=pipe)#stderr=log)
                            elif statusAW == parent.AUTHORITY_LOCKED_OUT or parent.user_aol_list[index] >= parent.retrylimit_list[index]:
                                parent.user_aol_list[index] = parent.retrylimit_list[index] + 1
                                subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '08' + timeStr, hash_pwd, 'User' + parent.user_list[index], parent.devname], stdout=pipe)#stderr=log)
                                subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '40' + timeStr, hash_pwd, 'User' + parent.user_list[index], parent.devname], stdout=pipe)#stderr=log)
                                subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '09' + timeStr, hash_pwd, 'User' + parent.user_list[index], parent.devname], stdout=pipe)#stderr=log)
                    #elif parent.datastore_list[index] == 'Supported':
                    #    statusAW = subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '03' + timeStr, password, 'Admin1', parent.devname], stdout=pipe)#stderr=log)
                    #    if statusAW == 0:
                    #        subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '22' + timeStr, password, 'Admin1', parent.devname], stdout=pipe)#stderr=log)
                    #        txt = os.popen(parent.prefix + 'sedutil-cli -n -t --auditread "' + password + '" Admin1 ' + parent.devname ).read()
                    #        parent.admin_aol_list[index] = 0
                    #        if parent.pass_sav.get_active():
                    #            save_status = runprocess.passSaveUSB(password, parent.drive_menu.get_active_text(), parent.dev_vendor.get_text(), parent.dev_sn.get_text(), pass_usb, #parent.auth_menu.get_active_text())
                    #             if save_status == 0:
                    #                statusAW = subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '23' + timeStr, password, 'Admin1', parent.devname], stdout=pipe)#stderr=log)
                    #    elif statusAW == parent.NOT_AUTHORIZED and parent.admin_aol_list[index] < parent.retrylimit_list[index]:
                    #        parent.admin_aol_list[index] = parent.admin_aol_list[index] + 1
                    #        subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '04' + timeStr, hash_pwd, 'User' + parent.user_list[index], parent.devname], stdout=pipe)#stderr=log)
                    #        subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '40' + timeStr, hash_pwd, 'User' + parent.user_list[index], parent.devname], stdout=pipe)#stderr=log)
                    #    elif statusAW == parent.AUTHORITY_LOCKED_OUT or parent.admin_aol_list[index] >= parent.retrylimit_list[index]:
                    #        parent.admin_aol_list[index] = parent.retrylimit_list[index] + 1
                    #        subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '05' + timeStr, hash_pwd, 'User' + parent.user_list[index], parent.devname], stdout=pipe)#stderr=log)
                    #        subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '40' + timeStr, hash_pwd, 'User' + parent.user_list[index], parent.devname], stdout=pipe)#stderr=log)
                    #        subprocess.call([parent.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '09' + timeStr, hash_pwd, 'User' + parent.user_list[index], parent.devname], stdout=pipe)#stderr=log)

            else:
                if parent.datastore_list[index] == 'Supported':
                    if auth_level == 0:
                        statusAW = os.system(parent.prefix + 'sedutil-cli -n -t --auditwrite 03' + timeStr + ' "' + password + '" Admin1 ' + parent.devname)
                        if statusAW == 0:
                            if rd_usb:
                                os.system(parent.prefix + 'sedutil-cli -n -t --auditwrite 25' + timeStr + ' "' + password + '" Admin1 ' + parent.devname)
                            os.system(parent.prefix + 'sedutil-cli -n -t --auditwrite 22' + timeStr + ' "' + password + '" Admin1 ' + parent.devname)
                            txt = os.popen(parent.prefix + 'sedutil-cli -n -t --auditread "' + password + '" Admin1 ' + parent.devname ).read()
                            parent.admin_aol_list[index] = 0
                            runprocess.passSaveAppData(password, parent.dev_vendor.get_text(), parent.dev_sn.get_text(), parent.auth_menu.get_active_text())
                            if parent.pass_sav.get_active():
                                save_status = runprocess.passSaveUSB(password, parent.drive_menu.get_active_text(), parent.dev_vendor.get_text(), parent.dev_sn.get_text(), pass_usb, parent.auth_menu.get_active_text())
                                if save_status == 0:
                                    statusAW = os.system(parent.prefix + 'sedutil-cli -n -t --auditwrite 23' + timeStr + ' "' + password + '" Admin1 ' + parent.devname)
                        elif statusAW == parent.NOT_AUTHORIZED and parent.admin_aol_list[index] < parent.retrylimit_list[index]:
                            parent.admin_aol_list[index] = parent.admin_aol_list[index] + 1
                            os.system(parent.prefix + 'sedutil-cli -n -t -u --auditwrite 04' + timeStr + ' "' + hash_pwd + '" User' + parent.user_list[index] + ' ' + parent.devname)
                            os.system(parent.prefix + 'sedutil-cli -n -t -u --auditwrite 40' + timeStr + ' "' + hash_pwd + '" User' + parent.user_list[index] + ' ' + parent.devname)
                        elif statusAW == parent.AUTHORITY_LOCKED_OUT or parent.admin_aol_list[index] >= parent.retrylimit_list[index]:
                            parent.admin_aol_list[index] = parent.retrylimit_list[index] + 1
                            os.system(parent.prefix + 'sedutil-cli -n -t -u --auditwrite 05' + timeStr + ' "' + hash_pwd + '" User' + parent.user_list[index] + ' ' + parent.devname)
                            os.system(parent.prefix + 'sedutil-cli -n -t -u --auditwrite 40' + timeStr + ' "' + hash_pwd + '" User' + parent.user_list[index] + ' ' + parent.devname)
                            os.system(parent.prefix + 'sedutil-cli -n -t -u --auditwrite 09' + timeStr + ' "' + hash_pwd + '" User' + parent.user_list[index] + ' ' + parent.devname)
                    else:
                        statusAW = os.system(parent.prefix + 'sedutil-cli -n -t -u --auditwrite 06' + timeStr + ' "' + password + '" User1 ' + parent.devname)
                        if statusAW == 0:
                            if rd_usb:
                                os.system(parent.prefix + 'sedutil-cli -n -t -u --auditwrite 26' + timeStr + ' "' + password + '" User1 ' + parent.devname)
                            os.system(parent.prefix + 'sedutil-cli -n -t -u --auditwrite 22' + timeStr + ' "' + password + '" User1 ' + parent.devname)
                            txt = os.popen(parent.prefix + 'sedutil-cli -n -t -u --auditread "' + password + '" User1 ' + parent.devname ).read()
                            parent.user_aol_list[index] = 0
                            runprocess.passSaveAppData(password, parent.dev_vendor.get_text(), parent.dev_sn.get_text(), parent.auth_menu.get_active_text())
                            if parent.pass_sav.get_active():
                                save_status = runprocess.passSaveUSB(password, parent.drive_menu.get_active_text(), parent.dev_vendor.get_text(), parent.dev_sn.get_text(), pass_usb, parent.auth_menu.get_active_text())
                                if save_status == 0:
                                    statusAW = os.system(parent.prefix + 'sedutil-cli -n -t -u --auditwrite 24' + timeStr + ' "' + hash_ap + '" User' + parent.user_list[index] + ' ' + parent.devname)
                        elif statusAW == parent.NOT_AUTHORIZED and parent.user_aol_list[index] < parent.retrylimit_list[index]:
                            parent.user_aol_list[index] = parent.user_aol_list[index] + 1
                            os.system(parent.prefix + 'sedutil-cli -n -t -u --auditwrite 07' + timeStr + ' "' + hash_pwd + '" User' + parent.user_list[index] + ' ' + parent.devname)
                            os.system(parent.prefix + 'sedutil-cli -n -t -u --auditwrite 40' + timeStr + ' "' + hash_pwd + '" User' + parent.user_list[index] + ' ' + parent.devname)
                        elif statusAW == parent.AUTHORITY_LOCKED_OUT or parent.user_aol_list[index] >= parent.retrylimit_list[index]:
                            parent.user_aol_list[index] = parent.retrylimit_list[index] + 1
                            os.system(parent.prefix + 'sedutil-cli -n -t -u --auditwrite 08' + timeStr + ' "' + hash_pwd + '" User' + parent.user_list[index] + ' ' + parent.devname)
                            os.system(parent.prefix + 'sedutil-cli -n -t -u --auditwrite 40' + timeStr + ' "' + hash_pwd + '" User' + parent.user_list[index] + ' ' + parent.devname)
                            os.system(parent.prefix + 'sedutil-cli -n -t -u --auditwrite 09' + timeStr + ' "' + hash_pwd + '" User' + parent.user_list[index] + ' ' + parent.devname)
                #elif parent.datastore_list[index] == 'Supported':
                #    statusAW = os.system(parent.prefix + 'sedutil-cli -n -t --auditwrite 03' + timeStr + ' "' + password + '" Admin1 ' + parent.devname)
                #    if statusAW == 0:
                #        os.system(parent.prefix + 'sedutil-cli -n -t --auditwrite 22' + timeStr + ' "' + password + '" Admin1 ' + parent.devname)
                #        txt = os.popen(parent.prefix + 'sedutil-cli -n -t --auditread "' + password + '" Admin1 ' + parent.devname ).read()
                #        parent.admin_aol_list[index] = 0
                #        if parent.pass_sav.get_active():
                #            save_status = runprocess.passSaveUSB(password, parent.drive_menu.get_active_text(), parent.dev_vendor.get_text(), parent.dev_sn.get_text(), pass_usb, parent.auth_menu.get_active_text())
                #            if save_status == 0:
                #                statusAW = os.system(parent.prefix + 'sedutil-cli -n -t --auditwrite 23' + timeStr + ' "' + password + '" Admin1 ' + parent.devname)
                #    elif statusAW == parent.NOT_AUTHORIZED and parent.admin_aol_list[index] < parent.retrylimit_list[index]:
                #        parent.admin_aol_list[index] = parent.admin_aol_list[index] + 1
                #        os.system(parent.prefix + 'sedutil-cli -n -t -u --auditwrite 04' + timeStr + ' "' + hash_pwd + '" User' + parent.user_list[index] + ' ' + parent.devname)
                #        os.system(parent.prefix + 'sedutil-cli -n -t -u --auditwrite 40' + timeStr + ' "' + hash_pwd + '" User' + parent.user_list[index] + ' ' + parent.devname)
                #    elif statusAW == parent.AUTHORITY_LOCKED_OUT or parent.admin_aol_list[index] >= parent.retrylimit_list[index]:
                #        parent.admin_aol_list[index] = parent.retrylimit_list[index] + 1
                #        os.system(parent.prefix + 'sedutil-cli -n -t -u --auditwrite 05' + timeStr + ' "' + hash_pwd + '" User' + parent.user_list[index] + ' ' + parent.devname)
                #        os.system(parent.prefix + 'sedutil-cli -n -t -u --auditwrite 40' + timeStr + ' "' + hash_pwd + '" User' + parent.user_list[index] + ' ' + parent.devname)
                #        os.system(parent.prefix + 'sedutil-cli -n -t -u --auditwrite 09' + timeStr + ' "' + hash_pwd + '" User' + parent.user_list[index] + ' ' + parent.devname)
    runop.postlock(index)
    gobject.idle_add(cleanop.openLog_cleanup, parent, index, statusAW, password, txt, no_pw, no_USB, not_detected, not_setup, not_active, auth_level, drive, rescan_needed, present, save_status)

def timeout_track(ui, max_time, start_time, op_procs, res_list, e, selected_list, status_list, cleanup_func, val, val2, val3, val4):
    done = False
    d = [0] * len(op_procs)
    while not done:
        time.sleep(1)
        c = 0
        for op_proc in op_procs:
            if op_proc == None:
                d[c] = 1
            elif op_proc.is_alive():
                curr_time = time.time()
                elapsed_time = curr_time - start_time
                if elapsed_time >= max_time:
                    done = True
                    break
            else:
                d[c] = 1
            c = c+1
        if sum(d) == len(d):
            done = True
        if not done and res_list != None:
            all_rep = True
            for i in range(len(res_list)):
                if res_list[i] == -1:
                    all_rep = False
            if all_rep:
                done = True
    #print 'end timeout_track'
    gobject.idle_add(cleanup_func, ui, max_time, start_time, op_procs, res_list, e, selected_list, status_list, val, val2, val3, val4)