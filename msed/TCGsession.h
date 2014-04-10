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
/*
 * Manage the session to a TPer.
 */
#include "TCGlexicon.h"
class TCGcommand;
class TCGdev;

class TCGdev;

class TCGsession {
public:
    TCGsession(TCGdev * device);
    ~TCGsession();
    uint8_t start(TCG_UID SP,
            char * HostChallenge = NULL,
            TCG_UID SignAuthority = TCG_UID::TCG_UID_HEXFF);
    void setProtocol(uint8_t value);
    void expectAbort();
    uint8_t sendCommand(TCGcommand * cmd);
private:
    TCGsession();
    uint8_t SEND(TCGcommand * cmd);
    uint8_t RECV(void * resp);
    char * methodStatus(uint8_t status);
    TCGdev * d;
    uint32_t bufferpos = 0;
    /* The session numbers should be taken from the
     * syncsession response so there will be no
     * issues with endianess
     */
    uint32_t TSN = 0;
    uint32_t HSN = 0;
    uint8_t willAbort = 0;
    uint8_t TCGProtocol = 0x01;
};

