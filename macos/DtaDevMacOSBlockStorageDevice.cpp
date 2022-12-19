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
#include <IOKit/storage/IOBlockStorageDevice.h>
#include <IOKit/storage/IOMedia.h>
#include <IOKit/storage/ata/ATASMARTLib.h>
#include <IOKit/storage/nvme/NVMeSMARTLibExternal.h>
#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CFNumber.h>
#include <string>
#include <regex>

#include "oui_vendor.hpp"
#include "stdio.h"

#include <SEDKernelInterface/SEDKernelInterface.h>
#include "DtaHashPwd.h"
#include "DtaDevMacOSTPer.h"
#include "DtaDevOS.h"

// Some macros to access the properties dicts and values
#define GetBool(dict,name) (CFBooleanRef)CFDictionaryGetValue(dict, CFSTR(name))
#define GetDict(dict,name) (CFDictionaryRef)CFDictionaryGetValue(dict, CFSTR(name))
#define GetData(dict,name) (CFDataRef)CFDictionaryGetValue(dict, CFSTR(name))
#define GetString(dict,name) (CFStringRef)CFDictionaryGetValue(dict, CFSTR(name))
#define GetNumber(dict,name) (CFNumberRef)CFDictionaryGetValue(dict, CFSTR(name))

#define ERRORS_TO_STDERR
//#undef ERRORS_TO_STDERR


static bool FillDeviceInfoFromProperties(CFDictionaryRef deviceProperties, CFDictionaryRef mediaProperties,
                                         DTA_DEVICE_INFO &device_info) {
    if (NULL != mediaProperties) {
        CFNumberRef size = GetNumber(mediaProperties, "Size");
        if (NULL != size) {
            CFNumberType numberType = CFNumberGetType(size);
            switch (numberType) {
                case kCFNumberLongLongType:
                    {
                        long long sSize ;
                        if (CFNumberGetValue(size, numberType, &sSize)) {
                            device_info.devSize = (uint64_t)sSize;
                        }
                    }
                    break;
                case kCFNumberSInt64Type:
                    {
                        int64_t sSize ;
                        if (CFNumberGetValue(size, numberType, &sSize)) {
                            device_info.devSize = (uint64_t)sSize;
                        }
                    }
                    break;
                default:
                    ;
            }
        }
    }
    if (NULL != deviceProperties ) {
        CFDictionaryRef deviceCharacteristics = GetDict(deviceProperties, "Device Characteristics");
        if (NULL != deviceCharacteristics) {
            CFStringRef vendorID = GetString(deviceCharacteristics, "Vendor ID");
            if (vendorID != NULL )
                CFStringGetCString(vendorID, (char *)&device_info.vendorID, sizeof(device_info.vendorID)+sizeof(device_info.vendorIDNull), kCFStringEncodingASCII);
            CFStringRef modelNumber = GetString(deviceCharacteristics, "Product Name");
            if (modelNumber != NULL)
                CFStringGetCString(modelNumber, (char *)&device_info.modelNum, sizeof(device_info.modelNum)+sizeof(device_info.modelNumNull), kCFStringEncodingASCII);
            CFStringRef firmwareRevision = GetString(deviceCharacteristics, "Product Revision Level");
            if (firmwareRevision != NULL)
                CFStringGetCString(firmwareRevision, (char *)&device_info.firmwareRev, sizeof(device_info.firmwareRev)+sizeof(device_info.firmwareRevNull), kCFStringEncodingASCII);
            CFStringRef serialNumber = GetString(deviceCharacteristics, "Serial Number");
            if (serialNumber != NULL )
                CFStringGetCString(serialNumber, (char *)&device_info.serialNum, sizeof(device_info.serialNum)+sizeof(device_info.serialNumNull), kCFStringEncodingASCII);
            
        }
            
        CFDictionaryRef protocolProperties = GetDict(deviceProperties, "Protocol Characteristics");
        if (NULL != protocolProperties) {
            CFStringRef interconnect = GetString(protocolProperties, "Physical Interconnect");
            if (NULL != interconnect) {
                CFStringGetCString(interconnect,
                                   (char *)device_info.physicalInterconnect,
                                   sizeof(device_info.physicalInterconnect)+sizeof(device_info.physicalInterconnectNull),
                                   kCFStringEncodingASCII);
            }
            CFStringRef interconnectLocation = GetString(protocolProperties, "Physical Interconnect Location");
            if (NULL != interconnectLocation) {
                CFStringGetCString(interconnectLocation,
                                   (char *)device_info.physicalInterconnectLocation,
                                   sizeof(device_info.physicalInterconnectLocation)+sizeof(device_info.physicalInterconnectLocationNull),
                                   kCFStringEncodingASCII);
            }
        }
        
        return true;
    }
    return false;
}

const DTA_DEVICE_INFO & DtaDevMacOSBlockStorageDevice::device_info() {
    assert(NULL != pdevice_info);
    return *pdevice_info;
}

#define GetPropertiesDict(name) GetDict(properties, name)
void DtaDevMacOSBlockStorageDevice::parse_properties_into_device_info() {
        if (pdevice_info == NULL)
            return;
        DTA_DEVICE_INFO & device_info = *pdevice_info;
        
        CFDictionaryRef tPerProperties = GetPropertiesDict("TPer");
        if (NULL != tPerProperties) {  // Probably not, since we are probably not a DtaDevMacOSTPer
            CFDataRef diData = GetData(tPerProperties, IODtaDeviceInfoKey);
            if (NULL != diData) {
                const uint8_t * pdi = CFDataGetBytePtr(diData);
                if (NULL != pdi) {
                    memcpy(&device_info, pdi, (unsigned)CFDataGetLength(diData));
                    return;
                }
            }
        }
        
        CFDictionaryRef deviceProperties = GetPropertiesDict("device");
        if (NULL != deviceProperties) {
            CFDictionaryRef protocolProperties = GetDict(deviceProperties, "Protocol Characteristics");
            if (NULL != protocolProperties) {
                CFStringRef interconnect = GetString(protocolProperties, "Physical Interconnect");
                if (NULL != interconnect) {
                    if (CFEqual(interconnect, CFSTR("USB"))) {
                        device_info.devType = DEVICE_TYPE_USB;
                    } else if (CFEqual(interconnect, CFSTR("Apple Fabric"))) {
                        device_info.devType = DEVICE_TYPE_NVME;
                    } else if (CFEqual(interconnect, CFSTR("PCI-Express"))) {
                        device_info.devType = DEVICE_TYPE_OTHER;  // TODO ... what?
                    } else if (CFEqual(interconnect, CFSTR("SATA"))) {
                        device_info.devType = DEVICE_TYPE_ATA;
                    } else
                        device_info.devType = DEVICE_TYPE_OTHER;
                }
            }
        } else {
            device_info.devType = DEVICE_TYPE_OTHER; // TODO -- generalize for other devices when they are supported by BPTperDriver
        }
        
        CFDictionaryRef mediaProperties = GetPropertiesDict("media");
        
        FillDeviceInfoFromProperties(deviceProperties, mediaProperties, device_info);
}
    
    
    
// Sorting order
bool DtaDevMacOSBlockStorageDevice::bsdNameLessThan(DtaDevMacOSBlockStorageDevice * a,
                                                    DtaDevMacOSBlockStorageDevice * b) {
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
    
    
std::vector<DtaDevMacOSBlockStorageDevice *> DtaDevMacOSBlockStorageDevice::enumerateBlockStorageDevices() {        std::vector<DtaDevMacOSBlockStorageDevice *>devices;
    io_iterator_t iterator = findMatchingServices(kIOBlockStorageDeviceClass);
    io_service_t aBlockStorageDevice;
    io_service_t media;
    io_service_t tPer;
    DTA_DEVICE_INFO * pdi;
    CFDictionaryRef deviceProperties;
    CFDictionaryRef mediaProperties;
    CFDictionaryRef tPerProperties;
    CFDictionaryRef allProperties;
    
    const CFIndex kCStringSize = 128;
    char nameBuffer[kCStringSize];
    bzero(nameBuffer,kCStringSize);
    
    string entryNameStr;
    string bsdNameStr;
    
    // Iterate over nodes of class IOBlockStorageDevice or subclass thereof
    while ( (IO_OBJECT_NULL != (aBlockStorageDevice = IOIteratorNext( iterator )))) {
        
        std::vector<const void *>keys;
        std::vector<const void *>values;
        CFDictionaryRef protocolCharacteristics;
        CFStringRef physicalInterconnectLocation;
        
        deviceProperties = copyProperties( aBlockStorageDevice );
        if (NULL == deviceProperties) {
            goto finishedWithDevice;
        }
        protocolCharacteristics = GetDict(deviceProperties, "Protocol Characteristics");
        if (NULL == protocolCharacteristics) {
            goto finishedWithDevice;
        }
        physicalInterconnectLocation = GetString(protocolCharacteristics, "Physical Interconnect Location");
        if (NULL == physicalInterconnectLocation ||
            CFEqual(physicalInterconnectLocation, CFSTR("File"))) {
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
        
        
        allProperties = CFDictionaryCreate(CFAllocatorGetDefault(), keys.data(), values.data(), (CFIndex)keys.size(), NULL, NULL);
        
        bzero(nameBuffer,kCStringSize);
        CFStringGetCString(GetString(mediaProperties, "BSD Name"),
                           nameBuffer, kCStringSize, kCFStringEncodingUTF8);
        bsdNameStr = string(nameBuffer);
        
        bzero(nameBuffer,kCStringSize);
        GetName(media,nameBuffer);
        entryNameStr = string(nameBuffer);
        
        pdi = static_cast <DTA_DEVICE_INFO *> (malloc(sizeof(DTA_DEVICE_INFO)));
        bzero(pdi, sizeof(DTA_DEVICE_INFO));
        
        devices.push_back( getBlockStorageDevice(entryNameStr, bsdNameStr, allProperties, pdi) );
        
        IOObjectRelease(tPer);
        
    finishedWithMedia:
        IOObjectRelease(media);
        
    finishedWithDevice:
        IOObjectRelease(aBlockStorageDevice);
    }
    
    sort(devices.begin(), devices.end(), DtaDevMacOSBlockStorageDevice::bsdNameLessThan);
    return devices;
}

    

static void trimBuffer(char * buffer, size_t len) {
    char * b = buffer;
    char * e = b + len;
    // Trim off trailing whitespace (and nulls)
    char * ne = e;
    while (b < ne && (ne[-1] == 0 || isspace(ne[-1])))
        *--ne = 0;
    // Slide left over leading whitespace
    char * nb = b;
    while (nb < ne && isspace(*nb))
        nb++;
    if (nb == ne) {
        bzero(buffer, len) ; // No contents
    } else if (b != nb) {
        memmove(b, nb, (size_t)(ne - nb));
        memset(b + (size_t)(ne - nb), 0,  len - (size_t)(ne - nb));
    }
   
}


void DtaDevMacOSBlockStorageDevice::polishDeviceInfo() {
    DTA_DEVICE_INFO & device_info = *pdevice_info;
    
#define is_not_all_NULs(field) (__is_not_all_NULs(device_info.field, sizeof(device_info.field)))
#define is_not_all_zeroes(field) (__is_not_all_zeroes(device_info.field, sizeof(device_info.field)))

#define _trim(field) trimBuffer((char *)device_info.field, sizeof(device_info.field))
    
    _trim(modelNum);
    _trim(vendorID);
    _trim(serialNum);
    _trim(firmwareRev);
    _trim(manufacturerName);

    
    
#define VENDOR_ID_MIGHT_BE_FIRST_WORD_OF_MODEL_NUMBER
#if defined( VENDOR_ID_MIGHT_BE_FIRST_WORD_OF_MODEL_NUMBER )
    if (!is_not_all_NULs(vendorID)) {
        std::regex leftmostWord("([^ ]*) (.*)");
        std::cmatch match;
        if (std::regex_match((const char *)device_info.modelNum,
                             match,
                             leftmostWord)) {
            string vendorName = match.str(1);
            const char * vendorID = vendorName.c_str();
            if (vendorID != NULL && 0 < strlen(vendorID)) {
                // Is the first word of modelNum also a vendorID (canonically, if necessary)?
                const char * vendor = vendor_for_vendorID_canonically_if_necessary(vendorID);
                if (vendor != NULL) {
                    size_t vendorID_length_plus_one=strnlen(vendorID, sizeof(device_info.vendorID))+1;
                    strncpy((char *)device_info.vendorID, vendorID, vendorID_length_plus_one);
                    
                    memset(device_info.modelNum, ' ', vendorID_length_plus_one);
                    _trim(modelNum);
                }
            }
        }
    }
#endif // defined( VENDOR_ID_MIGHT_BE_FIRST_WORD_OF_MODEL_NUMBER )

#undef _trim

#define EXTEND_DTA_DEVICE_INFO_WITH_OUI_VENDOR_DATA
#if defined( EXTEND_DTA_DEVICE_INFO_WITH_OUI_VENDOR_DATA )
    if (is_not_all_NULs(worldWideName)) {
        if (!is_not_all_NULs(manufacturerName)) {
            char oui[8]={0};
            snprintf(oui, 8, "%02X%02X%02X%02X",
                     device_info.worldWideName[0],
                     device_info.worldWideName[1],
                     device_info.worldWideName[2],
                     device_info.worldWideName[3]);
            assert(oui[0]=='5');
            const char * manufacturer = manufacturer_for_oui((const char *)&oui[1]);
            if (manufacturer != NULL) {
                strncpy((char *)device_info.manufacturerName,
                        manufacturer,
                        sizeof(device_info.manufacturerName));
            }
        }
        if (!is_not_all_NULs(vendorID)) {
            const char * vendorID =
            vendorID_for_vendor_canonically_if_necessary((const char *)device_info.manufacturerName);
            if (vendorID != NULL) {
                strncpy((char *)device_info.vendorID, vendorID, sizeof(device_info.vendorID));
            }
        }
    } else {
        if (is_not_all_NULs(vendorID)) {
            if (!is_not_all_NULs(manufacturerName)) {
                const char * vendor =
                    vendor_for_vendorID_canonically_if_necessary((const char *)device_info.vendorID);
                if (vendor != NULL) {
                    strncpy((char *)device_info.manufacturerName, vendor, sizeof(device_info.manufacturerName));
                }
                const char * oui =
                    oui_for_vendorID_canonically_if_necessary((const char *)device_info.vendorID);
                if (oui == NULL) {
                    oui = private_oui ; // "Private"
                }
                
                char worldWideNameHex[17];
                snprintf(worldWideNameHex, 17, "5%6s0%08X", oui, 0);

                char * serialNumberHex = worldWideNameHex+8;
                if (is_not_all_zeroes(serialNum)) {
                    // Is there a stretch of at least 8 hex digits in serialNum?
                    std::regex rightmostEightHexDigits(".*([A-Fa-f0-9]{8})");
                    std::cmatch match;
                    if (std::regex_match((const char *)device_info.serialNum,
                                         match,
                                         rightmostEightHexDigits)) {
                        strncpy(serialNumberHex, (const char *)(match.str(1).c_str()), 8);
                    } else {
                        // Synthesize WWN serial number as DtaHash of vendorID, modelNum, and serialNum
                        uint8_t uID[sizeof(device_info.vendorID)+ sizeof(device_info.modelNum)+1];
                        memcpy(&uID[0],
                               device_info.vendorID, sizeof(device_info.vendorID));
                        memcpy(&uID[sizeof(device_info.vendorID)],
                               device_info.modelNum, sizeof(device_info.modelNum));
                        vector<uint8_t> hash={4,0xd0, 0,0,0,0};
                        vector<uint8_t> salt(device_info.serialNum, device_info.serialNum+sizeof(device_info.serialNum));  // TODO: wrong -- getSalt()
                        DtaHashPassword(hash, (char *)uID, salt);
                        snprintf(serialNumberHex, 9, "%02X%02X%02X%02X", hash[2] , hash[3] , hash[4] , hash[5]);
                    }
                }
                
                
                for (size_t i=0; i<sizeof(device_info.worldWideName); i++) {
                    char nybbles[3];
                    nybbles[0]=worldWideNameHex[2*i  ];
                    nybbles[1]=worldWideNameHex[2*i+1];
                    nybbles[2]=0;
                    device_info.worldWideName[i] = (uint8_t)strtol(nybbles, NULL, 16);
                }

            }
        }
    }
#endif // defined( EXTEND_DTA_DEVICE_INFO_WITH_OUI_VENDOR_DATA )

    return;
}

    
// Factory for this class or subclass instances
    // Factory for this class or subclass instances
DtaDevMacOSBlockStorageDevice *
DtaDevMacOSBlockStorageDevice::getBlockStorageDevice(std::string entryName,
                                                     std::string bsdName,
                                                     CFDictionaryRef properties,
                                                     DTA_DEVICE_INFO * pdi) {

    CFDictionaryRef tPerProperties = GetPropertiesDict("TPer");
    DtaDevMacOSBlockStorageDevice * d = (NULL != tPerProperties)
        ? DtaDevMacOSTPer::getTPer(entryName, bsdName, tPerProperties, properties, pdi)
        : new DtaDevMacOSBlockStorageDevice(entryName, bsdName, properties, pdi);

    // Polishing up the DTA_DEVICE_INFO
    d->polishDeviceInfo();

    return d;
}
    
    
DtaDevMacOSBlockStorageDevice *
DtaDevMacOSBlockStorageDevice::getBlockStorageDevice(io_service_t aBlockStorageDevice,
                                                     const char *devref,
                                                     DTA_DEVICE_INFO *pdi) {
    DtaDevMacOSBlockStorageDevice * foundDevice = nil;
    io_service_t tPer;
    io_service_t media;
    CFDictionaryRef deviceProperties;
    CFDictionaryRef mediaProperties;
    CFDictionaryRef tPerProperties;
    CFDictionaryRef allProperties;
    
    const size_t kCStringSize = 128;
    char nameBuffer[kCStringSize];
    bzero(nameBuffer,kCStringSize);
    
    string entryName;
    string bsdName;
    
    
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
                       nameBuffer, (CFIndex)kCStringSize, kCFStringEncodingUTF8);
    if (strncmp(nameBuffer, devref, kCStringSize) == 0) {
        bsdName = string(nameBuffer);
        
        bzero(nameBuffer,kCStringSize);
        GetName(media,nameBuffer);
        entryName = string(nameBuffer);
        
        allProperties = CFDictionaryCreate(CFAllocatorGetDefault(),
                                           keys.data(), values.data(), (CFIndex)keys.size(),
                                           NULL, NULL);
        
        foundDevice = DtaDevMacOSBlockStorageDevice::getBlockStorageDevice(entryName,
                                                                           bsdName,
                                                                           allProperties,
                                                                           pdi);
    }
    
    IOObjectRelease(tPer);
    
finishedWithMedia:
    IOObjectRelease(media);
    
finishedWithDevice:
    IOObjectRelease(aBlockStorageDevice);
    
    return foundDevice;
}

DtaDevMacOSBlockStorageDevice *
DtaDevMacOSBlockStorageDevice::getBlockStorageDevice(const char * devref, DTA_DEVICE_INFO * pdi)
{
    DtaDevMacOSBlockStorageDevice * foundDevice = NULL;
    io_iterator_t iterator = findMatchingServices(kIOBlockStorageDeviceClass);
    io_service_t aBlockStorageDevice;
    
    // Iterate over nodes of class IOBlockStorageDevice or subclass thereof
    while ( foundDevice == NULL && (IO_OBJECT_NULL != (aBlockStorageDevice = IOIteratorNext( iterator )))) {
        foundDevice = getBlockStorageDevice(aBlockStorageDevice, devref, pdi);
    }
    return foundDevice;
}



DtaDevMacOSBlockStorageDevice::~DtaDevMacOSBlockStorageDevice () {
    if (properties != NULL )
        CFRelease(properties);
}

uint8_t DtaDevMacOSBlockStorageDevice::isAnySSC()
{
    return pdevice_info->ANY_OPAL_SSC;
}

const char * DtaDevMacOSBlockStorageDevice::getVendorID()
{
    return (const char *)&pdevice_info->vendorID;
}

const char * DtaDevMacOSBlockStorageDevice::getManufacturerName()
{
    return (const char *)&pdevice_info->manufacturerName;
}

const char * DtaDevMacOSBlockStorageDevice::getFirmwareRev()
{
    return (const char *)&pdevice_info->firmwareRev;
}

const char * DtaDevMacOSBlockStorageDevice::getModelNum()
{
    return (const char *)&pdevice_info->modelNum;
}

const char * DtaDevMacOSBlockStorageDevice::getSerialNum()
{
    return (const char *)&pdevice_info->serialNum;
}
    

const vector<uint8_t> DtaDevMacOSBlockStorageDevice::getPasswordSalt()
{
    const uint8_t * b=pdevice_info->passwordSalt;
    return vector<uint8_t>(b,b+sizeof(pdevice_info->passwordSalt));
}
    

const vector<uint8_t> DtaDevMacOSBlockStorageDevice::getWorldWideName()
{
    const uint8_t * b=pdevice_info->worldWideName;
    return vector<uint8_t>(b,b+sizeof(pdevice_info->worldWideName));
}

const char * DtaDevMacOSBlockStorageDevice::getPhysicalInterconnect()
{
    return (const char *)&pdevice_info->physicalInterconnect;
}
const char * DtaDevMacOSBlockStorageDevice::getPhysicalInterconnectLocation()
{
    return (const char *)&pdevice_info->physicalInterconnectLocation;
}
const char * DtaDevMacOSBlockStorageDevice::getBSDName()
{
    return (const char *)bsdName.c_str();
}

DTA_DEVICE_TYPE DtaDevMacOSBlockStorageDevice::getDevType()
{
    return pdevice_info->devType;
}

const std::string DtaDevMacOSBlockStorageDevice::getDevPath () {
    return ("/dev/"+bsdName).substr(0,25);
}

const unsigned long long DtaDevMacOSBlockStorageDevice::getSize() {
    return pdevice_info->devSize;
}
