#!/bin/bash
if [ -$1- = "--" ]; then echo device not specified; exit; fi 
echo Drive $1 needs to be in a Reverted state 
echo if you haven\'t already done so please do a PSID revert
echo on $1
echo -
echo Continuing with this test suite WILL ERASE ALL DATA ON $1
echo If you dont want that to happen hit Ctrl-C now.
echo press enter to continue or Ctrl-c to abort
read INOUT
echo Last chance to hit Ctrl-c an keep the data on your drive
read INPUT
## test msed commands
##MSED=../../dist/Release/GNU-Linux-x86/msed
MSED=./msed
echo testing msed `date` > msed_test.log
uname -a >> msed_test.log
$MSED --ValidatePBKDF2 >> msed_test.log
$MSED --scan >> msed_test.log
$MSED --query $1 >> msed_test.log
echo Perform the initial setup
$MSED --initialsetup passw0rd $1 >> msed_test.log
$MSED --query $1 >> msed_test.log
echo change the LSP Admin1 password
$MSED --setAdmin1Pwd passw0rd password $1 >> msed_test.log
echo test readlocking
$MSED  --setLR 0 RO password $1 >> msed_test.log
$MSED --query $1 >> msed_test.log
$MSED --unsetWriteLockedGlobal --password password $1 >> msed_test.log
$MSED --query $1 >> msed_test.log
echo test write locking
$MSED --setLR 0 RW password $1 >> msed_test.log
$MSED --query $1 >> msed_test.log
echo disable locking on the global range
$MSED --disableReadLockingGlobal --password password $1 >> msed_test.log
$MSED --disableWriteLockingGlobal --password password $1 >> msed_test.log
echo enable mbr shadowing
$MSED --setMBREnable on password $1 >> msed_test.log
$MSED --query $1 >> msed_test.log
echo set MBRDone
$MSED --setMBRDone ON password $1 >> msed_test.log
$MSED --query $1 >> msed_test.log
echo unset MBRDone
$MSED --setMBRDone off password $1 >> msed_test.log
$MSED --query $1 >> msed_test.log
echo disable mbr shadowing
$MSED --setMBREnable OFF password $1 >> msed_test.log
$MSED --query $1 >> msed_test.log
echo resetting device
##$MSED --revertnoerase passw0rd password $1 >> msed_test.log
##$MSED --query $1 >> msed_test.log
##$MSED --initialsetup passw0rd $1 >> msed_test.log
$MSED --revert passw0rd $1 >> msed_test.log
$MSED --query $1 >> msed_test.log
echo Thanks for running the test suite 
echo please e-mail msed_test.log to r0m30@r0m30.com
echo along with the OS, OS level and type of drive you have
read INPUT
exit