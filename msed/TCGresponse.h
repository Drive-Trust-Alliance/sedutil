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
#include <vector>
#include <string>
#include "TCGstructures.h"

typedef enum _TCG_TOKENID {
	TCG_TOKENID_BYTESTRING,
	TCG_TOKENID_SINT,
	TCG_TOKENID_UINT,
	TCG_TOKENID_TOKEN, // actual token is returned
} TCG_TOKENID;

class TCGresponse {
public:
    TCGresponse(void * buffer);
    ~TCGresponse();
	TCG_TOKENID tokenIs(uint32_t tokenNum);
	uint32_t getLength(uint32_t tokenNum);
	uint64_t getUint64(uint32_t tokenNum);
	uint32_t getUint32(uint32_t tokenNum);
	uint16_t getUint16(uint32_t tokenNum);
	uint8_t getUint8(uint32_t tokenNum);
	uint32_t getTokenCount();
	//int64_t getSint(uint32_t tokenNum);
	std::string getString(uint32_t tokenNum);
	void getBytes(uint32_t tokenNum, uint8_t bytearray[]);
	TCGHeader h;  // this should be private with accessors but .....

private:
	TCGresponse();
	std::vector<std::vector<uint8_t>> response;
};



