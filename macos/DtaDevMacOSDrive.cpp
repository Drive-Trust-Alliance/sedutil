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
#include "dirent.h"
#include "DtaEndianFixup.h"
#include "DtaHexDump.h"
#include "DtaDevMacOSDrive.h"
#include "DtaMacOSConstants.h"

namespace fs = std::__fs::filesystem;
io_connect_t DtaDevMacOSDrive::fdopen(const char * devref, io_registry_entry_t & dS, bool &isTPer)
{
    std::string bsdName = fs::path(devref).stem();
    io_registry_entry_t mediaService = findBSDName(bsdName.c_str());
    if (!mediaService)
        return false;

    io_registry_entry_t _dS;

    _dS = findDriverInParents(mediaService);
    isTPer = (_dS != IO_OBJECT_NULL);
    
    if (!isTPer) {
        _dS = findDriverSuperClassInParents(mediaService);
        if (_dS == IO_OBJECT_NULL) {
            IOObjectRelease(mediaService);
            return IO_OBJECT_NULL;
        } 
//        else {
//            fprintf(stderr, "Is not TPer; found driver superclass service in parents\n");
//        }
    } 
//    else {
//        fprintf(stderr, "Is TPer; found driver service in parents\n");
//    }
    IOObjectRelease(mediaService);
    dS = _dS;
    
    io_connect_t _c;
    kern_return_t  kernResult = OpenUserClient(dS, &_c);
    if (kernResult != kIOReturnSuccess || _c == IO_OBJECT_NULL) {
//        fprintf(stderr, "Failed to open user client -- error=0x%08X\n", kernResult);
        return IO_OBJECT_NULL;
    } 
    else {
//        fprintf(stderr,
//                "Driver service %u=0x%04x opened user client %u=0x%04x isTPer=%s\n",
//                _dS, _dS, _c, _c,
//                (isTPer ? "true" : "false"));
    }
    return _c;
}

void DtaDevMacOSDrive::fdclose()
{
    ReleaseIOObjects();
}

uint8_t DtaDevMacOSDrive::discovery0(DTA_DEVICE_INFO & disk_info) {
    return (isTPer)
            ? (KERN_SUCCESS == TPerUpdate(connection, driverService, &disk_info)) ? DTAERROR_SUCCESS : DTAERROR_COMMAND_ERROR
            :  DtaDevOSDrive::discovery0(disk_info);
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
        LOG(D1) << "Releasing connection";
        kern_return_t ret = CloseUserClient(connection);
        if ( kIOReturnSuccess != ret) {
            LOG(E) << "CloseUserClient returned " << ret;
        }
    }
    if ( driverService != IO_OBJECT_NULL ) {
        LOG(D1) << "Releasing driver service";
        IOObjectRelease(driverService);
    }
    ClearIOObjects();
}



using namespace std;
vector<string> DtaDevMacOSDrive::enumerateDtaDevMacOSDriveDevRefs()
{
  vector<string> devices;

  // MacOS drive names are disk0-disk99
  char devref[261];
  for (int i=0; i<=99; i++) {
      snprintf(devref,sizeof(devref),"/dev/disk%d",i);
      if (isDtaDevOSDriveDevRef(devref))
        devices.push_back(string(devref));
  }

  return devices;
}
