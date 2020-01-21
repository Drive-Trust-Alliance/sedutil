import gobject
import gtk
import os
import platform
import re
import runop
import runscan
from string import ascii_uppercase
if platform.system() == 'Windows':
    import subprocess
import threading
import time

DBT_DEVTYP_DEVICEINTERFACE = 0x00000005  # device interface class
DBT_DEVICEREMOVECOMPLETE = 0x8004  # device is gone
DBT_DEVICEARRIVAL = 0x8000  # system detected a new device
DBT_DEVNODES_CHANGED = 0x0007
WM_DEVICECHANGE = 0x0219

PBT_APMSUSPEND = 0x4
PBT_APMRESUMEAUTOMATIC = 0x12
WM_POWERBROADCAST = 0x0218

WM_QUERYENDSESSION = 0x11

def exitFL(_, self, *args):
    if self.DEV_OS == 'Windows':
        f = open('mountvol.txt','w')
        write_list = []
        for v in self.mv_list:
            if v[0] not in write_list:
                f.write(v[0] + ' ' + v[1] + ' ' + v[2] + '\n')
                write_list.append(v[0])
        for u in self.usb_mv_list:
            if u[0] not in write_list:
                f.write(u[0] + ' ' + u[1] + '\n')
                write_list.append(u[0])
        f.close()
        self.unhookWndProc()
    gtk.main_quit()
    
def exitX(_, self, *args):
    proceed = False
    if self.op_inprogress:
        message = gtk.MessageDialog(type=gtk.MESSAGE_WARNING, buttons=gtk.BUTTONS_YES_NO, parent = ui)
        message.set_markup("Warning: Closing Opal Lock while an operation is running may cause problems.\nAre you sure you want to close Opal Lock?")
        res = message.run()
        
        if res == gtk.RESPONSE_YES:
            proceed = True
    else:
        proceed = True
    if proceed:
        if self.DEV_OS == 'Windows':
            f = open('mountvol.txt','w')
            write_list = []
            for v in self.mv_list:
                if v[0] not in write_list:
                    f.write(v[0] + ' ' + v[1] + ' ' + v[2] + '\n')
                    write_list.append(v[0])
            for u in self.usb_mv_list:
                if u[0] not in write_list:
                    f.write(u[0] + ' ' + u[1] + '\n')
                    write_list.append(u[0])
            f.close()
        gtk.main_quit()

def reboot(_, self, *args):
    self.reboot_req = True
    if self.DEV_OS == 'Windows':
        f = open('mountvol.txt','w')
        write_list = []
        for v in self.mv_list:
            if v[0] not in write_list:
                f.write(v[0] + ' ' + v[1] + ' ' + v[2] + '\n')
                write_list.append(v[0])
        for u in self.usb_mv_list:
            if u[0] not in write_list:
                f.write(u[0] + ' ' + u[1] + '\n')
                write_list.append(u[0])
        f.close()
        self.unhookWndProc()
    gtk.main_quit()
        
def shutdown(_, self, *args):
    self.shutdown_req = True
    if self.ostype == 0 :
        f = open('mountvol.txt', 'w')
        txt = os.popen('mountvol').read()
        unmount_list = []
        for i in range(len(self.full_devs_list)):
            if self.full_isSetup_list[i] and not self.full_isLocked_list[i]:
                if self.full_devs_map[i] != -1:
                    runop.prelock(self.full_devs_map[i])
                for v in self.mv_list:
                    if v[2] == self.full_sn_list[i]:
                        regex_mounted = v[0] + '}\\\s*\n\s*([D-Z]:)'
                        v_mounted = re.search(regex_mounted,txt)
                        if v_mounted:
                            if v_mounted.group(1) not in unmount_list:
                                v[1] = v_mounted.group(1)
                                subprocess.call(['mountvol', v_mounted.group(1), '/d'])
                                unmount_list.append(v_mounted.group(1))
                if self.full_devs_map[i] != -1:
                    runop.postlock(self.full_devs_map[i])
        write_list = []
        for v in self.mv_list:
            if v[0] not in write_list:
                f.write(v[0] + ' ' + v[1] + ' ' + v[2] + '\n')
                write_list.append(v[0])
        for u in self.usb_mv_list:
            if u[0] not in write_list:
                f.write(u[0] + ' ' + u[1] + '\n')
                write_list.append(u[0])
        f.close()
        subprocess.call(['mountvol', '/n'])
        subprocess.call(['mountvol', '/r'])
    if self.DEV_OS == 'Windows':
        self.unhookWndProc()
    gtk.main_quit()
    
def hibernate(_, self, *args):
    txt = os.popen('mountvol').read()
    unmount_list = []
    for i in range(len(self.full_devs_list)):
        if self.full_isSetup_list[i] and not self.full_isLocked_list[i]:
            if self.full_devs_map[i] != -1:
                runop.prelock(self.full_devs_map[i])
            for v in self.mv_list:
                if v[2] == self.full_sn_list[i]:
                    regex_mounted = v[0] + '}\\\s*\n\s*([D-Z]:)'
                    v_mounted = re.search(regex_mounted,txt)
                    if v_mounted:
                        if v_mounted.group(1) not in unmount_list:
                            v[1] = v_mounted.group(1)
                            subprocess.call(['mountvol', v_mounted.group(1), '/d'])
                            unmount_list.append(v_mounted.group(1))
            if self.full_devs_map[i] != -1:
                runop.postlock(self.full_devs_map[i])
    subprocess.call(['mountvol', '/n'])
    subprocess.call(['mountvol', '/r'])
    subprocess.call(['shutdown', '-h'])

def devChange(self, wParam,lParam):
    #print "WM_DEVICECHANGE [WPARAM:%i][LPARAM:%i]"%(wParam,lParam)
    
    if wParam == DBT_DEVNODES_CHANGED:
        if not self.dnc_ip:
            self.dnc_ip = True
            #print 'got in'
            #def t_run():
            time.sleep(1)
            if not self.drc_ip and not self.da_ip:
                txt = os.popen('mountvol').read()
                regex_nm = '([a-z0-9]{8}-[a-z0-9]{4}-[a-z0-9]{4}-[a-z0-9]{4}-[a-z0-9]{12})}\\\s*\n\s*\*'
                nm_list = re.findall(regex_nm, txt)
                for v in nm_list:
                    done = False
                    mv_idx = 0
                    for e in self.mv_list:
                        if not done:
                            if e[0] == v:
                                if not os.path.isdir(e[1]):
                                    #print 'mountvol ' + e[1] + ' \\\\?\\Volume{' + e[0] + '}\\'
                                    subprocess.call(['mountvol', e[1], '\\\\?\\Volume{' + e[0] + '}\\'])
                                else:
                                    dir_idx = 3
                                    done = False
                                    while not done:
                                        dir = ascii_uppercase[dir_idx] + ':'
                                        if not os.path.isdir(dir):
                                            listed = False
                                            for v0 in self.mv_list:
                                                if v0[1] == dir:
                                                    listed = True
                                            if not listed:
                                                #print 'mountvol ' + dir + ' \\\\?\\Volume{' + e[0] + '}\\'
                                                self.dnc_mount = True
                                                subprocess.call(['mountvol', dir, '\\\\?\\Volume{' + e[0] + '}\\'])
                                                self.mv_list[mv_idx][1] = dir
                                                done = True
                                        if not done:
                                            dir_idx = dir_idx + 1
                                            if dir_idx == 26:
                                                done = True
                                done = True
                        mv_idx = mv_idx + 1
                #gobject.idle_add(cleanup,)
                        
                    
            #def cleanup():#rescan_needed):
            self.dnc_ip = False
            self.drc_ip = False
            self.da_ip = False
            #t = threading.Thread(target=t_run, args=())
            #t.start()
            

    if wParam == DBT_DEVICEREMOVECOMPLETE:
        self.drc_ip = True
        def t_run():
            dl_len = len(self.devs_list)
            for x in range(dl_len):
                runop.prelock(x)
            subprocess.call(['mountvol', '/n'])
            subprocess.call(['mountvol', '/r'])
            txt = os.popen(self.prefix + 'sedutil-cli --scan n').read()
            for x in range(dl_len):
                runop.postlock(x)
            rgx = '(\\\\\.\\PhysicalDrive[0-9]+)\s+[12ELPR]+|No\s+\S+(?:\s+[^:]+)*\s*:\s*[^:]+\s*:\s*(\S+)'
            list_d = re.findall(rgx, txt)
            d_present = [False] * len(self.devs_list)
            for entry in list_d:
                if entry[1] in self.sn_list:
                    d_idx = self.sn_list.index(entry[1])
                    d_present[d_idx] = True
            gobject.idle_add(cleanup, d_present)
        def cleanup(d_present):
            for i in range(len(self.devs_list)):
                if not d_present[i]:
                    self.admin_aol_list[i] = 0
                    self.user_aol_list[i] = 0
            if not self.dnc_ip:
                self.drc_ip = False
        t = threading.Thread(target=t_run, args=())
        t.start()
    elif wParam == DBT_DEVICEARRIVAL and not self.scan_ip and not self.arrival_hold:
        self.da_ip = True
        def t_run():
            self.arrival_hold = True
            time.sleep(2)
            while self.pbawrite_ip or self.op_inprogress:
                time.sleep(5)
                
            self.arrival_hold = False
            dl_len = len(self.devs_list)
            for x in range(dl_len):
                runop.prelock(x)
            txt = os.popen(self.prefix + 'sedutil-cli --scan n').read()
            for x in range(dl_len):
                runop.postlock(x)
            rgx = 'PhysicalDrive([0-9]+)\s+([12ELPR]+|No)\s+\S+(?:\s+[^:]+)*\s*:\s*[^:]+\s*:(.+)'
            list_d = re.findall(rgx, txt)
            rescan_needed = False
            for i in range(len(list_d)):
                drive_num = list_d[i][0]
                drive_tcg = list_d[i][1]
                drive_ser = list_d[i][2].replace(' ', '')
                for x in range(dl_len):
                    runop.prelock(x)
                txt_q = os.popen(self.prefix + 'sedutil-cli --query \\\\.\\PhysicalDrive' + drive_num).read()
                for x in range(dl_len):
                    runop.postlock(x)
                drive_UL = 'Locked = N'
                m_UL = re.search(drive_UL, txt_q)
                if drive_tcg == 'No' or m_UL:
                    vol_list = []
                    i = -1
                    try:
                        i = self.sn_list.index(drive_ser)
                    except ValueError:
                        i = -1
                    if i != -1:
                        if i < len(self.label_list) and self.label_list[i] != '':
                            regex = '[A-Z]:'
                            vol_list = re.findall(regex, self.label_list[i])
                        #runop.prelock(i)
                        for j in range(len(self.devs_list)):
                            runop.prelock(j)
                        p = subprocess.Popen(["diskpart"], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
                        res1 = p.stdin.write('select disk ' + drive_num + '\n')
                        res1 = p.stdin.write('detail disk\n')
                        res1 = p.stdin.write('exit\n')

                        output = p.communicate()[0]
                        #runop.postlock(i)
                        for j in range(len(self.devs_list)):
                            runop.postlock(j)
                        
                        vol_re = 'Volume [0-9]+\s+([A-Z])\s+'

                        list_v = re.findall(vol_re, output)
                        for vol in list_v:
                            dir = vol + ':'
                            if dir not in vol_list:
                                vol_list.append(dir)
                                rescan_needed = True
                        for v in vol_list:
                            if v != 'C:':
                                txt_m = os.popen('mountvol ' + v + ' /l').read()
                                regex_m = '[a-z0-9]{8}-[a-z0-9]{4}-[a-z0-9]{4}-[a-z0-9]{4}-[a-z0-9]{12}'
                                m = re.search(regex_m, txt_m)
                                if m != None:
                                    exists = False
                                    diffdir = -1
                                    idx = 0
                                    for e in self.mv_list:
                                        #if e[0] == m.group(0) and e[1] == v and e[2] == self.sn_list[i]:
                                        #    exists = True
                                        #el
                                        if e[0] == m.group(0) and e[2] == self.sn_list[i]:
                                            #diffdir = idx
                                            exists = True
                                        idx = idx + 1
                                    #if diffdir >= 0:
                                        #print 'pop outdated entry'
                                        #print self.mv_list[diffdir]
                                    #    self.mv_list.pop(diffdir)
                                    if not exists:
                                        entry = [m.group(0),v,self.sn_list[i]]
                                        #print 'new entry'
                                        #print entry
                                        self.mv_list.append(entry)
            #print self.mv_list
            if not self.dnc_ip:
                self.da_ip = False
                
            gobject.idle_add(cleanup, rescan_needed)
        def cleanup(rescan_needed):
            if rescan_needed:
                runscan.run_scan(None, self, False)
        if not self.dnc_mount:
            #print 'spawning thread'
            t = threading.Thread(target=t_run, args=())
            t.start()
        else:
            #print 'not spawning thread'
            self.dnc_mount = False
    
def powerBroadcast(self, wParam,lParam):
    #print "WM_POWERBROADCAST [WPARAM:%i][LPARAM:%i]"%(wParam,lParam)
    if wParam == PBT_APMSUSPEND:
        #print "PBT_APMSUSPEND"
        txt = os.popen('mountvol').read()
        unmount_list = []
        for i in range(len(self.full_devs_list)):
            if self.full_isSetup_list[i] and not self.full_isLocked_list[i]:
                if self.full_devs_map[i] != -1:
                    runop.prelock(self.full_devs_map[i])
                for v in self.mv_list:
                    if v[2] == self.full_sn_list[i]:
                        regex_mounted = v[0] + '}\\\s*\n\s*([D-Z]:)'
                        v_mounted = re.search(regex_mounted,txt)
                        if v_mounted:
                            if v_mounted.group(1) not in unmount_list:
                                v[1] = v_mounted.group(1)
                                subprocess.call(['mountvol', v_mounted.group(1), '/d'])
                                unmount_list.append(v_mounted.group(1))
                if self.full_devs_map[i] != -1:
                    runop.postlock(self.full_devs_map[i])
        subprocess.call(['mountvol', '/n'])
        subprocess.call(['mountvol', '/r'])
                        
                        
    if wParam == PBT_APMRESUMEAUTOMATIC:
        #print "PBT_APMRESUMEAUTOMATIC"
        self.scan_ip = True
        self.posthibern = True
        def t_run():
            time.sleep(1)
            self.admin_aol_list = [0]*len(self.devs_list)
            self.user_aol_list = [0]*len(self.devs_list)
            self.psid_aol_list = [0]*len(self.devs_list)
            #if os.path.isfile('mountvol.txt'):
            #    f = open('mountvol.txt', 'r')
            #    txt = f.read()
            #    f.close()
            txt = os.popen('mountvol').read()
            mv_rm = []
            for i in range(len(self.full_devs_list)):
                num_re = '[0-9]+'
                m = re.search(num_re, self.full_devs_list[i])
                if m:
                    #if self.full_devs_map[i] != -1:
                    #    runop.prelock(self.full_devs_map[i])
                    for j in range(len(self.devs_list)):
                        runop.prelock(j)
                    p = subprocess.Popen(["diskpart"], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
                    res1 = p.stdin.write('select disk ' + m.group(0) + '\n')
                    res1 = p.stdin.write('detail disk\n')
                    res1 = p.stdin.write('exit\n')
                    

                    output = p.communicate()[0]
                    #if self.full_devs_map[i] != -1:
                    #    runop.postlock(self.full_devs_map[i])
                    for j in range(len(self.devs_list)):
                        runop.postlock(j)
                    vol_re = 'Volume [0-9]+\s+([A-Z])\s+'

                    list_v = re.findall(vol_re, output)
                    if self.full_isSetup_list[i]:# and not self.full_isLocked_list[i]:
                        if self.full_devs_map[i] != -1:
                            runop.prelock(self.full_devs_map[i])
                        txt_q = os.popen(self.prefix + 'sedutil-cli --query ' + self.full_devs_list[i]).read()
                        if self.full_devs_map[i] != -1:
                            runop.postlock(self.full_devs_map[i])
                        regex_locked = 'Locked\s*=\s*Y'
                        m = re.search(regex_locked, txt_q)
                        if not m:
                            #regex = '([a-z0-9]{8}-[a-z0-9]{4}-[a-z0-9]{4}-[a-z0-9]{4}-[a-z0-9]{12})\s+([A-Z]:)\s+' + self.sn_list[i]
                            #vol_list = re.findall(regex, txt)
                            #for m in vol_list:
                            mv_idx = 0
                            for v in self.mv_list:
                                if v[2] == self.full_sn_list[i]:
                                #if not os.path.isdir(m[1]):
                                    n = re.search(v[0],txt)
                                    if n:
                                        regex_mounted = v[0] + '}\\\s*\n\s*([A-Z]):'
                                        v_mounted = re.search(regex_mounted,txt)
                                        if not v_mounted:
                                            if not os.path.isdir(v[1]):
                                                #print 'mountvol ' + v[1] + ' \\\\?\\Volume{' + v[0] + '}\\'
                                                subprocess.call(['mountvol', v[1], '\\\\?\\Volume{' + v[0] + '}\\'])
                                            else:
                                                dir_idx = 3
                                                done = False
                                                while not done:
                                                    dir = ascii_uppercase[dir_idx] + ':'
                                                    if not os.path.isdir(dir):
                                                        listed = False
                                                        for v0 in self.mv_list:
                                                            if v0[1] == dir:
                                                                listed = True
                                                        if not listed:
                                                            #print 'mountvol ' + dir + ' \\\\?\\Volume{' + v[0] + '}\\'
                                                            subprocess.call(['mountvol', dir, '\\\\?\\Volume{' + v[0] + '}\\'])
                                                            #mv_edit.append(mv_idx)
                                                            #mv_newdir.append(dir)
                                                            self.mv_list[mv_idx][1] = dir
                                                            done = True
                                                    if not done:
                                                        dir_idx = dir_idx + 1
                                                        if dir_idx == 26:
                                                            done = True
                                        else:
                                            if v_mounted.group(1) not in list_v:
                                                mv_rm.append(mv_idx)
                                                #print 'marked for removal because this partition is already mounted but is not listed for the drive'
                                                #print self.mv_list[mv_idx]
                                    #else:
                                    #    mv_rm.append(mv_idx)
                                    #    print 'marked for removal because UUID is not present even though drive is present'
                                    #    print self.mv_list[mv_idx]
                                mv_idx = mv_idx + 1
            if len(mv_rm) > 0:
                i_rm = len(mv_rm) - 1
                while i_rm >= 0:
                    self.mv_list.pop(mv_rm[i_rm])
                    i_rm = i_rm - 1
            gobject.idle_add(cleanup,)
            
        def cleanup():
            #print 'before run_scan'
            time.sleep(1)
            runscan.run_scan(None, self, True)
            #print 'after run_scan'
            #self.posthibern = False
        t = threading.Thread(target=t_run, args=())
        t.start()
                
def endSession(self, wParam,lParam):
    #print 'WM_QUERYENDSESSION [WPARAM:%i][LPARAM:%i]'%(wParam,lParam)
    
    if lParam == 0:
        if self.ostype == 0 :
            f = open('mountvol.txt', 'w')
            for v in self.mv_list:
                f.write(v[0] + ' ' + v[1] + ' ' + v[2] + '\n')
            for u in self.usb_mv_list:
                f.write(u[0] + ' ' + u[1] + '\n')
            f.close()
            
def remount(ui, y):
    txt = os.popen('mountvol').read()
    mv_idx = 0
    mv_rm = []
    for v in ui.mv_list:
        if v[2] == ui.sn_list[y]:
            if ui.lockstatus_list[y] == "Locked":
                m = re.search(v[0],txt)
                if m:
                    regex_mounted = v[0] + '}\\\s*\n\s*[A-Z]:'
                    v_mounted = re.search(regex_mounted,txt)
                    if not v_mounted:
                        if not os.path.isdir(v[1]):
                            #print 'mountvol ' + v[1] + ' \\\\?\\Volume{' + v[0] + '}\\'
                            ui.scan_ip = True
                            runop.prelock(y)
                            with open(os.devnull, 'w') as pipe:
                                subprocess.call(['mountvol', v[1], '\\\\?\\Volume{' + v[0] + '}\\'], stdout=pipe)#stderr=log)
                            runop.postlock(y)
                            m_count = m_count + 1
                        else:
                            dir_idx = 3
                            done = False
                            while not done:
                                if dir_idx == 26:
                                    done = True
                                else:
                                    dir = ascii_uppercase[dir_idx] + ':'
                                    if not os.path.isdir(dir):
                                        listed = False
                                        for v0 in ui.mv_list:
                                            if v0[1] == dir:
                                                listed = True
                                        if not listed:
                                            #print 'mountvol ' + dir + ' \\\\?\\Volume{' + v[0] + '}\\'
                                            ui.scan_ip = True
                                            runop.prelock(y)
                                            with open(os.devnull, 'w') as pipe:
                                                subprocess.call(['mountvol', dir, '\\\\?\\Volume{' + v[0] + '}\\'], stdout=pipe)#stderr=log)
                                            runop.postlock(y)
                                            m_count = m_count + 1
                                            v[1] = dir
                                            done = True
                                    if not done:
                                        dir_idx = dir_idx + 1
                    else:
                        mv_rm.append(mv_idx)
                        #print 'marked for removal due to it already being mounted'
                        #print ui.mv_list[mv_idx]
                #else:
                #    mv_rm.append(mv_idx)
                #    print 'marked for removal due to missing UUID'
                #    print ui.mv_list[mv_idx]
        mv_idx = mv_idx + 1
    if len(mv_rm) > 0:
        i_rm = len(mv_rm) - 1
        while i_rm >= 0:
            ui.mv_list.pop(mv_rm[i_rm])
            i_rm = i_rm - 1