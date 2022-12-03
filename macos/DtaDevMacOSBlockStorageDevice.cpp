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
#include <IOKit/storage/IOBlockStorageDevice.h>
#include <IOKit/storage/IOMedia.h>
#include <IOKit/storage/ata/ATASMARTLib.h>
#include <IOKit/storage/nvme/NVMeSMARTLibExternal.h>
#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CFNumber.h>
#include <string.h>

#include "oui_vendor.hpp"
#include "stdio.h"

#include <SEDKernelInterface/SEDKernelInterface.h>
#include "DtaDevMacOSTPer.h"

// Some macros to access the properties dicts and values
#define GetBool(dict,name) (CFBooleanRef)CFDictionaryGetValue(dict, CFSTR(name))
#define GetDict(dict,name) (CFDictionaryRef)CFDictionaryGetValue(dict, CFSTR(name))
#define GetData(dict,name) (CFDataRef)CFDictionaryGetValue(dict, CFSTR(name))
#define GetString(dict,name) (CFStringRef)CFDictionaryGetValue(dict, CFSTR(name))
#define GetNumber(dict,name) (CFNumberRef)CFDictionaryGetValue(dict, CFSTR(name))
#define GetPropertiesDict(name) GetDict(properties, name)

#define ERRORS_TO_STDERR
//#undef ERRORS_TO_STDERR


#if defined(TRY_SMART_LIBS)
static bool parseSMARTLibIdentifyData(io_service_t aBlockStorageDevice, CFDictionaryRef deviceProperties,
                                          DTA_DEVICE_INFO &disk_info)
{
    CFBooleanRef bSMARTCapable = GetBool(deviceProperties, kIOPropertySMARTCapableKey);
    if (!(bSMARTCapable != NULL))
        return false;
    if (!(CFGetTypeID(bSMARTCapable) == CFBooleanGetTypeID()))
        return false;
    if (!(CFBooleanGetValue(bSMARTCapable)))
        return false;
    
    CFDictionaryRef ioPluginTypes = GetDict( deviceProperties, kIOCFPlugInTypesKey);
    if (!(ioPluginTypes != NULL))
        return false;
    
    CFStringRef typeID= CFUUIDCreateString(CFAllocatorGetDefault(),kIOATASMARTUserClientTypeID);
    CFStringRef cfsrTypeName = (CFStringRef)CFDictionaryGetValue(ioPluginTypes, typeID);
    CFRelease(typeID);
    if (!(cfsrTypeName != NULL))
        return false;
    CFIndex typeNameLength = CFStringGetLength(cfsrTypeName);
    CFIndex typeNameMaxSize = CFStringGetMaximumSizeForEncoding(typeNameLength, kCFStringEncodingUTF8);
    char typeName[typeNameMaxSize];
            
    if (! CFStringGetCString(cfsrTypeName, typeName, typeNameMaxSize, kCFStringEncodingUTF8))
        return false;
    
    io_name_t entryName;
    IOReturn kr = IORegistryEntryGetName(aBlockStorageDevice, entryName);
    if ( !(kr == kIOReturnSuccess))
        return false;

    

    IOCFPlugInInterface     **plugInInterface = NULL;
    IONVMeSMARTInterface    **SMARTInterface = NULL;
    SInt32                  score;
    kr = IOCreatePlugInInterfaceForService(aBlockStorageDevice,
                                                    kIONVMeSMARTUserClientTypeID, kIOCFPlugInInterfaceID,
                                                    &plugInInterface, &score);
    
    if ((kIOReturnSuccess != kr) || plugInInterface == NULL) {
#if DEBUG && defined(ERRORS_TO_STDERR)
        fprintf(stderr, "IOCreatePlugInInterfaceForService returned 0x%08x -- typeName=%s entryName=%s\n", kr, typeName, entryName);   // TODO:  replace fprintf
#endif // DEBUG && defined(ERRORS_TO_STDERR)
        return false;
    }
#if DEBUG && defined(ERRORS_TO_STDERR)
    else {
        fprintf(stderr, "IOCreatePlugInInterfaceForService returned 0x%08x -- typeName=%s entryName=%s\n", kr, typeName, entryName);   // TODO:  replace fprintf
    }
#endif // DEBUG && defined(ERRORS_TO_STDERR)
    
    // Use the plugin interface to retrieve the device interface.
    HRESULT res = (*plugInInterface)->QueryInterface(plugInInterface, CFUUIDGetUUIDBytes(kIONVMeSMARTInterfaceID),
                                                     (LPVOID *)&SMARTInterface);
    // Now done with the plugin interface.
    (*plugInInterface)->Release(plugInInterface);
    
    if (0 != res || SMARTInterface == NULL) {
#if DEBUG && defined(ERRORS_TO_STDERR)
        fprintf(stderr, "QueryInterface returned %d.\n", (int) res);   // TODO:  replace fprintf
#endif // DEBUG && defined(ERRORS_TO_STDERR)
        return false;
    }
    uint32_t inNamespace = 0 ;
    IDENTIFY_RESPONSE data;
    
    kr = (*SMARTInterface) -> GetIdentifyData (SMARTInterface, &data, inNamespace );
    (*SMARTInterface)->Release(SMARTInterface);
    if ((kIOReturnSuccess != kr) ) {
#if DEBUG && defined(ERRORS_TO_STDERR)
        fprintf(stderr, "GetIdentifyData returned 0x%08x\n", kr);   // TODO:  replace fprintf
#endif // DEBUG && defined(ERRORS_TO_STDERR)
        return false;
    }
    
    memcpy(disk_info.serialNum, data.serialNumber, sizeof(disk_info.serialNum));
    memcpy(disk_info.firmwareRev, data.firmwareRevision, sizeof(disk_info.firmwareRev));
    memcpy(disk_info.modelNum, data.modelNum, sizeof(disk_info.modelNum));
    memcpy(disk_info.worldWideName, data.worldWideName, sizeof(disk_info.worldWideName));
    
    return true;
}


static bool parseNVMeSMARTLibIdentifyData(io_service_t aBlockStorageDevice, CFDictionaryRef deviceProperties,
                                          DTA_DEVICE_INFO &disk_info)
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
    CFStringRef cfsrTypeName = (CFStringRef)CFDictionaryGetValue(ioPluginTypes, typeID);
    CFRelease(typeID);
    if (!(cfsrTypeName != NULL))
        return false;
    CFIndex typeNameLength = CFStringGetLength(cfsrTypeName);
    CFIndex typeNameMaxSize = CFStringGetMaximumSizeForEncoding(typeNameLength, kCFStringEncodingUTF8);
    char typeName[typeNameMaxSize];
    if (! CFStringGetCString(cfsrTypeName, typeName, typeNameMaxSize, kCFStringEncodingUTF8))
        return false;

    io_name_t entryName;
    IOReturn kr = IORegistryEntryGetName(aBlockStorageDevice, entryName);
    if ( !(kr == kIOReturnSuccess))
        return false;


    IOCFPlugInInterface     **plugInInterface = NULL;
    IONVMeSMARTInterface    **NVMeSMARTInterface = NULL;
    SInt32                  score;
    kr = IOCreatePlugInInterfaceForService(aBlockStorageDevice,
                                                    kIONVMeSMARTUserClientTypeID, kIOCFPlugInInterfaceID,
                                                    &plugInInterface, &score);
    
    if ((kIOReturnSuccess != kr) || plugInInterface == NULL) {
#if DEBUG && defined(ERRORS_TO_STDERR)
        fprintf(stderr, "IOCreatePlugInInterfaceForService returned 0x%08x -- typeName=%s entryName=%s\n", kr, typeName, entryName);   // TODO:  replace fprintf
#endif // DEBUG && defined(ERRORS_TO_STDERR)
        return false;
    }
#if DEBUG && defined(ERRORS_TO_STDERR)
    else {
        fprintf(stderr, "IOCreatePlugInInterfaceForService returned 0x%08x -- typeName=%s entryName=%s\n", kr, typeName, entryName);   // TODO:  replace fprintf
    }
#endif // DEBUG && defined(ERRORS_TO_STDERR)

    // Use the plugin interface to retrieve the device interface.
    HRESULT res = (*plugInInterface)->QueryInterface(plugInInterface, CFUUIDGetUUIDBytes(kIONVMeSMARTInterfaceID),
                                                     (LPVOID *)&NVMeSMARTInterface);
    // Now done with the plugin interface.
    (*plugInInterface)->Release(plugInInterface);
    
    if (0 != res || NVMeSMARTInterface == NULL) {
#if DEBUG && defined(ERRORS_TO_STDERR)
        fprintf(stderr, "QueryInterface returned %d.\n", (int) res);   // TODO:  replace fprintf
#endif // DEBUG && defined(ERRORS_TO_STDERR)
        return false;
    }
    uint32_t inNamespace = 0 ;
    NVMeIdentifyControllerStruct data;
    
    kr = (*NVMeSMARTInterface) -> GetIdentifyData (NVMeSMARTInterface, &data, inNamespace );
    (*NVMeSMARTInterface)->Release(NVMeSMARTInterface);
    if ((kIOReturnSuccess != kr) ) {
#if DEBUG && defined(ERRORS_TO_STDERR)
        fprintf(stderr, "GetIdentifyData returned 0x%08x\n", kr);   // TODO:  replace fprintf
#endif // DEBUG && defined(ERRORS_TO_STDERR)
        return false;
    }
    
    memcpy(disk_info.serialNum, data.SERIAL_NUMBER, sizeof(disk_info.serialNum));  //TODO: Do these need byte flipping?
    memcpy(disk_info.firmwareRev, data.FW_REVISION, sizeof(disk_info.firmwareRev));
    memcpy(disk_info.modelNum, data.MODEL_NUMBER, sizeof(disk_info.modelNum));
    memcpy(disk_info.worldWideName, data.IEEE_OUI_ID, sizeof(disk_info.worldWideName));
    
    return true;
}
#endif // defined(TRY_SMART_LIBS)

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

#if defined(TRY_SMART_LIBS)
void DtaDevMacOSBlockStorageDevice::parse_properties_into_device_info(io_service_t aBlockStorageDevice) {
#else // !defined(TRY_SMART_LIBS)
void DtaDevMacOSBlockStorageDevice::parse_properties_into_device_info() {
#endif // defined(TRY_SMART_LIBS)
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
        
#if defined(TRY_SMART_LIBS)
        bool SMARTCapable = false;
        bool NVMeSMARTCapable = false;
#endif // defined(TRY_SMART_LIBS)
        
        CFDictionaryRef deviceProperties = GetPropertiesDict("device");
        if (NULL != deviceProperties) {
#if defined(TRY_SMART_LIBS)
            CFBooleanRef cfbrSMARTCapable = GetBool(deviceProperties, "SMART Capable");
            if (NULL != cfbrSMARTCapable) {
                SMARTCapable = CFBooleanGetValue(cfbrSMARTCapable);
            }
            CFBooleanRef cfbrNVMeSMARTCapable = GetBool(deviceProperties, "NVMe SMART Capable");
            if (NULL != cfbrNVMeSMARTCapable) {
                NVMeSMARTCapable = CFBooleanGetValue(cfbrNVMeSMARTCapable);
            }
#endif // defined(TRY_SMART_LIBS)
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
        
#if defined(TRY_SMART_LIBS)
        if (SMARTCapable &&
            parseSMARTLibIdentifyData(aBlockStorageDevice, deviceProperties, device_info) &&
            device_info.devType == DEVICE_TYPE_OTHER) {
            device_info.devType = DEVICE_TYPE_ATA;
        }
        
        if (NVMeSMARTCapable &&
            parseNVMeSMARTLibIdentifyData(aBlockStorageDevice, deviceProperties, device_info) &&
            device_info.devType == DEVICE_TYPE_OTHER) {
            device_info.devType = DEVICE_TYPE_NVME;
        }
#endif // defined(TRY_SMART_LIBS)
        
// TODO: ??
#undef HACK_MODELNUM_WITH_VENDORNAME
#if defined(HACK_MODELNUM_WITH_VENDORNAME)
        if (device_info.devType == DEVICE_TYPE_USB )
        {
            size_t vendorIDLength = strlen((const char *)device_info.vendorID);
            size_t modelNumLength = strlen((const char *)device_info.modelNum);
            size_t newModelNumLength = vendorIDLength + modelNumLength ;
            if (0 < newModelNumLength) {
                size_t maxModelNumLength = sizeof(device_info.modelNum);
                if (maxModelNumLength < newModelNumLength) {
                    size_t excessModelNumLength = newModelNumLength - maxModelNumLength ;
                    vendorIDLength -= excessModelNumLength;
                    newModelNumLength = maxModelNumLength;
                }
                uint8_t newModelNum[sizeof(device_info.modelNum)];
                bzero(newModelNum, sizeof(newModelNum));
                if (0 < vendorIDLength) {
                    memcpy(&newModelNum[0],device_info.vendorID,vendorIDLength);
                }
                if (0 < modelNumLength) {
                    memcpy(&newModelNum[vendorIDLength],device_info.modelNum,modelNumLength);
                }
                memcpy(device_info.modelNum,newModelNum,newModelNumLength);
            }
        }
#endif //defined(HACK_MODELNUM_WITH_VENDORNAME)


        return;
        
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
        DtaDevMacOSBlockStorageDevice * device;
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
        
        device =
#if defined(TRY_SMART_LIBS)
        getBlockStorageDevice(aBlockStorageDevice, entryNameStr, bsdNameStr, allProperties, pdi);
#else // !defined(TRY_SMART_LIBS)
        getBlockStorageDevice(entryNameStr, bsdNameStr, allProperties, pdi);
#endif // defined(TRY_SMART_LIBS)
        devices.push_back( device );
        
        IOObjectRelease(tPer);
        
    finishedWithMedia:
        IOObjectRelease(media);
        
    finishedWithDevice:
        IOObjectRelease(aBlockStorageDevice);
    }
    
    sort(devices.begin(), devices.end(), DtaDevMacOSBlockStorageDevice::bsdNameLessThan);
    return devices;
}

    
    
static __inline bool __is_not_all_zeros(const uint8_t * b, const size_t n) {
    for(const uint8_t * e = b + n; b<e; b++) {
        if (0!=*b)
            return true;
    }
    return false;
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

static void trimModelNum(DTA_DEVICE_INFO &device_info) {
    trimBuffer((char *)device_info.modelNum, sizeof(device_info.modelNum));
}

static void polishDeviceInfo(DTA_DEVICE_INFO *pdi) {
    DTA_DEVICE_INFO & device_info = *pdi;
        
#define TRIM_MODEL_NUMBER
#if defined( TRIM_MODEL_NUMBER )
    trimModelNum(device_info);
#endif // defined( TRIM_MODEL_NUMBER )
        
        
#define EXTEND_DTA_DEVICE_INFO_WITH_OUI_VENDOR_DATA
#if defined( EXTEND_DTA_DEVICE_INFO_WITH_OUI_VENDOR_DATA )
#define is_not_all_zeros(field) (__is_not_all_zeros(device_info.field, sizeof(device_info.field)))
    if (is_not_all_zeros(worldWideName)) {
        if (!is_not_all_zeros(manufacturerName)) {
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
        if (!is_not_all_zeros(vendorID)) {
            const char * vendorID =
            vendorID_for_vendor_canonically_if_necessary((const char *)device_info.manufacturerName);
            if (vendorID != NULL) {
                strncpy((char *)device_info.vendorID, vendorID, sizeof(device_info.vendorID));
            }
        }
    } else {
        if (is_not_all_zeros(vendorID)) {
            if (!is_not_all_zeros(manufacturerName)) {
                const char * vendor =
                vendor_for_vendorID_canonically_if_necessary((const char *)device_info.vendorID);
                if (vendor != NULL) {
                    strncpy((char *)device_info.manufacturerName, vendor, sizeof(device_info.manufacturerName));
                }
                // get oui from vendor/manufacturer name
                // make up worldWideName from those
            }
        }
    }
#endif // defined( EXTEND_DTA_DEVICE_INFO_WITH_OUI_VENDOR_DATA )
}

    
// Factory for this class or subclass instances
#if defined(TRY_SMART_LIBS)
DtaDevMacOSBlockStorageDevice *
DtaDevMacOSBlockStorageDevice::getBlockStorageDevice(io_service_t aBlockStorageDevice,
                                                     std::string entryName,
                                                     std::string bsdName,
                                                     CFDictionaryRef properties,
                                                     DTA_DEVICE_INFO * pdi) {

    CFDictionaryRef tPerProperties = GetPropertiesDict("TPer");
    DtaDevMacOSBlockStorageDevice * d = (NULL != tPerProperties)
        ? DtaDevMacOSTPer::getTPer(aBlockStorageDevice,
                                   entryName, bsdName, tPerProperties, properties, pdi)
        : new DtaDevMacOSBlockStorageDevice(aBlockStorageDevice,
                                            entryName, bsdName, properties, pdi);
    // Polishing up the DTA_DEVICE_INFO
    polishDeviceInfo(pdi);

    return d;
}

#else // ! defined(TRY_SMART_LIBS)

    
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
    polishDeviceInfo(pdi);

    return d;
}
#endif // ! defined(TRY_SMART_LIBS)


    
    
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
    bsdName = string(nameBuffer);
    
    if (bsdName == string(devref)) {
        bzero(nameBuffer,kCStringSize);
        GetName(media,nameBuffer);
        entryName = string(nameBuffer);
        
        allProperties = CFDictionaryCreate(CFAllocatorGetDefault(),
                                           keys.data(), values.data(), (CFIndex)keys.size(),
                                           NULL, NULL);
        
#if defined(TRY_SMART_LIBS)
        foundDevice = DtaDevMacOSBlockStorageDevice::getBlockStorageDevice(aBlockStorageDevice,
                                                                           entryName,
                                                                           bsdName,
                                                                           allProperties,
                                                                           pdi);
#else // !defined(TRY_SMART_LIBS)
        foundDevice = DtaDevMacOSBlockStorageDevice::getBlockStorageDevice(entryName,
                                                                           bsdName,
                                                                           allProperties,
                                                                           pdi);
#endif // defined(TRY_SMART_LIBS)
        
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

uint8_t DtaDevMacOSBlockStorageDevice::isOpal2()
{
    if (NULL == pdevice_info)
        return (uint8_t)NULL;
    else
        return pdevice_info->OPAL20;
}

uint8_t DtaDevMacOSBlockStorageDevice::isOpal1()
{
    return pdevice_info->OPAL10;
}

uint8_t DtaDevMacOSBlockStorageDevice::isEprise()
{
    return pdevice_info->Enterprise;
}

uint8_t DtaDevMacOSBlockStorageDevice::isAnySSC()
{
    return pdevice_info->ANY_OPAL_SSC;
}

uint8_t DtaDevMacOSBlockStorageDevice::MBREnabled()
{
    return pdevice_info->Locking_MBREnabled;
}

uint8_t DtaDevMacOSBlockStorageDevice::MBRDone()
{
    return pdevice_info->Locking_MBRDone;
}

uint8_t DtaDevMacOSBlockStorageDevice::Locked()
{
    return pdevice_info->Locking_locked;
}

uint8_t DtaDevMacOSBlockStorageDevice::LockingEnabled()
{
    return pdevice_info->Locking_lockingEnabled;
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

const std::string DtaDevMacOSBlockStorageDevice::getDevName () {
    return ("/dev/"+bsdName).substr(0,25);
}

const unsigned long long DtaDevMacOSBlockStorageDevice::getSize() {
    return pdevice_info->devSize;
}
