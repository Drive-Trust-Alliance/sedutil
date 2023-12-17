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
#pragma once

#if defined(__APPLE__) && defined(__MACH__)

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
            void * buffer, size_t bufferlen) = 0;
    
    /** Mac specific routine to identify the device
     * @param disk_info  DTA_DEVICE_INFO struct describing TPer
    */
    virtual kern_return_t identify(DTA_DEVICE_INFO& disk_info);

    /** Initialize a drive object for use
     * @param dev the bsdname (i.e. "disk3") of the device
     * @param doConnect whether to leave the device connected
     */
    virtual bool init(const char * dev, bool doConnect=false);

    /** Initialize a drive object for use
     * @param driverService   IO Registry entry for the driver
     * @param connect IO Connection to the driver
     *
     *  We do not own the driverService or the connect.  No error checking.
     */
    virtual void init(io_registry_entry_t driverService,
                      io_connect_t connect);

    /** Does the device conform to the OPAL 2 SSC */
    uint8_t isOpal2();
    /** Does the device conform to the OPAL 1.0 SSC */
    uint8_t isOpal1();
    /** Does the device conform to the OPAL Enterprise SSC */
    uint8_t isEprise();
    /** Is the MBREnabled flag set */
    uint8_t MBREnabled();
    /** Is the MBRDone flag set */
    uint8_t MBRDone();
    /** Is the Locked flag set */
    uint8_t Locked();
    /** Is the Locking SP enabled */
    uint8_t LockingEnabled();

    static DtaDevMacOSTPer * getTPer(std::string entryName,
                                     std::string bsdName,
                                     CFDictionaryRef tPerProperties,
                                     CFDictionaryRef properties,
                                     DTA_DEVICE_INFO * pdi) ;

    void GetIOObjects(io_registry_entry_t & ds, io_connect_t &c) {
        ds=driverService;
        c=connect;
    }
    void ClearOwnedIOObjects();
    void ReleaseOwnedIOObjects();

protected:
    bool findDriverService(const char * dev);
    io_registry_entry_t driverService;
    bool ownDriverService;

    uint8_t connectToUserClient();
    io_connect_t    connect;
    bool ownConnect;

};

#endif // defined(__APPLE__) && defined(__MACH__)
