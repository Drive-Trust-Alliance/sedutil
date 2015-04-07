Pre-requisites for building msed under Windows, tested under Windows 8.1:

1. Visual Studio 2013 Community Edition (minimum).
2. Git command-line client for Windows. [ http://git-scm.com/download/win ]
3. To generate the version.h file correctly, you will need to adjust the PowerShell security settings in two places.
   Note: (b) is also required because there are both internal-to and external-to powershell security policies.
   a. Run PowerShell as Administrator and execute the following command:
       set-executionpolicy unrestricted
   b. Run a standard command prompt as administrator and execute the following command (adjusting path as necessary): 
       c:\windows\syswow64\WindowsPowerShell\v1.0\powershell.exe -command set-executionpolicy unrestricted

You can restore the more secure powershell configuration by changing "unrestricted" to "restricted", 
(the default for Windows 8.1) and executing both command lines again. If using another version of windows,
you will want to query the original settings first using the get-executionpolicy equivalents so you know
what to restore the values to.
