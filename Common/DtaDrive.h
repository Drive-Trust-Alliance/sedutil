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

#include <string>
#include <map>
#include "log.h"
#include "DtaOS.h"
#include "DtaLexicon.h"
#include "DtaStructures.h"

typedef std::map<std::string,std::string>dictionary;
typedef std::map<std::string,std::string>::iterator dictionary_iterator;

/** virtual implementation for a disk interface-generic disk drive
 */
class DtaDrive {
public:
    typedef enum {
        BlockStorageDevice,
        AtaDrive,
        ScsiDrive,
        SataDrive,
        NvmeDrive,
        Unknown
    } Protocol;
    
    DtaDrive(OSDEVICEHANDLE _osDeviceHandle)
    : osDeviceHandle(_osDeviceHandle)
    {};
    
    // The next function must be implemented in an OS-specific build
    // as a pass-through since C++ does not have virtual static class functions.
    
    /** Factory function to look at the devref and create an instance of the appropriate subclass of
     *  DtaDrive
     *
     * @param devref OS device reference e.g. "/dev/sda" on a POSIX-style system
     * @param disk_info reference to DTA_DEVICE_INFO structure filled out during device identification
     */
    static DtaDrive * getDtaDrive(const char * devref,
                                  DTA_DEVICE_INFO & disk_info,
                                  bool& accessDenied);
    
    
    /** Method to send a command to the device
     * @param cmd command to be sent to the device
     * @param protocol security protocol to be used in the command
     * @param comID communications ID to be used
     * @param buffer input/output buffer
     * @param bufferlen length of the input/output buffer
     */
    virtual uint8_t sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                            void * buffer, unsigned int bufferlen) = 0;
    
    /** Routine to identify the device and fill out the device info structure.
     *
     * If it is an ATA device, perform an ATA Identify Device,
     * or if SCSI (SAS) , perform a SCSI Inquiry,
     * or if NVME, perform an NVMe Identify,
     * to fill out the disk_info structure
     * @param disk_info reference to the device info structure to fill out
     */
    virtual bool identify(DTA_DEVICE_INFO& disk_info) = 0;
    
    static bool identify(OSDEVICEHANDLE osDeviceHandle,
                         InterfaceDeviceID & interfaceDeviceIdentification,
                         DTA_DEVICE_INFO & device_info);
    
    virtual uint8_t discovery0(DTA_DEVICE_INFO & di);
    
    
    virtual ~DtaDrive() {
        LOG(D4) << "Destroying DtaDrive";
        closeDevice();
    }
    
    
    virtual bool isOpen(void) { return ( osDeviceHandle != INVALID_HANDLE_VALUE ) ;}
    
protected:
    using base_type = DtaDrive;
    
    void closeDevice(void){
        if (isOpen()) {
            OS.closeDeviceHandle(osDeviceHandle);
            osDeviceHandle = INVALID_HANDLE_VALUE;
        }
    }
    
    /** Factory function to look at the devref and create an instance of the appropriate subclass of
     *  DtaDrive
     *
     * @param p Protocol for this device
     * @param devref OS device reference e.g. "/dev/sda" on a POSIX-style system
     * @param disk_info reference to DTA_DEVICE_INFO structure filled out during device identification
     */
    static DtaDrive * getDtaDrive(Protocol p,
                                  const char * devref,
                                  DTA_DEVICE_INFO & disk_info);
    
public:  // *** TODO *** DEBUGGING *** this should just be protected
    OSDEVICEHANDLE osDeviceHandle = INVALID_HANDLE_VALUE;
};
