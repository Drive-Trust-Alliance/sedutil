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

#include <SEDKernelInterface/SEDKernelInterface.h>
#include "log.h"
#include "DtaDevOSDrive.h"
#include "DtaDevMacOSDrive.h"
#include "DtaDevMacOSBlockStorageDevice.h"

/** Factory functions
 *
 * Static class members of DtaDevOSDrive that are passed through
 * to DtaDevMacOSDrive
 *
 */


bool DtaDevOSDrive::isDtaDevOSDriveDevRef(const char * devref) {
  return DtaDevMacOSDrive::isDtaDevMacOSDriveDevRef(devref);
}

std::vector<std::string> DtaDevOSDrive::enumerateDtaDevOSDriveDevRefs() {
  return DtaDevMacOSDrive::enumerateDtaDevMacOSDriveDevRefs();
}

DtaDevOSDrive * DtaDevOSDrive::getDtaDevOSDrive(const char * devref,
                                                DTA_DEVICE_INFO &disk_info)
{
  return static_cast<DtaDevOSDrive *>(DtaDevMacOSDrive::getDtaDevMacOSDrive(devref, disk_info));
}


#if defined(NVME)
#include "DtaDevMacOSNvme.h"
#endif // defined(NVME)
#include "DtaDevMacOSScsi.h"
#include "DtaDevMacOSSata.h"


/** Factory functions
 *
 * Static class members that support instantiation of subclass members
 * with the subclass switching logic localized here for easier maintenance.
 *
 */

DtaDevMacOSDrive * DtaDevMacOSDrive::getDtaDevMacOSDrive(const char * devref,
                                                         DTA_DEVICE_INFO &disk_info)
{
    disk_info.devType = DEVICE_TYPE_OTHER;
    
    io_registry_entry_t driverService;
    io_connect_t connection = fdopen(devref, driverService);
    
    LOG(D4)
    << "getDtaDevMacOSDrive opening device " << devref
    << " "
    << "with driverService " << driverService
    << " "
    << "as connection " << connection
    ;
    //    fprintf(stderr, "isTper=%s\n", isTPer ? "true" : "false");
    
    // Best case: devref -> TPer driver
    if (connection != IO_OBJECT_NULL) {
        kern_return_t ret = TPerUpdate(connection, driverService, &disk_info);  // a/k/a discovery0, done by the driver
        if (ret!=KERN_SUCCESS)
        {
            fprintf(stderr, "TPerUpdate failed with return code %u=0x%08x", ret, ret);
            IOObjectRelease(driverService);
            CloseUserClient(connection);
            return NULL;
        }
        
        switch (disk_info.devType)
        {
            case DEVICE_TYPE_SAS:
                return new DtaDevMacOSScsi(driverService, connection);
            case DEVICE_TYPE_SATA:
                return new DtaDevMacOSSata(driverService, connection);
            default:
                LOG(E)
                << "TPerUpdate failed with unexpected di.devtype "
                << disk_info.devType << "=0x" << std::hex << std::setw(8) << disk_info.devType;
                IOObjectRelease(driverService);
                CloseUserClient(connection);
                return NULL;
        }
    }

    // OK case: devref -> BlockStorageDevice -- can't do useful I/O, but the I/O Registry can fill in useful info
    if (driverService != IO_OBJECT_NULL &&
        DtaDevMacOSBlockStorageDevice::BlockStorageDeviceUpdate(driverService, disk_info)) {
            return new DtaDevMacOSBlockStorageDevice(driverService);
    }
    
    return NULL;
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
