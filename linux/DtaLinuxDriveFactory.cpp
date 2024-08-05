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




#include "DtaLinuxBlockStorageDevice.h"
#include "DtaAtaDrive.h"
#include "DtaNvmeDrive.h"
#include "DtaScsiDrive.h"
#include "DtaSataDrive.h"


/** Factory functions
 *
 * Static class members that support instantiation of subclass members
 * with the subclass switching logic localized here for easier maintenance.
 *
 */


DtaDrive * DtaDrive::getDtaDrive(Protocol p,
                                 const char * devref,
                                 DTA_DEVICE_INFO & device_info){
  DtaDrive * drive = NULL;
  switch(p) {
  case BlockStorageDevice:
    drive = DtaLinuxBlockStorageDevice::getDtaLinuxBlockStorageDevice(devref, device_info);
    break;

  case AtaDrive:
    //    drive = DtaAtaDrive::getDtaAtaDrive(devref, device_info);
    drive = DtaAtaDrive::getDtaAtaDrive(devref, device_info);
    break;

  case NvmeDrive:
    //    drive = DtaLinuxNvmeDrive::getDtaLinuxNvmeDrive(devref, device_info);
    drive = DtaNvmeDrive::getDtaNvmeDrive(devref, device_info);
    break;

  case ScsiDrive:
    //    drive = DtaLinuxScsiDrive::getDtaLinuxScsiDrive(devref, device_info);
    drive = DtaScsiDrive::getDtaScsiDrive(devref, device_info);
    break;

  case SataDrive:
    //    drive = DtaLinuxSataDrive::getDtaLinuxSataDrive(devref, device_info);
    drive = DtaSataDrive::getDtaSataDrive(devref, device_info);
    break;

  default:
    break;
  }
  // DTA_DEVICE_TYPE deviceType = device_info.devType;
  // const char * deviceTypeName = DtaDevTypeName(deviceType);
  // LOG(E) << "DtaDrive::getDtaDrive(" << p << ", \"" << devref << "\", _):"
  //        << " "
  //        <<"deviceType=" << deviceType
  //        << " "
  //        << "deviceTypeName=" << deviceTypeName
  //   ;


  return drive;
}
