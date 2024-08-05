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
//
#include <SEDKernelInterface/SEDKernelInterface.h>
//

#include "DtaMacOSScsiDrive.h"

DtaScsiDrive * DtaScsiDrive::getDtaOSSpecificScsiDrive(OSDEVICEHANDLE _osDeviceHandle)
{
    return new DtaMacOSScsiDrive(_osDeviceHandle);
};


// uint8_t DtaMacOSScsiDrive::discovery0(DTA_DEVICE_INFO & disk_info) {
//     io_connect_t connect = handleConnection(osDeviceHandle);
//     io_registry_entry_t deviceService = handleDeviceService(osDeviceHandle);
//     if (!IORegistryEntryInPlane(deviceService, "IOService")) {
//         LOG(E) << "deviceService " << deviceService << " " << HEXON(4) << deviceService << " invalid!";
//         return DTAERROR_COMMAND_ERROR;
//     }
//     io_service_t controllerService = findParent(deviceService);
//     return ((connect != IO_OBJECT_NULL && deviceService != IO_OBJECT_NULL &&
//              KERN_SUCCESS == TPerUpdate(connect, controllerService, &disk_info)))
//             ? DTAERROR_SUCCESS
//             : DTAERROR_COMMAND_ERROR;
// }
