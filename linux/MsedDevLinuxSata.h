/* C:B**************************************************************************
This software is Copyright 2014,2015 Michael Romeo <r0m30@r0m30.com>

This file is part of msed.

msed is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

msed is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with msed.  If not, see <http://www.gnu.org/licenses/>.

 * C:E********************************************************************** */
#pragma once
#include "MsedStructures.h"
#include "MsedDevLinuxDrive.h"

/** Linux specific implementation of MsedDevOS.
 * Uses the SCSI generic ioctls to send commands to the 
 * device 
 */
class MsedDevLinuxSata: public MsedDevLinuxDrive {
public:
    /** Default constructor */
    MsedDevLinuxSata();
    /** Destructor */
    ~MsedDevLinuxSata();
    /** Sata Linux specific initialization.
     * This function should perform the necessary authority and environment checking
     * to allow proper functioning of the program, open the device, perform an ATA
     * identify, add the fields from the identify response to the disk info structure
     * and if the device is an ATA device perform a call to Discovery0() to complete
     * the disk_info structure
     * @param devref character representation of the device is standard OS lexicon
     */
    bool init(const char * devref);
    /** Sata Linux specific method to send an ATA command to the device
     * @param cmd ATA command to be sent to the device
     * @param protocol security protocol to be used in the command
     * @param comID communications ID to be used
     * @param buffer input/output buffer
     * @param bufferlen length of the input/output buffer
     */
    uint8_t sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
            void * buffer, uint16_t bufferlen);
    /** Linux specific routine to send an ATA identify to the device */
    void identify(OPAL_DiskInfo *disk_info);
	int fd; /**< Linux handle for the device  */
};
