/* C:B**************************************************************************
This software is Copyright 2014-2017 Bright Plaza Inc. <drivetrust@drivetrust.com>

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

/** virtual implementation for a disk interface-generic disk drive
 */
class DtaDevLinuxDrive {
public:
  virtual ~DtaDevLinuxDrive( void ) {};

  /**Initialization.
   * This function should perform the necessary authority and environment checking
   * to allow proper functioning of the program, open the device,
   * @param devref character representation of the device is standard OS lexicon
   */
  virtual bool init(const char * devref) = 0;

  /** Method to send a command to the device
   * @param cmd command to be sent to the device
   * @param protocol security protocol to be used in the command
   * @param comID communications ID to be used
   * @param buffer input/output buffer
   * @param bufferlen length of the input/output buffer
   */
  virtual uint8_t sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                          void * buffer, uint32_t bufferlen) = 0;

  /** Routine to identify the device and fill out the device info structure.
   *
   * If it is an ATA device, perform an ATA Identify,
   * or if SCSI (SAS) , perform a SCSI Inquiry,
   * or if NVME, perform an NVMe *** TODO FIXME WHAT IS THE NVME COMMAND CALLED,
   * and perform a call to discovery0() to complete the disk_info structure
   * @param disk_info reference to the device info structure to fill out
   */
  virtual void identify(DTA_DEVICE_INFO& disk_info) = 0;

  /** Factory function to look at the devref and create an instance of the appropriate subclass of
   *  DtaDevLinuxDrive
   *
   * @param devref OS device reference e.g. "/dev/sda"
   */
  static DtaDevLinuxDrive * getDtaDevLinuxDriveSubclassInstance(const char * devref);

protected:
  uint8_t discovery0(DTA_DEVICE_INFO & di);
};
