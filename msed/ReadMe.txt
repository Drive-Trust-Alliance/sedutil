This software is Copyright 2014 Michael Romeo <r0m30@r0m30.com>

    This file is part of msed.

    msed is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    msed is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with msed.  If not, see <http://www.gnu.org/licenses/>.

                               ***** Warning  ******
***** Warning  ****** PROPER USE OF CERTAIN FEATURES OF THIS PROGRAM 
***** Warning  ****** **WILL** CAUSE AN UNRECOVERABLE LOSS ALL OF THE DATA ON YOUR 
***** Warning  ****** DRIVE
***** Warning  ******
***** Warning  ****** A FAILURE OF AND/OR IMPROPER USE OF THIS PROGRAM 
***** Warning  ****** COULD CAUSE AN UNRECOVERABLE LOSS ALL OF THE DATA ON YOUR 
***** Warning  ****** DRIVE AND/OR PREVENT THE DRIVE FROM BEING USED AT 
***** Warning  ****** ALL EVER AGAIN
***** Warning  ******
***** Warning  ****** IF YOU ARE UNSURE OF WHAT YOU ARE DOING THEN 
***** Warning  ****** *PLEASE* GET SOME HELP USING THIS PROGRAM.
                               ***** Warning  ******
msed - Manage Self Encrypting Drives

This program and it's (tbd) accompanying Pre-Boot Authorization image allow
you to enable the locking in SED's that comply with the TCG OPAL 2.00
standard.   

You must be administrator/root to run this program

In Linux libata.allow_tpm must be set to 1. Either via adding libata.allow_tpm=1 to the kernel flags at boot time 
or changing the contents of /sys/module/libata/parameters/allow_tpm to a from a "0" to a "1" on a running system.

Currently the only operation that a general user would want to use is the PSID revert function
see linux/PSIDRevert_LINUX.txt or win32/PSIDRevert_WINDOWS.txt

Source code is available on GitHub at
Linux and Windows executables are available at //http:/r0m30.com/msed
