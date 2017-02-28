'''
Description: 

   Fidelity Lock is a temparary name. Final name will be decided later on

History:
   2016.09.15 inital draft sample program use gtk python 2.7
   2016.09.17 add horizontal box for password.  how to get the horizontal box's entry info from vbox ?????
              if not, I need to duplicate the same code for each horizonal entry
   2016.10.14 from scan result to determine OS type and set device name accordingly
              add self.ostype
              add self.prefix
              power off according to self.ostype
              use platform to determine delf.ostype
              add button for pwroff, pba, revert with user password
              self.devname setting according to os type and scan returned data
              
   2016.10.18 preload self.devname just in case user change devname manually
              add write PBA image button, 
              add PBA image file browser dialog
              add warnning message box twice when perform revert psid and rever user wirh password
 
    2017.02.09 beef up option handling. show usage for invalid option and exit 
    2017.02.15 utility button handle Linux/Darwin/Win

'''


import gtk
import os
import re
import sys
from sys import platform
#import platform
import getopt
import hashlib
import gobject
import subprocess
import time
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
    psids = dict ({'samsung 250GB NVMe': '1MJHH9BV6EPCX7VHA9EQP1D4LBKDFUJ4', #DELL
                   'samsung 250GB SSD': '12S97B41W3TX9JTZBMVDR2BG7U0U504K', 
                   'crucial 120GB SSD': '53028B2F57814C8AE100000089C981B5',
                   'samsung 500GB SSD': '1FE96DSD0U6K5BY006ESCF2FEPE5MWUG', 
                   'samsung 250GB NVMe': '1EE2DS8EKUYVCNW7Q73004NX02C0QT7J', #NUC
                   'crucial 512GB SSD': '54B3F885AB284E2CE100000089C981B5'})

    psid = psids["crucial 512GB SSD"]

    if os.path.isfile("psids.txt"):
        psids = {}
        with open("psids.txt") as f:
            for line in f:
                (key, val) = line.strip().split(' : ', 1)
                psids[key] = val

    image = "LINUXRelease.img"

    LKRNG = "0" 
    LKRNG_SLBA = "0"
    LKRNG_LBALEN = "0"
    LKATTR = "RW"

    PASSWORD_ONLY = False        
        
    try:
        opts, args = getopt.getopt(sys.argv[1:], [""], ["Passwordonly"])
    except getopt.GetoptError, err:
        print "Use --Passwordonly or no option at all"
        exit(2)

    for o, a in opts:
        if o in ("--Passwordonly"):
            PASSWORD_ONLY = True
        
        else:
            assert False, "OK"
   
    devs_list = [] #[ "/dev/sda" , "/dev/sdb",  "/dev/sdc",  "/dev/sdd",  "/dev/sde" ]
    vendor_list = [] # "Sandisk" 
    opal_ver_list = [] # 1, 2, or 12
    series_list = [] # series number 
    force_true = True
    
    def __init__(self):
        # Initialize window
        gtk.Window.__init__(self)
        self.set_title('              Fidelity Lock Disk Drive Security Manager')
        height = 400
        width = 800
        if self.PASSWORD_ONLY == True:
            height = 300
            
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
        self.vbox = gtk.VBox(False,0)
        
        if self.PASSWORD_ONLY == True:
            txt = '\n\nThe system internal Disk, is protected by a password authentication system. You cannot access data on this disk drive without \nthe correct password. Please type in the disk drive password and click \"Start OS\" button. Click the Additional \"Utilities\" button for more options\n\n'            
           
            label_passwordonly = gtk.Label( txt )                              
            self.vbox.pack_start(label_passwordonly, False)
            vsep = gtk.VSeparator()
            vsep.show()
            # show device on top 
            self.scan_passwordonly()
            self.scan()
            # show only the password entry 
            self.passworddialog()
            self.get_msid()
            # "GO" split to 
            # "Preboot Unlock", "Restart OS"

            self.box_goreboot = gtk.HBox(True, spacing)

            self.go = gtk.Button('Preboot Unlock')
            self.box_goreboot.pack_start(self.go, True, True, padding)
            self.go.connect('clicked', self.pba)

            # RestartOS 
            self.RestartOS = gtk.Button('Restart OS')
            self.box_goreboot.pack_start(self.RestartOS, True, True, padding)
            self.RestartOS.connect('clicked', self.reboot)
            self.vbox.pack_start(self.box_goreboot, False)
            
         
            self.util_but = gtk.Button('Additional Utilities')
            self.vbox.pack_start(self.util_but, False)
            self.util_but.connect('clicked', self.util)

            self.add(self.vbox)
            return

        
        # The go button
        self.go_button_scan   = gtk.Button('_Rescan TCG device')
        self.go_button_query  = gtk.Button('_Query TCG device')
        self.go_button_exit_reboot   = gtk.Button('_Exit and reboot')
        self.go_button_exit   = gtk.Button('_Exit')
        self.go_button_exit_pwroff = gtk.Button('_Exit and power off')
        self.go_button_PBA    = gtk.Button('_Authentication with admin password')
        self.go_button_revert_psid = gtk.Button('_Revert with PSID(Data will be destroyed)')
        self.go_button_revert_user = gtk.Button('_Revert with admin password(Data will be destroyed)')


        self.go_button_unlock = gtk.Button('_Unlock TCG device')
        self.go_button_lock   = gtk.Button('_Activation and setup')
        self.go_button_PBA_write   = gtk.Button('_Write PBA image to the selected device shadow MBR')


        # Connect the go button's callback
        self.go_button_scan.connect('clicked', self.scan)
        self.go_button_query.connect('clicked', self.query)
        self.go_button_exit_reboot.connect('clicked', self.reboot)
        self.go_button_exit_pwroff.connect('clicked', self.pwroff)
        self.go_button_exit.connect('clicked', self.exitapp)    
        self.go_button_PBA.connect('clicked', self.pba)
        self.go_button_revert_psid.connect('clicked', self.revert_psid)
        self.go_button_revert_user.connect('clicked', self.revert_user)

       
        self.go_button_unlock.connect('clicked', self.unlock)
        self.go_button_lock.connect('clicked', self.lock)
        self.go_button_PBA_write.connect('clicked', self.pba_write)

        # Put the controls in the vertical layout
        #self.vbox.pack_start(self.greeting, False)



        #
        # Create password buttons, entry with the appropriate settings
        #
        self.passworddialog()
        
        ''' ******************************
        
        width = 12
        self.box_pass = gtk.HBox(homogeneous, spacing)
        self.label_pass = gtk.Label("Password")
        self.label_pass.set_width_chars(width)
        self.box_pass.pack_start(self.label_pass, expand, fill, padding)
        self.label_pass.show()

        self.txt_pass = gtk.Entry()
        self.txt_pass.set_text("password")
        self.txt_pass.set_visibility(False)
        ##gtk_entry_set_visibility(self.txt_pass,False)

        ##gtk_entry_set_invisible_char(self.txt_pass,'#')
        self.box_pass.pack_start(self.txt_pass, True, True, padding)
        #e = gtk.Entry()
        #map = e.get_colormap()
        #colour = map.alloc_color("red") # light red
        #style = e.get_style().copy()
        #style.bg[gtk.STATE_NORMAL] = colour
        #e.set_style(style)
        
        check_box = gtk.CheckButton("Show Password")
        check_box.connect("toggled", self.entry_checkbox, check_box)
        check_box.set_active(False)  # Set the defaut
        check_box.show()
        self.box_pass.pack_start(check_box, expand, fill, padding)
        check_box.show()
        
        # check box for hash
        check_box_hash = gtk.CheckButton("Hash password")
        check_box_hash.connect("toggled", self.entry_checkbox_hash, check_box_hash)
        check_box_hash.set_active(True)  # Set the defaut to hash
        check_box_hash.show()
        self.box_pass.pack_start(check_box_hash, expand, fill, padding)
        check_box_hash.show()
        '''
        
        
        #
        # Create devname buttons, entry with the appropriate settings
        #
        
        self.scan_passwordonly()
        
        ''' ******************************************************
        self.box_dev = gtk.HBox(homogeneous, spacing)
        self.label_dev = gtk.Label("Select Device") 
        self.label_dev.set_width_chars(width)
        self.label_dev.set_justify(gtk.JUSTIFY_LEFT)
        self.box_dev.pack_start(self.label_dev, expand, fill, padding)
        self.label_dev.show()

        self.findos() #
        
        self.txt_dev = gtk.Entry()
        self.txt_dev.set_text("sdz") # default non-exist dev, so it won't accidentally self-delete

        if (gtk.gtk_version[1] > 24 or
            (gtk.gtk_version[1] == 24 and gtk.gtk_version[2] > 28)):
            self.dev_select = gtk.ComboBoxEntry()
            print 'gtk.ComboBoxEntry()'
        else:
            print 'gtk.combo_box_entry_new_text()'
            self.dev_select = gtk.combo_box_entry_new_text()
            self.dev_select.append = self.dev_select.append_text
        
        for i in range(len(self.devs_list)):
            self.dev_select.append( (self.devs_list[i]))
            
        self.box_dev.pack_start(self.dev_select, True, True, padding)
        self.dev_select.child.connect('changed', self.changed_cb)
        
        # entry to show real physical device
        
        self.dev_vendor = gtk.Entry()
        self.dev_vendor.set_text("")
        self.dev_vendor.show()
        self.box_dev.pack_start(self.dev_vendor, True, True, padding)
        
        # entry to show device Opal version
        
        self.label_opal_ver = gtk.Label('Opal Version')
        self.label_opal_ver.show()
        self.box_dev.pack_start(self.label_opal_ver, True, True, padding)
        
        self.dev_opal_ver = gtk.Entry()
        self.dev_opal_ver.set_text("")
        self.dev_opal_ver.show()
        self.box_dev.pack_start(self.dev_opal_ver, True, True, padding)
        
        # device default msid
        self.label_msid = gtk.Label('MSID')
        self.label_msid.show()
        self.box_dev.pack_start(self.label_msid, True, True, padding)
        
        self.dev_msid = gtk.Entry()
        self.dev_msid.set_text("")
        self.dev_msid.show()
        self.box_dev.pack_start(self.dev_msid, True, True, padding)        
        *************************************************************
        '''

        #
        # Create psid buttons, entry with the appropriate settings
        #
        self.box_psid = gtk.HBox(homogeneous, 4)
        
        self.box_psid.pack_start(self.go_button_revert_psid, False)

        self.button_psid = gtk.Button("PSID")
        self.button_psid.connect('clicked', self.psid_info)
        self.box_psid.pack_start(self.button_psid, expand, fill, padding)
        self.button_psid.show()

        # The psid selector --- note the use of the convenience
        # type ComboBoxText if available, otherwise convenience
        # function combo_box_new_text, which is deprecated
        if (gtk.gtk_version[1] > 24 or
            (gtk.gtk_version[1] == 24 and gtk.gtk_version[2] > 28)):
            ##self.psid_select = gtk.ComboBoxText()
            self.psid_select = gtk.ComboBoxEntry()
        else:
            self.psid_select = gtk.combo_box_entry_new_text()
            # fix method name to match gtk.ComboBoxText
            self.psid_select.append = self.psid_select.append_text
            
 
        # Add psid selector
        d = self.psids
        for key in d:
            map(self.psid_select.append, [d[key]])
 
        self.psid = self.psid_select.get_active_text() ##.title()
        self.box_psid.pack_start(self.psid_select, True, True, padding)


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


        #
        # Create Locking range Number (0 -15) Start LBA, range buttons, Locking attribute(R/RW/LK)
        # entry with the appropriate settings
        # LKRNG LKRNG_SLBA LKRNG_LBALEN LKATTR
         
        self.box_LKRNG = gtk.HBox(homogeneous, 0)
        self.button_LKRNG = gtk.Button("Locking Range")
        self.button_LKRNG.connect('clicked', self.LKRNG_msg)
        self.box_LKRNG.pack_start(self.button_LKRNG, expand, fill, padding)
        self.button_LKRNG.show()
        # Pull down entry Locking Range
            
        #self.txt_LKRNG = gtk.Entry()      
        #self.txt_LKRNG.set_text(self.LKRNG)
        #self.txt_LKRNG.set_width_chars(8)
        #self.box_LKRNG.pack_start(self.txt_LKRNG, True, True, padding)
            
        if (gtk.gtk_version[1] > 24 or
            (gtk.gtk_version[1] == 24 and gtk.gtk_version[2] > 28)):
            self.LKRNG_select = gtk.ComboBoxEntry()
            print 'gtk.ComboBoxEntry()'
        else:
            print 'gtk.combo_box_entry_new_text()'
            self.LKRNG_select = gtk.combo_box_entry_new_text()
            self.LKRNG_select.append = self.LKRNG_select.append_text
            self.LKRNG_list = ['0','1','2','3','4','5','6','7','8' ]
            
        for i in range(len(self.LKRNG_list)):
            self.LKRNG_select.append( (self.LKRNG_list[i]))          
            
        self.box_LKRNG.pack_start(self.LKRNG_select, True, True, padding)
        self.LKRNG_select.child.connect('changed', self.changed_LKRNG)    
            
        # Locking start LBA
        
        self.label_LKRNG_SLBA = gtk.Label("LBA")
        self.box_LKRNG.pack_start(self.label_LKRNG_SLBA, expand, fill, padding)
        self.label_LKRNG_SLBA.show()
        self.txt_LKRNG_SLBA = gtk.Entry()      
        self.txt_LKRNG_SLBA.set_text(self.LKRNG_SLBA)
        self.txt_LKRNG_SLBA.set_width_chars(20)
        self.box_LKRNG.pack_start(self.txt_LKRNG_SLBA, True, True, padding)
        # Locking LBA Length
        self.label_LKRNG_LBALEN = gtk.Label("Length")
        self.box_LKRNG.pack_start(self.label_LKRNG_LBALEN, expand, fill, padding)
        self.label_LKRNG_LBALEN.show()
        self.txt_LKRNG_LBALEN = gtk.Entry()      
        self.txt_LKRNG_LBALEN.set_text(self.LKRNG_LBALEN)
        self.txt_LKRNG_LBALEN.set_width_chars(20)
        self.box_LKRNG.pack_start(self.txt_LKRNG_LBALEN, True, True, padding)
        

        # create action button to setup Locking range SLBA and LBALEN, set Locking range attribute button in line
            ##self.box_LKRNG_setup = gtk.HBox(homogeneous, 4)
            ##self.button_LKRNG_setup   = gtk.Button('_setup LKRNG SLBA and LBALEN and ATTR')
            ##self.button_LKRNG_setup.connect('clicked', self.setup_LKRNG)
            ##self.box_LKRNG_setup.pack_start(self.button_LKRNG_setup, expand, fill, padding)
            ##self.button_LKRNG_setup.show()

            ##self.button_LKRNG_ATTR = gtk.Button("set LKRNG ATTR")
            ##self.button_LKRNG_ATTR.connect('clicked', self.set_LKRNG_ATTR)
            ##self.box_LKRNG_setup.pack_start(self.button_LKRNG_ATTR, True, True, padding)
            ##self.button_LKRNG_ATTR.show()

        # Locking attribute
        self.label_LKATTR = gtk.Label("State")
        ##self.label_LKATTR.set_width_chars(5)
        self.box_LKRNG.pack_start(self.label_LKATTR, expand, fill, padding)
        self.label_LKATTR.show()
        self.txt_LKATTR = gtk.Entry()      
        self.txt_LKATTR.set_text(self.LKATTR)
        self.txt_LKATTR.set_width_chars(8)
        self.box_LKRNG.pack_start(self.txt_LKATTR, True, True, padding)


        #
        #
        # arrange button vertically from top
        #
        #
        self.vbox.pack_start(self.box_dev, False)
        self.vbox.pack_start(self.box_pass, False)
        ##self.vbox.pack_start(self.box_psid, False)


        
        self.vbox.pack_start(self.box_image, False)
        # Do not show locking range on gui but all the entrys exist
        ##self.vbox.pack_start(self.box_LKRNG, False)
        ##self.vbox.pack_start(self.box_LKRNG_setup, False)

        ''' on hold for pgoress bar
        # Create a centering alignment object
        align = gtk.Alignment(0.5, 0.5, 0, 0)
        self.vbox.pack_start(align, False, False, 5)
        align.show()

        # Create the ProgressBar
        self.pbar = gtk.ProgressBar()

        align.add(self.pbar)
        self.pbar.show()

        separator = gtk.HSeparator()
        self.vbox.pack_start(separator, False, False, 0)
        separator.show()

        # rows, columns, homogeneous
        table = gtk.Table(2, 2, False)
        self.vbox.pack_start(table, False, True, 0)
        table.show()
        # Add a check button to select displaying of the trough text
        check = gtk.CheckButton("Show text")
        table.attach(check, 0, 1, 0, 1,
                     gtk.EXPAND | gtk.FILL, gtk.EXPAND | gtk.FILL,
                     5, 5)
        check.connect("clicked", self.toggle_show_text)
        check.show()
        ##above progress bar on hold 
        '''
        #
        #
        # arrange button vertically from buttom
        #
        #
        self.box_exit = gtk.HBox(homogeneous, 4)
        #self.button_exit = gtk.Button("Exit")
        #self.button_exit.show()
        self.box_exit.pack_start(self.go_button_exit, True, True, padding)
        self.box_exit.pack_start(self.go_button_exit_pwroff, True, True, padding)
        self.box_exit.pack_start(self.go_button_exit_reboot, True, True, padding)
        self.vbox.pack_end(self.box_exit, False)
        self.vbox.pack_end(self.go_button_scan, False)
        self.vbox.pack_end(self.go_button_query, False)

        self.vbox.pack_end(self.go_button_PBA, False)
        ##self.vbox.pack_end(self.go_button_revert_psid, False)
        self.vbox.pack_end(self.box_psid, False)
        self.vbox.pack_end(self.go_button_revert_user, False)
        self.vbox.pack_end(self.go_button_lock, False)
        self.vbox.pack_end(self.go_button_unlock, False)
        self.vbox.pack_end(self.go_button_PBA_write, False)


        ''' temparary out until partition info can be made
        # mulitiline textview for generalpurpose display, user, locking range
        box2 = gtk.VBox(False, 10)
        box2.set_border_width(10)
        self.vbox.pack_end(box2, True, True, 0)
        box2.show()
        sw = gtk.ScrolledWindow()
        sw.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
        txtview = gtk.TextView(buffer=None)
        self.buf = txtview.get_buffer()
        setting = txtview.get_editable()
        sw.add(txtview)
        sw.show()
        txtview.show()
        box2.pack_start(sw)

        #self.txtview.show()
        ###self.vbox.pack_end(self.sw, False)
        '''
        self.add(self.vbox)
        self.scan()

        
        
    def util(self, *args):
        print 'UTILITY button clicked, create timer here'
        self.source_id = gobject.timeout_add(2000, self.counter, self)



        
    def counter(self, *args):
        gobject.source_remove(self.source_id)
        if self.PASSWORD_ONLY == True:
            print '***** Utility button timeout *********'
            #run python Lock.py"
            #####'python `find / -name Lock.py`' -> NG
            #####'python ~/Downloads/util/py/Lock.py' -> ok
            #####'python /usr/local/bin/Lock.py' -> ok

            process = subprocess.Popen(self.cmd, shell=True,
                           stdout=subprocess.PIPE, 
                           stderr=subprocess.PIPE)

            # wait for the process to terminate
            # out, err = process.communicate()
            errcode = process.returncode
            #print ("errcode = ", errcode)
            #out, err = process.communicate()
            #print ("err = ",err)
            #print ("out = ",out)
            if errcode != None :
               self.msg_err("sprawning additional utility error")
            #time.sleep(5)
            if self.ostype != 1 :
                self.exitapp()
            else : # linux platform
                import platform
                if platform.architecture()[0] == '64bit' :
                   self.iconify()
                else :
                   self.exitapp()
                
        else:
            message = gtk.MessageDialog(type=gtk.MESSAGE_INFO, buttons=gtk.BUTTONS_OK)
            txt = 'Please Click OK to return to Utility GUI and Click \'Authentication with admin password\' to continue boot process'
            message.set_markup(txt)
            res = message.run()
            if res == gtk.RESPONSE_OK :
                message.destroy()  


    def entry_check_box_pass(self, widget, checkbox):
        #global b_entry_checkbox
        b_entry_checkbox = checkbox.get_active()
        print ('pass b_entry_checkbox = ', b_entry_checkbox)
        if b_entry_checkbox:
            print "Box checked"
            pass_show = True
        else:
            print "Not checked"
            pass_show = False
        self.txt_pass.set_visibility(pass_show)
        return    
        
        
    def entry_check_box_hash(self, widget, checkbox):
        #global b_entry_checkbox
        b_entry_checkbox = checkbox.get_active()
        print ('hash b_entry_checkbox = ', b_entry_checkbox)
        if b_entry_checkbox:
            print "Box checked"
            self.pass_hash = True
        else:
            print "Not checked"
            self.pass_hash = False
        ##self.txt_pass.set_visibility(pass_hash)
        # some hash function here  
        return 

    def hash_pass(self, *args):
        #global pass_hash
        if self.force_true == True:
            self.pass_hash = True
        else:
            self.pass_hash = False
        if self.pass_hash == True:
            print ("Use hashed password")
            hash_object = hashlib.sha256(self.txt_pass.get_text())
            hex_dig = hash_object.hexdigest()
            print('hashed password = ', hex_dig)
            return hex_dig
        else:
            print ("Do not hash password")
            return self.txt_pass.get_text()
# sha512 has 64-byte
#('hashed password = ', 'b109f3bbbc244eb82441917ed06d618b9008dd09b3befd1b5e07394c706a8bb980b1d7785e5976ec049b46df5f1326af5a2ea6d103fd07c95385ffab0cacbc86')
#  col  26 , 153 = 128 digits = 64-byte
# sha256 has 32-byte ???
# '5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8')
            
            
        
    
        
#    def callback(self, widget, data=None):
#        print "%s was toggled %s" % (data, ("OFF", "ON")[widget.get_active()])
 
    def changed_LKRNG(self, *args):
        print "changed Locking Range"
 
 
    def psid_info(self, *args):
        message = gtk.MessageDialog(type=gtk.MESSAGE_INFO, buttons=gtk.BUTTONS_OK)
        txt = ''
        d = self.psids
        for key in d:
            txt = txt + key + ' : ' + d[key] + "\n"
        message.set_markup(txt)
        res = message.run()
        if res == gtk.RESPONSE_OK :
            message.destroy()

    def query(self, *args):
        print("query physical device")
        #self.devname = self.txt_dev.get_text()
        self.devname = self.dev_select.get_active_text()
        print ("self.devname: ", self.devname)
        txt2 = ""
        txt = os.popen(self.prefix + "sedutil-cli --query " + self.devname ).read()
        txt11 = "Locked ="
        m = re.search(txt11, txt)
        if m:
            print m.group()

        t = [ "Locked = [YN], LockingEnabled = [YN], LockingSupported = [YN], MBRDone = [YN], MBREnabled = [YN], MediaEncrypt = [YN]",
            "Locking Objects = [0-9]",
            "Max Tables = [0-9]*, Max Size Tables = [0-9]*",
            "Locking Admins = [0-9].*, Locking Users = [0-9]." ]

        for txt11 in t:
            #print 'BBBBBBBBBBBBBBBBBBB  txt11 : ', txt11, ' BBBBBBBBBBBBBBBBBBBBBBBBBBBBBB'
            m = re.search(txt11, txt) # 1 or 2 follow by anything ; look for Opal drive
            if m:
                txt1 = m.group()
                #print "before replace string here" + txt11
                
                #print("search:", m.group())
                #print "after remove Locking string"
                txt11 = txt1.replace("Locking ", "")            
                #print txt11
                txt1 = txt11
                txt11 = txt1.replace(", ", "\n")
                txt2 = txt2 + txt11 + "\n"
                #print txt2
            #else:
                #print 'CCCCCCCCCCCC  txt11 not found : ', txt11, ' CCCCCCCCCCCCCCCCCCCCCCCCCCCCC'

        txt2 = self.devname + " " + self.dev_vendor.get_text() +  "\n" + txt2
        #print txt2
        
        # identify objects, table, table size, admins, users, and other status
        tt = [ "Locked = [YN]", 
                "LockingEnabled = [YN]",
                "LockingSupported = [YN]",
                "MBRDone = [YN]",
                "MBREnabled = [YN]",
                "MediaEncrypt = [YN]",
                "Objects = [0-9]*",
                "Max Tables = [0-9]*",
                "Max Size Tables = [0-9]*",
                "Admins = [0-9]",
                "Users = [0-9]*" ]
      
      
        txt3=""
        for txt_33 in tt:
            m = re.search(txt_33,txt2) 
            if m:
                t3 = m.group()
                x_words = t3.split(' = ',1) # (key, val) = line.strip().split(' : ', 1)
                #print ("x_words = ", x_words)
                #print x_words[1] # YN or [0-9]*
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
                elif x_words[0] == "MediaEncrypt":
                    sts_MediaEncrypt = x_words[1]
                
                # convert table size to MB
                elif x_words[0] == "Max Size Tables":
                     tblsz = int(x_words[1],10) # 10 base 
                     #tblsz = tblsz/1000000 # size in MB       
                     x_words[1] = x_words[1] + "(" + str(tblsz/1000000) + "MB)"
                     print ("x_words[1] = ", x_words[1])
                    
                elif x_words[0] == "Objects":
                    ##print 'AAAAAAAAAAAAAAAAA  Locking Objects AAAAAAAAAAAAAAAAA'
                    self.nbr_LockingObjects = x_words[1]
                    model = self.LKRNG_select.get_model()
                    #model = [ ] # NG 
                    iter = gtk.TreeIter
                    # very important, otherwiae , duplicated show up
                    for row in model: ##self.comboboxentry:
                        #print 'row[0] =', row[0]
                        #print 'row = ', row
                        model.remove(row.iter)
                    
                    
                    for i in range(int(self.nbr_LockingObjects)): 
                        #print 'Locking Range ', i 
                        self.LKRNG_select.append(str(i))
                    self.LKRNG_select.set_active(0)
                    #print self.LKRNG_select 
                    #print self.LKRNG_select.get_model()
                elif x_words[0] == "Max Tables":
                    nbr_MaxTables = x_words[1]
                elif x_words[0] == "Max Size Tables":
                    sz_MaxTables = x_words[1]
                elif x_words[0] == "Admins":
                    nbr_Admins = x_words[1]
                elif x_words[0] == "Users":
                    nbr_Users = x_words[1]  
                
                txt3 = txt3 + x_words[0] + " = " + x_words[1] + "\n"

        
        txt2A = txt3.replace("Max Tables", "Number of DataStore")
        #print ('************* txt2A= ', txt2A)
        txt2B = txt2A.replace("Objects", "Number of Locking Range")
        txt2C = txt2B.replace("Max Size Tables","DataStore Sizes")
        txt2D = txt2C.replace("Admins","Number of Admin")
        txt2 = txt2D.replace("Users","Number of User")
        
        message_query = gtk.MessageDialog(type=gtk.MESSAGE_INFO, buttons=gtk.BUTTONS_OK)
        
        message_query.set_markup(txt2)
        res = message_query.run()
        #print message.get_widget_for_response(gtk.RESPONSE_OK)
        #print gtk.RESPONSE_OK
            
        if res == gtk.RESPONSE_OK :
            message_query.destroy()
        #raw_input("******  get hashed password ***** Press Enter to continue...")
        # how do I know device use the hashed or non-hashed password?
        sav_hash = self.pass_hash 
        self.force_true = False # test only   
        password = self.hash_pass()
        self.pass_hash = sav_hash
        self.LKRNG_msg()
		
		
		
		


    def unlock(self, *args):
        #self.devname = self.txt_dev.get_text()
        self.devname = self.dev_select.get_active_text()
        print("unlock physical device " + self.devname)
        #password = self.txt_pass.get_text()
        password = self.hash_pass()
        status1 =  os.system(self.prefix + "sedutil-cli --disableLockingRange " + self.LKRNG + " " 
                + password + " " + self.devname )
        status2 =  os.system(self.prefix + "sedutil-cli --setMBREnable off " + password + " " + self.devname )
        if (status1 | status2) != 0 :
            self.msg_err("TCG Unlock unsuccess")
        else :
            self.msg_ok("TCG Unlock success")

    def pba(self, *args):  ## Authorize preboot to allow next boot into Locked OS partition
        #self.devname = self.txt_dev.get_text()
        self.devname = self.dev_select.get_active_text()
        print("PreBoot Authorization physical device " + self.devname)
        #password = self.txt_pass.get_text()
        password = self.hash_pass()
        status1 =  os.system(self.prefix + "sedutil-cli --setMBRdone on " + password + " " + self.devname )
        status2 =  os.system(self.prefix + "sedutil-cli --setLockingRange " + self.LKRNG + " " 
                + self.LKATTR + " " + password + " " + self.devname)
        # must manually reboot
        #if self.PASSWORD_ONLY == True:
        #    self.reboot()
        if (status1 | status2) != 0 :
            self.msg_err("PreBoot Unlock unsuccess")
        else :
            self.msg_ok("Preboot Unlock success")
                



    def setup_LKRNG(self, *args):  ## Authorize preboot to allow next boot into Locked OS partition
        #self.devname = self.txt_dev.get_text()
        self.devname = self.dev_select.get_active_text()
        print("PreBoot Authorization physical device " + self.devname)
        #password = self.txt_pass.get_text()
        password = self.hash_pass()
        self.LKRNG = self.txt_LKRNG.get_text()
        self.LKRNG_SLBA = self.txt_LKRNG_SLBA.get_text()
        self.LKRNG_LBALEN = self.txt_LKRNG_LBALEN.get_text()

        rng = self.LKRNG
        lba = self.LKRNG_SLBA
        length = self.LKRNG_LBALEN
        print rng
        print lba
        print length
        status =  os.system(self.prefix + "sedutil-cli --setupLockingRange " + rng + " "
                + lba + " " + length + " "
                + password + " " + self.devname)
        self.LKATTR = self.txt_LKATTR.get_text()
        status =  os.system(self.prefix + "sedutil-cli --setLockingRange " + self.LKRNG + " " + self.LKATTR 
                + " " + password + " " + self.devname)


    def set_LKRNG_ATTR(self, *args):  ## Authorize preboot to allow next boot into Locked OS partition
        #self.devname = self.txt_dev.get_text()
        self.devname = self.dev_select.get_active_text()
        #password = self.txt_pass.get_text()
        password = self.hash_pass()
        self.LKRNG = self.txt_LKRNG.get_text()
        self.LKATTR = self.txt_LKATTR.get_text()
        
        status =  os.system(self.prefix + "sedutil-cli --setLockingRange " + self.LKRNG + " " + self.LKATTR 
                + " " + password + " " + self.devname)

    def exitapp(self, *args):
        print ("User click exit button")
        exit(0)

    def pwroff(self, *args):
        print("Exit and power off")
        
        if self.ostype == 0 :
            status =  os.system("shutdown -s -t 0")
        elif self.ostype == 1 :
            status =  os.system(self.prefix + "poweroff")
        elif self.ostype == 2 :
            status =  os.system(self.prefix + "poweroff")    
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

    def lock(self, *args):
        #self.devname = self.txt_dev.get_text()
        self.devname = self.dev_select.get_active_text()
        print("lock physical device "+ self.devname)
        #password = self.txt_pass.get_text()
        password = self.hash_pass()
        status1 =  os.system(self.prefix + "sedutil-cli --initialSetup " + password + " " + self.devname )
        status2 =  os.system(self.prefix + "sedutil-cli --enableLockingRange " + str(self.LKRNG) + " "    + password + " " + self.devname )
        status3 =  os.system(self.prefix + "sedutil-cli --setMBRdone on " + password + " " + self.devname )
        status4 =  os.system(self.prefix + "sedutil-cli --setMBREnable on " + password + " " + self.devname )
        if (status1 | status2 | status3 | status4) !=0 :
            self.msg_err("Initial Setup unsuccess")
        else : 
            self.msg_ok("Initial Setup success")
        
    def revert_psid(self, *args):
        message = gtk.MessageDialog(type=gtk.MESSAGE_WARNING, buttons=gtk.BUTTONS_OK_CANCEL)
        message.set_markup("Warning : Revert with PSID erase all data. Do you want to proceed?")
        res = message.run()
        print message.get_widget_for_response(gtk.RESPONSE_OK)
        print gtk.RESPONSE_OK
        if res == gtk.RESPONSE_OK :
            print "User click OK button"
            messageA = gtk.MessageDialog(type=gtk.MESSAGE_WARNING, buttons=gtk.BUTTONS_OK_CANCEL)
            messageA.set_markup("Warning Warning Warning : Revert with PSID erase all data. Do you really want to proceed?")
            resA = messageA.run()

            if resA == gtk.RESPONSE_OK : 
                print "execute the revert with PSID"
                ##psid = self.txt_psid.get_text()
                psid = self.psid_select.get_active_text()
                #self.devname = self.txt_dev.get_text()
                self.devname = self.dev_select.get_active_text()
                status =  os.system(self.prefix + "sedutil-cli --yesIreallywanttoERASEALLmydatausingthePSID " + psid + " " + self.devname )
            messageA.destroy()
        message.destroy()
        if status != 0 :
            self.msg_err("Revert with PSID unsuccess")
        else :
            self.msg_ok("Revert with PSID success")

    def revert_user(self, *args):
        message = gtk.MessageDialog(type=gtk.MESSAGE_WARNING, buttons=gtk.BUTTONS_OK_CANCEL)
        message.set_markup("Warning : Revert with password erase all data. Do you want to proceed?")
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
                #password = self.txt_pass.get_text()
                password = self.hash_pass()
                #self.devname = self.txt_dev.get_text()
                self.devname = self.dev_select.get_active_text()
                status =  os.system(self.prefix + "sedutil-cli --revertTPer " + password + " " + self.devname )
            messageA.destroy()
        message.destroy()
        if status != 0 :
            self.msg_err("Revert with PSID unsuccess")
        else :
            self.msg_ok("Revert with PSID success")
            
    def image_dialog(self, *args):
    ##def on_file_open_activate(self, menuitem, data=None):
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


    def LKRNG_msg(self, *args):
        # get locking range info
        #password = self.txt_pass.get_text()
        password = self.hash_pass()
        #self.devname = self.txt_dev.get_text()
        self.devname = self.dev_select.get_active_text()
        txt = os.popen( self.prefix + "sedutil-cli --listLockingRanges " + password + " " + self.devname ).read()
        #print ("txt=", txt)
# show message
        if txt == "":
            txt = "Can not get Locking Range information"
            message = gtk.MessageDialog(type=gtk.MESSAGE_WARNING, buttons=gtk.BUTTONS_OK)
        else:
            message = gtk.MessageDialog(type=gtk.MESSAGE_INFO, buttons=gtk.BUTTONS_OK)
        message.set_markup(txt)
        res = message.run()
        message.destroy()


    def pba_write(self, *args):
        print "Write PBA image to shadow MBR"
        #password = self.txt_pass.get_text()
        password = self.hash_pass()
        #self.devname = self.txt_dev.get_text()
        self.devname = self.dev_select.get_active_text()
        self.image = self.txt_image.get_text()
        start = time.clock()
        status = os.system( self.prefix + "sedutil-cli --loadpbaimage " + password + " " + self.image + " "+ self.devname )
        stop = time.clock()
        elapsed = (stop - start)
        print ("elaspsed = ", elapsed)
        
        if status != 0 :
            self.msg_err("Write PBA image "+ self.image + " Unsuccess")
        else :
            self.msg_ok("Write PBA image "+ self.image + " Success")        
        '''if status != 0 :
            #dialog = gtk.Dialog(title=None, parent=None, flags=0, buttons=None)
            message = gtk.MessageDialog(type=gtk.MESSAGE_ERROR, buttons=gtk.BUTTONS_OK)
            message.set_markup("Error: Write PBA image "+ self.image)
            message.run()
            message.destroy()
        else:
            print "show progress bar"
            #  Create a centering alignment object
            align = gtk.Alignment(0.5, 0.5, 0, 0)
            self.vbox.pack_start(align, False, False, 5)
            align.show()
            # Create the ProgressBar
            self.pbar = gtk.ProgressBar()
            align.add(self.pbar)
            self.pbar.show()
        '''


############################################################################



    def changed_cb(self, entry):
        print 'Select device has changed', entry.get_text()
        act_idx = self.dev_select.get_active() # get active index
        print 'active index = ', act_idx
        self.dev_vendor.set_text(self.vendor_list[act_idx])
        print 'dev_vendor = ',self.dev_vendor
        
        self.dev_series.set_text(self.series_list[act_idx])
        print 'dev_series = ',self.dev_series        
        
        
        self.dev_opal_ver.set_text(self.opal_ver_list[act_idx])
        width = int (self.opal_ver_list[act_idx])
        print ('width = ', width)
        self.dev_opal_ver.set_width_chars(width)
        print 'dev_opal_ver = ',self.opal_ver_list
        if self.PASSWORD_ONLY == False:
            self.query() # Preboot do not query on device change
        self.get_msid()
        

    def get_msid(self, *args):  
        txt = os.popen(self.prefix + "sedutil-cli --printDefaultPassword " + self.devname ).read()
        if txt == '' :
            return
        x_words = txt.split(': ',1)
        if self.devs_list != [] :
            self.dev_msid.set_text(x_words[1])
            self.dev_msid.set_width_chars(8)

        
    def scan(self, *args):
        if self.firstscan == False:
            print '*************** enter rescan ********************'
            model = self.dev_select.get_model() # model is the list of entry
            print ('model:',model)
        
            iter = gtk.TreeIter
            # very important, otherwiae , duplicated show up
            for row in model: ##self.comboboxentry:
                #print 'row[0] =', row[0]
                #print 'row = ', row
                model.remove(row.iter)
                
            #self.dev_select.clear()     # NG pull down get destroyed
            #self.dev_select.set_model() # NG pull down not work
            
            
            print 'after delete all entry, model= ', model

            if len(self.vendor_list) > 0:
                self.vendor_list = []
                print 'after remove all item, vendor_list = ', self.vendor_list
                self.opal_ver_list = []
                print 'after remove all item, opal_ver_list = ', self.opal_ver_list
                
        self.finddev()
        for idx in range(len(self.devs_list)) :
            ##map(self.dev_select.append, [self.devs_list[idx]]) # populate the dev array
            self.dev_select.append( self.devs_list[idx]) # populate the dev array

        if len(self.devs_list) > 0: 
            self.dev_select.set_active(0) ##.title()
            self.txt_dev = self.dev_select.get_active_text() ##.title()
            ##self.dev_vendor = self.vendor_list[0]
            self.dev_vendor.set_text(self.vendor_list[0])
            self.dev_opal_ver.set_text(self.opal_ver_list[0])
            
        self.firstscan = False
        self.get_msid() #?????
##############################################################################

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
        txt = os.popen(self.prefix + 'sedutil-cli --scan').read()
        ##m = re.search("/dev/sd[a-z] .[12].", txt) # 1 or 2 follow by anything
        ## find out what device name returned 1.PhysicalDrive(Window) 2./dev/sd?(Linux) 3. /dev/disk?(OSX)
        print "scan result : "
        print txt
		
     
        names = ['PhysicalDrive[0-9]', '/dev/sd[a-z]', '/dev/nvme[0-9]',  '/dev/disk[0-9]']
        idx = 0
        self.devs_list = []
        for index in range(len(names)): #index=0(window) 1(Linux) 2(OSX)
            print ("index= ", index)
            print names[index]
        
            m = re.search(names[index] + ".*", txt) # 1st search identify OS type and the pattern we are looking for
            
            if m:
                self.ostype = index
                if (index == 0 ):
                   self.prefix = ""
                else:
                   self.prefix = "sudo "
                

                txt11 = names[index] + " .[12LP] *[A-z0-9].*"
                print("looking for Opal device ", txt11)

                m = re.findall(txt11, txt) # 1 or 2 follow by anything ; look for Opal drive
                print ("m = ", m)
                if m:
                    for tt in m:
                        print tt
                        m2 = re.match(names[index],tt)
                        x_words = tt.split() 
                        #print ("x_words = ", x_words)
                        #print x_words[2] # vendor name
                        #print x_words[1] # Opal version
                        if (index == 0) : 
                            self.devname = "\\\\.\\" + m2.group()
                        else:
                            self.devname =  m2.group()
                        self.devs_list.append(self.devname)
                        ## anything after x_words[2] shall be concatenated
                        print len(x_words)
                        if len(x_words) > 3 : # concatenate all word together
                            print "x_words len > 3"
                            for y in range(3,(len)(x_words)):###for y in range(3,(len)(x_words):
                                x_words[2] = x_words[2] + " " + x_words[y]
                        
                        #x_word contain vendor name and series number separate with :
                        
                        #print x_words[2] # vendor name #DEBUG
                        y_words = x_words[2].split(":",1)
                        print ("y_words = ", y_words)
                        print ("y_words[0] = ", y_words[0])
                        print ("y_words[1] = ", y_words[1])
                        self.vendor_list.append(y_words[0])
                        y = y_words[1].replace(" ", "")
                        y_words[1] = y 
                        self.series_list.append(y_words[1]) 
                        self.opal_ver_list.append(x_words[1])
            else:
                print "No Matched : ", names[index]
                #print "device name : ",self.devname
        if self.devs_list != []:
            print ("devs_list: ",  self.devs_list)
            print ("vendor_list: ", self.vendor_list)
            print ("series_list: ", self.series_list)
            print ("opal_ver_list: ", self.opal_ver_list)

  

    def passworddialog(self, *args):
        # Create a new hbox with the appropriate homogeneous and spacing settings
        homogeneous = False
        spacing = 0
        expand = False
        fill = False
        padding = 0
        #
        # Create password buttons, entry with the appropriate settings
        #
        width = 12
        self.box_pass = gtk.HBox(False, 0)
        self.label_pass = gtk.Label("Password")
        self.label_pass.set_width_chars(width)
        self.box_pass.pack_start(self.label_pass, expand, fill, padding)
        self.label_pass.show()

        '''
        # save for future if user ask for color
        map = self.button_pass.get_colormap()
        colour = map.alloc_color("blue") # light red
        style = self.button_pass.get_style().copy()
        style.bg[gtk.STATE_NORMAL] = colour
        self.button_pass.set_style(style)
        '''
        self.txt_pass = gtk.Entry()
        self.txt_pass.set_text("password")
        self.txt_pass.set_visibility(False)
        self.box_pass.pack_start(self.txt_pass, True, True, padding)
        
        self.check_box_pass = gtk.CheckButton("Show Password")
        self.check_box_pass.connect("toggled", self.entry_check_box_pass, self.check_box_pass)
        self.check_box_pass.set_active(False)  # Set the defaut
        self.check_box_pass.show()
        self.box_pass.pack_start(self.check_box_pass, expand, fill, padding)
        
        # check box for hash
        self.check_box_hash = gtk.CheckButton("Use Hashed password")
        self.check_box_hash.connect("toggled", self.entry_check_box_hash, self.check_box_hash)
        self.check_box_hash.set_active(True)  # Set the defaut to hash
        self.check_box_hash.show()
        self.box_pass.pack_start(self.check_box_hash, expand, fill, padding)

        
        # allow user to save password option
 
        self.pass_sav = gtk.Button("Save to USB")
        self.pass_sav.connect("clicked", self.pass_save2usb)
        self.pass_sav.show()
        self.box_pass.pack_start(self.pass_sav, expand, fill, padding)
     
        self.vbox.pack_start(self.box_pass, False)
        
        # allow user to retrive password option
 
        self.pass_rd = gtk.Button("Read from USB")
        self.pass_rd.connect("clicked", self.pass_rdusb)
        self.pass_rd.show()
        self.box_pass.pack_start(self.pass_rd, expand, fill, padding)
     
        self.vbox.pack_start(self.box_pass, False)


    def pass_save2usb(self, *args):
        print ("Enter save2usb")
        #find it usb drive exist
        txt = os.popen("wmic logicaldisk get name").read()
        print ("Drive = ",txt)
        m = re.search("[A-z]:",txt)
        if m:
            print m.group()
        drv = m.group()
        #write password into file import print seems to cause more problem, skip for now
        #print(self.txt_pass.get_text(), file=fname)
 
        # %y : Year without century as a zero-padded decimal number
        # %m : Month as a zero-padded decimal number
        # %d : Day of the month ;
        # %X : Locale's appropriate time representation
        t = time.strftime("%y%m%d%X")
        t = t.replace(":","")
        print ("time = ", t)
        fname = "C:\\" + "password_" + self.dev_series.get_text() + "_" + self.hash_pass() + "_" + t + ".psw"
        print ("file name = ", fname)
        # make sure create file
        
        
        try:
            connect = open( fname, "r")
        except IOError:
            connect = open( fname, "a")
            connect.close()
            print ("create ", fname)
        
        #No Need to write to file anymore
        #txt = self.dev_series.get_text() + ":" + self.dev_vendor.get_text() + ":" + self.txt_pass.get_text() + ":" + t
        #print ("password file contents = ", txt) 
        #with open(fname,"w") as f:
        #    f.write(txt)
        #f.close()
        
    def pass_rdusb(self, *args):
        print ("Enter rdusb")
        #find it usb drive exist
        
        # scan id password_*.psw exit
        import glob
        
        g = glob.glob("C:\password_*.psw")
        if not g == [] :
            print ("password file in this directory")
            
            latest_f = ""
            latest_t = "0"
            latest_n = ""
            latest_ps = ""
            
            for f in g:
                print ("filename : ", f)
                (p, n , ps, t)  = f.split("_") # password_series_hashedpsw_tttttt.psw
                t = t.replace(".psw","")
                print ("p = ",p)
                print ("n = ",n)                            # find the latest password file
                print ("ps = ",ps) 
                print ("t = ",t)
                print ("self.dev_series = ", self.dev_series.get_text())
                ## JERRY raw_input("split file name into 4 piece")
                # look for matching series number then determine if latest
                if self.dev_series.get_text() == n : # if series number match, we have candidate of password
                    ## JERRY raw_input("find_passmatch return True, hit any key to continue :")
                    print ("interge of t = ",int(t))
                    print ("integer of latest_t = ", int(latest_t))
                    if int(t) > int(latest_t):
                            ## JERRY raw_input("find the latest password file, does it match the device ?")
                    
                            latest_t = t
                            latest_f = f 
                            latest_n = n
                            latest_ps = ps
                            print ("current timestamp is > original timestamp")
                            print ("latest_t = ",latest_t)
                            print ("latest_f = ",latest_f)                            # find the latest password file
                            print ("latest_n = ",latest_n) 
                            print ("latest_ps = ",latest_ps) 
                            
            if not ps == "": 
                self.txt_pass.set_text(latest_ps) # use this password
                print ("got latest password")
                return
                            
            # if auto read doesn't work, allow user to choose its own password file                
            # how to finalize the latest matching password file
            # now latest_f contain 
            self.txt_pass.set_text() # write to password entry
                                
        else:
            print "No Matching device found"
        
        ## JERRY raw_input("hit anykey to continue")
        
        # use file dialog for now
        self.fcd = gtk.FileChooserDialog("Open...",
             None,
             gtk.FILE_CHOOSER_ACTION_OPEN,
             (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL, gtk.STOCK_OPEN, gtk.RESPONSE_OK))
        self.response = self.fcd.run()
        if self.response == gtk.RESPONSE_OK:
            print "Selected filepath: %s" % self.fcd.get_filename()
            fname = self.fcd.get_filename()
            print ("file name : ", fname)
            
            self.find_pass(fname)

                #f.close()
        self.fcd.destroy()
        
    
    
    
    
    
    
    

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

    def scan_passwordonly(self, *args):
        homogeneous = False
        spacing = 0
        expand = False
        fill = False
        padding = 0
        width = 12
                #
        # Create devname buttons, entry with the appropriate settings
        #
        self.box_dev = gtk.HBox(homogeneous, spacing)
        self.label_dev = gtk.Label(" Select Device") 
        self.label_dev.set_width_chars(width)
        self.label_dev.set_justify(gtk.JUSTIFY_LEFT)
        self.box_dev.pack_start(self.label_dev, expand, fill, padding)
        self.label_dev.show()

        self.findos() #
        
        self.txt_dev = gtk.Entry()
        self.txt_dev.set_text("sdz") # default non-exist dev, so it won't accidentally self-delete

        if (gtk.gtk_version[1] > 24 or
            (gtk.gtk_version[1] == 24 and gtk.gtk_version[2] > 28)):
            self.dev_select = gtk.ComboBoxEntry()
            print 'gtk.ComboBoxEntry()'
        else:
            print 'gtk.combo_box_entry_new_text()'
            self.dev_select = gtk.combo_box_entry_new_text()
            #self.dev_select.get_model()
            #self.dev_select.set_model(none)
            #self.dev_select.clear()
            self.dev_select.append = self.dev_select.append_text
        
        for i in range(len(self.devs_list)):
            #map(self.dev_select.append, (self.devs_list[i]))
            # do not use map
            self.dev_select.append( (self.devs_list[i]))
            
        self.box_dev.pack_start(self.dev_select, True, True, padding)
        self.dev_select.child.connect('changed', self.changed_cb)
        
        # entry to show real physical device
        
        self.dev_vendor = gtk.Entry()
        self.dev_vendor.set_text("")
        self.dev_vendor.set_property("editable", False)
        self.dev_vendor.show()
        self.box_dev.pack_start(self.dev_vendor, True, True, padding)
        
        self.vbox.pack_start(self.box_dev, False)
        
        
        # move series number to next line of gui
        # Label expand=False;  Entry expand=False; field will expand according to input text
        
        self.box_ser = gtk.HBox(False, 20)
        
        # entry to show series number
        
        self.label_series = gtk.Label(' Series #')
        self.label_series.show()
        self.box_ser.pack_start(self.label_series, False, False, padding)

        self.dev_series = gtk.Entry()
        self.dev_series.set_text("")
        self.dev_series.set_property("editable", False)
        self.dev_series.show()
        self.box_ser.pack_start(self.dev_series, False, True, padding)
        
        # entry to show device Opal version
        
        self.label_opal_ver = gtk.Label('Opal Version')
        self.label_opal_ver.show()
        self.box_ser.pack_start(self.label_opal_ver, False, False, padding)
        
        self.dev_opal_ver = gtk.Entry()
        self.dev_opal_ver.set_text("")
        self.dev_opal_ver.set_property("editable", False)
        self.dev_opal_ver.show()
        self.box_ser.pack_start(self.dev_opal_ver, False, True, padding)
        
        # device default msid
       
        self.label_msid = gtk.Label('MSID')
        self.label_msid.show()
        self.box_ser.pack_start(self.label_msid, False, False, padding)
        
        self.dev_msid = gtk.Entry()
        self.dev_msid.set_text("")
        self.dev_msid.set_property("editable", False)
        self.dev_msid.show()
        self.box_ser.pack_start(self.dev_msid, True, True, padding)  
        
        self.vbox.pack_start(self.box_ser, False)
        
       
            
    def run(self):
        ''' Run the app. '''
        self.show_all()
        gtk.main()



app = LockApp()
app.run()
