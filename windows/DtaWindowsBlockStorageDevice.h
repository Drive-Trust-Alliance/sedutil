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
#pragma once
#include "DtaStructures.h"
#include "DtaBlockStorageDevice.h"

/** Windows specific implementation SCSI generic ioctls to send commands to the
 * device
 */
class DtaWindowsBlockStorageDevice: public DtaBlockStorageDevice {
public:
    using DtaBlockStorageDevice::DtaBlockStorageDevice;
    /** Factory function to look at the devref and create an instance of
     *  DtaDevWindowsBlockStorageDevice itself (for unrecognized drives)
     *
     * @param devref OS device reference e.g. "/dev/sda"
     * @param disk_info reference to DTA_DEVICE_INFO structure filled out during device identification
     */
    static
    DtaWindowsBlockStorageDevice *
    getDtaWindowsBlockStorageDevice(const char * devref,
                                  DTA_DEVICE_INFO & disk_info);


  virtual uint8_t discovery0(DTA_DEVICE_INFO & di);

  ~DtaWindowsBlockStorageDevice(){}


    static bool identify(OSDEVICEHANDLE osDeviceHandle,
        const char* devref,
        InterfaceDeviceID & interfaceDeviceIdentification,
                         DTA_DEVICE_INFO & device_info);

};
