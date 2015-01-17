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
#include "os.h"
#include <stdio.h>
#include "MsedCommand.h"
#include "MsedEndianFixup.h"
#include "MsedHexDump.h"
#include "MsedStructures.h"

/*
 * Initialize: allocate the buffers *ONLY*
 * reset needs to be called to
 * initialize the headers etc
 */
MsedCommand::MsedCommand()
{
    LOG(D1) << "Creating MsedCommand()";
    cmdbuf = (uint8_t *) ALIGNED_ALLOC(4096, IO_BUFFER_LENGTH);
    respbuf = (uint8_t *) ALIGNED_ALLOC(4096, IO_BUFFER_LENGTH);
}

/* Fill in the header information and format the call */
MsedCommand::MsedCommand(OPAL_UID InvokingUid, OPAL_METHOD method)
{
    LOG(D1) << "Creating MsedCommand(ID, InvokingUid, method)";
    /* allocate the cmdbuf */
    cmdbuf = (uint8_t *) ALIGNED_ALLOC(4096, IO_BUFFER_LENGTH);
	respbuf = (uint8_t *)ALIGNED_ALLOC(4096, IO_BUFFER_LENGTH);
    reset(InvokingUid, method);
}

/* Fill in the header information ONLY (no call) */
void
MsedCommand::reset()
{
    LOG(D1) << "Entering MsedCommand::reset()";
    memset(cmdbuf, 0, IO_BUFFER_LENGTH);
    OPALHeader * hdr;
    hdr = (OPALHeader *) cmdbuf;

    bufferpos = sizeof (OPALHeader);
}

void
MsedCommand::reset(OPAL_UID InvokingUid, OPAL_METHOD method)
{
    LOG(D1) << "Entering MsedCommand::reset(InvokingUid, method)";
    reset(); // build the headers
    cmdbuf[bufferpos++] = OPAL_TOKEN::CALL;
    cmdbuf[bufferpos++] = OPAL_SHORT_ATOM::BYTESTRING8;
    memcpy(&cmdbuf[bufferpos], &OPALUID[InvokingUid][0], 8); /* bytes 2-9 */
    bufferpos += 8;
    cmdbuf[bufferpos++] = OPAL_SHORT_ATOM::BYTESTRING8;
    memcpy(&cmdbuf[bufferpos], &OPALMETHOD[method][0], 8); /* bytes 11-18 */
    bufferpos += 8;
}

void
MsedCommand::addToken(uint64_t number)
{
    int startat = 0;
    LOG(D1) << "Entering MsedCommand::addToken(uint64_t number)";
    if (number < 64) {
        cmdbuf[bufferpos++] = (uint8_t) number & 0x000000000000003f;
    }
    else {
        if (number < 0x100) {
            cmdbuf[bufferpos++] = 0x81;
            startat = 0;
        }
        else if (number < 0x10000) {
            cmdbuf[bufferpos++] = 0x82;
            startat = 1;
        }
        else if (number < 0x100000000) {
            cmdbuf[bufferpos++] = 0x84;
            startat = 3;
        }
        else {
            cmdbuf[bufferpos++] = 0x88;
            startat = 7;
        }
        for (int i = startat; i > -1; i--) {
            cmdbuf[bufferpos++] = (uint8_t) ((number >> (i * 8)) & 0x00000000000000ff);
        }
    }
}

void
MsedCommand::addToken(std::vector<uint8_t> token)
{
    LOG(D1) << "Entering addToken(std::vector<uint8_t>)";
    for (uint32_t i = 0; i < token.size(); i++) {
        cmdbuf[bufferpos++] = token[i];
    }
}

void
MsedCommand::addToken(const char * bytestring)
{
    LOG(D1) << "Entering MsedCommand::addToken(const char * bytestring)";
    uint16_t length = (uint16_t) strlen(bytestring);
    if (length < 16) {
        /* use tiny atom */
        cmdbuf[bufferpos++] = (uint8_t) length | 0xa0;
    }
    else if (length < 2048) {
        /* Use Medium Atom */
        cmdbuf[bufferpos++] = 0xd0 | (uint8_t) ((length >> 8) & 0x07);
        cmdbuf[bufferpos++] = (uint8_t) (length & 0x00ff);
    }
    else {
        /* Use Large Atom */
        LOG(E) << "FAIL -- can't send LARGE ATOM size bytestring in 2048 Packet";
    }
    memcpy(&cmdbuf[bufferpos], bytestring, length);
    bufferpos += length;

}

void
MsedCommand::addToken(OPAL_TOKEN token)
{
    LOG(D1) << "Entering MsedCommand::addToken(OPAL_TOKEN token)";
    cmdbuf[bufferpos++] = (uint8_t) token;
}

void
MsedCommand::addToken(OPAL_TINY_ATOM token)
{
    LOG(D1) << "Entering MsedCommand::addToken(OPAL_TINY_ATOM token)";
    cmdbuf[bufferpos++] = (uint8_t) token;
}

void
MsedCommand::addToken(OPAL_UID token)
{
    LOG(D1) << "Entering MsedCommand::addToken(OPAL_UID token)";
    cmdbuf[bufferpos++] = OPAL_SHORT_ATOM::BYTESTRING8;
    memcpy(&cmdbuf[bufferpos], &OPALUID[token][0], 8);
    bufferpos += 8;
}

void
MsedCommand::complete(uint8_t EOD)
{
    LOG(D1) << "Entering MsedCommand::complete(uint8_t EOD)";
    if (EOD) {
        cmdbuf[bufferpos++] = OPAL_TOKEN::ENDOFDATA;
        cmdbuf[bufferpos++] = OPAL_TOKEN::STARTLIST;
        cmdbuf[bufferpos++] = 0x00;
        cmdbuf[bufferpos++] = 0x00;
        cmdbuf[bufferpos++] = 0x00;
        cmdbuf[bufferpos++] = OPAL_TOKEN::ENDLIST;
    }
    /* fill in the lengths and add the modulo 4 padding */
    OPALHeader * hdr;
    hdr = (OPALHeader *) cmdbuf;
    hdr->subpkt.length = SWAP32(bufferpos - sizeof (OPALHeader));
    while (bufferpos % 4 != 0) {
        cmdbuf[bufferpos++] = 0x00;
    }
    hdr->pkt.length = SWAP32((bufferpos - sizeof (OPALComPacket))
                             - sizeof (OPALPacket));
    hdr->cp.length = SWAP32(bufferpos - sizeof (OPALComPacket));
}

void
MsedCommand::changeInvokingUid(std::vector<uint8_t> Invoker)
{
    LOG(D1) << "Entering MsedCommand::changeInvokingUid()";
    int offset = sizeof (OPALHeader) + 1; /* bytes 2-9 */
    for (uint32_t i = 0; i < Invoker.size(); i++) {
        cmdbuf[offset + i] = Invoker[i];
    }

}

void *
MsedCommand::getCmdBuffer()
{
    return cmdbuf;
}

void *
MsedCommand::getRespBuffer()
{
    return respbuf;
}

void
MsedCommand::setcomID(uint16_t comID)
{
    OPALHeader * hdr;
    hdr = (OPALHeader *) cmdbuf;
    LOG(D1) << "Entering MsedCommand::setcomID()";
    hdr->cp.extendedComID[0] = ((comID & 0xff00) >> 8);
    hdr->cp.extendedComID[1] = (comID & 0x00ff);
    hdr->cp.extendedComID[2] = 0x00;
    hdr->cp.extendedComID[3] = 0x00;
}

void
MsedCommand::setTSN(uint32_t TSN)
{
	LOG(D1) << "Entering MsedCommand::setTSN()";
    OPALHeader * hdr;
    hdr = (OPALHeader *) cmdbuf;
    hdr->pkt.TSN = TSN;
}

void
MsedCommand::setHSN(uint32_t HSN)
{
	LOG(D1) << "Entering MsedCommand::setHSN()";
    OPALHeader * hdr;
    hdr = (OPALHeader *) cmdbuf;
    hdr->pkt.HSN = HSN;
}

MsedCommand::~MsedCommand()
{
    LOG(D1) << "Destroying MsedCommand";
    ALIGNED_FREE(cmdbuf);
    ALIGNED_FREE(respbuf);
}
