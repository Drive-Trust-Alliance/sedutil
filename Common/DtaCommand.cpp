/* C:B**************************************************************************
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
#include "os.h"
#include <stdio.h>
#include "DtaCommand.h"
#include "DtaEndianFixup.h"
#include "DtaHexDump.h"
#include "DtaStructures.h"

using namespace std;

DtaCommand::DtaCommand()
{
    LOG(D1) << "Creating DtaCommand()";
	cmdbuf = commandbuffer + IO_BUFFER_ALIGNMENT;
	cmdbuf = (uint8_t*)((uintptr_t)cmdbuf & (uintptr_t)~(IO_BUFFER_ALIGNMENT - 1));
	respbuf = responsebuffer + IO_BUFFER_ALIGNMENT;
	respbuf = (uint8_t*)((uintptr_t)respbuf & (uintptr_t)~(IO_BUFFER_ALIGNMENT - 1));
}

/* Fill in the header information and format the call */
DtaCommand::DtaCommand(OPAL_UID InvokingUid, OPAL_METHOD method)
{
    LOG(D1) << "Creating DtaCommand(ID, InvokingUid, method)";
	cmdbuf = commandbuffer + IO_BUFFER_ALIGNMENT;
	cmdbuf = (uint8_t*)((uintptr_t)cmdbuf & (uintptr_t)~(IO_BUFFER_ALIGNMENT - 1));
	respbuf = responsebuffer + IO_BUFFER_ALIGNMENT;
	respbuf = (uint8_t*)((uintptr_t)respbuf & (uintptr_t)~(IO_BUFFER_ALIGNMENT - 1));
	reset(InvokingUid, method);
}

void
DtaCommand::reset()
{
    LOG(D1) << "Entering DtaCommand::reset()";
    memset(cmdbuf, 0, IO_BUFFER_LENGTH);
	memset(respbuf, 0, IO_BUFFER_LENGTH);
    bufferpos = sizeof (OPALHeader);
}
void 
DtaCommand::reset(OPAL_UID InvokingUid, vector<uint8_t> method){
	LOG(D1) << "Entering DtaCommand::reset(OPAL_UID,uint8_t)";
	reset();
	cmdbuf[bufferpos++] = OPAL_TOKEN::CALL;
	addToken(InvokingUid);
	addToken(method);
}
void 
DtaCommand::reset(vector<uint8_t> InvokingUid, vector<uint8_t> method){
	LOG(D1) << "Entering DtaCommand::reset(uint8_t,uint8_t)";
	reset();
	cmdbuf[bufferpos++] = OPAL_TOKEN::CALL;
	addToken(InvokingUid);
	addToken(method);
}

void
DtaCommand::reset(OPAL_UID InvokingUid, OPAL_METHOD method)
{
    LOG(D1) << "Entering DtaCommand::reset(OPAL_UID, OPAL_METHOD)";
    reset(); 
    cmdbuf[bufferpos++] = OPAL_TOKEN::CALL;
	addToken(InvokingUid);
    cmdbuf[bufferpos++] = OPAL_SHORT_ATOM::BYTESTRING8;
    memcpy(&cmdbuf[bufferpos], &OPALMETHOD[method][0], 8); /* bytes 11-18 */
    bufferpos += 8;
}

void
DtaCommand::addToken(uint64_t number)
{
    int startat = 0;
    LOG(D1) << "Entering DtaCommand::addToken(uint64_t)";
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
DtaCommand::addToken(vector<uint8_t> token)
{
    LOG(D1) << "Entering addToken(vector<uint8_t>)";
    for (uint32_t i = 0; i < token.size(); i++) {
        cmdbuf[bufferpos++] = token[i];
    }
}

void
DtaCommand::addToken(const char * bytestring)
{
    LOG(D1) << "Entering DtaCommand::addToken(const char * )";
    uint16_t length = (uint16_t) strlen(bytestring);
    if (length == 0) {
        /* null token e.g. default password */
        cmdbuf[bufferpos++] = (uint8_t)0xa1;
        cmdbuf[bufferpos++] = (uint8_t)0x00;
    }
    else if (length < 16) {
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
		exit(EXIT_FAILURE);
    }
    memcpy(&cmdbuf[bufferpos], bytestring, length);
    bufferpos += length;

}

void
DtaCommand::addToken(OPAL_TOKEN token)
{
    LOG(D1) << "Entering DtaCommand::addToken(OPAL_TOKEN)";
    cmdbuf[bufferpos++] = (uint8_t) token;
}

void
DtaCommand::addToken(OPAL_SHORT_ATOM token)
{
    LOG(D1) << "Entering DtaCommand::addToken(OPAL_SHORT_ATOM)";
    cmdbuf[bufferpos++] = (uint8_t)token;
}

void
DtaCommand::addToken(OPAL_TINY_ATOM token)
{
    LOG(D1) << "Entering DtaCommand::addToken(OPAL_TINY_ATOM)";
    cmdbuf[bufferpos++] = (uint8_t) token;
}

void
DtaCommand::addToken(OPAL_UID token)
{
    LOG(D1) << "Entering DtaCommand::addToken(OPAL_UID)";
    cmdbuf[bufferpos++] = OPAL_SHORT_ATOM::BYTESTRING8;
    memcpy(&cmdbuf[bufferpos], &OPALUID[token][0], 8);
    bufferpos += 8;
}

void
DtaCommand::complete(uint8_t EOD)
{
    LOG(D1) << "Entering DtaCommand::complete(uint8_t EOD)";
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
	LOG(D1) << "bufferpos=" << bufferpos << endl;
	LOG(D1) << "sizeof(OPALHeader)=" << sizeof(OPALHeader) << endl;
	LOG(D1) << "sizeof(OPALComPacket)=" << sizeof(OPALComPacket) << endl;
    hdr->subpkt.length = SWAP32(bufferpos - (sizeof (OPALHeader)));
    while (bufferpos % 4 != 0) {
        cmdbuf[bufferpos++] = 0x00;
    }
    hdr->pkt.length = SWAP32((bufferpos - sizeof (OPALComPacket))
                             - sizeof (OPALPacket));
    hdr->cp.length = SWAP32(bufferpos - sizeof (OPALComPacket));
	LOG(D1) << "bufferpos=" << bufferpos << endl;
	LOG(D1) << "hdr->subpkt.length=" << SWAP32(hdr->subpkt.length) << endl;
	LOG(D1) << "hdr->pkt.length=" << SWAP32(hdr->pkt.length) << endl;
	LOG(D1) << "hdr->cp.length=" << SWAP32(hdr->cp.length) << endl;
	if (bufferpos > 57444) { 
		LOG(E) << " Buffer Overrun " << bufferpos;
		exit(EXIT_FAILURE);
	}
}

void
DtaCommand::changeInvokingUid(std::vector<uint8_t> Invoker)
{
    LOG(D1) << "Entering DtaCommand::changeInvokingUid()";
    int offset = sizeof (OPALHeader) + 1; /* bytes 2-9 */
    for (uint32_t i = 0; i < Invoker.size(); i++) {
        cmdbuf[offset + i] = Invoker[i];
    }

}

void *
DtaCommand::getCmdBuffer()
{
    return cmdbuf;
}

void *
DtaCommand::getRespBuffer()
{
    return respbuf;
}
void
DtaCommand::dumpCommand()
{
	OPALHeader * hdr = (OPALHeader *)cmdbuf;
	DtaHexDump(cmdbuf, SWAP32(hdr->cp.length) + sizeof(OPALComPacket));
}
void
DtaCommand::dumpResponse()
{
	OPALHeader *hdr = (OPALHeader *)respbuf;
	DtaHexDump(respbuf, SWAP32(hdr->cp.length) + sizeof(OPALComPacket));
}

void
DtaCommand::setcomID(uint16_t comID)
{
    OPALHeader * hdr;
    hdr = (OPALHeader *) cmdbuf;
    LOG(D1) << "Entering DtaCommand::setcomID()";
    hdr->cp.extendedComID[0] = ((comID & 0xff00) >> 8);
    hdr->cp.extendedComID[1] = (comID & 0x00ff);
    hdr->cp.extendedComID[2] = 0x00;
    hdr->cp.extendedComID[3] = 0x00;
}

void
DtaCommand::setTSN(uint32_t TSN)
{
	LOG(D1) << "Entering DtaCommand::setTSN()";
    OPALHeader * hdr;
    hdr = (OPALHeader *) cmdbuf;
    hdr->pkt.TSN = TSN;
}

void
DtaCommand::setHSN(uint32_t HSN)
{
	LOG(D1) << "Entering DtaCommand::setHSN()";
    OPALHeader * hdr;
    hdr = (OPALHeader *) cmdbuf;
    hdr->pkt.HSN = HSN;
}

DtaCommand::~DtaCommand()
{
    LOG(D1) << "Destroying DtaCommand";
}
