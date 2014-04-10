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
#include "os.h"
#include <stdio.h>
#include "TCGcommand.h"
#include "TCGdev.h"
#include "endianfixup.h"
#include "hexDump.h"
#include "TCGstructures.h"
#include "noparser.h"

/*
 * Initialize: allocate the buffers *ONLY*
 * reset needs to be called to
 * initialize the headers etc
 */
TCGcommand::TCGcommand()
{
	LOG(D4) << "Creating TCGcommand()";
    cmdbuf = (uint8_t *) ALIGNED_ALLOC(4096, IO_BUFFER_LENGTH);
	respbuf = (uint8_t *) ALIGNED_ALLOC(4096, IO_BUFFER_LENGTH);
}

/* Fill in the header information and format the call */
TCGcommand::TCGcommand(TCG_UID InvokingUid, TCG_METHOD method)
{
	LOG(D4) << "Creating TCGvommand(uint16_t ID, TCG_UID InvokingUid, TCG_METHOD method)";
    /* allocate the cmdbuf */
    cmdbuf = (uint8_t *) ALIGNED_ALLOC(4096, IO_BUFFER_LENGTH);
    reset(InvokingUid, method);
}

/* Fill in the header information ONLY (no call) */
void
TCGcommand::reset()
{
	LOG(D4) << "Entering TCGcommand::reset(uint16_t comID)";
    memset(cmdbuf, 0, IO_BUFFER_LENGTH);
    TCGHeader * hdr;
    hdr = (TCGHeader *) cmdbuf;

    bufferpos = sizeof (TCGHeader);
}

void
TCGcommand::reset(TCG_UID InvokingUid, TCG_METHOD method)
{
	LOG(D4) << "Entering TCGcommand::reset(uint16_t comID, TCG_UID InvokingUid, TCG_METHOD method)";
    reset(); // build the headers
    cmdbuf[bufferpos++] = TCG_TOKEN::CALL;
    cmdbuf[bufferpos++] = TCG_SHORT_ATOM::BYTESTRING8;
    memcpy(&cmdbuf[bufferpos], &TCGUID[InvokingUid][0], 8); /* bytes 2-9 */
    bufferpos += 8;
    cmdbuf[bufferpos++] = TCG_SHORT_ATOM::BYTESTRING8;
    memcpy(&cmdbuf[bufferpos], &TCGMETHOD[method][0], 8); /* bytes 11-18 */
    bufferpos += 8;
}

void
TCGcommand::addToken(uint16_t number)
{
	LOG(D4) << "Entering TCGcommand::addToken(uint16_t number)";
    cmdbuf[bufferpos++] = 0x82;
    cmdbuf[bufferpos++] = ((number & 0xff00) >> 8);
    cmdbuf[bufferpos++] = (number & 0x00ff);
}

void
TCGcommand::addToken(const char * bytestring)
{
	LOG(D4) << "Entering TCGcommand::addToken(const char * bytestring)";
	uint16_t length = (uint16_t) strlen(bytestring);
    if (strlen(bytestring) < 16) {
        /* use tiny atom */
        cmdbuf[bufferpos++] = (uint8_t) length | 0xa0;
    }
    else if(length < 2048) {
        /* Use Medium Atom */
        cmdbuf[bufferpos++] = 0xd0 | (uint8_t) ((length >> 8) & 0x07);
        cmdbuf[bufferpos++] = (uint8_t) (length & 0x00ff);
    }
    else {
        /* Use Large Atom */
        LOG(E) << "FAIL -- can't send LARGE ATOM size bytestring in 2048 Packet";
    }
    memcpy(&cmdbuf[bufferpos], bytestring, (strlen(bytestring)));
    bufferpos += (strlen(bytestring));

}

void
TCGcommand::addToken(TCG_TOKEN token)
{
	LOG(D4) << "Entering TCGcommand::addToken(TCG_TOKEN token)";
	cmdbuf[bufferpos++] = (uint8_t) token;
}

void
TCGcommand::addToken(TCG_TINY_ATOM token)
{
	LOG(D4) << "Entering TCGcommand::addToken(TCG_TINY_ATOM token)";
	cmdbuf[bufferpos++] = (uint8_t) token;
}

void
TCGcommand::addToken(TCG_UID token)
{
	LOG(D4) << "Entering TCGcommand::addToken(TCG_UID token)";
	cmdbuf[bufferpos++] = TCG_SHORT_ATOM::BYTESTRING8;
    memcpy(&cmdbuf[bufferpos], &TCGUID[token][0], 8);
    bufferpos += 8;
}

void
TCGcommand::addToken(uint8_t bytes[], uint16_t size)
{
	LOG(D4) << "Entering TCGcommand::addToken(uint8_t bytes[], uint16_t size)";
	memcpy(&cmdbuf[bufferpos], &bytes[0], size);
	bufferpos += size;
}

void
TCGcommand::complete(uint8_t EOD)
{
	LOG(D4) << "Entering TCGcommand::complete(uint8_t EOD)";
    if (EOD) {
        cmdbuf[bufferpos++] = TCG_TOKEN::ENDOFDATA;
        cmdbuf[bufferpos++] = TCG_TOKEN::STARTLIST;
        cmdbuf[bufferpos++] = 0x00;
        cmdbuf[bufferpos++] = 0x00;
        cmdbuf[bufferpos++] = 0x00;
        cmdbuf[bufferpos++] = TCG_TOKEN::ENDLIST;
    }
    /* fill in the lengths and add the modulo 4 padding */
    TCGHeader * hdr;
    hdr = (TCGHeader *) cmdbuf;
    hdr->subpkt.length = SWAP32(bufferpos - sizeof (TCGHeader));
    while (bufferpos % 4 != 0) {
        cmdbuf[bufferpos++] = 0x00;
    }
    hdr->pkt.length = SWAP32((bufferpos - sizeof (TCGComPacket))
                             - sizeof (TCGPacket));
    hdr->cp.length = SWAP32(bufferpos - sizeof (TCGComPacket));
}
void
TCGcommand::changeInvokingUid(uint8_t Invoker[])
{
	LOG(D4) << "Entering TCGcommand::changeInvokingUid(uint8_t Invoker[])";
	memcpy(&cmdbuf[sizeof (TCGHeader) + 2], &Invoker[0], 8); /* bytes 2-9 */
}
void *
TCGcommand::getCmdBuffer()
{
	return cmdbuf;
}
void *
TCGcommand::getRespBuffer()
{
	return respbuf;
}

void
TCGcommand::setcomID(uint16_t comID)
{
	TCGHeader * hdr;
	hdr = (TCGHeader *)cmdbuf;
	LOG(D4) << "Entering TCGcommand::setcomID()";
	hdr->cp.extendedComID[0] = ((comID & 0xff00) >> 8);
	hdr->cp.extendedComID[1] = (comID & 0x00ff);
	hdr->cp.extendedComID[2] = 0x00;
	hdr->cp.extendedComID[3] = 0x00;
}

void 
TCGcommand::setTSN(uint32_t TSN) {
	TCGHeader * hdr;
	hdr = (TCGHeader *)cmdbuf;
	LOG(D4) << "Entering TCGcommand::setTSN()";
	hdr->pkt.TSN = TSN;
}

void 
TCGcommand::setHSN(uint32_t HSN) {
	TCGHeader * hdr;
	hdr = (TCGHeader *)cmdbuf;
	LOG(D4) << "Entering TCGcommand::setHSN()";
	hdr->pkt.HSN = HSN;
}

TCGcommand::~TCGcommand()
{
	LOG(D4) << "Destroying TCGcommand";
    ALIGNED_FREE(cmdbuf);
	ALIGNED_FREE(respbuf);
}
