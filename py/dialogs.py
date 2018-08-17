import gtk
import os
import re
import sys
import getopt
import gobject
import subprocess
import time
import datetime
import pbkdf2
import powerset
import string
import hashlib
import platform
import lockhash
import runop
import random
import csv

class QueryDialog(gtk.Window):
    def __init__(self, parent, queryTextList):
        self.queryWinText = ''.join(queryTextList)
    
        gtk.Window.__init__(self)
        self.set_title('Query Drive')
        self.set_border_width(10)
        
        scrolledWin = gtk.ScrolledWindow()
        scrolledWin.set_policy(gtk.POLICY_NEVER, gtk.POLICY_AUTOMATIC)
        
        self.set_default_size(400, 500)
        if os.path.isfile('icon.ico'):
            self.set_icon_from_file('icon.ico')
        
        queryVbox = gtk.VBox(False,0)
        
        queryTextView = gtk.TextView()
        queryTextView.set_editable(False)
        self.queryTextBuffer = queryTextView.get_buffer()
        self.queryTextBuffer.set_text('')
        scrolledWin.add_with_viewport(queryTextView)
        queryVbox.pack_start(scrolledWin, True, True, 0)
        
        self.query_instr = gtk.Label('Enter the drive\'s password to access more query information.')
        queryVbox.pack_start(self.query_instr, False, False, 0)
        
        if parent.VERSION % 3 == 0:
            self.authQuery = None
            if (gtk.gtk_version[1] > 24 or
                (gtk.gtk_version[1] == 24 and gtk.gtk_version[2] > 28)):
                self.authQuery = gtk.ComboBoxEntry()
            else:
                self.authQuery = gtk.combo_box_entry_new_text()
                self.authQuery.append = self.authQuery.append_text
            self.authBoxQ = gtk.HBox(False, 0)
            authLabel = gtk.Label('Auth Level')
            self.authBoxQ.pack_start(authLabel, True, True, 0)
            
            self.authQuery.append('Admin')
            self.authQuery.append('User')
            self.authQuery.set_active(0)
            
            self.authBoxQ.pack_start(self.authQuery, True, True,0)
            queryVbox.pack_start(self.authBoxQ, False, False, 0)
        
        self.passBoxQ = gtk.HBox(False, 0)
        plTxt = ''
        if parent.VERSION == 1 and parent.PBA_VERSION != 1:
            plTxt = 'Enter Admin Password'
        else:
            plTxt = 'Enter Password'
        passLabel = gtk.Label(plTxt)
        self.queryPass = gtk.Entry()
        self.queryPass.set_visibility(False)
        
        submitPass = gtk.Button('Submit')
        submitPass.connect("clicked", self.queryAuth, parent)
        if parent.VERSION == 0:
            submitPass.set_sensitive(False)
        self.passBoxQ.pack_start(passLabel, True, True, 0)
        self.passBoxQ.pack_start(self.queryPass, True, True, 0)
        self.passBoxQ.pack_start(submitPass, False, False, 0)
        queryVbox.pack_start(self.passBoxQ, False, False, 0)
        if parent.VERSION != 1:
            save_instr = gtk.Label('Press \'Save to text file\' to save the query information in a file.')
            queryVbox.pack_start(save_instr, False, False, 0)
            
            querySave = gtk.Button('_Save to text file')
            querySave.connect("clicked", self.saveToText)
            if parent.VERSION == 0:
                querySave.set_sensitive(False)
            queryVbox.pack_start(querySave, False, False, 0)
            
        self.queryTextBuffer.set_text(self.queryWinText)
        
        if parent.VERSION != 1:
            self.queryPass.set_text('')
        
        self.add(queryVbox)
        self.show_all()
        
    def queryAuth(self, button, parent):
        salt = ''
        user = ''
        index = parent.dev_select.get_active()
        parent.devname = parent.devs_list[index]
        #if parent.view_state == 0:
        salt = parent.salt_list[index]
        user = parent.user_list[index]
        
        devpass = lockhash.hash_pass(self.queryPass.get_text(), salt, parent.dev_msid.get_text())
        level = 0
        if parent.VERSION % 3 == 0:
            level = self.authQuery.get_active()
        m3 = ''
        if level == 0:
            f0 = os.popen(parent.prefix + "sedutil-cli -n -t --getmbrsize " + devpass + " " + parent.devname)
            p0 = f0.read()
            rc = f0.close()
            out_regex = 'Shadow.+(?:\n.+)+'
            m3 = re.search(out_regex, p0)
            if not m3:
                if rc == parent.NOT_AUTHORIZED or rc == parent.AUTHORITY_LOCKED_OUT:
                    pwd = lockhash.get_val() + salt
                    hash_pwd = lockhash.hash_pass(pwd, salt, parent.dev_msid.get_text())
                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                    timeStr = timeStr[2:]
                    statusAW = os.system(parent.prefix + "sedutil-cli -n -t -u --auditwrite 04" + timeStr + " " + hash_pwd + " User" + user + " " + parent.devname)
                    if rc == parent.AUTHORITY_LOCKED_OUT:
                        statusAW = os.system(parent.prefix + "sedutil-cli -n -t -u --auditwrite 10" + timeStr + " " + hash_pwd + " User" + user + " " + parent.devname)
                        parent.msg_err('Error: Retry limit has been reached, please power cycle your drive to try again.')
                    else:
                        parent.msg_err('Error: Invalid password')
                elif rc == parent.SP_BUSY:
                    parent.msg_err('SP_BUSY')
                else:
                    parent.msg_err("Error: Failed to retrieve additional drive information.")
                return
        p1 = ''
        p2 = ''
        if parent.VERSION != 1:
            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
            timeStr = timeStr[2:]
            if level == 1:
                statusAW = os.system(parent.prefix + "sedutil-cli -n -t -u --auditwrite 05" + timeStr + " " + devpass + " User1 " + parent.devname)
                if statusAW == parent.NOT_AUTHORIZED or statusAW == parent.AUTHORITY_LOCKED_OUT:
                    statusAW = os.system(parent.prefix + "sedutil-cli -n -t -u --auditwrite 06" + timeStr + " " + devpass + " User" + user + " " + parent.devname)
                    if statusAW == parent.AUTHORITY_LOCKED_OUT:
                        statusAW = os.system(parent.prefix + "sedutil-cli -n -t -u --auditwrite 10" + timeStr + " " + devpass + " User" + user + " " + parent.devname)
                        parent.msg_err("Error: Retry limit reached, please power cycle your drive to try again.")
                    else:
                        parent.msg_err("Error: Invalid password")
                    return
                elif statusAW == parent.SP_BUSY:
                    parent.msg_err('SP_BUSY')
                    return
                elif statusAW != 0:
                    parent.msg_err("Error: Failed to retrieve the information.")
                    return
                p2 = os.popen(parent.prefix + "sedutil-cli -n -t -u --auditread " + devpass + " User1 " + parent.devname).read()
            else:
                statusAW = os.system(parent.prefix + "sedutil-cli -n -t --auditwrite 03" + timeStr + " " + devpass + " Admin1 " + parent.devname)
                if statusAW == parent.NOT_AUTHORIZED or statusAW == parent.AUTHORITY_LOCKED_OUT:
                    statusAW = os.system(parent.prefix + "sedutil-cli -n -t --auditwrite 06" + timeStr + " " + devpass + " Admin1 " + parent.devname)
                    if statusAW == parent.AUTHORITY_LOCKED_OUT:
                        statusAW = os.system(parent.prefix + "sedutil-cli -n -t --auditwrite 10" + timeStr + " " + devpass + " Admin1 " + parent.devname)
                        parent.msg_err("Error: Retry limit reached, please power cycle your drive to try again.")
                    else:
                        parent.msg_err("Error: Invalid password")
                    return
                elif statusAW == parent.SP_BUSY:
                    parent.msg_err('SP_BUSY')
                    return
                elif statusAW != 0:
                    parent.msg_err("Error: Failed to retrieve the information.")
                    return
                p2 = os.popen(parent.prefix + "sedutil-cli -n -t --auditread " + devpass + " Admin1 " + parent.devname).read()
            if level == 0:
                p1 = os.popen(parent.prefix + "sedutil-cli -n -t --pbaValid " + devpass + " " + parent.devname).read()
                r = 'PBA image version\s*:'
                t = re.search(r, p1)
                if not t:
                    parent.msg_err("Error: Invalid password")
                    return
        pba_regex = 'PBA image version\s*:\s*(\S+)'
        audit_regex = 'Fidelity Audit Log Version\s*([0-9]+\.[0-9]+)\s*:'
        m1 = re.search(pba_regex, p1)
        m2 = re.search(audit_regex, p2)
        pba_ver = ''
        audit_ver = ''
        if m1:
            pba_ver = m1.group(1)
        else:
            pba_ver = 'N/A'
        if m2:
            audit_ver = m2.group(1)
        else:
            audit_ver = 'N/A'
        if parent.VERSION != 1:
            if level == 0:
                self.queryWinText = self.queryWinText + "\n" + m3.group(0) + "\n\nPreboot Image Version: " + pba_ver + "\nAudit Log Version: " + audit_ver
            else:
                self.queryWinText = self.queryWinText + "\nAudit Log Version: " + audit_ver
        else:
            self.queryWinText = self.queryWinText + "\n" + m3.group(0) + "\nAudit Log Version: " + audit_ver
        self.queryTextBuffer.set_text(self.queryWinText)
        self.passBoxQ.hide()
        if parent.VERSION % 3 == 0:
            self.authBoxQ.hide()
        self.query_instr.hide()
        
    def saveToText(self, *args):
        chooser = gtk.FileChooserDialog(title=None,action=gtk.FILE_CHOOSER_ACTION_SAVE,buttons=(gtk.STOCK_CANCEL,gtk.RESPONSE_CANCEL,gtk.STOCK_SAVE,gtk.RESPONSE_OK))
        
        filter = gtk.FileFilter()
        filter.set_name("All files")
        filter.add_pattern("*")
        chooser.add_filter(filter)
        
        filter = gtk.FileFilter()
        filter.set_name("Text Files")
        filter.add_mime_type("text/plain")
        filter.add_pattern("*.txt")
        chooser.add_filter(filter)
        
        response = chooser.run()
        if response == gtk.RESPONSE_OK:
            filename = chooser.get_filename()
            if not filename.endswith('.txt'):
                filename += '.txt'
            f = open(filename, 'w')
            f.write(self.queryWinText)
            f.close()
        chooser.destroy()

class AuditDialog(gtk.Dialog):
    eventDescriptions = dict({1 : 'Activate',
                              2 : 'Initial Setup',
                              3 : 'Admin Login',
                              4 : 'Failed Admin Login',
                              5 : 'User Login',
                              6 : 'Failed User Login',
                              7 : 'Potential Intrusion Attempt Detected',
                              8 : 'Preboot Image written to MBR',
                              9 : 'User created',
                              10: 'User removed',
                              11: 'SID password changed',
                              12: 'Admin password changed',
                              13: 'User password changed',
                              14: 'Drive unlocked',
                              15: 'Preboot unlock from PBA in MBR',
                              16: 'Preboot unlock from USB',
                              17: 'Drive locked',
                              18: 'Password saved to USB',
                              19: 'Password read from USB',
                              20: 'Attempt to remove lock',
                              21: 'Attempt to remove lock and erase data',
                              22: 'Attempt to remove lock and erase data using PSID',
                              23: 'Lock removed',
                              24: 'Lock removed and data erased using password',
                              25: 'Lock removed and data erased using PSID',
                              26: 'Crytographic Erase',
                              27: 'Attempt to remove lock failed',
                              28: 'Attempt to remove lock and erase data failed',
                              29: 'Attempt to remove lock and erase data using PSID failed'})

    def __init__(self, parent, a):
        if (parent.VERSION == 3 or (parent.VERSION == 1 and parent.PBA_VERSION != 1)) and parent.pass_sav.get_active():
            #print "openLog passSaveUSB " + password + " "  + self.auth_menu.get_active_text()
            runop.passSaveUSB(self, password, parent.drive_menu.get_active_text(), parent.dev_vendor.get_text(), parent.dev_sn.get_text())
        columns = ["Level", "Date and Time", "Event ID", "Event Description"]
        self.auditEntries = []
        self.errorEntries = []
        self.warnerrEntries = []

        gtk.Dialog.__init__(self, 'Audit Log', parent, 0, (gtk.STOCK_CLOSE, gtk.RESPONSE_CLOSE))
        self.set_border_width(10)
        self.set_default_size(500, 500)
        
        if os.path.isfile('icon.ico'):
            self.set_icon_from_file('icon.ico')
        vbox = self.get_content_area()
        
        self.listStore = gtk.ListStore(str, str, int, str)
    
        numEntries = int(a.group(1))
        logList = a.group(2).split('\n')
        auditRegex = "([0-9]+/[0-9]+/[0-9]+\s+[0-9]+:[0-9]+:[0-9]+)\s+([0-9]+)"
        pattern = re.compile(auditRegex)
        
        for i in range(numEntries):
            m = pattern.match(logList[i])
            dateTime = m.group(1)
            eventID = int(m.group(2))
            eventDes = self.eventDescriptions[eventID]
            eventLevel = "Information"
            if eventID == 4 or eventID == 6 or eventID == 27 or eventID == 28 or eventID == 29:
                eventLevel = "Error"
                self.errorEntries.append((eventLevel, dateTime, eventID, eventDes))
                self.warnerrEntries.append((eventLevel, dateTime, eventID, eventDes))
            elif eventID == 7 or eventID == 20 or eventID == 21 or eventID == 22:
                eventLevel = "Warning"
                self.warnerrEntries.append((eventLevel, dateTime, eventID, eventDes))
            self.auditEntries.append((eventLevel, dateTime, eventID, eventDes))
        for i in range(len(self.auditEntries)):
            self.listStore.append(self.auditEntries[i])
    
        treeView = gtk.TreeView(model=self.listStore)
        
        
        for i in range(len(columns)):
            cell = gtk.CellRendererText()
            col = gtk.TreeViewColumn(columns[i], cell, text=i)
            if i < 3:
                col.set_sort_column_id(gtk.SORT_DESCENDING)
                col.set_sort_indicator(True)
            treeView.append_column(col)
            
        scrolledWin = gtk.ScrolledWindow()
        scrolledWin.set_policy(gtk.POLICY_NEVER, gtk.POLICY_AUTOMATIC)
        scrolledWin.add_with_viewport(treeView)
        
        vbox.pack_start(scrolledWin)
        
        halign = gtk.Alignment(1,0,0,0)
        filter_box = gtk.HBox(False, 0)
        
        if parent.VERSION != 1:
            self.saveLog_button = gtk.Button('Save as CSV file')
            self.saveLog_button.connect('clicked', self.saveToCSV, parent)
            filter_box.pack_start(self.saveLog_button, False, False, 5)
        
        self.viewAll_button = gtk.Button('View all entries')
        self.viewAll_button.connect("clicked", self.filterLog, self.auditEntries, 0)
        self.viewAll_button.set_sensitive(False)
        filter_box.pack_start(self.viewAll_button, False, False, 5)
        
        self.viewWarnErr_button = gtk.Button('View Warnings & Errors')
        self.viewWarnErr_button.connect("clicked", self.filterLog, self.warnerrEntries, 1)
        filter_box.pack_start(self.viewWarnErr_button, False, False, 5)
        
        self.viewErr_button = gtk.Button('View Errors')
        self.viewErr_button.connect("clicked", self.filterLog, self.errorEntries, 2)
        filter_box.pack_start(self.viewErr_button, False, False, 5)
        
        halign.add(filter_box)
        vbox.pack_start(halign, False, False, 0)
        
        self.show_all()
        
    def saveToCSV(self, button, parent):
        chooser = gtk.FileChooserDialog(title=None,action=gtk.FILE_CHOOSER_ACTION_SAVE,buttons=(gtk.STOCK_CANCEL,gtk.RESPONSE_CANCEL,gtk.STOCK_SAVE,gtk.RESPONSE_OK))
        
        filter = gtk.FileFilter()
        filter.set_name("All files")
        filter.add_pattern("*")
        chooser.add_filter(filter)
        
        filter = gtk.FileFilter()
        filter.set_name("CSV Files")
        filter.add_mime_type("text/csv")
        filter.add_pattern("*.csv")
        chooser.add_filter(filter)
        
        response = chooser.run()
        if response == gtk.RESPONSE_OK:
            filename = chooser.get_filename()
            if not filename.endswith('.csv'):
                filename += '.csv'
            f = open(filename, 'wb')
            with f:
                data = []
                data.append(['Drive', parent.devname])
                data.append(['Model', parent.dev_vendor.get_text()])
                data.append(['Serial Number', parent.dev_sn.get_text()])
                timeStr = datetime.datetime.now().strftime('%Y/%m/%d %H:%M:%S')
                data.append(['Time', timeStr])
                data.append(['Level', 'Date/Time', 'Event ID', 'Event Description'])
                for row in self.listStore:
                    row_data = [row[0], row[1], row[2], row[3]]
                    data.append(row_data)
                writer = csv.writer(f)
                writer.writerows(data)
            f.close()
        chooser.destroy()
        
    def filterLog(self, button, entries, mode):
        self.listStore.clear()
        for i in range(len(entries)):
            self.listStore.append(entries[i])
        self.viewAll_button.set_sensitive(mode != 0)
        self.viewWarnErr_button.set_sensitive(mode != 1)
        self.viewErr_button.set_sensitive(mode != 2)

class OpalDialog(gtk.Dialog):
    def __init__(self, parent):
        columns = ['Drive', 'Model Number', 'Serial Number', 'TCG Version']
        gtk.Dialog.__init__(self, 'TCG Drives', parent, 0, (gtk.STOCK_CLOSE, gtk.RESPONSE_CLOSE))
        self.set_border_width(10)
        self.set_default_size(500, 500)
        if os.path.isfile('icon.ico'):
            self.set_icon_from_file('icon.ico')
        vbox = self.get_content_area()
        lsOpal = gtk.ListStore(str, str, str, str)
        tcgEntries = []
        for i in parent.tcg_list:
            tcgEntries.append((parent.devs_list[i], parent.vendor_list[i], parent.sn_list[i], parent.opal_ver_list[i]))
        
        for e in tcgEntries:
            lsOpal.append(e)

        tvOpal = gtk.TreeView(model=lsOpal)
        
        for i in range(len(columns)):
            cell = gtk.CellRendererText()
            col = gtk.TreeViewColumn(columns[i], cell, text=i)
            col.set_sort_column_id(gtk.SORT_ASCENDING)
            col.set_sort_indicator(True)
            tvOpal.append_column(col)
            
        scrolledWin = gtk.ScrolledWindow()
        scrolledWin.set_policy(gtk.POLICY_NEVER, gtk.POLICY_AUTOMATIC)
        scrolledWin.add_with_viewport(tvOpal)
        
        vbox.pack_start(scrolledWin)
        self.show_all()

class AboutDialog(gtk.Dialog):
    def __init__(self, parent):
        gtk.Dialog.__init__(self, 'About', parent, 0, (gtk.STOCK_CLOSE, gtk.RESPONSE_CLOSE))

        self.set_default_size(250, 250)
        
        self.set_border_width(10)
        if os.path.isfile('icon.ico'):
            self.set_icon_from_file('icon.ico')


        box = self.get_content_area()
        
        prefix = ''
        
        dev_os = platform.system()
        if dev_os != 'Windows':
            prefix = 'sudo '
        
        txtVersion = os.popen(prefix + "sedutil-cli --version" ).read()
        regex_ver = 'Fidelity Lock Version\s*:\s*.*'
        m = re.search(regex_ver, txtVersion)
        ver_parse = m.group()
        about_label = gtk.Label(ver_parse)
        box.add(about_label)
        self.show_all()
    
class SetPowerDialog(gtk.Dialog):
    def __init__(self, parent, mode):
        gtk.Dialog.__init__(self, 'Power Settings', parent, 0, (gtk.STOCK_APPLY, gtk.RESPONSE_APPLY))
        
        self.set_default_size(250, 250)
        
        self.set_border_width(10)
        if os.path.isfile('icon.ico'):
            self.set_icon_from_file('icon.ico')
        vbox = self.get_content_area()
        pwr_instr = None
        if mode == 0:
            pwr_instr = gtk.Label('Your power settings need to be changed.\nSleep will be disabled and Hibernate will be used instead.')
        else:
            pwr_instr = gtk.Label('Modify your power settings.')
        screen_label = gtk.Label('Turn off screen after')
        self.screen_menu = None
        hib_label = gtk.Label('Hibernate PC after')
        self.hib_menu = None
        
        if (gtk.gtk_version[1] > 24 or
            (gtk.gtk_version[1] == 24 and gtk.gtk_version[2] > 28)):
            self.screen_menu = gtk.ComboBoxEntry()
            self.hib_menu = gtk.ComboBoxEntry()
        else:
            self.screen_menu = gtk.combo_box_entry_new_text()
            self.hib_menu = gtk.combo_box_entry_new_text()
            
            self.screen_menu.append = self.screen_menu.append_text
            self.hib_menu.append = self.hib_menu.append_text
            
        self.screen_menu.append('1 minute')
        self.screen_menu.append('2 minutes')
        self.screen_menu.append('3 minutes')
        self.screen_menu.append('5 minutes')
        self.screen_menu.append('10 minutes')
        self.screen_menu.append('15 minutes')
        self.screen_menu.append('20 minutes')
        self.screen_menu.append('25 minutes')
        self.screen_menu.append('30 minutes')
        self.screen_menu.append('45 minutes')
        self.screen_menu.append('1 hour')
        self.screen_menu.append('2 hours')
        self.screen_menu.append('3 hours')
        self.screen_menu.append('4 hours')
        self.screen_menu.append('5 hours')
        
        self.hib_menu.append('5 minutes')
        self.hib_menu.append('10 minutes')
        self.hib_menu.append('15 minutes')
        self.hib_menu.append('20 minutes')
        self.hib_menu.append('25 minutes')
        self.hib_menu.append('30 minutes')
        self.hib_menu.append('45 minutes')
        self.hib_menu.append('1 hour')
        self.hib_menu.append('2 hours')
        self.hib_menu.append('3 hours')
        self.hib_menu.append('4 hours')
        self.hib_menu.append('5 hours')
        
        settings = powerset.get_power()
        
        self.screen_menu.set_active(settings[0])
        self.hib_menu.set_active(settings[1])
        
        vbox.pack_start(pwr_instr)
        vbox.pack_start(screen_label)
        vbox.pack_start(self.screen_menu)
        vbox.pack_start(hib_label)
        vbox.pack_start(self.hib_menu)
        
        self.show_all()