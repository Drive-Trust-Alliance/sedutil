import cleanop
import datetime
import gobject
import lockhash
import os
import platform
import re
import runop
import runprocess
import runthread
import runupdate
if platform.system() == 'Windows':
    import subprocess
import threading
import time

from multiprocessing import Process, Value, Array, Event

def rt_setupFull(ui, selected_list, index2, preserved_files):
    rescan_needed = False
    scan_check = False
    
    txt_s = ''
    rgx = '(PhysicalDrive[0-9]+|/dev/sd[a-z][0-9]?|/dev/nvme[0-9])\s+(?:[12ELP]+|No)\s+([^\:\s]+(?:\s[^\:\s]+)*)\s*:\s*[^:]+\s*:\s*(\S+)'
    
    list_d = []
    actual_d = []
    
    ui.op_inprogress = True
    
    for index in selected_list:
        if ui.tcg_usb_list[index]:
            scan_check = True
    if scan_check:
        for x in range(len(ui.devs_list)):
            runop.prelock(x)
        txt_s = os.popen(ui.prefix + 'sedutil-cli --scan n').read()
        list_d = re.findall(rgx, txt_s)
        for x in range(len(ui.devs_list)):
            runop.postlock(x)
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
    
    ad_idx = 0
    for i in selected_list:
        if ui.msid_list[i] == 'N/A' and actual_d[ad_idx] != None:
            runop.prelock(ad_idx)
            txt_msid = os.popen(ui.prefix + "sedutil-cli --printDefaultPassword " + actual_d[ad_idx] ).read()
            runop.postlock(ad_idx)
            if txt_msid != '' :
                regex_msid = None
                #if dev_os == 'Windows':
                #    regex_msid = re.compile(ur'MSID:\s*(\S+)', re.UNICODE)
                #elif dev_os == 'Linux':
                #regex_msid = re.compile(ur'MSID:\s*(\S+)')
                regex_msid = 'MSID:\s*(\S+)'
                mm = re.search(regex_msid, txt_msid)
                if mm:
                    msid = mm.group(1)
                    if msid != 'N/A':
                        ui.msid_list[i] = msid
        ad_idx = ad_idx + 1
                
            
                
    e_to = Event()
    start_time = time.time()
    
    #thread_list = [None] * len(selected_list)
    proc_list = [None] * len(selected_list)
    #result_list = [None] * len(selected_list)
    result_list = Array('i', len(selected_list))
    #status_list = [None] * len(selected_list)
    status_list = Array('i', len(selected_list))
    
    trylimit_list = Array('i', len(selected_list))
    #status_usb = [None]
    status_usb = Value('i', -1)
    #rc_list = [None] * len(selected_list)
    rc_list = Array('i', len(selected_list))
    ui.process_list = [None] * len(selected_list)
    count = 0
    proceed = True
    for idx in range(len(selected_list)):
        result_list[idx] = -1
        status_list[idx] = -1
        rc_list[idx] = -1
        if actual_d[idx] == None:
            result_list[idx] = 6
            status_list[idx] = -2
            rc_list[idx] = -2
            proceed = False
            
    if proceed:
        pw_trim = re.sub('\s','',ui.confirm_pass_entry.get_text())
        
        for j in range(len(ui.devs_list)):
            runop.prelock(j)
        
        for i in selected_list:
            if actual_d[count] != None:
                fn = ui.salt_list[i]
                au = lockhash.get_val() + ui.salt_list[i]
                password = lockhash.hash_pass(pw_trim, i, ui)#ui.salt_list[i], ui.msid_list[i])
                au_pwd = lockhash.hash_pass(au, i, ui)
                if os.path.isfile(fn):
                    os.remove(fn)
                #thread_list[count] = threading.Thread(target=t1_run, args=(e_to, i, thread_list, result_list, status_list, count, rc_list, password, status_usb, usb_dir))
                #thread_list[count].start()
                usb_idx = -1
                if ui.VERSION != 4:
                    usb_idx = ui.usb_list[index2][1]
                proc_list[count] = Process(target=runprocess.rp_setupFull, args=(e_to, i, result_list, status_list, trylimit_list, count, rc_list, password, status_usb, ui.pass_dir, actual_d[count], ui.prefix, ui.msid_list[i], ui.vendor_list[i], ui.sn_list[i], usb_idx, ui.pba_list[i], ui.admin_list[i], ui.user_list[i], ui.VERSION, preserved_files, au_pwd))
                proc_list[count].start()
            count = count + 1
        
        t2 = None
        if ui.VERSION == 4:
            t2 = threading.Thread(target=runthread.timeout_track, args=(ui, 60.0 * len(selected_list), start_time, proc_list, result_list, e_to, selected_list, status_list, cleanop.setupFull_cleanup, rc_list, status_usb, trylimit_list, rescan_needed))
        else:
            t2 = threading.Thread(target=runthread.timeout_track, args=(ui, 1800.0 * len(selected_list), start_time, proc_list, result_list, e_to, selected_list, status_list, cleanop.setupFull_cleanup, rc_list, status_usb, trylimit_list, rescan_needed))
        t2.start()
        
        def t_pb_run(sel_list, res_list):
            regex_ip = '([0-9]+)\(([0-9]+)\)'
            regex_c = '[0-9]+\([0-9]+\) Complete PBA write'
            write_list = []
            len_orig = len(ui.salt_list)
            abort = False
            #print 'sel_list'
            #print sel_list
            #print 'salt_list'
            #print ui.salt_list
            for i in sel_list:
                if ui.datastore_list[i] != 'Not Supported':
                    write_list.append(i)
            #print 'write list'
            #print write_list
            #print 'filenames'
            #for w in write_list:
                #print ui.salt_list[w]
            write_start = False
            first_start = -1
            for j in write_list:
                #print 'pair ' + ui.devs_list[j] + ' ' + ui.salt_list[j]
                if os.path.isfile(ui.salt_list[j]):
                    if not write_start:
                        write_start = True
                        first_start = j
            while not write_start:
                time.sleep(1)
                if not ui.pbawrite_ip:
                    write_start = True
                    abort = True
                else:
                    for p in write_list:
                        if os.path.isfile(ui.salt_list[p]):
                            if not write_start:
                                write_start = True
                                first_start = p
            if not abort:
                m = None
                while not m:
                    time.sleep(1)
                    f = open(ui.salt_list[first_start], 'r')
                    txt = f.read()
                    f.close()
                    m = re.search(regex_ip, txt)
                ind_total = int(m.group(2))
                total = ind_total * len(write_list)
                
                all_done = False
                done = [False] * len(write_list)
                tracker = [0] * len(write_list)
                while not all_done:
                    curr = 0
                    all_done = True
                    for k in range(len(write_list)):
                        fn = ui.salt_list[write_list[k]]
                        
                        if os.path.isfile(fn):
                            f = open(fn, 'r')
                            txt = f.read()
                            f.close()
                            m1 = re.search(regex_ip, txt)
                            m2 = re.search(regex_c, txt)
                            if not done[k]:
                                if m2:
                                    done[k] = True
                                    curr = curr + ind_total
                                else:
                                    if res_list[k] != -1:
                                        done[k] = True
                                    else:
                                        all_done = False
                                    ind_curr = tracker[k]
                                    if m1:
                                        ind_curr_new = int(m1.group(1))
                                        if ind_curr < ind_curr_new:
                                            ind_curr = ind_curr_new
                                        tracker[k] = ind_curr
                                    #else:
                                    #    print ui.devs_list[sel_list[k]] + ui.salt_list[sel_list[k]]
                                    #    print 'failed to match'
                                    #    print txt
                                    curr = curr + ind_curr
                            else:
                                #if m2:
                                curr = curr + ind_total
                                #else:
                                #    ind_curr = tracker[k]
                                #    if m1:
                                #        ind_curr = int(m1.group(1))
                                #        tracker[k] = ind_curr
                                #    #else:
                                #        #print txt
                                #    curr = curr + ind_curr
                        else:
                            #print ui.devs_list[sel_list[k]] + ui.salt_list[sel_list[k]]
                            #print 'file not found'
                            curr = curr + tracker[k]
                            if tracker[k] < ind_total:
                                all_done = False
                    if not ui.pbawrite_ip:
                        all_done = True
                    ui.progress_bar.set_fraction(curr / (total * 1.0))
                    time.sleep(1)
            gobject.idle_add(cleanup_pb, write_list)
        
        #delete files after completion
        def cleanup_pb(sel_list):
            for i in sel_list:
                fn = ui.salt_list[i]
                #print str(i) + ' ' + fn
                if os.path.isfile(fn):
                    os.remove(fn)
            ui.progress_bar.hide();
        if ui.VERSION != 4:
            t_pb = threading.Thread(target=t_pb_run, args=(selected_list, result_list))
            t_pb.start()
    else:
        gobject.idle_add(cleanop.setupFull_abort, ui)
        


def rt_pbaWrite(ui, selected_list):
    ui.op_inprogress = True
    ui.pbawrite_ip = True
    
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
        list_d = re.findall(rgx, txt_s)
        for x in range(len(ui.devs_list)):
            runop.postlock(x)
            
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
    
    #e_to = threading.Event()
    e_to = Event()
    
    #thread_list = [None] * len(selected_list)
    proc_list = [None] * len(selected_list)
    #result_list = [None] * len(selected_list)
    result_list = Array('i', len(selected_list))
    #status_list = [None] * len(selected_list)
    status_list = Array('i', len(selected_list))
    ui.process_list = [None] * len(selected_list)
    count = 0
    #pw_list = []
    for idx in range(len(selected_list)):
        result_list[idx] = -1
        status_list[idx] = -1
        if actual_d[idx] == None:
            result_list[idx] = 6
            status_list[idx] = -2
    for j in range(len(ui.devs_list)):
        runop.prelock(j)
    for i in selected_list:
        if actual_d[count] != None:
            fn = ui.salt_list[i]
            password = None
            if ui.check_pass_rd.get_active():
                password = None
                #(pw, pass_usb) = runprocess.passReadUSB('Admin', ui.devs_list[i], ui.vendor_list[i], ui.sn_list[i])
                #pw_list.append(pw)
            else:
                pw_trim = re.sub('\s','',ui.pass_entry.get_text())
                password = lockhash.hash_pass(pw_trim, i, ui)#ui.salt_list[i], ui.msid_list[i])
                #pw_list.append(password)
            #au_pwd = '"' + lockhash.get_val() + ui.salt_list[i] + '"'
            au_pwd = lockhash.get_val() + ui.salt_list[i]
            hash_au_pwd = lockhash.hash_pass(au_pwd, i, ui)#ui.salt_list[i], ui.msid_list[i])
            if os.path.isfile(fn):
                os.remove(fn)
            ps_ga = 0
            #if ui.VERSION == 3:
            ps_ga = ui.pass_sav.get_active()
            proc_list[count] = Process(target=runupdate.rp_pbaWrite, args=(e_to, i, result_list, status_list, count, password, hash_au_pwd, actual_d[count], ui.prefix, ui.vendor_list[i], ui.sn_list[i], ui.datastore_list[i], ui.user_list[i], ui.VERSION, ui.admin_aol_list[i], ps_ga, ui.drive_menu.get_active_text()))
            proc_list[count].start()
        count = count + 1
    start_time = time.time()
    t2 = threading.Thread(target=runthread.timeout_track, args=(ui, 1800.0*len(selected_list), start_time, proc_list, result_list, e_to, selected_list, status_list, cleanop.pbaWrite_cleanup, rescan_needed, None, None, None))
    t2.start()
    
    #separate thread, pass selected_list
    #check whether the first file exists, sleep for a second if it doesn't exist yet
    #once first file exists, check each file every 5 seconds
    #update progress bar after each round of checking
    def t_pb_run(sel_list, res_list):
        #print 'sel_list'
        #print sel_list
        #print 'salt_list'
        #print ui.salt_list
        regex_ip = '([0-9]+)\(([0-9]+)\)'
        regex_c = '[0-9]+\([0-9]+\) Complete PBA write'
        #for s in sel_list:
            #print ui.salt_list[s]
        write_start = False
        first_start = -1
        abort = False
        for i in sel_list:
            #print 'pair ' + ui.devs_list[i] + ' ' + ui.salt_list[i]
            if os.path.isfile(ui.salt_list[i]):
                if not write_start:
                    write_start = True
                    first_start = i
        while not write_start:
            time.sleep(1)
            if not ui.pbawrite_ip:
                write_start = True
                abort = True
            else:
                for j in sel_list:
                    if os.path.isfile(ui.salt_list[j]):
                        if not write_start:
                            write_start = True
                            first_start = j
        if not abort:
            m = None
            while not m:
                time.sleep(1)
                f = open(ui.salt_list[first_start], 'r')
                txt = f.read()
                f.close()
                m = re.search(regex_ip, txt)
            ind_total = int(m.group(2))
            total = ind_total * len(sel_list)
            
            all_done = False
            done = [False] * len(sel_list)
            tracker = [0] * len(sel_list)
            while not all_done:
                curr = 0
                all_done = True
                for k in range(len(sel_list)):
                    fn = ui.salt_list[sel_list[k]]
                    
                    if os.path.isfile(fn):
                        f = open(fn, 'r')
                        txt = f.read()
                        f.close()
                        m1 = re.search(regex_ip, txt)
                        m2 = re.search(regex_c, txt)
                        if not done[k]:
                            if m2:
                                done[k] = True
                                curr = curr + ind_total
                            else:
                                if res_list[k] != -1:
                                    done[k] = True
                                else:
                                    all_done = False
                                ind_curr = tracker[k]
                                if m1:
                                    ind_curr_new = int(m1.group(1))
                                    if ind_curr < ind_curr_new:
                                        ind_curr = ind_curr_new
                                    tracker[k] = ind_curr
                                #else:
                                #    print ui.devs_list[sel_list[k]] + ui.salt_list[sel_list[k]]
                                #    print 'failed to match'
                                #    print txt
                                curr = curr + ind_curr
                        else:
                            #if m2:
                            curr = curr + ind_total
                            #else:
                            #    ind_curr = tracker[k]
                            #    if m1:
                            #        ind_curr = int(m1.group(1))
                            #        tracker[k] = ind_curr
                            #    #else:
                            #        #print txt
                            #    curr = curr + ind_curr
                    else:
                        #print ui.devs_list[sel_list[k]] + ui.salt_list[sel_list[k]]
                        #print 'file not found'
                        curr = curr + tracker[k]
                        if tracker[k] < ind_total:
                            all_done = False
                if not ui.pbawrite_ip:
                    all_done = True
                ui.progress_bar.set_fraction(curr / (total * 1.0))
                time.sleep(1)
        gobject.idle_add(cleanup_pb, sel_list)
    
    #delete files after completion
    def cleanup_pb(sel_list):
        for i in sel_list:
            fn = ui.salt_list[i]
            if os.path.isfile(fn):
                os.remove(fn)
        ui.progress_bar.hide()
    
    t_pb = threading.Thread(target=t_pb_run, args=(selected_list, result_list))
    t_pb.start()

def rt_changePW(ui, selected_list, level):
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
        list_d = re.findall(rgx, txt_s)
        for x in range(len(ui.devs_list)):
            runop.postlock(x)
            
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

    def t1_run(e, dev, index, thread_list, result_list, status_list, count, old_hash, new_hash, hash_au):
        pass_usb = ''
        noUSB = False
        noPW = False
        status_final = -1
        #dev = ui.devs_list[index]
        save_status = -1
        if old_hash == None:
            (old_hash, pass_usb) = runprocess.passReadUSB(ui.auth_menu.get_active_text(), ui.devs_list[index], ui.vendor_list[index], ui.sn_list[index])
        if old_hash == None or old_hash == 'x':
            noPW = True
        if ui.pass_sav.get_active() and pass_usb == '' and not noPW:
            
            drive = ui.drive_menu.get_active_text()
            if ui.DEV_OS == 'Windows':
                drive = drive + '\\'
            if not os.path.isdir(drive):
                noUSB = True
        runop.prelock(index)
        if not e.is_set() and not noPW:
            if old_hash != None and old_hash != 'x':
                if ui.DEV_OS == 'Windows':
                    with open(os.devnull, 'w') as pipe:
                        if ui.VERSION % 2 == 1 and level == 1:
                            status = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--setpassword', old_hash, 'User1', new_hash, dev], stdout=pipe)#stderr=log)
                            status_final = status
                            if e.is_set() and status == 0:
                                status = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--setpassword', new_hash, 'User1', old_hash, dev], stdout=pipe)#stderr=log)
                        else:
                            status1 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--setAdmin1Pwd', old_hash, new_hash, dev], stdout=pipe)#stderr=log)
                            status_final = status1
                            if e.is_set() and status == 0:
                                status1 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--setAdmin1Pwd', new_hash, old_hash, dev], stdout=pipe)#stderr=log)
                            elif status1 == 0:
                                status2 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--setSIDPassword', old_hash, new_hash, dev], stdout=pipe)#stderr=log)
                                status_final = status2
                                if e.is_set():
                                    if status2 == 0:
                                        status1 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--setAdmin1Pwd', new_hash, old_hash, dev], stdout=pipe)#stderr=log)
                                        status2 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--setSIDPassword', new_hash, old_hash, dev], stdout=pipe)#stderr=log)
                                elif status2 != 0:
                                    status1 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--setAdmin1Pwd', new_hash, old_hash, dev], stdout=pipe)#stderr=log)
                else:
                    if ui.VERSION % 2 == 1 and level == 1:
                        status = os.system(ui.prefix + 'sedutil-cli -n -t -u --setpassword "' + old_hash + '" User1 "' + new_hash + '" ' + dev)
                        #print status
                        status_final = status
                        if e.is_set() and status == 0:
                            status = os.system(ui.prefix + 'sedutil-cli -n -t -u --setpassword "' + new_hash + '" User1 "' + old_hash + '" ' + dev)
                        #elif status == 0:
                            
                    else:
                        status1 = os.system(ui.prefix + 'sedutil-cli -n -t --setAdmin1Pwd "' + old_hash + '" "' + new_hash + '" ' + dev)
                        status_final = status1
                        if e.is_set() and status == 0:
                            status1 = os.system(ui.prefix + 'sedutil-cli -n -t --setAdmin1Pwd "' + new_hash + '" "' + old_hash + '" ' + dev)
                        elif status1 == 0:
                            status2 = os.system(ui.prefix + 'sedutil-cli -n -t --setSIDPassword "' + old_hash + '" "' + new_hash + '" ' + dev)
                            status_final = status2
                            if e.is_set():
                                if status2 == 0:
                                    status2 = os.system(ui.prefix + 'sedutil-cli -n -t --setAdmin1Pwd "' + new_hash + '" "' + old_hash + '" ' + dev)
                                    status1 = os.system(ui.prefix + 'sedutil-cli -n -t --setSIDPassword "' + new_hash + '" "' + old_hash + '" ' + dev)
                            elif status2 != 0:
                                status1 = os.system(ui.prefix + 'sedutil-cli -n -t --setAdmin1Pwd "' + new_hash + '" "' + old_hash + '" ' + dev)
                            
        #print status_final
        if not e.is_set():
            if status_final == 0:
                runprocess.passSaveAppData(new_hash, ui.vendor_list[index], ui.sn_list[index], ui.auth_menu.get_active_text())
                if ui.pass_sav.get_active():
                    save_status = runprocess.passSaveUSB(new_hash, ui.drive_menu.get_active_text(), ui.vendor_list[index], ui.sn_list[index], pass_usb, ui.auth_menu.get_active_text())
                #if ui.auth_menu.get_active() == 0:
                #    if ui.pass_sav.get_active():
                #        save_status = runprocess.passSaveUSB(new_hash, ui.drive_menu.get_active_text(), ui.vendor_list[index], ui.sn_list[index], pass_usb, ui.auth_menu.get_active_text())
                #else:
                #    if ui.pass_sav.get_active():
                #        save_status = runprocess.passSaveUSB(new_hash, ui.drive_menu.get_active_text(), ui.vendor_list[index], ui.sn_list[index], pass_usb, ui.auth_menu.get_active_text())
            def aw_run(dev, i, status_final, save_status, auth, new_hash):
                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                timeStr = timeStr[2:]
                if ui.DEV_OS == 'Windows':
                    with open(os.devnull, 'w') as pipe:
                        if status_final != 0:
                            if auth == 0:
                                if status_final == ui.NOT_AUTHORIZED and ui.admin_aol_list[i] < ui.retrylimit_list[i]:
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '04' + timeStr, hash_au, 'User' + ui.user_list[i], dev], stdout=pipe)#stderr=log)
                                elif status_final == ui.AUTHORITY_LOCKED_OUT or ui.admin_aol_list[i] >= ui.retrylimit_list[i]:
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '05' + timeStr, hash_au, 'User' + ui.user_list[i], dev], stdout=pipe)#stderr=log)
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '09' + timeStr, hash_au, 'User' + ui.user_list[i], dev], stdout=pipe)#stderr=log)
                                subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '31' + timeStr, hash_au, 'User' + ui.user_list[i], dev], stdout=pipe)#stderr=log)
                            else:
                                if status_final == ui.NOT_AUTHORIZED and ui.user_aol_list[i] < ui.retrylimit_list[i]:
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '07' + timeStr, hash_au, 'User' + ui.user_list[i], dev], stdout=pipe)#stderr=log)
                                elif status_final == ui.AUTHORITY_LOCKED_OUT or ui.user_aol_list[i] >= ui.retrylimit_list[i]:
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '08' + timeStr, hash_au, 'User' + ui.user_list[i], dev], stdout=pipe)#stderr=log)
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '09' + timeStr, hash_au, 'User' + ui.user_list[i], dev], stdout=pipe)#stderr=log)
                                subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '32' + timeStr, hash_au, 'User' + ui.user_list[i], dev], stdout=pipe)#stderr=log)
                        else:
                            if auth == 0:
                                if ui.check_pass_rd.get_active():
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '25' + timeStr, new_hash, 'Admin1', dev], stdout=pipe)#stderr=log)
                                statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '03' + timeStr, new_hash, 'Admin1', dev], stdout=pipe)#stderr=log)
                                statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '13' + timeStr, new_hash, 'Admin1', dev], stdout=pipe)#stderr=log)
                                if save_status == 0:
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '23' + timeStr, new_hash, 'Admin1', dev], stdout=pipe)#stderr=log)
                            else:
                                if ui.check_pass_rd.get_active():
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '26' + timeStr, new_hash, 'User1', dev], stdout=pipe)#stderr=log)
                                statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '06' + timeStr, new_hash, 'User1', dev], stdout=pipe)#stderr=log)
                                statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '14' + timeStr, new_hash, 'User1', dev], stdout=pipe)#stderr=log)
                                if save_status == 0:
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '24' + timeStr, new_hash, 'User1', dev], stdout=pipe)#stderr=log)
                else:
                    if status_final != 0:
                        if auth == 0:
                            if status_final == ui.NOT_AUTHORIZED and ui.admin_aol_list[i] < ui.retrylimit_list[i]:
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 04' + timeStr + ' "' + hash_au + '" User' + ui.user_list[i] + ' ' + dev)
                            elif status_final == ui.AUTHORITY_LOCKED_OUT or ui.admin_aol_list[i] >= ui.retrylimit_list[i]:
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 05' + timeStr + ' "' + hash_au + '" User' + ui.user_list[i] + ' ' + dev)
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 09' + timeStr + ' "' + hash_au + '" User' + ui.user_list[i] + ' ' + dev)
                            os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 31' + timeStr + ' "' + hash_au + '" User' + ui.user_list[i] + ' ' + dev)
                        else:
                            if status_final == ui.NOT_AUTHORIZED and ui.user_aol_list[i] < ui.retrylimit_list[i]:
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 07' + timeStr + ' "' + hash_au + '" User' + ui.user_list[i] + ' ' + dev)
                            elif status_final == ui.AUTHORITY_LOCKED_OUT or ui.user_aol_list[i] >= ui.retrylimit_list[i]:
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 08' + timeStr + ' "' + hash_au + '" User' + ui.user_list[i] + ' ' + dev)
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 09' + timeStr + ' "' + hash_au + '" User' + ui.user_list[i] + ' ' + dev)
                            os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 32' + timeStr + ' "' + hash_au + '" User' + ui.user_list[i] + ' ' + dev)
                    else:
                        if auth == 0:
                            if ui.check_pass_rd.get_active():
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 25' + timeStr + ' "' + new_hash + '" Admin1 ' + dev)
                            statusAW = os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 03' + timeStr + ' "' + new_hash + '" Admin1 ' + dev)
                            statusAW = os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 13' + timeStr + ' "' + new_hash + '" Admin1 ' + dev)
                            if save_status == 0:
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 23' + timeStr + ' "' + new_hash + '" Admin1 ' + dev)
                        else:
                            if ui.check_pass_rd.get_active():
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 26' + timeStr + ' "' + new_hash + '" User1 ' + dev)
                            statusAW = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 06' + timeStr + ' "' + new_hash + '" User1 ' + dev)
                            statusAW = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 14' + timeStr + ' "' + new_hash + '" User1 ' + dev)
                            if save_status == 0:
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 24' + timeStr + ' "' + new_hash + '" User1 ' + dev)
                runop.postlock(i)
            if ui.datastore_list[index] == 'Supported':
                t_aw = threading.Thread(target=aw_run, args=(dev, index, status_final, save_status, ui.auth_menu.get_active(), new_hash))
                t_aw.start()
                time.sleep(0.5)
            else:
                runop.postlock(index)
        gobject.idle_add(cleanup1, dev, index, status_final, thread_list, result_list, status_list, count, noUSB, e, new_hash, pass_usb, save_status, hash_au)
        
    def cleanup1(dev, i, status, thread_list, result_list, status_list, count, no_usb, e, new_hash, pass_usb, save_status, hash_au):
        dialog_str = ''
        if e.is_set():
            result_list[count] = 2
        elif status != 0:
            if status == ui.NOT_AUTHORIZED or status == ui.AUTHORITY_LOCKED_OUT:
                result_list[count] = 1
            elif status == ui.SP_BUSY:
                result_list[count] = 4
            else:
                result_list[count] = 3
            
            if ui.auth_menu.get_active() == 0:
                if status == ui.NOT_AUTHORIZED and ui.admin_aol_list[i] < ui.retrylimit_list[i]:
                    ui.admin_aol_list[i] = ui.admin_aol_list[i] + 1
                elif status == ui.AUTHORITY_LOCKED_OUT or ui.admin_aol_list[i] >= ui.retrylimit_list[i]:
                    ui.admin_aol_list[i] = ui.retrylimit_list[i] + 1
            else:
                if status == ui.NOT_AUTHORIZED and ui.admin_aol_list[i] < ui.retrylimit_list[i]:
                    ui.user_aol_list[i] = ui.user_aol_list[i] + 1
                elif status == ui.AUTHORITY_LOCKED_OUT or ui.admin_aol_list[i] >= ui.retrylimit_list[i]:
                    ui.user_aol_list[i] = ui.retrylimit_list[i] + 1
            
            
        else:
            
            if ui.auth_menu.get_active() == 0:
                ui.admin_aol_list[i] = 0
            else:
                ui.user_aol_list[i] = 0
            
            if save_status <= 0:
                result_list[count] = 0
            else:
                result_list[count] = 5
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
            old_hash = None
            if ui.check_pass_rd.get_active():
                old_hash = None
                #old_hash = runprocess.passReadUSB(ui, ui.vendor_list[i], ui.sn_list[i], ui.auth_menu.get_active())
            else:
                old_pass = ui.pass_entry.get_text()
                old_trim = re.sub('\s', '', old_pass)
                old_hash = lockhash.hash_pass(old_trim, i, ui)#ui.salt_list[i], ui.msid_list[i])
            pw_new_trim = re.sub('\s','',ui.new_pass_entry.get_text())
            new_hash = lockhash.hash_pass(pw_new_trim, i, ui)#ui.salt_list[i], ui.msid_list[i])
            #au = '"' + lockhash.get_val() + ui.salt_list[i] + '"'
            au = lockhash.get_val() + ui.salt_list[i]
            hash_au = lockhash.hash_pass(au, i, ui)#ui.salt_list[i], ui.msid_list[i])
            proc_list[count] = threading.Thread(target=t1_run, args=(e_to, actual_d[count], i, proc_list, result_list, status_list, count, old_hash, new_hash, hash_au))
            proc_list[count].start()
        count = count + 1
        
    t2 = threading.Thread(target=runthread.timeout_track, args=(ui, 60.0*len(selected_list), start_time, proc_list, result_list, e_to, selected_list, status_list, cleanop.changePW_cleanup, rescan_needed, -1, -1, -1))
    t2.start()

def rt_setupUSB(ui, index, index2, e, preserved_files):
    pass_usb = ''
    noPW = False
    wrongPW = False
    hash_pwd = ''
    
    s = -1
    
    dev1 = ui.devs_list[index]
    
    new_devname = None

    rescan_needed = False
    present = True
            
    if ui.tcg_usb_list[index]:
        matched = False
        present = False
        rgx = '(PhysicalDrive[0-9]+|/dev/sd[a-z][0-9]?|/dev/nvme[0-9])\s+(?:[12ELP]+|No)\s+([^\:\s]+(?:\s[^\:\s]+)*)\s*:\s*[^:]+\s*:\s*(\S+)'
        for x in range(len(ui.devs_list)):
            runop.prelock(x)
        txt_s = os.popen(ui.prefix + 'sedutil-cli --scan n').read()
        for x in range(len(ui.devs_list)):
            runop.postlock(x)
        list_d = re.findall(rgx, txt_s)
        for entry in list_d:
            if not present or not matched:
                test_dev = entry[0]
                if ui.DEV_OS == 'Windows':
                    test_dev = '\\\\.\\' + test_dev
                if test_dev == dev1 and entry[1] == ui.vendor_list[index] and entry[2] == ui.sn_list[index]:
                    present = True
                    matched = True
                elif entry[1] == ui.vendor_list[index] and entry[2] == ui.sn_list[index]:
                    present = True
                    new_devname = test_dev
        if present and not matched:
            dev1 = new_devname
        if not present or not matched:
            rescan_needed = True
            
    if present:
        runop.prelock(index)
        if ui.pass_sav.get_active():
            if ui.check_pass_rd.get_active():
                (hash_pwd, pass_usb) = runprocess.passReadUSB(ui.auth_menu.get_active_text(), dev1, ui.vendor_list[index], ui.sn_list[index])
                if hash_pwd == None or hash_pwd == 'x':
                    noPW = True
                pass_usb = ''
            else:
                pw_trim = re.sub('\s', '', ui.pass_entry.get_text())
                hash_pwd = lockhash.hash_pass(pw_trim, index, ui)#ui.salt_list[index], ui.dev_msid.get_text())
            if not noPW:
                if ui.DEV_OS == 'Windows':
                    with open(os.devnull, 'w') as pipe:
                        if ui.auth_menu.get_active() == 0:
                            s = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--getmbrsize', hash_pwd, dev1], stdout=pipe)#stderr=log)
                        else:
                            s = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--setpassword', hash_pwd, 'User1', hash_pwd, dev1], stdout=pipe)#stderr=log)
                        timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                        timeStr = timeStr[2:]
                        if s != 0:
                            wrongPW = True
                            au = lockhash.get_val() + ui.salt_list[index]
                            hash_au = lockhash.hash_pass(au, index, ui)
                            if ui.auth_menu.get_active() == 0:
                                if s == ui.NOT_AUTHORIZED and ui.admin_aol_list[index] < ui.retrylimit_list[index]:
                                    subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '04' + timeStr, hash_au, 'User' + ui.user_list[index], dev1], stdout=pipe)#stderr=log)
                                elif s == ui.AUTHORITY_LOCKED_OUT or ui.admin_aol_list[index] >= ui.retrylimit_list[index]:
                                    subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '05' + timeStr, hash_au, 'User' + ui.user_list[index], dev1], stdout=pipe)#stderr=log)
                                    subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '09' + timeStr, hash_au, 'User' + ui.user_list[index], dev1], stdout=pipe)#stderr=log)
                            else:
                                if s == ui.NOT_AUTHORIZED and ui.user_aol_list[index] < ui.retrylimit_list[index]:
                                    subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '07' + timeStr, hash_au, 'User' + ui.user_list[index], dev1], stdout=pipe)#stderr=log)
                                elif s == ui.AUTHORITY_LOCKED_OUT or ui.user_aol_list[index] >= ui.retrylimit_list[index]:
                                    subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '08' + timeStr, hash_au, 'User' + ui.user_list[index], dev1], stdout=pipe)#stderr=log)
                                    subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '09' + timeStr, hash_au, 'User' + ui.user_list[index], dev1], stdout=pipe)#stderr=log)
                        elif ui.auth_menu.get_active() == 1:
                            if ui.check_pass_rd.get_active():
                                subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '26' + timeStr, hash_pwd, 'User1', dev1], stdout=pipe)#stderr=log)
                            subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '06' + timeStr, hash_pwd, 'User1', dev1], stdout=pipe)#stderr=log)
                        else:
                            if ui.check_pass_rd.get_active():
                                subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '25' + timeStr, hash_pwd, 'Admin1', dev1], stdout=pipe)#stderr=log)
                            subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '03' + timeStr, hash_pwd, 'Admin1', dev1], stdout=pipe)#stderr=log)
                else:
                    if ui.auth_menu.get_active() == 0:
                        s = os.system(ui.prefix + 'sedutil-cli -n -t --getmbrsize "' + hash_pwd + '" ' + dev1)
                    else:
                        s = os.system(ui.prefix + 'sedutil-cli -n -t -u --setpassword "' + hash_pwd + '" User1 ' + hash_pwd + ' ' + dev1)
                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                    timeStr = timeStr[2:]
                    if s != 0:
                        wrongPW = True
                        au = lockhash.get_val() + ui.salt_list[index]
                        hash_au = lockhash.hash_pass(au, index, ui)
                        if ui.auth_menu.get_active() == 0:
                            if s == ui.NOT_AUTHORIZED and ui.admin_aol_list[index] < ui.retrylimit_list[index]:
                                os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 04' + timeStr + ' "' + hash_au + '" User' + ui.user_list[index] + ' ' + dev1)
                            elif s == ui.AUTHORITY_LOCKED_OUT or ui.admin_aol_list[index] >= ui.retrylimit_list[index]:
                                os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 05' + timeStr + ' "' + hash_au + '" User' + ui.user_list[index] + ' ' + dev1)
                                os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 09' + timeStr + ' "' + hash_au + '" User' + ui.user_list[index] + ' ' + dev1)
                        else:
                            if s == ui.NOT_AUTHORIZED and ui.user_aol_list[index] < ui.retrylimit_list[index]:
                                os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 07' + timeStr + ' "' + hash_au + '" User' + ui.user_list[index] + ' ' + dev1)
                            elif s == ui.AUTHORITY_LOCKED_OUT or ui.user_aol_list[index] >= ui.retrylimit_list[index]:
                                os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 08' + timeStr + ' "' + hash_au + '" User' + ui.user_list[index] + ' ' + dev1)
                                os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 09' + timeStr + ' "' + hash_au + '" User' + ui.user_list[index] + ' ' + dev1)
                    elif ui.auth_menu.get_active() == 1:
                        if ui.check_pass_rd.get_active():
                            os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 26' + timeStr + ' "' + hash_pwd + '" User1 ' + dev1)
                        os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 06' + timeStr + ' "' + hash_pwd + '" User1 ' + dev1)
                    else:
                        if ui.check_pass_rd.get_active():
                            os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 25' + timeStr + ' "' + hash_pwd + '" Admin1 ' + dev1)
                        os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 03' + timeStr + ' "' + hash_pwd + '" Admin1 ' + dev1)
                    
                    
        if not noPW and not wrongPW and not e.is_set():
            if ui.DEV_OS == 'Windows':
                ui.process =  subprocess.Popen([ui.prefix + "sedutil-cli", "--createUSB", "UEFI", dev1, "\\\\.\\PhysicalDrive" + ui.usb_list[index2][1]])
                ui.process.communicate()
                if ui.process != None:
                    status1 = ui.process.returncode
                    if status1 == 0:
                        #if (ui.VERSION % 3 == 0 or (ui.VERSION == 1 and ui.PBA_VERSION != 1)):
                            ##print "setupUSB runprocess.passSaveUSB " + hash_pwd
                            #Windows usb_list: disk number, disk name
                            #search through drive letters to find the matching disk number
                        if ui.DEV_OS == 'Windows':
                            usb_drive = ''
                            #print 'before diskpart to save password to USB'
                            for j in range(len(ui.devs_list)):
                                if j != index:
                                    runop.prelock(j)
                            p = subprocess.Popen(["diskpart"], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
                            res1 = p.stdin.write('select disk ' + ui.usb_list[index2][1] + '\n')
                            res1 = p.stdin.write('detail disk\n')
                            res1 = p.stdin.write('exit\n')
                            output = p.communicate()[0]
                            for j in range(len(ui.devs_list)):
                                if j != index:
                                    runop.postlock(j)
                            disk_regex = 'Volume [0-9]+\s+([A-Z])\s+'
                            m = re.search(disk_regex, output)
                            if m:
                                usb_drive = m.group(1) + ':'
                            if usb_drive != '':
                                if len(preserved_files) > 0:
                                    os.makedirs('%s/OpalLock' % usb_drive)
                                for fp in preserved_files:
                                    try:
                                        f = open(fp[0], 'w')
                                        f.write(fp[1])
                                        f.close()
                                    except IOError:
                                        pass
                                runprocess.passSaveAppData(hash_pwd, ui.dev_vendor.get_text(), ui.dev_sn.get_text(), ui.auth_menu.get_active_text())
                                if ui.pass_sav.get_active():
                                    save_status = runprocess.passSaveUSB(hash_pwd, usb_drive, ui.dev_vendor.get_text(), ui.dev_sn.get_text(), pass_usb, ui.auth_menu.get_active_text())
                                    if save_status == 0:
                                        timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                                        timeStr = timeStr[2:]
                                        with open(os.devnull, 'w') as pipe:
                                            if ui.auth_menu.get_active() == 1:
                                                subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '24' + timeStr, hash_pwd, 'User1', dev1], stdout=pipe)#stderr=log)
                                            else:
                                                subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '23' + timeStr, hash_pwd, 'Admin1', dev1], stdout=pipe)#stderr=log)
                                #else:
                                #    ui.msg_err('Partition could not be found to save password.')
                    gobject.idle_add(cleanop.setupUSB_cleanup, ui, index, status1, noPW, s, e, preserved_files, present, rescan_needed)
                else:
                    gobject.idle_add(cleanop.setupUSB_cleanup, ui, index, -1, noPW, s, e, preserved_files, present, rescan_needed)
            elif ui.DEV_OS == 'Linux':
                ui.process =  subprocess.Popen([ui.prefix + "sedutil-cli", "--createUSB", "UEFI", dev1, ui.usb_list[index2][0]])
                ui.process.communicate()
                if ui.process != None:
                    status1 = ui.process.returncode
                    if status1 == 0:
                        save_status = runprocess.passSaveUSB(hash_pwd, ui.usb_list[index2][1], ui.dev_vendor.get_text(), ui.dev_sn.get_text(), pass_usb, ui.auth_menu.get_active_text())
                        if save_status == 0:
                            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                            timeStr = timeStr[2:]
                            if ui.auth_menu.get_active() == 1:
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 24' + timeStr + ' "' + hash_pwd + '" User1 ' + dev1)
                            else:
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 23' + timeStr + ' "' + hash_pwd + '" Admin1 ' + dev1)
                    gobject.idle_add(cleanop.setupUSB_cleanup, ui, index, status1, noPW, s, e, preserved_files, present, rescan_needed)
                else:
                    gobject.idle_add(cleanop.setupUSB_cleanup, ui, index, -1, noPW, s, e, preserved_files, present, rescan_needed)
        else:
            gobject.idle_add(cleanop.setupUSB_cleanup, ui, index, -1, noPW, s, e, preserved_files, present, rescan_needed)
        runop.postlock(index)
    else:
        gobject.idle_add(cleanop.setupUSB_cleanup, ui, index, -1, noPW, s, e, preserved_files, present, rescan_needed)

def rt_setupUser(ui, index, e):
    pass_usb = ''
    password_a = ''
    notify_TO = False
    noPW = False
    noUSB = False
    
    new_devname = None
    
    save_status = -1
            
    rescan_needed = False
    present = True
            
    if ui.tcg_usb_list[index]:
        matched = False
        present = False
        for x in range(len(ui.devs_list)):
            runop.prelock(x)
        txt_s = os.popen(ui.prefix + 'sedutil-cli --scan n').read()
        rgx = '(PhysicalDrive[0-9]+|/dev/sd[a-z][0-9]?|/dev/nvme[0-9])\s+(?:[12ELP]+|No)\s+([^\:\s]+(?:\s[^\:\s]+)*)\s*:\s*[^:]+\s*:\s*(\S+)'
        list_d = re.findall(rgx, txt_s)
        for x in range(len(ui.devs_list)):
            runop.postlock(x)
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
        if ui.check_pass_rd.get_active():
            ui.auth_menu.set_active(0)
            (password_a, pass_usb) = runprocess.passReadUSB("Admin", ui.devs_list[index], ui.dev_vendor.get_text(), ui.dev_sn.get_text())
            if password_a == None or password_a == 'x':
                noPW = True
            pass_usb = ''
        else:
            pw_trim = re.sub('\s', '', ui.pass_entry.get_text())
            password_a = lockhash.hash_pass(pw_trim, index, ui)#ui.salt_list[index], ui.dev_msid.get_text())
        new_pw = ui.new_pass_entry.get_text()
        new_pw_trim = re.sub('\s', '', new_pw)
        password_u = lockhash.hash_pass(new_pw_trim, index, ui)#ui.salt_list[index], ui.dev_msid.get_text())
        if ui.pass_sav.get_active():
            drive = ''
            if pass_usb != '':
                #print ui.pass_usb
                drive = pass_usb
            else:
                drive = ui.drive_menu.get_active_text()
                
                if ui.DEV_OS == 'Windows':
                    drive = drive + '\\'
            if not os.path.isdir(drive):
                
                noUSB = True
        if e.is_set():
            notify_TO = True
            gobject.idle_add(cleanop.setupUser_cleanup, ui, index, -1, notify_TO, noPW, noUSB, password_a, password_u, present, rescan_needed, save_status)
        elif not noPW and not noUSB:
            
            if e.is_set():
                notify_TO = True
                gobject.idle_add(cleanop.setupUser_cleanup, ui, index, -1, notify_TO, noPW, noUSB, password_a, password_u, present, rescan_needed, save_status)
            else:
                runop.prelock(index)
                if ui.DEV_OS == 'Windows':
                    with open(os.devnull, 'w') as pipe:
                        s3 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--setpassword', password_a, 'User1', password_u, ui.devname], stdout=pipe)#stderr=log)
                        if s3 == 0:
                            ui.auth_menu.set_active(1)
                            runprocess.passSaveAppData(password_u, ui.dev_vendor.get_text(), ui.dev_sn.get_text(), ui.auth_menu.get_active_text())
                            if ui.pass_sav.get_active():
                                save_status = runprocess.passSaveUSB(password_u, ui.drive_menu.get_active_text(), ui.dev_vendor.get_text(), ui.dev_sn.get_text(), pass_usb, ui.auth_menu.get_active_text())
                else:
                    s3 = os.system(ui.prefix + 'sedutil-cli -n -t --setpassword "' + password_a + '" User1 "' + password_u + '" ' + ui.devname)
                    if s3 == 0:
                        if ui.pass_sav.get_active():
                            ui.auth_menu.set_active(1)
                            save_status = runprocess.passSaveUSB(password_u, ui.drive_menu.get_active_text(), ui.dev_vendor.get_text(), ui.dev_sn.get_text(), pass_usb, ui.auth_menu.get_active_text())
                def aw_run(status, save_status, index, hash_a, dev):
                    pwd = lockhash.get_val() + ui.salt_list[index]
                    hash_pwd = lockhash.hash_pass(pwd, index, ui)
                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                    timeStr = timeStr[2:]
                    if ui.DEV_OS == 'Windows':
                        with open(os.devnull, 'w') as pipe:
                            if status != 0:
                                if status == ui.NOT_AUTHORIZED and ui.admin_aol_list[index] < ui.retrylimit_list[index]:
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '04' + timeStr, hash_pwd, 'User' + ui.user_list[index], dev], stdout=pipe)#stderr=log)
                                elif status == ui.AUTHORITY_LOCKED_OUT or ui.admin_aol_list[index] >= ui.retrylimit_list[index]:
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '05' + timeStr, hash_pwd, 'User' + ui.user_list[index], dev], stdout=pipe)#stderr=log)
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '09' + timeStr, hash_pwd, 'User' + ui.user_list[index], dev], stdout=pipe)#stderr=log)
                                subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '29' + timeStr, hash_pwd, 'User' + ui.user_list[index], dev], stdout=pipe)#stderr=log)
                            else:
                                if ui.check_pass_rd.get_active():
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '25' + timeStr, hash_a, 'Admin1', dev], stdout=pipe)#stderr=log)
                                statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '03' + timeStr, hash_a, 'Admin1', dev], stdout=pipe)#stderr=log)
                                statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '11' + timeStr, hash_a, 'Admin1', dev], stdout=pipe)#stderr=log)
                                statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '14' + timeStr, hash_a, 'Admin1', dev], stdout=pipe)#stderr=log)
                                if save_status == 0:
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '24' + timeStr, hash_a, 'Admin1', dev], stdout=pipe)#stderr=log)
                                s1 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--datastoreread', hash_a, 'Admin1', 'dataread.txt', '0', '130000', '130', dev], stdout=pipe)#stderr=log)
                                f1 = open('dataread.txt', 'r')
                                f1_txt = f1.read()
                                f1.close()
                                try:
                                    os.remove('dataread.txt')
                                except WindowsError:
                                    print 'Error: dataread.txt is somehow not closed'
                                hdr = 'OpalLock Datastore Info'
                                m1 = re.search(hdr, f1_txt)
                                if m1:
                                    new_txt = f1_txt[:32] + '2' + f1_txt[33:]
                                    f2 = open('datawrite.txt','w+')
                                    f2.write(new_txt)
                                    f2.close()
                                    s2 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--datastorewrite', hash_a, 'Admin1', 'datawrite.txt', '0', '130000', '130', dev], stdout=pipe)#stderr=log)
                                    try:
                                        os.remove('datawrite.txt')
                                    except WindowsError:
                                        print 'Error: datawrite.txt is somehow not closed'
                    else:
                        if status != 0:
                            if status == ui.NOT_AUTHORIZED and ui.admin_aol_list[index] < ui.retrylimit_list[index]:
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 04' + timeStr + ' "' + hash_pwd + '" User' + ui.user_list[index] + ' ' + dev)
                            elif status == ui.AUTHORITY_LOCKED_OUT or ui.admin_aol_list[index] >= ui.retrylimit_list[index]:
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 05' + timeStr + ' "' + hash_pwd + '" User' + ui.user_list[index] + ' ' + dev)
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 09' + timeStr + ' "' + hash_pwd + '" User' + ui.user_list[index] + ' ' + dev)
                            os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 29' + timeStr + ' "' + hash_pwd + '" User' + ui.user_list[index] + ' ' + dev)
                        else:
                            if ui.check_pass_rd.get_active():
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 25' + timeStr + ' "' + hash_a + '" Admin1 ' + dev)
                            statusAW = os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 03' + timeStr + ' "' + hash_a + '" Admin1 ' + dev)
                            statusAW1 = os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 11' + timeStr + ' "' + hash_a + '" Admin1 ' + dev)
                            statusAW1 = os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 14' + timeStr + ' "' + hash_a + '" Admin1 ' + dev)
                            if save_status == 0:
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 24' + timeStr + ' "' + hash_a + '" Admin1 ' + dev)
                            s1 = os.system(ui.prefix + 'sedutil-cli -n -t --datastoreread "' + hash_a + '" Admin1 dataread.txt 0 130000 130 ' + dev)
                            f1 = open('dataread.txt', 'r')
                            f1_txt = f1.read()
                            f1.close()
                            try:
                                os.remove('dataread.txt')
                            except WindowsError:
                                print 'Error: dataread.txt is somehow not closed'
                            hdr = 'OpalLock Datastore Info'
                            m1 = re.search(hdr, f1_txt)
                            if m1:
                                new_txt = f1_txt[:32] + '2' + f1_txt[33:]
                                f2 = open('datawrite.txt','w+')
                                f2.write(new_txt)
                                f2.close()
                                s2 = os.system(ui.prefix + 'sedutil-cli -n -t --datastorewrite "' + hash_a + '" Admin1 datawrite.txt 0 130000 130 ' + dev)
                                try:
                                    os.remove('datawrite.txt')
                                except WindowsError:
                                    print 'Error: datawrite.txt is somehow not closed'
                    runop.postlock(index)
                if ui.datastore_list[index] == 'Supported':
                    t_aw = threading.Thread(target=aw_run, args=(s3, save_status, index, password_a, ui.devname))
                    t_aw.start()
                    time.sleep(0.5)
                else:
                    runop.postlock(index)
                gobject.idle_add(cleanop.setupUser_cleanup, ui, index, s3, notify_TO, noPW, noUSB, password_a, password_u, present, rescan_needed, save_status)
        else:
            gobject.idle_add(cleanop.setupUser_cleanup, ui, index, -1, notify_TO, noPW, noUSB, password_a, password_u, present, rescan_needed, save_status)
    else:
        gobject.idle_add(cleanop.setupUser_cleanup, ui, index, -1, notify_TO, noPW, noUSB, password_a, password_u, present, rescan_needed, save_status)

def rt_removeUser(ui, index, e):
    pass_usb = ''
    no_pw = False
    
    new_devname = None
    
    save_status = -1
            
    rescan_needed = False
    present = True
            
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
        if ui.check_pass_rd.get_active():
            ui.auth_menu.set_active(0)
            (password_a, pass_usb) = runprocess.passReadUSB('Admin', ui.devs_list[index], ui.dev_vendor.get_text(), ui.dev_sn.get_text())
            if password_a == None or password_a == 'x':
                no_pw = True
        else:
            pw_trim = re.sub('\s', '', ui.pass_entry.get_text())
            password_a = lockhash.hash_pass(pw_trim, index, ui)#ui.salt_list[index], ui.dev_msid.get_text())
        rc = -1
        notify_TO = False
        if e.is_set():
            notify_TO = True
        
        elif not no_pw:
            #f1 = os.popen(ui.prefix + "sedutil-cli -n -t --getmbrsize " + password_a + " " + ui.devname)
            #f_txt = f1.read()
            #rc = f1.close()
            #userRegex = 'User1 enabled\s*=\s*1'
            #noRegex = 'User1 enabled\s*=\s*0'
            #m = re.search(userRegex,f_txt)
            #m_no = re.search(noRegex,f_txt)
            if e.is_set():
                notify_TO = True
                rc = -1
            #elif m:
            else:
                runop.prelock(index)
                if ui.DEV_OS == 'Windows':
                    with open(os.devnull, 'w') as pipe:
                        rc = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--setpassword', password_a, 'User1', 'USER1', ui.devname], stdout=pipe)#stderr=log)
                        if rc == 0:
                            runprocess.removeUserUSB(ui.prefix, ui.dev_vendor.get_text(), ui.dev_sn.get_text())
                            ui.auth_menu.set_active(0)
                            runprocess.passSaveAppData(password_a, ui.dev_vendor.get_text(), ui.dev_sn.get_text(), ui.auth_menu.get_active_text())
                            if ui.pass_sav.get_active():
                                save_status = runprocess.passSaveUSB(password_a, ui.drive_menu.get_active_text(), ui.dev_vendor.get_text(), ui.dev_sn.get_text(), pass_usb, ui.auth_menu.get_active_text())
                else:
                    rc = os.system(ui.prefix + 'sedutil-cli -n -t --setpassword "' + password_a + '" User1 USER1 ' + ui.devname)
                    if rc == 0:
                        runprocess.removeUserUSB(ui.prefix, ui.dev_vendor.get_text(), ui.dev_sn.get_text())
                        if ui.pass_sav.get_active():
                            ui.auth_menu.set_active(0)
                            save_status = runprocess.passSaveUSB(password_a, ui.drive_menu.get_active_text(), ui.dev_vendor.get_text(), ui.dev_sn.get_text(), pass_usb, ui.auth_menu.get_active_text())
                def aw_run(status, save_status, index, hash_a, dev):
                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                    timeStr = timeStr[2:]
                    pwd = lockhash.get_val() + ui.salt_list[index]
                    hash_pwd = lockhash.hash_pass(pwd, index, ui)
                    if ui.DEV_OS == 'Windows':
                        with open(os.devnull, 'w') as pipe:
                            if status != 0:
                                if status == ui.NOT_AUTHORIZED and ui.admin_aol_list[index] < ui.retrylimit_list[index]:
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '04' + timeStr, hash_pwd, 'User' + ui.user_list[index], dev], stdout=pipe)#stderr=log)
                                elif status == ui.AUTHORITY_LOCKED_OUT or ui.admin_aol_list[index] >= ui.retrylimit_list[index]:
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '05' + timeStr, hash_pwd, 'User' + ui.user_list[index], dev], stdout=pipe)#stderr=log)
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '09' + timeStr, hash_pwd, 'User' + ui.user_list[index], dev], stdout=pipe)#stderr=log)
                                subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '-u', '--auditwrite', '30' + timeStr, hash_pwd, 'User' + ui.user_list[index], dev], stdout=pipe)#stderr=log)
                            else:
                                if ui.check_pass_rd.get_active():
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '25' + timeStr, hash_a, 'Admin1', dev], stdout=pipe)#stderr=log)
                                statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '03' + timeStr, hash_a, 'Admin1', dev], stdout=pipe)#stderr=log)
                                statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '12' + timeStr, hash_a, 'Admin1', dev], stdout=pipe)#stderr=log)
                                if save_status == 0:
                                    statusAW = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--auditwrite', '23' + timeStr, hash_a, 'Admin1', dev], stdout=pipe)#stderr=log)
                                s1 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--datastoreread', hash_a, 'Admin1', 'dataread.txt', '0', '130000', '130', dev], stdout=pipe)#stderr=log)
                                f1 = open('dataread.txt', 'r')
                                f1_txt = f1.read()
                                f1.close()
                                try:
                                    os.remove('dataread.txt')
                                except WindowsError:
                                    print 'Error: dataread.txt is somehow not closed'
                                hdr = 'OpalLock Datastore Info'
                                m1 = re.search(hdr, f1_txt)
                                if m1:
                                    new_txt = f1_txt[:32] + '1' + f1_txt[33:]
                                    f2 = open('datawrite.txt','w+')
                                    f2.write(new_txt)
                                    f2.close()
                                    s2 = subprocess.call([ui.prefix + 'sedutil-cli', '-n', '-t', '--datastorewrite', hash_a, 'Admin1', 'datawrite.txt', '0', '130000', '130', dev], stdout=pipe)#stderr=log)
                                    try:
                                        os.remove('datawrite.txt')
                                    except WindowsError:
                                        print 'Error: datawrite.txt is somehow not closed'
                    else:
                        if status != 0:
                            if status == ui.NOT_AUTHORIZED and ui.admin_aol_list[index] < ui.retrylimit_list[index]:
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 04' + timeStr + ' "' + hash_pwd + '" User' + ui.user_list[index] + ' ' + dev)
                            elif status == ui.AUTHORITY_LOCKED_OUT or ui.admin_aol_list[index] >= ui.retrylimit_list[index]:
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 05' + timeStr + ' "' + hash_pwd + '" User' + ui.user_list[index] + ' ' + dev)
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 09' + timeStr + ' "' + hash_pwd + '" User' + ui.user_list[index] + ' ' + dev)
                            os.system(ui.prefix + 'sedutil-cli -n -t -u --auditwrite 30' + timeStr + ' "' + hash_pwd + '" User' + ui.user_list[index] + ' ' + dev)
                        else:
                            if ui.check_pass_rd.get_active():
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 25' + timeStr + ' "' + hash_a + '" Admin1 ' + dev)
                            statusAW = os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 03' + timeStr + ' "' + hash_a + '" Admin1 ' + dev)
                            statusAW1 = os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 12' + timeStr + ' "' + hash_a + '" Admin1 ' + dev)
                            if save_status == 0:
                                statusAW = os.system(ui.prefix + 'sedutil-cli -n -t --auditwrite 23' + timeStr + ' "' + hash_a + '" Admin1 ' + dev)
                            s1 = os.system(ui.prefix + 'sedutil-cli -n -t --datastoreread "' + hash_a + '" Admin1 dataread.txt 0 130000 130 ' + dev)
                            f1 = open('dataread.txt', 'r')
                            f1_txt = f1.read()
                            f1.close()
                            try:
                                os.remove('dataread.txt')
                            except WindowsError:
                                print 'Error: dataread.txt is somehow not closed'
                            hdr = 'OpalLock Datastore Info'
                            m1 = re.search(hdr, f1_txt)
                            if m1:
                                new_txt = f1_txt[:32] + '1' + f1_txt[33:]
                                f2 = open('datawrite.txt','w+')
                                f2.write(new_txt)
                                f2.close()
                                s2 = os.system(ui.prefix + 'sedutil-cli -n -t --datastorewrite "' + hash_a + '" Admin1 datawrite.txt 0 130000 130 ' + dev)
                                try:
                                    os.remove('datawrite.txt')
                                except WindowsError:
                                    print 'Error: datawrite.txt is somehow not closed'
                    runop.postlock(index)
                if ui.datastore_list[index] == 'Supported':
                    t_aw = threading.Thread(target=aw_run, args=(rc, save_status, index, password_a, ui.devname))
                    t_aw.start()
                    time.sleep(0.5)
                else:
                    runop.postlock(index)
        #elif m_no:
        #    rc = None
    gobject.idle_add(cleanop.removeUser_cleanup, ui, index, rc, notify_TO, no_pw, password_a, pass_usb, present, rescan_needed, save_status)


