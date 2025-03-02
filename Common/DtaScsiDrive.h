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

#include "DtaDrive.h"

/**  specific implementation SCSI generic ioctls to send commands to the
 * device
 */
class DtaScsiDrive: public DtaDrive {
public:
    using DtaDrive::DtaDrive;

    DtaScsiDrive(OSDEVICEHANDLE _osDeviceHandle)
      : DtaDrive(_osDeviceHandle)
    {}

    static DtaScsiDrive * getDtaOSSpecificScsiDrive(OSDEVICEHANDLE _osDeviceHandle);
  /** Factory function to look at the devref and create an instance of
   *  (possibly the appropriate subclass of) DtaDevScsi, which will either be
   *  DtaDevScsi itself (for SAS drives) or
   *  DtaDevSata (SCSI/ATA translation for SATA drives)
   *    (if the device seems to know the SCSI ATA pass-through protocol)
   *
   * @param devref OS device reference e.g. "/dev/sda"
   * @param disk_info reference to DTA_DEVICE_INFO structure filled out during device identification
   */
  static DtaDrive * getDtaScsiDrive(const char * devref,
                                    DTA_DEVICE_INFO & disk_info);


  /** Attempt an ATA security command TRUSTED_SEND/TRUSTED_RECEIVE to a Scsi device
   *  (Note that Sata devices are a separate subclass.)
   */
  virtual uint8_t sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                      void * buffer, unsigned int bufferlen);


  /** Identify this device using SCSI Inquiry Standard Data All command
   *  to obtain data to fill out disk_info.
   *  (Note that Sata devices are a separate subclass.)
   */
  virtual bool identify(DTA_DEVICE_INFO& disk_info);

  ~DtaScsiDrive(){}

  static
  bool identifyUsingSCSIInquiry(OSDEVICEHANDLE osDeviceHandle,
                                InterfaceDeviceID & interfaceDeviceIdentification,
                                DTA_DEVICE_INFO & disk_info);


private:
  static
  bool deviceIsStandardSCSI(OSDEVICEHANDLE osDeviceHandle,
                            InterfaceDeviceID & interfaceDeviceIdentification,
                            DTA_DEVICE_INFO & disk_info);

  static
  int inquiryStandardDataAll_SCSI(OSDEVICEHANDLE osDeviceHandle, void * inquiryResponse, unsigned int & dataSize );


  static
  int __inquiry(OSDEVICEHANDLE osDeviceHandle, uint8_t evpd, uint8_t page_code, void * buffer, unsigned int & dataSize);


  static
  dictionary *
  parseInquiryStandardDataAllResponse(const unsigned char * response,
                                      InterfaceDeviceID & interfaceDeviceIdentification,
                                      DTA_DEVICE_INFO & di);

};
