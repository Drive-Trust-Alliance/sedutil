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
#if defined(NVME)
#include "DtaDevMacOSNvme.h"
#endif  // defined(NVME)
#include "DtaDevMacOSScsi.h"


/** Factory functions
 *
 * Static class members that support instantiation of subclass members
 * with the subclass switching logic localized here for easier maintenance.
 *
 */

bool DtaDevMacOSDrive::isDtaDevMacOSDriveDevRef(const char * devref)
{
  return
#if defined(NVME)
          DtaDevMacOSNvme::isDtaDevMacOSNvmeDevRef(devref) ||
#endif  // defined(NVME)
          DtaDevMacOSScsi::isDtaDevMacOSScsiDevRef(devref) ;
}




DtaDevOSDrive * DtaDevOSDrive::getDtaDevOSDrive(const char * devref,
                                                DTA_DEVICE_INFO &disk_info)
{
  return static_cast<DtaDevOSDrive *>(DtaDevMacOSDrive::getDtaDevMacOSDrive(devref, disk_info));
}


DtaDevMacOSDrive * DtaDevMacOSDrive::getDtaDevMacOSDrive(const char * devref,
                                                         DTA_DEVICE_INFO &disk_info)
{
  DtaDevMacOSDrive * drive ;

  disk_info.devType = DEVICE_TYPE_OTHER;

#if defined(NVME)
  if ( (drive = DtaDevMacOSNvme::getDtaDevMacOSNvme(devref, disk_info)) != NULL )
    return drive ;
  //  LOG(D4) << "DtaDevMacOSNvme::getDtaDevMacOSNvme(\"" << devref <<  "\", disk_info) returned NULL";
#endif  // defined(NVME)

  if ( (drive = DtaDevMacOSScsi::getDtaDevMacOSScsi(devref, disk_info)) != NULL )
    return drive ;
  // LOG(D4) << "DtaDevMacOSScsi::getDtaDevMacOSScsi(\"" << devref <<  "\", disk_info) returned NULL";

  return NULL ;
}
