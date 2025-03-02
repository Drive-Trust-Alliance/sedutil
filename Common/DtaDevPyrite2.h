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

#include "DtaDevPyrite.h"

using namespace std;
/** Class representing a disk device, this class is represents a disk that conforms
 * to the OPAL 2.0 SSC
 *
 * testing so far indicates that the functions implemented in this program
 * function the same in OPAL 1.0 and Pyrite 2.0
 */
class DtaDevPyrite2 : public DtaDevPyrite {
public:
  ~DtaDevPyrite2();

  DtaDevPyrite2(const char * devref);

    /** return the communication ID to be used with this device */
  uint16_t comID();

  // /** Short-circuit routine re-uses initialized drive and disk_info */
  DtaDevPyrite2(const char * devref, DtaDrive * drive, DTA_DEVICE_INFO& di)
    : DtaDevPyrite(devref, drive, di)
  {} ;

};
