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


#include "DtaDev.h"
#include "DtaDevOS.h"


/** Factory functions
 *
 * Static class member that passes through DtaDev instantiaion to DtaDevOS.
 *
 */


uint8_t DtaDev::getDtaDev(const char * devref, DtaDev * & device, bool genericIfNotTPer)
{
  DtaDevOS * d;
  uint8_t result = DtaDevOS::getDtaDevOS(devref, d, genericIfNotTPer);
  if (result == DTAERROR_SUCCESS) {
    device = static_cast<DtaDev *>(d);
  }
  return result;
}




#include "DtaDevOpal1.h"
#include "DtaDevOpal2.h"
#include "DtaDevEnterprise.h"
#include "DtaDevGeneric.h"



/** Factory functions
 *
 * Static class members that support instantiation of subclass members
 * with the subclass switching logic localized here for easier maintenance.
 *
 * This method is invoked after the instantiation process for `drive`
 * has initialized it and filled out the device information in `di`.
 */


DtaDevOS* DtaDevOS::getDtaDevOS(const char * devref,
                                DtaDevOSDrive * drive,
                                DTA_DEVICE_INFO & di,
                                bool genericIfNotTPer) {
  if (DTAERROR_SUCCESS == drive->discovery0(di)) {  // drive responds to most basic IF_RECV
    if (di.OPAL20)        return new DtaDevOpal2(devref, drive, di);
    if (di.OPAL10)        return new DtaDevOpal1(devref, drive, di);
    if (di.Enterprise)    return new DtaDevEnterprise(devref, drive, di);
    //  if (di.RUBY) ...  etc.
  }
  if (genericIfNotTPer) return new DtaDevGeneric(devref, drive, di);

  LOG(E) << "Unknown OPAL SSC ";
  return NULL;
}
