/* C:B**************************************************************************
This software is © 2014 Bright Plaza Inc. <drivetrust@drivetrust.com>

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
#include <arpa/inet.h>

// Why can't I find these??
#define TRUE 1
#define FALSE 0
// a few OS specific methods that need to be worked out
#define SNPRINTF snprintf
#define DEVICEMASK snprintf(devname,23,"/dev/sd%c",(char) 0x61+i)
#define DEVICEMASKN snprintf(devname,23,"/dev/nvme%i",j)
#define DEVICEEXAMPLE "/dev/sdc"

#define  __unimplemented__ {throw __PRETTY_FUNCTION__;}


/** OS specific command to Wait for specified number of milliseconds
 * @param milliseconds  number of milliseconds to wait
 */
static inline void osmsSleep(uint32_t milliseconds) {
    (void)usleep(milliseconds * 1000);
}

typedef void * OSDEVICEHANDLE;
#define INVALID_HANDLE_VALUE (reinterpret_cast<OSDEVICEHANDLE>( -1 ))

#define handle(descriptor)(reinterpret_cast<OSDEVICEHANDLE>(static_cast<uintptr_t>(descriptor)))
#define handleDescriptor(handle) (static_cast<int>(static_cast<unsigned int>(reinterpret_cast<uintptr_t>(handle))))
