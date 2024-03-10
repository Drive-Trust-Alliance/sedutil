/* C:B**************************************************************************
This software is Copyright (c) 2014-2024 Bright Plaza Inc. <drivetrust@drivetrust.com>

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
#include "DtaDevMacOSDrive.h"
#include <string>
#include <map>

typedef std::map<std::string,std::string>dictionary;
typedef std::map<std::string,std::string>::iterator dictionary_iterator;

/** MacOS specific implementation SCSI generic ioctls to send commands to the
 * device
 */
class DtaDevMacOSBlockStorageDevice: public DtaDevMacOSDrive {
public:
    using DtaDevMacOSDrive::DtaDevMacOSDrive;
    
  /** Factory function to look at the devref to filter whether it could be an instance
   *
   * @param devref OS device reference e.g. "/dev/sda"
   */
  static bool isDtaDevMacOSBlockStorageDeviceDevRef(const char * devref);

  /** Attempt an ATA security command IF_SEND/IF_RECV to a BlockStorageDevice device
   *  (Note that Sata devices are a separate subclass.)
   */
  virtual uint8_t sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                      void * buffer, unsigned int bufferlen);


  /** Identify this device using SCSI Inquiry Standard Data All command
   *  to obtain data to fill out disk_info.
   *  (Note that Sata devices are a separate subclass.)
   */
  virtual bool identify(DTA_DEVICE_INFO& disk_info);

  virtual uint8_t discovery0(DTA_DEVICE_INFO & di);

  ~DtaDevMacOSBlockStorageDevice(){}


      /** Factory function to enumerate all the devrefs that pass the above filter
       *
       */
    static
    std::vector<std::string> enumerateDtaDevMacOSBlockStorageDeviceDevRefs(void);

    static
  bool identifyUsingSCSIInquiry(io_connect_t connection,
                                InterfaceDeviceID & interfaceDeviceIdentification,
                                DTA_DEVICE_INFO & disk_info);

    /** Factory function to look at the devref and create an instance of
     *  (possibly the appropriate subclass of) DtaDevMacOSBlockStorageDevice, which will either be
     *  DtaDevMacOSBlockStorageDevice itself (for SAS drives) or
     *  DtaDevMacOSSata (SCSI/ATA translation for SATA drives)
     *    (if the device seems to know the SCSI ATA pass-through protocol)
     *
     * @param driverService I/O registry entry for block storage device node from which to fill out `disk_info'`
     * @param disk_info reference to DTA_DEVICE_INFO structure filled out 
     */
    
    
    
    // Get info from I/O Registry
    static
    bool BlockStorageDeviceUpdate(io_registry_entry_t driverService, DTA_DEVICE_INFO & disk_info);

    DtaDevMacOSBlockStorageDevice(io_registry_entry_t dS)
    : DtaDevMacOSDrive::DtaDevMacOSDrive(dS, IO_OBJECT_NULL)
    {};
};
