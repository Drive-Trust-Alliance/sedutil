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
/** A class to build & send TCG Command streams to a TPer.
 * This class attempts to closely mimic the command
 * pseudo code used in the TCG documents, the syntactic
 * sugar is not represented.  See TCG document Storage Architecture
 * Core Specification R2.00 V2.00 Section 3.2.1.2 for all
 * the gory details.
 *
 * See also TCGLexicon for structs, typedefs and enums used to encode
 * the bytestream.
 */
#include "TCGlexicon.h"
class TCGdev;

class TCGcommand {
public:
    TCGcommand();
    TCGcommand(TCG_UID InvokingUid, TCG_METHOD method);
    ~TCGcommand();
	void * getCmdBuffer();
	void * getRespBuffer();
    void addToken(TCG_TOKEN token);
    void addToken(TCG_TINY_ATOM token);
    void addToken(TCG_UID token);
    void addToken(const char * bytestring);
	void addToken(uint8_t bytes[], uint16_t size);
    void addToken(uint16_t);
	void setcomID(uint16_t comID);
	void setHSN(uint32_t HSN);
	void setTSN(uint32_t TSN);
    void complete(uint8_t EOD = 1);
    void reset();
    void reset(TCG_UID InvokingUid, TCG_METHOD method);
	void changeInvokingUid(uint8_t Invoker[]);
private:
    uint8_t *cmdbuf;
	uint8_t *respbuf;
    uint32_t bufferpos = 0;
};

