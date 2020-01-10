import background
import dialogs
import os
import platform
import runop
import runscan
import subprocess
import time

def timeout_cleanup(ui, max_time, start_time, op_procs, res_list, e, selected_list, status_list, val, val2, val3, val4):
    curr_time = time.time()
    elapsed_time = curr_time - start_time
    if elapsed_time >= max_time:
        e.set()
        for op_proc in op_procs:
            if op_proc != None and op_proc.is_alive():
                if ui.process != None:
                    ui.process.terminate()
                    ui.process = None
                elif ui.process_list != []:
                    for p in ui.process_list:
                        if p != None:
                            p.terminate()
                            p = None
                    ui.process_list = []
                op_proc.join(0.0)
    
    #ui.stop_spin()

def setupFull_cleanup(ui, max_time, start_time, op_procs, res_list, e_to, selected_list, status_list, rc_list, status_usb, trylimit_list, rescan_needed):
    curr_time = time.time()
    elapsed_time = curr_time - start_time
    if elapsed_time >= max_time:
        e_to.set()
        for op_proc in op_procs:
            if op_proc != None and op_proc.is_alive():
                if ui.process != None:
                    ui.process.terminate()
                    ui.process = None
                elif ui.process_list != []:
                    for p in ui.process_list:
                        if p != None:
                            p.terminate()
                            p = None
                    ui.process_list = []
                op_proc.join(0.0)
    
    for i in range(len(ui.devs_list)):
        runop.postlock(i)
    
    #print rc_list[:]
            
    ##print res_list
    ui.op_inprogress = False
    ui.pbawrite_ip = False
    
    #no_usb = False
    
    if status_usb.value != 0:
        ui.msg_err('USB setup failed. Setup aborted.')
        ui.stop_spin()
        ui.setup_prompt1()
    
    else:
        start_f0 = True
        start_p0 = True
        list_f0 = ''
        list_p0 = ''
        
        start_miss = True
        list_miss = ''
        
        for z in range(len(selected_list)):
            ui.devname = ui.devs_list[selected_list[z]]
            if rc_list[z] != -1:
                if rc_list[z] == 0:
                    if not start_p0:
                        list_p0 = list_p0 + ', '
                    else:
                        start_p0 = False
                    list_p0 = list_p0 + ui.devs_list[selected_list[z]]
                    ui.mbr_setup_list.append(selected_list[z])
                    ui.mbr_setup_list.sort()
                elif rc_list[z] != -2:
                    if not start_f0:
                        list_f0 = list_f0 + ', '
                    else:
                        start_f0 = False
                    list_f0 = list_f0 + ui.devs_list[selected_list[z]]
                else:
                    if not start_miss:
                        list_miss = list_miss + ', '
                    else:
                        start_miss = False
                    list_miss = list_miss + ui.devs_list[selected_list[z]]
            
        usb_fail = []
        for i in range(len(res_list)):
            if res_list[i] == -1:
                if e_to.is_set():
                    res_list[i] = 2
                else:
                    res_list[i] = 1
            elif res_list[i] == 3:
                usb_fail.append(i)
                res_list[i] = 0
                
        if list_miss != '':
            ui.msg_err(list_miss + ' not detected.')
                
        if len(usb_fail) > 0:
            start = True
            usb_f = ''
            for j in usb_fail:
                if not start:
                    usb_f = usb_f + ', '
                else:
                    start = False
                usb_f = usb_f + ui.devs_list[selected_list[j]]
            ui.msg_err('Password could not be saved to USB for the following drive(s): ' + usb_f + '.')
                
                
        res_sum = sum(res_list)
        
        if res_sum != 0:
            start_f = True
            start_p = True
            start_to = True
            start_na = True
            start_nat = True
            start_alo = True
            list_f = ''
            list_p = ''
            list_to = ''
            list_na = ''
            list_nat = ''
            list_alo = ''
            count = 0
            sel_list_pass = []
            #print ui.mv_list
            for y in range(len(selected_list)):
                ui.devname = ui.devs_list[selected_list[y]]
                if res_list[count] == 0:
                    dialogs.query(None,ui,1)
                    if not start_p:
                        list_p = list_p + ', '
                    else:
                        start_p = False
                    list_p = list_p + ui.devs_list[selected_list[y]]
                    ui.setupstatus_list[selected_list[y]] = 'Yes'
                    ui.setupuser_list[selected_list[y]] = 'No'
                    if trylimit_list[y] != None:
                        ui.retrylimit_list[selected_list[y]] = trylimit_list[y]
                    else:
                        ui.retrylimit_list[selected_list[y]] = 10
                    ui.updateDevs(selected_list[y],[2,3])
                    if os.path.isfile('datawrite' + ui.sn_list[selected_list[y]] + '.txt'):
                        f = open('datawrite' + ui.sn_list[selected_list[y]] + '.txt', 'r')
                        pv = f.read()
                        f.close()
                        ui.pba_list[selected_list[y]] = pv.strip('\0')
                        act_idx = ui.dev_select.get_active()
                        if selected_list[y] == act_idx:
                            ui.dev_pbaVer.set_text(pv.strip('\0'))
                        try:
                            os.remove('datawrite' + ui.sn_list[selected_list[y]] + '.txt')
                        except WindowsError:
                            print 'file removal failed'
                else:
                    if ui.pass_dir != '' and status_usb.value == 0:
                        fn = ui.pass_dir + '\\OpalLock\\' + ui.vendor_list[selected_list[y]] + '_' + ui.sn_list[selected_list[y]] + '.psw'
                        if os.path.isfile(fn):
                            try:
                                os.remove(fn)
                            except WindowsError:
                                print 'Failed to remove password file after setup failure'
                    
                    if res_list[count] == 1:
                        if status_list[y] == ui.NOT_AUTHORIZED:
                            if not start_nat:
                                list_nat = list_nat + ', '
                            else:
                                start_nat = False
                            list_nat = list_nat + ui.devs_list[selected_list[y]]
                        elif status_list[y] == ui.AUTHORITY_LOCKED_OUT:
                            if not start_alo:
                                list_alo = list_alo + ', '
                            else:
                                start_alo = False
                            list_alo = list_alo + ui.devs_list[selected_list[y]]
                        else:
                            if not start_f:
                                list_f = list_f + ', '
                            else:
                                start_f = False
                            list_f = list_f + ui.devs_list[selected_list[y]]
                    elif res_list[count] == 2:
                        if not start_to:
                            list_to = list_to + ', '
                        else:
                            start_to = False
                        list_to = list_to + ui.devs_list[selected_list[y]]
                    elif res_list[count] != 6:
                        if not start_na:
                            list_na = list_na + ', '
                        else:
                            start_na = False
                        list_na = list_na + ui.devs_list[selected_list[y]]
                count = count + 1
            ui.pass_dir = ''
            if list_na != '':
                ui.msg_err('Setup failed for ' + list_na + '.')
            if list_nat != '':
                ui.msg_err('Setup failed, password already set for ' + list_nat + '.')
            if list_alo != '':
                ui.msg_err('Setup failed. Please power cycle ' + list_nat + ' before trying again.')
            if list_p == '':
                if list_f == '':
                    ui.msg_err('Password set up failed for ' + list_f + '.')
                    ui.msg_ok('USB setup complete.')
                elif list_to == '':
                    ui.msg_err('Password set up timed out for ' + list_to + '.')
                    ui.msg_ok('USB setup complete.')
                else:
                    ui.msg_err('Password set up failed for ' + list_f + ', and timed out for ' + list_to + '.')
                    ui.msg_ok('USB setup complete.')
                ui.op_instr.show()
                ui.box_newpass.show()
                ui.box_newpass_confirm.show()
                ui.check_box_pass.show()
                ui.setup_next.show()
                ui.cancel_button.show()
            else:
                if list_to == '':
                    ui.msg_err('Password successfully set up for ' + list_p + ', but failed for ' + list_f + '.')
                    ui.msg_ok('USB setup complete.')
                elif list_f == '':
                    ui.msg_err('Password successfully set up for ' + list_p + ', but timed out for ' + list_to + '.')
                    ui.msg_ok('USB setup complete.')
                else:
                    ui.msg_err('Password successfully set up for ' + list_p + ', but failed for ' + list_f + ' and timed out for ' + list_to + '.')
                    ui.msg_ok('USB setup complete.')
                if list_p0 != '' or list_f0 != '':
                    if list_p0 == '':
                        ui.msg_err('Preboot image setup failed for ' + list_f0 + '.')
                    elif list_f0 == '':
                        ui.msg_ok('Preboot image set up successfully for ' + list_p0 + '.')
                    else:
                        ui.msg_err('Preboot image set up successfully for ' + list_p0 + ', but failed for ' + list_f0 + '.')
            ui.setup_prompt1()
        else:
            start = True
            liststr = ''
            y = 0
            for d in selected_list:
                ui.devname = ui.devs_list[d]
                dialogs.query(None,ui,1)
                if not start:
                    liststr = liststr + ', '
                else:
                    start = False
                liststr = liststr + ui.devs_list[d]
                ui.setupstatus_list[d] = 'Yes'
                ui.setupuser_list[d] = 'No'
                if trylimit_list[y] != None:
                    ui.retrylimit_list[d] = trylimit_list[y]
                else:
                    ui.retrylimit_list[d] = 10
                ui.updateDevs(d,[2,3])
                
                if os.path.isfile('datawrite' + ui.sn_list[d] + '.txt'):
                    f = open('datawrite' + ui.sn_list[d] + '.txt', 'r')
                    pv = f.read()
                    f.close()
                    ui.pba_list[d] = pv.strip('\0')
                    act_idx = ui.dev_select.get_active()
                    if d == act_idx:
                        ui.dev_pbaVer.set_text(pv.strip('\0'))
                    try:
                        os.remove('datawrite' + ui.sn_list[d] + '.txt')
                    except WindowsError:
                        print 'file removal failed'
                y = y + 1
                
            ui.msg_ok('Password successfully set up for ' + liststr + '. Bootable USB set up successfully.')
            if list_p0 != '' or list_f0 != '':
                if list_p0 == '':
                    ui.msg_err('Preboot image setup failed for ' + list_f0 + '.')
                elif list_f0 == '':
                    ui.msg_ok('Preboot image set up successfully for ' + list_p0 + '.')
                else:
                    ui.msg_err('Preboot image set up successfully for ' + list_p0 + ', but failed for ' + list_f0 + '.')
            ui.returnToMain(None, True)
        ui.stop_spin()
        if rescan_needed:
            ui.msg_ok('A rescan is needed to update the drive list, press OK to proceed.')
            runscan.run_scan(None, ui, True)

def pbaWrite_cleanup(ui, max_time, start_time, op_threads, res_list, e_to, selected_list, status_list, rescan_needed, val2, val3, val4):
    curr_time = time.time()
    elapsed_time = curr_time - start_time
    if elapsed_time >= max_time:
        e_to.set()
        for op_thread in op_threads:
            if op_thread.is_alive():
                if ui.process != None:
                    ui.process.terminate()
                    ui.process = None
                elif ui.process_list != []:
                    i = 0
                    for p in ui.process_list:
                        if p != None:
                            p.terminate()
                            p = None
                            res_list[i] = 1
                        i = i + 1
                    ui.process_list = []
                op_thread.join(0.0)
        
    ui.op_inprogress = False
    ui.pbawrite_ip = False
    save_fail = []
    for i in range(len(res_list)):
        if res_list[i] == -1:
            if e_to.is_set():
                res_list[i] = 2
            else:
                res_list[i] = 3
        elif res_list[i] == 5:
            save_fail.append(i)
            res_list[i] = 0
            
    for i in range(len(ui.devs_list)):
        runop.postlock(i)
            
    if len(save_fail) > 0:
        start = True
        sf_txt = ''
        for j in save_fail:
            if not start:
                sf_txt = sf_txt + ', '
            else:
                start = False
            sf_txt = sf_txt + ui.devs_list[selected_list[j]]
        ui.msg_err('Password could not be saved to USB for the following drive(s): ' + sf_txt + '.')
            
    res_sum = sum(res_list)
    
    
    
    act_idx = ui.dev_select.get_active()
    
    if res_sum != 0:
        start_f = True
        start_p = True
        start_alo = True
        start_to = True
        start_na = True
        start_spb = True
        start_miss = True
        list_f = ''
        list_p = ''
        list_alo = ''
        list_to = ''
        list_na = ''
        list_spb = ''
        list_miss = ''
        count = 0
        last_f = -1
        count_f = 0
        for y in selected_list:
            ui.devname = ui.devs_list[y]
            if res_list[count] == 0:
                ui.admin_aol_list[y] = 0
                dialogs.query(None,ui,1)
                if not start_p:
                    list_p = list_p + ', '
                else:
                    start_p = False
                list_p = list_p + ui.devs_list[y]
                if os.path.isfile('datawrite' + ui.sn_list[y] + '.txt'):
                    f = open('datawrite' + ui.sn_list[y] + '.txt', 'r')
                    pv = f.read()
                    f.close()
                    ui.pba_list[y] = pv.strip('\0')
                    if y == act_idx:
                        ui.dev_pbaVer.set_text(pv.strip('\0'))
                    try:
                        os.remove('datawrite' + ui.sn_list[y] + '.txt')
                    except WindowsError:
                        print 'file removal failed'
            elif res_list[count] == 1:
                if status_list[count] == ui.NOT_AUTHORIZED and ui.admin_aol_list[y] < ui.retrylimit_list[y]:
                    ui.admin_aol_list[y] = ui.admin_aol_list[y] + 1
                    if not start_f:
                        list_f = list_f + ', '
                    else:
                        start_f = False
                    list_f = list_f + ui.devs_list[y]
                    count_f = count_f + 1
                    last_f = y
                elif status_list[count] == ui.AUTHORITY_LOCKED_OUT or ui.admin_aol_list[y] >= ui.retrylimit_list[y]:
                    ui.admin_aol_list[y] = ui.retrylimit_list[y] + 1
                    if not start_alo:
                        list_alo = list_alo + ', '
                    else:
                        start_alo = False
                    list_alo = list_alo + ui.devs_list[y]
            elif res_list[count] == 3:
                if not start_na:
                    list_na = list_na + ', '
                else:
                    start_na = False
                list_na = list_na + ui.devs_list[y]
            elif res_list[count] == 4:
                if not start_spb:
                    list_spb = list_spb + ', '
                else:
                    start_spb = False
                list_spb = list_spb + ui.devs_list[y]
            elif res_list[count] == 6:
                if not start_miss:
                    list_miss = list_miss + ', '
                else:
                    start_miss = False
                list_miss = list_miss + ui.devs_list[y]
            else:
                if not start_to:
                    list_to = list_to + ', '
                else:
                    start_to = False
                list_to = list_to + ui.devs_list[y]
            count = count + 1
        if list_miss != '':
            ui.msg_err(list_miss + ' not detected.')
        if list_na != '':
            ui.msg_err('Preboot image write failed for ' + list_na + '.')
        if list_spb != '':
            ui.msg_err('Preboot image write failed for ' + list_spb + '. There was an error while attempting to access the drive, please power cycle the drive before trying again.')
            
        if list_to != '':
            ui.msg_err('Preboot image write timed out for ' + list_to + '.')
        if list_f != '':
            if count_f > 1:
                ui.msg_err('Preboot image write failed for ' + list_f + '. Invalid password.')
            else:
                ui.msg_err('Preboot image write failed for ' + list_f + '. Invalid password. Attempt ' + str(ui.admin_aol_list[last_f]) + ' of ' + str(ui.retrylimit_list[last_f]) + '.')
        if list_alo != '':
            ui.msg_err('Retry limit reached for ' + list_alo + '. Please power cycle the drive before trying again.')
        if list_p != '':
            ui.msg_ok('Preboot image successfully written for ' + list_p + '.')
        ui.updatePBA_prompt()
    else:
        start = True
        liststr = ''
        count = 0
        for d in selected_list:
            ui.devname = ui.devs_list[d]
            ui.admin_aol_list[d] = 0
            dialogs.query(None,ui,1)
            if not start:
                liststr = liststr + ', '
            else:
                start = False
            liststr = liststr + ui.devs_list[d]
            if os.path.isfile('datawrite' + ui.sn_list[d] + '.txt'):
                f = open('datawrite' + ui.sn_list[d] + '.txt', 'r')
                pv = f.read()
                f.close()
                ui.pba_list[d] = pv.strip('\0')
                if d == act_idx:
                    ui.dev_pbaVer.set_text(pv.strip('\0'))
                try:
                    os.remove('datawrite' + ui.sn_list[d] + '.txt')
                except WindowsError:
                    print 'file removal failed'
            count = count + 1
        ui.msg_ok('Preboot image successfully written for ' + liststr + '.')
        ui.returnToMain(None, False)
    ui.stop_spin()
    if rescan_needed:
        ui.msg_ok('A rescan is needed to update the drive list, press OK to proceed.')
        runscan.run_scan(None, ui, True)
        
def changePW_cleanup(ui, max_time, start_time, op_threads, res_list, e_to, selected_list, status_list, rescan_needed, val2, val3, val4):
    curr_time = time.time()
    elapsed_time = curr_time - start_time
    if elapsed_time >= max_time:
        e_to.set()
        for op_thread in op_threads:
            if op_thread.is_alive():
                if ui.process != None:
                    ui.process.terminate()
                    ui.process = None
                elif ui.process_list != []:
                    for p in ui.process_list:
                        if p != None:
                            p.terminate()
                            p = None
                    ui.process_list = []
                op_thread.join(0.0)
    
    ui.op_inprogress = False
    save_fail = []
    for i in range(len(res_list)):
        if res_list[i] == -1:
            if e_to.is_set():
                res_list[i] = 2
            else:
                res_list[i] = 3
        elif res_list[i] == 5:
            save_fail.append(i)
            res_list[i] = 0
            
    if len(save_fail) > 0:
        start = True
        sf_txt = ''
        for j in save_fail:
            if not start:
                sf_txt = sf_txt + ', '
            else:
                start = False
            sf_txt = sf_txt + ui.devs_list[selected_list[j]]
        ui.msg_err('Password could not be saved to USB for the following drives: ' + sf_txt + '.')
            
    res_sum = sum(res_list)
    
    if res_sum != 0:
        start_f = True
        start_p = True
        start_alo = True
        start_to = True
        start_na = True
        start_spb = True
        start_miss = True
        list_f = ''
        list_p = ''
        list_alo = ''
        list_to = ''
        list_na = ''
        list_spb = ''
        list_miss = ''
        count = 0
        count_f = 0
        last_f = -1
        for y in selected_list:
            ui.devname = ui.devs_list[y]
            if res_list[count] == 0:
                dialogs.query(None,ui,1)
                if not start_p:
                    list_p = list_p + ', '
                else:
                    start_p = False
                list_p = list_p + ui.devs_list[y]
            elif res_list[count] == 1:
                if status_list[count] == ui.NOT_AUTHORIZED and ((ui.auth_menu.get_active() == 0 and ui.admin_aol_list[y] <= ui.retrylimit_list[y]) or (ui.auth_menu.get_active() == 1 and ui.user_aol_list[y] <= ui.retrylimit_list[y])):
                    if not start_f:
                        list_f = list_f + ', '
                    else:
                        start_f = False
                    list_f = list_f + ui.devs_list[y]
                    count_f = count_f + 1
                    last_f = y
                elif status_list[count] == ui.AUTHORITY_LOCKED_OUT or ((ui.auth_menu.get_active() == 0 and ui.admin_aol_list[y] > ui.retrylimit_list[y]) or (ui.auth_menu.get_active() == 1 and ui.user_aol_list[y] > ui.retrylimit_list[y])):
                    if not start_alo:
                        list_alo = list_alo + ', '
                    else:
                        start_alo = False
                    list_alo = list_alo + ui.devs_list[y]
            elif res_list[count] == 3:
                if not start_na:
                    list_na = list_na + ', '
                else:
                    start_na = False
                list_na = list_na + ui.devs_list[y]
            elif res_list[count] == 4:
                if not start_:
                    list_spb = list_spb + ', '
                else:
                    start_spb = False
                list_spb = list_spb + ui.devs_list[y]
            elif res_list[count] == 6:
                if not start_miss:
                    list_miss = list_miss + ', '
                else:
                    start_miss = False
                list_miss = list_miss + ui.devs_list[y]
            else:
                if not start_to:
                    list_to = list_to + ', '
                else:
                    start_to = False
                list_to = list_to + ui.devs_list[y]
            count = count + 1
        if list_miss != '':
            ui.msg_err(list_miss + ' not detected.')
        if list_na != '':
            ui.msg_err('Password change failed for ' + list_na + '.')
        if list_spb != '':
            ui.msg_err('Password change failed for ' + list_spb + '. There was an error while attempting to access the drive, please power cycle the drive before trying again.')
            
        if list_to != '':
            ui.msg_err('Password change timed out for ' + list_to + '.')
        if list_f != '':
            if count_f > 1:
                ui.msg_err('Password change failed for ' + list_f + '. Invalid password.')
            elif ui.auth_menu.get_active() == 0:
                ui.msg_err('Password change failed for ' + list_f + '. Invalid password. Attempt ' + str(ui.admin_aol_list[last_f]) + ' of ' + str(ui.retrylimit_list[last_f]) + '.')
            else:
                ui.msg_err('Password change failed for ' + list_f + '. Invalid password. Attempt ' + str(ui.user_aol_list[last_f]) + ' of ' + str(ui.retrylimit_list[last_f]) + '.')
        if list_alo != '':
            if ui.auth_menu.get_active() == 0:
                ui.msg_err('Retry limit reached for ' + list_alo + ', please power cycle the drive(s) before trying again.')
            else:
                ui.msg_err('User retry limit reached for ' + list_alo + ', please power cycle the drive(s) before trying again.')
        if list_p != '':
            ui.msg_ok('Password successfully changed for ' + list_p + '.')
        ui.changePW_prompt()
    else:
        start = True
        liststr = ''
        for d in selected_list:
            ui.devname = ui.devs_list[d]
            dialogs.query(None,ui,1)
            if not start:
                liststr = liststr + ', '
            else:
                start = False
            liststr = liststr + ui.devs_list[d]
        ui.msg_ok('Password successfully changed for ' + liststr + '.')
        
        ui.returnToMain(None, False)
    ui.stop_spin()
    if rescan_needed:
        ui.msg_ok('A rescan is needed to update the drive list, press OK to proceed.')
        runscan.run_scan(None, ui, True)

def unlockPBA_cleanup(ui, max_time, start_time, op_threads, res_list, e_to, selected_list, status_list, reboot, rescan_needed, val3, val4):
    #print 'start unlockPBA_cleanup'
    if rescan_needed:
        ui.scan_ip = True
    curr_time = time.time()
    elapsed_time = curr_time - start_time
    m_count = 0
    if elapsed_time >= max_time:
        e_to.set()
        for op_thread in op_threads:
            if op_thread.is_alive():
                if ui.process != None:
                    ui.process.terminate()
                    ui.process = None
                elif ui.process_list != []:
                    for p in ui.process_list:
                        if p != None:
                            p.terminate()
                            p = None
                    ui.process_list = []
                op_thread.join(0.0)
    
    ui.op_inprogress = False
    save_fail = []
    for i in range(len(res_list)):
        if res_list[i] == -1:
            if e_to.is_set():
                res_list[i] = 2
            else:
                res_list[i] = 3
        elif res_list[i] == 5:
            save_fail.append(i)
            res_list[i] = 0
            
    if len(save_fail) > 0:
        start = True
        sf_txt = ''
        for j in save_fail:
            if not start:
                sf_txt = sf_txt + ', '
            else:
                start = False
            sf_txt = sf_txt + ui.devs_list[selected_list[j]]
        ui.msg_err('Password could not be saved to USB for the following drives: ' + sf_txt + '.')
        
        
    res_sum = sum(res_list)
    
    
    any_unlocked = False
    sel_idx = 0
    for x in selected_list:
        if x in ui.locked_list and res_list[sel_idx] == 0:
            any_unlocked = True
        sel_idx = sel_idx + 1
    if any_unlocked and ui.DEV_OS == 'Windows':
        dl_len = len(ui.devs_list)
        for i in range(dl_len):
            runop.prelock(i)
        with open(os.devnull, 'w') as pipe:
            f = open('rescan.txt', 'w')
            f.write('rescan')
            f.close()
            subprocess.call(['diskpart', '/s', 'rescan.txt'], stdout=pipe)#stderr=log)
            os.remove('rescan.txt')
        for i in range(dl_len):
            runop.postlock(i)
    
    if res_sum != 0:
        start_f = True
        start_p = True
        start_alo = True
        start_to = True
        start_spb = True
        start_na = True
        start_miss = True
        list_f = ''
        list_p = ''
        list_alo = ''
        list_to = ''
        list_spb = ''
        list_na = ''
        list_miss = ''
        count = 0
        count_f = 0
        last_f = -1
        for y in selected_list:
            ui.devname = ui.devs_list[y]
            if res_list[count] == 0:
                dialogs.query(None,ui,1)
                if not start_p:
                    list_p = list_p + ', '
                else:
                    start_p = False
                list_p = list_p + ui.devs_list[y]
                if ui.DEV_OS == 'Windows':
                    background.remount(ui, y)
                ui.lockstatus_list[y] = "Unlocked"
                
                ui.updateDevs(y,[2])
                
                    
            elif res_list[count] == 1:
                if status_list[count] == ui.NOT_AUTHORIZED and ((ui.auth_menu.get_active() == 0 and ui.admin_aol_list[y] <= ui.retrylimit_list[y]) or (ui.auth_menu.get_active() == 1 and ui.user_aol_list[y] <= ui.retrylimit_list[y])):
                    if not start_f:
                        list_f = list_f + ', '
                    else:
                        start_f = False
                    list_f = list_f + ui.devs_list[y]
                    count_f = count_f + 1
                    last_f = y
                elif status_list[count] == ui.AUTHORITY_LOCKED_OUT or ((ui.auth_menu.get_active() == 0 and ui.admin_aol_list[y] > ui.retrylimit_list[y]) or (ui.auth_menu.get_active() == 1 and ui.user_aol_list[y] > ui.retrylimit_list[y])):
                    if not start_alo:
                        list_alo = list_alo + ', '
                    else:
                        start_alo = False
                    list_alo = list_alo + ui.devs_list[y]
            elif res_list[count] == 3:
                if not start_na:
                    list_na = list_na + ', '
                else:
                    start_na = False
                list_na = list_na + ui.devs_list[y]
            elif res_list[count] == 4:
                if not start_spb:
                    list_spb = list_spb + ', '
                else:
                    start_spb = False
                list_spb = list_spb + ui.devs_list[y]
            elif res_list[count] == 6:
                if not start_miss:
                    list_miss = list_miss + ', '
                else:
                    start_miss = False
                list_miss = list_miss + ui.devs_list[y]
            else:
                if not start_to:
                    list_to = list_to + ', '
                else:
                    start_to = False
                list_to = list_to + ui.devs_list[y]
            count = count + 1
        #print ui.mv_list
        if list_miss != '':
            ui.msg_err(list_miss + ' not detected.')
        if list_na != '':
            ui.msg_err('Failed to unlock ' + list_na + '.')
        if list_spb != '':
            ui.msg_err('Failed to unlock ' + list_spb + '. There was an error while attempting to access the drive, please power cycle the drive before trying again.')
            
        if list_to != '':
            ui.msg_err('Unlock timed out for ' + list_to + '.')
        if list_f != '':
            if count_f > 1:
                ui.msg_err('Failed to unlock ' + list_f + '. Invalid password.')
            elif ui.auth_menu.get_active() == 0:
                ui.msg_err('Failed to unlock ' + list_f + '. Invalid password. Attempt ' + str(ui.admin_aol_list[last_f]) + ' of ' + str(ui.retrylimit_list[last_f]) + '.')
            else:
                ui.msg_err('Failed to unlock ' + list_f + '. Invalid password. Attempt ' + str(ui.user_aol_list[last_f]) + ' of ' + str(ui.retrylimit_list[last_f]) + '.')
        if list_alo != '':
            if ui.auth_menu.get_active() == 0:
                ui.msg_err('Retry limit reached for ' + list_alo + ', please power cycle the drive(s) before trying again.')
            else:
                ui.msg_err('User retry limit reached for ' + list_alo + ', please power cycle the drive(s) before trying again.')
            
        if list_p != '':
            ui.msg_ok(list_p + ' successfully unlocked.')
        ui.stop_spin()
        ui.unlock_prompt(ui)
        if m_count > 0 and not rescan_needed:
            runscan.run_scan(None, ui, False)
    else:
        if not reboot:
            start = True
            liststr = ''
            for d in selected_list:
                ui.devname = ui.devs_list[d]
                dialogs.query(None,ui,1)
                if not start:
                    liststr = liststr + ', '
                else:
                    start = False
                liststr = liststr + ui.devs_list[d]
                
                if ui.DEV_OS == 'Windows':
                    background.remount(ui, d)
                
                ui.lockstatus_list[d] = "Unlocked"
                ui.updateDevs(d,[2])
                if d in ui.ulocked_list:
                    ui.ulocked_list.remove(d)
                
            ui.msg_ok(liststr + ' successfully unlocked.')
            ui.stop_spin()
            ui.returnToMain(None, False)
            if m_count > 0 and not rescan_needed:
                runscan.run_scan(None, ui, False)
    if rescan_needed:
        ui.msg_ok('A rescan is needed to update the drive list, press OK to proceed.')
        runscan.run_scan(None, ui, True)
    #ui.scan_ip = False
    if res_sum == 0 and reboot:
        ui.stop_spin()
        ui.reboot()

def revertKeep_cleanup(ui, max_time, start_time, op_threads, res_list, e_to, selected_list, status_list, rescan_needed, val2, val3, val4):
    if rescan_needed:
        ui.scan_ip = True
    curr_time = time.time()
    elapsed_time = curr_time - start_time
    m_count = 0
    if elapsed_time >= max_time:
        e_to.set()
        for op_thread in op_threads:
            if op_thread.is_alive():
                if ui.process != None:
                    ui.process.terminate()
                    ui.process = None
                elif ui.process_list != []:
                    for p in ui.process_list:
                        if p != None:
                            p.terminate()
                            p = None
                    ui.process_list = []
                op_thread.join(0.0)
    
    ui.op_inprogress = False
    for i in range(len(res_list)):
        if res_list[i] == -1:
            if e_to.is_set():
                res_list[i] = 2
            else:
                res_list[i] = 3
    res_sum = sum(res_list)
    
    
    any_unlocked = False
    sel_idx = 0
    for x in selected_list:
        if x in ui.locked_list and res_list[sel_idx] == 0:
            any_unlocked = True
        sel_idx = sel_idx + 1
    if any_unlocked and ui.DEV_OS == 'Windows':
        for z in range(len(ui.devs_list)):
            runop.prelock(z)
        with open(os.devnull, 'w') as pipe:
            f = open('rescan.txt', 'w')
            f.write('rescan')
            f.close()
            subprocess.call(['diskpart', '/s', 'rescan.txt'], stdout=pipe)#stderr=log)
            os.remove('rescan.txt')
        for z in range(len(ui.devs_list)):
            runop.postlock(z)
    
    if res_sum != 0:
        start_f = True
        start_p = True
        start_alo = True
        start_to = True
        start_na = True
        start_spb = True
        start_miss = True
        list_f = ''
        list_p = ''
        list_alo = ''
        list_to = ''
        list_na = ''
        list_spb = ''
        list_miss = ''
        count = 0
        count_f = 0
        last_f = -1
        for y in selected_list:
            ui.devname = ui.devs_list[y]
            if res_list[count] == 0:
                dialogs.query(None,ui,1)
                if not start_p:
                    list_p = list_p + ', '
                else:
                    start_p = False
                list_p = list_p + ui.devs_list[y]
                
                if ui.DEV_OS == 'Windows':
                    background.remount(ui, y)
                    
                
                ui.lockstatus_list[y] = "Unlocked"
                ui.setupstatus_list[y] = "No"
                ui.setupuser_list[y] = 'N/A'
                ui.pba_list[y] = 'N/A'
                if y in ui.mbr_setup_list:
                    ui.mbr_setup_list.remove(y)
                ui.updateDevs(y,[4])
            elif res_list[count] == 1:
                if status_list[count] == ui.NOT_AUTHORIZED and ui.admin_aol_list[y] <= ui.retrylimit_list[y]:
                    if not start_f:
                        list_f = list_f + ', '
                    else:
                        start_f = False
                    list_f = list_f + ui.devs_list[y]
                    count_f = count_f + 1
                    last_f = y
                if status_list[count] == ui.AUTHORITY_LOCKED_OUT or ui.admin_aol_list[y] > ui.retrylimit_list[y]:
                    if not start_alo:
                        list_alo = list_alo + ', '
                    else:
                        start_alo = False
                    list_alo = list_alo + ui.devs_list[y]
            elif res_list[count] == 2:
                if not start_to:
                    list_to = list_to + ', '
                else:
                    start_to = False
                list_to = list_to + ui.devs_list[y]
            elif res_list[count] == 4:
                if not start_spb:
                    list_spb = list_spb + ', '
                else:
                    start_spb = False
                list_spb = list_spb + ui.devs_list[y]
            elif res_list[count] == 6:
                if not start_miss:
                    list_miss = list_miss + ', '
                else:
                    start_miss = False
                list_miss = list_miss + ui.devs_list[y]
            else:
                if not start_na:
                    list_na = list_na + ', '
                else:
                    start_to = False
                list_na = list_na + ui.devs_list[y]
            count = count + 1
        if list_miss != '':
            ui.msg_err(list_miss + ' not detected.')
        if list_spb != '':
            ui.msg_err('Failed to remove lock for ' + list_spb + '. There was an error while attempting to access the drive, please power cycle the drive before trying again.')
        if list_na != '':
            ui.msg_err('Failed to remove lock for ' + list_na + '.')
        if list_to != '':
            ui.msg_err('Remove lock timed out for ' + list_to + '.')
        if list_f != '':
            if count_f > 1:
                ui.msg_err('Failed to remove lock for ' + list_f + '. Invalid password.')
            else:
                ui.msg_err('Failed to remove lock for ' + list_f + '. Invalid password. Attempt ' + str(ui.admin_aol_list[last_f]) + ' of ' + str(ui.retrylimit_list[last_f]) + '.')
        if list_alo != '':
            ui.msg_err('Retry limit reached for ' + list_alo + ', please power cycle the drive(s) before trying again.')
        if list_p != '':
            ui.msg_ok('Successfully removed lock for ' + list_p + '.')
        ui.revert_keep_prompt(ui)
        if m_count > 0 and not rescan_needed:
            runscan.run_scan(None, ui, False)
    else:
        start = True
        liststr = ''
        for d in selected_list:
            ui.devname = ui.devs_list[d]
            dialogs.query(None,ui,1)
            if not start:
                liststr = liststr + ', '
            else:
                start = False
            liststr = liststr + ui.devs_list[d]
            
            
            if ui.DEV_OS == 'Windows':
                background.remount(ui, d)
            ui.lockstatus_list[d] = "Unlocked"
            ui.setupstatus_list[d] = "No"
            ui.setupuser_list[d] = 'N/A'
            ui.pba_list[d] = 'N/A'
            if d in ui.mbr_setup_list:
                ui.mbr_setup_list.remove(d)
            ui.updateDevs(d,[4])
        ui.msg_ok('Successfully removed lock for ' + liststr + '.')
        
        ui.returnToMain(None, False)
        if m_count > 0 and not rescan_needed:
            runscan.run_scan(None, ui, False)
    ui.stop_spin()
    if rescan_needed:
        ui.msg_ok('A rescan is needed to update the drive list, press OK to proceed.')
        runscan.run_scan(None, ui, True)
    #ui.scan_ip = False
        
def revertErase_cleanup(ui, max_time, start_time, op_threads, res_list, e_to, selected_list, status_list, rescan_needed, val2, val3, val4):
    curr_time = time.time()
    elapsed_time = curr_time - start_time
    
    if elapsed_time >= max_time:
        e_to.set()
        for op_thread in op_threads:
            if op_thread.is_alive():
                if ui.process != None:
                    ui.process.terminate()
                    ui.process = None
                elif ui.process_list != []:
                    for p in ui.process_list:
                        if p != None:
                            p.terminate()
                            p = None
                    ui.process_list = []
                op_thread.join(0.0)
    
    ui.op_inprogress = False
    for i in range(len(res_list)):
        if res_list[i] == -1:
            if e_to.is_set():
                res_list[i] = 2
            else:
                res_list[i] = 3
    res_sum = sum(res_list)
    
    any_removed = False
    sel_idx = 0
    
    for x in selected_list:
        if res_list[sel_idx] == 0:
            any_removed = True
        sel_idx = sel_idx + 1
    if any_removed and platform.system() == 'Windows':
        for z in range(len(ui.devs_list)):
            runop.prelock(z)
        with open(os.devnull, 'w') as pipe:
            f = open('rescan.txt', 'w')
            f.write('rescan')
            f.close()
            subprocess.call(['diskpart', '/s', 'rescan.txt'], stdout=pipe)#stderr=log)
            os.remove('rescan.txt')
        for z in range(len(ui.devs_list)):
            runop.postlock(z)
    any_success = False
    if res_sum != 0:
        start_f = True
        start_p = True
        start_alo = True
        start_to = True
        start_na = True
        start_spb = True
        start_miss = True
        list_f = ''
        list_p = ''
        list_alo = ''
        list_to = ''
        list_na = ''
        list_spb = ''
        list_miss = ''
        count = 0
        count_f = 0
        last_f = -1
        for y in selected_list:
            ui.devname = ui.devs_list[y]
            if res_list[count] == 0:
                dialogs.query(None,ui,1)
                if not start_p:
                    list_p = list_p + ', '
                else:
                    start_p = False
                list_p = list_p + ui.devs_list[y]
                
                if ui.DEV_OS == 'Windows':
                    txt = os.popen('mountvol').read()
                    del_entries = []
                    idx = 0
                    for v in ui.mv_list:
                        if v[2] == ui.sn_list[y]:
                            del_entries.append(idx)
                        idx = idx + 1
                    x = len(del_entries) - 1
                    while x >= 0:
                        ui.mv_list.pop(del_entries[x])
                        x = x - 1
                ui.lockstatus_list[y] = "Unlocked"
                ui.setupstatus_list[y] = "No"
                ui.setupuser_list[y] = 'N/A'
                ui.pba_list[y] = 'N/A'
                if y in ui.mbr_setup_list:
                    ui.mbr_setup_list.remove(y)
                ui.updateDevs(y,[4])
            elif res_list[count] == 1:
                if status_list[count] == ui.NOT_AUTHORIZED and ui.admin_aol_list[y] <= ui.retrylimit_list[y]:
                    if not start_f:
                        list_f = list_f + ', '
                    else:
                        start_f = False
                    list_f = list_f + ui.devs_list[y]
                    count_f = count_f + 1
                    last_f = y
                elif status_list[count] == ui.AUTHORITY_LOCKED_OUT or ui.admin_aol_list[y] > ui.retrylimit_list[y]:
                    if not start_alo:
                        list_alo = list_alo + ', '
                    else:
                        start_alo = False
                    list_alo = list_alo + ui.devs_list[y]
            elif res_list[count] == 2:
                if not start_to:
                    list_to = list_to + ', '
                else:
                    start_to = False
                list_to = list_to + ui.devs_list[y]
            elif res_list[count] == 4:
                if not start_spb:
                    list_spb = list_spb + ', '
                else:
                    start_spb = False
                list_spb = list_spb + ui.devs_list[y]
            elif res_list[count] == 6:
                if not start_miss:
                    list_miss = list_miss + ', '
                else:
                    start_miss = False
                list_miss = list_miss + ui.devs_list[y]
            else:
                if not start_na:
                    list_na = list_na + ', '
                else:
                    start_na = False
                list_na = list_na + ui.devs_list[y]
            count = count + 1
        if list_miss != '':
            ui.msg_err(list_miss + ' not detected.')
        if list_spb != '':
            ui.msg_err('Failed to remove lock and erase data for ' + list_spb + '. There was an error while attempting to access the drive, please power cycle the drive before trying again.')
        if list_na != '':
            ui.msg_err('Failed to remove lock and erase data for ' + list_na + '.')
            
        if list_to != '':
            ui.msg_err('Remove lock and erase data timed out for ' + list_to + '.')
        if list_f != '':
            if count_f > 1:
                ui.msg_err('Failed to remove lock and erase data for ' + list_f + '. Invalid password.')
            else:
                ui.msg_err('Failed to remove lock and erase data for ' + list_f + '. Invalid password. Attempt ' + str(ui.admin_aol_list[last_f]) + ' of ' + str(ui.retrylimit_list[last_f]) + '.')
        if list_alo != '':
            ui.msg_err('Retry limit reached for ' + list_alo + ', please power cycle the drive(s) before trying again.')
            
        if list_p != '':
            any_success = True
            ui.msg_ok('Successfully removed lock and erased data for ' + list_p + '.')
        ui.revert_erase_prompt(ui)
    else:
        any_success = True
        start = True
        liststr = ''
        for d in selected_list:
            ui.devname = ui.devs_list[d]
            dialogs.query(None,ui,1)
            if not start:
                liststr = liststr + ', '
            else:
                start = False
            liststr = liststr + ui.devs_list[d]
            if ui.DEV_OS == 'Windows':
                txt = os.popen('mountvol').read()
                del_entries = []
                idx = 0
                for v in ui.mv_list:
                    if v[2] == ui.sn_list[d]:
                        del_entries.append(idx)
                    idx = idx + 1
                x = len(del_entries) - 1
                while x >= 0:
                    ui.mv_list.pop(del_entries[x])
                    x = x - 1
            ui.lockstatus_list[d] = "Unlocked"
            ui.setupstatus_list[d] = "No"
            ui.setupuser_list[d] = 'N/A'
            ui.pba_list[d] = 'N/A'
            if d in ui.mbr_setup_list:
                ui.mbr_setup_list.remove(d)
            ui.updateDevs(d,[4])
        ui.msg_ok('Successfully removed lock and erased data for ' + liststr + '.')
        
        ui.returnToMain(None, False)
    ui.stop_spin()
    if rescan_needed:
        ui.msg_ok('A rescan is needed to update the drive list, press OK to proceed.')
        runscan.run_scan(None, ui, True)
    elif any_success:
        runscan.run_scan(None, ui, False)