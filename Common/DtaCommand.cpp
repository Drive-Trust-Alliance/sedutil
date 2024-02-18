/* C:B**************************************************************************
 This software is Copyright (c) 2014-2024 Bright Plaza Inc. <drivetrust@drivetrust.com>

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
#include <log/log.h>
#include "DtaCommand.h"
#include "DtaEndianFixup.h"
#include "DtaHexDump.h"
#include "DtaStructures.h"

using namespace std;

DtaCommand::DtaCommand()
{
    LOG(D1) << "Creating DtaCommand()";
	cmdbuf = commandbuffer + IO_BUFFER_ALIGNMENT - 1;
	cmdbuf = (uint8_t*)((uintptr_t)cmdbuf & (uintptr_t)~(IO_BUFFER_ALIGNMENT - 1));
    assert(cmdbuf + MAX_BUFFER_LENGTH <= commandbuffer + sizeof(commandbuffer));
	respbuf = responsebuffer + IO_BUFFER_ALIGNMENT - 1;
	respbuf = (uint8_t*)((uintptr_t)respbuf & (uintptr_t)~(IO_BUFFER_ALIGNMENT - 1));
    assert(respbuf + MIN_BUFFER_LENGTH <= responsebuffer + sizeof(responsebuffer));
}

/* Fill in the header information and format the call */
DtaCommand::DtaCommand(OPAL_UID InvokingUid, OPAL_METHOD method)
{
    LOG(D1) << "Creating DtaCommand(ID, InvokingUid, method)";
    LOG(D2) << "InvokingUID is " << InvokingUid << ", method is " << method;
	cmdbuf = commandbuffer + IO_BUFFER_ALIGNMENT - 1;
	cmdbuf = (uint8_t*)((uintptr_t)cmdbuf & (uintptr_t)~(IO_BUFFER_ALIGNMENT - 1));
    assert(cmdbuf + MAX_BUFFER_LENGTH <= commandbuffer + sizeof(commandbuffer));
	respbuf = responsebuffer + IO_BUFFER_ALIGNMENT - 1;
	respbuf = (uint8_t*)((uintptr_t)respbuf & (uintptr_t)~(IO_BUFFER_ALIGNMENT - 1));
    assert(respbuf + MIN_BUFFER_LENGTH <= responsebuffer + sizeof(responsebuffer));
	reset(InvokingUid, method);
}

void
DtaCommand::reset()
{
    LOG(D1) << "Entering DtaCommand::reset()";
    memset(cmdbuf, 0, MAX_BUFFER_LENGTH);
	memset(respbuf, 0, MIN_BUFFER_LENGTH);
    bufferpos = sizeof (DTA_Header);
}
void
DtaCommand::reset(OPAL_UID InvokingUid, vector<uint8_t> method){
    LOG(D1) << "Entering DtaCommand::reset(OPAL_UID,vector<uint8_t>)";
    LOG(D2) << "InvokingUID is " << InvokingUid << ", method is " << method.data();
	reset();
	cmdbuf[bufferpos++] = OPAL_TOKEN::CALL;
	addToken(InvokingUid);
	addToken(method);
}
void
DtaCommand::reset(vector<uint8_t> InvokingUid, vector<uint8_t> method){
    LOG(D1) << "Entering DtaCommand::reset(vector<uint8_t>,vector<uint8_t>)";
    LOG(D2) << "InvokingUID is " << InvokingUid.data() << ", method is " << method.data();
	reset();
	cmdbuf[bufferpos++] = OPAL_TOKEN::CALL;
	addToken(InvokingUid);
	addToken(method);
}

void
DtaCommand::reset(OPAL_UID InvokingUid, OPAL_METHOD method)
{
    LOG(D1) << "Entering DtaCommand::reset(OPAL_UID, OPAL_METHOD)";
    LOG(D2) << "InvokingUID is " << InvokingUid << ", method is " << method;
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
    LOG(D2) << "number is " << number;
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
    LOG(D2) << "token is " << token.data();
    for (uint32_t i = 0; i < token.size(); i++) {
        cmdbuf[bufferpos++] = token[i];
    }
}

void
DtaCommand::addToken(const char * bytestring)
{
    LOG(D1) << "Entering DtaCommand::addToken(const char *)";
    LOG(D2) << "bytestring is \"" << bytestring << "\"";
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
    LOG(D2) << "token is " << token;
    cmdbuf[bufferpos++] = (uint8_t) token;
}

void
DtaCommand::addToken(OPAL_SHORT_ATOM token)
{
    LOG(D1) << "Entering DtaCommand::addToken(OPAL_SHORT_ATOM)";
    LOG(D2) << "token is " << token;
    cmdbuf[bufferpos++] = (uint8_t)token;
}

void
DtaCommand::addToken(OPAL_TINY_ATOM token)
{
    LOG(D1) << "Entering DtaCommand::addToken(OPAL_TINY_ATOM)";
    LOG(D2) << "token is " << token;
    cmdbuf[bufferpos++] = (uint8_t) token;
}

void
DtaCommand::addToken(OPAL_UID token)
{
    LOG(D1) << "Entering DtaCommand::addToken(OPAL_UID)";
    LOG(D2) << "token is " << token;
    cmdbuf[bufferpos++] = OPAL_SHORT_ATOM::BYTESTRING8;
    memcpy(&cmdbuf[bufferpos], &OPALUID[token][0], 8);
    bufferpos += 8;
}

void
DtaCommand::addToken(OPAL_UID token,uint8_t factor)
{
	LOG(D1) << "Entering DtaCommand::addToken(OPAL_UID, factor)";
    LOG(D2) << "token is " << token << ", factor is " << factor;
	cmdbuf[bufferpos++] = OPAL_SHORT_ATOM::BYTESTRING4;
	memcpy(&cmdbuf[bufferpos], &OPALUID[token][0], factor);
	bufferpos += factor;
}

void
DtaCommand::complete(uint8_t EOD)
{
    LOG(D1) << "Entering DtaCommand::complete(uint8_t EOD)";
    LOG(D2) << "EOD is " << (bool)EOD;
    if (EOD) {
        cmdbuf[bufferpos++] = OPAL_TOKEN::ENDOFDATA;
        cmdbuf[bufferpos++] = OPAL_TOKEN::STARTLIST;
        cmdbuf[bufferpos++] = 0x00;
        cmdbuf[bufferpos++] = 0x00;
        cmdbuf[bufferpos++] = 0x00;
        cmdbuf[bufferpos++] = OPAL_TOKEN::ENDLIST;
    }
    /* fill in the lengths and add the modulo 4 padding */
    DTA_Header * hdr;
    hdr = (DTA_Header *) cmdbuf;
    hdr->subpkt.length = SWAP32(bufferpos - (uint32_t)(sizeof (DTA_Header)));
    while (bufferpos % 4 != 0) {
        cmdbuf[bufferpos++] = 0x00;
    }
    hdr->pkt.length = SWAP32(bufferpos - (uint32_t)(sizeof (DTA_ComPacketHeader))
                             - (uint32_t)(sizeof (DTA_PacketHeader)));
    hdr->cp.length = SWAP32(bufferpos - (uint32_t)(sizeof (DTA_ComPacketHeader)));
	if (bufferpos > MAX_BUFFER_LENGTH) {
		LOG(D1) << " Standard Buffer Overrun " << bufferpos;
		exit(EXIT_FAILURE);
	}
}

void
DtaCommand::changeInvokingUid(std::vector<uint8_t> Invoker)
{
    LOG(D1) << "Entering DtaCommand::changeInvokingUid()";
    LOG(D2) << "Invoker is " << Invoker.data();
    int offset = sizeof (DTA_Header) + 1; /* bytes 2-9 */
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
uint16_t
DtaCommand::outputBufferSize() {
	//	if (MIN_BUFFER_LENGTH + 1 > bufferpos) return(MIN_BUFFER_LENGTH);
    return (uint16_t)(((bufferpos + 511) / 512) * 512);
}
void
DtaCommand::dumpCommand()
{
	DTA_Header * hdr = (DTA_Header *)cmdbuf;
	DtaHexDump(cmdbuf, SWAP32(hdr->cp.length) + sizeof(DTA_ComPacketHeader));
}
void
DtaCommand::dumpResponse()
{
	DTA_Header *hdr = (DTA_Header *)respbuf;
	DtaHexDump(respbuf, SWAP32(hdr->cp.length) + sizeof(DTA_ComPacketHeader));
}
void
DtaCommand::setcomID(uint16_t comID)
{
    LOG(D1) << "Entering DtaCommand::setcomID()";
    LOG(D2) << "comID " << comID;
    DTA_Header * hdr;
    hdr = (DTA_Header *) cmdbuf;
    hdr->cp.extendedComID[0] = ((comID & 0xff00) >> 8);
    hdr->cp.extendedComID[1] = (comID & 0x00ff);
    hdr->cp.extendedComID[2] = 0x00;
    hdr->cp.extendedComID[3] = 0x00;
}

void
DtaCommand::setTSN(uint32_t TSN)
{
    LOG(D1) << "Entering DtaCommand::setTSN()";
    LOG(D2) << "TSN is " << TSN;
    DTA_Header * hdr;
    hdr = (DTA_Header *) cmdbuf;
    hdr->pkt.TSN = TSN;
}

void
DtaCommand::setHSN(uint32_t HSN)
{
    LOG(D1) << "Entering DtaCommand::setHSN()";
    LOG(D2) << "HSN is " << HSN;
    DTA_Header * hdr;
    hdr = (DTA_Header *) cmdbuf;
    hdr->pkt.HSN = HSN;
}

DtaCommand::~DtaCommand()
{
    LOG(D1) << "Destroying DtaCommand";
}
