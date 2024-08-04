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

#include "DtaDrive.h"
#include <string>
#include <map>

typedef std::map<std::string,std::string>dictionary;
typedef std::map<std::string,std::string>::iterator dictionary_iterator;

/**  specific implementation SCSI generic ioctls to send commands to the
 * device
 */
class DtaBlockStorageDevice: public DtaDrive {
public:
    using DtaDrive::DtaDrive;


  /** Attempt an ATA security command TRUSTED_SEND/TRUSTED_RECEIVE to a BlockStorageDevice device
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

  ~DtaBlockStorageDevice(){}

};
