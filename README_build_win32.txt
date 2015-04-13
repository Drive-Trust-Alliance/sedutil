Pre-requisites for building msed under Windows, tested under Windows 8.1 (which has stricter powershell defaults than Windows 7):

1. Visual Studio 2013 Community Edition (minimum).
2. Git command-line client for Windows. [ http://git-scm.com/download/win ]
3. To generate the version.h file correctly, you will need to adjust the PowerShell security setting for *external* script invocations. 
   The "least insecure" setting that'll allow it to work without setting up local script signing is 'remotesigned'.
   Run a standard command prompt (not powershell) as Administrator and execute the following command (adjusting path as necessary): 
     c:\windows\syswow64\WindowsPowerShell\v1.0\powershell.exe -command set-executionpolicy remotesigned

Those are the basics that will allow it to build. 

Additional powershell/signed-script notes:

4. If you can sign your own scripts, you should sign the script and change the above policy to 'allsigned' for the "least insecure" setting.

5. If you can't sign and you make changes to the GetVersion.ps1 script, you'll need to change the above policy to 'unrestricted', which is "most insecure".

6. If you need to run the GetVersion.ps1 script directly from a powershell window you'll also need to adjust the security setting for *internal* script invocations.
   Run PowerShell as Administrator and execute one of the following commands (ordered from least insecure to most insecure), depending on your above needs:
    set-executionpolicy allsigned
	set-executionpolicy remotesigned
	set-executionpolicy unrestricted
   
7. You can restore the more secure powershell configuration to the default for Windows 8.x by adjustng the lines to end with [set-executionpolicy "restricted"] and reinvoking them.

8. If using another version of windows, you will want to query the original settings first so you know what to restore the values to. The command to query the policy is: get-executionpolicy

