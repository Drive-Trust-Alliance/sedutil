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
#include "os.h"

#include <IOKit/IOKitLib.h>
#include <IOKit/scsi/SCSITaskLib.h>

#include <SEDKernelInterface/SEDKernelInterface.h>

#include "DtaMacOSConstants.h"
#include "DtaDevMacOSTPer.h"
#include "DtaDevMacOSTPer_SAT.h"
#include "DtaDevMacOSTPer_SCSI.h"

using namespace std;

/** The Device class represents a single disk device.
 *  Mac specific implementation using the USB interface
 */

// https://gist.github.com/peter-bloomfield/1b228e2bb654702b1e50ef7524121fb9#file-cfstringtostdstring-cpp

#include <CoreFoundation/CoreFoundation.h>
#include <string>
#include <vector>

/**
 * Converts a CFString to a UTF-8 std::string if possible.
 *
 * @param input A reference to the CFString to convert.
 * @return Returns a std::string containing the contents of CFString converted to UTF-8. Returns
 *  an empty string if the input reference is null or conversion is not possible.
 */
std::string cfStringToStdString(CFStringRef input)
{
    if (!input)
        return {};

    // Attempt to access the underlying buffer directly. This only works if no conversion or
    //  internal allocation is required.
    auto originalBuffer{ CFStringGetCStringPtr(input, kCFStringEncodingUTF8) };
    if (originalBuffer)
        return originalBuffer;

    // Copy the data out to a local buffer.
    auto lengthInUtf16{ CFStringGetLength(input) };
    unsigned long maxLengthInUtf8 = (unsigned long)CFStringGetMaximumSizeForEncoding(lengthInUtf16,
        kCFStringEncodingUTF8) + 1 ; // <-- leave room for null terminator
    std::vector<char> localBuffer(maxLengthInUtf8);

    if (CFStringGetCString(input, localBuffer.data(), (CFIndex)maxLengthInUtf8, (unsigned int)maxLengthInUtf8))
        return localBuffer.data();

    return {};
}


#define GetString(dict,name) (CFStringRef)CFDictionaryGetValue(dict, CFSTR(name))

#if defined(TRY_SMART_LIBS)
DtaDevMacOSTPer * DtaDevMacOSTPer::getTPer(io_service_t aBlockStorageDevice,
                                           std::string entryName,
                                           std::string bsdName,
                                           CFDictionaryRef tPerProperties,
                                           CFDictionaryRef properties,
                                           DTA_DEVICE_INFO * pdi)
{
    const CFStringRef interfaceType=GetString(tPerProperties, IOInterfaceTypeKey);
    if (CFEqual(interfaceType, CFSTR(IOInterfaceTypeSAT))) {
        return new DtaDevMacOSTPer_SAT(aBlockStorageDevice, entryName, bsdName, properties, pdi);
    } else if (CFEqual(interfaceType, CFSTR(IOInterfaceTypeSCSI))) {
        return new DtaDevMacOSTPer_SCSI(aBlockStorageDevice, entryName, bsdName, properties, pdi);
    } else {
        return nil;
    }
}
#else // !defined(TRY_SMART_LIBS)
DtaDevMacOSTPer * DtaDevMacOSTPer::getTPer(std::string entryName,
                                           std::string bsdName,
                                           CFDictionaryRef tPerProperties,
                                           CFDictionaryRef properties,
                                           DTA_DEVICE_INFO * pdi)
{
    const CFStringRef interfaceType=GetString(tPerProperties, IOInterfaceTypeKey);
    if (CFEqual(interfaceType, CFSTR(IOInterfaceTypeSAT))) {
        return new DtaDevMacOSTPer_SAT(entryName, bsdName, properties, pdi);
    } else if (CFEqual(interfaceType, CFSTR(IOInterfaceTypeSCSI))) {
        return new DtaDevMacOSTPer_SCSI(entryName, bsdName, properties, pdi);
    } else {
        return nil;
    }
}
#endif // defined(TRY_SMART_LIBS)

bool DtaDevMacOSTPer::findBrightPlazaDriverService(const char * dev)
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


uint8_t DtaDevMacOSTPer::connectToUserClient()
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
    return KERN_SUCCESS;
}

bool DtaDevMacOSTPer::init(const char * dev, bool doConnect)
{
    return findBrightPlazaDriverService(dev) && (!doConnect || KERN_SUCCESS==connectToUserClient());
}

void DtaDevMacOSTPer::init(io_registry_entry_t ds, io_connect_t c)
{
    ClearOwnedIOObjects();
    driverService=ds;
    connect=c;
}

uint8_t DtaDevMacOSTPer::identify(DTA_DEVICE_INFO& disk_info )
{
    LOG(D4) << "Entering DtaDevMacOSDrive::identify()";
    return TPerUpdate(connect, &disk_info) == KERN_SUCCESS ? KERN_SUCCESS : KERN_FAILURE;
}


/** Close the device reference so this object can be delete. */
void DtaDevMacOSTPer::ClearOwnedIOObjects()
{
    connect = IO_OBJECT_NULL;
    ownConnect = false;
    driverService = IO_OBJECT_NULL;
    ownDriverService = false;
}


/** Close the device reference so this object can be delete. */
void DtaDevMacOSTPer::ReleaseOwnedIOObjects()
{
    if ( connect && ownConnect ) {
        LOG(D1) << "Releasing owned connection";
        CloseUserClient(connect);
    }
    if ( driverService && ownDriverService ) {
        LOG(D1) << "Releasing owned driver service";
        IOObjectRelease(driverService);
    }
    ClearOwnedIOObjects();
}


/** Close the device reference so this object can be delete. */
DtaDevMacOSTPer::~DtaDevMacOSTPer()
{
    LOG(D1) << "Destroying DtaDevMacOSDrive";
    ReleaseOwnedIOObjects();
}
