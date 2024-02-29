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

#include <cstdint>
#include <cstring>
#include "os.h"
#include "log.h"
#include "DtaEndianFixup.h"
#include "DtaHexDump.h"
#include "DtaDevOSDrive.h"

#include "ParseDiscovery0Features.h"

uint8_t DtaDevOSDrive::discovery0(DTA_DEVICE_INFO & disk_info) {
  uint8_t d0Response[MIN_BUFFER_LENGTH]; // TODO: ALIGNMENT?
  memset(d0Response, 0, MIN_BUFFER_LENGTH);

  uint8_t lastRC = sendCmd(IF_RECV, 0x01, 0x0001, d0Response, MIN_BUFFER_LENGTH);
  if ((lastRC ) != 0) {
    LOG(D) << "Acquiring Discovery 0 response failed " << (uint16_t)lastRC;
    return DTAERROR_COMMAND_ERROR;
  }
  parseDiscovery0Features(d0Response, disk_info);
  return DTAERROR_SUCCESS;
}
