/* C:B**************************************************************************
This software is Copyright 2016-2018 Alexander Motin <mav@FreeBSD.org>
This software is Copyright 2014-2016 Bright Plaza Inc. <drivetrust@drivetrust.com>

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
#include "DtaStructures.h"
#include "DtaDevFreeBSDDrive.h"

/** FreeBSD specific implementation of DtaDevOS.
 * Uses the SCSI generic ioctls to send commands to the 
 * device 
 */
class DtaDevFreeBSDSata: public DtaDevFreeBSDDrive {
public:
    /** Default constructor */
    DtaDevFreeBSDSata();
    /** Destructor */
    ~DtaDevFreeBSDSata();
    /** Sata FreeBSD specific initialization.
     * This function should perform the necessary authority and environment checking
     * to allow proper functioning of the program, open the device, perform an ATA
     * identify, add the fields from the identify response to the disk info structure
     * and if the device is an ATA device perform a call to Discovery0() to complete
     * the disk_info structure
     * @param devref character representation of the device is standard OS lexicon
     */
    bool init(const char * devref);
    /** Sata FreeBSD specific method to send an ATA command to the device
     * @param cmd ATA command to be sent to the device
     * @param protocol security protocol to be used in the command
     * @param comID communications ID to be used
     * @param buffer input/output buffer
     * @param bufferlen length of the input/output buffer
     */
    uint8_t sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
            void * buffer, uint32_t bufferlen);
    void identify(OPAL_DiskInfo& disk_info);
    struct cam_device *camdev;
    int isSAS; /* The device is sas */
};
