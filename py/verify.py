import background
import gtk
import platform
import os
import re
import requests
import sys

from HTMLParser import HTMLParser

import lockhash

if platform.system() == 'Windows':
    import PyExtLic
    import vfysig
import PyExtOb

class MyHTMLParser(HTMLParser):
    latest_major = 0
    latest_minor = 0
    latest_build = 0
    def handle_data(self, data):
        txt = 'The latest version is ([0-9]+)\.([0-9]+)\.([0-9]+)'
        m = re.search( txt , data)
        if m : 
            self.latest_major = int(m.group(1))
            self.latest_minor = int(m.group(2))
            self.latest_build = int(m.group(3))

def pbaVerify(self, *args):
    pba_devlist = []
        
    pba_devidx = -1
    txt = os.popen(self.prefix + 'sedutil-cli --scan n').read()
                    
    #print txt
 
    names = ['/dev/sd[a-z]', '/dev/nvme[0-9]']
    
    
    
    mn_tmp = []
    s_tmp = []
    
    
    for index in range(len(names)):
    
        m = re.search(names[index] + ".*", txt)
        
        if m:
            
            txt11 = names[index] + "\s+.[A-z0-9]+\s+.*"
            m1 = re.findall(txt11, txt)
            if m1:
                
                for tt in m1:
                    
                    rgx = '(?:/dev/sd[a-z][0-9]?|/dev/nvme[0-9])\s+([12ELP]+|No)\s+(\S+(?:\s\S+)*)\s*:\s*([^:]+)\s*:(.+)'
                    md = re.match(rgx,tt)
                    if md:
                        mn_tmp.append(md.group(2))
                        s_tmp.append(md.group(4).ljust(20))
    present = False
    lic_file_regex = 'hash-(sd[a-z][0-9]*|nvme[0-9]n[0-9])-lic.txt'
    #get list of files in /tmp/h.d/ and compare regex to file names
    dir_files = os.listdir('/tmp/h.d/')
    for fn in dir_files:
        m = re.match(lic_file_regex, fn)
        if m:
            lic_file = '/tmp/h.d/hash-' + m.group(1) + '-lic.txt'
            f = open(lic_file, 'r')
            f_info = f.read().lower()
            f.close()
            reg_z = '0{64}'
            m1 = re.match(reg_z, f_info)
            if not m1:
                pba_devlist.append(m.group(1))
    #valid = True
    #print pba_devlist
    if len(pba_devlist) > 0:
        valid = False
        #list of 0's of len(self.devs_list)
        #once verified, set to 1
        #afterwards, remove non-verified drives from lists
        ver_list = [0] * len(mn_tmp)
        for i in pba_devlist:
            lic_file = '/tmp/h.d/hash-' + i + '-lic.txt'
            if os.path.isfile(lic_file):
                f = open(lic_file, 'r')
                f_info = f.read().lower()
                f.close()
                if len(f_info) == 64:
                    #change to construct levels from module
                    #change audituser password to be constructed as a global or return from module
                    levels = []
                    #for L in [1, 2, 4, 16, 32, 64]:
                    for L in [64, 32, 16, 4, 2, 1]:
                        levels.append(PyExtOb.get_str(L))
                    count = 0
                    for lic in levels:
                        for j in range(len(s_tmp)):
                            salt = s_tmp[j]
                            #f_curr = lockhash.hash_pbkdf2(lic, salt)
                            f_curr = lockhash.hash_sig(lic, salt)
                            if f_curr == f_info:
                                valid = True
                                if count == 0:
                                    self.PBA_VERSION = 5
                                    self.MAX_DEV = sys.maxint
                                elif count == 1:
                                    self.PBA_VERSION = 4
                                    self.MAX_DEV = 100
                                elif count == 2:
                                    self.PBA_VERSION = 3
                                    self.MAX_DEV = 25
                                elif count == 3:
                                    self.PBA_VERSION = 2
                                    self.MAX_DEV = 5
                                elif count == 4:
                                    self.PBA_VERSION = 1
                                    self.MAX_DEV = 5
                                elif count == 5:
                                    self.PBA_VERSION = 0
                                    self.MAX_DEV = sys.maxint
                                self.pba_devname = i
                                pba_devidx = j
                                ver_list[j] = 1
                                break
                        count = count + 1
                        if self.pba_devname != None:
                            break
                    if self.pba_devname != None:
                        break
        if sum(ver_list) == 0:
            self.invalid_pba = True

def licCheck(self, *args):
    if self.VERSION != 1 and self.VERSION != -1 and self.VERSION != 1:
        version_text = os.popen(self.prefix + 'sedutil-cli --version').read()
        regex_license = '0:([0-9]+);'
        f = re.search(regex_license, version_text)
        v_check = -1
        md_check = -1
        if f:
            v = f.group(1)
            v_int = int(v)
            v_bit = format(v_int, '08b')
            bit_rgx = '1[0,1]*'
            m_bit = re.search(bit_rgx,v_bit)
            if m_bit:
                top_bit = len(m_bit.group(0))
                if top_bit == 1:
                    v_check = 0
                    md_check = sys.maxint
                elif top_bit == 2:
                    v_check = 2
                    md_check = 5
                elif top_bit == 3:
                    v_check = 3
                    md_check = 5
                elif top_bit == 5:
                    v_check = 3
                    md_check = 25
                elif top_bit == 6:
                    v_check = 3
                    md_check = 100
                elif top_bit == 7:
                    v_check = 3
                    md_check = sys.maxint
            if v_check != self.VERSION and md_check != self.MAX_DEV:
                self.msg_err('License change detected. Closing the application.')
                self.exitFL(self)
            else:
                return
        else:
            self.msg_err('License not detected. Closing the application.')
            self.exitFL(self)

def initCheck(self, *args):
    version_text = ''
    #if self.DEV_OS == 'Windows':
    #    version_text = PyExtLic.get_lic()
    #else:
    if platform.system() == 'Windows':
        rsig = vfysig.vfysig()
        if rsig == 0 :
            info  =  'One of software component is not signed properly Please make sure you get the authentic files from Fidelity Height LLC!!' 
            dialog = gtk.MessageDialog(type=gtk.MESSAGE_INFO,message_format=info,buttons=gtk.BUTTONS_OK) 
            dialog.set_title('') 
            dialog.run() 
            dialog.destroy()
 
    version_text = os.popen(self.prefix + 'sedutil-cli --version').read()
    regex_license = '0:([0-9]+);'
    f = re.search(regex_license, version_text)
    regex_build = 'FL\.([0-9]+)\.([0-9]+)\.([0-9]+)'
    f1 = re.search(regex_build, version_text)
    if f or self.VERSION != -1:
        if self.VERSION == -1:
            v = f.group(1)
            v_int = int(v)
            v_bit = format(v_int, '08b')
            bit_rgx = '1[0,1]*'
            m_bit = re.search(bit_rgx,v_bit)
            if m_bit:
                top_bit = len(m_bit.group(0))
                if top_bit == 1:
                    self.VERSION = 0
                    self.MAX_DEV = sys.maxint
                elif top_bit == 2:
                    self.VERSION = 2
                    self.MAX_DEV = 5
                elif top_bit == 3:
                    self.VERSION = 3
                    self.MAX_DEV = 5
                elif top_bit == 5:
                    self.VERSION = 3
                    self.MAX_DEV = 25
                elif top_bit == 6:
                    self.VERSION = 3
                    self.MAX_DEV = 100
                elif top_bit == 7:
                    self.VERSION = 3
                    self.MAX_DEV = sys.maxint

        
        if self.VERSION == 1 and self.DEV_OS == 'Linux':
            pbaVerify(self)
            
        
        res = lockhash.testPBKDF2()
        
        status = res[0]
        
        hash_v = os.popen(self.prefix + 'sedutil-cli --validatePBKDF2').read()
        f = re.search(res[1], hash_v)

        if status != 0 or not f:
            self.msg_err("Hash validation failed")
            gtk.main_quit()
    if f1 and self.VERSION != 1:
        curr_major = int(f1.group(1))
        curr_minor = int(f1.group(2))
        curr_build = int(f1.group(3))
        urlStr = "https://quicklicensemanager.com/fidelityheight/qlmLicenseServer/qlmservice.asmx/GetLatestVersionHttp?is_productid=1&is_majorversion=1&is_minorversion=0"
        r = ''
        r =requests.get(urlStr)
        if r.status_code == 200:
            parser = MyHTMLParser()
            parser.feed(r.text)
            if curr_major < parser.latest_major:
                self.up_to_date = False
            elif curr_major == parser.latest_major:
                if curr_minor < parser.latest_minor:
                    self.up_to_date = False
                elif curr_minor == parser.latest_minor:
                    if curr_build < parser.latest_build:
                        self.up_to_date = False
        
    return f
    
def updateCheck(button, self):
    version_text = os.popen(self.prefix + 'sedutil-cli --version').read()
    regex_build = 'FL\.([0-9]+)\.([0-9]+)\.([0-9]+)'
    f1 = re.search(regex_build, version_text)
    self.up_to_date = True
    if f1 and self.VERSION != 1:
        curr_major = int(f1.group(1))
        curr_minor = int(f1.group(2))
        curr_build = int(f1.group(3))
        urlStr = "https://quicklicensemanager.com/fidelityheight/qlmLicenseServer/qlmservice.asmx/GetLatestVersionHttp?is_productid=1&is_majorversion=1&is_minorversion=0"
        r = ''
        r =requests.get(urlStr)
        if r.status_code == 200:
            parser = MyHTMLParser()
            parser.feed(r.text)
            if curr_major < parser.latest_major:
                self.up_to_date = False
            elif curr_major == parser.latest_major:
                if curr_minor < parser.latest_minor:
                    self.up_to_date = False
                elif curr_minor == parser.latest_minor:
                    if curr_build < parser.latest_build:
                        self.up_to_date = False
            if not self.up_to_date:
                message = gtk.MessageDialog(type=gtk.MESSAGE_QUESTION, buttons=gtk.BUTTONS_YES_NO, parent = self)
                message.set_markup("A new version of Opal Lock is available, would you like to update now?")
                res = message.run()
                message.destroy()
                if res == gtk.RESPONSE_YES:
                    os.system("start \"\" https://fidelityheight.com/download/OpalLock_setup.exe")
                    #gtk.main_quit()
                else:
                    self.update_link.show()
            else:
                self.msg_ok('You are running the latest version of Opal Lock.')
        else:
            self.msg_warn('Could not check online for updates, please check your connection or try again later.')