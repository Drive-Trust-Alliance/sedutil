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
echo testing msed $date @ $time > msed_test.log
uname -a >> msed_test.log
$MSED -V $1 >> msed_test.log
$MSED -s >> msed_test.log
$MSED -q $1 >> msed_test.log
echo take ownership and activate the Locking SP
$MSED -t -p passw0rd $1 >> msed_test.log
$MSED -l -p passw0rd $1 >> msed_test.log
echo change the LSP Admin1 password
$MSED -S -p passw0rd -u Admin1 -n password $1 >> msed_test.log
echo enable read/write locking on the global range
$MSED -a -p password $1 >> msed_test.log
$MSED -b -p password $1 >> msed_test.log
$MSED -q $1 >> msed_test.log
echo test readlocking
$MSED -f -p password $1 >> msed_test.log
$MSED -q $1 >> msed_test.log
$MSED -i -p password $1 >> msed_test.log
$MSED -q $1 >> msed_test.log
echo test write locking
$MSED -g -p password $1 >> msed_test.log
$MSED -q $1 >> msed_test.log
$MSED -j -p password $1 >> msed_test.log
echo disable locking on the global range
$MSED -c -p password $1 >> msed_test.log
$MSED -d -p password $1 >> msed_test.log
echo enable mbr shadowing
$MSED -y -p password $1 >> msed_test.log
$MSED -q $1 >> msed_test.log
echo set MBRDone
$MSED -z -p password $1 >> msed_test.log
$MSED -q $1 >> msed_test.log
echo unset MBRDone
$MSED -x -p password $1 >> msed_test.log
$MSED -q $1 >> msed_test.log
echo disable mbr shadowing
$MSED -w -p password $1 >> msed_test.log
$MSED -q $1 >> msed_test.log
$MSED -L -p password $1 >> msed_test.log
$MSED -T -p passw0rd $1 >> msed_test.log
$MSED -q $1 >> msed_test.log
echo Thanks for running the test suite 
echo please e-mail msed_test.log to r0m30@r0m30.com
echo along with the OS, OS level and type of drive you have
read INPUT
exit