import gtk
import os
import re
import sys
import background
import getopt
import gobject
import threading
import string
import platform
if platform.system() == 'Windows':
    import subprocess
import runop
import runscan
import dialogs
import ctypes
import multiprocessing
import verify
from ctypes import c_long, c_int



GWL_WNDPROC = -4
WM_DESTROY  = 2
DBT_DEVTYP_DEVICEINTERFACE = 0x00000005  # device interface class
DBT_DEVICEREMOVECOMPLETE = 0x8004  # device is gone
DBT_DEVICEARRIVAL = 0x8000  # system detected a new device
DBT_DEVNODES_CHANGED = 0x0007
WM_DEVICECHANGE = 0x0219

PBT_APMSUSPEND = 0x4
PBT_APMRESUMEAUTOMATIC = 0x12
WM_POWERBROADCAST = 0x0218

WM_QUERYENDSESSION = 0x11

if platform.system() == 'Windows':
    SetWindowLong = ctypes.windll.user32.SetWindowLongW
    CallWindowProc = ctypes.windll.user32.CallWindowProcW

    WndProcType = ctypes.WINFUNCTYPE(ctypes.c_long, ctypes.c_int, ctypes.c_uint, ctypes.c_int, ctypes.c_int)

class WndProcHookMixin:
    def __init__(self):
        self.__msgDict = {}
        ## We need to maintain a reference to the WndProcType wrapper
        ## because ctypes doesn't
        self.__localWndProcWrapped = None 
        self.rtnHandles = []

    def hookWndProc(self):
        self.__localWndProcWrapped = WndProcType(self.localWndProc)
        self.__oldWndProc = SetWindowLong(self.window.handle,
                                        GWL_WNDPROC,
                                        self.__localWndProcWrapped)
    def unhookWndProc(self):
        SetWindowLong(self.window.handle,
                        GWL_WNDPROC,
                        self.__oldWndProc)

        ## Allow the ctypes wrapper to be garbage collected
        self.__localWndProcWrapped = None

    def addMsgHandler(self,messageNumber,handler):
        self.__msgDict[messageNumber] = handler

    def localWndProc(self, hWnd, msg, wParam, lParam):
        if msg in self.__msgDict:
            if self.__msgDict[msg](wParam,lParam) == False:
                return

        if msg == WM_DESTROY: self.unhookWndProc()

        return CallWindowProc(self.__oldWndProc,
                                hWnd, msg, wParam, lParam)

if __name__ == "__main__":
    multiprocessing.freeze_support()
    gobject.threads_init()
        
    class LockApp(gtk.Window,WndProcHookMixin):
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
                       
        DEV_OS = platform.system()

        LKRNG = "0" 
        LKRNG_SLBA = "0"
        LKRNG_LBALEN = "0"
        LKATTR = "RW"

        VERSION = -1
        PBA_VERSION = -1
        MAX_DEV = -1
        
        
        
        shutdown_req = False
        reboot_req = False
        
        op_inprogress = False
        
        #__gsignals__ = {
        #    "delete-event" : "override"
        #}
         
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
                MAX_DEV = sys.maxint
            elif o in ("--standard"):
                VERSION = 2
                MAX_DEV = 5
            elif o in ("--premium"):
                VERSION = 3
                MAX_DEV = 5
        
        full_devs_list = []
        full_devs_map = []
        full_sn_list = []
        full_salt_list = []
        full_user_list = []
        
        full_isSetup_list = []
        full_isLocked_list = []
        
        full_ver_list = []
        
        full_dsSup_list = []
       
        devs_list = [] #[ "/dev/sda" , "/dev/sdb",  "/dev/sdc",  "/dev/sdd",  "/dev/sde" ]
        locked_list = []
        setup_list = []
        nonsetup_list = []
        tcg_list = []
        sel_list = []
        mbr_list = []
        mbr_setup_list = []
        usetup_list = []
        ulocked_list = []
        
        msid_list = []
        pba_list = []
        
        retrylimit_list = []
        
        mv_list = []
        usb_mv_list = []
        
        warned = False
        orig = ''
        
        mode_setupUSB = False
        
        usb_boot = False
        
        dnc_ip = False
        drc_ip = False
        da_ip = False
        dnc_mount = False
        
        user_list = []
        admin_list = []
        
        setupuser_list = []
        datastore_list = []
        
        label_list = []
        partition_list = []
        
        process_list = []
        
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
        
        admin_aol_list = []
        user_aol_list = []
        psid_aol_list = []
        
        tcg_usb_list = []
        
        pbawrite_ip = False
        scan_ip = False
        
        arrival_hold = False
        
        check_exclusive = False
        check_both = False
        
        #pass_usb = ''
        pass_dir = ''
        
        scanning = False
        view_state = 0
        if VERSION == 1:
            view_state = 1
        op_prompt = 0
        
        invalid_pba = False
        pba_devname = None
        
        posthibern = False
        
        NOT_AUTHORIZED = 1
        AUTHORITY_LOCKED_OUT = 18
        SP_BUSY = 3
        INVALID_PARAMETER = 12
        if DEV_OS == 'Linux':
            NOT_AUTHORIZED = 256
            AUTHORITY_LOCKED_OUT = 4608
            SP_BUSY = 768
            INVALID_PARAMETER = 3072
        
        def __init__(self):
            if sys.platform == "linux" or sys.platform == "linux2":
                self.ostype = 1
                self.prefix = "sudo "
            elif sys.platform == "darwin":
                self.ostype = 2
                self.prefix = "sudo "
            elif sys.platform == "win32":
                self.ostype = 0
                self.prefix = ""
            
            f = verify.initCheck(self)
            
            if f or self.VERSION != -1:
                if self.DEV_OS == 'Windows':
                    WndProcHookMixin.__init__(self)
                gtk.Window.__init__(self)
                if self.VERSION == 0:
                    self.set_title('Opal Lock - Demo')
                elif self.VERSION == 1:
                    self.set_title('Opal Lock - PBA')
                elif self.VERSION == 2:
                    self.set_title('Opal Lock - Standard')
                elif self.MAX_DEV == 5:
                    self.set_title('Opal Lock - Premium5')
                elif self.MAX_DEV == 25:
                    self.set_title('Opal Lock - Premium25')
                elif self.MAX_DEV == 100:
                    self.set_title('Opal Lock - Premium100')
                else:
                    self.set_title('Opal Lock - PremiumUnlimited')
                    
                if os.path.isfile('icon.ico'):
                    self.set_icon_from_file('icon.ico')

                height = 540
                width = 630
                if self.DEV_OS == 'Linux' and self.VERSION == 1:
                    height = 600
                    width = 700
                    
                self.set_size_request(width, height)
                
                self.set_resizable(False)

                self.connect('destroy', background.exitX, self)
                
                if self.DEV_OS == 'Windows':
                    theme = os.path.join(os.getcwd(), 'gtkrc')
                    gtk.rc_set_default_files([theme])
                    gtk.rc_reparse_all_for_settings(gtk.settings_get_default(), True)
                    gtk.rc_reset_styles(gtk.settings_get_for_screen(self.get_screen()))
                
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
                self.backToMain.connect("activate", self.returnToMain, False)
                self.backToMain.set_tooltip_text('Return to home view')
                self.navMenu.append(self.backToMain)
                self.exitApp = gtk.MenuItem("Exit")
                self.exitApp.connect("activate", background.exitFL, self)
                self.exitApp.set_tooltip_text('Exit the app')
                self.navMenu.append(self.exitApp)
                self.exitReboot = gtk.MenuItem("Restart")
                self.exitReboot.connect("activate", background.reboot, self)
                self.exitReboot.set_tooltip_text('Restarts the computer')
                self.navMenu.append(self.exitReboot)
                if self.DEV_OS == 'Windows':
                    self.exitHibernate = gtk.MenuItem('Hibernate')
                    self.exitHibernate.connect('activate', background.hibernate, self)
                    self.exitHibernate.set_tooltip_text('Hibernates the computer')
                    self.navMenu.append(self.exitHibernate)
                self.exitShutDown = gtk.MenuItem("Shut Down")
                self.exitShutDown.connect("activate", background.shutdown, self)
                self.exitShutDown.set_tooltip_text('Shuts down the computer')
                self.navMenu.append(self.exitShutDown)
                
                
                self.menuBar.append(self.navM)
                
                self.devMenu = gtk.Menu()
                self.devM = gtk.MenuItem("Drive")
                self.devM.set_submenu(self.devMenu)
                self.devQuery = gtk.MenuItem("Query drive")
                self.devQuery.connect("activate", dialogs.query, self, 0)
                self.devQuery.set_tooltip_text('Query the currently selected drive')
                self.devMenu.append(self.devQuery)
                self.readLog = gtk.MenuItem("View Audit Log")
                self.readLog.connect("activate", self.openLog_prompt)
                self.readLog.set_tooltip_text('Access a drive\'s event log')
                self.devMenu.append(self.readLog)
                self.devScan = gtk.MenuItem("Rescan drives")
                self.devScan.connect("activate", runscan.run_scan, self, True)
                self.devScan.set_tooltip_text('Update the list of detected drives')
                self.devMenu.append(self.devScan)
                self.devOpal = gtk.MenuItem("View Opal drives")
                self.devOpal.connect("activate", dialogs.openOpal, self)
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
                    
                    self.setupUSB = gtk.MenuItem("Setup Bootable USB")
                    self.setupUSB.connect("activate", self.setupUSB_prompt)
                    self.setupUSB.set_tooltip_text('Set up bootable USB')
                    self.setupMenu.append(self.setupUSB)
                    
                    if self.VERSION % 3 == 0:
                        self.setupUser = gtk.MenuItem("Setup User")
                        self.setupUser.connect("activate", self.setupUser_prompt)
                        self.setupUser.set_tooltip_text('Set up a user password')
                        self.setupMenu.append(self.setupUser)
                        
                        self.removeUser = gtk.MenuItem("Remove User")
                        self.removeUser.connect("activate", self.removeUser_prompt)
                        self.removeUser.set_tooltip_text('Disable the user')
                        self.setupMenu.append(self.removeUser)
                    
                    self.mngPower = gtk.MenuItem("Power Settings")
                    self.mngPower.connect("activate", dialogs.mngPower_prompt, self, 1)
                    self.mngPower.set_tooltip_text('Manage system power settings')
                    self.setupMenu.append(self.mngPower)
                    
                    self.menuBar.append(self.setupM)
                
                
                self.unlockMenu = gtk.Menu()
                self.unlockM = gtk.MenuItem("Unlock")
                self.unlockM.set_submenu(self.unlockMenu)
                self.unlock1 = gtk.MenuItem("Unlock Drive(s)")
                self.unlock1.connect("activate", self.unlock_prompt)
                self.unlock1.set_tooltip_text('Unlock a drive to boot into')
                self.unlockMenu.append(self.unlock1)
            
                self.menuBar.append(self.unlockM)
                
                self.revertMenu = gtk.Menu()
                self.revertM = gtk.MenuItem("Remove Lock")
                self.revertM.set_submenu(self.revertMenu)
                self.revertO = gtk.MenuItem("and Keep Data")
                self.revertO.connect("activate", self.revert_keep_prompt)
                self.revertO.set_tooltip_text('Use password to revert the drive\'s LockingSP while keeping its data')
                self.revertMenu.append(self.revertO)
                self.revertPW = gtk.MenuItem("and Erase Data")
                self.revertPW.connect("activate", self.revert_erase_prompt)
                self.revertPW.set_tooltip_text('Use password to revert the drive\'s LockingSP and erase data')
                self.revertMenu.append(self.revertPW)
                self.revertPSID = gtk.MenuItem("and Erase Data with PSID")
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
                self.aboutM.connect("activate", dialogs.show_about, self)
                self.aboutM.set_tooltip_text('About Opal Lock')
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
                
                self.setup_next = gtk.Button('_Set Up Drive(s)')
                

                
                self.setupUserPW = gtk.Button('_Set Up User Password')
                self.removeUser_button = gtk.Button('_Remove User')
                
                self.updatePBA_button = gtk.Button('_Update')
                self.updatePBA_button.set_flags(gtk.CAN_DEFAULT)
                
                self.changePW_button = gtk.Button('_Change Password')
                
                self.setupUSB_button = gtk.Button('_Create bootable USB')
                
                self.pbaUnlockReboot = gtk.Button("_Unlock and Restart")
                self.pbaUnlockReboot.set_flags(gtk.CAN_DEFAULT)
                self.pbaUnlockOnly = gtk.Button("_Unlock only")
                self.pbaUSB_button = gtk.Button('_Unlock with USB')
                
                self.revertOnly_button = gtk.Button('_Remove Lock')
                self.revertOnly_button.set_flags(gtk.CAN_DEFAULT)
                
                self.revertUser_button = gtk.Button('_Remove Lock and Erase')
                self.revertUser_button.set_flags(gtk.CAN_DEFAULT)
                self.revertPSID_button = gtk.Button('_Remove Lock and Erase with PSID')
                
                self.op_label = gtk.Label('Main')
                self.op_label.set_alignment(0,0.5)
                self.vbox.pack_start(self.op_label, False, False, 0)
                
                top_box = gtk.HBox(homogeneous, 0)
                
                self.noTCG_instr = gtk.Label('No TCG drives were detected, please insert a TCG drive and use \'Rescan drives\' to continue.')
                self.noTCG_instr.set_alignment(0,0.5)
                top_box.pack_start(self.noTCG_instr, False, False, 0)
                
                self.select_instr = gtk.Label('Select a drive from the dropdown menu.')
                self.select_instr.set_alignment(0,0.5)
                top_box.pack_start(self.select_instr, False, False, 0)
                
                self.selectMulti_instr = gtk.Label('Select one or more drives from the list.')
                self.selectMulti_instr.set_alignment(0,0.5)
                top_box.pack_start(self.selectMulti_instr, False, False, 0)
                
                self.main_instr = gtk.Label('Select an operation from the menu bar above.')
                self.main_instr.set_alignment(0,0.5)
                top_box.pack_start(self.main_instr, False, False, 0)
                
                self.naDevices_instr = gtk.Label('No drives available for this operation.')
                self.naDevices_instr.set_alignment(0,0.5)
                top_box.pack_start(self.naDevices_instr, False, False, 0)
                
                blank_instr = gtk.Label('')
                top_box.pack_start(blank_instr, False, False, 0)
                
                self.toggleSingle_radio = gtk.RadioButton(None, 'Single')
                self.toggleSingle_radio.connect('toggled', self.mode_toggled)
                self.toggleMulti_radio = gtk.RadioButton(self.toggleSingle_radio, 'Multi')

                top_box.pack_end(self.toggleMulti_radio, False, False, 0)
                top_box.pack_end(self.toggleSingle_radio, False, False, 0)
                
                self.vbox.pack_start(top_box, False, False, 0)
                
                self.op_instr = gtk.Label('')
                self.op_instr.set_alignment(0,0.5)
                
                self.na_instr = gtk.Label('')
                self.na_instr.set_alignment(0,0.5)
                
                self.naUSB_instr = gtk.Label('No USB detected for Setup USB')
                self.naUSB_instr.set_alignment(0,0.5)
                
                self.cancel_button.connect('clicked', self.returnToMain, False)
                self.revertUser_button.connect('clicked', runop.run_revertErase, self)
                self.revertPSID_button.connect('clicked', runop.run_revertPSID, self)
                self.changePW_button.connect('clicked', runop.run_changePW, self)
                self.setup_next.connect('clicked', runop.run_setupFull, self)
                self.updatePBA_button.connect('clicked', runop.run_pbaWrite, self)
                

                
                self.setupUSB_button.connect('clicked', runop.run_setupUSB, self)
                
                self.setupUserPW.connect('clicked', runop.run_setupUser, self)
                self.removeUser_button.connect('clicked', runop.run_removeUser, self)
                
                self.pbaUnlockReboot.connect("clicked", runop.run_unlockPBA, self, True, False, None)
                self.pbaUnlockOnly.connect("clicked", runop.run_unlockPBA, self, False, False, None)
                
                self.revertOnly_button.connect('clicked', runop.run_revertKeep, self)
                
                self.viewLog.connect('clicked', dialogs.openLog, self)

                self.display_single()
                
                self.display_grid()
                
                self.wait_instr = gtk.Label('Please wait, this may take up to a minute...')
                self.multi_wait_instr = gtk.Label('Please wait, this may take up to a minute per drive...')
                self.pba_wait_instr = gtk.Label('Please wait, writing the preboot image may take up to 15 minutes per drive...\nDo not turn off your computer while setup is ongoing.')
                self.setup_wait_instr = gtk.Label('Please wait, password setup and USB creation will take a few minutes.\nWriting the preboot image to the drive may take up to 15 minutes per drive.\nDo not turn off your computer while setup is ongoing.')
                self.load_instr = gtk.Label('Loading drive information...')
                self.progress_bar = gtk.ProgressBar()
                self.vbox.pack_start(self.wait_instr, False, False, 5)
                self.vbox.pack_start(self.multi_wait_instr, False, False, 5)
                self.vbox.pack_start(self.pba_wait_instr, False, False, 5)
                self.vbox.pack_start(self.setup_wait_instr, False, False, 5)
                self.vbox.pack_start(self.load_instr, False, False, 5)
                self.vbox.pack_start(self.progress_bar, False, False, 5)
                
                self.waitSpin = gtk.Spinner()
                self.vbox.pack_start(self.waitSpin, False, False, 5)
                
                self.vbox.pack_start(self.op_instr, False, False, 5)
                self.vbox.pack_start(self.na_instr, False, False, 5)
                self.vbox.pack_start(self.naUSB_instr, False, False, 5)
                
                self.box_psid = gtk.HBox(homogeneous, 0)
                
                self.revert_psid_label = gtk.Label("Enter PSID (omit dashes)")
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
                    self.drive_menu = gtk.ComboBox()
                    self.auth_menu = gtk.ComboBox()
                    #self.usb_menu = gtk.ComboBox()
                else:
                    self.drive_menu = gtk.combo_box_new_text()
                    self.auth_menu = gtk.combo_box_new_text()
                    #self.usb_menu = gtk.combo_box_new_text()
                    
                    #self.drive_menu.append = self.drive_menu.append_text
                    #self.auth_menu.append = self.auth_menu.append_text
                    #self.usb_menu.append = self.usb_menu.append_text
                
                self.auth_menu.connect('changed', self.auth_changed)
                
                self.box_drive = gtk.HBox(homogeneous, 0)
                
                self.drive_label = gtk.Label("USB (FAT only)")
                self.drive_label.set_width_chars(22)
                self.drive_label.set_alignment(0,0.5)
                self.box_drive.pack_start(self.drive_label, expand, fill, padding)
                
                self.box_drive.pack_start(self.drive_menu, True, True, padding)
                
                self.drive_refresh = gtk.Button('Refresh')
                self.drive_refresh.connect('clicked', self.showDrive)
                
                self.box_drive.pack_start(self.drive_refresh, False, False, padding)
                
                
                drv_blnk = gtk.Label('')
                self.box_drive.pack_start(drv_blnk, False, False, 0)
                
                self.drive_list = []
                
                #self.box_drive.pack_start(self.usb_menu, True, True, padding)
                
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
                self.auth_menu.append_text('Admin')
                self.auth_menu.append_text('User')
                self.auth_menu.set_active(0)
                self.box_auth.pack_start(self.auth_menu, False, False, padding)
                
                self.buttonBox.pack_start(self.setup_next, False, False, padding)

                self.buttonBox.pack_start(self.changePW_button, False, False, padding)
                self.buttonBox.pack_start(self.revertUser_button, False, False, padding)
                self.buttonBox.pack_start(self.revertPSID_button, False, False, padding)
                self.buttonBox.pack_start(self.pbaUnlockReboot, False, False, padding)
                self.buttonBox.pack_start(self.pbaUnlockOnly, False, False, padding)
                self.buttonBox.pack_start(self.updatePBA_button, False, False, padding)
                self.buttonBox.pack_start(self.pbaUSB_button, False, False, padding)
                self.buttonBox.pack_start(self.revertOnly_button, False, False, padding)
                self.buttonBox.pack_start(self.viewLog, False, False, padding)
                self.buttonBox.pack_start(self.setupUSB_button, False, False, padding)
                self.buttonBox.pack_start(self.setupUserPW, False, False, padding)
                self.buttonBox.pack_start(self.removeUser_button, False, False, padding)
                
                self.buttonBox.pack_start(self.cancel_button, False, False, padding)
                
                self.pass_dialog()
                self.new_pass_dialog()
                
                self.mbr_radio = gtk.RadioButton(None, 'Write Preboot Image to Shadow MBR')
                self.skip_radio = gtk.RadioButton(self.mbr_radio, 'I already have another drive or USB for unlocking this drive')
                self.mbr_radio.connect('toggled', self.hideUSB)
                self.skip_radio.connect('toggled', self.hideUSB)
                
                halign = gtk.Alignment(1,0,0,0)
                halign.add(self.buttonBox)
                self.vbox.pack_end(halign, False, False, padding)
                self.vbox.pack_end(self.skip_radio, False)
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
                
                
                self.load_instr.hide()
                
                self.select_box.show()
                self.box_dev.show()
                if self.VERSION == 2 or self.PBA_VERSION == 1:
                    self.userSetup_box.hide()
                
                
                self.main_instr.show()
                self.op_label.show()
                
                
                
                    
                runscan.run_scan(None, self, True)
                
                
                
                if self.DEV_OS == 'Windows':
                    self.addMsgHandler(WM_DEVICECHANGE, self.onDeviceChange)
                    self.addMsgHandler(WM_POWERBROADCAST, self.onPowerBroadcast)
                    self.addMsgHandler(WM_QUERYENDSESSION, self.onEndSession)
                    self.hookWndProc()
                
                ##print self.devs_list
                ##print self.vendor_list
                ##print self.sn_list
                ##print self.salt_list
                ##print self.msid_list
                ##print self.series_list
                
                ##print self.locked_list
                ##print self.setup_list
                ##print self.nonsetup_list
                ##print self.tcg_list
                
                
                #if len(self.devs_list) == 0:
                #    self.msg_err('No drives detected, try running this application with Administrator.')
                #    gtk.main_quit()
                    
                
                        
                
                
            else:
                self.msg_err('No valid license of Opal Lock found, please register to get demo license or buy basic/premium license')
                gtk.main_quit()
               
        def display_single(self, *args):
            homogeneous = False
            spacing = 0
            expand = False
            fill = False
            padding = 0
            width = 12
            
            self.box_dev = gtk.HBox(homogeneous, spacing)
            
            frm_d = gtk.Frame()
            frm_o = gtk.Frame()
            
            dev_info = gtk.VBox(homogeneous, 5)
            opal_info = gtk.VBox(homogeneous, 5)
            
            frm_d.add(dev_info)
            frm_o.add(opal_info)
            
            frm_d.set_label('Drive Information')
            frm_o.set_label('TCG Information')
            
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
            self.userSetup_box = gtk.HBox(homogeneous, spacing)
            
            self.label_dev = gtk.Label("Drive")
            self.label_dev.set_alignment(0, 0.5)
            self.label_dev.set_width_chars(14)
            self.select_box.pack_start(self.label_dev, expand, fill, padding)
            self.label_dev.show()
            
            #self.label_dev2 = gtk.Label("Drive") 
            #self.label_dev2.set_alignment(0, 0.5)
            #self.label_dev2.set_width_chars(14)
            #self.select_box.pack_start(self.label_dev2, expand, fill, padding)
            #self.label_dev2.show()

            if (gtk.gtk_version[1] > 24 or
                (gtk.gtk_version[1] == 24 and gtk.gtk_version[2] > 28)):
                self.dev_select = gtk.ComboBox()
            else:
                self.dev_select = gtk.combo_box_new_text()
                
                #self.dev_select.append = self.dev_select.append_text
                
                
            self.select_box.pack_start(self.dev_select, True, True, padding)
            
            self.dev_select.connect('changed', self.changed_cb)
            
            #self.dev_single = gtk.Entry()
            #self.dev_single.set_width_chars(35)
            #self.dev_single.set_sensitive(False)

            #self.select_box.pack_start(self.dev_single, True, True, padding)
            
            self.vbox.pack_start(self.select_box, False, True, padding)
            
            #dev_label = gtk.Label(" Drive information")
            #dev_label.show()
            #dev_info.pack_start(dev_label, False, False, padding)
            
            vendor_label = gtk.Label("Model Number")
            vendor_label.set_alignment(0, 0.5)
            #vendor_label.set_width_chars(14)
            vendor_label.show()
            vendor_box.pack_start(vendor_label, False, False, 5)
            
            self.dev_vendor = gtk.Entry()
            self.dev_vendor.set_text("")
            self.dev_vendor.set_property("editable", False)
            #self.dev_vendor.set_sensitive(False)
            self.dev_vendor.show()
            self.dev_vendor.set_width_chars(40)
            vendor_box.pack_end(self.dev_vendor, False, False, 5)
            
            dev_info.pack_start(vendor_box, False, False, 1)
            
            sn_label = gtk.Label("Serial Number")
            sn_label.set_alignment(0, 0.5)
            #sn_label.set_width_chars(14)
            sn_label.show()
            sn_box.pack_start(sn_label, False, False, 5)
            
            self.dev_sn = gtk.Entry()
            self.dev_sn.set_text("")
            self.dev_sn.set_property("editable", False)
            #self.dev_sn.set_sensitive(False)
            self.dev_sn.show()
            self.dev_sn.set_width_chars(40)
            sn_box.pack_end(self.dev_sn, False, False, 5)
            
            dev_info.pack_start(sn_box, False, False, 1)
            
            msid_label = gtk.Label("MSID")
            msid_label.set_alignment(0,0.5)
            #@msid_label.set_width_chars(14)
            msid_label.show()
            msid_box.pack_start(msid_label, False, False, 5)
            
            self.dev_msid = gtk.Entry()
            self.dev_msid.set_text("")
            self.dev_msid.set_property("editable", False)
            #self.dev_msid.set_editable(False)
            self.dev_msid.show()
            self.dev_msid.set_width_chars(40)
            msid_box.pack_end(self.dev_msid, False, False, 5)
            
            dev_info.pack_start(msid_box, False, False, 1)
            
            series_label = gtk.Label("Firmware")
            series_label.set_alignment(0,0.5)
            #series_label.set_width_chars(14)
            series_label.show()
            series_box.pack_start(series_label, False, False, 5)
            
            self.dev_series = gtk.Entry()
            self.dev_series.set_text("")
            self.dev_series.set_property("editable", False)
            #self.dev_series.set_sensitive(False)
            self.dev_series.show()
            self.dev_series.set_width_chars(40)
            series_box.pack_end(self.dev_series, False, False, 5)
            
            dev_info.pack_start(series_box, False, False, 1)
            
            dev_pbalabel = gtk.Label("Preboot Image")
            dev_pbalabel.set_alignment(0,0.5)
            #dev_pbalabel.set_width_chars(14)
            dev_pbalabel.show()
            pbaver_box.pack_start(dev_pbalabel, False, False, 5)
            
            self.dev_pbaVer = gtk.Entry()
            self.dev_pbaVer.set_property("editable", False)
            #self.dev_pbaVer.set_sensitive(False)
            self.dev_pbaVer.set_width_chars(40)
            self.dev_pbaVer.show()
            pbaver_box.pack_end(self.dev_pbaVer, False, False, 5)
            
            
            
            dev_info.pack_start(pbaver_box, False, False, 1)
            
            
            
            #opal_info
            
            #opal_label = gtk.Label(" TCG information")
            #opal_label.show()
            #opal_info.pack_start(opal_label, False, False, padding)
            
            self.dev_opal_ver = gtk.Entry()
            self.dev_opal_ver.set_text("")
            self.dev_opal_ver.set_width_chars(18)
            self.dev_opal_ver.set_property("editable", False)
            #self.dev_opal_ver.set_sensitive(False)
            self.dev_opal_ver.show()
            ssc_box.pack_end(self.dev_opal_ver, False, False, 5)
            
            label_opal_ver = gtk.Label('TCG Version')
            label_opal_ver.set_alignment(0,0.5)
            #label_opal_ver.set_width_chars(14)
            label_opal_ver.show()
            ssc_box.pack_start(label_opal_ver, False, False, 5)
            
            opal_info.pack_start(ssc_box, False, True, 1)
            
            self.dev_status = gtk.Entry()
            self.dev_status.set_text("")
            self.dev_status.set_property("editable", False)
            #self.dev_status.set_sensitive(False)
            self.dev_status.set_width_chars(18)
            self.dev_status.show()
            status_box.pack_end(self.dev_status, False, False, 5)
            
            status_label = gtk.Label("Lock Status")
            status_label.set_alignment(0,0.5)
            #status_label.set_width_chars(14)
            status_label.show()
            status_box.pack_start(status_label, False, False, 5)
            
            opal_info.pack_start(status_box, False, False, 1)
            
            self.dev_setup = gtk.Entry()
            self.dev_setup.set_text("")
            self.dev_setup.set_property("editable", False)
            #self.dev_setup.set_sensitive(False)
            self.dev_setup.set_width_chars(18)
            self.dev_setup.show()
            setup_box.pack_end(self.dev_setup, False, False, 5)
            
            setup_label = gtk.Label("Setup Status")
            setup_label.set_alignment(0,0.5)
            #setup_label.set_width_chars(14)
            setup_label.show()
            setup_box.pack_start(setup_label, False, False, 5)
            
            opal_info.pack_start(setup_box, False, False, 1)
            
            
            self.dev_enc = gtk.Entry()
            self.dev_enc.set_text("")
            self.dev_enc.set_property("editable", False)
            #self.dev_enc.set_sensitive(False)
            self.dev_enc.set_width_chars(18)
            self.dev_enc.show()
            enc_box.pack_end(self.dev_enc, False, False, 5)
            
            enc_label = gtk.Label("Encryption")
            enc_label.set_alignment(0,0.5)
            #enc_label.set_width_chars(14)
            enc_label.show()
            enc_box.pack_start(enc_label, False, False, 5)
            
            opal_info.pack_start(enc_box, False, False, 1)
            
            
            self.dev_blockSID = gtk.Entry()
            self.dev_blockSID.set_text("")
            self.dev_blockSID.set_property("editable", False)
            #self.dev_blockSID.set_sensitive(False)
            self.dev_blockSID.set_width_chars(18)
            self.dev_blockSID.show()
            blockSID_box.pack_end(self.dev_blockSID, False, False, 5)
            
            blockSID_label = gtk.Label("Block SID")
            blockSID_label.set_alignment(0,0.5)
            #blockSID_label.set_width_chars(14)
            blockSID_label.show()
            blockSID_box.pack_start(blockSID_label, False, False, 5)
            
            opal_info.pack_start(blockSID_box, False, False, 1)
            
            
            self.dev_userSetup = gtk.Entry()
            self.dev_userSetup.set_text("")
            self.dev_userSetup.set_property("editable", False)
            #self.dev_blockSID.set_sensitive(False)
            self.dev_userSetup.set_width_chars(18)
            if self.VERSION == 3 or self.PBA_VERSION != 1:
                self.dev_userSetup.show()
            else:
                self.dev_userSetup.hide()
            self.userSetup_box.pack_end(self.dev_userSetup, False, False, 5)
            
            self.userSetup_label = gtk.Label("User Setup")
            self.userSetup_label.set_alignment(0,0.5)
            #self.userSetup_label.set_width_chars(14)
            if self.VERSION == 3 or self.PBA_VERSION != 1:
                self.userSetup_label.show()
            else:
                self.userSetup_label.hide()
            self.userSetup_box.pack_start(self.userSetup_label, False, False, 5)
            
            opal_info.pack_start(self.userSetup_box, False, False, 1)
            
            self.box_dev.pack_start(frm_d, True, True, padding)
            self.box_dev.pack_start(frm_o, True, True, padding)
            
            self.vbox.pack_start(self.box_dev, False)
            
        def display_grid(self,*args):
            self.selectAll_check = gtk.CheckButton('Select/Deselect All')
            self.selectAll_check.connect('toggled', self.selectAll_toggle)
            
        
            column_names = ['Select', 'Drive', 'Model Number', 'Serial Number']
            self.liststore = gtk.ListStore(bool, str, str, str, bool)
            self.treeview = gtk.TreeView()
            
            self.tvcolumn = [None] * len(column_names)
            
            cell_check = gtk.CellRendererToggle()
            cell_check.connect('toggled', self.grid_ms_toggle)
            self.tvcolumn[0] = gtk.TreeViewColumn(column_names[0], cell_check, active=0, visible=4)
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
            verify.licCheck(self)
            self.selectAll_check.set_inconsistent(False)
            value = self.selectAll_check.get_active()
            #def Toggle(store, path, itr, v):
            #    self.liststore[path][0] = v
            #self.liststore.foreach(Toggle, value)
            index = 0
            iter = self.liststore.get_iter_first()
            
            while iter != None:
                if self.view_state == 1 and self.auth_menu.get_active() == 0 and index in self.locked_list:
                    self.liststore.set_value(iter, 0, value)
                elif self.view_state == 1 and self.auth_menu.get_active() == 1 and index in self.ulocked_list:
                    self.liststore.set_value(iter, 0, value)
                elif self.view_state == 2 and self.auth_menu.get_active() == 0 and index in self.setup_list:
                    self.liststore.set_value(iter, 0, value)
                elif self.view_state == 2 and self.auth_menu.get_active() == 1 and index in self.usetup_list:
                    self.liststore.set_value(iter, 0, value)
                elif self.view_state == 4 and index in self.nonsetup_list:
                    self.liststore.set_value(iter, 0, value)
                elif self.view_state == 5 and index in self.tcg_list:
                    self.liststore.set_value(iter, 0, value)
                elif self.view_state == 7 and index in self.mbr_setup_list:
                    self.liststore.set_value(iter, 0, value)
                iter = self.liststore.iter_next(iter)
                index = index + 1
            
        def grid_ms_toggle(self, cell, path, data=None):
            verify.licCheck(self)
            i = self.liststore.get_iter(path)
            curr_val = self.liststore.get_value(i, 0)
            self.liststore.set_value(i, 0, not curr_val)
            
            #self.treeview.grab_focus()
            #self.treeview.emit('toggle-cursor-row')
            
            checked = list()
            self.liststore.foreach(lambda store, path, itr: checked.append(store[path][0]))
            all_checked = True
            none_checked = True
            for i in range(len(checked)):
                if self.view_state == 1 and self.auth_menu.get_active() == 0:
                    if i in self.locked_list and checked[i]:
                        none_checked = False
                    elif i in self.locked_list and not checked[i]:
                        all_checked = False
                elif self.view_state == 1:
                    if i in self.ulocked_list and checked[i]:
                        none_checked = False
                    elif i in self.ulocked_list and not checked[i]:
                        all_checked = False
                elif self.view_state == 2 and self.auth_menu.get_active() == 0:
                    if i in self.setup_list and checked[i]:
                        none_checked = False
                    elif i in self.setup_list and not checked[i]:
                        all_checked = False
                elif self.view_state == 2:
                    if i in self.usetup_list and checked[i]:
                        none_checked = False
                    elif i in self.usetup_list and not checked[i]:
                        all_checked = False
                elif self.view_state == 4:
                    if i in self.nonsetup_list and checked[i]:
                        none_checked = False
                    elif i in self.nonsetup_list and not checked[i]:
                        all_checked = False
                elif self.view_state == 5:
                    if i in self.tcg_list and checked[i]:
                        none_checked = False
                    elif i in self.tcg_list and not checked[i]:
                        all_checked = False
                elif self.view_state == 7:
                    if i in self.mbr_setup_list and checked[i]:
                        none_checked = False
                    elif i in self.mbr_setup_list and not checked[i]:
                        all_checked = False
            if all_checked:
                self.selectAll_check.set_inconsistent(False)
                self.selectAll_check.set_active(True)
            elif none_checked:
                self.selectAll_check.set_inconsistent(False)
                self.selectAll_check.set_active(False)
            else:
                self.selectAll_check.set_inconsistent(True)
            
        def entry_check_box_pass(self, widget, checkbox):
            verify.licCheck(self)
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
            verify.licCheck(self)
            b_entry_checkbox = checkbox.get_active()
            if b_entry_checkbox:
                self.pass_entry.set_text("")
                self.pass_entry.set_sensitive(False)
            else:
                self.pass_entry.set_sensitive(True)
                
            if self.check_pass_rd.get_active():
                if self.check_both:
                    self.pass_sav.set_active(True)
                    self.drive_label.hide()
                    self.drive_menu.hide()
                    self.drive_refresh.hide()
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
                
        def showDrive(self, *args):
            b_entry_checkbox = self.pass_sav.get_active()
            verify.licCheck(self)
            if not self.mode_setupUSB:
                if b_entry_checkbox and (not self.check_pass_rd.get_active() or self.op_prompt == 6):
                    #print 'save checked and read unchecked or in setup user prompt, drive menu should appear'
                    def t1_run():
                        #self.drive_list = []
                        d_list = []
                        
                        if self.DEV_OS == 'Windows':
                            txt = os.popen('wmic logicaldisk where "drivetype=2" get caption,filesystem').read()
                            txt_regex = '([D-Z]:)\s+FAT'
                            d_list = re.findall(txt_regex,txt)
                        elif self.DEV_OS == 'Linux':
                            txt = os.popen("for DLIST in `dmesg  | grep \"Attached SCSI removable disk\" | cut -d\" \" -f 3  | sed -e 's/\[//' -e 's/\]//'` ; do\necho $DLIST\ndone ").read()
                            #print txt
                            txt_regex = 'sd[a-z]'
                            list_u = re.findall(txt_regex,txt)
                            #print list_u
                            for u in list_u:
                                txt1 = os.popen(self.prefix + 'mount').read()
                                m = re.search(u,txt1)
                                if not m:
                                    txt2 = os.popen(self.prefix + 'blkid').read()
                                    rgx = '(' + u + '1?).+'
                                    m1 = re.search(rgx,txt2)
                                    if m1:
                                        r2 = '\s+TYPE="([a-z]+)"'
                                        txt3 = m1.group(0)
                                        m2 = re.search(r2,txt3)
                                        type_a = m2.group(1)
                                        if type_a != 'ntfs' and type_a != 'exfat':
                                            s = os.system(self.prefix + 'mount -t ' + type_a + ' /dev/' + m1.group(1))
                        
                            txt = os.popen(self.prefix + 'mount').read()
                            dev_regex = '/dev/(sd[a-z])[1-9]?\s*on\s*(\S+)\s*type'
                            full_list = re.findall(dev_regex, txt)
                            for m in full_list:
                                if m[0] in list_u:
                                    blktxt = os.popen(self.prefix + 'blkid').read()
                                    dev_d = '/dev/' + m[0]
                                    n = re.search(dev_d, blktxt)
                                    if n:
                                        #self.drive_list.append(m[1])
                                        d_list.append(m[1])
                        gobject.idle_add(cleanup, d_list)
                                        
                    
                    def cleanup(d_list):
                        #print 'showDrive thread cleanup, menu should be showing'
                        self.drive_list = d_list
                        model = self.drive_menu.get_model()
                        
                        iter = gtk.TreeIter
                        for row in model:
                            model.remove(row.iter)
                        
                        length = len(self.drive_list)
                        
                        if length > 0:
                            for d in self.drive_list:
                                self.drive_menu.append_text(d)
                            self.drive_menu.set_active(0)
                        
                        self.drive_label.show()
                        self.drive_menu.show()
                        self.drive_refresh.show()
                    t = threading.Thread(target=t1_run, args=())
                    t.start()
                else:
                    self.drive_label.hide()
                    self.drive_menu.hide()
                    self.drive_refresh.hide()
                if self.check_exclusive and self.pass_sav.get_active():
                    self.check_pass_rd.set_sensitive(False)
                elif self.check_exclusive:
                    self.check_pass_rd.set_sensitive(True)
            else:
                if b_entry_checkbox:
                    if self.op_prompt != 2:
                        i = self.dev_select.get_active()
                        if self.setupuser_list[i] == 'Yes' or self.setupuser_list[i] == None:
                            self.box_auth.show()
                        self.box_pass.show()
                    self.box_drive.show()
                    self.drive_label.hide()
                    self.drive_menu.hide()
                    self.drive_refresh.hide()
                    self.check_box_pass.show()
                else:
                    if self.op_prompt != 2:
                        self.box_auth.hide()
                        self.auth_menu.set_active(0)
                        self.box_pass.hide()
                    self.box_drive.hide()

        def onDeviceChange(self,wParam,lParam):
            background.devChange(self, wParam, lParam)
            
        def onPowerBroadcast(self,wParam,lParam):
            background.powerBroadcast(self, wParam, lParam)

        def onEndSession(self,wParam, lParam):
            background.endSession(self, wParam, lParam)
        
        def changed_cb(self, entry):
            verify.licCheck(self)
            #if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
            #    self.check_pass_rd.set_active(False)
            #    self.pass_sav.set_active(False)
            act_idx = self.dev_select.get_active()
            index = -1
            #if self.view_state != 7:
            index = act_idx
            if self.view_state == 1:
                if act_idx in self.locked_list:
                    self.na_instr.hide()
                    self.op_instr.show()
                    if self.VERSION != 0 and self.PBA_VERSION != 0:
                        self.enable_entries_buttons()
                    if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
                        if self.check_pass_rd.get_active():
                            self.pass_entry.set_text("")
                            self.pass_entry.set_sensitive(False)
                        if self.setupuser_list[act_idx] == 'Yes':# or self.setupuser_list[act_idx] == None:
                            self.box_auth.show()
                        else:
                            self.box_auth.hide()
                            self.auth_menu.set_active(0)
                else:
                    self.op_instr.hide()
                    self.disable_entries_buttons()
                    if act_idx in self.tcg_list:
                        self.na_instr.set_text('This drive is not locked.')
                    else:
                        self.na_instr.set_text('This drive is not a TCG drive.')
                    if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
                        if self.setupuser_list[act_idx] == 'Yes':# or self.setupuser_list[act_idx] == None:
                            self.box_auth.show()
                        else:
                            self.box_auth.hide()
                            self.auth_menu.set_active(0)
                    self.na_instr.show()
            elif self.view_state == 2:
                if act_idx in self.setup_list and (self.op_prompt != 6 or self.setupuser_list[act_idx] != 'Yes') and (self.op_prompt != 12 or self.setupuser_list[act_idx] != 'No') and (self.op_prompt != 3 or self.datastore_list[act_idx] == 'Supported'):
                    self.na_instr.hide()
                    self.op_instr.show()
                    if self.VERSION != 0 and self.PBA_VERSION != 0:
                        self.enable_entries_buttons()
                    if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
                        if self.check_pass_rd.get_active():
                            self.pass_entry.set_text("")
                            self.pass_entry.set_sensitive(False)
                    if (self.op_prompt == 4 or self.op_prompt == 5) and (self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1)) and self.pass_sav.get_active():
                        if self.setupuser_list[act_idx] == 'Yes':# or self.setupuser_list[act_idx] == None:
                            self.box_auth.show()
                        else:
                            self.box_auth.hide()
                            self.auth_menu.set_active(0)
                    #elif self.op_prompt == 5 and (self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1)) and self.pass_sav.get_active():
                    #    if self.setupuser_list[act_idx] == 'Yes' or self.setupuser_list[act_idx] == None:
                    #        self.box_auth.show()
                    #    else:
                    #        self.box_auth.hide()
                    #        self.auth_menu.set_active(0)
                else:
                    self.op_instr.hide()
                    self.disable_entries_buttons()
                    if self.op_prompt == 6 and self.setupuser_list[act_idx] == 'Yes':
                        self.na_instr.set_text('User has already been set up for this drive.')
                    elif self.op_prompt == 6 and self.setupuser_list[act_idx] == 'Not Supported':
                        self.na_instr.set_text('User is not supported on this drive.')
                    elif self.op_prompt == 12 and self.setupuser_list[act_idx] != 'Yes':
                        self.na_instr.set_text('User is not set up for this drive.')
                    elif self.op_prompt == 3 and self.datastore_list[act_idx] == 'Not Supported':
                        self.na_instr.set_text('Preboot image is not supported on this drive.')
                    
                    elif act_idx in self.tcg_list:
                        self.na_instr.set_text('This drive has not been set up.')
                    else:
                        self.na_instr.set_text('This drive is not a TCG drive.')
                    if (self.op_prompt == 4 or self.op_prompt == 5) and (self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1)) and self.pass_sav.get_active():
                        if self.setupuser_list[act_idx] == 'Yes':# or self.setupuser_list[act_idx] == None:
                            self.box_auth.show()
                        else:
                            self.box_auth.hide()
                            self.auth_menu.set_active(0)
                    self.na_instr.show()
            elif self.view_state == 4:
                if act_idx in self.nonsetup_list and (self.VERSION != 2 or self.datastore_list[act_idx] == 'Supported'):
                    self.na_instr.hide()
                    self.op_instr.show()
                    if self.VERSION != 0 and self.PBA_VERSION != 0:
                        self.enable_entries_buttons()
                    if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
                        if self.check_pass_rd.get_active():
                            self.pass_entry.set_text("")
                            self.pass_entry.set_sensitive(False)
                else:
                    self.op_instr.hide()
                    self.disable_entries_buttons()
                    if act_idx in self.setup_list:
                        self.na_instr.set_text('This drive is already set up.')
                    elif act_idx in self.nonsetup_list:
                        self.na_instr.set_text('Standard version only supports drives that support preboot image.')
                    else:
                        self.na_instr.set_text('This drive is not a TCG drive.')
                    self.na_instr.show()
            elif self.view_state == 5:
                if act_idx in self.tcg_list:
                    self.na_instr.hide()
                    self.op_instr.show()
                    if self.VERSION != 0 and self.PBA_VERSION != 0:
                        self.enable_entries_buttons()
                    if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
                        if self.check_pass_rd.get_active():
                            self.pass_entry.set_text("")
                            self.pass_entry.set_sensitive(False)
                else:
                    self.op_instr.hide()
                    self.disable_entries_buttons()
                    self.na_instr.set_text('This drive is not a TCG drive.')
                    self.na_instr.show()
            elif self.view_state == 6:
                if act_idx in self.tcg_list and self.datastore_list[act_idx] == 'Supported':
                    self.na_instr.hide()
                    self.op_instr.show()
                    if self.VERSION != 0 and self.PBA_VERSION != 0:
                        self.enable_entries_buttons()
                    if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
                        if self.check_pass_rd.get_active():
                            self.pass_entry.set_text("")
                            self.pass_entry.set_sensitive(False)
                        if self.op_prompt == 1:
                            if self.setupuser_list[act_idx] == 'Yes':# or self.setupuser_list[act_idx] == None:
                                self.box_auth.show()
                            else:
                                self.box_auth.hide()
                                self.auth_menu.set_active(0)
                else:
                    self.op_instr.hide()
                    self.disable_entries_buttons()
                    if act_idx in self.tcg_list:
                        self.na_instr.set_text('Audit Log is not supported on this drive.')
                    else:
                        self.na_instr.set_text('This drive is not a TCG drive.')
                    if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
                        if self.op_prompt == 1:
                            if self.setupuser_list[act_idx] == 'Yes':# or self.setupuser_list[act_idx] == None:
                                self.box_auth.show()
                            else:
                                self.box_auth.hide()
                                self.auth_menu.set_active(0)
                    self.na_instr.show()
                    
            elif self.view_state == 7:# and act_idx >= 0:
                #index = self.sel_list[act_idx]
                if act_idx in self.mbr_setup_list:
                    self.na_instr.hide()
                    self.op_instr.show()
                    if self.VERSION != 0 and self.PBA_VERSION != 0:
                        self.enable_entries_buttons()
                    if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
                        if self.check_pass_rd.get_active():
                            self.pass_entry.set_text("")
                            self.pass_entry.set_sensitive(False)
                else:
                    self.op_instr.hide()
                    self.disable_entries_buttons()
                    if act_idx in self.mbr_list and act_idx in self.nonsetup_list:
                        self.na_instr.set_text('This drive has not been set up.')
                    elif act_idx in self.tcg_list:
                        self.na_instr.set_text('Preboot image is not supported on this drive.')
                    else:
                        self.na_instr.set_text('This drive is not a TCG drive.')
                    self.na_instr.show()
                #if self.pba_list[index] == 'Not Supported': #change to get rid of usb radio, disable mbr_radio
                #    self.na_instr.set_text('Preboot image is not supported on this drive.')
                #    self.na_instr.show()
                #else:
                #    self.na_instr.hide()
            #self.dev_single.set_text(self.devs_list[index] + self.label_list[index])
            if index >= 0:
                self.devname = self.devs_list[index]
                
                self.dev_vendor.set_text(self.vendor_list[index])
                self.dev_sn.set_text(self.sn_list[index])
                if self.msid_list[index] != None:
                    self.dev_msid.set_text(self.msid_list[index])
                else:
                    self.dev_msid.set_text('Loading...')
                self.dev_series.set_text(self.series_list[index])
                if self.pba_list[index] != None:
                    self.dev_pbaVer.set_text(self.pba_list[index])
                else:
                    self.dev_pbaVer.set_text('Loading...')
                
                self.dev_opal_ver.set_text(self.opal_ver_list[index])
                self.dev_status.set_text(self.lockstatus_list[index])
                if self.setupstatus_list[index] != None:
                    self.dev_setup.set_text(self.setupstatus_list[index])
                else:
                    self.dev_setup.set_text('Loading...')
                self.dev_enc.set_text(self.encsup_list[index])
                self.dev_blockSID.set_text(self.blockSID_list[index])
                if self.setupuser_list[index] != None:
                    self.dev_userSetup.set_text(self.setupuser_list[index])
                else:
                    self.dev_userSetup.set_text('Loading...')
                
                if self.opal_ver_list[index] != "None":
                    self.scanning = True
                    dialogs.query(None,self,1)
                    self.scanning = False
                else:
                    self.dev_opal_ver.set_text("None")
                    self.dev_status.set_text("N/A")
                    self.dev_enc.set_text("N/A")
                    self.dev_msid.set_text("N/A")
                    self.dev_setup.set_text("N/A")
                    if self.series_list[index] != None:
                        self.dev_series.set_text(self.series_list[index])
                    else:
                        self.dev_series.set_text("N/A")
                    self.dev_blockSID.set_text("N/A")
                    self.dev_pbaVer.set_text("N/A")
                    self.dev_userSetup.set_text('N/A')
                if self.VERSION == 3 or (self.VERSION == 1 and self.PBA_VERSION != 1):
                    if self.op_prompt == 4 or self.op_prompt == 7:
                        i = self.dev_select.get_active()
                        if self.setupuser_list[i] == 'Yes':# or self.setupuser_list[i] == None:
                            self.box_auth.show()
                        else:
                            self.box_auth.hide()
                            self.auth_menu.set_active(0)
                           
        def auth_changed(self, *args):
            verify.licCheck(self)
            if (self.op_prompt == 4 or self.op_prompt == 7) and self.toggleMulti_radio.get_active():
                self.liststore.clear()
                self.selectAll_check.set_inconsistent(False)
                self.selectAll_check.set_active(False)
                self.selectAll_check.show()
                if self.op_prompt == 4:
                    if self.auth_menu.get_active() == 0:
                        for i in range(len(self.devs_list)):
                            if i in self.setup_list:
                                self.liststore.append([False, self.devs_list[i], self.vendor_list[i], self.sn_list[i], True])
                            else:
                                self.liststore.append([False, self.devs_list[i], self.vendor_list[i], self.sn_list[i], False])
                        if len(self.setup_list) > 0:
            #                for i in self.setup_list:
            #                    self.liststore.append([False, self.devs_list[i], self.vendor_list[i], self.sn_list[i]])
                            if self.VERSION != 0 and self.PBA_VERSION != 0:
                                self.enable_entries_buttons()
                            self.op_instr.show()
                            self.na_instr.hide()
                            self.selectMulti_instr.show()
                        else:
                            self.disable_entries_buttons()
                            self.selectMulti_instr.hide()
                            self.naDevices_instr.show()
                    else:
                        for i in range(len(self.devs_list)):
                            if i in self.usetup_list:
                                self.liststore.append([False, self.devs_list[i], self.vendor_list[i], self.sn_list[i], True])
                            else:
                                self.liststore.append([False, self.devs_list[i], self.vendor_list[i], self.sn_list[i], False])
                        if len(self.usetup_list) > 0:
            #                for i in self.usetup_list:
            #                    self.liststore.append([False, self.devs_list[i], self.vendor_list[i], self.sn_list[i]])
                            if self.VERSION != 0 and self.PBA_VERSION != 0:
                                self.enable_entries_buttons()
                            self.op_instr.show()
                            self.na_instr.hide()
                            self.selectMulti_instr.show()
                        else:
                            self.disable_entries_buttons()
                            self.selectMulti_instr.hide()
                            self.naDevices_instr.show()
                else:
                    if self.auth_menu.get_active() == 0:
                        for i in range(len(self.devs_list)):
                            if i in self.locked_list:
                                self.liststore.append([False, self.devs_list[i], self.vendor_list[i], self.sn_list[i], True])
                            else:
                                self.liststore.append([False, self.devs_list[i], self.vendor_list[i], self.sn_list[i], False])
                        if len(self.locked_list) > 0:
            #                for i in self.locked_list:
            #                    self.liststore.append([False, self.devs_list[i], self.vendor_list[i], self.sn_list[i]])
                            if self.VERSION != 0 and self.PBA_VERSION != 0:
                                self.enable_entries_buttons()
                            self.op_instr.show()
                            self.na_instr.hide()
                            self.selectMulti_instr.show()
                        else:
                            self.disable_entries_buttons()
                            self.selectMulti_instr.hide()
                            self.naDevices_instr.show()
                    else:
                        for i in range(len(self.devs_list)):
                            if i in self.ulocked_list:
                                self.liststore.append([False, self.devs_list[i], self.vendor_list[i], self.sn_list[i], True])
                            else:
                                self.liststore.append([False, self.devs_list[i], self.vendor_list[i], self.sn_list[i], False])
                        if len(self.ulocked_list) > 0:
            #                for i in self.ulocked_list:
            #                    self.liststore.append([False, self.devs_list[i], self.vendor_list[i], self.sn_list[i]])
                            if self.VERSION != 0 and self.PBA_VERSION != 0:
                                self.enable_entries_buttons()
                            self.op_instr.show()
                            self.na_instr.hide()
                            self.selectMulti_instr.show()
                        else:
                            self.disable_entries_buttons()
                            self.selectMulti_instr.hide()
                            self.naDevices_instr.show()
                selection = self.treeview.get_selection()
                selection.set_select_function(
                    # Row selectable only if sensitive
                    lambda path: self.liststore[path][4]
                )
                selection.set_mode(gtk.SELECTION_MULTIPLE)
                
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
                print "VERSION: " + str(self.VERSION) + " PBA_VERSION: " + str(self.PBA_VERSION)
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
                print "VERSION: " + str(self.VERSION) + " PBA_VERSION: " + str(self.PBA_VERSION)
                self.pass_sav = gtk.CheckButton("Save to USB")
                self.pass_sav.connect("clicked", self.showDrive)
                self.pass_sav.show()
                self.pass_sav.set_tooltip_text('Save the password to a file on a USB drive')
                self.box_newpass.pack_end(self.pass_sav, False, False, padding)
            
        def openLog_prompt(self, *args):
            verify.licCheck(self)
            self.hideAll()
            self.select_box.show()
            self.box_dev.show()
            if self.VERSION == 2 or self.PBA_VERSION == 1:
                self.userSetup_box.hide()
            self.op_label.set_text('View Audit Log')
            self.op_instr.set_text('A drive\'s audit log stores a log of actions done on the drive by this application.\nEnter the drive\'s password to access its audit log.')
            self.na_instr.set_text('Audit log is not available for non-TCG drives')
            self.cancel_button.show()
            
            curr_idx = self.dev_select.get_active()
            
            self.op_prompt = 1
            
            self.mode_setupUSB = False
            
            if self.view_state != 6:
                self.view_state = 6
            
            if len(self.devs_list) > 0:
                self.select_instr.show()
                
            if len(self.devs_list) == 0:
                self.naDev()
            else:
                self.box_pass.show()
                    
                self.box_newpass.show()
                self.new_pass_label.hide()
                self.new_pass_entry.hide()
                
                #if self.VERSION == 1 and self.PBA_VERSION == 1:
                #    self.check_pass_rd.hide()
                #    self.pass_sav.hide()
                
                self.box_drive.show()
                self.drive_label.hide()
                self.drive_menu.hide()
                self.drive_refresh.hide()
                #self.usb_menu.hide()
                
                self.check_exclusive = True
                
                self.viewLog.show()
                
                self.check_box_pass.show()


                self.pass_entry.set_activates_default(True)
                self.pass_entry.grab_focus()
            
                self.set_default(self.viewLog)
                
                if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
                    i = self.dev_select.get_active()
                    if self.setupuser_list[i] == 'Yes' or self.setupuser_list[i] == None:
                        self.box_auth.show()
                
                if curr_idx in self.tcg_list:
                    self.op_instr.show()
                    self.na_instr.hide()
                    if self.VERSION != 0 and self.PBA_VERSION != 0:
                        self.enable_entries_buttons()
                    else:
                        self.disable_entries_buttons()
                
                else:
                    self.na_instr.show()
                    self.disable_entries_buttons()
                    
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

        def returnToMain(self, button, reset, *args):
            verify.licCheck(self)
            self.hideAll()
            self.select_box.show()
            self.selectMulti_instr.hide()
            self.select_instr.hide()
            self.box_dev.show()
            if self.VERSION == 2 or self.PBA_VERSION == 1:
                self.userSetup_box.hide()
            self.op_label.set_text('Main')
            self.main_instr.show()
            self.op_instr.hide()
            self.op_prompt = 0
            self.enable_menu()
            
            if self.view_state != 0:
                self.view_state = 0
            
            length = 0
                
            length = len(self.devs_list)
            
            if reset:
                model = self.dev_select.get_model()
                
                iter = gtk.TreeIter
                for row in model:
                    model.remove(row.iter)
                
                
                for i in range(length):
                    if self.label_list[i] != '':
                        self.dev_select.append_text(self.devs_list[i] + ' ' + self.label_list[i])
                    else:
                        self.dev_select.append_text(self.devs_list[i])
                
                if len(self.devs_list) > 0:
                    self.dev_select.set_active(0)
                
            dialogs.query(None,self,1)

            if length == 0:
                self.dev_vendor.set_text('N/A')
                self.dev_sn.set_text('N/A')
                self.dev_msid.set_text('N/A')
                self.dev_opal_ver.set_text('N/A')
                self.dev_status.set_text('N/A')
                self.dev_setup.set_text('N/A')
                self.dev_series.set_text('N/A')
                self.dev_blockSID.set_text('N/A')
                self.dev_enc.set_text('N/A')
                self.dev_pbaVer.set_text('N/A')
                
            numTCG = len(self.tcg_list)
            if numTCG == 0:
                self.noTCG_instr.show()
                self.main_instr.hide()
            else:
                self.noTCG_instr.hide()
                self.main_instr.show()
                
        def setup_prompt1(self, *args):
            verify.licCheck(self)
            self.hideAll()
            self.select_box.show()
            self.box_dev.show()
            if self.VERSION == 2 or self.PBA_VERSION == 1:
                self.userSetup_box.hide()
            curr_idx = self.dev_select.get_active()
            
            self.mode_setupUSB = True
            
            self.op_prompt = 2
            
            self.cancel_button.show()
            self.op_label.set_text('Set Up Drive(s)')
            self.op_instr.set_text('Setting up a drive includes setting a password which you can use to unlock the drive.\nEnter the new password for the drive and click \'Set up Drive(s)\'.')
            
            if self.view_state != 4:
                self.view_state = 4
            if len(self.devs_list) > 0 and len(self.nonsetup_list) <= 1:
                self.select_instr.show()
            elif len(self.nonsetup_list) > 1:
                self.selectMulti_instr.show()
            
            if self.VERSION % 3 == 0:
                self.toggleSingle_radio.show()
                self.toggleMulti_radio.show()
                if len(self.nonsetup_list) > 1:
                    if self.toggleSingle_radio.get_active():
                        self.toggleMulti_radio.set_active(True)
                    else:
                        self.mode_toggled(None)
                else:
                    if self.toggleMulti_radio.get_active():
                        self.toggleSingle_radio.set_active(True)
                    else:
                        self.mode_toggled(None)
                
            
                
            if len(self.devs_list) > 0:
                self.box_newpass.show()
                self.new_pass_label.show()
                self.new_pass_entry.show()
                
                self.box_drive.show()
                self.drive_menu.hide()
                self.drive_label.hide()
                self.drive_refresh.hide()
                #self.usb_menu.hide()
                
                count = 0
                
                if self.VERSION == 3:
                    
                    self.pass_sav.set_active(True)
                    self.pass_sav.set_sensitive(False)
                
                self.box_newpass_confirm.show()
                self.setup_next.show()
                
                self.check_box_pass.show()
                
                if len(self.nonsetup_list) <= 1:
                    if curr_idx in self.nonsetup_list:# and (self.VERSION != 3 or count > 0):
                        self.op_instr.show()
                        self.na_instr.hide()
                        if self.VERSION != 0 and self.PBA_VERSION != 0:
                            self.enable_entries_buttons()
                        else:
                            self.disable_entries_buttons()
                    else:
                        if curr_idx in self.tcg_list and curr_idx in self.setup_list:
                            #if count > 0:
                            self.na_instr.set_text('This drive has already been set up.')
                            #else:
                            #    self.na_instr.set_text('No USB detected.  A USB is required for setting up a bootable USB, which can then be used as a way\nto unlock your drive(s).')
                        elif self.blockSID_list[curr_idx] == 'Enabled':
                            self.na_instr.set_text('This drive cannot be set up because BlockSID is enabled.\nBlockSID prevents the password from being set.')
                        else:
                            self.na_instr.set_text('This drive is not a TCG drive.')
                        self.op_instr.hide()
                        self.na_instr.show()
                        self.disable_entries_buttons()
                else:
                    #if count > 0:
                    self.op_instr.show()
                    self.na_instr.hide()
                    if self.VERSION % 3 == 0:
                        if self.VERSION != 0 and self.PBA_VERSION != 0:
                            self.enable_entries_buttons()
                        else:
                            self.disable_entries_buttons()
                        if curr_idx not in self.nonsetup_list:
                            if curr_idx in self.tcg_list:
                                self.na_instr.set_text('This drive has already been set up.')
                            elif self.blockSID_list[curr_idx] == 'Enabled':
                                self.na_instr.set_text('This drive cannot be set up because BlockSID is enabled.\nBlockSID prevents the password from being set.')
                            else:
                                self.na_instr.set_text('This drive is not a TCG drive.')
                    elif curr_idx not in self.nonsetup_list:
                        if curr_idx in self.tcg_list:
                            self.na_instr.set_text('This drive has already been set up.')
                        elif self.blockSID_list[curr_idx] == 'Enabled':
                            self.na_instr.set_text('This drive cannot be set up because BlockSID is enabled.\nBlockSID prevents the password from being set.')
                        else:
                            self.na_instr.set_text('This drive is not a TCG drive.')
                        self.op_instr.hide()
                        self.na_instr.show()
                        self.disable_entries_buttons()
                    else:
                        self.enable_entries_buttons()
                    
            else:
                self.naDevices_instr.show()
            
        def setupUser_prompt(self, *args):
            verify.licCheck(self)
            self.hideAll()
            self.select_box.show()
            self.box_dev.show()
            if self.VERSION == 2 or self.PBA_VERSION == 1:
                self.userSetup_box.hide()
            self.cancel_button.show()
            self.op_label.set_text('Set Up User Password')
            self.op_instr.set_text('Setting up a user password creates a second password with limited authority.\nUser password can be used to unlock the drive and access the audit log.')
            
            curr_idx = self.dev_select.get_active()
            
            self.op_prompt = 6
            
            self.mode_setupUSB = False
            
            if self.view_state != 2:
                self.view_state = 2
            if len(self.devs_list) > 1:
                self.select_instr.show()
                
            if len(self.devs_list) > 0:
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
                self.drive_refresh.hide()
                #self.usb_menu.hide()
                    
                
                
                if curr_idx in self.setup_list and self.setupuser_list[curr_idx] != 'Yes':
                    self.op_instr.show()
                    self.na_instr.hide()
                    if self.VERSION != 0 and self.PBA_VERSION != 0:
                        self.enable_entries_buttons()
                    else:
                        self.disable_entries_buttons()
                else:
                    if self.setupuser_list[curr_idx] == 'Yes':
                        self.na_instr.set_text('User has already been set up for this drive.')
                    elif curr_idx in self.tcg_list:
                        self.na_instr.set_text('This drive has not been set up.')
                    else:
                        self.na_instr.set_text('This drive is not a TCG drive.')
                    self.na_instr.show()
                    self.disable_entries_buttons()
                
            else:
                self.naDevices_instr.show()
                
        def removeUser_prompt(self, *args):
            verify.licCheck(self)
            self.hideAll()
            self.select_box.show()
            self.box_dev.show()
            if self.VERSION == 2 or self.PBA_VERSION == 1:
                self.userSetup_box.hide()
            self.cancel_button.show()
            self.op_label.set_text('Remove User')
            self.op_instr.set_text('Disable the limited authority user account.')
            
            curr_idx = self.dev_select.get_active()
            
            self.op_prompt = 12
            
            self.mode_setupUSB = False
            
            if self.view_state != 2:
                self.view_state = 2
            if len(self.devs_list) > 1:
                self.select_instr.show()
                
            if len(self.devs_list) > 0:
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
                self.drive_refresh.hide()
                #self.usb_menu.hide()
                
                if curr_idx in self.setup_list and (self.setupuser_list[curr_idx] == 'Yes' or self.setupuser_list[curr_idx] == None or self.setupuser_list[curr_idx] == 'Unknown'):
                    self.op_instr.show()
                    self.na_instr.hide()
                    if self.VERSION != 0 and self.PBA_VERSION != 0:
                        self.enable_entries_buttons()
                    else:
                        self.disable_entries_buttons()
                else:
                    if self.setupuser_list[curr_idx] != 'Yes':
                        self.na_instr.set_text('User is not set up for this drive.')
                    elif curr_idx in self.tcg_list:
                        self.na_instr.set_text('This drive has not been set up.')
                    else:
                        self.na_instr.set_text('This drive is not a TCG drive.')
                    self.na_instr.show()
                    self.disable_entries_buttons()
                
            else:
                self.naDevices_instr.show()
            
        def updatePBA_prompt(self, *args):
            verify.licCheck(self)
            self.hideAll()
            self.select_box.show()
            self.box_dev.show()
            if self.VERSION == 2 or self.PBA_VERSION == 1:
                self.userSetup_box.hide()
            self.cancel_button.show()
            
            self.op_prompt = 3
            
            self.mode_setupUSB = False
            
            curr_idx = self.dev_select.get_active()
            
            if self.view_state != 7:
                self.view_state = 7
            if len(self.devs_list) > 1 and len(self.mbr_setup_list) <= 1:
                self.select_instr.show()
            elif len(self.mbr_setup_list) > 1:
                self.selectMulti_instr.show()
            
            if self.VERSION % 3 == 0:
                self.toggleSingle_radio.show()
                self.toggleMulti_radio.show()
                if len(self.mbr_setup_list) > 1:
                    if self.toggleSingle_radio.get_active():
                        self.toggleMulti_radio.set_active(True)
                    else:
                        self.mode_toggled(None)
                else:
                    if self.toggleMulti_radio.get_active():
                        self.toggleSingle_radio.set_active(True)
                    else:
                        self.mode_toggled(None)
            
            self.op_label.set_text('Update Preboot Image')
            self.op_instr.set_text('Use this to rewrite the Preboot Image or write the image to a set up drive.\nEnter the drive\'s password and press \'Update\'.')
            
            
                
            if len(self.devs_list) > 0:
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
                self.drive_refresh.hide()
                #self.usb_menu.hide()
                self.check_box_pass.show()
                    
                if len(self.mbr_setup_list) <= 1:
                    if curr_idx in self.mbr_setup_list:
                        self.op_instr.show()
                        self.na_instr.hide()
                        if self.VERSION != 0 and self.PBA_VERSION != 0:
                            self.enable_entries_buttons()
                        else:
                            self.disable_entries_buttons()
                    else:
                        if curr_idx not in self.mbr_list and curr_idx in self.tcg_list:
                            self.na_instr.set_text('Preboot image is not supported on this drive.')
                        elif curr_idx in self.mbr_list and curr_idx in self.nonsetup_list:
                            self.na_instr.set_text('This drive has not been set up.')
                        else:
                            self.na_instr.set_text('This drive is not a TCG drive.')
                        self.op_instr.hide()
                        self.na_instr.show()
                        self.disable_entries_buttons()
                else:
                    self.op_instr.show()
                    self.na_instr.hide()
                    if self.VERSION % 3 == 0:
                        if self.VERSION != 0 and self.PBA_VERSION != 0:
                            self.enable_entries_buttons()
                        else:
                            self.disable_entries_buttons()
                        if curr_idx not in self.mbr_setup_list:
                            if curr_idx not in self.mbr_list and curr_idx in self.tcg_list:
                                self.na_instr.set_text('Preboot image is not supported on this drive.')
                            elif curr_idx in self.mbr_list and curr_idx in self.nonsetup_list:
                                self.na_instr.set_text('This drive has not been set up.')
                            else:
                                self.na_instr.set_text('This drive is not a TCG drive.')
                    elif curr_idx not in self.mbr_setup_list:
                        if curr_idx not in self.mbr_list and curr_idx in self.tcg_list:
                            self.na_instr.set_text('Preboot image is not supported on this drive.')
                        elif curr_idx in self.mbr_list and curr_idx in self.nonsetup_list:
                            self.na_instr.set_text('This drive has not been set up.')
                        else:
                            self.na_instr.set_text('This drive is not a TCG drive.')
                        self.op_instr.hide()
                        self.na_instr.show()
                        self.disable_entries_buttons()
                    else:
                        self.enable_entries_buttons()
            else:
                self.naDevices_instr.show()

        def changePW_prompt(self, *args):
            verify.licCheck(self)
            self.hideAll()
            self.select_box.show()
            self.box_dev.show()
            if self.VERSION == 2 or self.PBA_VERSION == 1:
                self.userSetup_box.hide()
            self.cancel_button.show()
            
            self.op_prompt = 4
            
            self.mode_setupUSB = False
            
            curr_idx = self.dev_select.get_active()
            
            if self.view_state != 2:
                self.view_state = 2
            
            if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
                self.toggleSingle_radio.show()
                self.toggleMulti_radio.show()
                if len(self.setup_list) > 1:
                    if self.toggleSingle_radio.get_active():
                        self.toggleMulti_radio.set_active(True)
                    else:
                        self.mode_toggled(None)
                else:
                    if self.toggleMulti_radio.get_active():
                        self.toggleSingle_radio.set_active(True)
                    else:
                        self.mode_toggled(None)
            
            self.op_label.set_text('Change Password')
            self.op_instr.set_text('To change the password of the selected drive, enter the drive\'s current password\nand the new password.')
            
            
            
            if len(self.devs_list) > 1 and len(self.setup_list) <= 1:
                self.select_instr.show()
            elif len(self.setup_list) > 1:
                self.selectMulti_instr.show()
                
            if len(self.devs_list) > 0:
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
                self.drive_refresh.hide()
                self.check_box_pass.show()
                
                #self.usb_menu.hide()
                
                
                    
                if len(self.setup_list) <= 1:
                    if curr_idx in self.setup_list:
                        self.op_instr.show()
                        self.na_instr.hide()
                        if self.VERSION != 0 and self.PBA_VERSION != 0:
                            self.enable_entries_buttons()
                        else:
                            self.disable_entries_buttons()
                    else:
                        if curr_idx in self.tcg_list:
                            self.na_instr.set_text('This drive has not been set up.')
                        else:
                            self.na_instr.set_text('This drive is not a TCG drive.')
                        self.na_instr.show()
                        self.op_instr.hide()
                        self.disable_entries_buttons()
                else:
                    self.op_instr.show()
                    self.na_instr.hide()
                    if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
                        if self.VERSION != 0 and self.PBA_VERSION != 0:
                            self.enable_entries_buttons()
                        else:
                            self.disable_entries_buttons()
                        if curr_idx not in self.setup_list:
                            if curr_idx in self.tcg_list:
                                self.na_instr.set_text('This drive has not been set up.')
                            else:
                                self.na_instr.set_text('This drive is not a TCG drive.')
                    elif curr_idx not in self.setup_list:
                        if curr_idx in self.tcg_list:
                            self.na_instr.set_text('This drive has not been set up.')
                        else:
                            self.na_instr.set_text('This drive is not a TCG drive.')
                        self.na_instr.show()
                        self.op_instr.hide()
                        self.disable_entries_buttons()
                    else:
                        self.enable_entries_buttons()
            else:
                self.naDevices_instr.show()
                
        def setupUSB_prompt(self, *args):
            verify.licCheck(self)
            self.hideAll()
            self.select_box.show()
            self.box_dev.show()
            if self.VERSION == 2 or self.PBA_VERSION == 1:
                self.userSetup_box.hide()
            self.cancel_button.show()
            self.op_label.set_text('Set up USB')
            self.op_instr.set_text('This will write the bootable image to a USB drive.\nYou can then use the USB drive to unlock the selected drive.\nWARNING: Setting up the USB will erase its contents, use an empty USB.')
            
            self.op_prompt = 5
            
            curr_idx = self.dev_select.get_active()
            
            self.mode_setupUSB = True
            
            if self.view_state != 2:
                self.view_state = 2
            if len(self.devs_list) > 0:
                if len(self.devs_list) > 1:
                    self.select_instr.show()
                    
                self.setupUSB_button.show()
                
                self.box_newpass.show()
                self.new_pass_label.hide()
                self.new_pass_entry.hide()
                
                if curr_idx in self.setup_list:
                    self.op_instr.show()
                    self.na_instr.hide()
                    if self.VERSION != 0 and self.PBA_VERSION != 0:
                        self.enable_entries_buttons()
                    else:
                        self.disable_entries_buttons()
                else:
                    if curr_idx in self.tcg_list:
                        self.na_instr.set_text('This drive has not been set up.')
                    else:
                        self.na_instr.set_text('This drive is not a TCG drive.')
                    self.na_instr.show()
                    self.disable_entries_buttons()
            else:
                self.naDevices_instr.show()
                self.select_instr.hide()
            
        def revert_erase_prompt(self, *args):
            verify.licCheck(self)
            self.hideAll()
            self.select_box.show()
            self.box_dev.show()
            if self.VERSION == 2 or self.PBA_VERSION == 1:
                self.userSetup_box.hide()
            self.op_prompt = 10
            
            curr_idx = self.dev_select.get_active()
            
            if self.view_state != 5:
                self.view_state = 5
                
            self.mode_setupUSB = False
            
            if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
                self.toggleSingle_radio.show()
                self.toggleMulti_radio.show()
                if len(self.tcg_list) > 1:
                    if self.toggleSingle_radio.get_active():
                        self.toggleMulti_radio.set_active(True)
                    else:
                        self.mode_toggled(None)
                else:
                    if self.toggleMulti_radio.get_active():
                        self.toggleSingle_radio.set_active(True)
                    else:
                        self.mode_toggled(None)
            
            self.op_label.set_text('Remove Lock and Erase Data')
            self.op_instr.set_text('Revert with Password reverts the drive\'s LockingSP.\nThis resets the drive\'s password and disables locking.\nEnter the drive\'s password and choose whether or not to erase all data.')
            self.cancel_button.show()
            
            if len(self.devs_list) > 1 and len(self.tcg_list) <= 1:
                self.select_instr.show()
            elif len(self.tcg_list) > 1:
                self.selectMulti_instr.show()
                
            if len(self.devs_list) > 0:
                if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
                    self.pass_label.set_text('Enter Admin Password')
                self.box_pass.show()
                #if self.VERSION == 1 and self.PBA_VERSION == 1:
                #    self.check_pass_rd.hide()
                self.box_revert_agree.show()
                self.pass_entry.set_activates_default(True)
                self.pass_entry.grab_focus()
                
                self.revertUser_button.show()
                self.set_default(self.revertUser_button)
                
                self.box_drive.show()
                self.drive_label.hide()
                self.drive_menu.hide()
                self.drive_refresh.hide()
                #self.usb_menu.hide()
                self.check_box_pass.show()
                
                if len(self.tcg_list) <= 1:
                    if curr_idx in self.tcg_list:
                        self.op_instr.show()
                        self.na_instr.hide()
                        if self.VERSION != 0 and self.PBA_VERSION != 0:
                            self.enable_entries_buttons()
                        else:
                            self.disable_entries_buttons()
                    else:
                        self.na_instr.set_text('This drive is not a TCG drive.')
                        self.na_instr.show()
                        self.disable_entries_buttons()
                else:
                    self.op_instr.show()
                    self.na_instr.hide()
                    if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
                        if self.VERSION != 0 and self.PBA_VERSION != 0:
                            self.enable_entries_buttons()
                        else:
                            self.disable_entries_buttons()
                        if curr_idx not in self.tcg_list:
                            self.na_instr.set_text('This drive is not a TCG drive.')
                    elif curr_idx not in self.tcg_list:
                        self.na_instr.set_text('This drive is not a TCG drive.')
                        self.op_instr.hide()
                        self.na_instr.show()
                        self.disable_entries_buttons()
                    else:
                        self.enable_entries_buttons()
            else:
                self.naDevices_instr.show()
            
        def revert_psid_prompt(self, *args):
            verify.licCheck(self)
            self.hideAll()
            self.select_box.show()
            self.box_dev.show()
            if self.VERSION == 2 or self.PBA_VERSION == 1:
                self.userSetup_box.hide()
            self.cancel_button.show()
            self.op_label.set_text('Remove Lock and Erase Data with PSID')
            self.op_instr.set_text('Reverting with PSID reverts the drive to manufacturer settings and erases all data.\nEnter the drive\'s PSID and press \'Remove Lock with PSID\'.')
            
            self.op_prompt = 11
            
            self.mode_setupUSB = False
            
            curr_idx = self.dev_select.get_active()
            
            if self.view_state != 5:
                self.view_state = 5
            if len(self.devs_list) > 1:
                self.select_instr.show()
                
            if len(self.devs_list) > 0:
                self.box_psid.show()
                self.box_revert_agree.show()
                self.revertPSID_button.show()
                self.revert_psid_entry.set_text("")
                if curr_idx in self.tcg_list:
                    self.op_instr.show()
                    self.na_instr.hide()
                    if self.VERSION != 0 and self.PBA_VERSION != 0:
                        self.enable_entries_buttons()
                    else:
                        self.disable_entries_buttons()
                else:
                    self.na_instr.set_text('This drive is not a TCG drive.')
                    self.na_instr.show()
                    self.disable_entries_buttons()
            else:
                self.naDevices_instr.show()
            
        def unlock_prompt(self, *args):
            verify.licCheck(self)
            self.hideAll()
            self.select_box.show()
            self.box_dev.show()
            if self.VERSION == 2 or self.PBA_VERSION == 1:
                self.userSetup_box.hide()
            self.op_prompt = 7
            
            curr_idx = self.dev_select.get_active()
            
            if self.view_state != 1:
                self.view_state = 1
                
            self.mode_setupUSB = False
            
            if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
                self.toggleSingle_radio.show()
                self.toggleMulti_radio.show()
                if len(self.locked_list) > 1:
                    if self.toggleSingle_radio.get_active():
                        self.toggleMulti_radio.set_active(True)
                    else:
                        self.mode_toggled(None)
                else:
                    if self.toggleMulti_radio.get_active():
                        self.toggleSingle_radio.set_active(True)
                    else:
                        self.mode_toggled(None)
            
            self.op_label.set_text('Preboot Unlock')
            self.op_instr.set_text('Preboot Unlock unlocks a drive for bootup.\nEnter the drive\'s password and press \'Preboot Unlock\'\nAfterwards, reboot into the unlocked drive.')
            self.cancel_button.show()
            
            
                
            if len(self.devs_list) > 1 and len(self.locked_list) <= 1:
                self.select_instr.show()
            elif len(self.locked_list) > 1:
                self.selectMulti_instr.show()
                
            if len(self.devs_list) > 0:
                self.box_pass.show()
                    
                self.pass_entry.set_activates_default(True)
                self.pass_entry.grab_focus()
                
                #if self.VERSION == 1 and self.PBA_VERSION == 1:
                #    self.check_pass_rd.hide()
                if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
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
                self.drive_refresh.hide()
                #self.usb_menu.hide()
                self.check_box_pass.show()
                
                if len(self.locked_list) <= 1:
                    if curr_idx in self.locked_list:
                        self.op_instr.show()
                        self.na_instr.hide()
                        if self.VERSION != 0 and self.PBA_VERSION != 0:
                            self.enable_entries_buttons()
                        else:
                            self.disable_entries_buttons()
                    else:
                        if curr_idx in self.tcg_list:
                            self.na_instr.set_text('This drive is not locked.')
                        else:
                            self.na_instr.set_text('This drive is not a TCG drive.')
                        self.na_instr.show()
                        self.disable_entries_buttons()
                else:
                    self.op_instr.show()
                    self.na_instr.hide()
                    if self.VERSION != 0 and self.PBA_VERSION != 0:
                        self.enable_entries_buttons()
                    else:
                        self.disable_entries_buttons()
                    if curr_idx not in self.setup_list:
                        if curr_idx in self.tcg_list:
                            self.na_instr.set_text('This drive is not locked.')
                        else:
                            self.na_instr.set_text('This drive is not a TCG drive.')
            else:
                self.naDevices_instr.show()
                    
        def revert_keep_prompt(self, *args):
            verify.licCheck(self)
            self.hideAll()
            self.select_box.show()
            self.box_dev.show()
            if self.VERSION == 2 or self.PBA_VERSION == 1:
                self.userSetup_box.hide()
            self.op_prompt = 9
            
            self.mode_setupUSB = False
            
            curr_idx = self.dev_select.get_active()
            
            if self.view_state != 5:
                self.view_state = 5
            
            if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
                self.toggleSingle_radio.show()
                self.toggleMulti_radio.show()
                if len(self.tcg_list) > 1:
                    if self.toggleSingle_radio.get_active():
                        self.toggleMulti_radio.set_active(True)
                    else:
                        self.mode_toggled(None)
                else:
                    if self.toggleMulti_radio.get_active():
                        self.toggleSingle_radio.set_active(True)
                    else:
                        self.mode_toggled(None)
            
            self.op_label.set_text('Remove Lock and Keep Data')
            self.op_instr.set_text('Reverting a drive disables locking and resets the drive password.\nEnter the password and press \'Remove Lock\'.')
            self.cancel_button.show()
            
            

            if len(self.devs_list) > 1 and len(self.tcg_list) <= 1:
                self.select_instr.show()
            elif len(self.tcg_list) > 1:
                self.selectMulti_instr.show()
                
            if len(self.devs_list) > 0:
                if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
                    self.pass_label.set_text('Enter Admin Password')
                self.box_pass.show()
                
                #if self.VERSION == 1 and self.PBA_VERSION == 1:
                #    self.check_pass_rd.hide()
                
                self.pass_entry.set_activates_default(True)
                self.pass_entry.grab_focus()
                
                self.revertOnly_button.show()
                self.set_default(self.revertOnly_button)
                
                self.box_drive.show()
                self.drive_label.hide()
                self.drive_menu.hide()
                self.drive_refresh.hide()
                #self.usb_menu.hide()
                self.check_box_pass.show()
                if len(self.tcg_list) <= 1:
                    if curr_idx in self.tcg_list:
                        self.op_instr.show()
                        self.na_instr.hide()
                        if self.VERSION != 0 and self.PBA_VERSION != 0:
                            self.enable_entries_buttons()
                        else:
                            self.disable_entries_buttons()
                    else:
                        self.na_instr.set_text('This drive is not a TCG drive.')
                        self.na_instr.show()
                        self.disable_entries_buttons()
                else:
                    self.op_instr.show()
                    self.na_instr.hide()
                    if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
                        if self.VERSION != 0 and self.PBA_VERSION != 0:
                            self.enable_entries_buttons()
                        else:
                            self.disable_entries_buttons()
                        if curr_idx not in self.tcg_list:
                            self.na_instr.set_text('This drive is not a TCG drive.')
                    elif curr_idx not in self.tcg_list:
                        self.na_instr.set_text('This drive is not a TCG drive.')
                        self.op_instr.hide()
                        self.na_instr.show()
                        self.disable_entries_buttons()
                    else:
                        self.enable_entries_buttons()
            else:
                self.naDevices_instr.show()
                
        def hideAll(self, *args):
            verify.licCheck(self)
            self.select_box.hide()
            self.box_dev.hide()
            
            self.scrolledWin_grid.hide()
            
            self.toggleMulti_radio.hide()
            self.toggleSingle_radio.hide()
        
            self.select_instr.hide()
            self.selectMulti_instr.hide()
            self.naDevices_instr.hide()
            self.main_instr.hide()
            self.noTCG_instr.hide()
            self.wait_instr.hide()
            self.multi_wait_instr.hide()
            self.pba_wait_instr.hide()
            self.setup_wait_instr.hide()
            self.op_instr.hide()
            self.na_instr.hide()
            self.naUSB_instr.hide()
            
            self.progress_bar.hide()
        
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

            self.pbaUnlockReboot.hide()
            self.pbaUnlockOnly.hide()
            self.updatePBA_button.hide()
            self.setupUSB_button.hide()
            self.setupUserPW.hide()
            self.removeUser_button.hide()
            
            self.revertOnly_button.hide()
            self.viewLog.hide()
            self.pbaUSB_button.hide()
            
            self.box_newpass.hide()
            self.box_newpass_confirm.hide()
            
            self.pass_label.set_text('Enter Password')
            self.new_pass_label.set_text('Enter New Password')
            self.confirm_pass_label.set_text('Confirm New Password')
            
            self.mbr_radio.hide()
            #self.usb_radio.hide()
            self.skip_radio.hide()
            
            self.box_drive.hide()
            self.box_auth.hide()
            self.auth_menu.set_active(0)
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
            self.selectAll_check.set_inconsistent(False)
            self.selectAll_check.set_active(False)
            
            self.sel_list = []
            
            self.pass_entry.set_activates_default(False)
            self.set_default(None)
            
            self.warned = False
            self.orig = ''
            
            self.mode_setupUSB = False
            
        def start_spin(self, *args):
            self.op_instr.hide()
            self.disable_menu()
            self.waitSpin.show()
            self.waitSpin.start()
            
        def stop_spin(self, *args):
            self.enable_menu()
            self.waitSpin.stop()
            self.waitSpin.hide()
            if self.op_prompt != 0:
                self.op_instr.show()
            self.pba_wait_instr.hide()
            self.setup_wait_instr.hide()
            self.wait_instr.hide()
            self.multi_wait_instr.hide()
            
        def mode_toggled(self, button):
            verify.licCheck(self)
            if self.toggleSingle_radio.get_active():
                self.scrolledWin_grid.hide()
                self.select_box.show()
                self.box_dev.show()
                if self.VERSION == 2 or self.PBA_VERSION == 1:
                    self.userSetup_box.hide()
                self.naDevices_instr.hide()
                self.op_instr.hide()
                self.selectMulti_instr.hide()
                
                self.selectAll_check.hide()
                if len(self.devs_list) > 1:
                    self.select_instr.show()
                #idx = self.dev_select.get_active()
                #if (self.view_state == 1 and idx in self.locked_list) or (self.view_state == 4 and idx in self.nonsetup_list) or (self.view_state == 2 and idx in self.setup_list) or (self.view_state == 5 and idx in self.tcg_list):
                #    if self.VERSION != 0 and self.PBA_VERSION != 0:
                #        self.enable_entries_buttons()
                #    self.op_instr.show()
                #    self.na_instr.hide()
                #else:
                #    self.disable_entries_buttons()
                #    self.na_instr.show()
                #if self.op_prompt == 4 or self.op_prompt == 7:
                #    i = self.dev_select.get_active()
                #    if self.setupuser_list[i] == 'Yes' or self.setupuser_list[i] == None:
                #        self.box_auth.show()
                #    else:
                #        self.box_auth.hide()
                #        self.auth_menu.set_active(0)
                self.changed_cb(self.dev_select)
            else:
                self.na_instr.hide()
                self.op_instr.hide()
                self.select_instr.hide()
                self.box_dev.hide()
                
                self.select_box.hide()
                self.scrolledWin_grid.show()
                self.liststore.clear()
                self.selectAll_check.set_inconsistent(False)
                self.selectAll_check.set_active(False)
                self.selectAll_check.show()
                
                if self.view_state == 1 and self.auth_menu.get_active() == 0:# and len(self.locked_list) > 0:
                    #for i in self.locked_list:
                    for i in range(len(self.devs_list)):
                        if i in self.locked_list:
                            self.liststore.append([False, self.devs_list[i], self.vendor_list[i], self.sn_list[i], True])
                        else:
                            self.liststore.append([False, self.devs_list[i], self.vendor_list[i], self.sn_list[i], False])
                    if len(self.locked_list) > 0:
                        if self.VERSION != 0 and self.PBA_VERSION != 0:
                            self.enable_entries_buttons()
                        else:
                            self.disable_entries_buttons()
                        self.op_instr.show()
                        self.na_instr.hide()
                        self.selectMulti_instr.show()
                    else:
                        self.disable_entries_buttons()
                        self.selectMulti_instr.hide()
                        self.naDevices_instr.show()
                elif self.view_state == 1 and self.auth_menu.get_active() == 1:# and len(self.ulocked_list) > 0:
                    #for i in self.ulocked_list:
                    for i in range(len(self.devs_list)):
                        if i in self.ulocked_list:
                            self.liststore.append([False, self.devs_list[i], self.vendor_list[i], self.sn_list[i], True])
                        else:
                            self.liststore.append([False, self.devs_list[i], self.vendor_list[i], self.sn_list[i], False])
                    if len(self.ulocked_list) > 0:
                        if self.VERSION != 0 and self.PBA_VERSION != 0:
                            self.enable_entries_buttons()
                        else:
                            self.disable_entries_buttons()
                        self.op_instr.show()
                        self.na_instr.hide()
                        self.selectMulti_instr.show()
                    else:
                        self.disable_entries_buttons()
                        self.selectMulti_instr.hide()
                        self.naDevices_instr.show()
                elif self.view_state == 2 and self.auth_menu.get_active() == 0:# and len(self.setup_list) > 0:
                    #for i in self.setup_list:
                    for i in range(len(self.devs_list)):
                        if i in self.setup_list:
                            self.liststore.append([False, self.devs_list[i], self.vendor_list[i], self.sn_list[i], True])
                        else:
                            self.liststore.append([False, self.devs_list[i], self.vendor_list[i], self.sn_list[i], False])
                    if len(self.setup_list) > 0:
                        if self.VERSION != 0 and self.PBA_VERSION != 0:
                            self.enable_entries_buttons()
                        else:
                            self.disable_entries_buttons()
                        self.op_instr.show()
                        self.na_instr.hide()
                        self.selectMulti_instr.show()
                    else:
                        self.disable_entries_buttons()
                        self.selectMulti_instr.hide()
                        self.naDevices_instr.show()
                elif self.view_state == 2 and self.auth_menu.get_active() == 1:# and len(self.usetup_list) > 0:
                    #for i in self.usetup_list:
                    for i in range(len(self.devs_list)):
                        if i in self.usetup_list:
                            self.liststore.append([False, self.devs_list[i], self.vendor_list[i], self.sn_list[i], True])
                        else:
                            self.liststore.append([False, self.devs_list[i], self.vendor_list[i], self.sn_list[i], False])
                    if len(self.usetup_list) > 0:
                        if self.VERSION != 0 and self.PBA_VERSION != 0:
                            self.enable_entries_buttons()
                        else:
                            self.disable_entries_buttons()
                        self.op_instr.show()
                        self.na_instr.hide()
                        self.selectMulti_instr.show()
                    else:
                        self.disable_entries_buttons()
                        self.selectMulti_instr.hide()
                        self.naDevices_instr.show()
                elif self.view_state == 4:# and len(self.nonsetup_list) > 0:
                    #for i in self.nonsetup_list:
                    for i in range(len(self.devs_list)):
                        if i in self.nonsetup_list:
                            self.liststore.append([False, self.devs_list[i], self.vendor_list[i], self.sn_list[i], True])
                        else:
                            self.liststore.append([False, self.devs_list[i], self.vendor_list[i], self.sn_list[i], False])
                    if len(self.nonsetup_list) > 0:
                        if self.VERSION != 0 and self.PBA_VERSION != 0:# and (self.VERSION != 3 or len(self.usb_list) != 0):
                            self.enable_entries_buttons()
                            self.op_instr.show()
                            self.na_instr.hide()
                        else:
                            self.disable_entries_buttons()
                            self.na_instr.show()
                        self.selectMulti_instr.show()
                    else:
                        self.disable_entries_buttons()
                        self.selectMulti_instr.hide()
                        self.naDevices_instr.show()
                elif self.view_state == 5:# and len(self.tcg_list) > 0:
                    #for i in self.tcg_list:
                    for i in range(len(self.devs_list)):
                        if i in self.tcg_list:
                            self.liststore.append([False, self.devs_list[i], self.vendor_list[i], self.sn_list[i], True])
                        else:
                            self.liststore.append([False, self.devs_list[i], self.vendor_list[i], self.sn_list[i], False])
                    if len(self.tcg_list) > 0:
                        if self.VERSION != 0 and self.PBA_VERSION != 0:
                            self.enable_entries_buttons()
                        else:
                            self.disable_entries_buttons()
                        self.op_instr.show()
                        self.na_instr.hide()
                        self.selectMulti_instr.show()
                    else:
                        self.disable_entries_buttons()
                        self.selectMulti_instr.hide()
                        self.naDevices_instr.show()
                elif self.view_state == 7:# and len(self.mbr_setup_list) > 0:
                    #for i in self.mbr_setup_list:
                    for i in range(len(self.devs_list)):
                        if i in self.mbr_setup_list:
                            self.liststore.append([False, self.devs_list[i], self.vendor_list[i], self.sn_list[i], True])
                        else:
                            self.liststore.append([False, self.devs_list[i], self.vendor_list[i], self.sn_list[i], False])
                    if len(self.mbr_setup_list) > 0:
                        if self.VERSION != 0 and self.PBA_VERSION != 0:
                            self.enable_entries_buttons()
                        else:
                            self.disable_entries_buttons()
                        self.op_instr.show()
                        self.na_instr.hide()
                        self.selectMulti_instr.show()
                    else:
                        self.disable_entries_buttons()
                        self.selectMulti_instr.hide()
                        self.naDevices_instr.show()
                #elif self.view_state != 0:
                #    self.disable_entries_buttons()
                #    self.selectMulti_instr.hide()
                #    self.naDevices_instr.show()
                if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
                    if self.op_prompt == 4 or self.op_prompt == 7:
                        if 'Yes' in self.setupuser_list:
                            self.box_auth.show()
                        else:
                            self.box_auth.hide()
                            self.auth_menu.set_active(0)
                selection = self.treeview.get_selection()
                selection.set_select_function(
                    # Row selectable only if sensitive
                    lambda path: self.liststore[path][4]
                )
                selection.set_mode(gtk.SELECTION_MULTIPLE)
                
        def hideUSB(self, *args):
            self.box_drive.hide()
            
        def disable_menu(self, *args):
            self.navM.set_sensitive(False)
            self.devM.set_sensitive(False)
            self.revertM.set_sensitive(False)
            self.helpM.set_sensitive(False)
            self.unlockM.set_sensitive(False)
            #self.select_box.set_sensitive(False)
            self.dev_select.set_sensitive(False)
            #self.treeview.grab_focus()
            #self.treeview.emit('toggle-cursor-row')
            self.scrolledWin_grid.set_sensitive(False)
            self.selectAll_check.set_sensitive(False)
            if self.VERSION != 1:
                self.setupM.set_sensitive(False)
                if self.VERSION != 3 or self.MAX_DEV != sys.maxint:
                    self.upgradeM.set_sensitive(False)
                    
            self.toggleMulti_radio.set_sensitive(False)
            self.toggleSingle_radio.set_sensitive(False)
            
            self.disable_entries_buttons()
            
            self.dev_vendor.set_sensitive(False)
            self.dev_sn.set_sensitive(False)
            self.dev_msid.set_sensitive(False)
            self.dev_series.set_sensitive(False)
            self.dev_pbaVer.set_sensitive(False)
            
            self.dev_opal_ver.set_sensitive(False)
            self.dev_status.set_sensitive(False)
            self.dev_setup.set_sensitive(False)
            self.dev_enc.set_sensitive(False)
            self.dev_blockSID.set_sensitive(False)
            self.dev_userSetup.set_sensitive(False)
            
        def disable_entries_buttons(self, *args):
            
            self.buttonBox.set_sensitive(False)
            
            self.mbr_radio.set_sensitive(False)
            self.skip_radio.set_sensitive(False)
            
            self.check_box_pass.set_active(False)
            self.check_box_pass.set_sensitive(False)
            
            #self.box_pass.set_sensitive(False)
            self.pass_entry.set_sensitive(False)
            if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
                self.check_pass_rd.set_sensitive(False)
                self.pass_sav.set_sensitive(False)
            #self.box_newpass.set_sensitive(False)
            self.new_pass_entry.set_sensitive(False)
            
            #self.box_newpass_confirm.set_sensitive(False)
            self.confirm_pass_entry.set_sensitive(False)
            #self.box_drive.set_sensitive(False)
            self.drive_menu.set_sensitive(False)
            self.drive_refresh.set_sensitive(False)
            #self.box_auth.set_sensitive(False)
            self.auth_menu.set_sensitive(False)
            #self.setup_next.set_sensitive(False)
            #self.box_revert_agree.set_sensitive(False)
            self.revert_agree_entry.set_sensitive(False)
            #self.box_psid.set_sensitive(False)
            self.revert_psid_entry.set_sensitive(False)
            
        def enable_menu(self, *args):
            self.navM.set_sensitive(True)
            self.devM.set_sensitive(True)
            self.revertM.set_sensitive(True)
            self.helpM.set_sensitive(True)
            self.unlockM.set_sensitive(True)
            
            self.dev_select.set_sensitive(True)
            self.scrolledWin_grid.set_sensitive(True)
            self.selectAll_check.set_sensitive(True)
            if self.VERSION != 1:
                self.setupM.set_sensitive(True)
                if self.VERSION != 3 or self.MAX_DEV != sys.maxint:
                    self.upgradeM.set_sensitive(True)
                    
            self.toggleMulti_radio.set_sensitive(True)
            self.toggleSingle_radio.set_sensitive(True)
            
            if self.VERSION != 0 and self.PBA_VERSION != 0:
                self.enable_entries_buttons()
                
            self.dev_vendor.set_sensitive(True)
            self.dev_sn.set_sensitive(True)
            self.dev_msid.set_sensitive(True)
            self.dev_series.set_sensitive(True)
            self.dev_pbaVer.set_sensitive(True)
            
            self.dev_opal_ver.set_sensitive(True)
            self.dev_status.set_sensitive(True)
            self.dev_setup.set_sensitive(True)
            self.dev_enc.set_sensitive(True)
            self.dev_blockSID.set_sensitive(True)
            self.dev_userSetup.set_sensitive(True)
            
            if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
                self.check_pass_rd.set_active(False)
                self.check_pass_rd.set_sensitive(True)
                self.pass_sav.set_active(False)
                self.pass_sav.set_sensitive(True)
                self.pass_entry.set_sensitive(True)
            
        def enable_entries_buttons(self, *args):
            
            self.buttonBox.set_sensitive(True)
            
            self.mbr_radio.set_sensitive(True)
            #self.usb_radio.set_sensitive(True)
            self.skip_radio.set_sensitive(True)
            
            if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
                if not self.check_pass_rd.get_active() or self.op_prompt == 5 or self.op_prompt == 6:
                    self.check_box_pass.set_sensitive(True)
            else:
                self.check_box_pass.set_sensitive(True)
            
            #self.box_pass.set_sensitive(True)
            self.pass_entry.set_sensitive(True)
            if self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1):
                self.check_pass_rd.set_sensitive(True)
                self.check_pass_rd.set_active(False)
                if self.op_prompt != 2:
                    self.pass_sav.set_sensitive(True)
                    self.pass_sav.set_active(False)
            #self.box_newpass.set_sensitive(True)
            self.new_pass_entry.set_sensitive(True)
            #if self.op_prompt != 2 and (self.VERSION % 3 == 0 or (self.VERSION == 1 and self.PBA_VERSION != 1)):
            #    if not self.check_pass_rd.get_active():
            #        self.pass_sav.set_sensitive(True)
            #self.box_newpass_confirm.set_sensitive(True)
            self.confirm_pass_entry.set_sensitive(True)
            #self.box_drive.set_sensitive(True)
            self.drive_menu.set_sensitive(True)
            self.drive_refresh.set_sensitive(True)
            #self.box_auth.set_sensitive(True)
            self.auth_menu.set_sensitive(True)
            #self.setup_next.set_sensitive(True)
            #self.box_revert_agree.set_sensitive(True)
            self.revert_agree_entry.set_sensitive(True)
            #self.box_psid.set_sensitive(True)
            self.revert_psid_entry.set_sensitive(True)
            
        def updateDevs(self, index, add, *args):
            if index in self.locked_list:
                self.locked_list.remove(index)
            if index in self.setup_list:
                self.setup_list.remove(index)
            if index in self.nonsetup_list:
                self.nonsetup_list.remove(index)
            for j in add:
                if j == 1:
                    self.locked_list.append(index)
                elif j == 2:
                    self.setup_list.append(index)
                elif j == 4:
                    self.nonsetup_list.append(index)
                    
            self.locked_list.sort()
            self.setup_list.sort()
            self.nonsetup_list.sort()
                            
        def run(self):
            ''' Run the app. '''
            
            gtk.main()
            #self.unhookWndProc()
            if self.shutdown_req:
                if self.ostype == 0:
                    subprocess.call(['shutdown', '-s', '-t', '2'])
                elif self.ostype == 1 :
                    os.system("sudo poweroff")
                elif self.ostype == 2 :
                    os.system("sudo poweroff") 
            elif self.reboot_req:
                if self.ostype == 0 :
                    subprocess.call(['shutdown', '-r', '-t', '2'])
                elif self.ostype == 1 :
                    os.system("sudo reboot now")
                elif self.ostype == 2 :
                    os.system("sudo reboot now")  
            
    runop.define_lock_t()
    app = LockApp()
    app.run()