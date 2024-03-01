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
class DtaDevMacOSScsi: public DtaDevMacOSDrive {
public:

  /** Factory function to look at the devref to filter whether it could be an instance
   *
   * @param devref OS device reference e.g. "/dev/sda"
   */
  static bool isDtaDevMacOSScsiDevRef(const char * devref);

  /** Factory function to look at the devref and create an instance of
   *  (possibly the appropriate subclass of) DtaDevMacOSScsi, which will either be
   *  DtaDevMacOSScsi itself (for SAS drives) or
   *  DtaDevMacOSSata (SCSI/ATA translation for SATA drives)
   *    (if the device seems to know the SCSI ATA pass-through protocol)
   *
   * @param devref OS device reference e.g. "/dev/sda"
   * @param disk_info weak reference to DTA_DEVICE_INFO structure filled out during device identification
   */
  static DtaDevMacOSScsi * getDtaDevMacOSScsi(const char * devref,
                                              DTA_DEVICE_INFO & disk_info);


  /** Attempt an ATA security command IF_SEND/IF_RECV to a Scsi device
   *  (Note that Sata devices are a separate subclass.)
   */
  virtual uint8_t sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                      void * buffer, unsigned int bufferlen);


  /** Identify this device using SCSI Inquiry Standard Data All command
   *  to obtain data to fill out disk_info.
   *  (Note that Sata devices are a separate subclass.)
   */
  virtual bool identify(DTA_DEVICE_INFO& disk_info);

  DtaDevMacOSScsi(int _fd)
    : DtaDevMacOSDrive(_fd)
  {}

  ~DtaDevMacOSScsi(){}

  static
  bool identifyUsingSCSIInquiry(int fd,
                                InterfaceDeviceID & interfaceDeviceIdentification,
                                DTA_DEVICE_INFO & disk_info);

protected:

    // Stolen from sg.h in case we are not Linux ...
#if !defined(SG_DXFER_TO_DEV)
#define SG_DXFER_TO_DEV -2      /* e.g. a SCSI WRITE command */
#endif  // !defined(SG_DXFER_TO_DEV)
#if !defined(SG_DXFER_FROM_DEV)
#define SG_DXFER_FROM_DEV -3    /* e.g. a SCSI READ command */
#endif  // !defined(SG_DXFER_FROM_DEV)

#define PSC_TO_DEV   SG_DXFER_TO_DEV
#define PSC_FROM_DEV SG_DXFER_FROM_DEV

  /** Perform a SCSI command using the SCSI generic interface. (member function)
   *
   * @param dxfer_direction direction of transfer PSC_FROM/TO_DEV
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
                         unsigned char * sense, unsigned char& senselen,
                         unsigned char * pmasked_status=NULL)
  {
    return DtaDevMacOSScsi::PerformSCSICommand(this->fd,
                                               dxfer_direction,
                                               cdb, cdb_len,
                                               buffer, bufferlen,
                                               sense, senselen,
                                               pmasked_status);
  }


protected:
  /** Perform a SCSI command using the SCSI generic interface. (static class function)
   *
   * @param fd              file descriptor of already-opened raw device file
   * @param dxfer_direction direction of transfer PSC_FROM/TO_DEV
   * @param cdb             SCSI command data buffer
   * @param cdb_len         length of SCSI command data buffer (often 12)
   * @param buffer          SCSI data buffer
   * @param bufferlen       SCSI data buffer len, also output transfer length
   * @param sense           SCSI sense data buffer
   * @param senselen        SCSI sense data buffer len (usually 32?)
   * @param pmasked_status  pointer to storage for masked_status, or NULL if not desired
   * @param timeout         optional timeout (in msecs)
   *
   * Returns the result of the ioctl call, as well as possibly setting *pmasked_status
   */
  static int PerformSCSICommand(int fd,
                                int dxfer_direction,
                                uint8_t * cdb,   unsigned char cdb_len,
                                void * buffer,   unsigned int& bufferlen,
                                unsigned char * sense, unsigned char & senselen,
                                unsigned char * pmasked_status,
                                unsigned int timeout=60000);

private:
  static
  bool deviceIsStandardSCSI(int fd,
                            InterfaceDeviceID & interfaceDeviceIdentification,
                            DTA_DEVICE_INFO & disk_info);

  static
  int inquiryStandardDataAll_SCSI(int fd, void * inquiryResponse, unsigned int & dataSize );


  static
  int __inquiry(int fd, uint8_t evpd, uint8_t page_code, void * buffer, unsigned int & dataSize);


  static
  dictionary *
  parseInquiryStandardDataAllResponse(const unsigned char * response,
                                      InterfaceDeviceID & interfaceDeviceIdentification,
                                      DTA_DEVICE_INFO & di);

};


/*
 *  Status codes
 */

#define GOOD                 0x00
#define CHECK_CONDITION      0x01
#define CONDITION_GOOD       0x02
#define BUSY                 0x04
#define INTERMEDIATE_GOOD    0x08
#define INTERMEDIATE_C_GOOD  0x0a
#define RESERVATION_CONFLICT 0x0c
#define COMMAND_TERMINATED   0x11
#define QUEUE_FULL           0x14

#define STATUS_MASK          0x3e

#define CaseForStatus(status) case status: return std::string( #status )
static inline std::string statusName(unsigned char statusValue)
{
  switch (statusValue)
  {
    CaseForStatus( GOOD                 );
    CaseForStatus( CHECK_CONDITION      );
    CaseForStatus( CONDITION_GOOD       );
    CaseForStatus( BUSY                 );
    CaseForStatus( INTERMEDIATE_GOOD    );
    CaseForStatus( INTERMEDIATE_C_GOOD  );
    CaseForStatus( RESERVATION_CONFLICT );
    CaseForStatus( COMMAND_TERMINATED   );
    CaseForStatus( QUEUE_FULL           );
  default: return std::string("????");
  }
}
#undef CaseForStatus
