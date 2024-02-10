/* C:B**************************************************************************
This software is Copyright 2014-2017 Bright Plaza Inc. <drivetrust@drivetrust.com>

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
#include <vector>
#include "DtaStructures.h"

using namespace std;
/** virtual implementation for a disk interface-generic disk drive
 */
class DtaDevLinuxDrive {
public:
    virtual ~DtaDevLinuxDrive( void ) {};
    /**Initialization.
     * This function should perform the necessary authority and environment checking
     * to allow proper functioning of the program, open the device, perform an ATA
     * identify, add the fields from the identify response to the disk info structure
     * and if the device is an ATA device perform a call to Discovery0() to complete
     * the disk_info structure
     * @param devref character representation of the device is standard OS lexicon
     */
    virtual bool init(const char * devref) = 0;
    /** Method to send a command to the device
     * @param cmd command to be sent to the device
     * @param protocol security protocol to be used in the command
     * @param comID communications ID to be used
     * @param buffer input/output buffer
     * @param bufferlen length of the input/output buffer
     */
    virtual uint8_t sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
            void * buffer, uint32_t bufferlen) = 0;
    /** Routine to send an identify to the device */
    virtual void identify(OPAL_DiskInfo& disk_info) = 0;
    /** Save the password hash to the kernel for S3 sleep wakeup */
    uint8_t prepareForS3Sleep(uint8_t lockingrange, const vector<uint8_t> &password_hash);
    int fd; /**< Linux handle for the device  */
};
