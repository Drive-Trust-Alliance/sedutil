import gtk
import os
import re
import gobject
import datetime
import powerset
import runop
import runscan
import runthread
import csv
import threading
import verify
import pango

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
        
        if parent.VERSION != 1:
            #ave_instr = gtk.Label('Press \'Save to text file\' to save the query information in a file.')
            #queryVbox.pack_start(save_instr, False, False, 0)
            self.save_box = gtk.HBox(False,0)
            querySave = gtk.Button('_Save to text file')
            querySave.connect("clicked", self.saveToText, parent)
            querySave.set_tooltip_text('Saves the query information in a file.')
            if parent.VERSION == 0:
                querySave.set_sensitive(False)
            self.save_box.pack_end(querySave, False, False, 0)
            queryVbox.pack_start(self.save_box, False, False, 0)
        
        self.query_instr = gtk.Label('Enter the drive\'s password to access more query information.')
        queryVbox.pack_start(self.query_instr, False, False, 0)
        
        self.spinQ = gtk.Spinner()
        queryVbox.pack_start(self.spinQ, False, False, 0)
        index = parent.dev_select.get_active()
        
        #self.authQuery = None
        #
        #if parent.VERSION % 3 == 0  and (parent.setupuser_list[index] == 'Yes' or parent.setupuser_list[index] == None):
        #    
        #    if (gtk.gtk_version[1] > 24 or
        #        (gtk.gtk_version[1] == 24 and gtk.gtk_version[2] > 28)):
        #        self.authQuery = gtk.ComboBox()
        #    else:
        #        self.authQuery = gtk.combo_box_new_text()
        #        #self.authQuery.append = self.authQuery.append_text
        #    self.authBoxQ = gtk.HBox(False, 0)
        #    authLabel = gtk.Label('Auth Level')
        #    authLabel.set_width_chars(20)
        #    self.authBoxQ.pack_start(authLabel, False, False, 0)
        #    
        #    self.authQuery.append_text('Admin')
        #    self.authQuery.append_text('User')
        #    self.authQuery.set_active(0)
        #    
        #    
        #    
        #    self.authBoxQ.pack_start(self.authQuery, True, True,0)
        #    queryVbox.pack_start(self.authBoxQ, False, False, 0)
        
        
        
        self.passBoxQ = gtk.VBox(False, 0)
        hbox1 = gtk.HBox(False, 0)
        plTxt = ''
        if parent.VERSION == 3 or (parent.VERSION == 1 and parent.PBA_VERSION != 1):
            plTxt = 'Enter Admin Password'
        else:
            plTxt = 'Enter Password'
        passLabel = gtk.Label(plTxt)
        passLabel.set_width_chars(20)
        self.queryPass = gtk.Entry()
        self.queryPass.set_visibility(False)
        
        self.submitPass = gtk.Button('Submit')
        self.submitPass.connect("clicked", self.queryAuth, parent)
        if parent.VERSION == 0 or parent.PBA_VERSION == 0:
            self.submitPass.set_sensitive(False)
        hbox1.pack_start(passLabel, False, False, 0)
        hbox1.pack_start(self.queryPass, True, True, 0)
        hbox1.pack_start(self.submitPass, False, False, 0)
        self.passBoxQ.pack_start(hbox1, False, False, 0)
        queryVbox.pack_start(self.passBoxQ, False, False, 0)
        
        spBoxQ = gtk.HBox(False, 0)
        self.showPassQ = gtk.CheckButton("Show Password")
        self.showPassQ.connect("toggled", self.showPass_toggled, parent)
        self.showPassQ.set_active(False)
        spBoxQ.pack_end(self.showPassQ, False, False, 0)
        self.passBoxQ.pack_start(spBoxQ, False, False, 0)
        
        if parent.VERSION % 3 == 0 or (parent.VERSION == 1 and parent.PBA_VERSION != 1):
            checkBoxQ = gtk.HBox(False, 0)
            self.passReadQ = gtk.CheckButton("Read password from USB")
            self.passReadQ.connect("toggled", self.check_toggled, parent)
            checkBoxQ.pack_end(self.passReadQ, False, False, 0)
            self.passBoxQ.pack_start(checkBoxQ, False, False, 0)
        
        
        self.queryTextBuffer.set_text(self.queryWinText)
        
        if parent.VERSION != 1:
            self.queryPass.set_text('')
        
        self.add(queryVbox)
        self.show_all()
        
        self.spinQ.hide()
        parent.disable_menu()
        self.connect('destroy', self.onDestroy, parent)
        
    def onDestroy(self, button, parent):
        parent.enable_menu()
        
    def check_toggled(self, checkbox, parent):
        verify.licCheck(parent)
        is_checked = checkbox.get_active()
        if is_checked:
            self.queryPass.set_text("")
            self.queryPass.set_sensitive(False)
            self.showPassQ.set_active(False)
            self.showPassQ.set_sensitive(False)
        else:
            self.queryPass.set_sensitive(True)
            self.showPassQ.set_sensitive(True)

    def showPass_toggled(self, checkbox, parent):
        verify.licCheck(parent)
        if self.showPassQ.get_active():
            self.queryPass.set_visibility(True)
        else:
            self.queryPass.set_visibility(False)

    def msg_err(self, msg):
        message = gtk.MessageDialog(type=gtk.MESSAGE_ERROR, buttons=gtk.BUTTONS_OK, parent = self)
        
        message.set_markup(msg)
        res = message.run()
        message.destroy()
        
    def queryAuth(self, button, parent):
        verify.licCheck(parent)
        pw_strip = re.sub('\s', '', self.queryPass.get_text())
        if self.queryPass.get_text() == '' and not ((parent.VERSION == 3 or (parent.VERSION == 1 and parent.PBA_VERSION != 1)) and self.passReadQ.get_active()):
            self.msg_err('Enter the password.')
            return
        elif pw_strip == '' and not ((parent.VERSION == 3 or (parent.VERSION == 1 and parent.PBA_VERSION != 1)) and self.passReadQ.get_active()):
            self.msg_err('Invalid password. Passwords must have non-whitespace characters.')
            return
        salt = ''
        user = ''
        index = parent.dev_select.get_active()
        self.showPassQ.set_active(False)
        parent.devname = parent.devs_list[index]
        drive_na = 'Locking Enabled: N'
        m_na = re.search(drive_na, self.queryWinText)
        if m_na:
            self.msg_err('This drive has not been activated with Opal Lock.')
            return
        salt = parent.salt_list[index]
        user = parent.user_list[index]
        
        
        
        #if parent.VERSION % 3 == 0 and parent.setupuser_list[index] == 'Yes':
        #    self.authQuery.set_sensitive(False)
        self.queryPass.set_sensitive(False)
        self.submitPass.set_sensitive(False)
        self.query_instr.hide()
        self.showPassQ.set_sensitive(False)
        if (parent.VERSION == 3 or parent.PBA_VERSION >= 2):
            self.passReadQ.set_sensitive(False)
        if parent.VERSION != 1:
            self.save_box.set_sensitive(False)
            
        
            
        
        self.startSpin(parent)
        t1 = threading.Thread(target=runthread.rt_queryAuth, args=(self, parent, index, salt, user))
        t1.start()
            
    def startSpin(self, parent, *args):
        self.spinQ.show()
        self.spinQ.start()
            
    def stopSpin(self, parent, *args):
        self.spinQ.stop()
        self.spinQ.hide()
        
    def saveToText(self, _, parent):
        verify.licCheck(parent)
        chooser = gtk.FileChooserDialog(title=None,action=gtk.FILE_CHOOSER_ACTION_SAVE,buttons=(gtk.STOCK_CANCEL,gtk.RESPONSE_CANCEL,gtk.STOCK_SAVE,gtk.RESPONSE_OK))
        chooser.set_do_overwrite_confirmation(True)
        
        filter = gtk.FileFilter()
        filter.set_name("Text Files")
        filter.add_mime_type("text/plain")
        filter.add_pattern("*.txt")
        chooser.add_filter(filter)
        
        filter = gtk.FileFilter()
        filter.set_name("All files")
        filter.add_pattern("*")
        chooser.add_filter(filter)
        
        response = chooser.run()
        if response == gtk.RESPONSE_OK:
            filename = chooser.get_filename()
            #if not filename.endswith('.txt'):
            #    filename += '.txt'
            try:
                f = open(filename, 'w')
                f.write(self.queryWinText)
                f.close()
                chooser.destroy()
            except IOError:
                chooser.destroy()
                self.msg_err('Invalid file path.')
        else:
            chooser.destroy()
        
    def msg_err(self, msg):
        message = gtk.MessageDialog(type=gtk.MESSAGE_ERROR, buttons=gtk.BUTTONS_OK, parent = self)
        message.set_markup(msg)
        
        res = message.run()
        message.destroy()
        
    def msg_ok(self, msg):
        message = gtk.MessageDialog(type=gtk.MESSAGE_INFO, buttons=gtk.BUTTONS_OK, parent = self)
        message.set_markup(msg)
        
        res = message.run()
        message.destroy()
        
def query(button, parent, mode):
    verify.licCheck(parent)
    index = -1
    if len(parent.devs_list) > 0:
        index = parent.dev_select.get_active()
    else :
        parent.msg_err('No drive selected')
        return
    parent.devname = parent.devs_list[index]
    
    parent.dev_vendor.set_text(parent.vendor_list[index])
    parent.dev_sn.set_text(parent.sn_list[index])
    parent.dev_series.set_text(parent.series_list[index])
    if parent.msid_list[index] != None:
        parent.dev_msid.set_text(parent.msid_list[index])
    else:
        parent.dev_msid.set_text('Loading...')
    if parent.pba_list[index] != None:
        parent.dev_pbaVer.set_text(parent.pba_list[index])
    else:
        parent.dev_pbaVer.set_text('Loading...')
    
    parent.dev_opal_ver.set_text(parent.opal_ver_list[index])
    parent.dev_status.set_text(parent.lockstatus_list[index])
    if parent.setupstatus_list[index] != None:
        parent.dev_setup.set_text(parent.setupstatus_list[index])
    else:
        parent.dev_setup.set_text('Loading...')
    if parent.setupuser_list[index] != None:
        parent.dev_userSetup.set_text(parent.setupuser_list[index])
    else:
        parent.dev_userSetup.set_text('Loading...')
    parent.dev_enc.set_text(parent.encsup_list[index])
    parent.dev_blockSID.set_text(parent.blockSID_list[index])
    
    txt2 = ""
    if mode == 0 and index in parent.tcg_list:
        if not parent.scanning:
            verify.licCheck(parent)
            new_devname = None
        
            rescan_needed = False
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
                    #print entry
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
        
            txt = os.popen(parent.prefix + "sedutil-cli --query " + parent.devname ).read()
        
            queryTextList = ["Opal Lock Query information for drive " + parent.devname + "\n"]
            
            #txtVersion = os.popen(parent.prefix + "sedutil-cli --version" ).read()
            #regex_ver = 'OpalLock Version\s*:\s*.*'
            #m = re.search(regex_ver, txtVersion)
            #ver_parse = m.group()
            #queryTextList.append(ver_parse + "\nGUI Version 0.12.1\n\nDrive information\n")
            queryTextList.append("\nDrive information\n")
            
            queryTextList.append("Model: " + parent.dev_vendor.get_text() + "\n")
            queryTextList.append("Serial Number: " + parent.dev_sn.get_text() + "\n")
            queryTextList.append("TCG SSC: " + parent.dev_opal_ver.get_text() + "\n")
            queryTextList.append("MSID: " + parent.dev_msid.get_text() + "\n")
            
            txtState = os.popen(parent.prefix + "sedutil-cli --getmfgstate " + parent.devname).read()
            for i in range(dl_len):
                runop.postlock(i)
            regex_sp = 'adminSP life cycle state\s*:\s*(.*)\nlockingSP life cycle state\s*:\s*(.*)'
            m = re.search(regex_sp, txtState)
            if m:
                admin_state = m.group(1)
                locking_state = m.group(2)
            
                queryTextList.append("Admin SP State: " + admin_state + "\nLocking SP State: " + locking_state + "\n\nLocking Information\n")
            
            
            t = [ "Locked = [YN], LockingEnabled = [YN], MBR shadowing Not Supported = [YN], MBRDone = [YN], MBREnabled = [YN]",
                "Locking Objects = [0-9]*",
                "Max Tables = [0-9]*, Max Size Tables = [0-9]*",
                "Locking Admins = [0-9]*.*, Locking Users = [0-9]*.",
                "Policy = [NY].*",
                "Base comID = 0x[0-9A-F]*, Initial PIN = 0x[0-9A-F]*"]

            for txt11 in t:
                m = re.search(txt11, txt)
                if m:
                    txt1 = m.group()
                    txt11 = txt1.replace("Locking ", "")            
                    txt1 = txt11
                    txt11 = txt1.replace(", ", "\n")
                    txt2 = txt2 + txt11 + "\n"
            txt2 = parent.devname + " " + parent.dev_vendor.get_text() + "\n" + txt2
        
            tt = [ "Locked = [YN]", 
                    "LockingEnabled = [YN]",
                    "MBR shadowing Not Supported = [YN]",
                    "MBRDone = [YN]",
                    "MBREnabled = [YN]",
                    "Objects = [0-9]*",
                    "Max Tables = [0-9]*",
                    "Max Size Tables = [0-9]*",
                    "Admins = [0-9]",
                    "Users = [0-9]*",
                    "Policy = [YN]",
                    "Base comID = 0x[0-9A-F]*",
                    "Initial PIN = 0x[0-9A-F]*"]
                    
            sts_Locked = ""
            sts_LockingEnabled = ""
            sts_MBRShadowNotSupported = "N"
            sts_MBRDone = ""
            sts_MBREnabled = ""
            tblsz = ""
            nbr_MaxTables = ""
            nbr_Admins = ""
            nbr_Users = ""
            singleUser = ""
            comID_base = ""
            initialPIN = ""
            nbr_Objects = ""
            for txt_33 in tt:
                m = re.search(txt_33,txt2) 
                if m:
                    t3 = m.group()
                    x_words = t3.split(' = ',1)
                    if x_words[0] == "Locked":
                        sts_Locked = x_words[1]
                    elif x_words[0] == "LockingEnabled":
                        sts_LockingEnabled = x_words[1]                   
                    elif x_words[0] == "MBR shadowing Not Supported":
                        sts_MBRShadowNotSupported = x_words[1]
                    elif x_words[0] == "MBRDone":
                        sts_MBRDone = x_words[1]
                    elif x_words[0] == "MBREnabled":
                        sts_MBREnabled = x_words[1]
                    elif x_words[0] == "Max Size Tables":
                        tblsz_i = int(x_words[1],10)   
                        tblsz = str(tblsz_i/1000000) + "MB"
                    elif x_words[0] == "Max Tables":
                        nbr_MaxTables = x_words[1]
                    elif x_words[0] == "Objects":
                        nbr_Objects = x_words[1]
                    elif x_words[0] == "Admins":
                        nbr_Admins = x_words[1]
                    elif x_words[0] == "Users":
                        nbr_Users = x_words[1]  
                    elif x_words[0] == "Policy":
                        singleUser = x_words[1]
                    elif x_words[0] == "Base comID":
                        comID_base = x_words[1]
                    elif x_words[0] == "Initial PIN":
                        initialPIN = x_words[1]
            if singleUser == '':
                singleUser = 'N'
                nbr_Objects = 'N/A'
            queryTextList.append("Locked: " + sts_Locked + "\n")
            queryTextList.append("Locking Enabled: " + sts_LockingEnabled + "\n")
            queryTextList.append("MBR Shadowing Not Supported: " + sts_MBRShadowNotSupported + "\n")
            queryTextList.append("Shadow MBR Enabled: " + sts_MBREnabled + "\n")
            queryTextList.append("Shadow MBR Done: " + sts_MBRDone + "\n\nSingle User information\n")
            queryTextList.append("Single User Mode Support: " + singleUser + "\n")
            queryTextList.append("Number of Locking Ranges Supported: " + nbr_Objects + "\n\nDataStore information\n")
            queryTextList.append("DataStore Table Size: " + tblsz + "\n")
            queryTextList.append("Number of DataStore Tables: " + nbr_MaxTables + "\n\nOpal information\n")
            queryTextList.append("Number of Admins: " + nbr_Admins + "\n")
            queryTextList.append("Number of Users: " + nbr_Users + "\n")
            queryTextList.append("Base comID: " + comID_base + "\n")
            queryTextList.append("Initial PIN: " + initialPIN + "\n")
        
        
            queryWin = QueryDialog(parent, queryTextList)
        
            #queryWin.run()
        
            #queryWin.destroy()
            if rescan_needed:
                parent.msg_ok('A rescan is needed to update the drive list, press OK to proceed.')
                runscan.run_scan(None, parent, True)
            
            
        else:
            parent.scanning = False
    elif mode == 0:
        parent.msg_err('Non-TCG drives cannot be queried.')
        
class AuditDialog(gtk.Dialog):
    eventDescriptions = dict({1 : 'Drive Activated',
                              2 : 'Initial Drive Setup',
                              3 : 'Admin Authenticated',
                              4 : 'Invalid Admin Password',
                              5 : 'Admin Locked Out',
                              6 : 'User Authenticated',
                              7 : 'Invalid User Password',
                              8 : 'User Locked Out',
                              9 : 'Potential Intrusion Attempt Detected',
                              10: 'Preboot Image written to MBR',
                              11: 'User Setup',
                              12: 'User removed',
                              13: 'SID and Admin password changed',
                              14: 'User password changed',
                              15: 'Drive unlocked',
                              16: 'Preboot unlock from MBR',
                              17: 'Preboot unlock from USB',
                              18: 'Lock removed',
                              19: 'Lock removed and data erased using password',
                              20: 'Lock removed and data erased using PSID',
                              21: 'Query information accessed',
                              22: 'Audit Log accessed',
                              23: 'Admin Password saved to USB',
                              24: 'User Password saved to USB',
                              25: 'Admin Password read from USB',
                              26: 'User Password read from USB',
                              27: 'Crytographic Erase',
                              28: 'Preboot Image write to MBR failed',
                              29: 'User Setup failed',
                              30: 'User removal failed',
                              31: 'SID and Admin password change failed',
                              32: 'User password change failed',
                              33: 'Drive unlock failed',
                              34: 'Preboot unlock from MBR failed',
                              35: 'Preboot unlock from USB failed',
                              36: 'Revert setup failed',
                              37: 'Revert setup and erase data failed',
                              38: 'Revert setup and erase data using PSID failed',
                              39: 'Query information Access Failed',
                              40: 'Audit Log Access Failed'})

    def __init__(self, parent, a):
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
        auditRegex = "([0-9]{4}/[0-9]{2}/[0-9]{2}\s+[0-9]{2}:[0-9]{2}:[0-9]{2})\s+([0-9]{1,3})"
        #pattern = re.compile(auditRegex)
        
        for i in range(numEntries):
            #m = pattern.match(logList[i])
            m = re.match(auditRegex, logList[i])
            if m:
                dateTime = m.group(1)
                eventID = int(m.group(2))
                if eventID <= 40:
                    eventDes = self.eventDescriptions[eventID]
                    eventLevel = "Information"
                    if eventID == 4 or eventID == 7 or (eventID >= 28 and eventID <= 40):
                        eventLevel = "Error"
                        self.errorEntries.append((eventLevel, dateTime, eventID, eventDes))
                        self.warnerrEntries.append((eventLevel, dateTime, eventID, eventDes))
                    elif eventID == 5 or eventID == 8 or eventID == 9:
                        eventLevel = "Warning"
                        self.warnerrEntries.append((eventLevel, dateTime, eventID, eventDes))
                    if eventID <= 40 and eventID >= 1:
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
        verify.licCheck(parent)
        chooser = gtk.FileChooserDialog(title=None,action=gtk.FILE_CHOOSER_ACTION_SAVE,buttons=(gtk.STOCK_CANCEL,gtk.RESPONSE_CANCEL,gtk.STOCK_SAVE,gtk.RESPONSE_OK))
        chooser.set_do_overwrite_confirmation(True)
        
        filter = gtk.FileFilter()
        filter.set_name("CSV Files")
        filter.add_mime_type("text/csv")
        filter.add_pattern("*.csv")
        
        filter = gtk.FileFilter()
        filter.set_name("All files")
        filter.add_pattern("*")
        chooser.add_filter(filter)
        
        response = chooser.run()
        if response == gtk.RESPONSE_OK:
            filename = chooser.get_filename()
            #if not filename.endswith('.csv'):
            #    filename += '.csv'
            try:
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
            except IOError:
                chooser.destroy()
                message = gtk.MessageDialog(type=gtk.MESSAGE_ERROR, buttons=gtk.BUTTONS_OK, parent = self)
                message.set_markup('Invalid file path.')
                
                res = message.run()
                message.destroy()
        else:
            chooser.destroy()
        
    def filterLog(self, button, entries, mode):
        self.listStore.clear()
        for i in range(len(entries)):
            self.listStore.append(entries[i])
        self.viewAll_button.set_sensitive(mode != 0)
        self.viewWarnErr_button.set_sensitive(mode != 1)
        self.viewErr_button.set_sensitive(mode != 2)
        
def openLog(button, parent, *args):
    verify.licCheck(parent)
    pw_strip = re.sub('\s', '', parent.pass_entry.get_text())
    if parent.pass_entry.get_text() == '' and not ((parent.VERSION == 3 or (parent.VERSION == 1 and parent.PBA_VERSION != 1)) and parent.check_pass_rd.get_active()):
        parent.msg_err('Enter the password.')
        return
    elif pw_strip == '' and not ((parent.VERSION == 3 or (parent.VERSION == 1 and parent.PBA_VERSION != 1)) and parent.check_pass_rd.get_active()):
        parent.msg_err('Invalid password. Passwords must have non-whitespace characters.')
        return
    if parent.VERSION % 3 == 0 or (parent.VERSION == 1 and parent.PBA_VERSION != 1):
        if parent.pass_sav.get_active() and parent.drive_menu.get_active() < 0:
            parent.msg_err('No USB detected.')
            return
    
    index = parent.dev_select.get_active()
    parent.devname = parent.devs_list[index]
    
    parent.start_spin()
    t = threading.Thread(target=runthread.rt_openLog, args=(parent, index))
    t.start()
        
class OpalDialog(gtk.Dialog):
    def __init__(self, parent):
        columns = ['Drive', 'Model No.', 'Serial Number', 'TCG Version']
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
            col.set_min_width(170)
            col.set_alignment(0.5)
            col.set_sort_column_id(gtk.SORT_ASCENDING)
            col.set_sort_indicator(True)
            tvOpal.append_column(col)
            
        scrolledWin = gtk.ScrolledWindow()
        scrolledWin.set_policy(gtk.POLICY_NEVER, gtk.POLICY_AUTOMATIC)
        scrolledWin.add_with_viewport(tvOpal)
        
        vbox.pack_start(scrolledWin)
        self.show_all()

def openOpal(button, parent, *args):
    verify.licCheck(parent)
    tcgWin = OpalDialog(parent)
    tcgWin.run()
    
    tcgWin.destroy()

class USBDialog(gtk.Dialog):
    def __init__(self, parent):
        gtk.Dialog.__init__(self, 'USB', parent, 0, (gtk.STOCK_OK, gtk.RESPONSE_OK, gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL))
        
        self.set_default_size(250,250)
        
        self.set_border_width(10)
        if os.path.isfile('icon.ico'):
            self.set_icon_from_file('icon.ico')


        box = self.get_content_area()

        usb_instr = gtk.Label('Select a USB.\nA bootable USB will be created with the preboot image embedded in it.\nAfter the USB is set up, it can be used to unlock the selected drive.\nWARNING: All data on the USB will be erased (except for any previously saved password files).')
        attr = pango.AttrList()
        fg_color = pango.AttrForeground(65535, 0, 0, 154, 249)
        attr.insert(fg_color)
        usb_instr.set_attributes(attr)
        
        
        self.na_instr = gtk.Label('No USB detected. Insert a USB and press \'Rescan\' to continue.')
        
        self.wait_instr = gtk.Label('Loading USB list, please wait...')
        
        self.usb_menu = None
        usb_label = gtk.Label('USB')
        
        if (gtk.gtk_version[1] > 24 or
            (gtk.gtk_version[1] == 24 and gtk.gtk_version[2] > 28)):
            self.usb_menu = gtk.ComboBox()
        else:
            self.usb_menu = gtk.combo_box_new_text()
            
        button_box = gtk.HBox(False, 0)
        self.refresh_button = gtk.Button('Rescan')
        self.refresh_button.connect('clicked', self.refreshUSB, parent)
        self.refresh_button.set_sensitive(False)
        button_box.pack_start(self.usb_menu, True, True)
        button_box.pack_start(self.refresh_button, False, False)
            
        def t1_run():
            #parent.usb_list = []
            usb_new = []
            
            if parent.DEV_OS == 'Windows':
                #print 'USBDialog before wmic'
                txt = os.popen(parent.prefix + 'wmic diskdrive where "mediatype=\'Removable Media\'" get caption,deviceid').read()
                #print txt
                #print 'USBDialog after wmic'
                mod_regex = '\n([^\\\\]+)[^0-9]+([0-9]+)'
                #print txt
                #parent.usb_list = re.findall(mod_regex, txt)
                usb_new = re.findall(mod_regex, txt)
            elif parent.DEV_OS == 'Linux':
                txt = os.popen("for DLIST in `dmesg  | grep \"Attached SCSI removable disk\" | cut -d\" \" -f 3  | sed -e 's/\[//' -e 's/\]//'` ; do\necho $DLIST\ndone ").read()
                txt_regex = '/dev/sd[a-z]'
                list_u = re.findall(txt_regex,txt)
                for u in list_u:
                    txt1 = os.popen(parent.prefix + 'mount').read()
                    m = re.search(u,txt1)
                    if not m:
                        txt2 = os.popen(parent.prefix + 'blkid').read()
                        rgx = u + '.+'
                        m1 = re.search(rgx,txt2)
                        if m1:
                            r2 = '\s+TYPE="([a-z]+)"'
                            txt3 = m1.group(0)
                            m2 = re.search(r2,txt3)
                            type_a = m2.group(1)
                            s = os.system(parent.prefix + 'mount -t ' + type_a + ' ' + u + '1')
                txt3 = os.popen('mount').read()
                dev_regex3 = '(/dev/sd[a-z][1-9]?)\s*on\s*(\S+)\s*type'
                #parent.usb_list = re.findall(dev_regex3, txt3)
                usb_new = re.findall(dev_regex3, txt3)
                
            gobject.idle_add(cleanup, usb_new)
            
        def cleanup(usb_new):
                
            length = len(usb_new)
            #print parent.usb_list
                    
            if length > 0:
                count = 0
                usb_final = []
                for d in usb_new:
                    
                    if parent.DEV_OS == 'Windows':
                        mod = '\\\\.\\PhysicalDrive' + d[1]
                        if mod not in parent.devs_list:
                            self.usb_menu.append_text(d[0])
                            usb_final.append(d)
                            count = count + 1
                    elif parent.DEV_OS == 'Linux':
                        if d[0] not in parent.devs_list:
                            self.usb_menu.append_text(d[0])
                            usb_final.append(d)
                            count = count + 1
                parent.usb_list = usb_final
                
                if count > 0:
                    self.usb_menu.set_active(0)
                else:
                    self.na_instr.show()
            else:
                self.na_instr.show()
            self.wait_instr.hide()
            self.refresh_button.set_sensitive(True)
                
        t1 = threading.Thread(target=t1_run, args=())
        t1.start()
                
        box.pack_start(usb_instr, False, False)
        
        #box.pack_start(self.usb_menu, True, False)
        box.pack_end(button_box, False, False)
        box.pack_end(self.na_instr, False, False)
        box.pack_end(self.wait_instr, False, False)
        
        self.show_all()
        
        
        #if len(parent.usb_list) > 0:
        self.na_instr.hide()
        
    def refreshUSB(self, button, parent):
        self.refresh_button.set_sensitive(False)
        model = self.usb_menu.get_model()
            
        iter = gtk.TreeIter
        for row in model:
            model.remove(row.iter)
    
        parent.usb_list = []
        
        if parent.DEV_OS == 'Windows':
            txt = os.popen(parent.prefix + 'wmic diskdrive where "mediatype=\'Removable Media\'" get caption,deviceid').read()
            mod_regex = '\n([^\\\\]+)[^0-9]+([0-9]+)'
            parent.usb_list = re.findall(mod_regex, txt)
        elif parent.DEV_OS == 'Linux':
            txt = os.popen("for DLIST in `dmesg  | grep \"Attached SCSI removable disk\" | cut -d\" \" -f 3  | sed -e 's/\[//' -e 's/\]//'` ; do\necho $DLIST\ndone ").read()
            txt_regex = '/dev/sd[a-z]'
            list_u = re.findall(txt_regex,txt)
            for u in list_u:
                txt1 = os.popen(parent.prefix + 'mount').read()
                m = re.search(u,txt1)
                if not m:
                    txt2 = os.popen(parent.prefix + 'blkid').read()
                    rgx = u + '.+'
                    m1 = re.search(rgx,txt2)
                    if m1:
                        r2 = '\s+TYPE="([a-z]+)"'
                        txt3 = m1.group(0)
                        m2 = re.search(r2,txt3)
                        type_a = m2.group(1)
                        s = os.system(parent.prefix + 'mount -t ' + type_a + ' ' + u + '1')
            txt3 = os.popen('mount').read()
            dev_regex3 = '(/dev/sd[a-z][1-9]?)\s*on\s*(\S+)\s*type'
            parent.usb_list = re.findall(dev_regex3, txt3)
            
        length = len(parent.usb_list)
                
        if length > 0:
            count = 0
            usb_final = []
            dl_len = len(parent.devs_list)
            for x in range(dl_len):
                runop.prelock(x)
            txt_s = os.popen(parent.prefix + 'sedutil-cli --scan n').read()
            for x in range(dl_len):
                runop.postlock(x)
            rgx = '(PhysicalDrive[0-9]+|/dev/sd[a-z][0-9]?|/dev/nvme[0-9])\s+(?:[12ELP]+|No)\s+[^\:\s]+(?:\s[^\:\s]+)*\s*:\s*[^:]+\s*:\s*\S+'
            list_d = re.findall(rgx, txt_s)
            for d in parent.usb_list:
                
                if parent.DEV_OS == 'Windows':
                    mod = '\\\\.\\PhysicalDrive' + d[1]
                    if mod not in list_d:
                        self.usb_menu.append_text(d[0])
                        #print 'appended'
                        #print d
                        usb_final.append(d)
                        count = count + 1
                elif parent.DEV_OS == 'Linux':
                    if d[0] not in list_d:
                        self.usb_menu.append_text(d[0])
                        usb_final.append(d)
                        count = count + 1
            parent.usb_list = usb_final
            if count > 0:
                self.usb_menu.set_active(0)
        if len(parent.usb_list) > 0:
            self.na_instr.hide()
        else:
            self.na_instr.show()
        self.refresh_button.set_sensitive(True)
             
def show_about(button, parent, *args):
    verify.licCheck(parent)
    aboutWin = gtk.AboutDialog()
    aboutWin.set_program_name('Opal Lock')
    prefix = ''
    
    if parent.DEV_OS != 'Windows':
        prefix = 'sudo '
    
    txtVersion = os.popen(prefix + "sedutil-cli --version" ).read()
    regex_ver = 'Fidelity Lock Version\s*:\s*(.*)'
    m = re.search(regex_ver, txtVersion)
    ver_parse = m.group(1)
    
    aboutWin.set_version('GUI v0.25.1')
    aboutWin.set_comments('Opal Lock Version: ' + ver_parse)
    aboutWin.set_copyright('(c) 2019 Fidelity Height LLC. All rights reserved.')
    if parent.VERSION != 1:
        aboutWin.set_logo(gtk.gdk.pixbuf_new_from_file('icon.ico'))
    
    
    
    aboutWin.run()
    
    aboutWin.destroy()
    
class SetPowerDialog(gtk.Dialog):
    def __init__(self, parent, mode):
        gtk.Dialog.__init__(self, 'Power Settings', parent, 0, ('Apply and Close', gtk.RESPONSE_APPLY, 'Skip', gtk.RESPONSE_CANCEL))
        
        self.set_default_size(300, 300)
        
        self.set_border_width(10)
        if os.path.isfile('icon.ico'):
            self.set_icon_from_file('icon.ico')
        vbox = self.get_content_area()
        pwr_instr = None
        if mode == 0:
            pwr_instr = gtk.Label('Your power settings need to be changed.\nSleep/Standby is not compatible with Opal Lock.\nIf a system set up using Opal Lock is put on Sleep/Standby, it cannot awaken properly.\nTherefore, Sleep/Standby will be disabled and Hibernate will be used instead.')
        else:
            pwr_instr = gtk.Label('Modify your power settings.\nSleep/Standby is not compatible with Opal Lock.\nIf a system set up using Opal Lock is put on\nSleep/Standby, it cannot awaken properly.\nSleep/Standby will be disabled and Hibernate\nwill be used instead.')
        screen_label = gtk.Label('Turn off screen after:')
        self.screen_menu = None
        hib_label = gtk.Label('Hibernate PC after:')
        self.hib_menu = None
        sleep_label = gtk.Label('Sleep after:')
        self.sleep_menu = None
        pb_label = gtk.Label('When I press the power button:')
        self.pb_menu = None
        sb_label = gtk.Label('When I press the sleep button:')
        self.sb_menu = None
        lid_label = gtk.Label('When I close the lid (for laptops):')
        self.lid_menu = None
        
        if (gtk.gtk_version[1] > 24 or
            (gtk.gtk_version[1] == 24 and gtk.gtk_version[2] > 28)):
            self.screen_menu = gtk.ComboBox()
            self.hib_menu = gtk.ComboBox()
            self.sleep_menu = gtk.ComboBox()
            self.pb_menu = gtk.ComboBox()
            self.sb_menu = gtk.ComboBox()
            self.lid_menu = gtk.ComboBox()
        else:
            self.screen_menu = gtk.combo_box_new_text()
            self.hib_menu = gtk.combo_box_new_text()
            self.sleep_menu = gtk.combo_box_new_text()
            self.pb_menu = gtk.combo_box_new_text()
            self.sb_menu = gtk.combo_box_new_text()
            self.lid_menu = gtk.combo_box_new_text()
            
        self.screen_menu.append_text('1 minute')
        self.screen_menu.append_text('2 minutes')
        self.screen_menu.append_text('3 minutes')
        self.screen_menu.append_text('5 minutes')
        self.screen_menu.append_text('10 minutes')
        self.screen_menu.append_text('15 minutes')
        self.screen_menu.append_text('20 minutes')
        self.screen_menu.append_text('25 minutes')
        self.screen_menu.append_text('30 minutes')
        self.screen_menu.append_text('45 minutes')
        self.screen_menu.append_text('1 hour')
        self.screen_menu.append_text('2 hours')
        self.screen_menu.append_text('3 hours')
        self.screen_menu.append_text('4 hours')
        self.screen_menu.append_text('5 hours')
        self.screen_menu.append_text('Never')
        
        self.hib_menu.append_text('5 minutes')
        self.hib_menu.append_text('10 minutes')
        self.hib_menu.append_text('15 minutes')
        self.hib_menu.append_text('20 minutes')
        self.hib_menu.append_text('25 minutes')
        self.hib_menu.append_text('30 minutes')
        self.hib_menu.append_text('45 minutes')
        self.hib_menu.append_text('1 hour')
        self.hib_menu.append_text('2 hours')
        self.hib_menu.append_text('3 hours')
        self.hib_menu.append_text('4 hours')
        self.hib_menu.append_text('5 hours')
        self.hib_menu.append_text('Never')
        
        self.sleep_menu.append_text('Never')
        
        self.pb_menu.append_text('Do nothing')
        self.pb_menu.append_text('Hibernate')
        self.pb_menu.append_text('Shut down')
        self.pb_menu.append_text('Turn off the display')
        
        self.sb_menu.append_text('Do nothing')
        self.sb_menu.append_text('Hibernate')
        self.sb_menu.append_text('Shut down')
        self.sb_menu.append_text('Turn off the display')
        
        self.lid_menu.append_text('Do nothing')
        self.lid_menu.append_text('Hibernate')
        self.lid_menu.append_text('Shut down')
        
        settings = powerset.get_power()
        
        self.screen_menu.set_active(settings[0])
        self.hib_menu.set_active(settings[1])
        self.sleep_menu.set_active(0)
        self.pb_menu.set_active(settings[2])
        self.sb_menu.set_active(settings[3])
        self.lid_menu.set_active(settings[4])
        
        self.sleep_menu.set_sensitive(False)
        
        vbox.pack_start(pwr_instr)
        hbox0 = gtk.HBox(False, 0)
        hbox0.pack_start(screen_label, False, False)
        hbox0.pack_end(self.screen_menu, False, False)
        vbox.pack_start(hbox0, True, False)
        hbox1 = gtk.HBox(False, 0)
        hbox1.pack_start(hib_label, False, False)
        hbox1.pack_end(self.hib_menu, False, False)
        vbox.pack_start(hbox1, True, False)
        hbox2 = gtk.HBox(False, 0)
        hbox2.pack_start(sleep_label, False, False)
        hbox2.pack_end(self.sleep_menu, False, False)
        vbox.pack_start(hbox2, True, False)
        hbox3 = gtk.HBox(False, 0)
        hbox3.pack_start(pb_label, False, False)
        hbox3.pack_end(self.pb_menu, False, False)
        vbox.pack_start(hbox3, True, False)
        hbox4 = gtk.HBox(False, 0)
        hbox4.pack_start(sb_label, False, False)
        hbox4.pack_end(self.sb_menu, False, False)
        vbox.pack_start(hbox4, True, False)
        hbox5 = gtk.HBox(False, 0)
        hbox5.pack_start(lid_label, False, False)
        hbox5.pack_end(self.lid_menu, False, False)
        vbox.pack_start(hbox5, True, False)
        
        self.show_all()
        
def mngPower_prompt(button, parent, mode):
    verify.licCheck(parent)
    done = False
    while not done:
        dialog = SetPowerDialog(parent, mode)
        res = dialog.run()
        
        if res == gtk.RESPONSE_APPLY:
            s = powerset.set_power(dialog)
            if s == 0:
                parent.msg_ok('Power settings applied successfully.')
                done = True
            else:
                parent.msg_err('There was an error while changing your power settings.')
        else:
            done = True
            
        dialog.destroy()