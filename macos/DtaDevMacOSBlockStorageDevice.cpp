/* C:B**************************************************************************
   This software is Copyright (c) 2014-2024 Bright Plaza Inc. <drivetrust@drivetrust.com>

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
#include <vector>
#include <fstream>
#include <map>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
/***
#include <scsi/sg.h>
#include <linux/hdreg.h>
 ***/
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fnmatch.h>


#include "log.h"
#include "os.h"

#include <SEDKernelInterface/SEDKernelInterface.h>

#include "DtaDevMacOSBlockStorageDevice.h"


bool DtaDevMacOSBlockStorageDevice::isDtaDevMacOSBlockStorageDeviceDevRef(const char * devref) {
  return ( 0 == fnmatch("/dev/disk[0-9]", devref, 0) )
     ||  ( 0 == fnmatch("/dev/disk[1-9][0-9]", devref, 0) );
}

/** Send an ioctl to the device using pass through. */
uint8_t DtaDevMacOSBlockStorageDevice::sendCmd(ATACOMMAND /* cmd */, uint8_t /* protocol */, uint16_t /* comID */,
                                 void * /* buffer */, unsigned int /* bufferlen */)
{
  LOG(D4) << "Entering DtaDevMacOSBlockStorageDevice::sendCmd";


  LOG(D4) << "Returning 0xff from DtaDevMacOSBlockStorageDevice::sendCmd";
  return 0xff;
}

void DtaDevMacOSBlockStorageDevice::BlockStorageDeviceUpdate(io_registry_entry_t driverService, DTA_DEVICE_INFO& disk_info){
    (void)driverService;
    (void)disk_info;

}

bool DtaDevMacOSBlockStorageDevice::identify(DTA_DEVICE_INFO& disk_info)
{
    (void)disk_info;  // TODO: fill this in from the I/O registry;
    return true;
}
