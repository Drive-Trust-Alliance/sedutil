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


#include "DtaDevOSDrive.h"
#include "DtaDevLinuxDrive.h"

/** Factory functions
 *
 * Static class members of DtaDevOSDrive that are passed through
 * to DtaDevLinuxDrive
 *
 */


bool DtaDevOSDrive::isDtaDevOSDriveDevRef(const char * devref) {
  return DtaDevLinuxDrive::isDtaDevLinuxDriveDevRef(devref);
}

std::vector<std::string> DtaDevOSDrive::enumerateDtaDevOSDriveDevRefs() {
  return DtaDevLinuxDrive::enumerateDtaDevLinuxDriveDevRefs();
}

DtaDevOSDrive * DtaDevOSDrive::getDtaDevOSDrive(const char * devref,
                                                DTA_DEVICE_INFO &disk_info)
{
  return static_cast<DtaDevOSDrive *>(DtaDevLinuxDrive::getDtaDevLinuxDrive(devref, disk_info));
}



#include "DtaDevLinuxNvme.h"
#include "DtaDevLinuxScsi.h"


/** Factory functions
 *
 * Static class members that support instantiation of subclass members
 * with the subclass switching logic localized here for easier maintenance.
 *
 */


bool DtaDevLinuxDrive::isDtaDevLinuxDriveDevRef(const char * devref)
{
  return DtaDevLinuxNvme::isDtaDevLinuxNvmeDevRef(devref)
    ||   DtaDevLinuxScsi::isDtaDevLinuxScsiDevRef(devref) ;
}

DtaDevLinuxDrive * DtaDevLinuxDrive::getDtaDevLinuxDrive(const char * devref,
                                                         DTA_DEVICE_INFO &disk_info)
{
  DtaDevLinuxDrive * drive ;

  disk_info.devType = DEVICE_TYPE_OTHER;

  if ( (drive = DtaDevLinuxNvme::getDtaDevLinuxNvme(devref, disk_info)) != NULL )
    return drive ;
  //  LOG(D4) << "DtaDevLinuxNvme::getDtaDevLinuxNvme(\"" << devref <<  "\", disk_info) returned NULL";

  if ( (drive = DtaDevLinuxScsi::getDtaDevLinuxScsi(devref, disk_info)) != NULL )
    return drive ;
  // LOG(D4) << "DtaDevLinuxScsi::getDtaDevLinuxScsi(\"" << devref <<  "\", disk_info) returned NULL";

  return NULL ;
}
