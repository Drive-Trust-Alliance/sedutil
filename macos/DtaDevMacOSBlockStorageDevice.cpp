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
//#include "os.h"
//#include <mach/mach_port.h>
//#include <IOKit/IOKitLib.h>
//#include <IOKit/scsi/SCSITaskLib.h>
//#include "TPerKernelInterface.h"
//#include "PrintBuffer.h"
//#include "RegistryUtilities.h"
//#include "DtaMacOSConstants.h"
#include "SEDKernelInterface/SEDKernelInterface.h"
#include <IOKit/storage/IOBlockStorageDevice.h>
#include <IOKit/storage/IOMedia.h>
#include <IOKit/storage/nvme/NVMeSMARTLibExternal.h>
#include <IOKit/IOKitLib.h>
#include <string.h>
#include "DtaDevMacOSTPer.h"

// Some macros to access the properties dicts and values
#define GetBool(dict,name) (CFBooleanRef)CFDictionaryGetValue(dict, CFSTR(name))
#define GetDict(dict,name) (CFDictionaryRef)CFDictionaryGetValue(dict, CFSTR(name))
#define GetData(dict,name) (CFDataRef)CFDictionaryGetValue(dict, CFSTR(name))
#define GetString(dict,name) (CFStringRef)CFDictionaryGetValue(dict, CFSTR(name))
#define GetPropertiesDict(name) GetDict(properties, name)

static bool parseNVMeSMARTLibIdentifyData(io_service_t aBlockStorageDevice, CFDictionaryRef deviceProperties, DTA_DEVICE_INFO &disk_info)
{
    CFBooleanRef bNVMeSMARTCapable = GetBool(deviceProperties, kIOPropertyNVMeSMARTCapableKey);
    if (!(bNVMeSMARTCapable != NULL))
        return false;
    if (!(CFGetTypeID(bNVMeSMARTCapable) == CFBooleanGetTypeID()))
        return false;
    if (!(CFBooleanGetValue(bNVMeSMARTCapable)))
        return false;
    
    CFDictionaryRef ioPluginTypes = GetDict( deviceProperties, kIOCFPlugInTypesKey);
    if (!(ioPluginTypes != NULL))
        return false;
    
    CFStringRef typeID= CFUUIDCreateString(CFAllocatorGetDefault(),kIONVMeSMARTUserClientTypeID);
    CFStringRef name = (CFStringRef)CFDictionaryGetValue(ioPluginTypes, typeID);
    CFRelease(typeID);
    if (!(name != NULL))
        return false;
    
    
    IOCFPlugInInterface     **plugInInterface = NULL;
    IONVMeSMARTInterface    **NVMeSMARTInterface = NULL;
    SInt32                  score;
    IOReturn kr = IOCreatePlugInInterfaceForService(aBlockStorageDevice, kIONVMeSMARTUserClientTypeID, kIOCFPlugInInterfaceID, &plugInInterface, &score);
    
    if ((kIOReturnSuccess != kr) || plugInInterface == NULL) {
        fprintf(stderr, "IOCreatePlugInInterfaceForService returned 0x%08x", kr);   // TODO:  replace fprintf
        return false;
    }
    
    // Use the plugin interface to retrieve the device interface.
    HRESULT res = (*plugInInterface)->QueryInterface(plugInInterface, CFUUIDGetUUIDBytes(kIONVMeSMARTInterfaceID),
                                                     (LPVOID *)&NVMeSMARTInterface);
    // Now done with the plugin interface.
    (*plugInInterface)->Release(plugInInterface);
    
    if (0 != res || NVMeSMARTInterface == NULL) {
        fprintf(stderr, "QueryInterface returned %d.\n", (int) res);
        return false;
    }
    uint32_t inNamespace = 0 ;
    NVMeIdentifyControllerStruct data;
    
    kr = (*NVMeSMARTInterface) -> GetIdentifyData (NVMeSMARTInterface, &data, inNamespace );
    (*NVMeSMARTInterface)->Release(NVMeSMARTInterface);
    if ((kIOReturnSuccess != kr) ) {
        fprintf(stderr, "GetIdentifyData returned 0x%08x", kr);
        return false;
    }
    
    memcpy(disk_info.serialNum, data.SERIAL_NUMBER, sizeof(disk_info.serialNum));
    memcpy(disk_info.firmwareRev, data.FW_REVISION, sizeof(disk_info.firmwareRev));
    memcpy(disk_info.modelNum, data.MODEL_NUMBER, sizeof(disk_info.modelNum));
    memcpy(disk_info.worldWideName, data.IEEE_OUI_ID, sizeof(disk_info.worldWideName));
    
    return true;
}

static bool FillDiskInfoFromDeviceProperties(CFDictionaryRef deviceProperties, DTA_DEVICE_INFO &disk_info) {
    if (NULL != deviceProperties ) {
        CFDictionaryRef deviceCharacteristics = GetDict(deviceProperties, "Device Characteristics");
        if (NULL != deviceCharacteristics) {
            CFStringRef vendorName = GetString(deviceCharacteristics, "Vendor Name");
            if (vendorName != NULL )
                CFStringGetCString(vendorName, (char *)&disk_info.vendorName, sizeof(disk_info.vendorName)+sizeof(disk_info.null0), kCFStringEncodingASCII);
            CFStringRef modelNumber = GetString(deviceCharacteristics, "Product Name");
            if (modelNumber != NULL)
                CFStringGetCString(modelNumber, (char *)&disk_info.modelNum, sizeof(disk_info.modelNum)+sizeof(disk_info.null1), kCFStringEncodingASCII);
            CFStringRef firmwareRevision = GetString(deviceCharacteristics, "Product Revision Level");
            if (firmwareRevision != NULL)
                CFStringGetCString(firmwareRevision, (char *)&disk_info.firmwareRev, sizeof(disk_info.firmwareRev)+sizeof(disk_info.null2), kCFStringEncodingASCII);
            CFStringRef serialNumber = GetString(deviceCharacteristics, "Serial Number");
            if (serialNumber != NULL )
                CFStringGetCString(serialNumber, (char *)&disk_info.serialNum, sizeof(disk_info.serialNum)+sizeof(disk_info.null0), kCFStringEncodingASCII);
            return true;
        }
    }
    return false;
}

void DtaDevMacOSBlockStorageDevice::parse_properties_into_disk_info(io_service_t aBlockStorageDevice) {
    // Parse from properties instance var
    // Parse TPer properties, if available
    if (pdisk_info == NULL)
        return;
    DTA_DEVICE_INFO & disk_info = *pdisk_info;
    
    CFDictionaryRef tPerProperties = GetPropertiesDict("TPer");
    if (NULL != tPerProperties) {  // Probably not, since we are probably not a DtaDevMacOSTPer
        CFDataRef diData = GetData(tPerProperties, IOOPALDiskInfoKey);
        if (NULL != diData) {
            const uint8_t * pdi = CFDataGetBytePtr(diData);
            if (NULL != pdi) {
                memcpy(&disk_info, pdi, (unsigned)CFDataGetLength(diData));
                return;
            }
        }
    }
    
    CFDictionaryRef deviceProperties = GetPropertiesDict("device");
    if (NULL != deviceProperties) {
        CFBooleanRef NVMeSMARTCapable = GetBool(deviceProperties, "NVMe SMART Capable");
        if (NULL != NVMeSMARTCapable && CFBooleanGetValue(NVMeSMARTCapable)) {
            disk_info.devType = DEVICE_TYPE_NVME;
        } else {
            CFDictionaryRef protocolProperties = GetDict(deviceProperties, "Protocol Characteristics");
            if (NULL != protocolProperties) {
                CFStringRef interconnect = GetString(protocolProperties, "Physical Interconnect");
                if (NULL != interconnect) {
                    if (CFEqual(interconnect, CFSTR("USB"))) {
                        disk_info.devType = DEVICE_TYPE_USB;
                    } else if (CFEqual(interconnect, CFSTR("Apple Fabric"))) {
                        disk_info.devType = DEVICE_TYPE_NVME;
                    } else if (CFEqual(interconnect, CFSTR("PCI-Express"))) {
                        disk_info.devType = DEVICE_TYPE_OTHER;  // TODO ... what?
                    } else if (CFEqual(interconnect, CFSTR("SATA"))) {
                        disk_info.devType = DEVICE_TYPE_ATA;
                    } else
                        disk_info.devType = DEVICE_TYPE_OTHER;
                }
            }
        }
    } else {
        disk_info.devType = DEVICE_TYPE_OTHER; // TODO -- generalize for other devices when they are supported by BPTperDriver
    }

    
    FillDiskInfoFromDeviceProperties(deviceProperties, disk_info);

    
    switch (disk_info.devType) {
        case DEVICE_TYPE_NVME:
        case DEVICE_TYPE_OTHER:
            if (parseNVMeSMARTLibIdentifyData(aBlockStorageDevice, deviceProperties, disk_info))
                break;
            break;
        case DEVICE_TYPE_USB:
#define HACK_MODELNUM_WITH_VENDORNAME
#if defined(HACK_MODELNUM_WITH_VENDORNAME)
        {
            size_t vendorNameLength = strlen((const char *)disk_info.vendorName);
            size_t modelNumLength = strlen((const char *)disk_info.modelNum);
            size_t newModelNumLength = vendorNameLength + modelNumLength ;
            if (0 < newModelNumLength) {
                size_t maxModelNumLength = sizeof(disk_info.modelNum);
                if (maxModelNumLength < newModelNumLength) {
                    size_t excessModelNumLength = newModelNumLength - maxModelNumLength ;
                    vendorNameLength -= excessModelNumLength;
                    newModelNumLength = maxModelNumLength;
                }
                uint8_t newModelNum[sizeof(disk_info.modelNum)];
                bzero(newModelNum, sizeof(newModelNum));
                if (0 < vendorNameLength) {
                    memcpy(&newModelNum[0],disk_info.vendorName,vendorNameLength);
                }
                if (0 < modelNumLength) {
                    memcpy(&newModelNum[vendorNameLength],disk_info.modelNum,modelNumLength);
                }
                memcpy(disk_info.modelNum,newModelNum,newModelNumLength);
            }
        }
#endif //defined(HACK_MODELNUM_WITH_VENDORNAME)
            break;
        case DEVICE_TYPE_ATA:
            // identify();
            break;
        default:
            ;
    }

    return;

}



// Sorting order
bool DtaDevMacOSBlockStorageDevice::bsdNameLessThan(DtaDevMacOSBlockStorageDevice * a, DtaDevMacOSBlockStorageDevice * b) {
    const string & aName = a->bsdName;
    const string & bName = b->bsdName;
    const auto aNameLength = aName.length();
    const auto bNameLength = bName.length();
    if (aNameLength < bNameLength ) {
        return true;
    }
    if (bNameLength < aNameLength ) {
        return false;
    }
    return aName < bName ;
}

// extern CFMutableDictionaryRef copyProperties(io_registry_entry_t service);
// #import <Foundation/Foundation.h>
static
CFMutableDictionaryRef copyProperties(io_registry_entry_t service) {
    CFMutableDictionaryRef cfproperties = NULL;
    if (KERN_SUCCESS != IORegistryEntryCreateCFProperties(service,
                                                          &cfproperties,
                                                          CFAllocatorGetDefault(),
                                                          0)) {
        return NULL;
    }
    return cfproperties;
}


std::vector<DtaDevMacOSBlockStorageDevice *> DtaDevMacOSBlockStorageDevice::enumerateBlockStorageDevices() {
    std::vector<DtaDevMacOSBlockStorageDevice *>devices;
    io_iterator_t iterator = findMatchingServices(kIOBlockStorageDeviceClass);
    io_service_t aBlockStorageDevice;
    io_service_t media;
    io_service_t tPer;
    DTA_DEVICE_INFO * pdi;
    CFDictionaryRef deviceProperties;
    CFDictionaryRef mediaProperties;
    CFDictionaryRef tPerProperties;
    CFDictionaryRef properties;

    const CFIndex kCStringSize = 128;
    char nameBuffer[kCStringSize];
    bzero(nameBuffer,kCStringSize);

    string entryName;
    string bsdName;
    
    // Iterate over nodes of class IOBlockStorageDevice or subclass thereof
    while ( (IO_OBJECT_NULL != (aBlockStorageDevice = IOIteratorNext( iterator )))) {
        
        std::vector<const void *>keys;
        std::vector<const void *>values;
        
        deviceProperties = copyProperties( aBlockStorageDevice );
        if (NULL == deviceProperties) {
            goto finishedWithDevice;
        }
        keys.push_back( CFSTR("device"));
        values.push_back( deviceProperties);

        
        media = findServiceForClassInChildren(aBlockStorageDevice, kIOMediaClass);
        if (IO_OBJECT_NULL == media) {
            goto finishedWithMedia;
        }
        mediaProperties = copyProperties( media );
        if (NULL == mediaProperties ) {
            goto finishedWithMedia ;
        }
        keys.push_back( CFSTR("media"));
        values.push_back( mediaProperties);


        tPer = findParent(aBlockStorageDevice);
        if (IOObjectConformsTo(tPer, kBrightPlazaDriverClass)) {
            tPerProperties = copyProperties( tPer );
            keys.push_back( CFSTR("TPer"));
            values.push_back(tPerProperties);
        }
        
        
        properties = CFDictionaryCreate(CFAllocatorGetDefault(), keys.data(), values.data(), (CFIndex)keys.size(), NULL, NULL);

        bzero(nameBuffer,kCStringSize);
        CFStringGetCString(GetString(mediaProperties, "BSD Name"),
                           nameBuffer, kCStringSize, kCFStringEncodingUTF8);
        bsdName = string(nameBuffer);
        
        bzero(nameBuffer,kCStringSize);
        GetName(media,nameBuffer);
        entryName = string(nameBuffer);
        
        pdi = static_cast <DTA_DEVICE_INFO *> (malloc(sizeof(DTA_DEVICE_INFO)));
        bzero(pdi, sizeof(DTA_DEVICE_INFO));
        
        devices.push_back( getBlockStorageDevice(aBlockStorageDevice, entryName, bsdName, properties, pdi) );
        
        IOObjectRelease(tPer);

    finishedWithMedia:
        IOObjectRelease(media);

    finishedWithDevice:
        IOObjectRelease(aBlockStorageDevice);
    }
    
    sort(devices.begin(), devices.end(), DtaDevMacOSBlockStorageDevice::bsdNameLessThan);
    return devices;
}

// Factory for this class or subclass instances
DtaDevMacOSBlockStorageDevice * DtaDevMacOSBlockStorageDevice::getBlockStorageDevice(io_service_t aBlockStorageDevice,
                                                                                     std::string entryName,
                                                                                     std::string bsdName,
                                                                                     CFDictionaryRef properties,
                                                                                     DTA_DEVICE_INFO * pdi) {
    
    CFDictionaryRef tPerProperties = GetPropertiesDict("TPer");
    if (NULL != tPerProperties)
        return DtaDevMacOSTPer::getTPer(aBlockStorageDevice, entryName, bsdName, tPerProperties, properties, pdi);
    else
        return new DtaDevMacOSBlockStorageDevice(aBlockStorageDevice, entryName, bsdName, properties, pdi);
}

 DtaDevMacOSBlockStorageDevice * DtaDevMacOSBlockStorageDevice::getBlockStorageDevice(const char * devref, DTA_DEVICE_INFO * pdi)
{
    DtaDevMacOSBlockStorageDevice * foundDevice = NULL;
    io_iterator_t iterator = findMatchingServices(kIOBlockStorageDeviceClass);
    io_service_t aBlockStorageDevice;
    io_service_t tPer;
    io_service_t media;
    CFDictionaryRef deviceProperties;
    CFDictionaryRef mediaProperties;
    CFDictionaryRef tPerProperties;
    CFDictionaryRef allProperties;

    const CFIndex kCStringSize = 128;
    char nameBuffer[kCStringSize];
    bzero(nameBuffer,kCStringSize);

    string entryName;
    string bsdName;

    // Iterate over nodes of class IOBlockStorageDevice or subclass thereof
    while ( foundDevice == NULL && (IO_OBJECT_NULL != (aBlockStorageDevice = IOIteratorNext( iterator )))) {

        std::vector<const void *>keys;
        std::vector<const void *>values;

        deviceProperties = copyProperties( aBlockStorageDevice );
        if (NULL == deviceProperties) {
            goto finishedWithDevice;
        }
        keys.push_back( CFSTR("device"));
        values.push_back( deviceProperties);


        media = findServiceForClassInChildren(aBlockStorageDevice, kIOMediaClass);
        if (IO_OBJECT_NULL == media) {
            goto finishedWithMedia;
        }
        mediaProperties = copyProperties( media );
        if (NULL == mediaProperties ) {
            goto finishedWithMedia ;
        }
        keys.push_back( CFSTR("media"));
        values.push_back( mediaProperties);


        tPer = findParent(aBlockStorageDevice);
        if (IOObjectConformsTo(tPer, kBrightPlazaDriverClass)) {
            tPerProperties = copyProperties( tPer );
            keys.push_back( CFSTR("TPer"));
            values.push_back(tPerProperties);
        }


        bzero(nameBuffer,kCStringSize);
        CFStringGetCString(GetString(mediaProperties, "BSD Name"),
                           nameBuffer, kCStringSize, kCFStringEncodingUTF8);
        bsdName = string(nameBuffer);

        if (bsdName == string(devref)) {
            bzero(nameBuffer,kCStringSize);
            GetName(media,nameBuffer);
            entryName = string(nameBuffer);

            allProperties = CFDictionaryCreate(CFAllocatorGetDefault(),
                                               keys.data(), values.data(), (CFIndex)keys.size(),
                                               NULL, NULL);

            foundDevice = DtaDevMacOSBlockStorageDevice::getBlockStorageDevice(aBlockStorageDevice,
                                                                               entryName,
                                                                               bsdName,
                                                                               allProperties,
                                                                               pdi);
            
        }

        IOObjectRelease(tPer);

    finishedWithMedia:
        IOObjectRelease(media);

    finishedWithDevice:
        IOObjectRelease(aBlockStorageDevice);
    }

    return foundDevice;
}



DtaDevMacOSBlockStorageDevice::~DtaDevMacOSBlockStorageDevice () {
    if (properties != NULL )
        CFRelease(properties);
}

uint8_t DtaDevMacOSBlockStorageDevice::isOpal2()
{
    return pdisk_info->OPAL20;
}
uint8_t DtaDevMacOSBlockStorageDevice::isOpal1()
{
    return pdisk_info->OPAL10;
}
uint8_t DtaDevMacOSBlockStorageDevice::isEprise()
{
    return pdisk_info->Enterprise;
}

uint8_t DtaDevMacOSBlockStorageDevice::isAnySSC()
{
    return pdisk_info->ANY_OPAL_SSC;
}
uint8_t DtaDevMacOSBlockStorageDevice::MBREnabled()
{
    return pdisk_info->Locking_MBREnabled;
}
uint8_t DtaDevMacOSBlockStorageDevice::MBRDone()
{
    return pdisk_info->Locking_MBRDone;
}
uint8_t DtaDevMacOSBlockStorageDevice::Locked()
{
    return pdisk_info->Locking_locked;
}
uint8_t DtaDevMacOSBlockStorageDevice::LockingEnabled()
{
    return pdisk_info->Locking_lockingEnabled;
}
char *DtaDevMacOSBlockStorageDevice::getVendorName()
{
    return (char *)&pdisk_info->vendorName;
}
char *DtaDevMacOSBlockStorageDevice::getFirmwareRev()
{
    return (char *)&pdisk_info->firmwareRev;
}
char *DtaDevMacOSBlockStorageDevice::getModelNum()
{
    return (char *)&pdisk_info->modelNum;
}
char *DtaDevMacOSBlockStorageDevice::getSerialNum()
{
    return (char *)&pdisk_info->serialNum;
}

DTA_DEVICE_TYPE DtaDevMacOSBlockStorageDevice::getDevType()
{
    return pdisk_info->devType;
}

const std::string DtaDevMacOSBlockStorageDevice::getDevName () {
    char devname[26];
    bzero(devname, sizeof(devname));
    snprintf(devname,25,"/dev/%s", bsdName.c_str());
    return devname;
}

