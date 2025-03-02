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


#include "os.h"
#include "log.h"



#include "DtaDevOpal1.h"
#include "DtaDevOpal2.h"
#include "DtaDevPyrite1.h"
#include "DtaDevPyrite2.h"
#include "DtaDevEnterprise.h"
#include "DtaDevGeneric.h"



/** Factory functions
 *
 * Static class members that support instantiation of subclass members
 * with the subclass switching logic localized here for easier maintenance.
 */

/**
 * This method is invoked after the instantiation process for `drive`
 * has initialized it and filled out the device information in `di`.
 *
 * The initial test for whether the drive is a `Tper` (`drive->discovery0(di)`)
 * could potentially also as a side-effect further fill out fields in `di`.
 */
DtaDev* DtaDev::getDtaDev(const char * devref,
                                DtaDrive * drive,
                                DTA_DEVICE_INFO & di,
                                bool genericIfNotTPer) {
  LOG(D4) << "DtaDev::getDtaDev("
          << "devref="           << "\"" << devref << "\""                      << ", "
          << "drive"                                                            << ", "
          << "di="               << HEXON(8) << reinterpret_cast<intptr_t>(&di) << ", "
          << "genericIfNotTPer=" << std::boolalpha << genericIfNotTPer          << ")"
          << " (in " << __FILE__ << ")";
  if (DTAERROR_SUCCESS == drive->discovery0(di)) {  // drive responds to most basic TRUSTED_RECEIVE
    LOG(D4) << "DtaDev::getDtaDev discovery0 succeeded.";
    if (di.OPAL20)     return new DtaDevOpal2(devref, drive, di);
    if (di.OPAL10)     return new DtaDevOpal1(devref, drive, di);
    if (di.PYRITE2)    return new DtaDevPyrite2(devref, drive, di);
    if (di.PYRITE)     return new DtaDevPyrite1(devref, drive, di);
    if (di.Enterprise) return new DtaDevEnterprise(devref, drive, di);
    //  if (di.RUBY) ...  etc.
    LOG(D4) << "DtaDev::getDtaDev no known SSC -- trying generic.";
  } else {
    LOG(D4) << "DtaDev::getDtaDev discovery0 failed -- trying generic.";
  }
  if (genericIfNotTPer) return new DtaDevGeneric(devref, drive, di);
  LOG(D4) << "DtaDev::getDtaDev genericIfNotTper is false -- returning NULL.";
  return NULL;
}
