/* C:B**************************************************************************
This software is Copyright 2014, 2022 Bright Plaza Inc. <drivetrust@drivetrust.com>

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
#include "os.h"
#include "os.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fnmatch.h>
#include <fcntl.h>
#include <sys/ioctl.h>
// #include <scsi/sg.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
// #include <linux/hdreg.h>
#include <errno.h>
#include <map>
#include <vector>
#include <fstream>
#include <mach/mach_port.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/scsi/SCSITaskLib.h>
//#include <SEDKernelInterface/SEDKernelInterface.h>
#include "TPerKernelInterface.h"
#include "PrintBuffer.h"
#include "RegistryUtilities.h"
#include "DtaMacOSConstants.h"
#include "DtaDevMacOSDrive.h"
#include <map>
#include <string>
#include <vector>

using namespace std;

/** The Device class represents a single disk device.
 *  Mac specific implementation using the USB interface
 */

bool DtaDevMacOSDrive::findBrightPlazaDriverService(const char * dev)
{
    io_registry_entry_t mediaService = findBSDName(dev);
    if (!mediaService)
        return false;
    driverService = findBrightPlazaDriverInParents(mediaService);
    IOObjectRelease(mediaService);
    if (driverService == IO_OBJECT_NULL) {
        return false;
    }
    ownDriverService=true;
    return true;
}


uint8_t DtaDevMacOSDrive::connectToUserClient()
{
    if (driverService == IO_OBJECT_NULL) {
        return DTAERROR_DRIVER_NOT_INSTALLED;
    }
    kern_return_t  kernResult = OpenUserClient(driverService, &connect);
    if (kernResult != kIOReturnSuccess || connect == IO_OBJECT_NULL) {
        fprintf(stderr, "Failed to open user client -- error =0x%08X\n", kernResult);
        return DTAERROR_DRIVER_CONNECTION_FAILED;
    }
    ownConnect=true;
    return 0 ;
}

bool DtaDevMacOSDrive::init(const char * dev, bool doConnect)
{
    return findBrightPlazaDriverService(dev) && (!doConnect || KERN_SUCCESS==connectToUserClient());
}

void DtaDevMacOSDrive::init(io_registry_entry_t ds, io_connect_t c)
{
    ClearOwnedIOObjects();
    driverService=ds;
    connect=c;
}

uint8_t DtaDevMacOSDrive::GetIdentifyResponseFromIORegistry(uint8_t * response)
{
    CFDataRef data = (CFDataRef)IORegistryEntryCreateCFProperty(driverService,
                                                                CFSTR( IOIdentifyDeviceResponseKey),
                                                                kCFAllocatorDefault, 0);
    if (data) {
        CFDataGetBytes(data, CFRangeMake(0,CFDataGetLength(data)), response);
        CFRelease(data);
        return KERN_SUCCESS;
    }
    return KERN_FAILURE;
}

static void ReformatIdentifyResponseAsDiskInfo(uint8_t * response, DTA_DEVICE_INFO& disk_info)
{
    IDENTIFY_RESPONSE & id = * (IDENTIFY_RESPONSE *) response;
    disk_info.devType = DEVICE_TYPE_USB; // TODO -- generalize for other devices when they are supported by BPTperDriver
    for (size_t i = 0; i < sizeof (disk_info.serialNum); i += 2) {         // uint8_t[20]
        disk_info.serialNum[i] = id.serialNum[i + 1];
        disk_info.serialNum[i + 1] = id.serialNum[i];
    }
    for (size_t i = 0; i < sizeof (disk_info.firmwareRev); i += 2) {       // uint8_t[8]
        disk_info.firmwareRev[i] = id.firmwareRev[i + 1];
        disk_info.firmwareRev[i + 1] = id.firmwareRev[i];
    }
    for (size_t i = 0; i < sizeof (disk_info.modelNum); i += 2) {          // uint8_t[40]
        disk_info.modelNum[i] = id.modelNum[i + 1];
        disk_info.modelNum[i + 1] = id.modelNum[i];
    }
}

uint8_t DtaDevMacOSDrive::identify(DTA_DEVICE_INFO& disk_info )
{
    uint8_t response[IDENTIFY_RESPONSE_SIZE]={0};
    
    LOG(D4) << "Entering DtaDevMacOSDrive::identify()";
    if (KERN_SUCCESS == GetIdentifyResponseFromIORegistry(response) ){
        ReformatIdentifyResponseAsDiskInfo(response, disk_info);
        return KERN_SUCCESS;
    }
    return KERN_FAILURE;
}

uint8_t DtaDevMacOSDrive::GetD0ResponseFromIORegistry(uint8_t * response)
{
    kern_return_t ret = updateLockingPropertiesInIORegistry(connect);
    if (KERN_SUCCESS != ret)
        return (uint8_t)ret;
    return Discovery0(driverService, response) ? KERN_SUCCESS : KERN_FAILURE;
}


uint8_t DtaDevMacOSDrive::discovery0(uint8_t * response)
{
    return GetD0ResponseFromIORegistry(response);
}


/** Close the device reference so this object can be delete. */
void DtaDevMacOSDrive::ClearOwnedIOObjects()
{
    connect = IO_OBJECT_NULL;
    ownConnect = false;
    driverService = IO_OBJECT_NULL;
    ownDriverService = false;
}


/** Close the device reference so this object can be delete. */
void DtaDevMacOSDrive::ReleaseOwnedIOObjects()
{
    if ( connect && ownConnect ) {
        LOG(D1) << "Releasing owned connection";
        kern_return_t ret = CloseUserClient(connect);
        if ( kIOReturnNotOpen != ret) {
            LOG(D) << "CloseUserClient returned " << ret;
        }
    }
    if ( driverService && ownDriverService ) {
        LOG(D1) << "Releasing owned driver service";
        IOObjectRelease(driverService);
    }
    ClearOwnedIOObjects();
}


/** Close the device reference so this object can be delete. */
DtaDevMacOSDrive::~DtaDevMacOSDrive()
{
    LOG(D1) << "Destroying DtaDevMacOSDrive";
    ReleaseOwnedIOObjects();
}
