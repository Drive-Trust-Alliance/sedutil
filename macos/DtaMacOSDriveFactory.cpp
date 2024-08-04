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


#include <SEDKernelInterface/SEDKernelInterface.h>

#include "DtaMacOSBlockStorageDevice.h"
// #include "DtaDevMacOSAtaDrive.h"
// #include "DtaDevMacOSNvmeDrive.h"
#include "DtaMacOSScsiDrive.h"
#include "DtaMacOSSataDrive.h"


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
            drive = DtaMacOSBlockStorageDevice::getDtaMacOSBlockStorageDevice(devref, device_info);
            break;
            
        case AtaDrive:
//            drive = DtaMacOSBlockStorageDevice::getDtaMacOSBlockStorageDevice(devref, device_info);
            break;
            
        case NvmeDrive:
//            drive = DtaMacOSBlockStorageDevice::getDtaMacOSBlockStorageDevice(devref, device_info);
            break;
            
        case ScsiDrive:
            drive = DtaMacOSScsiDrive::getDtaMacOSScsiDrive(devref, device_info);
            break;
            
        case SataDrive:
            drive = DtaMacOSSataDrive::getDtaMacOSSataDrive(devref, device_info);
            break;
            
        default:
            break;
    }
    
    return drive;
}
