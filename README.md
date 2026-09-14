![alt tag](https://avatars0.githubusercontent.com/u/13870012?v=3&s=200)

This software is Copyright 2014-2017 Bright Plaza Inc. <drivetrust@drivetrust.com>

This file is part of sedutil.

sedutil is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

sedutil is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sedutil.  If not, see <http://www.gnu.org/licenses/>.


sedutil - The Drive Trust Alliance Self Encrypting Drive Utility

This program and its accompanying Pre-Boot Authorization image allow
you to enable the locking in SED's that comply with the TCG OPAL 2.00
standard on bios machines.   

You must be administrator/root to run the host management program

In Linux libata.allow_tpm must be set to 1 for SATA-based drives,
including NGFF/M.2 SATA drives.Either adding libata.allow_tpm=1
to the kernel flags at boot time or changing the contents of
/sys/module/libata/parameters/allow_tpm from a "0" to a "1" on
a running system if possible will accomplish this. NVMe drives
do not need this parameter.

***** sleep (S3) is not supported.

Source code is available on GitHub at https://github.com/Drive-Trust-Alliance/sedutil 

Linux and Windows executables are available at https://github.com/Drive-Trust-Alliance/sedutil/wiki/Executable-Distributions

If you are looking for the PSID revert function see linux/PSIDRevert_LINUX.txt or win32/PSIDRevert_WINDOWS.txt

PLEASE SEE CONTRIBUTING if you would like to make a code contribution.

If you are reporting a bug via an Issue or Pull request, 
if at all possible please donate an instance of the hardware manifesting the issue you are reporting
(but not already bricked)
to the maintenance effort.
