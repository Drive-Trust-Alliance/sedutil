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
#pragma once
#include "DtaDiskType.h"

using namespace std;

DtaDiskType::DtaDiskType() {}
DtaDiskType::~DtaDiskType() {}
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif
void DtaDiskType::init(const char * devref) {}
uint8_t	DtaDiskType::sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
	void * buffer, uint32_t bufferlen) {
	return 10;
}
void DtaDiskType::identify(OPAL_DiskInfo& disk_info) {}
#ifdef _MSC_VER
#pragma warning(pop)
#endif
