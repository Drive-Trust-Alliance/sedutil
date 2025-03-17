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

#include <filesystem>
#include "log.h"
#include "fnmatch.h"
#include <SEDKernelInterface/SEDKernelInterface.h>
#include <IOKit/storage/IOBlockStorageDevice.h>
#include <IOKit/storage/IOBlockStorageDriver.h>
#include <mach/mach_error.h>

#include "DtaMacOS.h"
#include "DtaMacOSBlockStorageDevice.h"
#include "DtaHexDump.h"
#include "NVMeStructures.h"

const std::string DtaOS::name="MacOS";
DtaOS * DtaOS::getDtaOS () { return new DtaMacOS(); }

OSDEVICEHANDLE DtaMacOS::openDeviceHandle(const char * devref, bool & accessDenied){
    LOG(D4) << "openDeviceHandle(\"" << devref << "\", _)";
    std::string bsdName = std::filesystem::path(devref).stem();
    if (bsdName.rfind("/dev/",0)==0)
        bsdName=bsdName.substr(5,bsdName.length());

    io_registry_entry_t mediaService = findBSDName(bsdName.c_str());
    if (!mediaService) {
        LOG(D4) << "could not find media service for bsdName=\"" << bsdName << "\"";
        return INVALID_HANDLE_VALUE;
    }
    LOG(D4) << "found media service for bsdName=\"" << bsdName << "\"";

    /**
     *
     *  For real devices, under the current regime, at least on the X86 machine I'm looking at right now, has to go
     *
     * IOSCSIPeripheralDeviceType00 or @kDriverClass or maybe even something else
     *  IOBlockStorageDevice
     *    IOBlockStorageDriver
     *      IOMedia    <-- which we just found by bsdName
     *
     *  Bogus containers, nested storage, etc. don't have the three-layer sandwich directly ending at IOMedia
     *
     **/
    io_registry_entry_t blockStorageDriverService=findParent(mediaService);
    IOObjectRelease(mediaService);
    if (!IOObjectConformsTo(blockStorageDriverService, kIOBlockStorageDriverClass)) {
        LOG(D4) << "parent of media service is not block storage driver service";
        IOObjectRelease(blockStorageDriverService);
        return INVALID_HANDLE_VALUE;
    }
    LOG(D4) << "parent of media service is block storage driver service";

    io_registry_entry_t blockStorageDeviceService=findParent(blockStorageDriverService);
    IOObjectRelease(blockStorageDriverService);
    if (!IOObjectConformsTo(blockStorageDeviceService, kIOBlockStorageDeviceClass)) {
        LOG(D4) << "parent of block storage driver service is not block storage device service";
        IOObjectRelease(blockStorageDeviceService);
        return INVALID_HANDLE_VALUE;
    }
    LOG(D4) << "parent of block storage driver service is block storage device service";

    io_connect_t connection=IO_OBJECT_NULL;
    kern_return_t kernResult=KERN_FAILURE;
    io_service_t possibleTPer=findParent(blockStorageDeviceService);
    LOG(D4) << "Device service "              << HEXOFF << blockStorageDeviceService << "=" << HEXON(4) << blockStorageDeviceService
            << " candidate TPer instance " << HEXOFF << possibleTPer              << "=" << HEXON(4) << possibleTPer;
    if (!IOObjectConformsTo(possibleTPer, kDriverClass)) {
        LOG(D4) << "parent of block storage device service is not TPer Driver instance";
    } else if (((kernResult = OpenUserClient(possibleTPer, &connection)) != kIOReturnSuccess || connection == IO_OBJECT_NULL)) {
        if (kernResult == (kern_return_t(0xE00002C2))) {
            LOG(D4) << "OpenUserClient denied access -- (" << mach_error_string(kernResult) << ")";;
            accessDenied = true;
        } else {
            LOG(E) << "Failed to open user client" << " --"
                   << " "
                   << "error=" << HEXON(8) << kernResult
                   << " "
                   << "(" << mach_error_string(kernResult) << ")";
        }
    } else {
        LOG(D4) << "Device service "              << HEXOFF << blockStorageDeviceService << "=" << HEXON(4) << blockStorageDeviceService
        << " connected to TPer instance " << HEXOFF << possibleTPer              << "=" << HEXON(4) << possibleTPer
        << " opened user client "         << HEXOFF << connection                << "=" << HEXON(4) << connection;
    }
    IOObjectRelease(possibleTPer);
    LOG(D4) << "Device service "              << HEXOFF << blockStorageDeviceService << "=" << HEXON(4) << blockStorageDeviceService
            << " released candidate TPer instance " << HEXOFF << possibleTPer              << "=" << HEXON(4) << possibleTPer;
    OSDEVICEHANDLE result = handle(blockStorageDeviceService,connection);

    if (accessDenied) {
        closeDeviceHandle(result);
        return INVALID_HANDLE_VALUE;
    }

    return result;
}

void DtaMacOS::closeDeviceHandle(OSDEVICEHANDLE osDeviceHandle){
    if (osDeviceHandle == INVALID_HANDLE_VALUE) return;

    io_registry_entry_t connection = handleConnection(osDeviceHandle);
    if ( connection != IO_OBJECT_NULL ) {
        LOG(D4) << "Releasing connection";
        kern_return_t ret = CloseUserClient(connection);
        if ( kIOReturnSuccess != ret) {
            LOG(E) << "CloseUserClient returned " << HEXON(8) << ret;
        }
    }

    io_connect_t blockStorageDeviceService = handleDeviceService(osDeviceHandle);
    if ( blockStorageDeviceService != IO_OBJECT_NULL ) {
        LOG(D4) << "Releasing driver service";
        IOObjectRelease(blockStorageDeviceService);
    }
    LOG(D4) << "Device service "              << HEXOFF << blockStorageDeviceService << "=" << HEXON(4) << blockStorageDeviceService << " released and"
    << " closed user client "         << HEXOFF << connection                << "=" << HEXON(4) << connection;
}


#include <dirent.h>
#include <regex>
std::vector<std::string> DtaMacOS::generateDtaDriveDevRefs()
{
  std::vector<std::string> devrefs;

  DIR *dir = opendir("/dev");
  if (dir==NULL) {
    LOG(E) << "Can't read /dev ?!";
    return devrefs;
  }

  struct dirent *dirent;
  while (NULL != (dirent=readdir(dir))) {
    const char * name = dirent->d_name;
    if (std::regex_match(std::string(name), std::regex("^disk[0-9]+$"))) {
        devrefs.push_back(std::string("/dev/")+name);
    }
  }

  closedir(dir);

  std::sort(devrefs.begin(),devrefs.end(),
            [](std::string d1, std::string d2){ // 9 == strlen("/dev/disk")
      return std::stoi(d1.substr(9,std::string::npos)) < std::stoi(d2.substr(9,std::string::npos));
  });

  return devrefs;
}


void DtaMacOS::errorNoAccess(const char* devref) {
    if (devref == NULL) {
        LOG(E) << "You do not have permission to access the raw disk(s) in write mode;";
    } else {
        LOG(E) << "You do not have permission to access the raw disk " << devref << " in write mode;";
    }
    LOG(E) << "try to run as root.";
}



/**
 * Converts a CFString to a UTF-8 std::string if possible.
 *
 * @param input A reference to the CFString to convert.
 * @return Returns a std::string containing the contents of CFString converted to UTF-8. Returns
 *  an empty string if the input reference is null or conversion is not possible.
 */
// Modified from https://gist.githubusercontent.com/peter-bloomfield/1b228e2bb654702b1e50ef7524121fb9/raw/934184166a8c3ff403dd5d7f8c0003810014f73d/cfStringToStdString.cpp per comments
static
std::string cfStringToStdString(CFStringRef input, bool & error) {
    error = false;
    if (!input)
        return {};

    // Attempt to access the underlying buffer directly. This only works if no conversion or
    //  internal allocation is required.
    auto originalBuffer{ CFStringGetCStringPtr(input, kCFStringEncodingUTF8) };
    if (originalBuffer)
        return originalBuffer;

    // Copy the data out to a local buffer.
    CFIndex lengthInUtf16{ CFStringGetLength(input) };
    CFIndex maxLengthInUtf8{ CFStringGetMaximumSizeForEncoding(lengthInUtf16,
                                                               kCFStringEncodingUTF8) + 1 }; // <-- leave room for null terminator
    std::vector<char> localBuffer((size_t)maxLengthInUtf8);

    if (CFStringGetCString(input, localBuffer.data(), maxLengthInUtf8, kCFStringEncodingUTF8))
        return localBuffer.data();

    error = true;
    return {};
}


// Create a copy of the properties of this I/O registry entry
// Receiver owns this CFMutableDictionary instance if not NULL
static void collectProperties(CFDictionaryRef cfproperties, dictionary * properties); // called recursively

static void collectProperty(const void *vkey, const void *vvalue, void * vproperties){
    dictionary * properties = (dictionary *)vproperties;

    // Get the key --  should be a string
    std::string key, value="<\?\?\?>";
    CFTypeID keyTypeID = CFGetTypeID(vkey);
    if (CFStringGetTypeID() == keyTypeID) {
        bool error=false;
        key = cfStringToStdString(reinterpret_cast<CFStringRef>(vkey), error);
        if (error) {
            LOG(E) << "Failed to get key as string " << HEXON(sizeof(const void *)) << vkey;
            return;
        }
    } else {
        LOG(E) << "Unrecognized key type " << (CFTypeRef)vkey;
        return;
    };

    // Get the value -- could be a Bool, Dict, Data, String, or Number
    CFTypeID valueTypeID = CFGetTypeID(vvalue);
    if (CFStringGetTypeID() == valueTypeID) {
        // String
        bool error=false;
        value = cfStringToStdString(reinterpret_cast<CFStringRef>(vvalue), error);
        if (error) {
            LOG(E) << "Failed to get key as string " << HEXON(sizeof(const void *)) << vkey;
            return;
        }
    } else if (CFBooleanGetTypeID() == valueTypeID) {
        // Bool
        value = std::string(CFBooleanGetValue(reinterpret_cast<CFBooleanRef>(vvalue)) ? "true" : "false");
    } else if (CFNumberGetTypeID() == valueTypeID) {
        // Number
        if (CFNumberIsFloatType(reinterpret_cast<CFNumberRef>(vvalue))) {
            // Float
            double dvalue=0.0;
            bool error=!CFNumberGetValue(reinterpret_cast<CFNumberRef>(vvalue), kCFNumberDoubleType, (void *)&dvalue);
            if (error) {
                LOG(E) << "Failed to get value as float " << HEXON(sizeof(vvalue)) << vvalue;
                return;
            }
            value = std::to_string(dvalue);
        } else {
            // Integer
            long long llvalue=0LL;
            bool error=!CFNumberGetValue(reinterpret_cast<CFNumberRef>(vvalue), kCFNumberLongLongType, (void *)&llvalue);
            if (error) {
                LOG(E) << "Failed to get value as integer " << HEXON(sizeof(vvalue)) << vvalue;
                return;
            }
            value = std::to_string(llvalue);
        }
    } else if (CFDataGetTypeID() == valueTypeID) {
        // Data
    } else if (CFArrayGetTypeID() == valueTypeID) {
        // Array
    } else if (CFDictionaryGetTypeID() == valueTypeID) {
        // Dict -- call recursively to flatten subdirectory properties into `properties'
        collectProperties(reinterpret_cast<CFDictionaryRef>(vvalue), properties);
        return;
    } else {
        // Unknown
        LOG(E) << "Failed to get value " << HEXON(sizeof(vvalue)) << vvalue << " with type ID "  << HEXON(sizeof(valueTypeID)) << valueTypeID;
        return;
    }

    (*properties)[key]=value;
}

static
void collectProperties(CFDictionaryRef cfproperties, dictionary * properties) {
    CFDictionaryApplyFunction(cfproperties, collectProperty, (void *)properties);
}


static
dictionary * copyDeviceProperties(io_service_t deviceService) {
    CFDictionaryRef cfproperties = createIOBlockStorageDeviceProperties(deviceService);

    if (cfproperties==NULL)
        return NULL;

    dictionary * properties = new dictionary;
    collectProperties(cfproperties, properties);
    return properties;
}


dictionary* DtaMacOS::getOSSpecificInformation(OSDEVICEHANDLE osDeviceHandle,
                                               const char* /* TODO: devref */,
                                               InterfaceDeviceID& /* TODO: interfaceDeviceIdentification */,
                                               DTA_DEVICE_INFO& device_info)
{
    io_service_t deviceService=handleDeviceService(osDeviceHandle);
    io_connect_t connection=handleConnection(osDeviceHandle);



    if (IO_OBJECT_NULL == deviceService) {
        return NULL;
    }

    bool success=false;
    if (IO_OBJECT_NULL != connection) {
        io_service_t controllerService = findParent(deviceService);
        success=(KERN_SUCCESS == TPerUpdate(connection, controllerService, &device_info));
        IOObjectRelease(controllerService);
    } else {
        success=(DtaMacOSBlockStorageDevice::BlockStorageDeviceUpdate(deviceService, device_info));
    }
    if (!success)
        return NULL;

    return copyDeviceProperties(deviceService);
}

void * DtaMacOS::alloc_aligned_MIN_BUFFER_LENGTH_buffer () {
    return aligned_alloc( IO_BUFFER_ALIGNMENT,
                         (((MIN_BUFFER_LENGTH + IO_BUFFER_ALIGNMENT - 1)
                           / IO_BUFFER_ALIGNMENT)
                          * IO_BUFFER_ALIGNMENT) );
}

void DtaMacOS::free_aligned_MIN_BUFFER_LENGTH_buffer (void * aligned_buffer) {
    free(aligned_buffer);
}



/** Perform a SCSI command using the current operating system SCSI interface
 *
 * @param osDeviceHandle            OSDEVICEHANDLE osDeviceHandle of already-opened raw device file
 * @param dxfer_direction direction of transfer PSC_FROM/TO_DEV
 * @param cdb             SCSI command data buffer
 * @param cdb_len         length of SCSI command data buffer (often 12)
 * @param buffer          SCSI data buffer
 * @param bufferlen       SCSI data buffer len, also output transfer length
 * @param sense           SCSI sense data buffer
 * @param senselen        SCSI sense data buffer len (usually 32?)
 * @param pmasked_status  pointer to storage for masked_status, or NULL if not desired
 * @param timeout         optional timeout (in msecs)
 *
 * Returns the result of the os system call, as well as possibly setting *pmasked_status
 */
int DtaMacOS::PerformSCSICommand(OSDEVICEHANDLE osDeviceHandle,
                                 int dxfer_direction,
                                 uint8_t * cdb,   unsigned char cdb_len,
                                 void * buffer,   unsigned int& bufferlen,
                                 unsigned char * sense, unsigned char & senselen,
                                 SCSI_STATUS_CODE * pmasked_status,
                                 unsigned int timeout)
{
    if (osDeviceHandle==INVALID_HANDLE_VALUE) {
        LOG(E) << "Scsi device not open";
        return EBADF;
    }

    IFLOG(D4)
    if (dxfer_direction ==  PSC_TO_DEV) {
        LOG(D4) << "PerformSCSICommand buffer before";
        DtaHexDump(buffer,bufferlen);
    }

    /*
     * Do the IO
     */
    (void)sense;
    (void)senselen;
    (void)pmasked_status;
    (void)timeout;


    SCSICommandDescriptorBlock scdb;
    memset(scdb, 0, sizeof(scdb));
    memcpy(scdb, cdb, cdb_len);

    const uint64_t bufferSize = bufferlen;
    const uint64_t requestedTransferLength = bufferlen;
    uint64_t LengthActuallyTransferred=0;
    kern_return_t kernResult = DriverPerformSCSICommand(handleConnection(osDeviceHandle),
                                                        scdb,
                                                        const_cast<const void *>(buffer),
                                                        bufferSize,
                                                        requestedTransferLength,
                                                        &LengthActuallyTransferred);



    LOG(D4) << "PerformSCSICommand kernResult=" << HEXON(8) << kernResult ;
    IFLOG(D4) {
        if (kernResult < 0) {
            LOG(D4) << "cdb after returned " << HEXON(8) << kernResult << ":";
            DtaHexDump(cdb, cdb_len);
        } else {
            LOG(D4) << "PerformSCSICommand buffer after kernResult " << HEXON(8) << kernResult ;
            DtaHexDump(buffer, bufferlen);
        }
    }
    if (kIOReturnSuccess != kernResult) {
        //        LOG(E) << "PerformSCSICommand returned error "  << HEXON(8) << kernResult;
        return 0xff;
    } else {
        return 0 ;
    }
}


/** Perform a NVMe command using the `nvme_admin_cmd' (NVMe standard) interface with ioctl `NVME_IOCTL_ADMIN_CMD'
 *
 * @param osDeviceHandle  OSDEVICEHANDLE of already-opened raw device file
 * @param pcmd             NVMe command struct
 *
 * Returns the result of the os system call, as well as possibly setting *pstatus
 */
int DtaMacOS::PerformNVMeCommand(OSDEVICEHANDLE osDeviceHandle,
                                 uint8_t * pcmd,
                                 uint32_t * /* pstatus */)
{
  if (osDeviceHandle==INVALID_HANDLE_VALUE) {
    LOG(E) << "Nvme device not open";
    return EBADF;
  }

  if (pcmd == NULL)
    return DTAERROR_COMMAND_ERROR;
  nvme_admin_cmd & cmd=*(reinterpret_cast<nvme_admin_cmd *>(pcmd));

  /*
   * Do the IO
   */

  IFLOG(D4) {
    LOG(D4) << " DtaMacOS::PerformNVMeCommand cmd:" ;
    DtaHexDump(&cmd, sizeof(cmd));
  }

  LOG(D3) << "DtaMacOS::PerformNVMeCommand UNIMPLEMENTED!!";

  return DTAERROR_FAILURE;
}
