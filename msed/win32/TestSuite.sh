#!/bin/bash
if [ -$1- = -- ] ; then echo  device not specified; exit; fi 
echo  Drive $1 needs to be in a Reverted state 
echo  if you haven\'t already done so please do a PSID revert
echo  on $1
echo  -
echo  Continuing with this test suite WILL ERASE ALL DATA ON $1
echo  If you dont want that to happen hit Ctrl-C now.
echo  press enter to continue or Ctrl-c to abort
read INOUT
echo  Last chance to hit Ctrl-c an keep the data on your drive
read INPUT
## test msed commands
MSED=../../Win32/Release/msed.exe
##MSED=../../x64/Release/msed.exe
##MSED=./msed,exe
##MSED=echo 
echo  testing msed `date` > ${LOGFILE}

LOGFILE=msed_test.baseline
OUTPUTSINK=">>  ${LOGFILE} 2>&1"
NIXGREP="| grep -a MBREnable "
##
echo  Begin TestSuite.cmd > ${LOGFILE}
uname -a >> ${LOGFILE}
echo testing msed `date` | tee -a ${LOGFILE} 
${MSED} --scan >>  ${LOGFILE} 2>&1
echo Perform the initial setup | tee -a ${LOGFILE}
${MSED} --initialsetup passw0rd $1 >>  ${LOGFILE} 2>&1
${MSED} --query $1 | grep -a MBREnable >>  ${LOGFILE} 2>&1
echo change the LSP Admin1 password | tee -a ${LOGFILE}
${MSED} --setAdmin1Pwd passw0rd password $1 >>  ${LOGFILE} 2>&1
echo test readlocking | tee -a ${LOGFILE}
${MSED}  --setLockingRange 0 RO password $1 >>  ${LOGFILE} 2>&1
${MSED} --query $1 | grep -a MBREnable >>  ${LOGFILE} 2>&1
${MSED} --disableLockingRange 0 password $1 >>  ${LOGFILE} 2>&1
${MSED} --query $1 | grep -a MBREnable >>  ${LOGFILE} 2>&1
${MSED} --enableLockingRange 0 password $1 >>  ${LOGFILE} 2>&1
echo test write locking | tee -a ${LOGFILE}
${MSED} --setLockingRange 0 RW password $1 >>  ${LOGFILE} 2>&1
${MSED} --query $1 | grep -a MBREnable >>  ${LOGFILE} 2>&1
echo set LockingRange 0 LK | tee -a ${LOGFILE}
${MSED} --setLockingRange 0 lk password $1 >>  ${LOGFILE} 2>&1
${MSED} --query $1 | grep -a MBREnable >>  ${LOGFILE} 2>&1
echo disable locking on the global range | tee -a ${LOGFILE}
${MSED} --disableLockingRange 0 password $1 >>  ${LOGFILE} 2>&1
${MSED} --query $1 | grep -a MBREnable >>  ${LOGFILE} 2>&1
echo enable mbr shadowing | tee -a ${LOGFILE}
${MSED} --setMBREnable on password $1 >>  ${LOGFILE} 2>&1
${MSED} --query $1 | grep -a MBREnable >>  ${LOGFILE} 2>&1
echo set MBRDone | tee -a ${LOGFILE}
${MSED} --setMBRDone ON password $1 >>  ${LOGFILE} 2>&1
${MSED} --query $1 | grep -a MBREnable >>  ${LOGFILE} 2>&1
echo unset MBRDone | tee -a ${LOGFILE}
${MSED} --setMBRDone off password $1 >>  ${LOGFILE} 2>&1
${MSED} --query $1 | grep -a MBREnable >>  ${LOGFILE} 2>&1
echo disable mbr shadowing | tee -a ${LOGFILE}
${MSED} --setMBREnable OFF password $1 >>  ${LOGFILE} 2>&1
${MSED} --query $1 | grep -a MBREnable >>  ${LOGFILE} 2>&1
echo resetting device | tee -a ${LOGFILE}
${MSED} --reverttper passw0rd $1 >>  ${LOGFILE} 2>&1
${MSED} --query $1 | grep -a MBREnable >>  ${LOGFILE} 2>&1
${MSED} --validatePBKDF2 >>  ${LOGFILE} 2>&1
echo  Thanks for running the test suite 
echo  please e-mail ${LOGFILE} to r0m30@r0m30.com
echo  along with the OS, OS level and type of drive you have
exit