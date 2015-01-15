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
#include <vector>
#include <string>
#include "MsedStructures.h"

typedef enum _OPAL_TOKENID {
	// 0xen to avoid collisions with real tokens 
	OPAL_TOKENID_BYTESTRING = 0xe0,
	OPAL_TOKENID_SINT = 0xe1,
	OPAL_TOKENID_UINT = 0xe2,
	OPAL_TOKENID_TOKEN =0xe3, // actual token is returned
} OPAL_TOKENID;

class MsedResponse {
public:
	MsedResponse();
    MsedResponse(void * buffer);
	~MsedResponse();
	void init(void * buffer);
	OPAL_TOKENID tokenIs(uint32_t tokenNum);
	uint32_t getLength(uint32_t tokenNum);
	uint64_t getUint64(uint32_t tokenNum);
	uint32_t getUint32(uint32_t tokenNum);
	uint16_t getUint16(uint32_t tokenNum);
	uint8_t getUint8(uint32_t tokenNum);
	uint32_t getTokenCount();
	//int64_t getSint(uint32_t tokenNum);
	std::string getString(uint32_t tokenNum);
	std::vector<uint8_t> getRawToken(uint32_t tokenNum);
	void getBytes(uint32_t tokenNum, uint8_t bytearray[]);
	OPALHeader h;  // this should be private with accessors but .....

private:

	std::vector<std::vector<uint8_t>> response;
};



