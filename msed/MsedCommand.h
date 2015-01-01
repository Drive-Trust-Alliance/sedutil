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
/** A class to build & send Opal Command streams to a TPer.
 * This class attempts to closely mimic the command
 * pseudo code used in the TCG documents, the syntactic
 * sugar is not represented.  See TCG document Storage Architecture
 * Core Specification R2.00 V2.00 Section 3.2.1.2 for all
 * the gory details.
 *
 * See also OpalLexicon for structs, typedefs and enums used to encode
 * the bytestream.
 */
#include <vector>
#include "MsedLexicon.h"
class MsedBaseDev;

class MsedCommand {
	friend class MsedBaseDev;
public:
    MsedCommand();
    MsedCommand(OPAL_UID InvokingUid, OPAL_METHOD method);
    ~MsedCommand();
    void addToken(OPAL_TOKEN token);
    void addToken(OPAL_TINY_ATOM token);
    void addToken(OPAL_UID token);
    void addToken(const char * bytestring);
    void addToken(std::vector<uint8_t> token);
    void addToken(uint64_t number);
    void setcomID(uint16_t comID);
    void setHSN(uint32_t HSN);
    void setTSN(uint32_t TSN);
    void complete(uint8_t EOD = 1);
    void reset();
    void reset(OPAL_UID InvokingUid, OPAL_METHOD method);
    void changeInvokingUid(std::vector<uint8_t> Invoker);
private:
	void * getCmdBuffer();
	void * getRespBuffer();
    uint8_t *cmdbuf;
    uint8_t *respbuf;
    uint32_t bufferpos = 0;
};
