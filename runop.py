import cleanop
import dialogs
import gtk
import os
import platform
import re
import runprocess
import runsetup
import runthread
if platform.system() == 'Windows':
    import subprocess
import threading
import time
import verify


def define_lock_t():
    global lock_t 
    global sync_thread
    sync_thread = True
    lock_t = [None] * 256
    for i in range(256):
        lock_t[i] = threading.Lock()

def prelock(dev_idx):
    if sync_thread == True:
        lock_t[dev_idx].acquire()
       
def postlock(dev_idx):	
    if sync_thread == True:
        try : 
            if lock_t[dev_idx].locked:
                lock_t[dev_idx].release()
        except Exception, e:
            print type(e)

def run_setupFull(button, ui):
    verify.licCheck(ui)
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
        if len(selected_list) == 0:
            ui.msg_err('No drives selected.')
            return
        
    if not ui.warned and ui.new_pass_entry.get_text() == '' or ui.confirm_pass_entry.get_text() == '':
        ui.msg_err('Enter and confirm a password.')
        return
        
    
        
    if not ui.warned:
        pw = ui.new_pass_entry.get_text()
        pw_confirm = ui.confirm_pass_entry.get_text()
        pw_trim = re.sub('\s', '', pw)
        pw_trim_confirm = re.sub(r'\s+', '', pw_confirm)
        usb_dir = ''
        if len(pw_trim) < 8:
            ui.msg_err("This password is too short.  Please enter a password at least 8 characters long excluding whitespace.")
            return
        elif ui.bad_pw.has_key(pw_trim):
            ui.msg_err("This password is on the blacklist of bad passwords, please enter a stronger password.")
            return
        elif pw != pw_confirm:
            ui.msg_err("The entered passwords do not match.")
            return
    
        message = gtk.MessageDialog(type=gtk.MESSAGE_WARNING, buttons=gtk.BUTTONS_YES_NO, parent = ui)
        message.set_markup("Warning: If you lose or forget your password, all data will be lost. Do you want to proceed?")
        
        res = message.run()
        if res == gtk.RESPONSE_YES:
            message.destroy()
            ui.warned = True
            ui.orig = ui.new_pass_entry.get_text()
            ui.new_pass_entry.get_buffer().delete_text(0,-1)
            ui.confirm_pass_entry.get_buffer().delete_text(0,-1)
            ui.op_instr.set_text('Confirm the password once more to verify.')
            ui.new_pass_entry.hide()
            ui.new_pass_label.hide()
        else:
            message.destroy()
        del pw_trim
        del pw_trim_confirm
        return
            
    if ui.warned and ui.orig != ui.confirm_pass_entry.get_text():
        ui.orig = ''
        ui.warned = False
        
        ui.msg_err('The passwords entered do not match. Try again.')
        ui.op_instr.set_text('Setting up a drive includes setting a password which you can use to unlock the drive.\nEnter the new password for the drive and click \'Continue\'.')
        return
        
    #ui.treeview.grab_focus()
    #ui.treeview.emit('toggle-cursor-row')
        
    
    
    message2 = gtk.MessageDialog(type=gtk.MESSAGE_WARNING, buttons=gtk.BUTTONS_OK_CANCEL, parent = ui)
    message2.set_markup("Final Warning: If you lose your password, all data will be lost. Are you sure you want to proceed?")
    
    res2 = message2.run()
    if res2 == gtk.RESPONSE_OK:
        message2.destroy()
        
        proceed = True
        usb_dialog = None
        
        usb_dialog = dialogs.USBDialog(ui)
        res = usb_dialog.run()
        if res != gtk.RESPONSE_OK:
            proceed = False
        else:
            if usb_dialog.usb_menu.get_active() >= 0:
                msg_usb = gtk.MessageDialog(type=gtk.MESSAGE_WARNING, buttons=gtk.BUTTONS_OK_CANCEL, parent = ui)
                markupString='<span foreground="red">WARNING: All data on the USB will be erased (except for any previously saved password files).  Are you sure you want to proceed?</span> '
                msg_usb.set_markup(markupString)

                res2 = msg_usb.run()
                if res2 != gtk.RESPONSE_OK:
                    proceed = False
                msg_usb.destroy()
            else:
                ui.msg_err('No USB selected')
                ui.op_instr.set_text('Setting up a drive includes setting a password which you can use to unlock the drive.\nEnter the new password for the drive and click \'Continue\'.')
                usb_dialog.destroy()
                ui.setup_prompt1()
                return
        
        if not proceed:
            ui.op_instr.set_text('Setting up a drive includes setting a password which you can use to unlock the drive.\nEnter the new password for the drive and click \'Continue\'.')
            usb_dialog.destroy()
            return
        index2 = -1
        index2 = usb_dialog.usb_menu.get_active()
        usb_dialog.destroy()
        
        preserved_files = []
        
        if (ui.VERSION % 3 == 0 or (ui.VERSION == 1 and ui.PBA_VERSION != 1)) and ui.pass_sav.get_active():
            
            drive = ''
            if ui.DEV_OS == 'Windows':
                usb_dev = ui.usb_list[index2][1]
                for j in range(len(ui.devs_list)):
                    prelock(j)
                p = subprocess.Popen(["diskpart"], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
                res1 = p.stdin.write('select disk ' + usb_dev + b'\n')
                res1 = p.stdin.write('detail disk\n')
                res1 = p.stdin.write('exit\n')

                output = p.communicate()[0]
                for j in range(len(ui.devs_list)):
                    postlock(j)
                
                vol_re = 'Volume [0-9]+\s+([A-Z])\s+'

                list_v = re.findall(vol_re, output)
                if list_v != []:
                    drive = list_v[0] + ':\\'
                    ui.pass_dir = list_v[0] + ':'
                    
                #else:
                #    ui.msg_err('Selected USB could not be detected.')
                #    return
            elif ui.DEV_OS == 'Linux':
                drive = ui.usb_list[index2][1]
                ui.pass_dir = drive
            #if not os.path.isdir(drive):
            #    ui.msg_err('Selected USB could not be detected.')
            #    return
            #else:
            if drive != '' and os.path.isdir(drive):
                dir_fl = drive + 'OpalLock\\'
                if os.path.isdir(dir_fl):
                    existingfiles = [f for f in os.listdir(dir_fl) if os.path.isfile(dir_fl + f)]
                    if len(existingfiles) > 0:
                        for p in existingfiles:
                            try:
                                f = open(dir_fl + p, 'r')
                                txt = f.read()
                                f.close()
                                tp = (dir_fl + p, txt)
                                preserved_files.append(tp)
                            except IOError:
                                pass
                            
        usb_check = []
        
        if ui.DEV_OS == 'Windows':
            txt = os.popen(ui.prefix + 'wmic diskdrive where "mediatype=\'Removable Media\'" get caption,deviceid').read()
            mod_regex = '\n([^\\\\]+)[^0-9]+([0-9]+)'
            usb_check = re.findall(mod_regex, txt)
            
            length = len(usb_check)
            
            if length > 0:
                count = 0
                usb_final = []
                for d in usb_check:
                    #if dev_os == 'Windows':
                    mod = '\\\\.\\PhysicalDrive' + d[1]
                    if mod not in ui.devs_list:
                        usb_final.append(d)
                        count = count + 1
                    #elif dev_os == 'Linux':
                    #    if d[0] not in ui.devs_list:
                    #        usb_final.append(d)
                    #        count = count + 1
                usb_check = usb_final
            
            found = False
            for entry in usb_check:
                if ui.usb_list[index2][0] == entry[0] and ui.usb_list[index2][1] == entry[1]:
                    found = True
            if not found:
                ui.msg_err('Selected USB could not be found. Setup aborted.')
                ui.op_instr.set_text('Setting up a drive includes setting a password which you can use to unlock the drive.\nEnter the new password for the drive and click \'Continue\'.')
                ui.setup_prompt1()
                return
        elif ui.DEV_OS == 'Linux':
            txt = os.popen("for DLIST in `dmesg  | grep \"Attached SCSI removable disk\" | cut -d\" \" -f 3  | sed -e 's/\[//' -e 's/\]//'` ; do\necho $DLIST\ndone ").read()
            txt_regex = 'sd[a-z]'
            list_u = re.findall(txt_regex,txt)
            for u in list_u:
                txt1 = os.popen(ui.prefix + 'mount').read()
                m = re.search(u,txt1)
                if not m:
                    txt2 = os.popen(ui.prefix + 'blkid').read()
                    rgx = u + '.+'
                    m1 = re.search(rgx,txt2)
                    if m1:
                        r2 = '\s+TYPE="([a-z]+)"'
                        txt3 = m1.group(0)
                        m2 = re.search(r2,txt3)
                        type_a = m2.group(1)
                        s = os.system(ui.prefix + 'mount -t ' + type_a + ' /dev/' + u + '1')
        
        
        
        ui.start_spin()
        ui.setup_wait_instr.show()
        ui.progress_bar.set_fraction(0.0)
        ui.progress_bar.show()
        
        ui.op_inprogress = True
        ui.pbawrite_ip = True
        
        
        t_m = threading.Thread(target=runsetup.rt_setupFull, args=(ui, selected_list, index2, preserved_files))
        t_m.start()
            
    else:
        ui.op_instr.set_text('Setting up a drive includes setting a password which you can use to unlock the drive.\nEnter the new password for the drive and click \'Continue\'.')
        message2.destroy()

def run_pbaWrite(button, ui):
    verify.licCheck(ui)
    status = -1
    password = ""
    
    dev_idx = -1
    
    pw_strip = re.sub('\s', '', ui.pass_entry.get_text())
    
    if ui.pass_entry.get_text() == '' and (ui.VERSION != 3 or not ui.check_pass_rd.get_active()):
        ui.msg_err('Enter the password.')
        return
    elif pw_strip == '' and (ui.VERSION != 3 or not ui.check_pass_rd.get_active()):
        ui.msg_err('Invalid password. Passwords must have non-whitespace characters.')
        return
    if (ui.VERSION % 3 == 0 or (ui.VERSION == 1 and ui.PBA_VERSION != 1)) and ui.pass_sav.get_active() and len(ui.drive_list) == 0:
        ui.msg_err('No USB selected for Save to USB.')
        return
    if (ui.VERSION % 3 == 0 or (ui.VERSION == 1 and ui.PBA_VERSION != 1)) and ui.pass_sav.get_active():
        drive = ui.drive_menu.get_active_text()
        
        if ui.DEV_OS == 'Windows':
            drive = drive + '\\'
        if not os.path.isdir(drive):
            ui.msg_err('Selected USB could not be detected.')
            return
        
    
    selected_list = []
    if ui.toggleSingle_radio.get_active():
        dev_idx = ui.dev_select.get_active()
        selected_list = [dev_idx]
        #print selected_list
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
        if len(selected_list) == 0:
            ui.msg_err('No drives selected.')
            return
            
    if ui.VERSION == 3 and ui.check_pass_rd.get_active():
        devs = []
        list_mn = []
        list_sn = []
        for a in selected_list:
            devs.append(ui.devs_list[a])
            list_mn.append(ui.vendor_list[a])
            list_sn.append(ui.sn_list[a])
        d_list = runprocess.findUSB('Admin', devs, list_mn, list_sn)
        drive_list = d_list[1]
        f_list = d_list[0]
        if len(drive_list) == 0:
            ui.msg_err('No USB detected.')
            return
        elif len(f_list) == 0:
            ui.msg_err('No password files found on USB.')
            return
        elif d_list[2] != '':
            ui.msg_err('Admin password not found for ' + d_list[2] + '.')
            return
        
        
    ui.start_spin()
    ui.pba_wait_instr.show()
    ui.progress_bar.set_fraction(0.0)
    ui.progress_bar.show()
    
    
    
    t_m = threading.Thread(target=runsetup.rt_pbaWrite, args=(ui, selected_list,))
    t_m.start()
        
def run_changePW(button, ui):
    verify.licCheck(ui)
    pw_strip = re.sub('\s', '', ui.pass_entry.get_text())
    if (ui.pass_entry.get_text() == '' and (ui.VERSION != 3 or not ui.check_pass_rd.get_active())) or ui.new_pass_entry.get_text() == '' or ui.confirm_pass_entry.get_text() == '':
        ui.msg_err('Enter the current password, new password, and confirm the new password.')
        return
    elif pw_strip == '' and (ui.VERSION != 3 or not ui.check_pass_rd.get_active()):
        ui.msg_err('Invalid password. Passwords must have non-whitespace characters.')
        return
    if ui.VERSION == 3 and ui.pass_sav.get_active() and not ui.check_pass_rd.get_active() and len(ui.drive_list) == 0:
        ui.msg_err('No USB selected for Save to USB.')
        return
    devs = []
    list_mn = []
    list_sn = []
    selected_list = []
    if ui.toggleSingle_radio.get_active():
        index = ui.dev_select.get_active()
        selected_list = [index]
        devs = [ui.devs_list[index]]
        list_mn = [ui.vendor_list[index]]
        list_sn = [ui.sn_list[index]]
    else:
        index = 0
        iter = ui.liststore.get_iter_first()
        while iter != None:
            selected = ui.liststore.get_value(iter, 0)
            if selected:
                if ui.auth_menu.get_active() == 0:
                    selected_list.append(index)
                    devs.append(ui.devs_list[index])
                    list_mn.append(ui.vendor_list[index])
                    list_sn.append(ui.sn_list[index])
                else:
                    selected_list.append(index)
                    devs.append(ui.devs_list[index])
                    list_mn.append(ui.vendor_list[index])
                    list_sn.append(ui.sn_list[index])
            iter = ui.liststore.iter_next(iter)
            index = index + 1
        if len(selected_list) == 0:
            ui.msg_err('No drives selected.')
            return
            
    if ui.VERSION == 3 and ui.check_pass_rd.get_active():
        f_list = runprocess.findUSB(ui.auth_menu.get_active_text(), devs, list_mn, list_sn)
        if len(f_list[1]) == 0:
            ui.msg_err('No USB detected.')
            return
        elif len(f_list[0]) == 0:
            ui.msg_err('No password files on USB.')
            return
        elif f_list[2] != '':
            if ui.auth_menu.get_active() == 0:
                ui.msg_err('Admin password not found for ' + f_list[2] + '.')
            else:
                ui.msg_err('User password not found for ' + f_list[2] + '.')
            return
    old_hash = ""
    new_pass = ui.new_pass_entry.get_text()
    new_pass_confirm = ui.confirm_pass_entry.get_text()
    
    pw_trim = re.sub('\s', '', new_pass)
    pw_trim_confirm = re.sub(r'\s+', '', new_pass_confirm)
    if len(pw_trim) < 8:
        ui.msg_err("The new password is too short.  Please enter a password at least 8 characters long excluding whitespace.")
        return
    elif ui.bad_pw.has_key(pw_trim):
        ui.msg_err("The new password is on the blacklist of weak passwords, please enter a stronger password.")
        return
    elif new_pass != new_pass_confirm:
        ui.msg_err("The new entered passwords do not match.")
        return
    elif (ui.VERSION % 3 == 0 or (ui.VERSION == 1 and ui.PBA_VERSION != 1)) and ui.pass_sav.get_active() and not ui.check_pass_rd.get_active():
        drive = ui.drive_menu.get_active_text()
        
        if ui.DEV_OS == 'Windows':
            drive = drive + "\\"
        if not os.path.isdir(drive):
            ui.msg_err('Selected USB could not be detected.')
            return
    ui.start_spin()
    if len(selected_list) == 1:
        ui.wait_instr.show()
    else:
        ui.multi_wait_instr.show()
    level = ui.auth_menu.get_active()
    ui.op_inprogress = True
    
    
    t_m = threading.Thread(target=runsetup.rt_changePW, args=(ui, selected_list, level))
    t_m.start()

def run_setupUSB(button, ui, *args):
    verify.licCheck(ui)
    pw_strip = re.sub('\s', '', ui.pass_entry.get_text())
    if ((ui.VERSION % 3 == 0 or (ui.VERSION == 1 and ui.PBA_VERSION != 1)) and ui.pass_sav.get_active()) and ui.pass_entry.get_text() == '' and not ui.check_pass_rd.get_active():
        ui.msg_err('Enter the password.')
        return
    elif ((ui.VERSION % 3 == 0 or (ui.VERSION == 1 and ui.PBA_VERSION != 1)) and ui.pass_sav.get_active()) and pw_strip == '' and not ui.check_pass_rd.get_active():
        ui.msg_err('Invalid password. Passwords must have non-whitespace characters.')
        return
    usb_dialog = dialogs.USBDialog(ui)
    res = usb_dialog.run()
    if res == gtk.RESPONSE_OK:
        if usb_dialog.usb_menu.get_active() >= 0:
            message = gtk.MessageDialog(type=gtk.MESSAGE_WARNING, buttons=gtk.BUTTONS_YES_NO, parent = ui)
            if ui.VERSION == 3 or (ui.VERSION == 1 and ui.PBA_VERSION != 1):
                message.set_markup("Warning: You will lose any files you have on the USB (except for any previously saved password files). Do you want to proceed?")
            else:
                message.set_markup("Warning: You will lose any files you have on the USB. Do you want to proceed?")
            
            res = message.run()
            if res == gtk.RESPONSE_YES:
                message.destroy()
                
                preserved_files = []
                
                idx1 = ui.dev_select.get_active()
                dev1 = ui.devs_list[idx1]
                index = ui.dev_select.get_active()
                index2 = usb_dialog.usb_menu.get_active()
                usb_dialog.destroy()
                
                
                usb_check = []
                
                if ui.DEV_OS == 'Windows':
                    #print 'before wmic'
                    txt = os.popen(ui.prefix + 'wmic diskdrive where "mediatype=\'Removable Media\'" get caption,deviceid').read()
                    #print 'after wmic'
                    #mod_regex = 'DeviceID=.+([1-9]|1[0-5])\s*\nModel=(.*)\r'
                    mod_regex = '\n([^\\\\]+)[^0-9]+([0-9]+)'
                    usb_check = re.findall(mod_regex, txt)
                    if len(usb_check) == 0:
                        ui.msg_err('No USB detected.')
                        ui.setupUSB_prompt()
                        return
                    found = False
                    for entry in usb_check:
                        if ui.usb_list[index2][0] == entry[0] and ui.usb_list[index2][1] == entry[1]:
                            found = True
                    if not found:
                        ui.msg_err('Selected USB could not be found.')
                        ui.setupUSB_prompt()
                        return
                elif ui.DEV_OS == 'Linux':
                    txt = os.popen("for DLIST in `dmesg  | grep \"Attached SCSI removable disk\" | cut -d\" \" -f 3  | sed -e 's/\[//' -e 's/\]//'` ; do\necho $DLIST\ndone ").read()
                    txt_regex = 'sd[a-z]'
                    list_u = re.findall(txt_regex,txt)
                    for u in list_u:
                        txt1 = os.popen(ui.prefix + 'mount').read()
                        m = re.search(u,txt1)
                        if not m:
                            txt2 = os.popen(ui.prefix + 'blkid').read()
                            rgx = u + '.+'
                            m1 = re.search(rgx,txt2)
                            if m1:
                                r2 = '\s+TYPE="([a-z]+)"'
                                txt3 = m1.group(0)
                                m2 = re.search(r2,txt3)
                                type_a = m2.group(1)
                                s = os.system(ui.prefix + 'mount -t ' + type_a + ' /dev/' + u + '1')
                    txt3 = os.popen(ui.prefix + 'mount').read()
                    dev_regex3 = '(/dev/sd[a-z][1-9]?)\s*on\s*(\S+)\s*type'
                    usb_check = re.findall(dev_regex3, txt3)
                    if len(usb_check) == 0:
                        ui.msg_err('No USB detected.')
                        ui.setupUSB_prompt()
                        return
                    found = False
                    for entry in usb_check:
                        if len(ui.usb_list) > index2 and ui.usb_list[index2][0] == entry[0] and ui.usb_list[index2][1] == entry[1]:
                            found = True
                    if not found:
                        ui.msg_err('Selected USB could not be found.')
                        ui.setupUSB_prompt()
                        return
                
                drive = ''
                if ui.DEV_OS == 'Windows':
                    for j in range(len(ui.devs_list)):
                        prelock(j)
                    p = subprocess.Popen(["diskpart"], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
                    res1 = p.stdin.write('select disk ' + ui.usb_list[index2][1] + '\n')
                    res1 = p.stdin.write('detail disk\n')
                    res1 = p.stdin.write('exit\n')
                    output = p.communicate()[0]
                    for j in range(len(ui.devs_list)):
                        postlock(j)
                    disk_regex = 'Volume [0-9]+\s+([A-Z])\s+'
                    #print output
                    m = re.search(disk_regex, output)
                    if m:
                        drive = m.group(1) + ':\\'
                    #if drive == '':
                    #    ui.msg_err('Partition could not be found to save password.')
                    #    return
                elif ui.DEV_OS == 'Linux':
                    drive = ui.usb_list[index2][1]
                if drive != '' and not os.path.isdir(drive):
                    ui.msg_err('Selected USB could not be detected')
                    ui.setupUSB_prompt()
                    return
                    
                if drive != '' and os.path.isdir(drive):
                    dir_fl = drive + 'OpalLock\\'
                    if os.path.isdir(dir_fl):
                        existingfiles = [f for f in os.listdir(dir_fl) if os.path.isfile(dir_fl + f)]
                        if len(existingfiles) > 0:
                            for p in existingfiles:
                                try:
                                    f = open(dir_fl + p, 'r')
                                    txt = f.read()
                                    f.close()
                                    tp = (dir_fl + p, txt)
                                    preserved_files.append(tp)
                                except IOError:
                                    pass
                    
                
                ui.start_spin()
                ui.wait_instr.show()
                ui.op_inprogress = True
                
                
                e = threading.Event()
                t1 = threading.Thread(target=runsetup.rt_setupUSB, args=(ui, index, index2, e, preserved_files))
                t1.start()
                start_time = time.time()
                t2 = threading.Thread(target=runthread.timeout_track, args=(ui, 120.0, start_time, [t1], None, e, None, None, cleanop.timeout_cleanup, -1, -1, -1, -1))
                t2.start()
            else:
                message.destroy()
                usb_dialog.destroy()
        else:
            ui.msg_err('No USB selected.')
            usb_dialog.destroy()
    else:
        usb_dialog.destroy()
        
def run_setupUser(button, ui):
    verify.licCheck(ui)
    pw_strip = re.sub('\s', '', ui.pass_entry.get_text())
    if ui.pass_entry.get_text() == '' and not ui.check_pass_rd.get_active():
        ui.msg_err('Enter the password.')
        return
    if pw_strip == '' and not ui.check_pass_rd.get_active():
        ui.msg_err('Invalid password. Passwords must have non-whitespace characters.')
        return
    if ui.new_pass_entry.get_text() == '' or ui.confirm_pass_entry.get_text() == '':
        ui.msg_err('Enter and confirm the new User password.')
        return
    if ui.pass_sav.get_active() and len(ui.drive_list) == 0:
        ui.msg_err('No USB selected for Save to USB.')
        return
    index = ui.dev_select.get_active()
    ui.devname = ui.devs_list[index]
    #if ui.check_pass_rd.get_active():
    #    sl = [ui.devname]
    #    mn = [ui.vendor_list[index]]
    #    sn = [ui.sn_list[index]]
    #    f_list = runprocess.findUSB("Admin", sl, mn, sn)
    #    if len(f_list[1]) == 0:
    #        ui.msg_err('No USB detected.')
    #        return
    #    elif len(f_list[0]) == 0:
    #        ui.msg_err('No password files found on USB.')
    #        return
    #    elif f_list[2] != '':
    #        ui.msg_err('Admin password not found for ' + f_list[2] + '.')
    #        return
    if ui.user_list[index] == '1':
        ui.msg_err('User setup is not supported on this drive')
        return
    msid = ui.msid_list[index]
    pw_u = ui.new_pass_entry.get_text()
    pw_u_confirm = ui.confirm_pass_entry.get_text()
    pw_u_trim = re.sub('\s', '', pw_u)
    pw_u_trim_confirm = re.sub(r'\s+', '', pw_u_confirm)
    if len(pw_u_trim) < 8:
        ui.msg_err("This password is too short.  Please enter a password at least 8 characters long excluding whitespace.")
        return
    elif ui.bad_pw.has_key(pw_u_trim):
        ui.msg_err("This password is on the blacklist of bad passwords, please enter a stronger password.")
        return
    elif pw_u != pw_u_confirm:
        ui.msg_err("The entered passwords do not match.")
        return
    
    
    
    ui.start_spin()
    ui.wait_instr.show()
    ui.op_inprogress = True
    
    
                
    e = threading.Event()
    t1 = threading.Thread(target=runsetup.rt_setupUser, args=(ui, index, e))
    t1.start()
    start_time = time.time()
    t2 = threading.Thread(target=runthread.timeout_track, args=(ui, 60.0, start_time, [t1], None, e, None, None, cleanop.timeout_cleanup, -1, -1, -1, -1))
    t2.start()
        
def run_removeUser(button, ui):
    verify.licCheck(ui)
    pw_strip = re.sub('\s', '', ui.pass_entry.get_text())
    if ui.pass_entry.get_text() == '' and not ui.check_pass_rd.get_active():
        ui.msg_err('Enter the password.')
        return
    if pw_strip == '' and not ui.check_pass_rd.get_active():
        ui.msg_err('Invalid password. Passwords must have non-whitespace characters.')
        return
    if ui.pass_sav.get_active() and len(ui.drive_list) == 0:
        ui.msg_err('No USB selected for Save to USB.')
        return
    index = ui.dev_select.get_active()
    ui.devname = ui.devs_list[index]
    
    if ui.user_list[index] == '1':
        ui.msg_err('User removal is not supported on this drive')
        return
    #if ui.check_pass_rd.get_active():
    #    sl = [ui.devname]
    #    mn = [ui.vendor_list[index]]
    #    sn = [ui.sn_list[index]]
    #    f_list = runprocess.findUSB("Admin",sl, mn, sn)
    #    if len(f_list[1]) == 0:
    #        ui.msg_err('No USB detected.')
    #        return
    #    elif len(f_list[0]) == 0:
    #        ui.msg_err('No password files found on USB.')
    #        return
    #    elif f_list[2] != '':
    #        ui.msg_err('Admin password not found for ' + f_list[2] + '.')
    #        return
    msid = ui.msid_list[index]
    pw = ui.pass_entry.get_text()
    pw_trim = re.sub('\s', '', pw)
    password_a = ''
    
    if (ui.VERSION % 3 == 0 or (ui.VERSION == 1 and ui.PBA_VERSION != 1)) and ui.pass_sav.get_active():
        drive = ui.drive_menu.get_active_text()
        
        if ui.DEV_OS == 'Windows':
            drive = drive + '\\'
        if not os.path.isdir(drive):
            ui.msg_err('Selected USB could not be detected')
            return
            
    #if (ui.VERSION % 3 == 0 or (ui.VERSION == 1 and ui.PBA_VERSION != 1)) and ui.check_pass_rd.get_active():
    #    f_list = runprocess.findUSB(ui)
    #    if len(f_list) == 0:
    #        ui.msg_err('No USB detected.')
    #        return
    
    ui.start_spin()
    ui.wait_instr.show()
    ui.op_inprogress = True
    
    
                
    e = threading.Event()
    t1 = threading.Thread(target=runsetup.rt_removeUser, args=(ui, index, e))
    t1.start()
    start_time = time.time()
    t2 = threading.Thread(target=runthread.timeout_track, args=(ui, 60.0, start_time, [t1], None, e, None, None, cleanop.timeout_cleanup, -1, -1, -1, -1))
    t2.start()
        
def run_unlockPBA(button, ui, reboot, autounlock, msg):
    verify.licCheck(ui)
    if msg:
        msg.destroy()
    selected_list = []
    devs = []
    list_mn = []
    list_sn = []
    if not autounlock:
        pw_strip = re.sub('\s', '', ui.pass_entry.get_text())
        
        if ui.pass_entry.get_text() == '' and not ((ui.VERSION % 3 == 0 or (ui.VERSION == 1 and ui.PBA_VERSION != 1)) and ui.check_pass_rd.get_active()):
            ui.msg_err('Enter the password.')
            return
        if pw_strip == '' and not ((ui.VERSION % 3 == 0 or (ui.VERSION == 1 and ui.PBA_VERSION != 1)) and ui.check_pass_rd.get_active()):
            ui.msg_err('Invalid password. Passwords must have non-whitespace characters.')
            return
        #print ui.VERSION
        #print ui.PBA_VERSION
        #print ui.drive_list
        if (ui.VERSION % 3 == 0 or (ui.VERSION == 1 and ui.PBA_VERSION != 1)) and ui.pass_sav.get_active() and len(ui.drive_list) == 0:
            ui.msg_err('No USB selected for Save to USB.')
            return
        if (ui.VERSION % 3 == 0 or (ui.VERSION == 1 and ui.PBA_VERSION != 1)) and ui.pass_sav.get_active():
            drive = ui.drive_menu.get_active_text()
            
            if ui.DEV_OS == 'Windows':
                drive = drive + '\\'
            if not os.path.isdir(drive):
                ui.msg_err('Selected USB not detected')
                return
        selected_list = []
        if ui.toggleSingle_radio.get_active():
            index = ui.dev_select.get_active()
            selected_list = [index]
            devs = [ui.devs_list[index]]
            list_mn = [ui.dev_vendor.get_text()]
            list_sn = [ui.dev_sn.get_text()]
        else:
            index = 0
            iter = ui.liststore.get_iter_first()
            while iter != None:
                selected = ui.liststore.get_value(iter, 0)
                if selected:
                    if ui.auth_menu.get_active() == 0:
                        selected_list.append(index)
                        devs.append(ui.devs_list[index])
                        list_mn.append(ui.vendor_list[index])
                        list_sn.append(ui.sn_list[index])
                    else:
                        selected_list.append(index)
                        devs.append(ui.devs_list[index])
                        list_mn.append(ui.vendor_list[index])
                        list_sn.append(ui.sn_list[index])
                iter = ui.liststore.iter_next(iter)
                index = index + 1
            if len(selected_list) == 0:
                ui.msg_err('No drives selected.')
                return
                
        if (ui.VERSION % 3 == 0 or (ui.VERSION == 1 and ui.PBA_VERSION != 1)) and ui.check_pass_rd.get_active():
            f_list = runprocess.findUSB(ui.auth_menu.get_active_text(), devs, list_mn, list_sn)
            if len(f_list[1]) == 0:
                ui.msg_err('No USB detected.')
                return
            elif len(f_list[0]) == 0:
                ui.msg_err('No password files found on USB.')
                return
            elif f_list[2] != '':
                if ui.auth_menu.get_active() == 0:
                    ui.msg_err('Admin password not found for ' + f_list[2] + '.')
                else:
                    ui.msg_err('User password not found for ' + f_list[2] + '.')
                return
        ui.LKATTR = "RW"
        ui.start_spin()
        if len(selected_list) == 1:
            ui.wait_instr.show()
        else:
            ui.multi_wait_instr.show()
        ui.op_inprogress = True
    else:
        selected_list = ui.locked_list
        folder_list = []
        
        if ui.DEV_OS == 'Windows':
            txt = os.popen('wmic logicaldisk where "drivetype=2" get caption,filesystem').read()
            txt_regex = '([A-Z]:)\s+FAT'
            drive_list = re.findall(txt_regex,txt)
            for d in drive_list:
                if os.path.isdir('%s\\OpalLock' % d):
                    folder_list.append(d)
        elif ui.DEV_OS == 'Linux':
            txt = os.popen(ui.prefix + 'mount').read()
            dev_regex = '/dev/sd[a-z][1-9]?\s*on\s*(\S+)\s*type'
            drive_list = re.findall(dev_regex, txt)
            txt2 = os.popen(ui.prefix + 'blkid').read()
            dev_regex2 = '(/dev/sd[a-z][1-9]?.+)'
            all_list = re.findall(dev_regex2, txt2)
            r1 = '/dev/sd[a-z][1-9]?'
            r2 = '\s+TYPE="([^"]+)"'
            for a in all_list:
                m1 = re.search(r1,a)
                m2 = re.search(r2,a)
                if m2:
                    dev_a = m1.group(0)
                    type_a = m2.group(1)
                    if dev_a not in drive_list:
                        
                        s = os.system(ui.prefix + 'mount -t ' + type_a + ' ' + dev_a)
                        
            txt = os.popen(ui.prefix + 'mount').read()
            dev_regex = '/dev/sd[a-z][1-9]?\s*on\s*(\S+)\s*type'
            drive_list = re.findall(dev_regex, txt)
            for d in drive_list:
                if os.path.isdir('%s/OpalLock' % d):
                    folder_list.append(d)
        if len(drive_list) == 0:
            ui.msg_err('No USB drives detected.')
            #ui.unlock_prompt()
            return
        elif len(folder_list) == 0:
            ui.msg_err('No password files found, check to make sure the USB is mounted.')
            #ui.unlock_prompt()
            return
        else:
            ui.start_spin()
            ui.wait_instr.show()
    
    
        
    t_m = threading.Thread(target=runthread.rt_unlockPBA, args=(ui, selected_list, reboot, autounlock))
    t_m.start()
    
def run_revertKeep(button, ui):
    verify.licCheck(ui)
    pw_strip = re.sub('\s', '', ui.pass_entry.get_text())
    
    if ui.pass_entry.get_text() == '' and not ((ui.VERSION % 3 == 0 or (ui.VERSION == 1 and ui.PBA_VERSION != 1)) and ui.check_pass_rd.get_active()):
        ui.msg_err('Enter the password.')
        return
    if pw_strip == '' and not ((ui.VERSION % 3 == 0 or (ui.VERSION == 1 and ui.PBA_VERSION != 1)) and ui.check_pass_rd.get_active()):
        ui.msg_err('Invalid password. Passwords must have non-whitespace characters.')
        return
    selected_list = []
    devs = []
    list_mn = []
    list_sn = []
    if ui.toggleSingle_radio.get_active():
        index = ui.dev_select.get_active()
        selected_list = [index]
        devs = [ui.devs_list[index]]
        list_mn = [ui.vendor_list[index]]
        list_sn = [ui.sn_list[index]]
    else:
        index = 0
        selected_list = []
        iter = ui.liststore.get_iter_first()
        while iter != None:
            selected = ui.liststore.get_value(iter, 0)
            if selected:
                selected_list.append(index)
                devs.append(ui.devs_list[index])
                list_mn.append(ui.vendor_list[index])
                list_sn.append(ui.sn_list[index])
            iter = ui.liststore.iter_next(iter)
            index = index + 1
        if len(selected_list) == 0:
            ui.msg_err('No drives selected.')
            return
            
    if (ui.VERSION % 3 == 0 or (ui.VERSION == 1 and ui.PBA_VERSION != 1)) and ui.check_pass_rd.get_active():
        f_list = runprocess.findUSB("Admin", devs, list_mn, list_sn)
        if len(f_list[1]) == 0:
            ui.msg_err('No USB detected.')
            return
        elif len(f_list[0]) == 0:
            ui.msg_err('No password files found on USB.')
            return
        elif f_list[2] != '':
            ui.msg_err('Admin password not found for ' + f_list[2] + '.')
            return
    password = ''
    
    ui.start_spin()
    if len(selected_list) == 1:
        ui.wait_instr.show()
    else:
        ui.multi_wait_instr.show()
    ui.op_inprogress = True
    
    
    t_m = threading.Thread(target=runthread.rt_revertKeep, args=(ui, selected_list,))
    t_m.start()
        
def run_revertErase(button, ui):
    verify.licCheck(ui)
    pw_strip = re.sub('\s', '', ui.pass_entry.get_text())
    
    if ui.pass_entry.get_text() == '' and not ((ui.VERSION % 3 == 0 or (ui.VERSION == 1 and ui.PBA_VERSION != 1)) and ui.check_pass_rd.get_active()):
        ui.msg_err('Enter the password.')
        return
    if pw_strip == '' and not ((ui.VERSION % 3 == 0 or (ui.VERSION == 1 and ui.PBA_VERSION != 1)) and ui.check_pass_rd.get_active()):
        ui.msg_err('Invalid password. Passwords must have non-whitespace characters.')
        return
    
    text_a = ui.revert_agree_entry.get_text()
    if text_a.lower() != 'i agree':
        ui.msg_err("Type 'I agree' into the entry box")
        return
    if not ui.warned:
        message = gtk.MessageDialog(type=gtk.MESSAGE_WARNING, buttons=gtk.BUTTONS_YES_NO, parent = ui)
        message.set_markup("Warning: Are you sure you want to revert setup and erase this drive's data?\nYou will not be able to recover your data after it is deleted.")
        
        res = message.run()
        if res == gtk.RESPONSE_YES:
            message.destroy()
            ui.warned = True
            ui.orig = ui.pass_entry.get_text()
            if ui.orig == '':
                ui.pass_entry.set_sensitive(False)
            ui.check_pass_rd.set_sensitive(False)
            ui.pass_entry.get_buffer().delete_text(0,-1)
            ui.revert_agree_entry.get_buffer().delete_text(0,-1)
            if ui.VERSION == 3 or (ui.VERSION == 1 and ui.PBA_VERSION != 1):
                ui.op_instr.set_text('Re-enter the password to verify that you want to erase data for the drive(s).')
            else:
                ui.op_instr.set_text('Re-enter the password to verify that you want to erase data for the drive.')
            #ui.dev_single.show()
            #ui.label_dev2.show()
            #ui.dev_select.hide()
            #ui.label_dev.hide()
            
        else:
            message.destroy()
            
    else :
        ui.warned = False
        if ui.orig != ui.pass_entry.get_text():
            ui.orig = ''
            ui.msg_err('The passwords entered do not match')
            ui.revert_erase_prompt()
            return
        ui.orig = ''
        messageA = gtk.MessageDialog(type=gtk.MESSAGE_WARNING, buttons=gtk.BUTTONS_YES_NO, parent = ui)
        if ui.VERSION == 3 or (ui.VERSION == 1 and ui.PBA_VERSION != 1):
            messageA.set_markup("Final Warning: Are you absolutely sure you want to proceed with removing lock and erasing all data from the selected drive(s)?")
        else:
            messageA.set_markup("Final Warning: Are you absolutely sure you want to proceed with removing lock and erasing all data from the selected drive?")
        resA = messageA.run()

        if resA == gtk.RESPONSE_YES : 
            messageA.destroy()
            
            
            selected_list = []
            devs = []
            list_mn = []
            list_sn = []
            if ui.toggleSingle_radio.get_active():
                index = ui.dev_select.get_active()
                selected_list = [index]
                devs = [ui.devs_list[index]]
                list_mn = [ui.vendor_list[index]]
                list_sn = [ui.sn_list[index]]
            else:
                index = 0
                selected_list = []
                iter = ui.liststore.get_iter_first()
                while iter != None:
                    selected = ui.liststore.get_value(iter, 0)
                    if selected:
                        selected_list.append(index)
                        devs.append(ui.devs_list[index])
                        list_mn.append(ui.vendor_list[index])
                        list_sn.append(ui.sn_list[index])
                    iter = ui.liststore.iter_next(iter)
                    index = index + 1
                if len(selected_list) == 0:
                    ui.msg_err('No drives selected.')
                    ui.revert_erase_prompt()
                    return
                    
            list_remove = []
            orig_len = len(selected_list)
            for x in range(len(selected_list)):
                drive_na = 'LockingEnabled = N'
                prelock(selected_list[x])
                txt_q = os.popen(ui.prefix + 'sedutil-cli --query ' + ui.devs_list[selected_list[x]]).read()
                postlock(selected_list[x])
                m_na = re.search(drive_na, txt_q)
                if m_na:
                    list_remove.append(x)
            if len(list_remove) > 0:
                start = True
                list_d = ''
                y = len(list_remove) - 1
                while y >= 0:
                    if not start:
                        list_d = list_d + ', '
                    else:
                        start = False
                    list_d = list_d + devs[list_remove[y]]
                    selected_list.pop(list_remove[y])
                    devs.pop(list_remove[y])
                    list_mn.pop(list_remove[y])
                    list_sn.pop(list_remove[y])
                    y = y - 1
                if len(list_remove) > 1:
                    ui.msg_err('Unable to erase ' + list_d + ' because the drives have not been activated with Opal Lock. Please set up the drives before proceeding.')
                else:
                    ui.msg_err('Unable to erase ' + list_d + ' because the drive has not been activated with Opal Lock. Please set up the drive before proceeding.')
                    
            if len(selected_list) == 0:
                ui.revert_erase_prompt()
                return
                    
            if (ui.VERSION % 3 == 0 or (ui.VERSION == 1 and ui.PBA_VERSION != 1)) and ui.check_pass_rd.get_active():
                f_list = runprocess.findUSB("Admin", devs, list_mn, list_sn)
                if len(f_list[1]) == 0:
                    ui.msg_err('No USB detected.')
                    return
                elif len(f_list[0]) == 0:
                    ui.msg_err('No password files found on USB.')
                    return
                elif f_list[2] != '':
                    ui.msg_err('Admin password not found for ' + f_list[2] + '.')
                    return
            ui.start_spin()
            if len(selected_list) == 1:
                ui.wait_instr.show()
            else:
                ui.multi_wait_instr.show()
            ui.op_inprogress = True
            password = ""
            
            
            t_m = threading.Thread(target=runthread.rt_revertErase, args=(ui, selected_list,))
            t_m.start()
        else:
            messageA.destroy()
        
def run_revertPSID(button, ui):
    verify.licCheck(ui)
    if ui.revert_psid_entry.get_text() == '':
        ui.msg_err('Enter the PSID.')
        return
    text_a = ui.revert_agree_entry.get_text()
    if text_a.lower() != 'i agree':
        ui.msg_err("Type 'I agree' into the entry box")
        return
        
    txt = ui.revert_psid_entry.get_text()
    psid = txt.strip('\r\n\0\s')

    if len(psid) != 32:
        ui.msg_err('The PSID is a 32-character string located on the drive label.')
        return
    
    
    index = ui.dev_select.get_active()
    ui.devname = ui.devs_list[index]
    
    drive_na = 'LockingEnabled = N'
    prelock(index)
    txt_q = os.popen(ui.prefix + 'sedutil-cli --query ' + ui.devname).read()
    postlock(index)
    m_na = re.search(drive_na, txt_q)
    if m_na:
        ui.msg_err('This drive has not been activated by Opal Lock. Please set up the drive before proceeding.')
        ui.revert_psid_prompt()
        return
    
    if not ui.warned:
        message = gtk.MessageDialog(type=gtk.MESSAGE_WARNING, buttons=gtk.BUTTONS_YES_NO, parent = ui)
        message.set_markup("Warning: Are you sure you want to revert setup for " + ui.devname + " and erase all of its data?\nYou will not be able to recover your data after it is deleted.")
        
        res = message.run()
        if res == gtk.RESPONSE_YES:
            message.destroy()
            ui.warned = True
            ui.orig = psid
            ui.revert_psid_entry.get_buffer().delete_text(0,-1)
            ui.revert_agree_entry.get_buffer().delete_text(0,-1)
            ui.op_instr.set_text('Re-enter ' + ui.devname + '\'s PSID and type \'I agree\' to verify that you want to erase this drive\'s data.')
        else:
            message.destroy()
    else:
        ui.warned = False
        if ui.orig != psid:
            ui.orig = ''
            ui.msg_err('The PSIDs entered do not match')
            ui.revert_psid_prompt()
            return
        ui.orig = ''
        message = gtk.MessageDialog(type=gtk.MESSAGE_WARNING, buttons=gtk.BUTTONS_YES_NO, parent = ui)
        message.set_markup("Final Warning: Are you absolutely sure you want to proceed with removing lock and erasing all data for the selected drive?")
        
        res = message.run()
        if res == gtk.RESPONSE_YES :
            message.destroy()
                
            ui.start_spin()
            ui.wait_instr.show()
            ui.op_inprogress = True
            
            
            
            
            e = threading.Event()
            t1 = threading.Thread(target=runthread.rt_revertPSID, args=(ui, psid,))
            t1.start()
            start_time = time.time()
            t2 = threading.Thread(target=runthread.timeout_track, args=(ui, 60.0, start_time, [t1], None, e, None, None, cleanop.timeout_cleanup, -1, -1, -1, -1))
            t2.start()
        else:
            message.destroy()
    


