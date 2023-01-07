/* C:B**************************************************************************
This software is Copyright 2014, 2022 Bright Plaza Inc. <drivetrust@drivetrust.com>

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

#if defined(__APPLE__) && defined(__MACH__)

#include "DtaDevMacOSTPer.h"

/** Mac specific implementation of DtaDevOS.
 * Uses  SAT to send commands to the device
 */
#define is_aligned(POINTER, BYTE_COUNT) \
    (((uintptr_t)(const void *)(POINTER)) % (BYTE_COUNT) == 0)
class DtaDevMacOSTPer_SCSI: public DtaDevMacOSTPer {
public:
//    /** USB specific initialization.
//     * This function should perform the necessary authority and environment checking
//     * to allow proper functioning of the program, open the device, perform an ATA
//     * identify, add the fields from the identify response to the disk info structure
//     * and if the device is an ATA device perform a call to Discovery0() to complete
//     * the disk_info structure
//     * @param _driverService IO Registry entry for the driver
//     * @param _connect IORegistry entry for the connection
//     */
//    DtaDevMacOSTPer_SAT(io_registry_entry_t _driverService, io_connect_t _connect){
//        init(_driverService, _connect);
//    };
    using DtaDevMacOSTPer::DtaDevMacOSTPer;

    /** USB specific method to send a command to the device
     * @param cmd command to be sent to the device
     * @param protocol security protocol to be used in the command
     * @param comID communications ID to be used
     * @param buffer input/output buffer
     * @param bufferlen length of the input/output buffer
     */
    uint8_t sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                    void * buffer, size_t bufferlen);
    
    /** Connect to "user client" (driver shim in userland)
     */
};

#endif // defined(__APPLE__) && defined(__MACH__)
