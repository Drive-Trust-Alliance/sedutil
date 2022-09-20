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
#include <SEDKernelInterface/SEDKernelInterface.h>
#include <IOKit/storage/IOBlockStorageDevice.h>
#include <IOKit/storage/IOMedia.h>
#include "string.h"
#include "DtaBlockStorageDevice.h"
//#include "UserKernelShared.h"


static void ReformatIdentifyResponseAsDiskInfo(const uint8_t * response, DTA_DEVICE_INFO& disk_info)
{
    const IDENTIFY_RESPONSE & id = * (const IDENTIFY_RESPONSE *) response;
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

// Some macros to access the properties dicts and values
#define GetDict(dict,name) (CFDictionaryRef)CFDictionaryGetValue(dict, CFSTR(name))
#define GetData(dict,name) (CFDataRef)CFDictionaryGetValue(dict, CFSTR(name))
#define GetString(dict,name) (CFStringRef)CFDictionaryGetValue(dict, CFSTR(name))
#define GetPropertiesDict(name) GetDict(properties, name)

void DtaBlockStorageDevice::parse_properties_into_disk_info() {
    memset((void *)&disk_info, 0, sizeof(disk_info));
    
    // Parse TPer properties, if available
    CFDictionaryRef tPerProperties = GetPropertiesDict("TPer");
    if (NULL != tPerProperties) {
        // Parse Identify Response, if any
        CFDataRef identifyResponseData = GetData(tPerProperties,"Identify Response");
        if (NULL != identifyResponseData) {
            const uint8_t * irDataBytes = CFDataGetBytePtr(identifyResponseData);
            if (NULL != irDataBytes) {
                ReformatIdentifyResponseAsDiskInfo(irDataBytes, disk_info);
            }
        }
        
        // Parse Discovery0 Response, if any
        CFDataRef discovery0Data = GetData(tPerProperties,"D0 Response");
        if (NULL != discovery0Data) {
            const uint8_t * discovery0 = CFDataGetBytePtr(discovery0Data);
            if (NULL != discovery0) {
                DtaDev::parseDiscovery0Features(discovery0, disk_info);
            }
        }
    } else {
        // TODO -- some devices make Identify info available via SMARTLIB interfaces
    }
    
    
    CFDictionaryRef deviceProperties = GetPropertiesDict("device");
    if (NULL != deviceProperties) {
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
    } else {
        disk_info.devType = DEVICE_TYPE_OTHER; // TODO -- generalize for other devices when they are supported by BPTperDriver
    }

    
}

// Sorting order
static inline
bool bsdNameLessThan(DtaBlockStorageDevice * a, DtaBlockStorageDevice * b) {
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

std::vector<DtaBlockStorageDevice *> DtaBlockStorageDevice::enumerateBlockStorageDevices() {
    std::vector<DtaBlockStorageDevice *>devices;
    io_iterator_t iterator = findMatchingServices(kIOBlockStorageDeviceClass);
    io_service_t device;
    io_service_t tPer;
    io_service_t media;
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
    while ( (IO_OBJECT_NULL != (device = IOIteratorNext( iterator )))) {
        
        std::vector<const void *>keys;
        std::vector<const void *>values;
        
        deviceProperties = copyProperties( device );
        if (NULL == deviceProperties) {
            goto finishedWithDevice;
        }
        keys.push_back( CFSTR("device"));
        values.push_back( deviceProperties);

        
        media = findServiceForClassInChildren(device, kIOMediaClass);
        if (IO_OBJECT_NULL == media) {
            goto finishedWithMedia;
        }
        mediaProperties = copyProperties( media );
        if (NULL == mediaProperties ) {
            goto finishedWithMedia ;
        }
        keys.push_back( CFSTR("media"));
        values.push_back( mediaProperties);


        tPer = findParent(device);
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
        

        devices.push_back( new DtaBlockStorageDevice(entryName, bsdName, properties) );
        
        IOObjectRelease(tPer);

    finishedWithMedia:
        IOObjectRelease(media);

    finishedWithDevice:
        IOObjectRelease(device);
    }
    
    sort(devices.begin(), devices.end(), bsdNameLessThan);
    return devices;
}

DtaBlockStorageDevice::~DtaBlockStorageDevice () {
    CFRelease(properties);
}

uint8_t DtaBlockStorageDevice::isOpal2()
{
    return disk_info.OPAL20;
}
uint8_t DtaBlockStorageDevice::isOpal1()
{
    return disk_info.OPAL10;
}
uint8_t DtaBlockStorageDevice::isEprise()
{
    return disk_info.Enterprise;
}

uint8_t DtaBlockStorageDevice::isAnySSC()
{
    return disk_info.ANY_OPAL_SSC;
}
uint8_t DtaBlockStorageDevice::MBREnabled()
{
    return disk_info.Locking_MBREnabled;
}
uint8_t DtaBlockStorageDevice::MBRDone()
{
    return disk_info.Locking_MBRDone;
}
uint8_t DtaBlockStorageDevice::Locked()
{
    return disk_info.Locking_locked;
}
uint8_t DtaBlockStorageDevice::LockingEnabled()
{
    return disk_info.Locking_lockingEnabled;
}
char *DtaBlockStorageDevice::getFirmwareRev()
{
    return (char *)&disk_info.firmwareRev;
}
char *DtaBlockStorageDevice::getModelNum()
{
    return (char *)&disk_info.modelNum;
}
char *DtaBlockStorageDevice::getSerialNum()
{
    return (char *)&disk_info.serialNum;
}

DTA_DEVICE_TYPE DtaBlockStorageDevice::getDevType()
{
    return disk_info.devType;
}
