/* C:B**************************************************************************
This software is Copyright 2014-2016 Bright Plaza Inc. <drivetrust@drivetrust.com>

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

 * C:E********************************************************************** */
#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include "DtaConstants.h"
#include "log.h"
// Why can't I find these??
#define TRUE 1
#define FALSE 0
// a few OS specific methods that need to be worked out
#define SNPRINTF snprintf
// Cumbersome, but trying to avoid editing common files. If /sys/block is not present, reverts to enumerating /dev/sd* devices. 
#define DEVICEMASK switch(DtaDevOS::getNextDevice(i)){ case 1  : snprintf(devname,23,"/dev/%s",DtaDevOS::getDeviceName()); break; case -1 : snprintf(devname,23,"/dev/sd%c",(char) 0x61+i); break; default : snprintf(devname,23,"/dev/sdX"); break; }
#define DEVICEEXAMPLE "/dev/sdc"
