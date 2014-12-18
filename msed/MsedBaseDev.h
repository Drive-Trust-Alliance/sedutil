/* C:B**************************************************************************
This software is Copyright 2014 Michael Romeo <r0m30@r0m30.com>

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
class MsedCommand;
class MsedResponse;

#include "MsedStructures.h"

class MsedBaseDev {
public:
    MsedBaseDev();
    ~MsedBaseDev();
    virtual uint8_t
    sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
            void * buffer, uint16_t bufferlen) = 0;

    uint8_t exec(MsedCommand * cmd, MsedResponse &response, uint8_t protocol = 0x01);
    uint8_t isOpal2();
    uint8_t isOpal1();
    uint8_t isEprise();
    uint8_t isANYSSC();
     uint8_t MBREnabled();
    uint8_t isPresent();
    uint16_t comID();
    void getFirmwareRev(uint8_t bytes[8]);
    void getModelNum(uint8_t bytes[40]);
    void getSerialNum(uint8_t bytes[20]);
    void puke();
protected:
    virtual void osmsSleep(uint32_t milliseconds) = 0;
    /** Decode the Discovery 0 response. Scans the D0 response and creates structure
     * that can be queried later as required.This code also takes care of
     * the endianess conversions either via a bitswap in the structure or executing
     * a macro when the input buffer is read.
     */
    void discovery0();
    virtual void identify() = 0;
    const char * dev;
    uint8_t isOpen = FALSE;
    OPAL_DiskInfo disk_info;
};