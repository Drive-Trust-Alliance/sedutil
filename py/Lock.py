import gtk
import os
import re
import sys
import getopt
import gobject
import subprocess
import time
import datetime
import threading
import pbkdf2
import powerset
import string
import hashlib
import platform
import lockhash
import runop
import random
import csv

gobject.threads_init()

def make_hbox(homogeneous, spacing, expand, fill, padding,lab,ent):

    box = gtk.HBox(homogeneous, spacing)

    button = gtk.Button(lab)
    box.pack_start(button, expand, fill, padding)
    button.show()

    txt = gtk.Entry()
    txt.set_text(ent)
    box.pack_start(txt, True, True, padding)
    return box
    
    
class LockApp(gtk.Window):
    ''' An application for pyGTK.  Instantiate
        and call the run method to run. '''
    firstscan = True
    firstmsg = True
    devname="\\\\.\\PhysicalDrive9"
    
    bad_pw = dict({'password': 1,
                   '12345678': 1,
                   '123456789': 1,
                   'football': 1,
                   'baseball': 1,
                   '1234567890': 1,
                   '1qaz2wsx': 1,
                   'princess': 1,
                   'qwertyuiop': 1,
                   'passw0rd': 1,
                   'starwars': 1,
                   '11111111': 1,
                   'sunshine': 1,
                   'zaq1zaq1': 1,
                   'password1': 1})
                   
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
                   
    image = "LINUXRelease.img"

    LKRNG = "0" 
    LKRNG_SLBA = "0"
    LKRNG_LBALEN = "0"
    LKATTR = "RW"

    VERSION = -1
    PBA_VERSION = -1
    MAX_DEV = -1
    
    queryWinText = ""
    
    __gsignals__ = {
        "delete-event" : "override"
    }
     
    try:
        opts, args = getopt.getopt(sys.argv[1:], [""], ["pba", "demo", "standard", "premium"])
    except getopt.GetoptError, err:
        exit(2)

    for o, a in opts:
        if o in ("--demo"):
            VERSION = 0
            MAX_DEV = sys.maxint
        elif o in ("--pba"):
            VERSION = 1
            PBA_VERSION = 5
            MAX_DEV = 5
        elif o in ("--standard"):
            VERSION = 2
            MAX_DEV = 5
        elif o in ("--premium"):
            VERSION = 3
            MAX_DEV = sys.maxint
    
   
    devs_list = [] #[ "/dev/sda" , "/dev/sdb",  "/dev/sdc",  "/dev/sdd",  "/dev/sde" ]
    locked_list = []
    setup_list = []
    unlocked_list = []
    nonsetup_list = []
    tcg_list = []
    sel_list = []
    usrsup_list = []
    
    msid_list = []
    pba_list = []
    
    warned = False
    orig = ''
    
    mode_setupUSB = False
    
    usb_boot = False
    
    user_list = []
    
    process = None
    
    vendor_list = [] # "Sandisk" 
    opal_ver_list = [] # 1, 2, or 12
    series_list = [] # series number 
    sn_list = []
    blockSID_list = []
    
    salt_list = []
    
    lockstatus_list = []
    setupstatus_list = []
    encsup_list = []
    
    check_exclusive = False
    check_both = False
    
    pass_usb = ''
    
    scanning = False
    view_state = 0
    op_prompt = 0
    NOT_AUTHORIZED = 256
    AUTHORITY_LOCKED_OUT = 4608
    SP_BUSY = 3
    
    def __init__(self):
        runop.findos(self)
        version_text = os.popen(self.prefix + 'sedutil-cli --version').read()
        regex_license = 'Valid Fidelity Lock License found 0:([0-9]+);'
        f = re.search(regex_license, version_text)
        if f or self.VERSION != -1:
            if self.VERSION == -1:
                v = f.group(1)
                if v == '1':
                    self.VERSION = 0
                    self.MAX_DEV = sys.maxint
                elif v == '2':
                    self.VERSION = 2
                    self.MAX_DEV = 5
                elif v == '4':
                    self.VERSION = 3
                    self.MAX_DEV = 5
                elif v == '16':
                    self.VERSION = 3
                    self.MAX_DEV = 25
                elif v == '32':
                    self.VERSION = 3
                    self.MAX_DEV = 100
                elif v == '64':
                    self.VERSION = 3
                    self.MAX_DEV = sys.maxint

    
            res = lockhash.testPBKDF2()
            status = res[0]
            hash_v = os.popen(self.prefix + 'sedutil-cli --validatePBKDF2').read()
            f = re.search(res[1], hash_v)
            
            if status != 0 or not f:
                self.msg_err("Hash validation failed")
                self.exitapp()
            
            
        
            gtk.Window.__init__(self)
            if self.VERSION == 0:
                self.set_title('Fidelity Lock Disk Drive Security Manager - Demo')
            elif self.VERSION == 1:
                self.set_title('Fidelity Lock Disk Drive Security Manager - PBA')
            elif self.VERSION == 2:
                self.set_title('Fidelity Lock Disk Drive Security Manager - Standard')
            elif self.MAX_DEV == 5:
                self.set_title('Fidelity Lock Disk Drive Security Manager - Premium5')
            elif self.MAX_DEV == 25:
                self.set_title('Fidelity Lock Disk Drive Security Manager - Premium25')
            elif self.MAX_DEV == 100:
                self.set_title('Fidelity Lock Disk Drive Security Manager - Premium100')
            else:
                self.set_title('Fidelity Lock Disk Drive Security Manager - PremiumUnlimited')
                
            if os.path.isfile('icon.jpg'):
                self.set_icon_from_file('icon.jpg')

            height = 550
            width = 575
            if platform.system() == 'Linux':
                width = 625
                
            self.set_size_request(width, height)

            self.connect('destroy', gtk.main_quit)
            
            self.connect('delete_event', self.destroy_main)
            
            homogeneous = False
            spacing = 0
            expand = False
            fill = False
            padding = 0
            
            self.vbox0 = gtk.VBox(False,0)
            self.hbox0 = gtk.HBox(False,0)
            self.hbox = gtk.HBox(False,0)
            self.vbox = gtk.VBox(False,5)
            
            self.menuBar = gtk.MenuBar()
            
            self.navMenu = gtk.Menu()
            self.navM = gtk.MenuItem("Main")
            self.navM.set_submenu(self.navMenu)
            self.backToMain = gtk.MenuItem("Home")
            self.backToMain.connect("activate", self.returnToMain)
            self.backToMain.set_tooltip_text('Return to Home View')
            self.navMenu.append(self.backToMain)
            self.exitApp = gtk.MenuItem("Exit")
            self.exitApp.connect("activate", self.exitapp)
            self.exitApp.set_tooltip_text('Exit the app')
            self.navMenu.append(self.exitApp)
            self.exitReboot = gtk.MenuItem("Reboot")
            self.exitReboot.connect("activate", self.reboot)
            self.exitReboot.set_tooltip_text('Reboots the computer')
            self.navMenu.append(self.exitReboot)
            self.exitShutDown = gtk.MenuItem("Shut Down")
            self.exitShutDown.connect("activate", self.shutdown)
            self.exitShutDown.set_tooltip_text('Shuts down the computer')
            self.navMenu.append(self.exitShutDown)
            
            
            self.menuBar.append(self.navM)
            
            self.devMenu = gtk.Menu()
            self.devM = gtk.MenuItem("Device")
            self.devM.set_submenu(self.devMenu)
            self.devQuery = gtk.MenuItem("Query drive")
            self.devQuery.connect("activate", self.query, 0)
            self.devQuery.set_tooltip_text('Query the currently selected drive')
            self.devMenu.append(self.devQuery)
            self.readLog = gtk.MenuItem("View Audit Log")
            self.readLog.connect("activate", self.openLog_prompt)
            self.readLog.set_tooltip_text('Access a drive\'s event log')
            self.devMenu.append(self.readLog)
            self.devScan = gtk.MenuItem("Rescan drives")
            self.devScan.connect("activate", self.run_scan)
            self.devScan.set_tooltip_text('Update the list of detected drives')
            self.devMenu.append(self.devScan)
            self.devOpal = gtk.MenuItem("View Opal drives")
            self.devOpal.connect("activate", self.openOpal)
            self.devOpal.set_tooltip_text('View a list of detected Opal drives and their Opal versions')
            self.devMenu.append(self.devOpal)
            
            self.menuBar.append(self.devM)
            
            
            if self.VERSION != 1:
                self.setupMenu = gtk.Menu()
                self.setupM = gtk.MenuItem("Setup")
                self.setupM.set_submenu(self.setupMenu)
                self.setupFull = gtk.MenuItem("Set Up Drive(s)")
                self.setupFull.connect("activate", self.setup_prompt1)
                self.setupFull.set_tooltip_text('Set up password and preboot image for a drive')
                self.setupMenu.append(self.setupFull)
                self.updatePBA = gtk.MenuItem("Update Preboot Image")
                self.updatePBA.connect("activate", self.updatePBA_prompt)
                self.updatePBA.set_tooltip_text('Update a drive\'s preboot image')
                self.setupMenu.append(self.updatePBA)
            
                self.changePassword = gtk.MenuItem("Change Password")
                self.changePassword.connect("activate", self.changePW_prompt)
                self.changePassword.set_tooltip_text('Change a drive\'s password')
                self.setupMenu.append(self.changePassword)
                if self.VERSION % 3 == 0:
                    self.setupUSB = gtk.MenuItem("Setup USB")
                    self.setupUSB.connect("activate", self.setupUSB_prompt)
                    self.setupUSB.set_tooltip_text('Set up bootable USB')
                    self.setupMenu.append(self.setupUSB)
                    
                    self.setupUser = gtk.MenuItem("Setup User")
                    self.setupUser.connect("activate", self.setupUser_prompt)
                    self.setupUser.set_tooltip_text('Set up a user password')
                    self.setupMenu.append(self.setupUser)
                    
                    self.removeUser = gtk.MenuItem("Remove User")
                    self.removeUser.connect("activate", self.removeUser_prompt)
                    self.removeUser.set_tooltip_text('Disable the user')
                    self.setupMenu.append(self.removeUser)
            
                self.menuBar.append(self.setupM)
            
            
            self.unlockMenu = gtk.Menu()
            self.unlockM = gtk.MenuItem("Unlock")
            self.unlockM.set_submenu(self.unlockMenu)
            self.unlock1 = gtk.MenuItem("Unlock Drive(s)")
            self.unlock1.connect("activate", self.unlock_prompt)
            self.unlock1.set_tooltip_text('Unlock a drive to boot into')
            self.unlockMenu.append(self.unlock1)
        
            self.menuBar.append(self.unlockM)
            if self.VERSION != 1:
                self.lockMenu = gtk.Menu()
                self.lockM = gtk.MenuItem("Lock")
                self.lockM.set_submenu(self.lockMenu)
                self.lockD = gtk.MenuItem("Lock Drive")
                self.lockD.connect("activate", self.lock_prompt)
                self.lockD.set_tooltip_text('Lock a drive')
                self.lockMenu.append(self.lockD)
                
                self.menuBar.append(self.lockM)
            
            self.revertMenu = gtk.Menu()
            self.revertM = gtk.MenuItem("Remove Lock")
            self.revertM.set_submenu(self.revertMenu)
            self.revertO = gtk.MenuItem("and Keep Data")
            self.revertO.connect("activate", self.revert_keep_prompt)
            self.revertO.set_tooltip_text('Use password to revert the drive\'s LockingSP while keeping its data')
            self.revertMenu.append(self.revertO)
            self.revertPW = gtk.MenuItem("and Erase")
            self.revertPW.connect("activate", self.revert_erase_prompt)
            self.revertPW.set_tooltip_text('Use password to revert the drive\'s LockingSP and erase data')
            self.revertMenu.append(self.revertPW)
            self.revertPSID = gtk.MenuItem("with PSID")
            self.revertPSID.connect("activate", self.revert_psid_prompt)
            self.revertPSID.set_tooltip_text('Use drive\'s PSID to revert drive to manufacturer settings')
            self.revertMenu.append(self.revertPSID)
            
            self.menuBar.append(self.revertM)
            
            self.helpMenu = gtk.Menu()
            self.helpM = gtk.MenuItem("Help")
            self.helpM.set_submenu(self.helpMenu)
            self.help1 = gtk.MenuItem("Help")
            self.help1.set_tooltip_text('Access online help')
            self.helpMenu.append(self.help1)
            if self.VERSION != 1:
                self.updateM = gtk.MenuItem("Check for updates")
                self.updateM.set_tooltip_text('Check for an updated version of this app')
                self.helpMenu.append(self.updateM)
            self.aboutM = gtk.MenuItem("About")
            self.aboutM.connect("activate", self.show_about)
            self.aboutM.set_tooltip_text('About Fidelity Lock')
            self.helpMenu.append(self.aboutM)
            
            self.menuBar.append(self.helpM)
            
            if self.VERSION != 1 and (self.VERSION != 3 or self.MAX_DEV != sys.maxint):
                self.upgradeMenu = gtk.Menu()
                self.upgradeM = gtk.MenuItem("License")
                self.upgradeM.set_submenu(self.upgradeMenu)
                if self.VERSION == 0:
                    self.upgradeBasic = gtk.MenuItem("Upgrade to Standard")
                    self.upgradeMenu.append(self.upgradeBasic)
                if self.VERSION != 3:
                    self.upgradePro5 = gtk.MenuItem("Upgrade to Premium5")
                    self.upgradeMenu.append(self.upgradePro5)
                if self.MAX_DEV == 5 or self.VERSION == 0:
                    self.upgradePro25 = gtk.MenuItem("Upgrade to Premium25")
                    self.upgradeMenu.append(self.upgradePro25)
                if self.MAX_DEV <= 25 or self.VERSION == 0:
                    self.upgradePro100 = gtk.MenuItem("Upgrade to Premium100")
                    self.upgradeMenu.append(self.upgradePro100)
                if self.MAX_DEV <= 100 or self.VERSION == 0:
                    self.upgradeProU = gtk.MenuItem("Upgrade to PremiumUnlimited")
                    self.upgradeMenu.append(self.upgradeProU)
                self.menuBar.append(self.upgradeM)
            
            self.hbox0.pack_start(self.menuBar, True, True, 0)
            self.vbox0.pack_start(self.hbox0, False, False, 0)
            
            self.buttonBox = gtk.HBox(homogeneous, 0)
            
            self.cancel_button = gtk.Button('_Cancel')
            
            self.viewLog = gtk.Button('_View Audit Log')
            self.viewLog.set_flags(gtk.CAN_DEFAULT)
            
            self.setup_next = gtk.Button('_Set Up Password')
            
            self.setupSelect = gtk.Button('_Continue')
            
            self.setupUserPW = gtk.Button('_Set Up User Password')
            self.removeUser_button = gtk.Button('_Remove User')
            
            self.updatePBA_button = gtk.Button('_Update')
            self.updatePBA_button.set_flags(gtk.CAN_DEFAULT)
            
            self.changePW_button = gtk.Button('_Change Password')
            
            self.setupUSB_button = gtk.Button('_Create bootable USB')
            
            self.pbaUnlockReboot = gtk.Button("_Unlock and Reboot")
            self.pbaUnlockReboot.set_flags(gtk.CAN_DEFAULT)
            self.pbaUnlockOnly = gtk.Button("_Unlock only")
            self.pbaUSB_button = gtk.Button('_Unlock with USB')
            
            self.revertOnly_button = gtk.Button('_Remove Lock')
            self.revertOnly_button.set_flags(gtk.CAN_DEFAULT)
            
            self.revertUser_button = gtk.Button('_Remove Lock and Erase')
            self.revertUser_button.set_flags(gtk.CAN_DEFAULT)
            self.revertPSID_button = gtk.Button('_Remove Lock and Erase with PSID')
            
            self.lock_button = gtk.Button('_Lock Drive')
            self.lock_button.set_flags(gtk.CAN_DEFAULT)
            
            self.op_label = gtk.Label('Main')
            self.op_label.set_alignment(0,0.5)
            self.vbox.pack_start(self.op_label, False, False, 0)
            
            top_box = gtk.HBox(homogeneous, 0)
            
            self.noTCG_instr = gtk.Label('No TCG drives were detected, please insert a TCG drive and use \'Rescan devices\' to continue.')
            self.noTCG_instr.set_alignment(0,0.5)
            top_box.pack_start(self.noTCG_instr, False, False, 0)
            
            self.select_instr = gtk.Label('Select a drive from the dropdown menu.')
            self.select_instr.set_alignment(0,0.5)
            top_box.pack_start(self.select_instr, False, False, 0)
            
            self.main_instr = gtk.Label('Select an operation from the menu bar above.  Below are all detected drives.')
            self.main_instr.set_alignment(0,0.5)
            top_box.pack_start(self.main_instr, False, False, 0)
            
            self.naDevices_instr = gtk.Label('No drives available for this operation.')
            self.naDevices_instr.set_alignment(0,0.5)
            top_box.pack_start(self.naDevices_instr, False, False, 0)
            
            blank_instr = gtk.Label('')
            top_box.pack_start(blank_instr, False, False, 0)
            
            self.toggleSingle_radio = gtk.RadioButton(None, 'Single')
            self.toggleSingle_radio.connect('toggled', self.mode_toggled, 0)
            self.toggleMulti_radio = gtk.RadioButton(self.toggleSingle_radio, 'Multi')
            self.toggleMulti_radio.connect('toggled', self.mode_toggled, 1)
            top_box.pack_end(self.toggleMulti_radio, False, False, 0)
            top_box.pack_end(self.toggleSingle_radio, False, False, 0)
            
            self.vbox.pack_start(top_box, False, False, 0)
            
            self.op_instr = gtk.Label('')
            self.op_instr.set_alignment(0,0.5)
            
            self.cancel_button.connect('clicked', self.returnToMain)
            self.revertUser_button.connect('clicked', runop.run_revertErase, self)
            self.revertPSID_button.connect('clicked', runop.run_revertPSID, self)
            self.changePW_button.connect('clicked', runop.run_changePW, self)
            self.setup_next.connect('clicked', runop.run_setupFull, self)
            self.updatePBA_button.connect('clicked', runop.run_pbaWrite, self, 0)
            
            self.setupSelect.connect('clicked', runop.run_setupPBA, self)
            
            self.setupUSB_button.connect('clicked', runop.run_setupUSB, self)
            
            self.setupUserPW.connect('clicked', runop.run_setupUser, self)
            self.removeUser_button.connect('clicked', runop.run_removeUser, self)
            
            self.pbaUnlockReboot.connect("clicked", runop.run_unlockPBA, self, True)
            self.pbaUnlockOnly.connect("clicked", runop.run_unlockPBA, self, False)
            
            self.lock_button.connect('clicked', runop.run_lockDrive, self)
            
            self.revertOnly_button.connect('clicked', runop.run_revertKeep, self)
            
            self.viewLog.connect('clicked', self.openLog, self)

            self.display_single()
            
            self.display_grid()
            
            self.wait_instr = gtk.Label('This may take a few seconds...')
            self.pba_wait_instr = gtk.Label('Please wait, writing the preboot image will take a few minutes...\nDo not turn off your computer while setup is ongoing.')
            self.vbox.pack_start(self.wait_instr, False, False, 5)
            self.vbox.pack_start(self.pba_wait_instr, False, False, 5)
            
            self.waitSpin = gtk.Spinner()
            self.vbox.pack_start(self.waitSpin, False, False, 5)
            
            self.vbox.pack_start(self.op_instr, False, False, 5)
            
            self.box_psid = gtk.HBox(homogeneous, 0)
            
            self.revert_psid_label = gtk.Label("Enter PSID")
            self.revert_psid_label.set_width_chars(22)
            self.revert_psid_label.set_alignment(0,0.5)
            self.box_psid.pack_start(self.revert_psid_label, expand, fill, padding)
            
            self.revert_psid_entry = gtk.Entry()
            self.revert_psid_entry.set_text("")
            self.box_psid.pack_start(self.revert_psid_entry, True, True, padding)
            
            self.box_newpass_confirm = gtk.HBox(homogeneous, 0)
            
            self.confirm_pass_label = gtk.Label("Confirm New Password")
            self.confirm_pass_label.set_width_chars(22)
            self.confirm_pass_label.set_alignment(0,0.5)
            self.box_newpass_confirm.pack_start(self.confirm_pass_label, expand, fill, padding)
            self.confirm_pass_entry = gtk.Entry()
            self.confirm_pass_entry.set_text("")
            self.confirm_pass_entry.set_width_chars(27)
            self.confirm_pass_entry.set_visibility(False)
            self.box_newpass_confirm.pack_start(self.confirm_pass_entry, False, False, padding)
            
            self.box_revert_agree = gtk.HBox(homogeneous, 0)
            
            self.revert_agree_label = gtk.Label("Type 'I agree'")
            self.revert_agree_label.set_width_chars(22)
            self.revert_agree_label.set_alignment(0,0.5)
            self.box_revert_agree.pack_start(self.revert_agree_label, expand, fill, padding)
            self.revert_agree_entry = gtk.Entry()
            self.revert_agree_entry.set_text("")
            self.revert_agree_entry.set_width_chars(27)
            self.box_revert_agree.pack_start(self.revert_agree_entry, False, False, padding)
            
            
            if (gtk.gtk_version[1] > 24 or
                (gtk.gtk_version[1] == 24 and gtk.gtk_version[2] > 28)):
                self.drive_menu = gtk.ComboBoxEntry()
                self.auth_menu = gtk.ComboBoxEntry()
                self.authQuery = gtk.ComboBoxEntry()
                self.usb_menu = gtk.ComboBoxEntry()
            else:
                self.drive_menu = gtk.combo_box_entry_new_text()
                self.auth_menu = gtk.combo_box_entry_new_text()
                self.authQuery = gtk.combo_box_entry_new_text()
                self.usb_menu = gtk.combo_box_entry_new_text()
                
                self.drive_menu.append = self.drive_menu.append_text
                self.auth_menu.append = self.auth_menu.append_text
                self.authQuery.append = self.authQuery.append_text
                self.usb_menu.append = self.usb_menu.append_text
            
            self.box_drive = gtk.HBox(homogeneous, 0)
            
            self.drive_label = gtk.Label("USB")
            self.drive_label.set_width_chars(22)
            self.drive_label.set_alignment(0,0.5)
            self.box_drive.pack_start(self.drive_label, expand, fill, padding)
            
            self.box_drive.pack_start(self.drive_menu, False, False, padding)
            
            drv_blnk = gtk.Label('')
            self.box_drive.pack_start(drv_blnk, False, False, 0)
            
            self.drive_list = []
            
            self.box_drive.pack_start(self.usb_menu, False, False, padding)
            
            self.check_box_pass = gtk.CheckButton("Show Password")
            self.check_box_pass.connect("toggled", self.entry_check_box_pass, self.check_box_pass)
            self.check_box_pass.set_active(False)
            self.box_drive.pack_end(self.check_box_pass, False, False, 0)
            
            
            self.usb_list = []
            
            self.box_auth = gtk.HBox(homogeneous, 0)
            
            self.auth_label = gtk.Label("Auth Level")
            self.auth_label.set_width_chars(22)
            self.auth_label.set_alignment(0,0.5)
            self.box_auth.pack_start(self.auth_label, expand, fill, padding)
            self.auth_menu.append('Admin')
            self.auth_menu.append('User')
            self.authQuery.append('Admin')
            self.authQuery.append('User')
            self.auth_menu.set_active(0)
            self.authQuery.set_active(0)
            self.box_auth.pack_start(self.auth_menu, False, False, padding)
            
            
            #self.checkbox_box = gtk.HBox(homogeneous, 0)
            
            
            #self.check_box_pass.show()
            #self.checkbox_box.pack_start(self.check_box_pass, expand, fill, padding)
            
            
            #check_align = gtk.Alignment(1,0,0,0)
            #check_align.add(self.checkbox_box)
            
            self.buttonBox.pack_start(self.setup_next, False, False, padding)
            self.buttonBox.pack_start(self.setupSelect, False, False, padding)
            self.buttonBox.pack_start(self.changePW_button, False, False, padding)
            self.buttonBox.pack_start(self.revertUser_button, False, False, padding)
            self.buttonBox.pack_start(self.revertPSID_button, False, False, padding)
            self.buttonBox.pack_start(self.pbaUnlockReboot, False, False, padding)
            self.buttonBox.pack_start(self.pbaUnlockOnly, False, False, padding)
            self.buttonBox.pack_start(self.updatePBA_button, False, False, padding)
            self.buttonBox.pack_start(self.pbaUSB_button, False, False, padding)
            self.buttonBox.pack_start(self.lock_button, False, False, padding)
            self.buttonBox.pack_start(self.revertOnly_button, False, False, padding)
            self.buttonBox.pack_start(self.viewLog, False, False, padding)
            self.buttonBox.pack_start(self.setupUSB_button, False, False, padding)
            self.buttonBox.pack_start(self.setupUserPW, False, False, padding)
            self.buttonBox.pack_start(self.removeUser_button, False, False, padding)
            
            self.buttonBox.pack_start(self.cancel_button, False, False, padding)
            
            self.pass_dialog()
            self.new_pass_dialog()
            
            self.mbr_radio = gtk.RadioButton(None, 'Write Preboot Image to Shadow MBR')
            self.usb_radio = gtk.RadioButton(self.mbr_radio, 'Write Preboot Image to USB')
            self.skip_radio = gtk.RadioButton(self.mbr_radio, 'I already have another drive or USB for unlocking this drive')
            self.mbr_radio.connect('toggled', self.hideUSB)
            self.usb_radio.connect('toggled', self.showUSB)
            self.skip_radio.connect('toggled', self.hideUSB)
            
            halign = gtk.Alignment(1,0,0,0)
            halign.add(self.buttonBox)
            self.vbox.pack_end(halign, False, False, padding)
            #self.vbox.pack_end(check_align, False, False, padding)
            #self.vbox.pack_end(self.box_pbausb, False)
            self.vbox.pack_end(self.skip_radio, False)
            self.vbox.pack_end(self.usb_radio, False)
            self.vbox.pack_end(self.mbr_radio, False)
            self.vbox.pack_end(self.box_drive, False)
            self.vbox.pack_end(self.box_revert_agree, False)
            self.vbox.pack_end(self.box_psid, False)
            self.vbox.pack_end(self.box_newpass_confirm, False)
            self.vbox.pack_end(self.box_newpass, False)
            self.vbox.pack_end(self.box_pass, False)
            self.vbox.pack_end(self.box_auth, False)
            
            self.hbox.set_border_width(20)
            
            self.hbox.pack_start(self.vbox, True, True, padding)
            self.vbox0.pack_start(self.hbox, True, True, padding)
            
            self.add(self.vbox0)
            self.show_all()
            
            self.hideAll()
            self.select_box.show()
            self.box_dev.show()
            
            
            self.main_instr.show()
            self.op_label.show()
            
            dev_os = platform.system()
            
            pba_devlist = []
            
            pba_devname = None
            
            pba_devidx = -1
                
            self.run_scan()
            
            print self.devs_list
            print self.sn_list
            print self.salt_list
            print self.locked_list
            print self.setup_list
            print self.unlocked_list
            print self.nonsetup_list
            print self.tcg_list
            
            if len(self.devs_list) == 0:
                self.msg_err('No drives detected, try running this application with Administrator.')
                self.exitapp()
                
            if self.VERSION == 1 and dev_os == 'Linux':
                lic_file_regex = 'hash-(sd[a-z][0-9]*|nvme[0-9]n[0-9])-lic.txt'
                #get list of files in /tmp/h.d/ and compare regex to file names
                dir_files = os.listdir('/tmp/h.d/')
                for fn in dir_files:
                    m = re.match(lic_file_regex, fn)
                    if m:
                        lic_file = '/tmp/h.d/hash-' + m.group(1) + '-lic.txt'
                        f = open(lic_file)
                        f_info = f.read().lower()
                        f.close()
                        reg_z = '0{64}'
                        m1 = re.match(reg_z, f_info)
                        if not m1:
                            pba_devlist.append(m.group(1))
                #valid = True
                print pba_devlist
                if len(pba_devlist) > 0:
                    valid = False
                    #list of 0's of len(self.devs_list)
                    #once verified, set to 1
                    #afterwards, remove non-verified drives from lists
                    ver_list = [0] * len(self.devs_list)
                    for i in pba_devlist:
                        lic_file = '/tmp/h.d/hash-' + i + '-lic.txt'
                        if os.path.isfile(lic_file):
                            f = open(lic_file)
                            f_info = f.read().lower()
                            f.close()
                            
                            if len(f_info) == 64:
                                #change to construct levels from module
                                #change audituser password to be constructed as a global or return from module
                                levels = lockhash.get_val(1)
                                for j in range(len(self.devs_list)):
                                    salt = self.salt_list[j]
                                    count = 0
                                    for lic in levels:
                                        f_curr = lockhash.hash_pbkdf2(lic, salt)
                                        if f_curr == f_info:
                                            valid = True
                                            if count == 0:
                                                self.PBA_VERSION = 0
                                                self.MAX_DEV = sys.maxint
                                            elif count == 1:
                                                self.PBA_VERSION = 1
                                                self.MAX_DEV = 5
                                            elif count == 2:
                                                self.PBA_VERSION = 2
                                                self.MAX_DEV = 5
                                            elif count == 3:
                                                self.PBA_VERSION = 3
                                                self.MAX_DEV = 25
                                            elif count == 4:
                                                self.PBA_VERSION = 4
                                                self.MAX_DEV = 100
                                            elif count == 5:
                                                self.PBA_VERSION = 5
                                                self.MAX_DEV = sys.maxint
                                            pba_devname = i
                                            pba_devidx = j
                                            ver_list[j] = 1
                                            print lic
                                            break
                                        count = count + 1
                                    if pba_devname != None:
                                        break
                            #elif m1:
                                
                                #valid = True
                    if sum(ver_list) > 0:
                        valid = True
                        #new lists to replace the original scan results
                        dev_new = []
                        locked_new = []
                        setup_new = []
                        unlocked_new = []
                        nonsetup_new = []
                        tcg_new = []
                        usrsup_new = []
                        
                        user_new = []
                        
                        vendor_new = []
                        salt_new = []
                        sn_new = []
                        msid_new = []
                        series_new = []
                        pba_new = []
                        
                        opal_ver_new = []
                        setupstatus_new = []
                        lockstatus_new = []
                        encsup_new = []
                        blockSID_new = []
                        new_idx = 0
                        for i in range(len(ver_list)):
                            if ver_list[i] == 1:
                                dev_new.append(self.devs_list[i])
                                if i in self.locked_list:
                                    locked_new.append(new_idx)
                                if i in self.setup_list:
                                    setup_new.append(new_idx)
                                if i in self.unlocked_list:
                                    unlocked_new.append(new_idx)
                                if i in self.nonsetup_list:
                                    nonsetup_new.append(new_idx)
                                if i in self.tcg_list:
                                    tcg_new.append(new_idx)
                                if i in self.usrsup_list:
                                    usrsup_new.append(new_idx)
                                
                                user_new.append(self.user_list[i])
                                
                                vendor_new.append(self.vendor_list[i])
                                salt_new.append(self.salt_list[i])
                                sn_new.append(self.sn_list[i])
                                msid_new.append(self.msid_list[i])
                                series_new.append(self.series_list[i])
                                pba_new.append(self.pba_list[i])
                                
                                opal_ver_new.append(self.opal_ver_list[i])
                                setupstatus_new.append(self.setupstatus_list[i])
                                lockstatus_new.append(self.lockstatus_list[i])
                                encsup_new.append(self.encsup_list[i])
                                blockSID_new.append(self.blockSID_list[i])
                                
                                new_idx = new_idx + 1
                        self.devs_list = dev_new
                        self.locked_list = locked_new
                        self.setup_list = setup_new
                        self.unlocked_list = unlocked_new
                        self.nonsetup_list = nonsetup_new
                        self.tcg_list = tcg_new
                        self.usrsup_list = usrsup_new
                        
                        self.user_list = user_new
                        
                        self.vendor_list = vendor_new
                        self.salt_list = salt_new
                        self.sn_list = sn_new
                        self.msid_list = msid_new
                        self.series_list = series_new
                        self.pba_list = pba_new
                        
                        self.opal_ver_list = opal_ver_new
                        self.setupstatus_list = setupstatus_new
                        self.lockstatus_list = lockstatus_new
                        self.encsup_list = encsup_new
                        self.blockSID_list = blockSID_new
                        
                        model = self.dev_select.get_model()
        
                        iter = gtk.TreeIter
                        for row in model:
                            model.remove(row.iter)
                        
                        length = len(self.locked_list)
                        if length > 0:
                            index = self.locked_list[0]
                        else:
                            index = 0
                        for idx in range(length) :
                            self.dev_select.append( self.devs_list[self.locked_list[idx]])
                            
                        if length > 0:
                            self.dev_select.set_active(0)
                            self.dev_single.set_text(self.devs_list[index])
                            self.dev_vendor.set_text(self.vendor_list[index])
                            self.dev_sn.set_text(self.sn_list[index])
                            self.dev_series.set_text(self.series_list[index])
                            self.dev_msid.set_text(self.msid_list[index])
                            self.dev_status.set_text(self.lockstatus_list[index])
                            self.dev_setup.set_text(self.setupstatus_list[index])
                        
                        numTCG = len(self.tcg_list)
                        
                        if numTCG == 0:
                            self.noTCG_instr.show()
                        else:
                            self.noTCG_instr.hide()
                        if length > 0:
                            self.scanning = True
                            self.query(None,1)
                            self.scanning = False
                            self.dev_select.set_active(0)
                        
                        if length <= 1:
                            self.dev_select.hide()
                            self.label_dev.hide()
                            self.dev_single.show()
                            self.label_dev2.show()
                        else:
                            self.dev_single.hide()
                            self.label_dev2.hide()
                            self.dev_select.show()
                            self.label_dev.show()
                            
                    if not valid:
                        self.msg_err('Invalid boot')
                        #self.shutdown()
                        self.exitapp()
                #else:
                #    self.msg_err('Invalid boot')
                #    #self.shutdown()
                #    self.exitapp()
                
            pwd_test = ''.join(random.SystemRandom().choice(string.ascii_uppercase + string.digits) for _ in range(16))
            salt_test = self.salt_list[0]
            dev_test = self.devs_list[0]
            sedutil_txt = os.popen(self.prefix + 'sedutil-cli --hashvalidation ' + pwd_test + ' ' + dev_test).read()
            sedutil_regex = 'hashed password\s*:\s*([A-F0-9]{64})'
            sedutil_match = re.search(sedutil_regex, sedutil_txt)
            sedutil_res = sedutil_match.group(1).lower()
            hash_res = lockhash.hash_pbkdf2(pwd_test, salt_test)
            if sedutil_res != hash_res:
                self.msg_err('Hash validation failed')
                self.exitapp()
            
            if self.VERSION == 1 and dev_os == 'Linux' and pba_devname != None:
                #bus_file = '/tmp/h.d/hash-' + pba_devname + '-bus.txt'
                #ser_file = '/tmp/h.d/hash-' + pba_devname + '-ser.txt'
                #if os.path.isfile(bus_file) and os.path.isfile(ser_file):
                #    b = open(bus_file)
                #    s = open(ser_file)
                #    b_info = b.read().lower()
                #    s_info = s.read().lower()
                #    b.close()
                #    s.close()
                #    reg_z = '0{64}'
                #    m1 = re.match(reg_z, b_info)
                #    m2 = re.match(reg_z, s_info)
                #    if len(b_info) == 64 and len(s_info) == 64 and not (m1 and m2):
                #        present = False
                #        bus_list = ['FidelityLockUSB', 'FidelityLockMBR']
                #        salt = self.salt_list[pba_devidx]
                #        s_curr = lockhash.hash_pbkdf2(salt, salt)
                #        if s_curr == s_info:
                #            print 'valid ser'
                #            for bus in bus_list:
                #                b_curr = lockhash.hash_pbkdf2(bus, salt)
                #                if b_curr == b_info:
                #                    present = True
                #                    print 'valid bus'
                #                    break
                #        if not present:
                #            self.msg_err('Invalid boot')
                #            self.exitapp()
                folder_list = []
                txt = os.popen(self.prefix + 'mount').read()
                dev_regex = '/dev/sd[a-z][1-9]?\s*on\s*(\S+)\s*type'
                drive_list = re.findall(dev_regex, txt)
                txt2 = os.popen(self.prefix + 'blkid').read()
                dev_regex2 = '(/dev/sd[a-z][1-9]?.+)'
                all_list = re.findall(dev_regex2, txt2)
                r1 = '/dev/sd[a-z][1-9]?'
                r2 = 'TYPE="([a-z]+)"'
                for a in all_list:
                    m1 = re.search(r1,a)
                    m2 = re.search(r2,a)
                    if m1 != None and m2 != None:
                        dev_a = m1.group(0)
                        type_a = m2.group(1)
                        if dev_a not in drive_list:
                            s = os.system(self.prefix + 'mount -t ' + type_a + ' ' + dev_a)
                txt = os.popen(self.prefix + 'mount').read()
                dev_regex = '/dev/sd[a-z][1-9]?\s*on\s*(\S+)\s*type'
                drive_list = re.findall(dev_regex, txt)
                for d in drive_list:
                    if os.path.isdir('%s/FidelityLock' % d):
                        folder_list.append(d)
                if folder_list != []:
                    present = False
                    for i0 in folder_list:
                        for i in self.locked_list:
                            fp = i0 + '/FidelityLock/' + self.vendor_list[i] + '_' + self.sn_list[i] + '.psw'
                            if os.path.isfile(fp):
                                present = True
                    if present:
                        message = gtk.MessageDialog(type=gtk.MESSAGE_QUESTION, buttons=gtk.BUTTONS_CANCEL, parent = self)
                        msg = 'USB detected, Proceeding to unlock with USB in 5 seconds.'
                        message.set_markup(msg)
                        self.timer = gobject.timeout_add(5000, runop.run_unlockUSB, None, self, 1, message)
                        res = message.run()
                        cancel = False
                        if res == gtk.RESPONSE_CANCEL :
                            cancel = True
                        message.destroy()
                        gobject.source_remove(self.timer)
                        self.timer = 0
                        if not cancel:
                            runop.run_unlockUSB(None, self, 1, None)
            
            if self.VERSION == 0 or self.PBA_VERSION == 0:
                self.setup_next.set_sensitive(False)
                self.setupSelect.set_sensitive(False)
                self.changePW_button.set_sensitive(False)
                self.revertUser_button.set_sensitive(False)
                self.revertPSID_button.set_sensitive(False)
                self.pbaUnlockReboot.set_sensitive(False)
                self.pbaUnlockOnly.set_sensitive(False)
                self.updatePBA_button.set_sensitive(False)
                self.pbaUSB_button.set_sensitive(False)
                self.lock_button.set_sensitive(False)
                self.revertOnly_button.set_sensitive(False)
                self.viewLog.set_sensitive(False)
                self.setupUSB_button.set_sensitive(False)
                self.setupUserPW.set_sensitive(False)
            if self.VERSION == 1:
                self.unlock_prompt()
                
            self.queryWin = gtk.Window()
            self.queryWin.set_border_width(10)
            self.queryWin.set_title("Query Device")
            
            scrolledWin = gtk.ScrolledWindow()
            scrolledWin.set_policy(gtk.POLICY_NEVER, gtk.POLICY_AUTOMATIC)
            
            self.queryWin.set_default_size(400, 500)
            if os.path.isfile('icon.jpg'):
                self.queryWin.set_icon_from_file('icon.jpg')
            
            queryVbox = gtk.VBox()
            self.queryWin.add(queryVbox)
            
            queryTextView = gtk.TextView()
            queryTextView.set_editable(False)
            self.queryTextBuffer = queryTextView.get_buffer()
            self.queryTextBuffer.set_text('')
            scrolledWin.add_with_viewport(queryTextView)
            queryVbox.pack_start(scrolledWin, True, True, 0)
            
            self.query_instr = gtk.Label('Enter the device\'s password to access more query information.')
            queryVbox.pack_start(self.query_instr, False, False, 0)
            
            if self.VERSION % 3 == 0:
                self.authBoxQ = gtk.HBox(False, 0)
                authLabel = gtk.Label('Auth Level')
                self.authBoxQ.pack_start(authLabel, True, True, 0)
                self.authBoxQ.pack_start(self.authQuery, True, True,0)
                queryVbox.pack_start(self.authBoxQ, False, False, 0)
            
            self.passBoxQ = gtk.HBox(False, 0)
            plTxt = ''
            if self.VERSION == 1 and self.PBA_VERSION != 1:
                plTxt = 'Enter Admin Password'
            else:
                plTxt = 'Enter Password'
            passLabel = gtk.Label(plTxt)
            self.queryPass = gtk.Entry()
            self.queryPass.set_visibility(False)
            
            submitPass = gtk.Button('Submit')
            submitPass.connect("clicked", self.queryAuth)
            if self.VERSION == 0:
                submitPass.set_sensitive(False)
            self.passBoxQ.pack_start(passLabel, True, True, 0)
            self.passBoxQ.pack_start(self.queryPass, True, True, 0)
            self.passBoxQ.pack_start(submitPass, False, False, 0)
            queryVbox.pack_start(self.passBoxQ, False, False, 0)
            if self.VERSION != 1:
                save_instr = gtk.Label('Press \'Save to text file\' to save the query information in a file.')
                queryVbox.pack_start(save_instr, False, False, 0)
                
                querySave = gtk.Button('_Save to text file')
                querySave.connect("clicked", self.saveToText)
                if self.VERSION == 0:
                    querySave.set_sensitive(False)
                queryVbox.pack_start(querySave, False, False, 0)
            
            self.queryWin.connect('delete_event', self.hide_queryWin)
            
            self.queryWin.hide()
            
            #self.msg_ok("Hash validation passed")
            
            if dev_os == 'Windows':
                verified = powerset.verify_power()
                if 0 in self.setup_list and not verified:
                    message = gtk.MessageDialog(type=gtk.MESSAGE_INFO, buttons=gtk.BUTTONS_YES_NO, parent = self)
                    message.set_markup("Your system does not have the recommended power settings, would you like to change the settings?")
                    res = message.run()
                    if res == gtk.RESPONSE_YES:
                        message.destroy()
                        powerset.set_power()
                        self.msg_ok("Your power settings have been changed successfully.")
                    elif res == gtk.RESPONSE_NO:
                        message.destroy()
                        
            
        else:
            self.msg_err('No valid license of Fidelity Lock found, please register to get demo license or buy basic/premium license')
            self.exitapp()
           
    def display_single(self, *args):
        homogeneous = False
        spacing = 0
        expand = False
        fill = False
        padding = 0
        width = 12
        
        self.box_dev = gtk.HBox(homogeneous, spacing)
        
        dev_info = gtk.VBox(homogeneous, 5)
        opal_info = gtk.VBox(homogeneous, 5)
        
        self.select_box = gtk.HBox(homogeneous, spacing)
        
        vendor_box = gtk.HBox(homogeneous, spacing)
        sn_box = gtk.HBox(homogeneous, spacing)
        msid_box = gtk.HBox(homogeneous, spacing)
        series_box = gtk.HBox(homogeneous, spacing)
        pbaver_box = gtk.HBox(homogeneous, spacing)
        
        ssc_box = gtk.HBox(homogeneous, spacing)
        status_box = gtk.HBox(homogeneous, spacing)
        setup_box = gtk.HBox(homogeneous, spacing)
        enc_box = gtk.HBox(homogeneous, spacing)
        blockSID_box = gtk.HBox(homogeneous, spacing)
        
        self.label_dev = gtk.Label("Devices")
        self.label_dev.set_alignment(0, 0.5)
        self.label_dev.set_width_chars(14)
        self.select_box.pack_start(self.label_dev, expand, fill, padding)
        self.label_dev.show()
        
        self.label_dev2 = gtk.Label("Device") 
        self.label_dev2.set_alignment(0, 0.5)
        self.label_dev2.set_width_chars(14)
        self.select_box.pack_start(self.label_dev2, expand, fill, padding)
        self.label_dev2.show()

        if (gtk.gtk_version[1] > 24 or
            (gtk.gtk_version[1] == 24 and gtk.gtk_version[2] > 28)):
            self.dev_select = gtk.ComboBoxEntry()
        else:
            self.dev_select = gtk.combo_box_entry_new_text()
            
            self.dev_select.append = self.dev_select.append_text
            
            
        self.select_box.pack_start(self.dev_select, True, True, padding)
        
        self.dev_select.child.connect('changed', self.changed_cb)
        
        self.dev_single = gtk.Entry()
        self.dev_single.set_width_chars(35)
        self.dev_single.set_sensitive(False)

        self.select_box.pack_start(self.dev_single, True, True, padding)
        
        self.vbox.pack_start(self.select_box, False, True, padding)
        
        dev_label = gtk.Label(" Device information")
        dev_label.show()
        dev_info.pack_start(dev_label, False, False, padding)
        
        vendor_label = gtk.Label("Model Number")
        vendor_label.set_alignment(0, 0.5)
        vendor_label.set_width_chars(14)
        vendor_label.show()
        vendor_box.pack_start(vendor_label, False, False, padding)
        
        self.dev_vendor = gtk.Entry()
        self.dev_vendor.set_text("")
        self.dev_vendor.set_property("editable", False)
        self.dev_vendor.set_sensitive(False)
        self.dev_vendor.show()
        self.dev_vendor.set_width_chars(38)
        vendor_box.pack_start(self.dev_vendor, False, False, padding)
        
        dev_info.pack_start(vendor_box, True, True, padding)
        
        sn_label = gtk.Label("Serial Number")
        sn_label.set_alignment(0, 0.5)
        sn_label.set_width_chars(14)
        sn_label.show()
        sn_box.pack_start(sn_label, False, False, padding)
        
        self.dev_sn = gtk.Entry()
        self.dev_sn.set_text("")
        self.dev_sn.set_property("editable", False)
        self.dev_sn.set_sensitive(False)
        self.dev_sn.show()
        self.dev_sn.set_width_chars(38)
        sn_box.pack_start(self.dev_sn, False, False, padding)
        
        dev_info.pack_start(sn_box, True, True, padding)
        
        msid_label = gtk.Label("MSID")
        msid_label.set_alignment(0,0.5)
        msid_label.set_width_chars(14)
        msid_label.show()
        msid_box.pack_start(msid_label, False, False, padding)
        
        self.dev_msid = gtk.Entry()
        self.dev_msid.set_text("")
        self.dev_msid.set_property("editable", False)
        self.dev_msid.set_sensitive(False)
        self.dev_msid.show()
        self.dev_msid.set_width_chars(38)
        msid_box.pack_start(self.dev_msid, False, False, padding)
        
        dev_info.pack_start(msid_box, True, True, padding)
        
        series_label = gtk.Label("Firmware")
        series_label.set_alignment(0,0.5)
        series_label.set_width_chars(14)
        series_label.show()
        series_box.pack_start(series_label, False, False, padding)
        
        self.dev_series = gtk.Entry()
        self.dev_series.set_text("")
        self.dev_series.set_property("editable", False)
        self.dev_series.set_sensitive(False)
        self.dev_series.show()
        self.dev_series.set_width_chars(38)
        series_box.pack_start(self.dev_series, False, False, padding)
        
        dev_info.pack_start(series_box, True, True, padding)
        
        dev_pbalabel = gtk.Label("Preboot Image")
        dev_pbalabel.set_alignment(0,0.5)
        dev_pbalabel.set_width_chars(14)
        dev_pbalabel.show()
        pbaver_box.pack_start(dev_pbalabel, False, False, padding)
        
        self.dev_pbaVer = gtk.Entry()
        self.dev_pbaVer.set_property("editable", False)
        self.dev_pbaVer.set_sensitive(False)
        self.dev_pbaVer.set_width_chars(38)
        self.dev_pbaVer.show()
        pbaver_box.pack_start(self.dev_pbaVer, False, False, padding)
        
        
        
        dev_info.pack_start(pbaver_box, False, False, padding)
        
        
        
        #opal_info
        
        opal_label = gtk.Label(" TCG information")
        opal_label.show()
        opal_info.pack_start(opal_label, False, False, padding)
        
        label_opal_ver = gtk.Label('TCG Version')
        label_opal_ver.set_alignment(0,0.5)
        label_opal_ver.set_width_chars(14)
        label_opal_ver.show()
        ssc_box.pack_start(label_opal_ver, False, False, padding)
        
        self.dev_opal_ver = gtk.Entry()
        self.dev_opal_ver.set_text("")
        self.dev_opal_ver.set_width_chars(18)
        self.dev_opal_ver.set_property("editable", False)
        self.dev_opal_ver.set_sensitive(False)
        self.dev_opal_ver.show()
        ssc_box.pack_start(self.dev_opal_ver, False, False, padding)
        
        opal_info.pack_start(ssc_box, False, True, padding)
        
        status_label = gtk.Label("Lock Status")
        status_label.set_alignment(0,0.5)
        status_label.set_width_chars(14)
        status_label.show()
        status_box.pack_start(status_label, False, False, padding)
        
        self.dev_status = gtk.Entry()
        self.dev_status.set_text("")
        self.dev_status.set_property("editable", False)
        self.dev_status.set_sensitive(False)
        self.dev_status.set_width_chars(18)
        self.dev_status.show()
        status_box.pack_start(self.dev_status, False, False, padding)
        
        opal_info.pack_start(status_box, False, False, padding)
        
        setup_label = gtk.Label("Setup Status")
        setup_label.set_alignment(0,0.5)
        setup_label.set_width_chars(14)
        setup_label.show()
        setup_box.pack_start(setup_label, False, False, padding)
        
        self.dev_setup = gtk.Entry()
        self.dev_setup.set_text("")
        self.dev_setup.set_property("editable", False)
        self.dev_setup.set_sensitive(False)
        self.dev_setup.set_width_chars(18)
        self.dev_setup.show()
        setup_box.pack_start(self.dev_setup, False, False, padding)
        
        opal_info.pack_start(setup_box, False, False, padding)
        
        
        enc_label = gtk.Label("Encryption")
        enc_label.set_alignment(0,0.5)
        enc_label.set_width_chars(14)
        enc_label.show()
        enc_box.pack_start(enc_label, False, False, padding)
        
        self.dev_enc = gtk.Entry()
        self.dev_enc.set_text("")
        self.dev_enc.set_property("editable", False)
        self.dev_enc.set_sensitive(False)
        self.dev_enc.set_width_chars(18)
        self.dev_enc.show()
        enc_box.pack_start(self.dev_enc, False, False, padding)
        
        opal_info.pack_start(enc_box, False, False, padding)
        
        
        self.dev_blockSID = gtk.Entry()
        self.dev_blockSID.set_text("")
        self.dev_blockSID.set_property("editable", False)
        self.dev_blockSID.set_sensitive(False)
        self.dev_blockSID.set_width_chars(18)
        self.dev_blockSID.show()
        blockSID_box.pack_end(self.dev_blockSID, False, False, padding)
        
        blockSID_label = gtk.Label("Block SID")
        blockSID_label.set_alignment(0,0.5)
        blockSID_label.set_width_chars(14)
        blockSID_label.show()
        blockSID_box.pack_end(blockSID_label, False, False, padding)
        
        opal_info.pack_start(blockSID_box, False, False, padding)
        
        
        
        #self.dev_user = gtk.Entry()
        #self.dev_user.set_property("editable", False)
        #self.dev_user.set_sensitive(False)
        #self.dev_user.set_width_chars(18)
        #self.dev_user.show()
        #pbaver_box.pack_end(self.dev_user, False, False, padding)
        
        #dev_userlabel = gtk.Label("User Setup")
        #dev_userlabel.set_alignment(0,0.5)
        #dev_userlabel.set_width_chars(12)
        #dev_userlabel.show()
        #pbaver_box.pack_end(dev_userlabel, False, False, padding)
        
        #if self.VERSION != 2:
        #    dev_userlabel.hide()
        #    self.dev_user.hide()
        
        self.box_dev.pack_start(dev_info, True, True, padding)
        self.box_dev.pack_start(opal_info, False, True, padding)
        
        self.vbox.pack_start(self.box_dev, False)
        
        
    def display_grid(self,*args):
        self.selectAll_check = gtk.CheckButton('Select/Deselect All')
        self.selectAll_check.connect('toggled', self.selectAll_toggle)
        
    
        column_names = ['Select', 'Drive', 'Model Number', 'Serial Number']
        self.liststore = gtk.ListStore(bool, str, str, str)
        self.treeview = gtk.TreeView()
        
        self.tvcolumn = [None] * len(column_names)
        
        cell_check = gtk.CellRendererToggle()
        cell_check.connect('toggled', self.grid_ms_toggle)
        self.tvcolumn[0] = gtk.TreeViewColumn(column_names[0], cell_check, active=0)
        self.treeview.append_column(self.tvcolumn[0])
        
        cell_dev = gtk.CellRendererText()
        cell_dev.set_property('editable', False)
        self.tvcolumn[1] = gtk.TreeViewColumn(column_names[1], cell_dev, text=1)
        self.treeview.append_column(self.tvcolumn[1])
        
        cell_model = gtk.CellRendererText()
        cell_model.set_property('editable', False)
        self.tvcolumn[2] = gtk.TreeViewColumn(column_names[2], cell_model, text=2)
        self.treeview.append_column(self.tvcolumn[2])
        
        cell_sn = gtk.CellRendererText()
        cell_sn.set_property('editable', False)
        self.tvcolumn[3] = gtk.TreeViewColumn(column_names[3], cell_sn, text=3)
        self.treeview.append_column(self.tvcolumn[3])
        
        self.scrolledWin_grid = gtk.ScrolledWindow()
        self.scrolledWin_grid.add(self.treeview)
        self.vbox.pack_start(self.selectAll_check, False, False, 0)
        self.vbox.pack_start(self.scrolledWin_grid, True, True, 0)
        
        self.treeview.set_model(self.liststore)
        
    def selectAll_toggle(self, *args):
        self.selectAll_check.set_inconsistent(False)
        value = self.selectAll_check.get_active()
        def Toggle(store, path, itr, v):
            self.liststore[path][0] = v
        self.liststore.foreach(Toggle, value)
        
    def grid_ms_toggle(self, cell, path, data=None):
        i = self.liststore.get_iter(path)
        curr_val = self.liststore.get_value(i, 0)
        self.liststore.set_value(i, 0, not curr_val)
        
        checked = list()
        self.liststore.foreach(lambda store, path, itr: checked.append(store[path][0]))
        if all(checked):
            self.selectAll_check.set_inconsistent(False)
            self.selectAll_check.set_active(True)
        elif not any(checked):
            self.selectAll_check.set_inconsistent(False)
            self.selectAll_check.set_active(False)
        else:
            self.selectAll_check.set_inconsistent(True)
        
        
    def entry_check_box_pass(self, widget, checkbox):
        b_entry_checkbox = checkbox.get_active()
        pass_show = False
        if b_entry_checkbox:
            pass_show = True
        else:
            pass_show = False
        self.pass_entry.set_visibility(pass_show)
        self.new_pass_entry.set_visibility(pass_show)
        self.confirm_pass_entry.set_visibility(pass_show)
        return 
        
    def check_passRead(self, checkbox):
        b_entry_checkbox = checkbox.get_active()
        if b_entry_checkbox:
            self.pass_entry.set_text("")
            self.pass_entry.set_sensitive(False)
        else:
            self.pass_entry.set_sensitive(True)
            
        if self.check_pass_rd.get_active():
            if self.check_both:
                self.pass_sav.set_active(True)
                #self.box_drive.hide()
                self.drive_label.hide()
                self.drive_menu.hide()
            if self.op_prompt != 4 and self.op_prompt != 6:
                self.check_box_pass.set_active(False)
                self.check_box_pass.set_sensitive(False)
            if self.op_prompt != 6:
                self.pass_sav.set_sensitive(False)
        else:
            self.pass_sav.set_sensitive(True)
            self.check_box_pass.set_sensitive(True)
            if self.check_both:
                self.pass_sav.set_active(False)
            
    def showDrive(self, checkbox):
        b_entry_checkbox = checkbox.get_active()
        if not self.mode_setupUSB:
            if b_entry_checkbox:
                self.drive_list = []
                dev_os = platform.system()
                if dev_os == 'Windows':
                    for drive in string.ascii_uppercase:
                        if drive != 'C' and os.path.isdir('%s:\\' % drive):
                            self.drive_list.append(drive + ':')
                elif dev_os == 'Linux':
                    txt = os.popen(self.prefix + 'mount').read()
                    dev_regex = '/dev/sd[a-z][1-9]?\s*on\s*(\S+)\s*type'
                    self.drive_list = re.findall(dev_regex, txt)
                model = self.drive_menu.get_model()
                
                iter = gtk.TreeIter
                for row in model:
                    model.remove(row.iter)
                
                length = len(self.drive_list)
                
                if length > 0:
                    for d in self.drive_list:
                        if dev_os == 'Windows':
                            self.drive_menu.append(d)
                        elif dev_os == 'Linux':
                            self.drive_menu.append(d)
                    self.drive_menu.set_active(0)
                #self.box_drive.show()
                self.drive_label.show()
                self.drive_menu.show()
            else:
                #self.box_drive.hide()
                self.drive_label.hide()
                self.drive_menu.hide()
            if self.check_exclusive and self.pass_sav.get_active():
                self.check_pass_rd.set_sensitive(False)
            elif self.check_exclusive:
                self.check_pass_rd.set_sensitive(True)
        else:
            if b_entry_checkbox:
                self.box_auth.show()
                self.box_pass.show()
                self.check_box_pass.show()
            else:
                self.box_auth.hide()
                self.box_pass.hide()
                self.check_box_pass.hide()
                
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
        
    def openOpal(self, *args):
        columns = ['Device', 'Model Number', 'Serial Number', 'TCG Version']
        tcgWin = gtk.Window()
        tcgWin.set_border_width(10)
        tcgWin.set_default_size(500, 500)
        tcgWin.set_title('TCG Drives')
        if os.path.isfile('icon.jpg'):
            tcgWin.set_icon_from_file('icon.jpg')
        vbox = gtk.VBox()
        tcgWin.add(vbox)
        lsOpal = gtk.ListStore(str, str, str, str)
        tcgEntries = []
        for i in self.tcg_list:
            tcgEntries.append((self.devs_list[i], self.vendor_list[i], self.sn_list[i], self.opal_ver_list[i]))
        
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
        tcgWin.show_all()
        
    def openLog(self, button, *args):
        index = self.dev_select.get_active()
        self.devname = self.devs_list[self.tcg_list[index]]
        print "Selected drive: " + self.devname
        password = ""
        if (self.VERSION == 3 or (self.VERSION == 1 and self.PBA_VERSION != 1)) and self.check_pass_rd.get_active():
            password = runop.passReadUSB(self, self.dev_vendor.get_text(), self.dev_sn.get_text())
            if password == None or password == 'x':
                self.msg_err('No password found for the drive.')
                return
            print 'USB password: ' + password
        else:
            password = lockhash.hash_pass(self.pass_entry.get_text(), self.salt_list[self.tcg_list[index]], self.dev_msid.get_text())
            self.pass_entry.get_buffer().delete_text(0,-1)
            print 'Typed password: ' + password
        if self.VERSION % 2 == 1 and self.pass_sav.get_active():
            drive = self.drive_menu.get_active_text()
            dev_os = platform.system()
            if dev_os == 'Windows':
                drive = drive + '\\'
            if not os.path.isdir(drive):
                self.msg_err('Selected USB not detected')
                return
        txt = ""
        timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
        timeStr = timeStr[2:]
        statusAW = -1
        if self.VERSION == 3:
            auth_level = self.auth_menu.get_active()
            
            if auth_level == 0:
                statusAW = os.system(self.prefix + "sedutil-cli -n -t --auditwrite 03" + timeStr + " " + password + " Admin1 " + self.devname)
                if statusAW == 0:
                    txt = os.popen(self.prefix + "sedutil-cli -n -t -u --auditread " + password + " Admin1 " + self.devname ).read()
                
            else:
                statusAW = os.system(self.prefix + "sedutil-cli -n -t --auditwrite 05" + timeStr + " " + password + " User1 " + self.devname)
                if statusAW == 0:
                    txt = os.popen(self.prefix + "sedutil-cli -n -t -u --auditread " + password + " User1 " + self.devname ).read()
        else:
            statusAW = os.system(self.prefix + "sedutil-cli -n -t --auditwrite 03" + timeStr + " " + password + " Admin1 " + self.devname)
            if statusAW == 0:
                txt = os.popen(self.prefix + "sedutil-cli -n -t --auditread " + password + " Admin1 " + self.devname ).read()
        if statusAW == self.NOT_AUTHORIZED:
            pwd = lockhash.get_val(0) + self.salt_list[self.tcg_list[index]]
            hash_pwd = lockhash.hash_pass(pwd, self.salt_list[self.tcg_list[index]], self.msid_list[self.tcg_list[index]])
            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
            timeStr = timeStr[2:]
            if auth_level == 0:
                statusAW = os.system(self.prefix + "sedutil-cli -n -t --auditwrite 04" + timeStr + " " + hash_pwd + " User" + self.user_list[self.tcg_list[index]] + " " + self.devname)
            else:
                statusAW = os.system(self.prefix + "sedutil-cli -n -t --auditwrite 06" + timeStr + " " + hash_pwd + " User" + self.user_list[self.tcg_list[index]] + " " + self.devname)
            self.msg_err('Audit Log could not be retrieved. Invalid password.')
            return
        elif statusAW == self.AUTHORITY_LOCKED_OUT:
            pwd = lockhash.get_val(0) + self.salt_list[self.tcg_list[index]]
            hash_pwd = lockhash.hash_pass(pwd, self.salt_list[self.tcg_list[index]], self.msid_list[self.tcg_list[index]])
            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
            timeStr = timeStr[2:]
            if auth_level == 0:
                statusAW = os.system(self.prefix + "sedutil-cli -n -t --auditwrite 04" + timeStr + " " + hash_pwd + " User" + self.user_list[self.tcg_list[index]] + " " + self.devname)
            else:
                statusAW = os.system(self.prefix + "sedutil-cli -n -t --auditwrite 06" + timeStr + " " + hash_pwd + " User" + self.user_list[self.tcg_list[index]] + " " + self.devname)
            statusAW = os.system(self.prefix + "sedutil-cli -n -t --auditwrite 10" + timeStr + " " + hash_pwd + " User" + self.user_list[self.tcg_list[index]] + " " + self.devname)
            self.msg_err('Audit Log could not be retrieved. Retry limit has been reached.  Please power cycle your drive to try again.')
            return
        elif statusAW == self.SP_BUSY:
            self.msg_err('SP_BUSY')
            return
        elif statusAW != 0:
            self.msg_err('Audit Log could not be retrieved.')
            return
        else:
            auditFullRegex = 'Total Number of Audit Entries\s*:\s*([0-9]+)\n((?:.+\n?)+)'
            a = re.search(auditFullRegex, txt)
            if txt == "Invalid Audit Signature or No Audit Entry log\n" or not a:
                
                self.msg_err("Invalid Audit Signature or No Audit Entry Log or Read Error")
            else:
                if (self.VERSION == 3 or (self.VERSION == 1 and self.PBA_VERSION != 1)) and self.pass_sav.get_active():
                    print "openLog passSaveUSB " + password + " "  + self.auth_menu.get_active_text()
                    runop.passSaveUSB(self, password, self.drive_menu.get_active_text(), self.dev_vendor.get_text(), self.dev_sn.get_text())
                columns = ["Level", "Date and Time", "Event ID", "Event Description"]
                self.auditEntries = []
                self.errorEntries = []
                self.warnerrEntries = []

                logWin = gtk.Window()
                logWin.set_border_width(10)
                logWin.set_default_size(500, 500)
                logWin.set_title("Audit Log")
                if os.path.isfile('icon.jpg'):
                    logWin.set_icon_from_file('icon.jpg')
                vbox = gtk.VBox()
                logWin.add(vbox)
                
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
                
                if self.VERSION != 1:
                    self.saveLog_button = gtk.Button('Save as CSV file')
                    self.saveLog_button.connect('clicked', self.saveToCSV)
                    filter_box.pack_start(self.saveLog_button, False, False, 5)
                
                self.viewAll_button = gtk.Button('View all entries')
                self.viewAll_button.connect("clicked", runop.filterLog, self, self.auditEntries, 0)
                self.viewAll_button.set_sensitive(False)
                filter_box.pack_start(self.viewAll_button, False, False, 5)
                
                self.viewWarnErr_button = gtk.Button('View Warnings & Errors')
                self.viewWarnErr_button.connect("clicked", runop.filterLog, self, self.warnerrEntries, 1)
                filter_box.pack_start(self.viewWarnErr_button, False, False, 5)
                
                self.viewErr_button = gtk.Button('View Errors')
                self.viewErr_button.connect("clicked", runop.filterLog, self, self.errorEntries, 2)
                filter_box.pack_start(self.viewErr_button, False, False, 5)
                
                halign.add(filter_box)
                vbox.pack_start(halign, False, False, 0)
                
                logWin.show_all()
                self.returnToMain()
            
    
        
    def saveToCSV(self, *args):
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
                data.append(['Drive', self.devname])
                data.append(['Model', self.dev_vendor.get_text()])
                data.append(['Serial Number', self.dev_sn.get_text()])
                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                timeStr = timeStr[2:]
                data.append(['Time', timeStr])
                data.append(['Level', 'Date/Time', 'Event ID', 'Event Description'])
                for row in self.listStore:
                    row_data = [row[0], row[1], row[2], row[3]]
                    data.append(row_data)
                writer = csv.writer(f)
                writer.writerows(data)
            f.close()
        chooser.destroy()
    
    def run_scan(self, *args):
        if self.firstscan or self.VERSION != 1:
            if self.firstscan == False:
                model = self.dev_select.get_model()
            
                iter = gtk.TreeIter
                for row in model:
                    model.remove(row.iter)

                if len(self.vendor_list) > 0:
                #    self.vendor_list = []
                    self.opal_ver_list = []
                #    self.sn_list = []
                #    self.salt_list = []
                    self.series_list = []
                #    self.pba_list = []
                #    self.msid_list = []
                    self.lockstatus_list = []
                    self.setupstatus_list = []
            
            self.main_drive = ''
            dev_os = platform.system()
            if dev_os == 'Windows':
                txt = os.popen("powershell -NoProfile -NoExit -Command Get-Disk (Get-Partition -DriveLetter 'C').DiskNumber").read()
                disknum_regex = '\n([0-9]+)'
                m = re.search(disknum_regex, txt)
                self.main_drive = '\\\\.\\PhysicalDrive' + m.group(1)
            
            runop.finddev(self)
                
        else:
            valid_list = [0] * len(self.devs_list)
            locked_new = []
            setup_new = []
            unlocked_new = []
            nonsetup_new = []
            tcg_new = []
            
            lockstatus_new = []
            setupstatus_new = []
            for i in range(len(self.devs_list)):
                queryText = os.popen(self.prefix + 'sedutil-cli --query ' + self.devs_list[i]).read()
                txt_TCG = "Locked = "
                isTCG = re.search(txt_TCG, queryText)
                if isTCG:
                    valid_list[i] = 1
                    
                    txt_L = "Locked = Y"
                    txt_S = "LockingEnabled = Y"
                    pwd = lockhash.get_val(0) + self.salt_list[i]
                    
                    hash_pwd = lockhash.hash_pass(pwd, self.salt_list[i], self.msid_list[i])
                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                    timeStr = timeStr[2:]
                    auditText = os.popen(self.prefix + 'sedutil-cli -n -t -u --auditread ' + hash_pwd + ' User' + self.user_list[i] + ' ' + self.devs_list[i]).read()
                    m = re.search('Fidelity Audit Log', auditText)
                    
                    isLocked = re.search(txt_L, queryText)
                    isSetup = (re.search(txt_S, queryText) != None) and m
                
                    if isLocked:
                        locked_new.append(i)
                        setup_new.append(i)
                        tcg_new.append(i)
                        #if only User1 available, don't add to usrsup_list
                        #if self.user_list[i] != '1':
                        #    self.usrsup_list.append(i)
                        lockstatus_new.append("Locked")
                        setupstatus_new.append("Yes")
                    elif isSetup:
                        setup_new.append(i)
                        #if it's the main drive, don't add to unlocked_list
                        #if self.devs_list[i] != self.main_drive:
                        #    self.unlocked_list.append(i)
                        tcg_new.append(i)
                        #if self.user_list[i] != '1':
                        #    self.usrsup_list.append(i)
                        lockstatus_new.append("Unlocked")
                        setupstatus_new.append("Yes")
                    else:
                        nonsetup_new.append(i)
                        tcg_new.append(i)
                        lockstatus_new.append("Unlocked")
                        setupstatus_new.append("No")
            self.locked_list = locked_new
            self.setup_list = setup_new
            self.unlocked_list = unlocked_new
            self.nonsetup_list = nonsetup_new
            self.tcg_list = tcg_new
            
            self.lockstatus_list = lockstatus_new
            self.setupstatus_list = setupstatus_new
        
        length = 0
        index = -1
        if self.view_state == 0:
            length = len(self.devs_list)
            index = 0
            for idx in range(length) :
                self.dev_select.append( self.devs_list[idx])
        elif self.view_state == 1:
            length = len(self.locked_list)
            if length > 0:
                index = self.locked_list[0]
            else:
                index = 0
            for idx in range(length) :
                self.dev_select.append( self.devs_list[self.locked_list[idx]])
        elif self.view_state == 2:
            length = len(self.setup_list)
            if length > 0:
                index = self.setup_list[0]
            else:
                index = 0
            for idx in range(length) :
                self.dev_select.append( self.devs_list[self.setup_list[idx]])
        elif self.view_state == 3:
            length = len(self.unlocked_list)
            if length > 0:
                index = self.unlocked_list[0]
            else:
                index = 0
            for idx in range(length) :
                self.dev_select.append( self.devs_list[self.unlocked_list[idx]])
        elif self.view_state == 4:
            length = len(self.nonsetup_list)
            if length > 0:
                index = self.nonsetup_list[0]
            else:
                index = 0
            for idx in range(length) :
                self.dev_select.append( self.devs_list[self.nonsetup_list[idx]])
        elif self.view_state == 5:
            length = len(self.tcg_list)
            if length > 0:
                index = self.tcg_list[0]
            else:
                index = 0
            for idx in range(length):
                self.dev_select.append(self.devs_list[self.tcg_list[idx]])
        else:
            length = len(self.usrsup_list)
            if length > 0:
                index = self.usrsup_list[0]
            else:
                index = 0
            for idx in range(length):
                self.dev_select.append(self.devs_list[self.usrsup_list[idx]])
        
        if length > 0:
            self.dev_select.set_active(0)
            self.dev_single.set_text(self.devs_list[index])
            self.dev_vendor.set_text(self.vendor_list[index])
            self.dev_sn.set_text(self.sn_list[index])
            self.dev_series.set_text(self.series_list[index])
            self.dev_msid.set_text(self.msid_list[index])
            self.dev_status.set_text(self.lockstatus_list[index])
            self.dev_setup.set_text(self.setupstatus_list[index])
        
        numTCG = len(self.tcg_list)
        
        if numTCG == 0:
            self.noTCG_instr.show()
        else:
            self.noTCG_instr.hide()
        if length > 0:
            self.scanning = True
            self.query(None,1)
            self.scanning = False
            self.dev_select.set_active(0)
        
        if length <= 1:
            self.dev_select.hide()
            self.label_dev.hide()
            self.dev_single.show()
            self.label_dev2.show()
        else:
            self.dev_single.hide()
            self.label_dev2.hide()
            self.dev_select.show()
            self.label_dev.show()
                
        
            
        if self.firstscan:
            self.firstscan = False
        else:
            self.msg_ok('Rescan complete')
            if self.op_prompt == 1:
                self.openLog_prompt()
            elif self.op_prompt == 2:
                self.setup_prompt1()
            elif self.op_prompt == 3:
                self.updatePBA_prompt()
            elif self.op_prompt == 4:
                self.changePW_prompt()
            elif self.op_prompt == 5:
                self.setupUSB_prompt()
            elif self.op_prompt == 6:
                self.setupUser_prompt()
            elif self.op_prompt == 7:
                self.unlock_prompt()
            elif self.op_prompt == 8:
                self.lock_prompt()
            elif self.op_prompt == 9:
                self.revert_keep_prompt()
            elif self.op_prompt == 10:
                self.revert_erase_prompt()
            elif self.op_prompt == 11:
                self.revert_psid_prompt()
            elif self.op_prompt == 12:
                self.removeUser_prompt()
        
    
            
        
            
    def query(self, button, mode):
        index = -1
        if self.view_state == 0 and len(self.devs_list) > 0:
            index = self.dev_select.get_active()
        elif self.view_state == 1 and len(self.locked_list) > 0:
            index = self.locked_list[self.dev_select.get_active()]
        elif self.view_state == 2 and len(self.setup_list) > 0:
            index = self.setup_list[self.dev_select.get_active()]
        elif self.view_state == 3 and len(self.unlocked_list) > 0:
            index = self.unlocked_list[self.dev_select.get_active()]
        elif self.view_state == 4 and len(self.nonsetup_list) > 0:
            index = self.nonsetup_list[self.dev_select.get_active()]
        elif self.view_state == 5 and len(self.tcg_list) > 0:
            index = self.tcg_list[self.dev_select.get_active()]
        elif self.view_state == 6 and len(self.usrsup_list) > 0:
            index = self.usrsup_list[self.dev_select.get_active()]
        elif self.view_state == 7 and len(self.sel_list) > 0:
            index = self.nonsetup_list[self.sel_list[self.dev_select.get_active()]]
        else:
            self.msg_err('No drive selected')
            return
        self.devname = self.devs_list[index]
        
        self.dev_vendor.set_text(self.vendor_list[index])
        self.dev_sn.set_text(self.sn_list[index])
        self.dev_series.set_text(self.series_list[index])
        self.dev_msid.set_text(self.msid_list[index])
        self.dev_pbaVer.set_text(self.pba_list[index])
        
        self.dev_opal_ver.set_text(self.opal_ver_list[index])
        self.dev_status.set_text(self.lockstatus_list[index])
        self.dev_setup.set_text(self.setupstatus_list[index])
        self.dev_enc.set_text(self.encsup_list[index])
        self.dev_blockSID.set_text(self.blockSID_list[index])
        
        txt2 = ""
        if mode == 0 and index in self.tcg_list:
            txt = os.popen(self.prefix + "sedutil-cli --query " + self.devname ).read()
        
            queryTextList = ["Fidelity Lock Query information for device " + self.devname + "\n"]
            
            txtVersion = os.popen(self.prefix + "sedutil-cli --version" ).read()
            regex_ver = 'Fidelity Lock Version\s*:\s*.*'
            m = re.search(regex_ver, txtVersion)
            ver_parse = m.group()
            queryTextList.append(ver_parse + "\nGUI Version 0.09.1\n\nDevice information\n")
            
            queryTextList.append("Model: " + self.dev_vendor.get_text() + "\n")
            queryTextList.append("Serial Number: " + self.dev_sn.get_text() + "\n")
            queryTextList.append("TCG SSC: " + self.dev_opal_ver.get_text() + "\n")
            queryTextList.append("MSID: " + self.dev_msid.get_text() + "\n")
            
            txtState = os.popen(self.prefix + "sedutil-cli --getmfgstate " + self.devname).read()
            regex_sp = 'adminSP life cycle state\s*:\s*(.*)\nlockingSP life cycle state\s*:\s*(.*)'
            m = re.search(regex_sp, txtState)
            if m:
                admin_state = m.group(1)
                locking_state = m.group(2)
            
                queryTextList.append("Admin SP State: " + admin_state + "\nLocking SP State: " + locking_state + "\n\nLocking Information\n")
            
            #queryTextList.append("Lock Status: " + self.dev_status.get_text() + "\n")
            
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
            txt2 = self.devname + " " + self.dev_vendor.get_text() + "\n" + txt2
        
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
            
            self.queryWinText = ''.join(queryTextList)
        
            if not self.scanning :
                self.queryTextBuffer.set_text(self.queryWinText)
                if self.VERSION != 1:
                    self.queryPass.set_text('')
                self.queryWin.show_all()
                
            else:
                self.scanning = False
        elif mode == 0:
            self.msg_err('Non-TCG drives cannot be queried.')

    def queryAuth(self, *args):
        salt = ''
        user = ''
        index = self.dev_select.get_active()
        self.devname = self.dev_select.get_active_text()
        if self.view_state == 0:
            salt = self.salt_list[index]
            user = self.user_list[index]
        elif self.view_state == 1:
            salt = self.salt_list[self.locked_list[index]]
            user = self.user_list[self.locked_list[index]]
        elif self.view_state == 2:
            salt = self.salt_list[self.setup_list[index]]
            user = self.user_list[self.setup_list[index]]
        elif self.view_state == 3:
            salt = self.salt_list[self.unlocked_list[index]]
            user = self.user_list[self.unlocked_list[index]]
        elif self.view_state == 4:
            salt = self.salt_list[self.nonsetup_list[index]]
            user = self.user_list[self.nonsetup_list[index]]
        elif self.view_state == 5:
            salt = self.salt_list[self.tcg_list[index]]
            user = self.user_list[self.tcg_list[index]]
        elif self.view_state == 6:
            salt = self.salt_list[self.usrsup_list[index]]
            user = self.user_list[self.usrsup_list[index]]
        devpass = lockhash.hash_pass(self.queryPass.get_text(), salt, self.dev_msid.get_text())
        level = self.authQuery.get_active()
        m3 = ''
        if level == 0:
            f0 = os.popen(self.prefix + "sedutil-cli -n -t --getmbrsize " + devpass + " " + self.devname)
            p0 = f0.read()
            rc = f0.close()
            out_regex = 'Shadow.+(?:\n.+)+'
            m3 = re.search(out_regex, p0)
            if not m3:
                if rc == self.NOT_AUTHORIZED or rc == self.AUTHORITY_LOCKED_OUT:
                    pwd = lockhash.get_val(0) + salt
                    hash_pwd = lockhash.hash_pass(pwd, salt, self.dev_msid.get_text())
                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                    timeStr = timeStr[2:]
                    statusAW = os.system(self.prefix + "sedutil-cli -n -t -u --auditwrite 04" + timeStr + " " + hash_pwd + " User" + user + " " + self.devname)
                    if rc == self.AUTHORITY_LOCKED_OUT:
                        statusAW = os.system(self.prefix + "sedutil-cli -n -t -u --auditwrite 10" + timeStr + " " + hash_pwd + " User" + user + " " + self.devname)
                        self.msg_err('Error: Retry limit has been reached, please power cycle your drive to try again.')
                    else:
                        self.msg_err('Error: Invalid password')
                elif rc == self.SP_BUSY:
                    self.msg_err('SP_BUSY')
                else:
                    self.msg_err("Error: Failed to retrieve additional drive information.")
                return
        p1 = ''
        p2 = ''
        if self.VERSION != 1:
            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
            timeStr = timeStr[2:]
            if level == 1:
                statusAW = os.system(self.prefix + "sedutil-cli -n -t -u --auditwrite 05" + timeStr + " " + devpass + " User1 " + self.devname)
                if statusAW == self.NOT_AUTHORIZED or statusAW == self.AUTHORITY_LOCKED_OUT:
                    statusAW = os.system(self.prefix + "sedutil-cli -n -t -u --auditwrite 06" + timeStr + " " + devpass + " User" + user + " " + self.devname)
                    if statusAW == self.AUTHORITY_LOCKED_OUT:
                        statusAW = os.system(self.prefix + "sedutil-cli -n -t -u --auditwrite 10" + timeStr + " " + devpass + " User" + user + " " + self.devname)
                        self.msg_err("Error: Retry limit reached, please power cycle your drive to try again.")
                    else:
                        self.msg_err("Error: Invalid password")
                    return
                elif statusAW == self.SP_BUSY:
                    self.msg_err('SP_BUSY')
                    return
                elif statusAW != 0:
                    self.msg_err("Error: Failed to retrieve the information.")
                    return
                p2 = os.popen(self.prefix + "sedutil-cli -n -t -u --auditread " + devpass + " User1 " + self.devname).read()
            else:
                statusAW = os.system(self.prefix + "sedutil-cli -n -t --auditwrite 03" + timeStr + " " + devpass + " Admin1 " + self.devname)
                if statusAW == self.NOT_AUTHORIZED or statusAW == self.AUTHORITY_LOCKED_OUT:
                    statusAW = os.system(self.prefix + "sedutil-cli -n -t --auditwrite 06" + timeStr + " " + devpass + " Admin1 " + self.devname)
                    if statusAW == self.AUTHORITY_LOCKED_OUT:
                        statusAW = os.system(self.prefix + "sedutil-cli -n -t --auditwrite 10" + timeStr + " " + devpass + " Admin1 " + self.devname)
                        self.msg_err("Error: Retry limit reached, please power cycle your drive to try again.")
                    else:
                        self.msg_err("Error: Invalid password")
                    return
                elif statusAW == self.SP_BUSY:
                    self.msg_err('SP_BUSY')
                    return
                elif statusAW != 0:
                    self.msg_err("Error: Failed to retrieve the information.")
                    return
                p2 = os.popen(self.prefix + "sedutil-cli -n -t --auditread " + devpass + " Admin1 " + self.devname).read()
            if level == 0:
                p1 = os.popen(self.prefix + "sedutil-cli -n -t --pbaValid " + devpass + " " + self.devname).read()
                r = 'PBA image version \s*:'
                t = re.search(r, p1)
                if not t:
                    self.msg_err("Error: Invalid password")
                    return
        pba_regex = 'PBA image version\s*:\s*([A-z0-9\.\-]+)'
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
        if self.VERSION != 1:
            if level == 0:
                self.queryWinText = self.queryWinText + "\n" + m3.group(0) + "\nPBA Image Version: " + pba_ver + "\nAudit Log Version: " + audit_ver
            else:
                self.queryWinText = self.queryWinText + "\nAudit Log Version: " + audit_ver
        else:
            self.queryWinText = self.queryWinText + "\n" + m3.group(0) + "\nAudit Log Version: " + audit_ver
        self.queryTextBuffer.set_text(self.queryWinText)
        self.passBoxQ.hide()
        if self.VERSION % 3 == 0:
            self.authBoxQ.hide()
        self.query_instr.hide()

    def exitapp(self, *args):
        exit(0)

    def reboot(self, *args):
        if self.ostype == 0 :
            status =  os.system("shutdown -r -t 0")
        elif self.ostype == 1 :
            status =  os.system(self.prefix + "reboot now")
        elif self.ostype == 2 :
            status =  os.system(self.prefix + "reboot now")    
        exit(0)
            
    def shutdown(self, *args):
        if self.ostype == 0 :
            status =  os.system("shutdown -s -t 0")
        elif self.ostype == 1 :
            status =  os.system(self.prefix + "poweroff")
        elif self.ostype == 2 :
            status =  os.system(self.prefix + "poweroff")    
        exit(0)
    
    def changed_cb(self, entry):
        act_idx = self.dev_select.get_active()
        self.dev_single.set_text(self.dev_select.get_active_text())
        if self.view_state == 0:
            index = act_idx
        elif self.view_state == 1:
            index = self.locked_list[act_idx]
        elif self.view_state == 2:
            index = self.setup_list[act_idx]
        elif self.view_state == 3:
            index = self.unlocked_list[act_idx]
        elif self.view_state == 4:
            index = self.nonsetup_list[act_idx]
        elif self.view_state == 5:
            index = self.tcg_list[act_idx]
        elif self.view_state == 6:
            index = self.usrsup_list[act_idx]
        elif self.view_state == 7:
            index = self.nonsetup_list[self.sel_list[act_idx]]
        self.dev_single.set_text(self.devs_list[index])
        self.devname = self.devs_list[index]
        
        self.dev_vendor.set_text(self.vendor_list[index])
        self.dev_sn.set_text(self.sn_list[index])
        self.dev_msid.set_text(self.msid_list[index])
        self.dev_series.set_text(self.series_list[index])
        self.dev_pbaVer.set_text(self.pba_list[index])
        
        self.dev_opal_ver.set_text(self.opal_ver_list[index])
        self.dev_status.set_text(self.lockstatus_list[index])
        self.dev_setup.set_text(self.setupstatus_list[index])
        self.dev_enc.set_text(self.encsup_list[index])
        self.dev_blockSID.set_text(self.blockSID_list[index])
        
        if self.opal_ver_list[act_idx] != "None":
            self.scanning = True
            self.query(None,1)
            self.scanning = False
        else:
            self.dev_opal_ver.set_text("None")
            self.dev_status.set_text("N/A")
            self.dev_enc.set_text("N/A")
            self.dev_msid.set_text("N/A")
            self.dev_setup.set_text("N/A")
            self.dev_series.set_text("N/A")
            self.dev_blockSID.set_text("N/A")
            self.dev_pbaVer.set_text("N/A")
    
    def pass_dialog(self, *args):
        homogeneous = False
        spacing = 0
        expand = False
        fill = False
        padding = 0
        width = 12
        self.box_pass = gtk.HBox(homogeneous, 0)
        
        self.pass_label = gtk.Label("Enter Password")
        self.pass_label.set_width_chars(22)
        self.pass_label.set_alignment(0,0.5)
        self.box_pass.pack_start(self.pass_label, expand, fill, padding)
        
        self.pass_entry = gtk.Entry()
        self.pass_entry.set_text("")
        self.pass_entry.set_visibility(False)
        self.pass_entry.set_width_chars(27)
        self.box_pass.pack_start(self.pass_entry, False, False, padding)
        
 
        if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
            self.check_pass_rd = gtk.CheckButton("Read password from USB")
            self.check_pass_rd.connect("toggled", self.check_passRead)
            self.check_pass_rd.show()
            self.check_pass_rd.set_tooltip_text('Authenticate using the drive\'s password file from USB')
            self.box_pass.pack_end(self.check_pass_rd, False, False, padding)
     
        
        
    def new_pass_dialog(self, *args):
        homogeneous = False
        spacing = 0
        expand = False
        fill = False
        padding = 0
    
        self.box_newpass = gtk.HBox(homogeneous, 0)
        
        self.new_pass_label = gtk.Label("Enter New Password")
        self.new_pass_label.set_width_chars(22)
        self.new_pass_label.set_alignment(0,0.5)
        self.box_newpass.pack_start(self.new_pass_label, expand, fill, padding)
        self.new_pass_entry = gtk.Entry()
        self.new_pass_entry.set_text("")
        self.new_pass_entry.set_visibility(False)
        self.new_pass_entry.set_width_chars(27)
        self.box_newpass.pack_start(self.new_pass_entry, False, False, padding)
 
        if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
            self.pass_sav = gtk.CheckButton("Save to USB")
            self.pass_sav.connect("clicked", self.showDrive)
            self.pass_sav.show()
            self.pass_sav.set_tooltip_text('Save the password to a file on a USB device')
            self.box_newpass.pack_end(self.pass_sav, False, False, padding)
        
        
        
    def openLog_prompt(self, *args):
        self.hideAll()
        self.select_box.show()
        self.box_dev.show()
        self.op_label.set_text('View Audit Log')
        self.op_instr.set_text('The audit log stores a log of actions done on the drive by this application.\nEnter the drive\'s password to access its audit log.')
        self.cancel_button.show()
        
        self.op_prompt = 1
        
        if self.view_state != 5:
            self.view_state = 5
            self.changeList()
        
        if len(self.tcg_list) > 0:
            self.select_instr.show()
            self.dev_select.set_active(0)
            
        if len(self.tcg_list) == 0:
            self.naDev()
        else:
            self.op_instr.show()
            
            self.box_pass.show()
            self.pass_entry.set_activates_default(True)
            self.pass_entry.grab_focus()
            
            self.box_newpass.show()
            self.new_pass_label.hide()
            self.new_pass_entry.hide()
            
            self.box_drive.show()
            self.drive_label.hide()
            self.drive_menu.hide()
            self.usb_menu.hide()
            
            self.check_exclusive = True
            
            self.viewLog.show()
            self.set_default(self.viewLog)
            
            self.check_box_pass.show()
            
            self.query(None,1)
            if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
                self.box_auth.show()
      
    def msg_err(self, msg):
        message = gtk.MessageDialog(type=gtk.MESSAGE_ERROR, buttons=gtk.BUTTONS_OK, parent = self)
        message.set_markup(msg)
        res = message.run()
        print message.get_widget_for_response(gtk.RESPONSE_OK)
        print gtk.RESPONSE_OK
        if res == gtk.RESPONSE_OK :
            print "OK button clicked"
        message.destroy()
        
    def msg_ok(self, msg):
        message = gtk.MessageDialog(type=gtk.MESSAGE_INFO, buttons=gtk.BUTTONS_OK, parent = self)
        message.set_markup(msg)
        res = message.run()
        print message.get_widget_for_response(gtk.RESPONSE_OK)
        print gtk.RESPONSE_OK
        if res == gtk.RESPONSE_OK :
            print "OK button clicked"
        message.destroy()
        
    def setup_finish(self, *args):
        indices = []
        for i in self.sel_list:
            indices.append(self.nonsetup_list[i])
        dev_os = platform.system()
        if dev_os == 'Windows':
            verified = powerset.verify_power()
            if 0 in indices and not verified:
                message = gtk.MessageDialog(type=gtk.MESSAGE_INFO, buttons=gtk.BUTTONS_YES_NO)
                message.set_markup("We recommend changing your power settings to better secure your data, would you like to proceed with the changes?")
                res = message.run()
                if res == gtk.RESPONSE_YES:
                    message.destroy()
                    powerset.set_power()
                    self.msg_ok("Your power settings have been changed successfully.")
                elif res == gtk.RESPONSE_NO:
                    message.destroy()
        
        start = True
        liststr = ''
        for index in indices:
            if start:
                start = False
                liststr = liststr + ', '
            liststr = liststr + self.devs_list[index]
            if self.main_drive != self.devs_list[index]:
                if self.user_list[index] == '1':
                    self.updateDevs(index,[2,3])
                else:
                    self.updateDevs(index,[2,3,6])
            else:
                if self.user_list[index] == '1':
                    self.updateDevs(index,[2])
                else:
                    self.updateDevs(index,[2,6])
        if len(indices) > 1:
            self.msg_ok('Devices ' + liststr + ' have been setup successfully.')
        else:
            self.msg_ok(liststr + ' has been setup successfully.')
        self.returnToMain()

    def returnToMain(self, *args):
        self.hideAll()
        self.select_box.show()
        self.box_dev.show()
        self.op_label.set_text('Main')
        self.main_instr.show()
        self.op_prompt = 0
        self.enable_menu()
        
        if self.view_state != 0:
            self.view_state = 0
            self.changeList()
        
        if len(self.devs_list) > 0:
            self.dev_select.set_active(0)
            
        self.query(None,1)
            
        #if self.VERSION == 1:
        #    self.unlock_prompt()
            
    def setup_prompt1(self, *args):
        self.hideAll()
        self.select_box.show()
        self.box_dev.show()
        
        self.op_prompt = 1
        
        if self.VERSION == 3 or (self.VERSION == 1 and self.PBA_VERSION != 1):
            self.toggleSingle_radio.show()
            self.toggleMulti_radio.show()
            self.toggleSingle_radio.set_active(True)
        self.cancel_button.show()
        self.op_label.set_text('Set Up Drive(s)')
        self.op_instr.set_text('Setting up a drive includes setting a password which you can use to unlock the drive.\nEnter the new password for the drive and click \'Continue\'.')
        
        if self.view_state != 4:
            self.view_state = 4
            self.changeList()
        
        if len(self.nonsetup_list) > 1:
            self.select_instr.show()
            
        if len(self.nonsetup_list) > 0:
            self.dev_select.set_active(0)
            self.op_instr.show()
            
            self.box_newpass.show()
            self.new_pass_label.show()
            self.new_pass_entry.show()
            
            self.box_drive.show()
            self.drive_label.hide()
            self.drive_menu.hide()
            self.usb_menu.hide()
            
            self.box_newpass_confirm.show()
            self.setup_next.show()
            
            self.check_box_pass.show()
            
            self.query(None,1)
        else:
            self.naDevices_instr.show()
        
    def setup_prompt2(self, list_s):
        self.navM.set_sensitive(False)
        self.devM.set_sensitive(False)
        self.setupM.set_sensitive(False)
        self.revertM.set_sensitive(False)
        self.helpM.set_sensitive(False)
        self.unlockM.set_sensitive(False)
        self.lockM.set_sensitive(False)
        if self.VERSION != 3 or self.MAX_DEV != sys.maxint:
            self.upgradeM.set_sensitive(False)
    
        model = self.dev_select.get_model()
        
        iter = gtk.TreeIter
        for row in model:
            model.remove(row.iter)
    
        length = len(list_s)
    
        for i in list_s:
            self.dev_select.append(self.devs_list[i])
            self.sel_list.append(i)
        self.dev_select.set_active(0)
        self.view_state = 7
        if length == 1:
            self.dev_single.set_text(self.devs_list[list_s[0]])
            
        if length <= 1:
            self.dev_single.show()
            self.label_dev2.show()
            self.dev_select.hide()
            self.label_dev.hide()
            if length == 0:
                self.dev_single.set_text('None')
                self.dev_vendor.set_text('N/A')
                self.dev_sn.set_text('N/A')
                self.dev_msid.set_text('N/A')
                self.dev_opal_ver.set_text('N/A')
                self.dev_status.set_text('N/A')
                self.dev_setup.set_text('N/A')
                self.dev_series.set_text('N/A')
                self.dev_blockSID.set_text('N/A')
        else:
            self.dev_single.hide()
            self.label_dev2.hide()
            self.dev_select.show()
            self.label_dev.show()
        
        self.toggleSingle_radio.set_active(True)
        self.toggleSingle_radio.hide()
        self.toggleMulti_radio.hide()
        self.selectAll_check.set_inconsistent(False)
        self.selectAll_check.set_active(False)
        self.selectAll_check.hide()
        
        self.box_drive.hide()
        
        self.box_newpass.hide()
        self.box_newpass_confirm.hide()
        self.setup_next.hide()
        self.check_box_pass.hide()
        self.setupSelect.show()
        
        self.scrolledWin_grid.hide()
        self.select_box.show()
        self.box_dev.show()
        
        if self.dev_pbaVer.get_text() == 'Not Supported':
            self.usb_radio.show()
            self.usb_radio.set_active(True)
            self.op_instr.set_text('The Preboot image is used to unlock the drive for use.\nTo write the image to a USB, press \'Write Preboot Image\'.')
        else:
            self.mbr_radio.show()
            self.mbr_radio.set_active(True)
            self.op_instr.set_text('The Preboot image is used to unlock the drive for use.\nTo write the image to the shadow MBR, press \'Write Preboot Image\'.')
        self.skip_radio.show()
        self.op_instr.show()
    
        self.cancel_button.hide()
        self.query(None,1)
            
    def setupUser_prompt(self, *args):
        self.hideAll()
        self.select_box.show()
        self.box_dev.show()
        self.cancel_button.show()
        self.op_label.set_text('Set Up User Password')
        self.op_instr.set_text('Setting up a user password creates a second password with limited authority.')
        
        self.op_prompt = 6
        
        if self.view_state != 6:
            self.view_state = 6
            self.changeList()
            
        if len(self.setup_list) > 1:
            self.select_instr.show()
            
        if len(self.setup_list) > 0:
            self.dev_select.set_active(0)
            self.op_instr.show()
            self.setupUserPW.show()
            
            self.pass_label.set_text('Enter Admin Password')
            self.new_pass_label.set_text('Enter User Password')
            self.confirm_pass_label.set_text('Confirm User Password')
            self.box_pass.show()
            self.box_newpass.show()
            self.new_pass_label.show()
            self.new_pass_entry.show()
            self.box_newpass_confirm.show()
            self.check_box_pass.show()
            
            self.box_drive.show()
            self.drive_label.hide()
            self.drive_menu.hide()
            self.usb_menu.hide()
            
            self.query(None,1)
            
        else:
            self.naDevices_instr.show()
            
    def removeUser_prompt(self, *args):
        self.hideAll()
        self.select_box.show()
        self.box_dev.show()
        self.cancel_button.show()
        self.op_label.set_text('Remove User')
        self.op_instr.set_text('Disable the limited authority user account.')
        
        self.op_prompt = 12
        
        if self.view_state != 6:
            self.view_state = 6
            self.changeList()
            
        if len(self.setup_list) > 1:
            self.select_instr.show()
            
        if len(self.setup_list) > 0:
            self.dev_select.set_active(0)
            self.op_instr.show()
            self.removeUser_button.show()
            
            self.pass_label.set_text('Enter Admin Password')
            self.box_pass.show()
            self.box_newpass.show()
            self.new_pass_label.hide()
            self.new_pass_entry.hide()

            self.check_box_pass.show()
            
            self.box_drive.show()
            self.drive_label.hide()
            self.drive_menu.hide()
            self.usb_menu.hide()
            
            self.query(None,1)
            
        else:
            self.naDevices_instr.show()
        
    def updatePBA_prompt(self, *args):
        self.hideAll()
        self.select_box.show()
        self.box_dev.show()
        self.cancel_button.show()
        self.op_label.set_text('Update Preboot Image')
        self.op_instr.set_text('Use this to rewrite the Preboot Image or write the image to a set up drive.\nEnter the drive\'s password and press \'Update\'.')
        
        self.op_prompt = 3
        
        if self.view_state != 2:
            self.view_state = 2
            self.changeList()
        
        if len(self.setup_list) > 1:
            self.select_instr.show()
            
        if len(self.setup_list) > 0:
            self.dev_select.set_active(0)
            self.op_instr.show()
            self.updatePBA_button.show()
            self.set_default(self.updatePBA_button)
            
            if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
                self.pass_label.set_text('Enter Admin Password')
            self.box_pass.show()
            self.pass_entry.set_activates_default(True)
            self.pass_entry.grab_focus()
            self.box_newpass.show()
            self.new_pass_label.hide()
            self.new_pass_entry.hide()
            
            self.check_exclusive = True
            
            self.box_drive.show()
            self.drive_label.hide()
            self.drive_menu.hide()
            self.usb_menu.hide()
            self.check_box_pass.show()
            
            
            
            self.query(None,1)
        else:
            self.naDevices_instr.show()

    def changePW_prompt(self, *args):
        self.hideAll()
        self.select_box.show()
        self.box_dev.show()
        self.cancel_button.show()
        
        if self.VERSION == 3 or (self.VERSION == 1 and self.PBA_VERSION != 1):
            self.toggleSingle_radio.show()
            self.toggleMulti_radio.show()
            self.toggleSingle_radio.set_active(True)
        
        self.op_label.set_text('Change Password')
        self.op_instr.set_text('To change the password of the selected drive, enter the drive\'s current password\nand the new password.')
        
        self.op_prompt = 4
        
        if self.view_state != 2:
            self.view_state = 2
            self.changeList()
        
        if len(self.setup_list) > 1:
            self.select_instr.show()
            
        if len(self.setup_list) > 0:
            self.dev_select.set_active(0)
            self.op_instr.show()
            
            self.box_pass.show()
            self.box_newpass.show()
            self.new_pass_label.show()
            self.new_pass_entry.show()
            self.box_newpass_confirm.show()
            self.changePW_button.show()
            
            self.check_both = True
            
            self.box_drive.show()
            self.drive_label.hide()
            self.drive_menu.hide()
            self.check_box_pass.show()
            
            self.usb_menu.hide()
            
            if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
                self.box_auth.show()
            
            self.query(None,1)
        else:
            self.naDevices_instr.show()
            
    def setupUSB_prompt(self, *args):
        self.hideAll()
        self.select_box.show()
        self.box_dev.show()
        self.cancel_button.show()
        self.op_label.set_text('Set up USB')
        self.op_instr.set_text('This will write the bootable image to a USB drive.  You can then use the USB drive to unlock the selected drive.\nWARNING: Setting up the USB will erase its contents, use an empty USB.')
        
        self.op_prompt = 5
        
        self.mode_setupUSB = True
        
        if self.view_state != 2:
            self.view_state = 2
            self.changeList()
            
        if len(self.setup_list) > 0:
            self.dev_select.set_active(0)
            self.query(None,1)
            if len(self.setup_list) > 1:
                self.select_instr.show()
            
            self.usb_list = []
            dev_os = platform.system()
            if dev_os == 'Windows':
                txt = os.popen(self.prefix + 'wmic diskdrive list brief /format:list').read()
                mod_regex = 'DeviceID=.+([1-9]|1[0-5])\s*\nModel=(.*)\r'
                self.usb_list = re.findall(mod_regex, txt)
            elif dev_os == 'Linux':
                txt = os.popen(self.prefix + 'mount').read()
                dev_regex = '/dev/sd[a-z][1-9]?\s*on\s*(\S+)\s*type'
                drive_list = re.findall(dev_regex, txt)
                
                txt2 = os.popen(self.prefix + 'blkid').read()
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
                        s = os.system(self.prefix + 'mount -t ' + type_a + ' ' + dev_a)
                        drive_list.append(dev_a)
                txt3 = os.popen('mount').read()
                dev_regex3 = '(/dev/sd[a-z][1-9]?)\s*on\s*(\S+)\s*type'
                self.usb_list = re.findall(dev_regex3, txt3)
            model = self.usb_menu.get_model()
            
            iter = gtk.TreeIter
            for row in model:
                model.remove(row.iter)
            
            length = len(self.usb_list)
            
            if length > 0:
                
                for d in self.usb_list:
                    if dev_os == 'Windows':
                        self.usb_menu.append(d[1])
                    elif dev_os == 'Linux':
                        self.usb_menu.append(d[0])
                #self.box_pbausb.show()
                self.usb_menu.set_active(0)
                self.op_instr.show()
                self.setupUSB_button.show()
                
                self.box_newpass.show()
                self.new_pass_label.hide()
                self.new_pass_entry.hide()
                
                self.box_drive.show()
                self.drive_label.show()
                self.drive_menu.hide()
                self.usb_menu.show()
            else:
                self.naDevices_instr.show()
        
        else:
            self.naDevices_instr.show()
        
    def revert_erase_prompt(self, *args):
        self.hideAll()
        self.select_box.show()
        self.box_dev.show()
        
        if self.VERSION == 3 or (self.VERSION == 1 and self.PBA_VERSION != 1):
            self.toggleSingle_radio.show()
            self.toggleMulti_radio.show()
            self.toggleSingle_radio.set_active(True)
        
        self.op_label.set_text('Remove Lock and Erase All Data')
        self.op_instr.set_text('Revert with Password reverts the drive\'s LockingSP.\nThis resets the drive\'s password and disables locking.\nEnter the drive\'s password and choose whether or not to erase all data.')
        self.cancel_button.show()
        
        self.op_prompt = 10
        
        if self.view_state != 2:
            self.view_state = 2
            self.changeList()
        
        if len(self.setup_list) > 1:
            self.select_instr.show()
            
        if len(self.setup_list) > 0:
            self.dev_select.set_active(0)
            self.op_instr.show()
            
            if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
                self.pass_label.set_text('Enter Admin Password')
            self.box_pass.show()
            self.box_revert_agree.show()
            self.pass_entry.set_activates_default(True)
            self.pass_entry.grab_focus()
            
            self.revertUser_button.show()
            self.set_default(self.revertUser_button)
            
            self.box_drive.show()
            self.drive_label.hide()
            self.drive_menu.hide()
            self.usb_menu.hide()
            self.check_box_pass.show()
            
            self.query(None,1)  
        else:
            self.naDevices_instr.show()
        
    def revert_psid_prompt(self, *args):
        self.hideAll()
        self.select_box.show()
        self.box_dev.show()
        self.cancel_button.show()
        self.op_label.set_text('Remove Lock and Erase all Data with PSID')
        self.op_instr.set_text('Reverting with PSID reverts the drive to manufacturer settings and erases all data.\nEnter the drive\'s PSID and press \'Remove Lock with PSID\'.')
        
        self.op_prompt = 11
        
        if self.view_state != 5:
            self.view_state = 5
            self.changeList()
            
        if len(self.tcg_list) > 1:
            self.select_instr.show()
            
        if len(self.tcg_list) > 0:
            self.dev_select.set_active(0)
            self.op_instr.show()
            
            self.box_psid.show()
            self.box_revert_agree.show()
            self.revertPSID_button.show()
            self.revert_psid_entry.set_text("")
            
            self.query(None,1)
        else:
            self.naDevices_instr.show()
        
    def unlock_prompt(self, *args):
        self.hideAll()
        self.select_box.show()
        self.box_dev.show()
        
        
        if self.VERSION == 3 or (self.VERSION == 1 and self.PBA_VERSION != 1):
            self.toggleSingle_radio.show()
            self.toggleMulti_radio.show()
            self.toggleSingle_radio.set_active(True)
        
        self.op_label.set_text('Preboot Unlock')
        self.op_instr.set_text('Preboot Unlock unlocks a drive for bootup.\nEnter the drive\'s password and press \'Preboot Unlock\'\nAfterwards, reboot into the unlocked drive.')
        self.cancel_button.show()
        
        self.op_prompt = 7
        
        if self.view_state != 1:
            self.view_state = 1
            self.changeList()
            
        if len(self.locked_list) > 1:
            self.select_instr.show()
            
        if len(self.locked_list) > 0:
            self.dev_select.set_active(0)
            self.op_instr.show()

            self.box_pass.show()
            self.pass_entry.set_activates_default(True)
            self.pass_entry.grab_focus()
            
            self.box_newpass.show()
            self.new_pass_label.hide()
            self.new_pass_entry.hide()
            
            self.check_exclusive = True
            
            self.pbaUnlockReboot.show()
            self.pbaUnlockOnly.show()
            self.set_default(self.pbaUnlockReboot)
            
            self.box_drive.show()
            self.drive_label.hide()
            self.drive_menu.hide()
            self.usb_menu.hide()
            self.check_box_pass.show()
            
            
            
        
            self.query(None,1)
            if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
                self.box_auth.show()
        else:
            self.naDevices_instr.show()
                
                
        if len(self.locked_list) > 1:
            self.select_instr.show()
                  
    def revert_keep_prompt(self, *args):
        self.hideAll()
        self.select_box.show()
        self.box_dev.show()
        
        
        if self.VERSION == 3 or (self.VERSION == 1 and self.PBA_VERSION != 1):
            self.toggleSingle_radio.show()
            self.toggleMulti_radio.show()
            self.toggleSingle_radio.set_active(True)
        
        self.op_label.set_text('Remove Lock and Keep Data')
        self.op_instr.set_text('Reverting a drive disables locking and resets the drive password.\nEnter the password and press \'Remove Lock\'.')
        self.cancel_button.show()
        
        self.op_prompt = 9
        
        if self.view_state != 2:
            self.view_state = 2
            self.changeList()

        if len(self.setup_list) > 1:
            self.select_instr.show()
            
        if len(self.setup_list) > 0:
            self.dev_select.set_active(0)
            self.op_instr.show()

            if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
                self.pass_label.set_text('Enter Admin Password')
            self.box_pass.show()
            self.pass_entry.set_activates_default(True)
            self.pass_entry.grab_focus()
            
            self.revertOnly_button.show()
            self.set_default(self.revertOnly_button)
            
            self.box_drive.show()
            self.drive_label.hide()
            self.drive_menu.hide()
            self.usb_menu.hide()
            self.check_box_pass.show()
            
            
            self.query(None,1)
        else:
            self.naDevices_instr.show()
            

    def unlockUSB_prompt(self, *args):
        self.hideAll()
        self.select_box.show()
        self.box_dev.show()
        
        self.op_label.set_text('Unlock with USB')
        self.op_instr.set_text('Unlock with USB reads passwords from files on your USB to unlock device(s).\nMake sure your USB is mounted and press \'Unlock with USB\'.')
        self.cancel_button.show()
        
        if self.view_state != 1:
            self.view_state = 1
            self.changeList()
            
        if len(self.locked_list) > 0:
            self.dev_select.set_active(0)
            self.op_instr.show()

            self.pbaUSB_button.show()
            
            self.query(None,1)
            
        else:
            self.naDevices_instr.show()

            
    def lock_prompt(self, *args):
        self.hideAll()
        self.select_box.show()
        self.box_dev.show()
        
        self.op_label.set_text('Lock')
        self.op_instr.set_text('This operation enables locking on a drive.\nEnter the selected drive\'s password and press \'Enable Lock\'.')
        
        self.cancel_button.show()
        
        self.op_prompt = 8
        
        if self.view_state != 3:
            self.view_state = 3
            self.changeList()
		
        if len(self.unlocked_list) > 1:
            self.select_instr.show()
            
        if len(self.unlocked_list) > 0:
            self.dev_select.set_active(0)
            self.op_instr.show()
            self.lock_button.show()
            self.set_default(self.lock_button)
            
            if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
                self.pass_label.set_text('Enter Admin Password')
            self.box_pass.show()
            self.pass_entry.set_activates_default(True)
            self.pass_entry.grab_focus()
            
            self.box_newpass.show()
            self.new_pass_label.hide()
            self.new_pass_entry.hide()
            
            self.check_exclusive = True
            
            self.box_drive.show()
            self.drive_label.hide()
            self.drive_menu.hide()
            self.usb_menu.hide()
            self.check_box_pass.show()
            
            
            
            self.query(None,1)
            
        else:
            self.naDevices_instr.show()
            
    def show_about(self, *args):
        aboutWin = gtk.Window()
        txtVersion = os.popen(self.prefix + "sedutil-cli --version" ).read()
        regex_ver = 'Fidelity Lock Version\s*:\s*.*'
        m = re.search(regex_ver, txtVersion)
        ver_parse = m.group()
        about_label = gtk.Label(ver_parse)
        aboutWin.add(about_label)
        aboutWin.show_all()
        
        

    def hideAll(self, *args):
        self.select_box.hide()
        self.box_dev.hide()
        
        self.scrolledWin_grid.hide()
        
        self.toggleMulti_radio.hide()
        self.toggleSingle_radio.hide()
    
        self.select_instr.hide()
        self.naDevices_instr.hide()
        self.main_instr.hide()
        self.wait_instr.hide()
        self.pba_wait_instr.hide()
        self.op_instr.hide()
    
        self.pass_entry.set_text("")
        self.new_pass_entry.set_text("")
        self.confirm_pass_entry.set_text("")
        self.revert_psid_entry.set_text("")
        self.revert_agree_entry.set_text("")
        self.check_box_pass.set_active(False)
        
        self.box_pass.hide()
        self.box_psid.hide()
        self.box_revert_agree.hide()
        self.cancel_button.hide()
        self.revertUser_button.hide()
        self.revertPSID_button.hide()
        self.changePW_button.hide()
        self.setupSelect.hide()
        self.pbaUnlockReboot.hide()
        self.pbaUnlockOnly.hide()
        self.updatePBA_button.hide()
        self.setupUSB_button.hide()
        self.setupUserPW.hide()
        self.removeUser_button.hide()
        
        self.revertOnly_button.hide()
        self.viewLog.hide()
        self.pbaUSB_button.hide()
        self.lock_button.hide()
        
        self.box_newpass.hide()
        self.box_newpass_confirm.hide()
        
        self.pass_label.set_text('Enter Password')
        self.new_pass_label.set_text('Enter New Password')
        self.confirm_pass_label.set_text('Confirm New Password')
        
        self.mbr_radio.hide()
        self.usb_radio.hide()
        self.skip_radio.hide()
        
        self.box_drive.hide()
        self.box_auth.hide()
        #self.box_pbausb.hide()
        self.setup_next.hide()
        
        self.check_box_pass.hide()
        
        self.auth_menu.set_active(0)
        
        self.check_exclusive = False
        self.check_both = False
        
        if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
            self.pass_sav.set_sensitive(True)
            self.pass_sav.set_active(False)
            self.check_pass_rd.set_sensitive(True)
            self.check_pass_rd.set_active(False)
        
        self.waitSpin.hide()
        
        self.selectAll_check.hide()
        
        self.sel_list = []
        
        self.pass_entry.set_activates_default(False)
        self.set_default(None)
        
        self.warned = False
        self.orig = ''
        
        self.mode_setupUSB = False
        
        self.pass_usb = ''
        
    def start_spin(self, *args):
        self.op_instr.hide()
        self.disable_menu()
        self.waitSpin.show()
        self.waitSpin.start()
        
    def stop_spin(self, *args):
        self.enable_menu()
        self.waitSpin.stop()
        self.waitSpin.hide()
        self.op_instr.show()
        self.pba_wait_instr.hide()
        self.wait_instr.hide()
        
    def mode_toggled(self, button, res):
        if res == 0:
            self.scrolledWin_grid.hide()
            self.select_box.show()
            self.box_dev.show()
            
            self.selectAll_check.hide()
            if (self.view_state == 1 and len(self.locked_list) > 1) or (self.view_state == 4 and len(self.nonsetup_list) > 1) or (self.view_state == 2 and len(self.setup_list) > 1):
                self.select_instr.show()
            #else:
            #    self.naDevices_instr.show()
        else:
            self.box_dev.hide()
            
            self.select_box.hide()
            self.scrolledWin_grid.show()
            self.liststore.clear()
            if self.view_state == 1 and len(self.locked_list) > 0:
                self.selectAll_check.show()
                for i in self.locked_list:
                    self.liststore.append([False, self.devs_list[i], self.vendor_list[i], self.sn_list[i]])
            elif self.view_state == 2 and len(self.setup_list) > 0:
                self.selectAll_check.show()
                for i in self.setup_list:
                    self.liststore.append([False, self.devs_list[i], self.vendor_list[i], self.sn_list[i]])
            elif self.view_state == 4 and len(self.nonsetup_list) > 0:
                self.selectAll_check.show()
                for i in self.nonsetup_list:
                    self.liststore.append([False, self.devs_list[i], self.vendor_list[i], self.sn_list[i]])
        
    def hideUSB(self, *args):
        self.box_drive.hide()
        
    def showUSB(self, *args):
        self.usb_list = []
        dev_os = platform.system()
        if dev_os == 'Windows':
            txt = os.popen(self.prefix + 'wmic diskdrive list brief /format:list').read()
            mod_regex = 'DeviceID=.+([1-9]|1[0-5])\s*\nModel=(.*)\r'
            self.usb_list = re.findall(mod_regex, txt)
        elif dev_os == 'Linux':
            txt = os.popen(self.prefix + 'mount').read()
            dev_regex = '/dev/sd[a-z][1-9]?\s*on\s*(\S+)\s*type'
            drive_list = re.findall(dev_regex, txt)
            
            txt2 = os.popen(self.prefix + 'blkid').read()
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
                    s = os.system(self.prefix + 'mount -t ' + type_a + ' ' + dev_a)
                    drive_list.append(dev_a)
            txt3 = os.popen('mount').read()
            dev_regex3 = '(/dev/sd[a-z][1-9]?)\s*on\s*(\S+)\s*type'
            self.usb_list = re.findall(dev_regex3, txt3)
        model = self.usb_menu.get_model()
        
        iter = gtk.TreeIter
        for row in model:
            model.remove(row.iter)
        
        length = len(self.usb_list)
        
        if length > 0:
            
            for d in self.usb_list:
                if dev_os == 'Windows':
                    self.usb_menu.append(d[1])
                elif dev_os == 'Linux':
                    self.usb_menu.append(d[0])
        self.box_drive.show()
        self.drive_label.show()
        self.drive_menu.hide()
        self.usb_menu.show()
        
    def disable_menu(self, *args):
        self.navM.set_sensitive(False)
        self.devM.set_sensitive(False)
        self.revertM.set_sensitive(False)
        self.helpM.set_sensitive(False)
        self.unlockM.set_sensitive(False)
        if self.VERSION != 1:
            self.lockM.set_sensitive(False)
            self.setupM.set_sensitive(False)
            if self.VERSION != 3 or self.MAX_DEV != sys.maxint:
                self.upgradeM.set_sensitive(False)
        
        self.cancel_button.set_sensitive(False)
        self.revertUser_button.set_sensitive(False)
        self.revertPSID_button.set_sensitive(False)
        self.changePW_button.set_sensitive(False)
        self.setupSelect.set_sensitive(False)
        self.setupUSB_button.set_sensitive(False)
        self.pbaUnlockReboot.set_sensitive(False)
        self.pbaUnlockOnly.set_sensitive(False)
        self.updatePBA_button.set_sensitive(False)
        self.pbaUSB_button.set_sensitive(False)
        self.lock_button.set_sensitive(False)
        self.revertOnly_button.set_sensitive(False)
        self.viewLog.set_sensitive(False)
        
        self.mbr_radio.set_sensitive(False)
        self.usb_radio.set_sensitive(False)
        self.skip_radio.set_sensitive(False)
        
        self.box_auth.set_sensitive(False)
        self.box_pass.set_sensitive(False)
        self.box_newpass.set_sensitive(False)
        self.box_newpass_confirm.set_sensitive(False)
        self.box_drive.set_sensitive(False)
        self.box_auth.set_sensitive(False)
        self.setup_next.set_sensitive(False)
        self.box_revert_agree.set_sensitive(False)
        
        self.toggleMulti_radio.set_sensitive(False)
        self.toggleSingle_radio.set_sensitive(False)
        
        
    def enable_menu(self, *args):
        self.navM.set_sensitive(True)
        self.devM.set_sensitive(True)
        self.revertM.set_sensitive(True)
        self.helpM.set_sensitive(True)
        self.unlockM.set_sensitive(True)
        if self.VERSION != 1:
            self.lockM.set_sensitive(True)
            self.setupM.set_sensitive(True)
            if self.VERSION != 3 or self.MAX_DEV != sys.maxint:
                self.upgradeM.set_sensitive(True)
        
        self.cancel_button.set_sensitive(True)
        self.revertUser_button.set_sensitive(True)
        self.revertPSID_button.set_sensitive(True)
        self.changePW_button.set_sensitive(True)
        self.setupSelect.set_sensitive(True)
        self.setupUSB_button.set_sensitive(True)
        self.pbaUnlockReboot.set_sensitive(True)
        self.pbaUnlockOnly.set_sensitive(True)
        self.updatePBA_button.set_sensitive(True)
        self.pbaUSB_button.set_sensitive(True)
        self.lock_button.set_sensitive(True)
        self.revertOnly_button.set_sensitive(True)
        self.viewLog.set_sensitive(True)
        
        self.mbr_radio.set_sensitive(True)
        self.usb_radio.set_sensitive(True)
        self.skip_radio.set_sensitive(True)
        
        self.box_pass.set_sensitive(True)
        self.box_newpass.set_sensitive(True)
        self.box_newpass_confirm.set_sensitive(True)
        self.box_drive.set_sensitive(True)
        self.box_auth.set_sensitive(True)
        self.setup_next.set_sensitive(True)
        self.box_revert_agree.set_sensitive(True)
        
        self.toggleMulti_radio.set_sensitive(True)
        self.toggleSingle_radio.set_sensitive(True)
        
    def updateDevs(self, index, add, *args):
        if index in self.locked_list:
            self.locked_list.remove(index)
        if index in self.setup_list:
            self.setup_list.remove(index)
        if index in self.unlocked_list:
            self.unlocked_list.remove(index)
        if index in self.nonsetup_list:
            self.nonsetup_list.remove(index)
        if index in self.usrsup_list:
            self.usrsup_list.remove(index)
        for j in add:
            if j == 1:
                self.locked_list.append(index)
            elif j == 2:
                self.setup_list.append(index)
            elif j == 3:
                self.unlocked_list.append(index)
            elif j == 4:
                self.nonsetup_list.append(index)
            elif j == 6:
                self.usrsup_list.append(index)
                
        self.locked_list.sort()
        self.setup_list.sort()
        self.unlocked_list.sort()
        self.nonsetup_list.sort()
        self.usrsup_list.sort()
        
    def changeList(self, *args):
        model = self.dev_select.get_model()
        
        iter = gtk.TreeIter
        for row in model:
            model.remove(row.iter)
        
        length = 0
        
        if self.view_state == 0:
            length = len(self.devs_list)
            for i in range(length):
                self.dev_select.append(self.devs_list[i])
            if length >= 1:
                self.dev_single.set_text(self.devs_list[0])
        elif self.view_state == 1:
            length = len(self.locked_list)
            for i in range(length):
                self.dev_select.append(self.devs_list[self.locked_list[i]])
            if length >= 1:
                self.dev_single.set_text(self.devs_list[self.locked_list[0]])
        elif self.view_state == 2:
            length = len(self.setup_list)
            for i in range(length):
                self.dev_select.append(self.devs_list[self.setup_list[i]])
            if length >= 1:
                self.dev_single.set_text(self.devs_list[self.setup_list[0]])
        elif self.view_state == 3:
            length = len(self.unlocked_list)
            for i in range(length):
                self.dev_select.append(self.devs_list[self.unlocked_list[i]])
            if length >= 1:
                self.dev_single.set_text(self.devs_list[self.unlocked_list[0]])
        elif self.view_state == 4:
            length = len(self.nonsetup_list)
            for i in range(length):
                self.dev_select.append(self.devs_list[self.nonsetup_list[i]])
            if length >= 1:
                self.dev_single.set_text(self.devs_list[self.nonsetup_list[0]])
        elif self.view_state == 5:
            length = len(self.tcg_list)
            for i in range(length):
                self.dev_select.append(self.devs_list[self.tcg_list[i]])
            if length >= 1:
                self.dev_single.set_text(self.devs_list[self.tcg_list[0]])
        elif self.view_state == 6:
            length = len(self.usrsup_list)
            for i in range(length):
                self.dev_select.append(self.devs_list[self.usrsup_list[i]])
            if length >= 1:
                self.dev_single.set_text(self.devs_list[self.usrsup_list[0]])
            
        
        if length <= 1:
            self.dev_single.show()
            self.label_dev2.show()
            self.dev_select.hide()
            self.label_dev.hide()
            if length == 0:
                self.dev_single.set_text('None')
                self.dev_vendor.set_text('N/A')
                self.dev_sn.set_text('N/A')
                self.dev_msid.set_text('N/A')
                self.dev_opal_ver.set_text('N/A')
                self.dev_status.set_text('N/A')
                self.dev_setup.set_text('N/A')
                self.dev_series.set_text('N/A')
                self.dev_blockSID.set_text('N/A')
        else:
            self.dev_single.hide()
            self.label_dev2.hide()
            self.dev_select.show()
            self.label_dev.show()
            
    def hide_queryWin(event, self, widget):
        self.hide()
        return True
        
    def destroy_main(event, self, widget):
        gtk.main_quit()
        return False
                        
    def run(self):
        ''' Run the app. '''
        
        gtk.main()

app = LockApp()
app.run()