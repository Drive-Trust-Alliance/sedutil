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
#include <string>
#include "TCGresponse.h"
#include "endianfixup.h"
using namespace std;
TCGresponse::TCGresponse(void * buffer)
{
	std::vector<uint8_t> bytestring;
	uint8_t * reply = (uint8_t *)buffer;
	uint32_t cpos = 0;
	uint32_t tokenLength;
	memcpy(&h, buffer, sizeof(TCGHeader));
	tokens = 0;
	response.clear();
	reply += sizeof(TCGHeader);
	while (cpos < SWAP32(h.subpkt.length)) {
		bytestring.clear();
		if (!(reply[cpos] & 0x80))      //tiny atom 
			tokenLength = 1;
		else if (!(reply[cpos] & 0x40))  // short atom
			tokenLength = (reply[cpos] & 0x0f) + 1;
		else if (!(reply[cpos] & 0x20)) // medium atom
			tokenLength = ((reply[cpos] & 0x07) << 8) | reply[cpos+1] + 2;
		else if (!(reply[cpos] & 0x10)) // long atom
			tokenLength = ((reply[cpos + 1] << 16) | (reply[cpos+2] << 8) | reply[cpos+3]) + 4;
		else // TOKEN
			tokenLength = 1;

		for(uint32_t i = 0; i < tokenLength; i++) {
			bytestring.push_back(reply[cpos++]);
		}
		response.push_back(bytestring);
		tokens++;
	}
}
TCG_TOKENID TCGresponse::tokenIs(uint32_t tokenNum){
	if (!(response[tokenNum][0] & 0x80)) {     //tiny atom 
		if ((response[tokenNum][0] & 0x40))
			return TCG_TOKENID::TCG_TOKENID_SINT;
		else
			return TCG_TOKENID::TCG_TOKENID_UINT;
	}
	else if (!(response[tokenNum][0] & 0x40)) {  // short atom
		if ((response[tokenNum][0] & 0x20))
			return TCG_TOKENID::TCG_TOKENID_BYTESTRING;
		else if ((response[tokenNum][0] & 0x10))
			return TCG_TOKENID::TCG_TOKENID_SINT;
		else
			return TCG_TOKENID::TCG_TOKENID_UINT;
	}
	else if (!(response[tokenNum][0] & 0x20)) { // medium atom
		if ((response[tokenNum][0] & 0x10))
		return TCG_TOKENID::TCG_TOKENID_BYTESTRING;
		else if ((response[tokenNum][0] & 0x08))
			return TCG_TOKENID::TCG_TOKENID_SINT;
		else
			return TCG_TOKENID::TCG_TOKENID_UINT;
	}
	else if (!(response[tokenNum][0] & 0x10)) { // long atom
		if ((response[tokenNum][0] & 0x02))
			return TCG_TOKENID::TCG_TOKENID_BYTESTRING;
		else if ((response[tokenNum][0] & 0x01))
			return TCG_TOKENID::TCG_TOKENID_SINT;
		else
			return TCG_TOKENID::TCG_TOKENID_UINT;
	}
	else // TOKEN
		return (TCG_TOKENID)response[tokenNum][0];
}
uint32_t TCGresponse::getLength(uint32_t tokenNum) {
	return response[tokenNum].size();
	}
uint64_t TCGresponse::getUint64(uint32_t tokenNum) {
	if (!(response[tokenNum][0] & 0x80)) {     //tiny atom 
		if ((response[tokenNum][0] & 0x40)) {
			LOG(E) << "unsigned int requested for signed tiny atom";
			return 0;
		}
		else {
			return (response[tokenNum][0] & 0x3f);
		}
	}
	else if (!(response[tokenNum][0] & 0x40)) {  // short atom
		if ((response[tokenNum][0] & 0x10)) {
			LOG(E) << "unsigned int requested for signed short atom";
			return 0;
		}
		else {
			uint64_t whatever = 0;
			int b = 0;
			for (uint32_t i = response[tokenNum].size() - 1; i > 0; i--) {
				whatever |= (response[tokenNum][i] << (8 * b));
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


uint32_t TCGresponse::getUint32(uint32_t tokenNum) {
	return (uint32_t) getUint64(tokenNum);
}
uint16_t TCGresponse::getUint16(uint32_t tokenNum) {
	return (uint16_t) getUint64(tokenNum);
}
uint8_t TCGresponse::getUint8(uint32_t tokenNum) {
	return (uint8_t) getUint64(tokenNum);
}
//int64_t TCGresponse::getSint(uint32_t tokenNum) {
//	LOG(E) << "TCGresponse::getSint() is not implemented";
//}
std::string TCGresponse::getString(uint32_t tokenNum) {
	std::string s;
	s.erase();
	int overhead = 0;
	if (!(response[tokenNum][0] & 0x80)) {     //tiny atom 
		LOG(E) << "Cannot get a string from a tiny atom";
		s;
	}
	else if (!(response[tokenNum][0] & 0x40)) {  // short atom
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
	for (uint32_t i = overhead; i < response[tokenNum].size() ; i++){
		s.push_back(response[tokenNum][i]);
	}
	return s;
}
void TCGresponse::getBytes(uint32_t tokenNum, uint8_t bytearray[]) {
	int overhead = 0;
	if (!(response[tokenNum][0] & 0x80)) {     //tiny atom 
		LOG(E) << "Cannot get a bytestring from a tiny atom";
		return;
	}
	else if (!(response[tokenNum][0] & 0x40)) {  // short atom
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

	for (uint32_t i = overhead; i < response[tokenNum].size(); i++){
		bytearray[i-overhead] = response[tokenNum][i];
	}
}
uint32_t TCGresponse::getTokenCount() { return tokens; }
TCGresponse::~TCGresponse()
{
}
