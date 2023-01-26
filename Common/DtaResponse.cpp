/* C:B**************************************************************************
This software is Copyright 2014, 2022 Bright Plaza Inc. <drivetrust@drivetrust.com>

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
#include <log/log.h>
#include "DtaResponse.h"
#include "DtaEndianFixup.h"
using namespace std;

DtaResponse::DtaResponse()
{
    LOG(D1) << "Creating  DtaResponse()";
}

DtaResponse::DtaResponse(void * buffer)
{
    LOG(D1) << "Creating  DtaResponse(buffer)";
    LOG(D2) << "buffer is " << buffer;
    init(buffer);
}

void
DtaResponse::init(void * buffer)
{
    LOG(D1) << "Entering  DtaResponse::init(buffer)";
    LOG(D2) << "buffer is " << buffer;
    std::vector<uint8_t> bytestring, empty_atom(1, 0xff);
    uint8_t * reply = (uint8_t *) buffer;
    uint32_t cpos = 0;
    uint32_t tokenLength;
    memcpy(&h, buffer, sizeof (DTA_Header));
    response.clear();
    reply += sizeof (DTA_Header);
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

OPAL_TOKEN DtaResponse::tokenIs(uint32_t tokenNum)
{
    LOG(D1) << "Entering  DtaResponse::tokenIs(tokenNum)";
    LOG(D2) << "tokenNum is " << tokenNum;
    if (!(response[tokenNum][0] & 0x80)) { //tiny atom
        if ((response[tokenNum][0] & 0x40))
            return OPAL_TOKEN::DTA_TOKENID_SINT;
        else
            return OPAL_TOKEN::DTA_TOKENID_UINT;
    }
    else if (!(response[tokenNum][0] & 0x40)) { // short atom
        if ((response[tokenNum][0] & 0x20))
            return OPAL_TOKEN::DTA_TOKENID_BYTESTRING;
        else if ((response[tokenNum][0] & 0x10))
            return OPAL_TOKEN::DTA_TOKENID_SINT;
        else
            return OPAL_TOKEN::DTA_TOKENID_UINT;
    }
    else if (!(response[tokenNum][0] & 0x20)) { // medium atom
        if ((response[tokenNum][0] & 0x10))
            return OPAL_TOKEN::DTA_TOKENID_BYTESTRING;
        else if ((response[tokenNum][0] & 0x08))
            return OPAL_TOKEN::DTA_TOKENID_SINT;
        else
            return OPAL_TOKEN::DTA_TOKENID_UINT;
    }
    else if (!(response[tokenNum][0] & 0x10)) { // long atom
        if ((response[tokenNum][0] & 0x02))
            return OPAL_TOKEN::DTA_TOKENID_BYTESTRING;
        else if ((response[tokenNum][0] & 0x01))
            return OPAL_TOKEN::DTA_TOKENID_SINT;
        else
            return OPAL_TOKEN::DTA_TOKENID_UINT;
    }
    else // TOKEN
        return (OPAL_TOKEN) response[tokenNum][0];
}

uint32_t DtaResponse::getLength(uint32_t tokenNum)
{
    return (uint32_t) response[tokenNum].size();
}

uint64_t DtaResponse::getUint64(uint32_t tokenNum)
{
    LOG(D1) << "Entering  DtaResponse::getUint64(tokenNum)";
    LOG(D2) << "tokenNum is " << tokenNum;
    if (!(response[tokenNum][0] & 0x80)) { //tiny atom
        if ((response[tokenNum][0] & 0x40)) {
            LOG(E) << "unsigned int requested for signed tiny atom";
			exit(EXIT_FAILURE);
        }
        else {
            return (uint64_t) (response[tokenNum][0] & 0x3f);
        }
    }
    else if (!(response[tokenNum][0] & 0x40)) { // short atom
        if ((response[tokenNum][0] & 0x10)) {
            LOG(E) << "unsigned int requested for signed short atom";
			exit(EXIT_FAILURE);
        }
        else {
            uint64_t whatever = 0;
            if (response[tokenNum].size() > 9) { LOG(E) << "UINT64 with greater than 8 bytes"; }
            int b = 0;
            for (uint32_t i = (uint32_t) response[tokenNum].size() - 1; i > 0; i--) {
				whatever |= ((uint64_t)response[tokenNum][i] << (8 * b));
                b++;
            }
            return whatever;
        }

    }
    else if (!(response[tokenNum][0] & 0x20)) { // medium atom
        LOG(E) << "unsigned int requested for medium atom is unsupported";
		exit(EXIT_FAILURE);
    }
    else if (!(response[tokenNum][0] & 0x10)) { // long atom
        LOG(E) << "unsigned int requested for long atom is unsupported";
		exit(EXIT_FAILURE);
    }
    else { // TOKEN
        LOG(E) << "unsigned int requested for token is unsupported";
		//exit(EXIT_FAILURE); // JERRY this exit cause the session not close and contribute to SP_BUSY
		return EXIT_FAILURE;
    }
}

uint32_t DtaResponse::getUint32(uint32_t tokenNum)
{
    LOG(D1) << "Entering  DtaResponse::getUint32(tokenNum)";
    LOG(D2) << "tokenNum is " << tokenNum;
    uint64_t i = getUint64(tokenNum);
    if (i > 0xffffffff) { LOG(E) << "UINT32 truncated "; }
    return (uint32_t) i;

}

uint16_t DtaResponse::getUint16(uint32_t tokenNum)
{
    LOG(D1) << "Entering  DtaResponse::getUint16(tokenNum)";
    LOG(D2) << "tokenNum is " << tokenNum;
    uint64_t i = getUint64(tokenNum);
    if (i > 0xffff) { LOG(E) << "UINT16 truncated "; }
    return (uint16_t) i;
}

uint8_t DtaResponse::getUint8(uint32_t tokenNum)
{
	LOG(D1) << "Entering  DtaResponse::getUint8(tokenNum)";
    LOG(D2) << "tokenNum is " << tokenNum;
    uint64_t i = getUint64(tokenNum);
    if (i > 0xff) { LOG(E) << "UINT8 truncated "; }
    return (uint8_t) i;
}
//int64_t DtaResponse::getSint(uint32_t tokenNum) {
//	LOG(E) << "DtaResponse::getSint() is not implemented";
//}

std::vector<uint8_t> DtaResponse::getRawToken(uint32_t tokenNum)
{
    return response[tokenNum];
}

std::string DtaResponse::getString(uint32_t tokenNum)
{
    LOG(D1) << "Entering  DtaResponse::getString(tokenNum)";
    LOG(D2) << "tokenNum is " << tokenNum;
    std::string s;
    s.erase();
    int overhead = 0;
    if (!(response[tokenNum][0] & 0x80)) { //tiny atom
        LOG(E) << "Cannot get a string from a tiny atom";
		exit(EXIT_FAILURE);
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

void DtaResponse::getBytes(uint32_t tokenNum, uint8_t bytearray[])
{
    LOG(D1) << "Entering  DtaResponse::getBytes(tokenNum, bytearray)";
    LOG(D2) << "tokenNum is " << tokenNum << " bytearray is " << bytearray;
    int overhead = 0;
    if (!(response[tokenNum][0] & 0x80)) { //tiny atom
        LOG(E) << "Cannot get a bytestring from a tiny atom";
		exit(EXIT_FAILURE);
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
		exit(EXIT_FAILURE);
    }

    for (uint32_t i = overhead; i < response[tokenNum].size(); i++) {
        bytearray[i - overhead] = response[tokenNum][i];
    }
}

uint32_t DtaResponse::getTokenCount()
{
    LOG(D1) << "Entering  DtaResponse::getTokenCount()";
    return (uint32_t) response.size();
}

DtaResponse::~DtaResponse()
{
    LOG(D1) << "Destroying DtaResponse";
}
