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


#include "os.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "DtaDevEnterprise.h"
#include "DtaHashPwd.h"
#include "DtaEndianFixup.h"
#include "DtaStructures.h"
#include "DtaCommand.h"
#include "DtaResponse.h"
#include "DtaSession.h"
#include "DtaHexDump.h"
#include "DtaAnnotatedDump.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
DtaToken::DtaToken(void) :
////////////////////////////////////////////////////////////////////////////////
    m_TokenType     (NONE),
    m_HeaderLength  (0),
    m_DataLength    (0),
    m_TokenLength   (0),
    m_token         (NULL),
    m_data          (NULL),
    m_value         (0ULL),
    m_sign          (false)
{
}

////////////////////////////////////////////////////////////////////////////////
DtaToken::~DtaToken(void)
////////////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////////////
void DtaToken::str2int(uint8_t * buf, bool byte)
////////////////////////////////////////////////////////////////////////////////
{
    // user says it's a byte string, or it's so large it must be
    if (byte || m_DataLength > sizeof(m_value))
    {
        m_data = buf;
        m_value = 0ULL;
        return;
    }

    // zero-sized ?
    if (m_DataLength == 0)
    {
        m_data = NULL;
        m_value = 0ULL;
        return;
    }

    // zero-extend 1B to 8B sized BE integer
    uint64_t V = 0;
    uint8_t * n = (uint8_t *) &V;

    memset(n, '\0', sizeof(V));
    memcpy(n+(sizeof(V)-m_DataLength), buf, m_DataLength);

    // convert integer to host byte order
    V = SWAP64(V);

    // sign-extend if appropriate
    if (m_sign && (m_DataLength < sizeof(V)))
    {
        const int shift = 8 * (sizeof(V) - m_DataLength);
        int64_t v = V;
        v = v << shift;
        v = v >> shift;
        V = v;
    }

    // write out integer value
    m_data = NULL;
    m_value = V;
}

////////////////////////////////////////////////////////////////////////////////
void DtaToken::parse(uint8_t * buf, uint32_t buflen)
////////////////////////////////////////////////////////////////////////////////
{
	LOG(D1) << "Entering DtaToken::parse " << buflen;
    m_token = buf;

    const CAtomHeader & atom = * (CAtomHeader *) buf;

    if (atom.TinyAtom.indicator == atom.TinyAtom.INDICATOR)
    {
        m_TokenType = TINY_ATOM;
        m_sign = atom.TinyAtom.sign;
        m_HeaderLength = atom.TinyAtom.HEADER_SIZE;
        m_DataLength = 0;
        m_TokenLength = atom.TinyAtom.HEADER_SIZE;
        m_data = NULL;
        if (m_sign)
        {
            m_value = int64_t(atom.TinyAtom_sign.value);
        }
        else
        {
            m_value = uint64_t(atom.TinyAtom.value);
        }
    }
    else if (atom.ShortAtom.indicator == atom.ShortAtom.INDICATOR)
    {
        m_TokenType = SHORT_ATOM;
        m_sign = atom.ShortAtom.sign;
        m_HeaderLength = atom.ShortAtom.HEADER_SIZE;
        m_DataLength = atom.ShortAtom.length;
        m_TokenLength = atom.ShortAtom.HEADER_SIZE + m_DataLength;
        str2int(m_token+atom.ShortAtom.HEADER_SIZE, atom.ShortAtom.byte);
    }
    else if (atom.MediumAtom.indicator == atom.MediumAtom.INDICATOR)
    {
        m_TokenType = MEDIUM_ATOM;
        m_sign = atom.MediumAtom.sign;
        m_HeaderLength = atom.MediumAtom.HEADER_SIZE;
        m_DataLength = uint32_t(atom.MediumAtom.length_msb) << 8 | uint32_t(atom.MediumAtom.length_lsb);
        m_TokenLength = atom.MediumAtom.HEADER_SIZE + m_DataLength;
        str2int(m_token+atom.MediumAtom.HEADER_SIZE, atom.MediumAtom.byte);
    }
    else if (atom.LongAtom.indicator == atom.LongAtom.INDICATOR)
    {
        m_TokenType = LONG_ATOM;
        m_sign = atom.LongAtom.sign;
        m_HeaderLength = atom.LongAtom.HEADER_SIZE;
        m_DataLength = (uint32_t(atom.LongAtom.length[0]) << 8 | uint32_t(atom.LongAtom.length[1])) << 8 | uint32_t(atom.LongAtom.length[2]);
        m_TokenLength = atom.LongAtom.HEADER_SIZE + m_DataLength;
        str2int(m_token+atom.LongAtom.HEADER_SIZE, atom.LongAtom.byte);
    }
    else
    {
        m_TokenType = TOKEN;
        m_sign = 0;
        m_HeaderLength = 1;
        m_DataLength = 0;
        m_TokenLength = 1;
        m_data = NULL;
        m_value = m_token[0];
    }
}

////////////////////////////////////////////////////////////////////////////////
int DtaToken::print(FILE *stream, uint32_t buflen)
////////////////////////////////////////////////////////////////////////////////
{
    int ret = 0;

    ret += fprintf(stream, "%u\t", m_TokenLength);

    // print out atom header
    unsigned int len = min(m_HeaderLength, buflen);
    ret += fprintf(stream, "(");
    for(unsigned int n=0; n<len; n++) ret += fprintf(stream, " %2.2X", uint32_t(m_token[n])&0XFF);
    ret += fprintf(stream, " )");
    buflen -= len;

    // print out atom value
    len = min(m_DataLength, buflen);

    if (m_TokenType == TOKEN)
    {
        ret += fprintf(stream, " ");
        ret += printTokenType(stream, m_token[0]);
        ret += fprintf(stream, "\n");
    }
    else if (m_data)
    {
        // byte string
        for(unsigned int n=0; n<len; n++) ret += fprintf(stream, " %2.2X", uint32_t(m_data[n])&0XFF);
        ret += printAscii(stream, m_data, len);
        ret += fprintf(stream, "\n");
    }
    else
    {
        // not byte string
        ret += fprintf(stream, " %llu (%llXh)\n", (long long unsigned int)m_value, (long long unsigned int)m_value);
    }
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
int DtaToken::printTokenType(FILE *stream, uint8_t token)
////////////////////////////////////////////////////////////////////////////////
{
    // see Table 04 p. 35 in TSG Storage Architecture Core Specification 2.0
    const char * p;
    switch(token)
    {
    default:
        p = "unrecognized token";
        break;
    case STARTLIST:
        p = "Start_List";
        break;
    case ENDLIST:
        p = "End_List";
        break;
    case STARTNAME:
        p = "Start_Name";
        break;
    case ENDNAME:
        p = "End_Name";
        break;
    case CALL:
        p = "Call";
        break;
    case ENDOFDATA:
        p = "End_of_Data";
        break;
    case ENDOFSESSION:
        p = "End_of_Session";
        break;
    case STARTTRANSACTON:
        p = "Start_Transaction";
        break;
    case ENDTRANSACTON:
        p = "End_Transaction";
        break;
    case EMPTYATOM:
        p = "Empty_Atom";
        break;
    }

    return fprintf(stream, "%s", p);
}

////////////////////////////////////////////////////////////////////////////////
int DtaToken::printUID(FILE *stream, uint8_t buf[8])
////////////////////////////////////////////////////////////////////////////////
{
    struct uidlist
    {
        uint8_t         UID[8];
        const char *    name;
    };
    
    struct uidlist list1[] =
    {
		{ { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 }, "ThisSP" },
		{ { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF }, "Session Manager UID" },
		{ { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x01 }, "Session Properties Method UID" },
		{ { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x02 }, "StartSessionMethod" },
		{ { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x03 }, "SyncSession Method UID" },
		{ { 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06 }, "Enterprise Get" },
		{ { 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x07 }, "Enterprise Set" },
		{ { 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x0C }, "Enterprise Authenticate" },
		{ { 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x10 }, "GenKey" },
		{ { 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x11 }, "RevertSP" },
		{ { 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x02, 0x02 }, "Revert" },
		{ { 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x08, 0x03 }, "Erase" },
		{ { 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x01 }, "Anybody" },
		{ { 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x06 }, "SID" },
		{ { 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x84, 0x01 }, "EraseMaster" },
		{ { 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x01 }, "C_PIN" },
		{ { 0x00, 0x00, 0x02, 0x05, 0x00, 0x00, 0x00, 0x01 }, "SP Admin" },
		{ { 0x00, 0x00, 0x02, 0x05, 0x00, 0x01, 0x00, 0x01 }, "SP" },
		{ { 0x00, 0x00, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00 }, "Enterprise Locking Info" },
	};

    const int len1 = sizeof(list1) / sizeof(list1[0]);

    for(int n=0; n<len1; n++)
    {
        if (!memcmp(list1[n].UID, buf, sizeof(list1[n].UID))) return fprintf(stream, "%s", list1[n].name);
    }

    // band-specific UID ?
    const uint32_t band = uint32_t((buf[6] & 0x7F) | buf[7]) - 1;
    if (band == uint32_t(-1)) return 0;

    uint8_t buf2[8];
    memcpy(buf2, buf, sizeof(buf2));
    buf2[6] &= 0x80;
    buf2[7] = 0x00;

    struct uidlist list2[] =
    {
		{ { 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x80, 0x00 }, "BandMaster%u" },
		{ { 0x00, 0x00, 0x08, 0x02, 0x00, 0x00, 0x00, 0x00 }, "Locking Range %u" },
		{ { 0x00, 0x00, 0x08, 0x06, 0x00, 0x00, 0x00, 0x00 }, "Band%u_AES_256" },
    };

    const int len2 = sizeof(list2) / sizeof(list2[0]);

    for(int n=0; n<len2; n++)
    {
        if (!memcmp(list2[n].UID, buf2, sizeof(list2[n].UID))) return fprintf(stream, list2[n].name, band);
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
int DtaToken::printAscii(FILE *stream, uint8_t * buf, uint32_t buflen)
////////////////////////////////////////////////////////////////////////////////
{
    int ret = 0;
    ret += fprintf(stream, " (\"");
    if (buflen == 8)
    {
        int ret2 = printUID(stream, buf);
        if (ret2 > 0)
        {
            buflen = 0;
            ret += ret2;
        }
    }
    for(unsigned int n=0; n<buflen; n++)
    {
        const char c = isprint(m_data[n]) ? m_data[n] : '.';
        ret += fprintf(stream, "%c", c);
    }
    ret += fprintf(stream, "\")");
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
uint8_t DtaAnnotatedDump(ATACOMMAND cmd, void * buffer, uint32_t bufferlen)
////////////////////////////////////////////////////////////////////////////////
{
    FILE * stream = stderr;

    // hello
    if (cmd == IF_RECV)
        fprintf(stream, "<< IF_RECV >>\n");
    else if (cmd == IF_SEND)
        fprintf(stream, "<< IF_SEND >>\n");
    else
        fprintf(stream, "<< 0x%2.2X\n >>\n", cmd);

    // echo header
    DTA_Header h;
    memcpy(&h, buffer, sizeof(h));
    IFLOG(D1)
    {
        fprintf(stream, "ComPacket.extendedComID    %2.2X%2.2X%2.2X%2.2X\n",
            h.cp.extendedComID[0], h.cp.extendedComID[1], h.cp.extendedComID[2], h.cp.extendedComID[3]);
        fprintf(stream, "ComPacket.outstandingData  %8.8X\n", SWAP32(h.cp.outstandingData));
        fprintf(stream, "ComPacket.minTransfer      %8.8X\n", SWAP32(h.cp.minTransfer));
        fprintf(stream, "ComPacket.length           %8.8X\n", SWAP32(h.cp.length));
        fprintf(stream, "Packet.TSN                 %8.8X\n", SWAP32(h.pkt.TSN));
        fprintf(stream, "Packet.HSN                 %8.8X\n", SWAP32(h.pkt.HSN));
        fprintf(stream, "Packet.seqNumber           %8.8X\n", SWAP32(h.pkt.seqNumber));
        fprintf(stream, "Packet.ackType             %8.8X\n", SWAP16(h.pkt.ackType));
        fprintf(stream, "Packet.acknowledgement     %8.8X\n", SWAP32(h.pkt.acknowledgement));
        fprintf(stream, "Packet.length              %8.8X\n", SWAP32(h.pkt.length));
        fprintf(stream, "DataSubPacket.kind         %8.8X\n", SWAP16(h.subpkt.kind));
        fprintf(stream, "DataSubPacket.length       %8.8X\n", SWAP32(h.subpkt.length));
    }
    
    const uint32_t buflen = SWAP32(h.subpkt.length);
    if (buflen > bufferlen)
    {
        fprintf(stream, "Overflow: h.subpkt.length = %u, bufferlen = %u\n", buflen, bufferlen);
        return 0xff;
    }

    // set up pointers to scan buffer
    unsigned char * p = (unsigned char *) buffer + sizeof(h);
    unsigned char * q = p + buflen;

    // scan buffer
    while(p < q)
    {
        DtaToken token;
        token.parse(p, (uint32_t)(q-p));
        token.print(stream, (uint32_t)(q-p));

        if (p + token.m_TokenLength > q) fprintf(stream, "(token truncated)");

        p += token.m_TokenLength;
    }

    return 0;
}
