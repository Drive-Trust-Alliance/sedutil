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
#include "InterfaceDeviceID.h"
#include "DtaDevLinuxDrive.h"
#include <map>

typedef std::map<std::string,std::string>dictionary;

/** Linux specific implementation SCSI generic ioctls to send commands to the
 * device
 */
class DtaDevLinuxScsi: public DtaDevLinuxDrive {
public:

  /** Factory function to look at the devref and create an instance of
   *  (possibly the appropriate subclass of) DtaDevLinuxScsi, which will either be
   *  DtaDevLinuxScsi itself (for SAS drives) or
   *  DtaDevLinuxSata (SCSI/ATA translation for SATA drives)
   *    (if the device seems to know the SCSI ATA pass-through protocol)
   *
   *  Identification will be completed by attempting discovery0 and
   *  when successful parsing the results into `disk_info`
   *
   * @param devref OS device reference e.g. "/dev/sda"
   * @param disk_info weak reference to DTA_DEVICE_INFO structure filled out during device identification
   */
  static DtaDevLinuxScsi * getDtaDevLinuxScsi(const char * devref,
                                              DTA_DEVICE_INFO & disk_info);


  virtual uint8_t sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                          void * buffer, uint32_t bufferlen);

  virtual bool identify(DTA_DEVICE_INFO& disk_info);


protected:

  DtaDevLinuxScsi(int _fd)
    : DtaDevLinuxDrive(_fd)
  {}


  /** Perform a SCSI command using the SCSI generic interface. (static class function)
   *
   * @param fd              file descriptor of already-opened raw device file
   * @param dxfer_direction direction of transfer SG_DXFER_FROM/TO_DEV
   * @param cdb             SCSI command data buffer
   * @param cdb_len         length of SCSI command data buffer (often 12)
   * @param buffer          SCSI data buffer
   * @param bufferlen       SCSI data buffer len, also output transfer length
   * @param sense           SCSI sense data buffer
   * @param senselen        SCSI sense data buffer len (usually 32?)
   * @param pmasked_status  pointer to storage for masked_status, or NULL if not desired
   *
   * Returns the result of the ioctl call, as well as possibly setting *pmasked_status
   */
  static int PerformSCSICommand(int fd,
                                int dxfer_direction,
                                uint8_t * cdb,   unsigned char cdb_len,
                                void * buffer,   unsigned int& bufferlen,
                                uint8_t * sense, unsigned char senselen,
                                unsigned char * pmasked_status);


  /** Perform a SCSI command using the SCSI generic interface. (member function)
   *
   * @param dxfer_direction direction of transfer SG_DXFER_FROM/TO_DEV
   * @param cdb             SCSI command data buffer
   * @param cdb_len         length of SCSI command data buffer (often 12)
   * @param buffer          SCSI data buffer
   * @param bufferlen       SCSI data buffer len, also output transfer length
   * @param sense           SCSI sense data buffer
   * @param senselen        SCSI sense data buffer len (usually 32?)
   * @param pmasked_status  pointer to storage for masked_status, or NULL if not desired
   *
   * Returns the result of the ioctl call, as well as possibly setting *pmasked_status
   */
  int PerformSCSICommand(int dxfer_direction,
                         uint8_t * cdb,   unsigned char cdb_len,
                         void * buffer,   unsigned int& bufferlen,
                         uint8_t * sense, unsigned char senselen,
                         unsigned char * pmasked_status)
  {
    return PerformSCSICommand(fd,
                              dxfer_direction,
                              cdb, cdb_len,
                              buffer, bufferlen,
                              sense, senselen,
                              pmasked_status);
  }


private:
  static
  bool identifyUsingSCSIInquiry(int fd,
                                InterfaceDeviceID interfaceDeviceIdentification,
                                DTA_DEVICE_INFO & disk_info);
  static
  bool deviceIsStandardSCSI(int fd,
                            InterfaceDeviceID & interfaceDeviceIdentification,
                            DTA_DEVICE_INFO & disk_info);

  static
  int inquiryStandardDataAll_SCSI(int fd, void * inquiryResponse, size_t dataSize );

  static
  int __inquiry(int fd, uint8_t evpd, uint8_t page_code, void * buffer, size_t & dataSize);


  static
  dictionary *
  parseInquiryStandardDataAllResponse(const unsigned char * response,
                                      InterfaceDeviceID & interfaceDeviceIdentification,
                                      DTA_DEVICE_INFO & di);

};
