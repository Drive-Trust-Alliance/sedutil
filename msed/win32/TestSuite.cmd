@echo off
if -%1-==-- echo device not specified & exit /b
echo Drive %1 needs to be in a Reverted state 
echo if you haven't already done so please do a PSID revert
echo on %1
echo -
echo Continuing with this test suite WILL ERASE ALL DATA ON %1
echo If you dont want that to happen hit Ctrl-C now.
echo press enter to continue or Ctrl-c to abort
pause
echo Last chance to hit Ctrl-c an keep the data on your drive
pause
:: test msed commands
::set MSED=..\..\Win32\Release\msed.exe
::set MSED=..\..\x64\Release\msed.exe
set MSED=msed.exe
set LOGFILE=msed_log.txt
set OUTPUTSINK=^>^> %LOGFILE% 2^>^&1
unset NIXGREP
::set NIXGREP=^| ^"C:\Program Files (x86)\Git\bin\grep.exe^" ^-a MBREnable
::
echo Begin TestSuite.cmd > %LOGFILE%
call :nottee "testing msed %date% @ %time%" 

%MSED% --scan %OUTPUTSINK%
call :nottee "Perform the initial setup"
%MSED% --initialsetup passw0rd %1 %OUTPUTSINK%
%MSED% --query %1 %NIXGREP% %OUTPUTSINK%
call :nottee "change the LSP Admin1 password"
%MSED% --setAdmin1Pwd passw0rd password %1 %OUTPUTSINK%
call :nottee "test readlocking"
%MSED%  --setLockingRange 0 RO password %1 %OUTPUTSINK%
%MSED% --query %1 %NIXGREP% %OUTPUTSINK%
%MSED% --disableLockingRange 0 password %1 %OUTPUTSINK%
%MSED% --query %1 %NIXGREP% %OUTPUTSINK%
%MSED% --enableLockingRange 0 password %1 %OUTPUTSINK%
call :nottee "test write locking"
%MSED% --setLockingRange 0 RW password %1 %OUTPUTSINK%
%MSED% --query %1 %NIXGREP% %OUTPUTSINK%
call :nottee "set LockingRange 0 LK"
%MSED% --setLockingRange 0 lk password %1 %OUTPUTSINK%
%MSED% --query %1 %NIXGREP% %OUTPUTSINK%
call :nottee "disable locking on the global range"
%MSED% --disableLockingRange 0 password %1 %OUTPUTSINK%
%MSED% --query %1 %NIXGREP% %OUTPUTSINK%
call :nottee "enable mbr shadowing"
%MSED% --setMBREnable on password %1 %OUTPUTSINK%
%MSED% --query %1 %NIXGREP% %OUTPUTSINK%
call :nottee "set MBRDone"
%MSED% --setMBRDone ON password %1 %OUTPUTSINK%
%MSED% --query %1 %NIXGREP% %OUTPUTSINK%
call :nottee "unset MBRDone"
%MSED% --setMBRDone off password %1 %OUTPUTSINK%
%MSED% --query %1 %NIXGREP% %OUTPUTSINK%
call :nottee "disable mbr shadowing"
%MSED% --setMBREnable OFF password %1 %OUTPUTSINK%
%MSED% --query %1 %NIXGREP% %OUTPUTSINK%
call :nottee "resetting device"
%MSED% --reverttper passw0rd %1 %OUTPUTSINK%
%MSED% --query %1 %NIXGREP% %OUTPUTSINK%
%MSED% --validatePBKDF2 %OUTPUTSINK%
echo Thanks for running the test suite 
echo please e-mail %LOGFILE% to r0m30@r0m30.com
echo along with the OS, OS level and type of drive you have
pause
exit /b
:nottee
echo %1
echo %1 %OUTPUTSINK%