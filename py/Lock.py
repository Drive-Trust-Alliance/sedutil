import gtk
import os
import re
import sys
from sys import platform
import getopt
import hashlib
import gobject
import subprocess
import time
import datetime
#from __future__ import print_function
#import random 


# need to figure out how to get the entry's value in this sub

def make_hbox(homogeneous, spacing, expand, fill, padding,lab,ent):

    # Create a new hbox with the appropriate homogeneous
    # and spacing settings
    box = gtk.HBox(homogeneous, spacing)

    # Create a series of buttons with the appropriate settings
    button = gtk.Button(lab)
    box.pack_start(button, expand, fill, padding)
    button.show()


    txt = gtk.Entry()
    txt.set_text(ent)
    box.pack_start(txt, True, True, padding)
    # button.show() # entry do not need show()
    return box
    
    
class LockApp(gtk.Window):
    ''' An application for pyGTK.  Instantiate
        and call the run method to run. '''
    print ('gtk version: ',gtk.gtk_version)
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
                              2 : 'Admin Login',
                              3 : 'User Login',
                              4 : 'RevertSP without erase data',
                              5 : 'RevertSP with erase data',
                              6 : 'Revert using PSID (erase data)',
                              7 : 'GenKey',
                              8 : 'Cryptographic Erase',
                              9 : 'Numerous failed authentication attempts, Datastore cannot be written',
                              10: 'Password changed for SID',
                              11: 'Password changed for Admins',
                              12: 'Password changed for Users',
                              13: 'Attempt to RevertSP',
                              14: 'RevertSP failed',
                              15: 'Attempt to Revert',
                              16: 'Revert failed',
                              17: 'Attempt to revert using PSID',
                              18: 'Revert using PSID failed'})
                   
    image = "LINUXRelease.img"

    LKRNG = "0" 
    LKRNG_SLBA = "0"
    LKRNG_LBALEN = "0"
    LKATTR = "RW"

    VERSION = 3
    
    queryWinText = ""
     
    try:
        opts, args = getopt.getopt(sys.argv[1:], [""], ["pba", "demo", "basic"])
    except getopt.GetoptError, err:
        print "Update the usage information"
        exit(2)

    for o, a in opts:
        if o in ("--demo"):
            VERSION = 0
        elif o in ("--pba"):
            VERSION = 1
        elif o in ("--basic"):
            VERSION = 2
        else:
            VERSION = 3
   
    devs_list = [] #[ "/dev/sda" , "/dev/sdb",  "/dev/sdc",  "/dev/sdd",  "/dev/sde" ]
    locked_list = []
    setup_list = []
    unlocked_list = []
    nonsetup_list = []
    
    vendor_list = [] # "Sandisk" 
    opal_ver_list = [] # 1, 2, or 12
    series_list = [] # series number 
    sn_list = []
    tcg_list = []
    force_true = False # test only to avoid confusion when developing code # True
    scanning = False
    view_state = 0
    
    def __init__(self):
        # Initialize window
        gtk.Window.__init__(self)
        if self.VERSION == 0:
            self.set_title('Fidelity Lock Disk Drive Security Manager - Demo version')
        elif self.VERSION == 1:
            self.set_title('Fidelity Lock Disk Drive Security Manager - PBA version')
        elif self.VERSION == 2:
            self.set_title('Fidelity Lock Disk Drive Security Manager - Basic version')
        else:
            self.set_title('Fidelity Lock Disk Drive Security Manager - Pro version')
        if os.path.isfile('icon.jpg'):
            self.set_icon_from_file('icon.jpg')

        height = 475
        width = 550
            
        self.set_size_request(width, height)

        self.connect('destroy', gtk.main_quit)
        
        # new hbox variable for appropriate homogeneous
        # and spacing settings
        homogeneous = False
        spacing = 0
        expand = False
        fill = False
        padding = 0

 
        # Structure the layout vertically
        self.vbox0 = gtk.VBox(False,0)
        self.hbox0 = gtk.HBox(False,0)
        self.hbox = gtk.HBox(False,0)
        self.vbox = gtk.VBox(False,5)
        
        self.menuBar = gtk.MenuBar()
        
        self.navMenu = gtk.Menu()
        self.navM = gtk.MenuItem("View")
        self.navM.set_submenu(self.navMenu)
        self.backToMain = gtk.MenuItem("Main")
        self.backToMain.connect("activate", self.returnToMain)
        self.navMenu.append(self.backToMain)
        self.readLog = gtk.MenuItem("Audit Log")
        self.readLog.connect("activate", self.openLog_prompt)
        self.navMenu.append(self.readLog)
        self.exitApp = gtk.MenuItem("Exit")
        self.exitApp.connect("activate", self.exitapp)
        self.navMenu.append(self.exitApp)
        
        self.menuBar.append(self.navM)
        
        self.devMenu = gtk.Menu()
        self.devM = gtk.MenuItem("Device")
        self.devM.set_submenu(self.devMenu)
        self.devQuery = gtk.MenuItem("Query device")
        self.devQuery.connect("activate", self.query, 0)
        self.devMenu.append(self.devQuery)
        self.devScan = gtk.MenuItem("Rescan devices")
        self.devScan.connect("activate", self.scan)
        self.devMenu.append(self.devScan)
        
        self.menuBar.append(self.devM)
        
        self.setupMenu = gtk.Menu()
        self.setupM = gtk.MenuItem("Setup")
        self.setupM.set_submenu(self.setupMenu)
        if self.VERSION != 1:
            self.setupFull = gtk.MenuItem("Full Setup")
            self.setupFull.connect("activate", self.setup_prompt1)
            self.setupMenu.append(self.setupFull)
            self.setupPW = gtk.MenuItem("Password only")
            self.setupPW.connect("activate", self.setupPW_prompt)
            self.setupMenu.append(self.setupPW)
            self.updatePBA = gtk.MenuItem("Update Preboot Image")
            self.updatePBA.connect("activate", self.updatePBA_prompt)
            self.setupMenu.append(self.updatePBA)
        self.changePassword = gtk.MenuItem("Change Password")
        self.changePassword.connect("activate", self.changePW_prompt)
        self.setupMenu.append(self.changePassword)
        
        self.menuBar.append(self.setupM)
        
        if self.VERSION != 1:
            self.unlockMenu = gtk.Menu()
            self.unlockM = gtk.MenuItem("Unlock")
            self.unlockM.set_submenu(self.unlockMenu)
            self.unlock1 = gtk.MenuItem("Preboot unlock")
            self.unlock1.connect("activate", self.unlock_prompt)
            self.unlockMenu.append(self.unlock1)
            self.unlockFull = gtk.MenuItem("Full unlock")
            self.unlockFull.connect("activate", self.unlockFull_prompt)
            self.unlockMenu.append(self.unlockFull)
        
            self.menuBar.append(self.unlockM)
            
            self.lockMenu = gtk.Menu()
            self.lockM = gtk.MenuItem("Lock")
            self.lockM.set_submenu(self.lockMenu)
            self.lockMI = gtk.MenuItem("Lock a device")
            self.lockMI.connect("activate", self.lock_prompt)
            self.lockMenu.append(self.lockMI)
            
            self.menuBar.append(self.lockM)
        
        self.revertMenu = gtk.Menu()
        self.revertM = gtk.MenuItem("Revert")
        self.revertM.set_submenu(self.revertMenu)
        self.revertPW = gtk.MenuItem("with Password")
        self.revertPW.connect("activate", self.revert_user_prompt)
        self.revertMenu.append(self.revertPW)
        self.revertPSID = gtk.MenuItem("with PSID")
        self.revertPSID.connect("activate", self.revert_psid_prompt)
        self.revertMenu.append(self.revertPSID)
        
        self.menuBar.append(self.revertM)
        
        self.helpMenu = gtk.Menu()
        self.helpM = gtk.MenuItem("Help")
        self.helpM.set_submenu(self.helpMenu)
        self.help1 = gtk.MenuItem("Help")
        #self.help1.connect
        self.helpMenu.append(self.help1)
        self.updateM = gtk.MenuItem("Check for updates")
        #self.updateM.connect
        self.helpMenu.append(self.updateM)
        self.aboutM = gtk.MenuItem("About")
        #self.aboutM.connect
        self.helpMenu.append(self.aboutM)
        
        self.menuBar.append(self.helpM)
        
        if self.VERSION < 3 and self.VERSION != 1:
            self.upgradeMenu = gtk.Menu()
            self.upgradeM = gtk.MenuItem("Upgrade")
            self.upgradeM.set_submenu(self.upgradeMenu)
            if self.VERSION == 0:
                self.upgradeBasic = gtk.MenuItem("Upgrade to Basic")
                self.upgradeMenu.append(self.upgradeBasic)
            self.upgradePro = gtk.MenuItem("Upgrade to Pro")
            self.upgradeMenu.append(self.upgradePro)
            self.menuBar.append(self.upgradeM)
        
        self.hbox0.pack_start(self.menuBar, True, True, 0)
        self.vbox0.pack_start(self.hbox0, False, False, 0)
        
        self.buttonBox = gtk.HBox(homogeneous, 0)
        
        self.go_button_cancel = gtk.Button('_Cancel')
        self.go_button_revert_user_confirm = gtk.Button('_Revert with password')
        self.go_button_revert_psid_confirm = gtk.Button('_Revert with PSID')
        self.go_button_changePW_confirm = gtk.Button('_Change device password')
        self.setup_next = gtk.Button('_Activate and Continue')
        self.pbaUnlock = gtk.Button("Unlock with password")
        
        self.setupLockOnly = gtk.Button('_Skip writing PBA image')
        self.setupLockPBA = gtk.Button('_Write PBA image')
        
        self.setupPWOnly = gtk.Button('_Setup password')
        
        self.updatePBA_button = gtk.Button('_Update device\'s preboot image')
        self.pbaUSB_button = gtk.Button('_Unlock with USB')
        self.lock_button = gtk.Button('_Lock Device')
        
        self.unlockFull_button = gtk.Button('_Full Unlock with password')
        self.unlockFull_USB = gtk.Button('_Full Unlock with USB')
        
        self.viewLog = gtk.Button('_View Audit Log for selected device')
        
        self.main_label = gtk.Label('Main')
        self.main_label.set_alignment(0,0.5)
        self.vbox.pack_start(self.main_label, False, False, 0)
        self.fullSetup_label = gtk.Label('Full Setup')
        self.fullSetup_label.set_alignment(0,0.5)
        self.vbox.pack_start(self.fullSetup_label, False, False, 0)
        self.pwSetup_label = gtk.Label('Setup password only')
        self.pwSetup_label.set_alignment(0,0.5)
        self.vbox.pack_start(self.pwSetup_label, False, False, 0)
        self.pbaUpdate_label = gtk.Label('Update Preboot image')
        self.pbaUpdate_label.set_alignment(0,0.5)
        self.vbox.pack_start(self.pbaUpdate_label, False, False, 0)
        self.pwUpdate_label = gtk.Label('Change password')
        self.pwUpdate_label.set_alignment(0,0.5)
        self.vbox.pack_start(self.pwUpdate_label, False, False, 0)
        self.revertUser_label = gtk.Label('Revert with Password')
        self.revertUser_label.set_alignment(0,0.5)
        self.vbox.pack_start(self.revertUser_label, False, False, 0)
        self.revertPSID_label = gtk.Label('Revert with PSID')
        self.revertPSID_label.set_alignment(0,0.5)
        self.vbox.pack_start(self.revertPSID_label, False, False, 0)
        self.unlock_label = gtk.Label('Preboot Unlock')
        self.unlock_label.set_alignment(0,0.5)
        self.vbox.pack_start(self.unlock_label, False, False, 0)
        self.unlockFull_label = gtk.Label('Full Unlock')
        self.unlockFull_label.set_alignment(0,0.5)
        self.vbox.pack_start(self.unlockFull_label, False, False, 0)
        self.lock_label = gtk.Label('Lock')
        self.lock_label.set_alignment(0,0.5)
        self.vbox.pack_start(self.lock_label, False, False, 0)
        self.viewLog_label = gtk.Label('View Audit Log')
        self.viewLog_label.set_alignment(0,0.5)
        self.vbox.pack_start(self.viewLog_label, False, False, 0)
        
        self.noTCG_instr = gtk.Label('No TCG devices were detected, please insert a TCG drive and rescan to continue.')
        self.noTCG_instr.set_alignment(0,0.5)
        self.vbox.pack_start(self.noTCG_instr, False, False, 0)
        
        self.select_instr = gtk.Label('Select a device from the dropdown menu.')
        self.select_instr.set_alignment(0,0.5)
        self.vbox.pack_start(self.select_instr, False, False, 0)
        
        self.main_instr = gtk.Label('Select an operation from the menu bar above.  Below are all detected devices.')
        self.main_instr.set_alignment(0,0.5)
        self.vbox.pack_start(self.main_instr, False, False, 0)
        
        self.naDevices_instr = gtk.Label('\nNo drives available for this operation.')
        self.naDevices_instr.set_alignment(0,0.5)
        self.vbox.pack_start(self.naDevices_instr, False, False, 0)
        
        self.unlock_instr = gtk.Label('\n\nEnter the password for the selected device and press \'Preboot Unlock\'.')
        self.unlock_instr.set_alignment(0,0.5)
        
        self.unlockFull_instr = gtk.Label('\n\nEnter the password for the selected device and press \'Full Unlock\'.')
        self.unlockFull_instr.set_alignment(0,0.5)
        
        self.setup1_instr = gtk.Label('\n\nCreate the password for the selected device.\nYour password must be at least 8 characters long excluding whitespace.\nRemember your password for future drive authentication.\nIf you lose your password, the drive\'s data cannot be recovered.')
        self.setup1_instr.set_alignment(0,0.5)
        
        self.setup2_instr = gtk.Label('\n\nYou have the option to write a PBA image to the drive\'s shadow MBR.\nThe PBA image is used for pre-boot authentication.\nYou can skip this step by pressing the \'Skip writing PBA image\' button.')
        self.setup2_instr.set_alignment(0,0.5)
        
        self.updatePBA_instr = gtk.Label('\n\nPress \'Update device preboot image\' to update the selected device\'s PBA image')
        self.updatePBA_instr.set_alignment(0,0.5)
        
        self.revert_psid_instr = gtk.Label('\n\nEnter the drive\'s PSID below and press \'Revert with PSID\'.\nThis action will erase all data on the drive being reverted.')
        self.revert_psid_instr.set_alignment(0,0.5)
        
        self.revert_user_instr = gtk.Label('\n\nEnter the drive\'s password below.\nChoose whether or not to erase all data on the drive being reverted.')
        self.revert_user_instr.set_alignment(0,0.5)
        
        self.changePW_instr = gtk.Label('\n\nFill in the fields below and press \'Change device password\' to change the drive\'s password.')
        self.changePW_instr.set_alignment(0,0.5)
        
        self.lock_instr = gtk.Label('\n\nEnter in the drive\'s password and press \'Lock Device\' to lock the device.')
        self.lock_instr.set_alignment(0,0.5)
        
        self.viewLog_instr = gtk.Label('\n\nEnter in the device\'s password and press the \'View Audit Log\' button to see the device\'s log.')
        self.viewLog_instr.set_alignment(0,0.5)
        
        self.go_button_cancel.connect('clicked', self.returnToMain)
        self.go_button_revert_user_confirm.connect('clicked', self.revert_user)
        self.go_button_revert_psid_confirm.connect('clicked', self.revert_psid)
        self.go_button_changePW_confirm.connect('clicked', self.changePW)
        self.setup_next.connect('clicked', self.lock, 0)
        self.updatePBA_button.connect('clicked', self.pba_write)
        
        self.setupLockOnly.connect('clicked', self.setup_finish)
        self.setupLockPBA.connect('clicked', self.pba_write)
        
        self.setupPWOnly.connect('clicked', self.lock, 1)
        
        self.pbaUnlock.connect("clicked", self.pba)    
        
        self.lock_button.connect('clicked', self.lockset)
        
        self.unlockFull_button.connect('clicked', self.unlock)
        
        self.viewLog.connect('clicked', self.openLog)

        # Create devname buttons, entry with the appropriate settings
        
        self.scan_passwordonly()
        
        self.vbox.pack_start(self.setup2_instr, True, False, 5)
        self.vbox.pack_start(self.setup1_instr, True, False, 5)
        self.vbox.pack_start(self.updatePBA_instr, True, False, 5)
        self.vbox.pack_start(self.changePW_instr, True, False, 5)
        self.vbox.pack_start(self.revert_psid_instr, True, False, 5)
        self.vbox.pack_start(self.revert_user_instr, True, False, 5)
        self.vbox.pack_start(self.unlock_instr, True, False, 5)
        self.vbox.pack_start(self.unlockFull_instr, True, False, 5)
        self.vbox.pack_start(self.lock_instr, True, False, 5)
        self.vbox.pack_start(self.viewLog_instr, True, False, 5)
        
        #
        # Create PBA image buttons, entry with the appropriate settings
        #
        self.box_image = gtk.HBox(homogeneous, 4)
        self.button_image = gtk.Button("PBA image")
        self.button_image.connect('clicked', self.image_dialog)
        self.box_image.pack_start(self.button_image, expand, fill, padding)
        self.button_image.show()
        self.txt_image = gtk.Entry()      
        self.txt_image.set_text(self.image)
        self.box_image.pack_start(self.txt_image, True, True, padding)
        
        self.vbox.pack_start(self.box_image, False)
        
        self.box_psid = gtk.HBox(homogeneous, 0)
        
        self.revert_psid_label = gtk.Label("Enter PSID")
        self.revert_psid_label.set_width_chars(22)
        self.revert_psid_label.set_alignment(0,0.5)
        self.box_psid.pack_start(self.revert_psid_label, expand, fill, padding)
        
        self.revert_psid_entry = gtk.Entry()
        self.revert_psid_entry.set_text("")
        self.box_psid.pack_start(self.revert_psid_entry, True, True, padding)
        
        self.box_newpass_confirm = gtk.HBox(homogeneous, 0)
        
        self.confirm_pass_label = gtk.Label("Confirm new password")
        self.confirm_pass_label.set_width_chars(22)
        self.confirm_pass_label.set_alignment(0,0.5)
        self.box_newpass_confirm.pack_start(self.confirm_pass_label, expand, fill, padding)
        self.confirm_pass_entry = gtk.Entry()
        self.confirm_pass_entry.set_text("")
        self.confirm_pass_entry.set_width_chars(27)
        self.confirm_pass_entry.set_visibility(False)
        self.box_newpass_confirm.pack_start(self.confirm_pass_entry, False, False, padding)
        
        self.checkbox_box = gtk.HBox(homogeneous, 0)
        
        self.check_box_pass = gtk.CheckButton("Show Password")
        self.check_box_pass.connect("toggled", self.entry_check_box_pass, self.check_box_pass)
        self.check_box_pass.set_active(False)  # Set the default
        self.check_box_pass.show()
        self.checkbox_box.pack_start(self.check_box_pass, expand, fill, padding)
        
        self.eraseData_check = gtk.CheckButton("Erase the drive's data")
        self.eraseData_check.set_active(False)  # By default don't erase
        self.eraseData_check.show()
        self.checkbox_box.pack_start(self.eraseData_check, expand, fill, padding)
        
        check_align = gtk.Alignment(1,0,0,0)
        check_align.add(self.checkbox_box)
        
        self.buttonBox.pack_start(self.setup_next, False, False, padding)
        self.buttonBox.pack_start(self.setupLockOnly, False, False, padding)
        self.buttonBox.pack_start(self.setupLockPBA, False, False, padding)
        self.buttonBox.pack_start(self.go_button_changePW_confirm, False, False, padding)
        self.buttonBox.pack_start(self.go_button_revert_user_confirm, False, False, padding)
        self.buttonBox.pack_start(self.go_button_revert_psid_confirm, False, False, padding)
        self.buttonBox.pack_start(self.pbaUnlock, False, False, padding)
        self.buttonBox.pack_start(self.setupPWOnly, False, False, padding)
        self.buttonBox.pack_start(self.updatePBA_button, False, False, padding)
        self.buttonBox.pack_start(self.pbaUSB_button, False, False, padding)
        self.buttonBox.pack_start(self.lock_button, False, False, padding)
        self.buttonBox.pack_start(self.unlockFull_button, False, False, padding)
        self.buttonBox.pack_start(self.unlockFull_USB, False, False, padding)
        self.buttonBox.pack_start(self.viewLog, False, False, padding)
        
        self.buttonBox.pack_start(self.go_button_cancel, False, False, padding)
        
        self.pass_dialog()
        self.new_pass_dialog()
        
        self.vbox.pack_start(self.box_newpass_confirm, False)
        self.vbox.pack_start(self.box_psid, False)
        
        self.vbox.pack_start(check_align, False, False, padding)
        
        valign = gtk.Alignment(0,1,0,0)
        self.vbox.pack_start(valign)
        halign = gtk.Alignment(1,0,0,0)
        halign.add(self.buttonBox)
        self.vbox.pack_end(halign, False, False, padding)
        
        self.hbox.set_border_width(20)
        
        self.hbox.pack_start(self.vbox, True, True, padding)
        self.vbox0.pack_start(self.hbox, True, True, padding)
        
        self.add(self.vbox0)
        self.show_all()
        
        self.hideAll()
        self.select_box.show()
        self.main_instr.show()
        self.main_label.show()
        
        if self.VERSION == 0:            #Demo
            self.updateM.set_sensitive(False)
            self.readLog.set_sensitive(False)
            self.setupFull.set_sensitive(False)
            self.setupPW.set_sensitive(False)
            self.updatePBA.set_sensitive(False)
            self.changePassword.set_sensitive(False)
            self.revertPW.set_sensitive(False)
            self.revertPSID.set_sensitive(False)
            self.unlock1.set_sensitive(False)
            self.unlockUSB.set_sensitive(False)
            self.lockMI.set_sensitive(False) 
                
        self.scan()
        
        print self.devs_list
        print self.locked_list
        print self.setup_list
        print self.unlocked_list
        print self.nonsetup_list
        
    def scan_passwordonly(self, *args):
        homogeneous = False
        spacing = 0
        expand = False
        fill = False
        padding = 0
        width = 12

        # Create devname buttons, entry with the appropriate settings
        
        self.box_dev = gtk.HBox(homogeneous, spacing)
        
        self.dev_info = gtk.VBox(homogeneous, 5)
        self.opal_info = gtk.VBox(homogeneous, 5)
        
        self.select_box = gtk.HBox(homogeneous, spacing)
        
        self.vendor_box = gtk.HBox(homogeneous, spacing)
        self.sn_box = gtk.HBox(homogeneous, spacing)
        
        self.tcg_box = gtk.HBox(homogeneous, spacing)
        self.ssc_box = gtk.HBox(homogeneous, spacing)
        self.status_box = gtk.HBox(homogeneous, spacing)
        
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

        self.findos()

        if (gtk.gtk_version[1] > 24 or
            (gtk.gtk_version[1] == 24 and gtk.gtk_version[2] > 28)):
            self.dev_select = gtk.ComboBoxEntry()
            print 'gtk.ComboBoxEntry()'
        else:
            print 'gtk.combo_box_entry_new_text()'
            self.dev_select = gtk.combo_box_entry_new_text()
            
            self.dev_select.append = self.dev_select.append_text
            
        self.select_box.pack_start(self.dev_select, True, True, padding)
        
        self.dev_select.child.connect('changed', self.changed_cb)
        
        self.dev_single = gtk.Entry()
        self.dev_single.set_width_chars(35)
        self.dev_single.set_sensitive(False)

        self.select_box.pack_start(self.dev_single, True, True, padding)
        
        self.vbox.pack_start(self.select_box, False, True, padding)
        
        self.dev_label = gtk.Label(" Device information")
        self.dev_label.show()
        self.dev_info.pack_start(self.dev_label, False, False, padding)
        
        # entry to show real physical device
        
        self.vendor_label = gtk.Label("Model Number")
        self.vendor_label.set_alignment(0, 0.5)
        self.vendor_label.set_width_chars(14)
        self.vendor_label.show()
        self.vendor_box.pack_start(self.vendor_label, False, False, padding)
        
        self.dev_vendor = gtk.Entry()
        self.dev_vendor.set_text("")
        self.dev_vendor.set_property("editable", False)
        self.dev_vendor.set_sensitive(False)
        self.dev_vendor.show()
        self.dev_vendor.set_width_chars(38)
        self.vendor_box.pack_start(self.dev_vendor, False, False, padding)
        
        self.dev_info.pack_start(self.vendor_box, True, True, padding)
        
        self.sn_label = gtk.Label("Serial Number")
        self.sn_label.set_alignment(0, 0.5)
        self.sn_label.set_width_chars(14)
        self.sn_label.show()
        self.sn_box.pack_start(self.sn_label, False, False, padding)
        
        self.dev_sn = gtk.Entry()
        self.dev_sn.set_text("")
        self.dev_sn.set_property("editable", False)
        self.dev_sn.set_sensitive(False)
        self.dev_sn.show()
        self.dev_sn.set_width_chars(38)
        self.sn_box.pack_start(self.dev_sn, False, False, padding)
        
        self.dev_info.pack_start(self.sn_box, True, True, padding)
        
        self.msid_box = gtk.HBox(False, 0)
        
        self.msid_label = gtk.Label("MSID")
        self.msid_label.set_alignment(0,0.5)
        self.msid_label.set_width_chars(14)
        self.msid_label.show()
        self.msid_box.pack_start(self.msid_label, False, False, padding)
        
        self.dev_msid = gtk.Entry()
        self.dev_msid.set_text("")
        self.dev_msid.set_property("editable", False)
        self.dev_msid.set_sensitive(False)
        self.dev_msid.show()
        self.dev_msid.set_width_chars(38)
        self.msid_box.pack_start(self.dev_msid, False, False, padding)
        
        self.dev_info.pack_start(self.msid_box, True, True, padding)
       
        
        self.opal_label = gtk.Label(" TCG information")
        self.opal_label.show()
        self.opal_info.pack_start(self.opal_label, False, False, padding)
        
        # entry to show device Opal version
        
        self.label_opal_ver = gtk.Label('TCG Version')
        self.label_opal_ver.set_width_chars(12)
        
        self.label_opal_ver.show()
        self.ssc_box.pack_start(self.label_opal_ver, False, False, padding)
        
        self.dev_opal_ver = gtk.Entry()
        self.dev_opal_ver.set_text("")
        self.dev_opal_ver.set_width_chars(18)
        self.dev_opal_ver.set_property("editable", False)
        self.dev_opal_ver.set_sensitive(False)
        self.dev_opal_ver.show()
        self.ssc_box.pack_start(self.dev_opal_ver, False, False, padding)
        
        self.opal_info.pack_start(self.ssc_box, False, True, padding)
        
        self.status_label = gtk.Label(" Lock Status ")
        self.status_label.set_width_chars(12)
        self.status_label.show()
        self.status_box.pack_start(self.status_label, False, False, padding)
        
        self.dev_status = gtk.Entry()
        self.dev_status.set_text("")
        self.dev_status.set_property("editable", False)
        self.dev_status.set_sensitive(False)
        self.dev_status.set_width_chars(18)
        self.dev_status.show()
        self.status_box.pack_start(self.dev_status, False, False, padding)
        
        self.opal_info.pack_start(self.status_box, False, False, padding)
        
        self.setup_box = gtk.HBox(False,0)
        
        self.setup_label = gtk.Label(" Setup Status ")
        self.setup_label.set_width_chars(12)
        self.setup_label.show()
        self.setup_box.pack_start(self.setup_label, False, False, padding)
        
        self.dev_setup = gtk.Entry()
        self.dev_setup.set_text("")
        self.dev_setup.set_property("editable", False)
        self.dev_setup.set_sensitive(False)
        self.dev_setup.set_width_chars(18)
        self.dev_setup.show()
        self.setup_box.pack_start(self.dev_setup, False, False, padding)
        
        self.opal_info.pack_start(self.setup_box, False, False, padding)
        
        self.box_dev.pack_start(self.dev_info, True, True, padding)
        self.box_dev.pack_start(self.opal_info, False, True, padding)
        
        self.vbox.pack_start(self.box_dev, False)


    def entry_check_box_pass(self, widget, checkbox):
        b_entry_checkbox = checkbox.get_active()
        print ('pass b_entry_checkbox = ', b_entry_checkbox)
        if b_entry_checkbox:
            print "Box checked"
            pass_show = True
        else:
            print "Not checked"
            pass_show = False
        self.pass_entry.set_visibility(pass_show)
        self.new_pass_entry.set_visibility(pass_show)
        self.confirm_pass_entry.set_visibility(pass_show)
        return 

    def hash_pass(self, pass_txt, *args):
        pw_trim = re.sub('\s', '', pass_txt)
        if self.force_true == True:
            self.pass_hash = True
        else:
            self.pass_hash = False
        if self.pass_hash == True:
            print ("Use hashed password")
            hash_object = hashlib.sha256(pw_trim)
            hex_dig = hash_object.hexdigest()
            print('hashed password = ', hex_dig)
            return hex_dig
        else:
            print ("Do not hash password")
            return pw_trim
 
    def changed_LKRNG(self, *args):
        print "changed Locking Range"

    def query(self, mode, *args):
        index = -1
        if self.view_state == 0:
            index = self.dev_select.get_active()
        elif self.view_state == 1:
            index = self.locked_list[self.dev_select.get_active()]
        elif self.view_state == 2:
            index = self.setup_list[self.dev_select.get_active()]
        elif self.view_state == 3:
            index = self.unlocked_list[self.dev_select.get_active()]
        else:
            index = self.nonsetup_list[self.dev_select.get_active()]
        self.devname = self.devs_list[index]
        self.dev_vendor.set_text(self.vendor_list[index])
        self.dev_sn.set_text(self.sn_list[index])
        self.dev_msid.set_text(self.get_msid())
        self.dev_opal_ver.set_text(self.opal_ver_list[index])
        print ("self.devname: ", self.devname)
        txt2 = ""
        txt = os.popen(self.prefix + "sedutil-cli --query " + self.devname ).read()
        
        if mode == 1:
            txt11 = "Locked ="
            m = re.search(txt11, txt)
            if m:
                print m.group()

            txt_L = "Locked = Y"
            txt_UL = "Locked = N"
            txt_S = "MBREnabled = Y"
            
            isLocked = re.search(txt_L, txt)
            isUnlocked = re.search(txt_UL, txt)
            isSetup = re.search(txt_S, txt)
            
            if isLocked:
                self.dev_status.set_text("Locked")
                self.dev_setup.set_text("Yes")
            elif isUnlocked:
                self.dev_status.set_text("Unlocked")
                if isSetup:
                    self.dev_setup.set_text("Yes")
                else:
                    self.dev_setup.set_text("No")
            else:
                self.dev_status.set_text("N/A")
                self.dev_setup.set_text("N/A")
        
        else:
            self.queryWinText = "Fidelity Lock Query information for device " + self.devname + "\n"
            
            txtVersion = os.popen(self.prefix + "sedutil-cli --version" ).read()
            self.queryWinText = self.queryWinText + txtVersion + "\n\nDevice information\n"
            
            self.queryWinText = self.queryWinText + "Model: " + self.dev_vendor.get_text() + "\n"
            self.queryWinText = self.queryWinText + "Serial Number: " + self.dev_sn.get_text() + "\n"
            self.queryWinText = self.queryWinText + "TCG SSC: " + self.dev_opal_ver.get_text() + "\n"
            self.queryWinText = self.queryWinText + "MSID: " + self.get_msid() + "\n"
            
            txtState = os.popen(self.prefix + "sedutil-cli --getmfgstate " + self.devname).read()
            self.queryWinText = self.queryWinText + txtState + "\nLocking information\n"
            
            self.queryWinText = self.queryWinText + "Lock Status: " + self.dev_status.get_text() + "\n"
            
            t = [ "Locked = [YN], LockingEnabled = [YN], LockingSupported = [YN], MBRDone = [YN], MBREnabled = [YN]",
                "Locking Objects = [0-9]*",
                "Max Tables = [0-9]*, Max Size Tables = [0-9]*",
                "Locking Admins = [0-9]*.*, Locking Users = [0-9]*.",
                "Policy = [NY].*",
                "Base comID = 0x[0-9A-F]*, Initial PIN = 0x[0-9A-F]*"]

            for txt11 in t:
                m = re.search(txt11, txt) # 1 or 2 follow by anything ; look for Opal drive
                if m:
                    txt1 = m.group()
                    txt11 = txt1.replace("Locking ", "")            
                    txt1 = txt11
                    txt11 = txt1.replace(", ", "\n")
                    txt2 = txt2 + txt11 + "\n"
            txt2 = self.devname + " " + self.dev_vendor.get_text() + "\n" + txt2
            print txt2
        
            tt = [ "Locked = [YN]", 
                    "LockingEnabled = [YN]",
                    "LockingSupported = [YN]",
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
            sts_LockingSupported = ""
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
                    print x_words[0] # keyword
                    if x_words[0] == "Locked":
                        sts_Locked = x_words[1]
                    elif x_words[0] == "LockingEnabled":
                        sts_LockingEnabled = x_words[1]                   
                    elif x_words[0] == "LockingSupported":
                        sts_LockingSupported = x_words[1]
                    elif x_words[0] == "MBRDone":
                        sts_MBRDone = x_words[1]
                    elif x_words[0] == "MBREnabled":
                        sts_MBREnabled = x_words[1]
                    elif x_words[0] == "Max Size Tables":
                        tblsz_i = int(x_words[1],10) # 10 base      
                        tblsz = str(tblsz_i/1000000) + "MB"
                        print ("x_words[1] = ", x_words[1])
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
            self.queryWinText = self.queryWinText + "Locked: " + sts_Locked + "\n"
            self.queryWinText = self.queryWinText + "Locking Enabled: " + sts_LockingEnabled + "\n"
            self.queryWinText = self.queryWinText + "Locking Supported: " + sts_LockingSupported + "\n"
            self.queryWinText = self.queryWinText + "Shadow MBR Enabled: " + sts_MBREnabled + "\n"
            self.queryWinText = self.queryWinText + "Shadow MBR Done: " + sts_MBRDone + "\n\nSingle User information\n"
            self.queryWinText = self.queryWinText + "Single User Mode Support: " + singleUser + "\n"
            self.queryWinText = self.queryWinText + "Number of Locking Ranges Supported: " + nbr_Objects + "\n\nDataStore information\n"
            self.queryWinText = self.queryWinText + "DataStore Table Size: " + tblsz + "\n"
            self.queryWinText = self.queryWinText + "Number of DataStore Tables: " + nbr_MaxTables + "\n\nOpal information\n"
            self.queryWinText = self.queryWinText + "Number of Admins: " + nbr_Admins + "\n"
            self.queryWinText = self.queryWinText + "Number of Users: " + nbr_Users + "\n"
            self.queryWinText = self.queryWinText + "Base comID: " + comID_base + "\n"
            self.queryWinText = self.queryWinText + "Initial PIN: " + initialPIN + "\n"
        
        
            if not self.scanning :
                queryWin = gtk.Window()
                queryWin.set_title("Query Device")
                
                winWidth = 400
                winHeight = 500
                
                scrolledWin = gtk.ScrolledWindow()
                scrolledWin.set_policy(gtk.POLICY_NEVER, gtk.POLICY_AUTOMATIC)
                
                queryWin.set_size_request(winWidth, winHeight)
                if os.path.isfile('icon.jpg'):
                    queryWin.set_icon_from_file('icon.jpg')
                
                queryText = gtk.Label(txt2)
                queryVbox = gtk.VBox(False, 0)
                queryVbox.set_border_width(10)
                
                queryTextView = gtk.TextView()
                queryTextView.set_editable(False)
                self.queryTextBuffer = queryTextView.get_buffer()
                self.queryTextBuffer.set_text(self.queryWinText)
                scrolledWin.add_with_viewport(queryTextView)
                queryVbox.pack_start(scrolledWin, True, True, 0)
                
                query_instr = gtk.Label('Enter the device\'s password to access more query information.')
                queryVbox.pack_start(query_instr, False, False, 0)
                
                self.passBoxQ = gtk.HBox(False, 0)
                passLabel = gtk.Label('Enter Password')
                self.queryPass = gtk.Entry()
                self.queryPass.set_visibility(False)
                
                submitPass = gtk.Button('Submit')
                submitPass.connect("clicked", self.queryAuth)
                self.passBoxQ.pack_start(passLabel, True, True, 0)
                self.passBoxQ.pack_start(self.queryPass, True, True, 0)
                self.passBoxQ.pack_start(submitPass, False, False, 0)
                queryVbox.pack_start(self.passBoxQ, False, False, 0)
                
                save_instr = gtk.Label('Press \'Save to text file\' to save the query information in a file.')
                queryVbox.pack_start(save_instr, False, False, 0)
                
                querySave = gtk.Button('_Save to text file')
                querySave.connect("clicked", self.saveToText)
                queryVbox.pack_start(querySave, False, False, 0)
                queryWin.add(queryVbox)
                queryWin.show_all()
                
            else:
                self.scanning = False
                
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
            print "Selected filepath: %s" % chooser.get_filename()
            filename = chooser.get_filename()
            if not filename.endswith('.txt'):
                filename += '.txt'
            f = open(filename, 'w')
            f.write(self.queryWinText)
            f.close()
        chooser.destroy()
        
    def queryAuth(self, *args):
        devpass = self.queryPass.get_text()
        txt1 = "NOT_AUTHORIZED"
        txt2 = "AUTHORITY_LOCKED_OUT"
        p = subprocess.check_output([self.prefix + "sedutil-cli", "--getmbrsize", devpass, self.devname])
        na = re.search(txt1, p)
        alo = re.search(txt2, p)
        if na :
            self.msg_err("Error: Invalid password, try again.")
        elif alo :
            self.msg_err("Error: Locked out due to multiple failed attempts.  Please reboot and try again.")
        else :
            self.queryWinText = self.queryWinText + "\n" + p
            self.queryTextBuffer.set_text(self.queryWinText)
            self.passBoxQ.hide()
            
	
    def scan(self, *args):
        if self.firstscan == False:
            print '*************** enter rescan ********************'
            model = self.dev_select.get_model() # model is the list of entry
            print ('model:',model)
        
            iter = gtk.TreeIter
            for row in model:
                model.remove(row.iter)

            if len(self.vendor_list) > 0:
                self.vendor_list = []
                print 'after remove all item, vendor_list = ', self.vendor_list
                self.opal_ver_list = []
                print 'after remove all item, opal_ver_list = ', self.opal_ver_list
                
        self.finddev()
        
        length = 0
        if self.view_state == 0:
            length = len(self.devs_list)
            for idx in range(length) :
                self.dev_select.append( self.devs_list[idx])
        elif self.view_state == 1:
            length = len(self.locked_list)
            for idx in range(length) :
                self.dev_select.append( self.devs_list[self.locked_list[idx]])
        elif self.view_state == 2:
            length = len(self.setup_list)
            for idx in range(length) :
                self.dev_select.append( self.devs_list[self.setup_list[idx]])
        elif self.view_state == 3:
            length = len(self.unlocked_list)
            for idx in range(length) :
                self.dev_select.append( self.devs_list[self.unlocked_list[idx]])
        else:
            length = len(self.nonsetup_list)
            for idx in range(length) :
                self.dev_select.append( self.devs_list[self.nonsetup_list[idx]])
        
        numTCG = len(self.setup_list) + len(self.nonsetup_list)
        
        if numTCG == 0:
            self.noTCG_instr.show()
        else:
            self.noTCG_instr.hide()
        if length >= 1:
            self.dev_single.set_text(self.devs_list[0])
            if self.view_state == 0:
                self.scanning = True
                self.query(1)
                self.scanning = False
                self.dev_select.set_active(0)
            
        self.firstscan = False
        
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
        

    def findos(self, *args):
        if platform == "linux" or platform == "linux2":
            print platform
            self.ostype = 1
            self.prefix = "sudo "
            self.cmd = 'python /usr/local/bin/Lock.py'
            print ("self.cmd = ", self.cmd)
        elif platform == "darwin":
            print platform
            self.ostype = 2
            self.prefix = "sudo "
            self.cmd = 'python /usr/local/bin/Lock.py'
        elif platform == "win32":
            print platform
            self.ostype = 0
            self.prefix = ""
            self.cmd = 'python Lock.py'
        

    def finddev(self, *args):
        txt = os.popen(self.prefix + 'sedutil-cli --scan n').read()
        print "scan result : "
        print txt
		
     
        names = ['PhysicalDrive[0-9]', '/dev/sd[a-z]', '/dev/nvme[0-9]',  '/dev/disk[0-9]']
        idx = 0
        self.devs_list = []
        self.locked_list = []
        self.setup_list = []
        self.unlocked_list = []
        self.nonsetup_list = []
        for index in range(len(names)): #index=0(window) 1(Linux) 2(OSX)
            print ("index= ", index)
            print names[index]
        
            m = re.search(names[index] + ".*", txt) # 1st search identify OS type and the pattern we are looking for
            
            if m:
                if (index == 0 ):
                   self.prefix = ""
                else:
                   self.prefix = "sudo "
                
                txt11 = names[index] + " .[A-z0-9]+ *[A-z0-9].*"
                print("looking for Opal device ", txt11)

                m = re.findall(txt11, txt) # 1 or 2 follow by anything ; look for Opal drive
                print ("m = ", m)
                if m:
                    for tt in m:
                        print tt
                        m2 = re.match(names[index],tt)
                        x_words = tt.split() 
                        if (index == 0) : 
                            self.devname = "\\\\.\\" + m2.group()
                        else:
                            self.devname =  m2.group()
                        self.devs_list.append(self.devname)
                        if len(x_words) > 3 : # concatenate all word together
                            print "x_words len > 3"
                            for y in range(3,(len)(x_words)):###for y in range(3,(len)(x_words):
                                x_words[2] = x_words[2] + " " + x_words[y]
                        y_words = x_words[2].split(":",1)
                        print ("y_words = ", y_words)
                        print ("y_words[0] = ", y_words[0])
                        print ("y_words[1] = ", y_words[1])
                        self.vendor_list.append(y_words[0])
                        y = y_words[1].replace(" ", "")
                        y_words[1] = y 
                        self.series_list.append(y_words[1]) 
                        
                        if x_words[1] == "No":
                            self.tcg_list.append(x_words[1])
                            self.opal_ver_list.append("None")
                        else:
                            self.tcg_list.append("Yes")
                            if x_words[1] == "1" or x_words[1] == "2":
                                self.opal_ver_list.append("Opal " + x_words[1] + ".0")
                            elif x_words[1] == "12":
                                self.opal_ver_list.append("Opal 1.0/2.0")
                            elif x_words[1] == "E":
                                self.opal_ver_list.append("Enterprise")
                            elif x_words[1] == "L":
                                self.opal_ver_list.append("Opallite")
                            elif x_words[1] == "P":
                                self.opal_ver_list.append("Pyrrite")
                            else:
                                self.opal_ver_list.append(x_words[1])
            else:
                print "No Matched : ", names[index]
        if self.devs_list != []:
            for i in range(len(self.devs_list)):
                queryText = os.popen(self.prefix + 'sedutil-cli --query ' + self.devs_list[i]).read()
                regex = ':\s*[A-z0-9]+\s+([A-z0-9]+)'
                p = re.compile(regex)
                m = p.search(queryText)
                if m:
                    self.sn_list.append(m.group(1))
                else:
                    self.sn_list.append("N/A")
                
                txt_L = "Locked = Y"
                txt_S = "MBREnabled = Y"
                txt_NS = "MBREnabled = N"
                isLocked = re.search(txt_L, queryText)
                isSetup = re.search(txt_S, queryText)
                isTCG = re.search(txt_NS, queryText)
                if isLocked:
                    self.locked_list.append(i)
                    self.setup_list.append(i)
                elif isSetup:
                    self.setup_list.append(i)
                    self.unlocked_list.append(i)
                elif isTCG:
                    self.nonsetup_list.append(i)
            print ("devs_list: ",  self.devs_list)
            print ("vendor_list: ", self.vendor_list)
            print ("opal_ver_list: ", self.opal_ver_list)
            print ("sn_list: ", self.sn_list)	

    def lock(self, mode, *args):
        index = self.dev_select.get_active()
        self.devname = self.devs_list[self.nonsetup_list[index]]
        print("lock physical device "+ self.devname)
        pw = self.new_pass_entry.get_text()
        pw_confirm = self.confirm_pass_entry.get_text()
        pw_trim = re.sub('\s', '', pw)
        pw_trim_confirm = re.sub(r'\s+', '', pw_confirm)
        print pw_trim
        if len(pw_trim) < 8:# and not (digit_error or uppercase_error or lowercase_error):
            self.msg_err("This password is too short.  Please enter a password at least 8 characters long excluding whitespace.")
        elif self.bad_pw.has_key(pw_trim):
            self.msg_err("This password is on the blacklist of bad passwords, please enter a stronger password.")
        elif pw_trim != pw_trim_confirm:
            self.msg_err("The entered passwords do not match.")
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
                
                    password = self.hash_pass(self.new_pass_entry.get_text())
                    
                    queryText = os.popen(self.prefix + 'sedutil-cli --query ' + self.devs_list[self.dev_select.get_active()]).read()
                    txt_LE = "LockingEnabled = N"
                    txt_ME = "MBREnabled = N"
                    unlocked = re.search(txt_LE, queryText)
                    activated = re.search(txt_ME, queryText)
                    status1 = -1
                    if unlocked:
                        status1 =  os.system(self.prefix + "sedutil-cli --initialSetup " + password + " " + self.devname )
                        
                        timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                        timeStr = timeStr[2:]
                        statusAW3 = os.system(self.prefix + "sedutil-cli --auditwrite 01" + timeStr + " " + password + " " + self.devname)
                        
                        timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                        timeStr = timeStr[2:]
                        statusAW = os.system(self.prefix + "sedutil-cli --auditwrite 10" + timeStr + " " + password + " " + self.devname)
                        timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                        timeStr = timeStr[2:]
                        statusAW = os.system(self.prefix + "sedutil-cli --auditwrite 11" + timeStr + " " + password + " " + self.devname)
                    elif activated:
                        dev_msid = self.get_msid()
                        s1 = os.system(self.prefix + "sedutil-cli --setSIDPassword " + dev_msid + " " + password + " " + self.devname)
                        s2 = os.system(self.prefix + "sedutil-cli --setAdmin1Pwd " + dev_msid + " " + password + " " + self.devname)
                        timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                        timeStr = timeStr[2:]
                        statusAW = os.system(self.prefix + "sedutil-cli --auditwrite 10" + timeStr + " " + password + " " + self.devname)
                        timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                        timeStr = timeStr[2:]
                        statusAW = os.system(self.prefix + "sedutil-cli --auditwrite 11" + timeStr + " " + password + " " + self.devname)
                        status1 = (s1 | s2)
                    
                    status2 =  os.system(self.prefix + "sedutil-cli --enableLockingRange " + self.LKRNG + " " + password + " " + self.devname )
                    status3 =  os.system(self.prefix + "sedutil-cli --setMBRdone on " + password + " " + self.devname )
                    status4 =  os.system(self.prefix + "sedutil-cli --setMBREnable on " + password + " " + self.devname )
                    if (s2 | status2 | status3 | status4) !=0 :
                        self.msg_err("Error: Setup of " + self.devname + " failed. Try again.")
                    else : 
                        if mode == 1:
                            status5 =  os.system(self.prefix + "sedutil-cli --enableLockingRange " + self.LKRNG + " " + password + " " + self.devname )
                            status6 =  os.system(self.prefix + "sedutil-cli --setLockingrange " + self.LKRNG + " LK " + password + " " + self.devname )
                            status7 =  os.system(self.prefix + "sedutil-cli --setMBRDone on " + password + " " + self.devname )
                            status8 =  os.system(self.prefix + "sedutil-cli --setMBREnable on " + password + " " + self.devname )
                        self.query(1)
                        self.msg_ok("Password for " + self.devname + " set up successfully.")
                        if mode == 0:
                            self.setup_prompt2()
                        else:
                            self.updateDevs(index,4,1)
                            self.returnToMain()
            
    def changePW(self, *args):
        #retrieve old, new, and confirm new password texts
        old_pass = self.pass_entry.get_text()
        new_pass = self.new_pass_entry.get_text()
        new_pass_confirm = self.confirm_pass_entry.get_text()
        old_trim = re.sub('\s', '', old_pass)
        pw_trim = re.sub('\s', '', new_pass)
        pw_trim_confirm = re.sub(r'\s+', '', new_pass_confirm)
        print pw_trim
        if len(pw_trim) < 8:# and not (digit_error or uppercase_error or lowercase_error):
            self.msg_err("The new password is too short.  Please enter a password at least 8 characters long excluding whitespace.")
        elif self.bad_pw.has_key(pw_trim):
            self.msg_err("The new password is on the blacklist of bad passwords, please enter a stronger password.")
        elif pw_trim != pw_trim_confirm:
            self.msg_err("The new entered passwords do not match.")
        #check to make sure new and confirm new are the same
        #run the setSIDPassword and setAdmin1Pwd commands
        else:
            index = self.dev_select.get_active()
            self.devname = self.devs_list[self.setup_list[index]]
            old_hash = self.hash_pass(self.pass_entry.get_text())
            new_hash = self.hash_pass(self.new_pass_entry.get_text())
            txt1 = "NOT_AUTHORIZED"
            txt2 = "AUTHORITY_LOCKED_OUT"
            p1 = subprocess.check_output([self.prefix + "sedutil-cli", "--setSIDPassword", old_hash, new_hash, self.devname])
            na1 = re.search(txt1, p1)
            alo1 = re.search(txt2, p1)
            p2 = subprocess.check_output([self.prefix + "sedutil-cli", "--setAdmin1Pwd", old_hash, new_hash, self.devname])
            na2 = re.search(txt1, p2)
            alo2 = re.search(txt2, p2)
            if alo1 | alo2:
                self.msg_err("Error: You have been locked out of " + self.devname + " due to multiple failed authentication attempts.  Please reboot and try again.")
            elif na1 | na2:
                self.msg_err("Error: Incorrect password, please try again.")
            else :
                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                timeStr = timeStr[2:]
                statusAW = os.system(self.prefix + "sedutil-cli --auditwrite 10" + timeStr + " " + new_hash + " " + self.devname)
                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                timeStr = timeStr[2:]
                statusAW = os.system(self.prefix + "sedutil-cli --auditwrite 11" + timeStr + " " + new_hash + " " + self.devname)
                self.msg_ok("Password for " + self.devname + " changed successfully.") 
                self.returnToMain()
                
    def revert_user(self, *args):
        if self.eraseData_check.get_active() == True:
            message = gtk.MessageDialog(type=gtk.MESSAGE_WARNING, buttons=gtk.BUTTONS_OK_CANCEL)
            message.set_markup("Warning : Revert with password erase all data. Do you want to proceed?")
            cancelPressed = False
            res = message.run()
            print message.get_widget_for_response(gtk.RESPONSE_OK)
            print gtk.RESPONSE_OK
            if res == gtk.RESPONSE_OK :
                print "User click OK button"
                messageA = gtk.MessageDialog(type=gtk.MESSAGE_WARNING, buttons=gtk.BUTTONS_OK_CANCEL)
                messageA.set_markup("Warning Warning Warning : Revert with password erase all data. Do you really want to proceed?")
                resA = messageA.run()

                if resA == gtk.RESPONSE_OK : 
                    print "execute the revert with password"
                    password = self.hash_pass(self.pass_entry.get_text())
                    index = self.dev_select.get_active()
                    self.devname = self.devs_list[self.setup_list[index]]
                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                    timeStr = timeStr[2:]
                    statusAW = os.system(self.prefix + "sedutil-cli --auditwrite 15" + timeStr + " " + password + " " + self.devname)
                    txt1 = "NOT_AUTHORIZED"
                    txt2 = "AUTHORITY_LOCKED_OUT"
                    p1 = subprocess.check_output([self.prefix + "sedutil-cli", "--revertTPer", password, self.devname])
                    na = re.search(txt1, p1)
                    alo = re.search(txt2, p1)
                else :
                    cancelPressed = True
                messageA.destroy()
            else :
                cancelPressed = True
            message.destroy()
            if not cancelPressed and na :
                self.msg_err("Error: Invalid password, try again.")
            elif not cancelPressed and alo :
                self.msg_err("Error: Locked out due to multiple failed attempts.  Please reboot and try again.")
            elif not cancelPressed :
                dev_msid = self.get_msid()
                
                status = os.system(self.prefix + "sedutil-cli --activate " + dev_msid + " " + self.devname)
                
                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                timeStr = timeStr[2:]
                statusAW1 = os.system(self.prefix + "sedutil-cli --auditwrite 05" + timeStr + " " + dev_msid + " " + self.devname)
                
                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                timeStr = timeStr[2:]
                statusAW2 = os.system(self.prefix + "sedutil-cli --auditwrite 08" + timeStr + " " + dev_msid + " " + self.devname)
                
                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                timeStr = timeStr[2:]
                statusAW3 = os.system(self.prefix + "sedutil-cli --auditwrite 01" + timeStr + " " + dev_msid + " " + self.devname)
                self.msg_ok("Device " + self.devname + " successfully reverted with password.")
                index = self.dev_select.get_active()
                self.query(1)
                if index in self.locked_list:
                    self.updateDevs(index,1,2)
                self.returnToMain()
        else:
            password = self.hash_pass(self.pass_entry.get_text())
            index = self.dev_select.get_active()
            self.devname = self.devs_list[self.setup_list[index]]
            
            txtLE = "LockingEnabled = N"
            
            txt1 = "NOT_AUTHORIZED"
            txt2 = "AUTHORITY_LOCKED_OUT"
            
            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
            timeStr = timeStr[2:]
            statusAW = os.system(self.prefix + "sedutil-cli --auditwrite 13" + timeStr + " " + password + " " + self.devname)
            
            p = subprocess.check_output([self.prefix + "sedutil-cli", "--revertnoerase", password, self.devname])
            na = re.search(txt1, p)
            alo = re.search(txt2, p)
            if na :
                self.msg_err("Error: Invalid password, try again.")
                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                timeStr = timeStr[2:]
                statusAW = os.system(self.prefix + "sedutil-cli --auditwrite 14" + timeStr + " " + dev_msid + " " + self.devname)
            elif alo :
                self.msg_err("Error: Locked out due to multiple failed attempts.  Please reboot and try again.")
                timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                timeStr = timeStr[2:]
                statusAW = os.system(self.prefix + "sedutil-cli --auditwrite 14" + timeStr + " " + dev_msid + " " + self.devname)
            else :
                #query to verify that LockingEnabled = N, proceed if affirm
                p0 = subprocess.check_output([self.prefix + "sedutil-cli", "--query", self.devname])
                le_check = re.search(txtLE, p0)
                if le_check:
                    dev_msid = self.get_msid()
                    p1 = subprocess.check_output([self.prefix + "sedutil-cli", "--revertTPer", password, self.devname])
                    
                    status = os.system(self.prefix + "sedutil-cli --activate " + dev_msid + " " + self.devname)
                    
                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                    timeStr = timeStr[2:]
                    statusAW1 = os.system(self.prefix + "sedutil-cli --auditwrite 04" + timeStr + " " + dev_msid + " " + self.devname)
                    
                    timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
                    timeStr = timeStr[2:]
                    statusAW2 = os.system(self.prefix + "sedutil-cli --auditwrite 01" + timeStr + " " + dev_msid + " " + self.devname)
                    self.msg_ok("Device " + self.devname + " successfully reverted with password.")
                    index = self.dev_select.get_active()
                    self.query(1)
                    if index in self.locked_list:
                        self.updateDevs(index,1,4)
                    else:
                        self.updateDevs(index,3,4)
                    self.returnToMain()
                else:
                    self.msg_err("Error: Revert failed.")
    
    def revert_psid(self, *args):
        message = gtk.MessageDialog(type=gtk.MESSAGE_WARNING, buttons=gtk.BUTTONS_OK_CANCEL)
        message.set_markup("Warning : Revert with PSID erase all data. Do you want to proceed?")
        cancelPressed = False
        res = message.run()
        print message.get_widget_for_response(gtk.RESPONSE_OK)
        print gtk.RESPONSE_OK
        if res == gtk.RESPONSE_OK :
            print "User click OK button"
            messageA = gtk.MessageDialog(type=gtk.MESSAGE_WARNING, buttons=gtk.BUTTONS_OK_CANCEL)
            messageA.set_markup("Warning Warning Warning : Revert with PSID erase all data. Do you really want to proceed?")
            resA = messageA.run()

            if resA == gtk.RESPONSE_OK :
                messageA.destroy()
                message.destroy()
                print "execute the revert with PSID"
                psid = self.revert_psid_entry.get_text()
                index = self.dev_select.get_active()
                self.devname = self.devs_list[self.setup_list[index]]
                
                status =  os.system(self.prefix + "sedutil-cli --yesIreallywanttoERASEALLmydatausingthePSID " + psid + " " + self.devname )
            else :
                cancelPressed = True
        else :
            cancelPressed = True
        if not cancelPressed and status != 0 :
            self.msg_err("Error: Incorrect PSID, please try again." )
        elif not cancelPressed :
            dev_msid = self.get_msid()
            status = os.system(self.prefix + "sedutil-cli --activate " + dev_msid + " " + self.devname)
            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
            timeStr = timeStr[2:]
            statusAW1 = os.system(self.prefix + "sedutil-cli --auditwrite 06" + timeStr + " " + dev_msid + " " + self.devname)
            
            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
            timeStr = timeStr[2:]
            statusAW = os.system(self.prefix + "sedutil-cli --auditwrite 08" + timeStr + " " + dev_msid + " " + self.devname)
            
            timeStr = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
            timeStr = timeStr[2:]
            statusAW2 = os.system(self.prefix + "sedutil-cli --auditwrite 01" + timeStr + " " + dev_msid + " " + self.devname)
            self.msg_ok("Device " + self.devname + " successfully reverted with PSID.")
            index = self.dev_select.get_active()
            self.query(1)
            if index in self.locked_list:
                self.updateDevs(index,1,4)
            else:
                self.updateDevs(index,3,4)
            self.returnToMain()
    
    def pba(self, *args):  ## Authorize preboot to allow next boot into Locked OS partition
        index = self.dev_select.get_active()
        self.devname = self.devs_list[self.locked_list[index]]
        print("PreBoot Authorization physical device " + self.devname)
        self.LKATTR = "RW"
        password = self.hash_pass(self.pass_entry.get_text())
        status1 =  os.system(self.prefix + "sedutil-cli --setMBRdone on " + password + " " + self.devname )
        status2 =  os.system(self.prefix + "sedutil-cli --setLockingRange " + self.LKRNG + " " 
                + self.LKATTR + " " + password + " " + self.devname)
        
        if (status1 | status2) != 0 :
            self.msg_err("Error: Preboot unlock of " + self.devname + " failed.")
        else :
            self.msg_ok(self.devname + " preboot unlocked successfully.")
            self.query(1)
            self.updateDevs(index,1,3)
            self.returnToMain()
            
    def lockset(self, *args):
        index = self.dev_select.get_active()
        self.devname = self.devs_list[self.unlocked_list[index]]
        print("Set Lock Attribute of physical device " + self.devname)
        password = self.hash_pass(self.pass_entry.get_text())
        status1 =  os.system(self.prefix + "sedutil-cli --enableLockingRange " + self.LKRNG + " " 
                + password + " " + self.devname )
        status2 =  os.system(self.prefix + "sedutil-cli --setLockingrange " + self.LKRNG + " LK "
                + password + " " + self.devname )
        status3 =  os.system(self.prefix + "sedutil-cli --setMBRDone on " + password + " " + self.devname )
        status4 =  os.system(self.prefix + "sedutil-cli --setMBREnable on " + password + " " + self.devname )
        if (status1 | status2 | status3 | status4 ) != 0 :
            self.msg_err("TCG Lock setting unsuccess")
        else :
            self.msg_ok("TCG Lock setting success") 
            self.query(1)
            self.updateDevs(index,3,1)
            self.returnToMain()
            
    def unlock(self, *args):
        index = self.dev_select.get_active()
        self.devname = self.devs_list[self.setup_list[index]]
        print("unlock physical device " + self.devname)
        password = self.hash_pass(self.pass_entry.get_text())
        status1 =  os.system(self.prefix + "sedutil-cli --disableLockingRange " + self.LKRNG + " " 
                + password + " " + self.devname )
        status2 =  os.system(self.prefix + "sedutil-cli --setMBREnable off " + password + " " + self.devname )
        dev_msid = self.get_msid()
        status3 = os.system(self.prefix + "sedutil-cli --setSIDPassword " + password + " " + dev_msid + " " + self.devname)
        status4 = os.system(self.prefix + "sedutil-cli --setAdmin1Pwd " + password + " " + dev_msid + " " + self.devname)
        if (status1 | status2 | status3 | status4) != 0 :
            self.msg_err("TCG Unlock unsuccess")
        else :
            self.msg_ok("TCG Unlock success")
            self.query(1)
            if index in self.locked_list:
                self.updateDevs(index,1,4)
            else:
                self.updateDevs(index,3,4)
            self.returnToMain()

    def exitapp(self, *args):
        print ("User click exit button")
        exit(0)

    def reboot(self, *args):
        print("Exit and reboot")
        if self.ostype == 0 :
            status =  os.system("shutdown -r -t 0")
        elif self.ostype == 1 :
            status =  os.system(self.prefix + "reboot now")
        elif self.ostype == 2 :
            status =  os.system(self.prefix + "reboot now")    
        exit(0)
            
    def image_dialog(self, *args):
        self.fcd = gtk.FileChooserDialog("Open...",
             None,
             gtk.FILE_CHOOSER_ACTION_OPEN,
             (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL, gtk.STOCK_OPEN, gtk.RESPONSE_OK))
        self.response = self.fcd.run()
        if self.response == gtk.RESPONSE_OK:
            print "Selected filepath: %s" % self.fcd.get_filename()
            self.image = self.fcd.get_filename()
            self.txt_image.set_text(self.image)
            self.fcd.destroy()
            return self.image
        


    def pba_write(self, *args):
        print "Write PBA image to shadow MBR"
        status = -1
        password = self.hash_pass(self.pass_entry.get_text())
        index = self.dev_select.get_active()
        self.devname = self.devs_list[self.setup_list[index]]
        self.image = self.txt_image.get_text()
        print "sedutil-cli --loadpbaimage " + password + " " + self.image + " " + self.devname
        start = time.clock()
        status = os.system( self.prefix + "sedutil-cli --loadpbaimage " + password + " " + self.image + " " + self.devname )
        stop = time.clock()
        elapsed = (stop - start)
        print ("elapsed = ", elapsed)
        
        if status != 0 :# and not cancelled :
            self.msg_err("Error: Writing PBA image to " + self.devname + " failed.")
        else :
            self.msg_ok("PBA image " + self.image + " written to " + self.devname + " successfully.")
            self.setup_finish()            

    def changed_cb(self, entry):
        print 'Select device has changed', entry.get_text()
        act_idx = self.dev_select.get_active() # get active index
        print 'active index = ', act_idx
        self.dev_vendor.set_text(self.vendor_list[act_idx])
        self.dev_single.set_text(self.devs_list[act_idx])
        print 'dev_opal_ver = ',self.opal_ver_list
        if self.tcg_list[act_idx] != "No":
            self.dev_sn.set_text(self.sn_list[act_idx])
            
            self.scanning = True
            self.query(1)
            self.scanning = False
        else:
            self.dev_opal_ver.set_text("None")
            self.dev_status.set_text("N/A")
            self.dev_sn.set_text("N/A")
            self.dev_msid.set_text("N/A")
            self.dev_setup.set_text("N/A")
            
    def get_msid(self, *args):  
        txt = os.popen(self.prefix + "sedutil-cli --printDefaultPassword " + self.devname ).read()
        if txt == '' :
            return
        x_words = txt.split(': ',1)
        msid = re.sub(r'\n', '', x_words[1])
        return msid
    
    def pass_dialog(self, *args):
        # Create a new hbox with the appropriate homogeneous and spacing settings
        homogeneous = False
        spacing = 0
        expand = False
        fill = False
        padding = 0
        # Create password buttons, entry with the appropriate settings
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
        
        # allow user to retrive password option
 
        self.pass_rd = gtk.Button("Read from USB")
        #self.pass_rd.connect("clicked", self.pass_rdusb)
        self.pass_rd.show()
        self.box_pass.pack_start(self.pass_rd, True, fill, padding)
     
        self.vbox.pack_start(self.box_pass, False)
        
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
        
        # allow user to save password option
 
        self.pass_sav = gtk.Button("Save to USB")
        #self.pass_sav.connect("clicked", self.pass_save2usb)
        self.pass_sav.show()
        self.box_newpass.pack_start(self.pass_sav, True, fill, padding)
        
        self.vbox.pack_start(self.box_newpass, False)
        
    def openLog_prompt(self, *args):
        self.hideAll()
        self.select_box.show()
        self.viewLog_label.show()
        self.go_button_cancel.show()
        
        self.view_state = 0
        
        if len(self.devs_list) > 1:
            self.select_instr.show()
            self.dev_select.set_active(0)
        if len(self.devs_list) == 0:
            self.naDev()
        else:
            self.viewLog_instr.show()
            
            self.box_pass.show()
            self.viewLog.show()
            
            self.check_box_pass.show()
            
            self.query(1)
        
    def openLog(self, *args):
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
        
        index = self.dev_select.get_active()
        self.devname = self.devs_list[index]
        password = self.hash_pass(self.pass_entry.get_text())
        
        txt = os.popen(self.prefix + "sedutil-cli --auditread " + password + " " + self.devname ).read()
        print txt
        
        if txt == "" or txt == "Invalid Audit Signature or No Audit Entry log\n":
            self.msg_err("Invalid Audit Signature or No Audit Entry Log or Read Error")
        else:
            outputLines = txt.split('\n')
            numEntriesLine = outputLines[1]
            entriesRegex = "Total Number of Audit Entries: ([0-9]+)"
            p = re.compile(entriesRegex)
            m0 = p.match(numEntriesLine)
            numEntries = int(m0.group(1))
            logList = outputLines[2:]
            auditRegex = "([0-9]+/[0-9]+/[0-9]+\s+[0-9]+:[0-9]+:[0-9]+)\s+([0-9]+)"
            pattern = re.compile(auditRegex)
            
            for i in range(numEntries):
                m = pattern.match(logList[i])
                dateTime = m.group(1)
                eventID = int(m.group(2))
                eventDes = self.eventDescriptions[eventID]
                eventLevel = "Information"
                if eventID == 9 or eventID == 14 or eventID == 16 or eventID == 18:
                    eventLevel = "Error"
                    self.errorEntries.append((eventLevel, dateTime, eventID, eventDes))
                    self.warnerrEntries.append((eventLevel, dateTime, eventID, eventDes))
                elif eventID == 13 or eventID == 15 or eventID == 17:
                    eventLevel = "Warning"
                    self.warnerrEntries.append((eventLevel, dateTime, eventID, eventDes))
                self.auditEntries.append((eventLevel, dateTime, eventID, eventDes))
            for i in range(len(self.auditEntries)):
                #print auditEntries[i]
                self.listStore.append(self.auditEntries[i])
        
        treeView = gtk.TreeView(model=self.listStore)
        
        for i in range(len(columns)):
            # cellrenderer to render the text
            cell = gtk.CellRendererText()
            # the column is created
            col = gtk.TreeViewColumn(columns[i], cell, text=i)
            if i < 3:
                col.set_sort_column_id(gtk.SORT_DESCENDING)
                col.set_sort_indicator(True)
            # and it is appended to the treeview
            treeView.append_column(col)
        
        vbox.pack_start(treeView)
        
        halign = gtk.Alignment(1,0,0,0)
        filter_box = gtk.HBox(False, 0)
        
        self.viewAll_button = gtk.Button('View all entries')
        self.viewAll_button.connect("clicked", self.filterLog, self.auditEntries, 0)
        filter_box.pack_start(self.viewAll_button, False, False, 5)
        
        self.viewWarnErr_button = gtk.Button('View Warnings & Errors')
        self.viewWarnErr_button.connect("clicked", self.filterLog, self.warnerrEntries, 1)
        filter_box.pack_start(self.viewWarnErr_button, False, False, 5)
        
        self.viewErr_button = gtk.Button('View Errors')
        self.viewErr_button.connect("clicked", self.filterLog, self.errorEntries, 2)
        filter_box.pack_start(self.viewErr_button, False, False, 5)
        
        self.eraseLog_button = gtk.Button('Erase the device\'s entire log')
        self.eraseLog_button.connect("clicked", self.eraseLog)
        filter_box.pack_start(self.eraseLog_button, False, False, 5)
        
        halign.add(filter_box)
        vbox.pack_start(halign, False, False, 0)
        
        logWin.show_all()
        
    def filterLog(self, button, entries, mode):
        self.listStore.clear()
        for i in range(len(entries)):
            self.listStore.append(entries[i])
        self.viewAll_button.set_sensitive(mode != 0)
        self.viewWarnErr_button.set_sensitive(mode != 1)
        self.viewErr_button.set_sensitive(mode != 2)
        
    def eraseLog(self, *args):
        #self.devname = self.devs_list[index]
        password = self.hash_pass(self.pass_entry.get_text())
        status = os.system( self.prefix + "sedutil-cli --auditerase " + password + " " + self.devname )
        if status != 0:
            self.msg_err('Error while attempting to erase the audit log')
        else:
            self.listStore.clear()
            self.auditEntries = []
            self.errorEntries = []
            self.warnerrEntries = []
            self.msg_ok('Audit Log erased successfully')
    
    def msg_err(self, msg):
        message = gtk.MessageDialog(type=gtk.MESSAGE_ERROR, buttons=gtk.BUTTONS_OK)
        message.set_markup(msg)
        res = message.run()
        print message.get_widget_for_response(gtk.RESPONSE_OK)
        print gtk.RESPONSE_OK
        if res == gtk.RESPONSE_OK :
            print "OK button clicked"
        message.destroy()
        
    def msg_ok(self, msg):
        message = gtk.MessageDialog(type=gtk.MESSAGE_INFO, buttons=gtk.BUTTONS_OK)
        message.set_markup(msg)
        res = message.run()
        print message.get_widget_for_response(gtk.RESPONSE_OK)
        print gtk.RESPONSE_OK
        if res == gtk.RESPONSE_OK :
            print "OK button clicked"
        message.destroy()
        
    def setup_finish(self, *args):
        #popup message to signal setup complete
        self.msg_ok("Device " + self.devname + "has been setup successfully.") 
        self.query(1)
        self.returnToMain()

    def returnToMain(self, *args):
        self.hideAll()
        self.main_label.show()
        self.main_instr.show()
        
        if self.view_state != 0:
            self.view_state = 0
            self.changeList()
        
        if len(self.devs_list) > 1:
            self.dev_select.set_active(0)
            
        self.query(1)
            
        if self.VERSION == 1:
            if len(self.devs_list) == 1 and self.dev_status.get_text() == "Locked":
                self.unlock_instr.show()

                self.box_pass.show()
                self.pbaUnlock.show()
            
    def setup_prompt1(self, *args):
        self.hideAll()
        self.go_button_cancel.show()
        self.fullSetup_label.show()
        
        if self.view_state != 4:
            self.view_state = 4
            self.changeList()
        
        if len(self.nonsetup_list) > 1:
            self.select_instr.show()
            self.dev_select.set_active(0)
            
        if len(self.nonsetup_list) > 0:
            self.setup1_instr.show()
            
            self.box_newpass.show()
            self.box_newpass_confirm.show()
            self.setup_next.show()
            
            self.check_box_pass.show()
            
            self.query(1)
        else:
            self.naDevices_instr.show()
        
    def setup_prompt2(self, *args):
        self.hideAll()
        self.setupLockOnly.show()
        self.setupLockPBA.show()
        self.setup2_instr.show()
    
        self.go_button_cancel.hide()
        self.setup2_instr.show()
        self.box_image.show()
        
    def setupPW_prompt(self, *args):
        self.hideAll()
        self.go_button_cancel.show()
        self.pwSetup_label.show()
        
        if self.view_state != 4:
            self.view_state = 4
            self.changeList()
        
        if len(self.nonsetup_list) > 1:
            self.select_instr.show()
            self.dev_select.set_active(0)
            
        if len(self.nonsetup_list) > 0:
            self.setup1_instr.show()
            
            self.box_newpass.show()
            self.box_newpass_confirm.show()
            self.setupPWOnly.show()
            
            self.check_box_pass.show()
            
            self.query(1)
        else:
            self.naDevices_instr.show()
        
    def updatePBA_prompt(self, *args):
        self.hideAll()
        self.go_button_cancel.show()
        self.pbaUpdate_label.show()
        
        if self.view_state != 2:
            self.view_state = 2
            self.changeList()
        
        if len(self.setup_list) > 1:
            self.select_instr.show()
            self.dev_select.set_active(0)
            
        if len(self.setup_list) > 0:
            self.updatePBA_instr.show()
            self.updatePBA_button.show()
            
            self.box_pass.show()
            self.box_image.show()
            self.check_box_pass.show()
            
            self.query(1)
        else:
            self.naDevices_instr.show()

    def changePW_prompt(self, *args):
        self.hideAll()
        self.go_button_cancel.show()
        self.pwUpdate_label.show()
        
        if self.view_state != 2:
            self.view_state = 2
            self.changeList()
        
        if len(self.setup_list) > 1:
            self.select_instr.show()
            self.dev_select.set_active(0)
            
        if len(self.setup_list) > 0:
            self.changePW_instr.show()
            
            self.box_pass.show()
            self.box_newpass.show()
            self.box_newpass_confirm.show()
            self.go_button_changePW_confirm.show()
            
            self.check_box_pass.show()
            
            self.query(1)
        else:
            self.naDevices_instr.show()
        
    def revert_user_prompt(self, *args):
        self.hideAll()
        self.revertUser_label.show()
        self.go_button_cancel.show()
        
        if self.view_state != 2:
            self.view_state = 2
            self.changeList()
        
        if len(self.setup_list) > 1:
            self.select_instr.show()
            self.dev_select.set_active(0)
            
        if len(self.setup_list) > 0:
            self.revert_user_instr.show()
            self.box_pass.show()
            self.go_button_revert_user_confirm.show()
            
            self.check_box_pass.show()
            self.eraseData_check.show()
            self.query(1)  
        else:
            self.naDevices_instr.show()
        
    def revert_psid_prompt(self, *args):
        self.hideAll()
        self.go_button_cancel.show()
        self.revertPSID_label.show()
        
        if self.view_state != 2:
            self.view_state = 2
            self.changeList()
            
        if len(self.setup_list) > 1:
            self.select_instr.show()
            self.dev_select.set_active(0)
            
        if len(self.setup_list) > 0:
            self.revert_psid_instr.show()
            
            self.box_psid.show()
            self.go_button_revert_psid_confirm.show()
            self.revert_psid_entry.set_text("")
            
            self.query(1)
        else:
            self.naDevices_instr.show()
        
    def unlock_prompt(self, *args):
        self.hideAll()
        self.unlock_label.show()
        self.go_button_cancel.show()
        
        if self.view_state != 2:
            self.view_state = 2
            self.changeList()
            
        if len(self.setup_list) > 1:
            self.select_instr.show()
            self.dev_select.set_active(0)
            
        if len(self.setup_list) > 0:
            self.unlock_instr.show()

            self.box_pass.show()
            self.pbaUnlock.show()
            self.pbaUSB_button.show()
            
            self.check_box_pass.show()
            
            self.query(1)
            
        else:
            self.naDevices_instr.show()
            
    def unlockFull_prompt(self, *args):
        self.hideAll()
        self.unlockFull_label.show()
        self.go_button_cancel.show()
        
        if self.view_state != 2:
            self.view_state = 2
            self.changeList()
            
        if len(self.setup_list) > 1:
            self.select_instr.show()
            self.dev_select.set_active(0)
            
        if len(self.setup_list) > 0:
            self.unlockFull_instr.show()

            self.box_pass.show()
            self.unlockFull_button.show()
            self.unlockFull_USB.show()
            
            self.check_box_pass.show()
            
            self.query(1)
        else:
            self.naDevices_instr.show()
            
    def lock_prompt(self, *args):
        self.hideAll()
        self.lock_label.show()
        self.go_button_cancel.show()
        
        if self.view_state != 3:
            self.view_state = 3
            self.changeList()
            
        if len(self.unlocked_list) > 1:
            self.select_instr.show()
            self.dev_select.set_active(0)
            
        if len(self.unlocked_list) > 0:
            self.lock_instr.show()
            self.lock_button.show()
            self.box_pass.show()
            self.check_box_pass.show()
            
            self.query(1)
            
        else:
            self.naDevices_instr.show()

    def hideAll(self, *args):
        self.main_label.hide()
        self.fullSetup_label.hide()
        self.pwSetup_label.hide()
        self.pbaUpdate_label.hide()
        self.pwUpdate_label.hide()
        self.revertUser_label.hide()
        self.revertPSID_label.hide()
        self.unlock_label.hide()
        self.unlockFull_label.hide()
        self.lock_label.hide()
        self.viewLog_label.hide()
    
        self.select_instr.hide()
        self.naDevices_instr.hide()
        self.main_instr.hide()
        self.setup1_instr.hide()
        self.setup2_instr.hide()
        self.updatePBA_instr.hide()
        self.changePW_instr.hide()
        self.revert_user_instr.hide()
        self.revert_psid_instr.hide()
        self.unlock_instr.hide()
        self.unlockFull_instr.hide()
        self.viewLog_instr.hide()
    
        #hide the various prompt components
        self.pass_entry.set_text("")
        self.new_pass_entry.set_text("")
        self.confirm_pass_entry.set_text("")
        self.eraseData_check.set_active(False)
        self.check_box_pass.set_active(False)
        self.box_pass.hide()
        self.box_psid.hide()
        self.go_button_cancel.hide()
        self.go_button_revert_user_confirm.hide()
        self.go_button_revert_psid_confirm.hide()
        self.go_button_changePW_confirm.hide()
        self.setupLockOnly.hide()
        self.setupLockPBA.hide()        
        self.pbaUnlock.hide()
        self.setupPWOnly.hide()
        self.updatePBA_button.hide()
        self.pbaUSB_button.hide()
        self.lock_button.hide()
        self.unlockFull_button.hide()
        self.unlockFull_USB.hide()
        self.viewLog.hide()
        
        self.box_newpass.hide()
        self.box_newpass_confirm.hide()
        self.setup_next.hide()
        self.setup1_instr.hide()
        self.setup2_instr.hide()
        self.revert_user_instr.hide()
        self.revert_psid_instr.hide()
        self.changePW_instr.hide()
        self.lock_instr.hide()
        
        self.check_box_pass.hide()
        self.eraseData_check.hide()
        
        self.box_image.hide()
        
    def updateDevs(self, index, old, new, *args):
        if old == 1:
            self.locked_list.remove(index)
            if new == 3:
                self.unlocked_list.append(index)
            elif new == 4:
                self.setup_list.remove(index)
                self.nonsetup_list.append(index)
        elif old == 3:
            self.unlocked_list.remove(index)
            if new == 1:
                self.locked_list.append(index)
            elif new == 4:
                self.setup_list.remove(index)
                self.nonsetup_list.append(index)
        elif old == 4:
            self.nonsetup_list.remove(index)
            self.setup_list.append(index)
            if new == 1:
                self.locked_list.append(index)
            elif new == 3:
                self.unlocked_list.append(index)
                
        self.locked_list.sort()
        self.setup_list.sort()
        self.unlocked_list.sort()
        self.nonsetup_list.sort()
        
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
            if length == 1:
                self.dev_single.set_text(self.devs_list[0])
        elif self.view_state == 1:
            length = len(self.locked_list)
            for i in range(length):
                self.dev_select.append(self.devs_list[self.locked_list[i]])
            if length == 1:
                self.dev_single.set_text(self.devs_list[self.locked_list[0]])
        elif self.view_state == 2:
            length = len(self.setup_list)
            for i in range(length):
                self.dev_select.append(self.devs_list[self.setup_list[i]])
            if length == 1:
                self.dev_single.set_text(self.devs_list[self.setup_list[0]])
        elif self.view_state == 3:
            length = len(self.unlocked_list)
            for i in range(length):
                self.dev_select.append(self.devs_list[self.unlocked_list[i]])
            if length == 1:
                self.dev_single.set_text(self.devs_list[self.unlocked_list[0]])
        else:
            length = len(self.nonsetup_list)
            for i in range(length):
                self.dev_select.append(self.devs_list[self.nonsetup_list[i]])
            if length == 1:
                self.dev_single.set_text(self.devs_list[self.nonsetup_list[0]])
        
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
        else:
            self.dev_single.hide()
            self.label_dev2.hide()
            self.dev_select.show()
            self.label_dev.show()
            
    def run(self):
        ''' Run the app. '''
        
        gtk.main()

app = LockApp()
app.run()