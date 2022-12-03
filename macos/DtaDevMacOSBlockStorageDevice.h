/* C:B**************************************************************************
This software is Copyright 2022 Bright Plaza Inc. <drivetrust@drivetrust.com>

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
#pragma once
#include <IOKit/IOKitLib.h>
#include <vector>
#include <string>
#include "DtaStructures.h"
#include "DtaDev.h"

/** virtual implementation for a block storage device
 */
//#define TRY_SMART_LIBS
#undef TRY_SMART_LIBS

class DtaDevMacOSBlockStorageDevice {
public:
#if defined(TRY_SMART_LIBS)
    DtaDevMacOSBlockStorageDevice(io_service_t aBlockStorageDevice, std::string entryName, std::string bsdName, CFDictionaryRef properties, DTA_DEVICE_INFO * pdi)
        : bsdName(bsdName),
          entryName(entryName),
          properties(properties),
          pdevice_info(pdi)
    {parse_properties_into_device_info(aBlockStorageDevice);};
#else // !defined(TRY_SMART_LIBS)
    DtaDevMacOSBlockStorageDevice(std::string entryName, std::string bsdName, CFDictionaryRef properties, DTA_DEVICE_INFO * pdi)
        : pdevice_info(pdi),
          bsdName(bsdName),
          entryName(entryName),
          properties(properties)
    {parse_properties_into_device_info();};
#endif // defined(TRY_SMART_LIBS)
    virtual ~DtaDevMacOSBlockStorageDevice();
    uint8_t isOpal2();
    /** Does the device conform to the OPAL 1.0 SSC */
    uint8_t isOpal1();
    /** Does the device conform to the OPAL Enterprise SSC */
    uint8_t isEprise();
    /** Does the device conform to ANY TCG storage SSC */
    uint8_t isAnySSC();
    /** Is the MBREnabled flag set */
    uint8_t MBREnabled();
    /** Is the MBRDone flag set */
    uint8_t MBRDone();
    /** Is the Locked flag set */
    uint8_t Locked();
    /** Is the Locking SP enabled */
    uint8_t LockingEnabled();
    /** Returns the Vendor ID reported by the Identify command */
    const char *getVendorID();
    /** Returns the Manufacturer Name reported by the Identify command */
    const char *getManufacturerName();
    /** Returns the Firmware revision reported by the identify command */
    const char *getFirmwareRev();
    /** Returns the Model Number reported by the Identify command */
    const char *getModelNum();
    /** Returns the Serial Number reported by the Identify command */
    const char *getSerialNum();
    /** Returns the Physical Interconnect technology */
    const char *getPhysicalInterconnect();
    /** Returns the Physical Interconnect Location */
    const char * getPhysicalInterconnectLocation();
    /** Returns the BSD Name */
    const char *getBSDName();
    /* What type of disk attachment is used */
    DTA_DEVICE_TYPE getDevType();

    const std::string getDevName ();

    const unsigned long long getSize();
    
    static std::vector<DtaDevMacOSBlockStorageDevice *> enumerateBlockStorageDevices();
    static DtaDevMacOSBlockStorageDevice * getBlockStorageDevice(io_service_t aBlockStorageDevice,
                                                     const char *devref,
                                                     DTA_DEVICE_INFO *pdi);
    
    static DtaDevMacOSBlockStorageDevice * getBlockStorageDevice(const char * devref, DTA_DEVICE_INFO * pdi);  // Factory for this class or subclass instances

#if defined(TRY_SMART_LIBS)
    static DtaDevMacOSBlockStorageDevice * getBlockStorageDevice(io_service_t aBlockStorageDevice,
                                                                 std::string entryName,
                                                                 std::string bsdName,
                                                                 CFDictionaryRef properties,
                                                                 DTA_DEVICE_INFO * pdi);  // Factory for this class or subclass instances
#else // !defined(TRY_SMART_LIBS)
    static DtaDevMacOSBlockStorageDevice * getBlockStorageDevice(std::string entryName,
                                                                 std::string bsdName,
                                                                 CFDictionaryRef properties,
                                                                 DTA_DEVICE_INFO * pdi);  // Factory for this class or subclass instances
#endif // defined(TRY_SMART_LIBS)
    
    const DTA_DEVICE_INFO & device_info(void);  /**< Weak reference to Structure containing info from properties, including identify and discovery 0 if available
                                           Asserts if no such reference.*/

private:
    DTA_DEVICE_INFO * pdevice_info;  /**< Weak reference to Structure containing info from properties, including identify and discovery 0 if available*/

    std::string bsdName;
    std::string entryName;
    CFDictionaryRef properties;
    
    // derived
#if defined(TRY_SMART_LIBS)
    void parse_properties_into_device_info(io_service_t aBlockStorageDevice);
#else // !defined(TRY_SMART_LIBS)
    void parse_properties_into_device_info(void);
#endif // defined(TRY_SMART_LIBS)
    static bool bsdNameLessThan(DtaDevMacOSBlockStorageDevice * a,
                                DtaDevMacOSBlockStorageDevice * b);  // for sorting
};
