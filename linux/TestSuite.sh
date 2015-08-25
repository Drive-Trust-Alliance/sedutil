#!/bin/bash
if [ -$1- = -- ] ; then echo  device not specified; exit; fi 
echo  Drive $1 needs to be in a Inactive state 
echo  if you haven\'t already done so please do a revertnoerase/reverttper or 
echo  PSID revert on $1
echo  -
echo  Continuing with this test suite WILL ERASE ALL DATA ON $1
echo  If you dont want that to happen hit Ctrl-C now.
echo  press enter to continue or Ctrl-c to abort
read INOUT
echo  Last chance to hit Ctrl-c an keep the data on your drive
read INPUT
## test msed commands
MSED=./CLI/dist/Release_i686/GNU-Linux-x86/sedutil-cli
##MSED=./CLI/dist/Release_x86_64/GNU-Linux-x86/sedutil-cli
##MSED=./msed
##MSED=echo 
LOGFILE=msed_test.baseline
OUTPUTSINK=">>  ${LOGFILE} 2>&1"
echo  testing msed `date` > ${LOGFILE}

##
echo  Begin TestSuite.cmd > ${LOGFILE}
uname -a >> ${LOGFILE}
echo testing msed `date` | tee -a ${LOGFILE} 
${MSED} --help | grep -a Copyright >>  ${LOGFILE} 2>&1

${MSED} --scan >>  ${LOGFILE} 2>&1
echo Perform the initial setup | tee -a ${LOGFILE}
${MSED} --initialsetup passw0rd $1 >>  ${LOGFILE} 2>&1
${MSED} --query $1 | grep -a MediaEncrypt >>  ${LOGFILE} 2>&1
echo change the LSP Admin1 password | tee -a ${LOGFILE}
${MSED} --setAdmin1Pwd passw0rd password $1 >>  ${LOGFILE} 2>&1
${MSED} --enableLockingRange 0 password $1 >>  ${LOGFILE} 2>&1
${MSED} --query $1 | grep -a MediaEncrypt >>  ${LOGFILE} 2>&1
echo test readlocking | tee -a ${LOGFILE}
${MSED}  --setLockingRange 0 RO password $1 >>  ${LOGFILE} 2>&1
${MSED} --query $1 | grep -a MediaEncrypt >>  ${LOGFILE} 2>&1
echo test write locking | tee -a ${LOGFILE}
${MSED} --setLockingRange 0 RW password $1 >>  ${LOGFILE} 2>&1
${MSED} --query $1 | grep -a MediaEncrypt >>  ${LOGFILE} 2>&1
echo set LockingRange 0 LK | tee -a ${LOGFILE}
${MSED} --setLockingRange 0 lk password $1 >>  ${LOGFILE} 2>&1
${MSED} --query $1 | grep -a MediaEncrypt >>  ${LOGFILE} 2>&1
echo disable locking on the global range | tee -a ${LOGFILE}
${MSED} --disableLockingRange 0 password $1 >>  ${LOGFILE} 2>&1
${MSED} --query $1 | grep -a MediaEncrypt >>  ${LOGFILE} 2>&1
echo unset MBRDone | tee -a ${LOGFILE}
${MSED} --setMBRDone OFF password $1 >>  ${LOGFILE} 2>&1
${MSED} --query $1 | grep -a MediaEncrypt >>  ${LOGFILE} 2>&1
echo disable mbr shadowing | tee -a ${LOGFILE}
${MSED} --setMBREnable off password $1 >>  ${LOGFILE} 2>&1
${MSED} --query $1 | grep -a MediaEncrypt >>  ${LOGFILE} 2>&1
echo resetting device | tee -a ${LOGFILE}
${MSED} --revertnoerase password $1 >>  ${LOGFILE} 2>&1
${MSED} --query $1 | grep -a MediaEncrypt >>  ${LOGFILE} 2>&1
${MSED} --reverttper passw0rd $1 >>  ${LOGFILE} 2>&1
${MSED} --query $1 | grep -a MediaEncrypt >>  ${LOGFILE} 2>&1
${MSED} --validatePBKDF2 >>  ${LOGFILE} 2>&1
echo  Thanks for running the test suite 
echo  please e-mail ${LOGFILE} to r0m30@r0m30.com
echo  along with the OS, OS level and type of drive you have
exit
