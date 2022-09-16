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
#pragma once

#include "DtaDevMacOSBlockStorageDevice.h"


/** virtual implementation for a disk interface-generic TPer
 */
class DtaDevMacOSTPer : public DtaDevMacOSBlockStorageDevice {
public:
    using DtaDevMacOSBlockStorageDevice::DtaDevMacOSBlockStorageDevice;

    virtual ~DtaDevMacOSTPer();

    /** Method to send a command to the device
     * @param cmd command to be sent to the device
     * @param protocol security protocol to be used in the command
     * @param comID communications ID to be used
     * @param buffer input/output buffer
     * @param bufferlen length of the input/output buffer
     */
    virtual uint8_t sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
            void * buffer, uint16_t bufferlen) = 0;
    
    /** Mac specific routine to identify the device
     * @param disk_info  DTA_DEVICE_INFO struct describing TPer
    */
    virtual uint8_t identify(DTA_DEVICE_INFO& disk_info);

    /** Initialize a drive object for use
     * @param dev the bsdname (i.e. "disk3") of the device
     * @param doConnect whether to leave the device connected
     */
    virtual bool init(const char * dev, bool doConnect=false);

    /** Initialize a drive object for use
     * @param driverService the IO Registry entry for the driver
     * @param connect the IOKit IOObject representing the connection to wrap
     */
    virtual void init(io_registry_entry_t driverService, io_connect_t connect);
    
    static DtaDevMacOSTPer * getTPer(io_service_t aBlockStorageDevice,
                                     std::string entryName,
                                     std::string bsdName,
                                     CFDictionaryRef tPerProperties,
                                     CFDictionaryRef properties,
                                     DTA_DEVICE_INFO * pdi) ;


protected:
    bool findBrightPlazaDriverService(const char * dev);
    io_registry_entry_t driverService;
    bool ownDriverService;

    uint8_t connectToUserClient();
    io_connect_t    connect;
    bool ownConnect;

    void ClearOwnedIOObjects();
    void ReleaseOwnedIOObjects();
};
