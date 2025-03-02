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

#include "DtaScsiDrive.h"


///** Windows specific implementation SCSI generic ioctls to send commands to the
// * device
// */
//class DtaWindowsScsiDrive: public DtaScsiDrive {
//public:
//    using DtaScsiDrive::DtaScsiDrive;
//
//    /** Factory function to look at the devref and create an instance of DtaWindowsScsiDrive
//     * @param devref OS device reference e.g. "/dev/sda"
//     * @param disk_info reference to DTA_DEVICE_INFO structure filled out during device identification
//     */
//    static DtaDrive * getDtaWindowsScsiDrive(const char * devref,
//                                                    DTA_DEVICE_INFO & disk_info)
//    {
//        return getDtaScsiDrive(devref,disk_info);
//    }
//};
