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
#include "MsedResponse.h"
#include "MsedEndianFixup.h"
using namespace std;

MsedResponse::MsedResponse()
{
    LOG(D4) << "Creating  MsedResponse()";
}

MsedResponse::MsedResponse(void * buffer)
{
    LOG(D4) << "Creating  MsedResponse(buffer)";
    init(buffer);
}

void
MsedResponse::init(void * buffer)
{
    LOG(D4) << "Entering  MsedResponse::init";
    std::vector<uint8_t> bytestring, empty_atom(1, 0xff);
    uint8_t * reply = (uint8_t *) buffer;
    uint32_t cpos = 0;
    uint32_t tokenLength;
    memcpy(&h, buffer, sizeof (OPALHeader));
    response.clear();
    reply += sizeof (OPALHeader);
    while (cpos < SWAP32(h.subpkt.length)) {
        bytestring.clear();
        if (!(reply[cpos] & 0x80)) //tiny atom
            tokenLength = 1;
        else if (!(reply[cpos] & 0x40)) // short atom
            tokenLength = (reply[cpos] & 0x0f) + 1;
        else if (!(reply[cpos] & 0x20)) // medium atom
            tokenLength = (((reply[cpos] & 0x07) << 8) | reply[cpos + 1]) + 2;
        else if (!(reply[cpos] & 0x10)) // long atom
            tokenLength = ((reply[cpos + 1] << 16) | (reply[cpos + 2] << 8) | reply[cpos + 3]) + 4;
        else // TOKEN
            tokenLength = 1;

        for (uint32_t i = 0; i < tokenLength; i++) {
            bytestring.push_back(reply[cpos++]);
        }
		if (bytestring != empty_atom)
			response.push_back(bytestring);
    }
}

OPAL_TOKENID MsedResponse::tokenIs(uint32_t tokenNum)
{
    LOG(D4) << "Entering  MsedResponse::tokenIs";
    if (!(response[tokenNum][0] & 0x80)) { //tiny atom
        if ((response[tokenNum][0] & 0x40))
            return OPAL_TOKENID::OPAL_TOKENID_SINT;
        else
            return OPAL_TOKENID::OPAL_TOKENID_UINT;
    }
    else if (!(response[tokenNum][0] & 0x40)) { // short atom
        if ((response[tokenNum][0] & 0x20))
            return OPAL_TOKENID::OPAL_TOKENID_BYTESTRING;
        else if ((response[tokenNum][0] & 0x10))
            return OPAL_TOKENID::OPAL_TOKENID_SINT;
        else
            return OPAL_TOKENID::OPAL_TOKENID_UINT;
    }
    else if (!(response[tokenNum][0] & 0x20)) { // medium atom
        if ((response[tokenNum][0] & 0x10))
            return OPAL_TOKENID::OPAL_TOKENID_BYTESTRING;
        else if ((response[tokenNum][0] & 0x08))
            return OPAL_TOKENID::OPAL_TOKENID_SINT;
        else
            return OPAL_TOKENID::OPAL_TOKENID_UINT;
    }
    else if (!(response[tokenNum][0] & 0x10)) { // long atom
        if ((response[tokenNum][0] & 0x02))
            return OPAL_TOKENID::OPAL_TOKENID_BYTESTRING;
        else if ((response[tokenNum][0] & 0x01))
            return OPAL_TOKENID::OPAL_TOKENID_SINT;
        else
            return OPAL_TOKENID::OPAL_TOKENID_UINT;
    }
    else // TOKEN
        return (OPAL_TOKENID) response[tokenNum][0];
}

uint32_t MsedResponse::getLength(uint32_t tokenNum)
{
    return (uint32_t) response[tokenNum].size();
}

uint64_t MsedResponse::getUint64(uint32_t tokenNum)
{
    LOG(D4) << "Entering  MsedResponse::getUint64";
    if (!(response[tokenNum][0] & 0x80)) { //tiny atom
        if ((response[tokenNum][0] & 0x40)) {
            LOG(E) << "unsigned int requested for signed tiny atom";
            return 0;
        }
        else {
            return (uint64_t) (response[tokenNum][0] & 0x3f);
        }
    }
    else if (!(response[tokenNum][0] & 0x40)) { // short atom
        if ((response[tokenNum][0] & 0x10)) {
            LOG(E) << "unsigned int requested for signed short atom";
            return 0;
        }
        else {
            uint64_t whatever = 0;
            if (response[tokenNum].size() > 9)
                LOG(E) << "UINT64 with greater than 8 bytes";
            int b = 0;
            for (uint32_t i = (uint32_t) response[tokenNum].size() - 1; i > 0; i--) {
				whatever |= ((uint64_t)response[tokenNum][i] << (8 * b));
                b++;
            }
            return whatever;
        }

    }
    else if (!(response[tokenNum][0] & 0x20)) { // medium atom
        LOG(E) << "unsigned int requested for medium atom is unsuported";
        return 0;
    }
    else if (!(response[tokenNum][0] & 0x10)) { // long atom
        LOG(E) << "unsigned int requested for long atom is unsuported";
        return 0;
    }
    else { // TOKEN
        LOG(E) << "unsigned int requested for token is unsuported";
        return 0;
    }
}

uint32_t MsedResponse::getUint32(uint32_t tokenNum)
{
    LOG(D4) << "Entering  MsedResponse::getUint32";
    uint64_t i = getUint64(tokenNum);
    if (i > 0xffffffff)
        LOG(E) << "UINT32 truncated ";
    return (uint32_t) i;

}

uint16_t MsedResponse::getUint16(uint32_t tokenNum)
{
    LOG(D4) << "Entering  MsedResponse::getUint16";
    uint64_t i = getUint64(tokenNum);
    if (i > 0xffff)
        LOG(E) << "UINT16 truncated ";
    return (uint16_t) i;
}

uint8_t MsedResponse::getUint8(uint32_t tokenNum)
{
	LOG(D4) << "Entering  MsedResponse::getUint8";
    uint64_t i = getUint64(tokenNum);
    if (i > 0xff)
        LOG(E) << "UINT8 truncated ";
    return (uint8_t) i;
}
//int64_t MsedResponse::getSint(uint32_t tokenNum) {
//	LOG(E) << "MsedResponse::getSint() is not implemented";
//}

std::vector<uint8_t> MsedResponse::getRawToken(uint32_t tokenNum)
{
    return response[tokenNum];
}

std::string MsedResponse::getString(uint32_t tokenNum)
{
    LOG(D4) << "Entering  MsedResponse::getString";
    std::string s;
    s.erase();
    int overhead = 0;
    if (!(response[tokenNum][0] & 0x80)) { //tiny atom
        LOG(E) << "Cannot get a string from a tiny atom";
        s;
    }
    else if (!(response[tokenNum][0] & 0x40)) { // short atom
        overhead = 1;
    }
    else if (!(response[tokenNum][0] & 0x20)) { // medium atom
        overhead = 2;
    }
    else if (!(response[tokenNum][0] & 0x10)) { // long atom
        overhead = 4;
    }
    else {
        LOG(E) << "Cannot get a string from a TOKEN";
        return s;
    }
    for (uint32_t i = overhead; i < response[tokenNum].size(); i++) {
        s.push_back(response[tokenNum][i]);
    }
    return s;
}

void MsedResponse::getBytes(uint32_t tokenNum, uint8_t bytearray[])
{
    LOG(D4) << "Entering  MsedResponse::getBytes";
    int overhead = 0;
    if (!(response[tokenNum][0] & 0x80)) { //tiny atom
        LOG(E) << "Cannot get a bytestring from a tiny atom";
        return;
    }
    else if (!(response[tokenNum][0] & 0x40)) { // short atom
        overhead = 1;
    }
    else if (!(response[tokenNum][0] & 0x20)) { // medium atom
        overhead = 2;
    }
    else if (!(response[tokenNum][0] & 0x10)) { // long atom
        overhead = 4;
    }
    else {
        LOG(E) << "Cannot get a bytestring from a TOKEN";
        return;
    }

    for (uint32_t i = overhead; i < response[tokenNum].size(); i++) {
        bytearray[i - overhead] = response[tokenNum][i];
    }
}

uint32_t MsedResponse::getTokenCount()
{
    LOG(D4) << "Entering  MsedResponse::getTokenCount()";
    return (uint32_t) response.size();
}

MsedResponse::~MsedResponse()
{
    LOG(D4) << "Destroying MsedResponse";
}
