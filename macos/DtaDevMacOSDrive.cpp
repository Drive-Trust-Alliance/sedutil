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

#include <cstdint>
#include <cstring>
#include <filesystem>
#include <SEDKernelInterface/SEDKernelInterface.h>
#include "os.h"
#include "log.h"
#include "fnmatch.h"
#include "DtaEndianFixup.h"
#include "DtaHexDump.h"
#include "DtaDevMacOSDrive.h"
#include "DtaMacOSConstants.h"


  
bool DtaDevMacOSDrive::isDtaDevMacOSDriveDevRef(const char * devref) {
  return ( 0 == fnmatch("/dev/disk[0-9]", devref, 0) )
     ||  ( 0 == fnmatch("/dev/disk[1-9][0-9]", devref, 0) );
}

namespace fs = std::__fs::filesystem;
#undef USEDRIVERUSPERCLASS
#define USEBLOCKSTORAGEDEVICE
io_connect_t DtaDevMacOSDrive::fdopen(const char * devref, io_registry_entry_t & dS)
{
    std::string bsdName = fs::path(devref).stem();
    io_registry_entry_t mediaService = findBSDName(bsdName.c_str());
    if (!mediaService)
        return false;

    dS = findDriverInParents(mediaService);
    if (dS != IO_OBJECT_NULL) {
        LOG(D4) << "Is TPer; found driver service in parents";
        io_connect_t _c = IO_OBJECT_NULL;
        kern_return_t  kernResult = OpenUserClient(dS, &_c);
        if (kernResult != kIOReturnSuccess || _c == IO_OBJECT_NULL) {
          LOG(E) << "Failed to open user client -- error=0x" << std::hex << std::setw(8) << kernResult;
        }
        else {
          LOG(D4) 
            << "Driver service " << dS << "=0x" << std::hex << std::setw(4) << dS << std::dec
            << " "
            << "-- opened user client " << _c << "=0x" << std::hex << std::setw(4) << _c;
        }
        IOObjectRelease(mediaService);
        return _c;
    }

#if defined(USEDRIVERUSPERCLASS)
    dS = findDriverSuperClassInParents(mediaService);
    if (dS != IO_OBJECT_NULL)  {
        LOG(D4) << "Is not TPer; found block storage device service in parents";
        LOG(D4) << "Driver service " << dS << "=0x" << std::hex << std::setw(4) << dS << std::dec
                << " "
                << "-- did not open user client --"
                << " "
                << "isTPer=false";
        IOObjectRelease(mediaService);
        return IO_OBJECT_NULL;
    }
#endif // defined(USEDRIVERUSPERCLASS)

#if defined(USEBLOCKSTORAGEDEVICE)
    dS = findBlockStorageDeviceInParents(mediaService);
    if (dS != IO_OBJECT_NULL)  {
        LOG(D4) << "Is not TPer; found block storage device service in parents";
        LOG(D4) 
          << "Driver service " << dS << "=0x" << std::hex << std::setw(4) << dS << std::dec
          << " "
          << "-- did not open user client"
        ;
        IOObjectRelease(mediaService);
        return IO_OBJECT_NULL;
    }

    IOObjectRelease(mediaService);
    return IO_OBJECT_NULL;
#endif // defined(USEBLOCKSTORAGEDEVICE)
}

void DtaDevMacOSDrive::fdclose()
{
    ReleaseIOObjects();
}

uint8_t DtaDevMacOSDrive::discovery0(DTA_DEVICE_INFO & disk_info) {
    return (KERN_SUCCESS == TPerUpdate(connection, driverService, &disk_info)) ? DTAERROR_SUCCESS : DTAERROR_COMMAND_ERROR;
}


/** Close the device reference so this object can be delete. */
void DtaDevMacOSDrive::ClearIOObjects()
{
    connection = IO_OBJECT_NULL;
    driverService = IO_OBJECT_NULL;
}


/** Close the device reference so this object can be delete. */
void DtaDevMacOSDrive::ReleaseIOObjects()
{
    if ( connection != IO_OBJECT_NULL ) {
        LOG(D4) << "Releasing connection";
        kern_return_t ret = CloseUserClient(connection);
        if ( kIOReturnSuccess != ret) {
            LOG(E) << "CloseUserClient returned " << ret;
        }
    }
    if ( driverService != IO_OBJECT_NULL ) {
        LOG(D4) << "Releasing driver service";
        IOObjectRelease(driverService);
    }
    ClearIOObjects();
}



using namespace std;
vector<string> DtaDevMacOSDrive::enumerateDtaDevMacOSDriveDevRefs()
{
  /**
   *  The code below does not work on MacOS because of container disks, which result in more than one
   *  disknn being assigned to the same physical device.  Instead we must abstain implementation at this
   *  level and instead ask DtaDevMacOSBlockStorageDevice to enumerate.
   */

#if defined(EACH_DEV_DISK_CORRESPONDS_TO_AT_MOST_ONE_BLOCK_STORAGE_DEVICE)
    vector<string> devices;

  // MacOS drive names are disk0-disk99
  char devref[261];
  for (int i=0; i<=99; i++) {
      snprintf(devref,sizeof(devref),"/dev/disk%d",i);
      if (isDtaDevOSDriveDevRef(devref))
        devices.push_back(string(devref));
  }

    return devices;
#endif // defined(EACH_DEV_DISK_CORRESPONDS_TO_AT_MOST_ONE_BLOCK_STORAGE_DEVICE)
    
    return DtaDevMacOSBlockStorageDevice::enumerateDtaDevMacOSBlockStorageDeviceDevRefs();
    
}
