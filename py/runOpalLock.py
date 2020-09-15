'''
Credit from URL
https://www.programcreek.com/python/example/97304/win32com.shell.shell.ShellExecuteEx
example6

Project: uac-a-mola   Author: ElevenPaths   File: admin.py    License: GNU General Public License v3.0

'''
def runAsAdmin(cmdLine=None, wait=True):
 
    import os, sys, types
    if os.name != 'nt':
        raise RuntimeError, "This function is only implemented on Windows."

    import win32api
    import win32con
    import win32event
    import win32process
    from win32com.shell.shell import ShellExecuteEx
    from win32com.shell import shellcon

    python_exe = sys.executable
    
    print ("python_exe=",python_exe, "sys.argv=",sys.argv)

    if cmdLine is None:
        cmdLine = [python_exe] + sys.argv
    elif type(cmdLine) not in (types.TupleType, types.ListType):
        raise ValueError, "cmdLine is not a sequence."
    cmd = '"%s"' % (cmdLine[0],)
    # XXX TODO: isn't there a function or something we can call to massage command line params?
    params = " ".join(['"%s"' % (x,) for x in cmdLine[1:]])
    print("params=", params)
    cmdDir = ''
    showCmd = win32con.SW_SHOWNORMAL
    #showCmd = win32con.SW_HIDE
    lpVerb = 'runas'  # causes UAC elevation prompt.

    print "Running", cmd, params

    # ShellExecute() doesn't seem to allow us to fetch the PID or handle
    # of the process, so we can't get anything useful from it. Therefore
    # the more complex ShellExecuteEx() must be used.

    # procHandle = win32api.ShellExecute(0, lpVerb, cmd, params, cmdDir, showCmd)

    procInfo = ShellExecuteEx(nShow=showCmd,
                              fMask=shellcon.SEE_MASK_NOCLOSEPROCESS,
                              lpVerb=lpVerb,
                              lpFile=cmd,
                              lpParameters=params)

    if wait:
        procHandle = procInfo['hProcess']
        obj = win32event.WaitForSingleObject(procHandle, win32event.INFINITE)
        rc = win32process.GetExitCodeProcess(procHandle)
        # print "Process handle %s returned code %s" % (procHandle, rc)
    else:
        rc = None

    return rc 
    

if __name__ == "__main__":
    # cmdLine need to be TupleType , list
    line=('OpalLock.exe', ' ')
    print("cmdLine=", line)
    rc = runAsAdmin(cmdLine=line,  wait=True)
    print ("rc=", rc) 
    
    