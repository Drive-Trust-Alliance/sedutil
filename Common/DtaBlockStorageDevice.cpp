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


#include "log.h"

#include "DtaBlockStorageDevice.h"



/** Send an ioctl to the device using pass through. */
uint8_t DtaBlockStorageDevice::sendCmd(ATACOMMAND /* cmd */, uint8_t /* protocol */, uint16_t /* comID */,
                                 void * /* buffer */, unsigned int /* bufferlen */)
{
  LOG(D4) << "Entering DtaDevBlockStorageDevice::sendCmd";


  LOG(D4) << "Returning 0xff from DtaDevBlockStorageDevice::sendCmd";
  return 0xff;
}


bool DtaBlockStorageDevice::identify(DTA_DEVICE_INFO& /* device_info */) {
    return false;
}

uint8_t DtaBlockStorageDevice::discovery0(DTA_DEVICE_INFO& /* device_info */ ) {
    return 0xff;
}
