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
/*
 * Manage the session to a TPer.
 */
#include "MsedLexicon.h"
#include <vector>
class MsedCommand;
class MsedBaseDev;
class MsedResponse;

using namespace std;

class MsedSession {
public:
    MsedSession(MsedBaseDev * device);
    ~MsedSession();
	uint8_t start(OPAL_UID SP);    // unauthenticated "Anybody" session
    uint8_t start(OPAL_UID SP, char * HostChallenge, OPAL_UID SignAuthority);
    void setProtocol(uint8_t value);
	void dontHashPwd();
    void expectAbort();
    uint8_t sendCommand(MsedCommand * cmd, MsedResponse & response);
private:
    MsedSession();
    char * methodStatus(uint8_t status);
    MsedBaseDev * d;
    uint32_t bufferpos = 0;
    uint32_t TSN = 0;
    uint32_t HSN = 0;
    uint8_t willAbort = 0;
	uint8_t hashPwd = 1;
    uint8_t SecurityProtocol = 0x01;
};

