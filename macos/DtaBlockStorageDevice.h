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
class DtaBlockStorageDevice {
public:
    DtaBlockStorageDevice(std::string entryName, std::string bsdName, CFDictionaryRef properties)
        : bsdName(bsdName),
          entryName(entryName),
          properties(properties)
        {parse_properties_into_disk_info();};
    virtual ~DtaBlockStorageDevice();
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
    /** Returns the Firmware revision reported by the identify command */
    char *getFirmwareRev();
    /** Returns the Model Number reported by the Identify command */
    char *getModelNum();
    /** Returns the Serial Number reported by the Identify command */
    char *getSerialNum();
    /* What type of disk attachment is used */
    DTA_DEVICE_TYPE getDevType();
    /** displays the information returned by the Discovery 0 reply */

    static std::vector<DtaBlockStorageDevice *> enumerateBlockStorageDevices();
    std::string bsdName;
    std::string entryName;
private:
    CFDictionaryRef properties;
    // derived
    void parse_properties_into_disk_info();
    DTA_DEVICE_INFO disk_info;  /**< Structure containing info from properties, including identify and discovery 0 if available*/
};
