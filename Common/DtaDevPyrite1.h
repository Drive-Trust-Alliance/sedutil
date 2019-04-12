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
#include "os.h"
#include "DtaDevOpal.h"

using namespace std;
/** Class representing a disk device, this class is represents a disk that conforms
* to the Pyrite 1.0 SSC
*/
class DtaDevPyrite1 : public DtaDevOpal {
public:
	DtaDevPyrite1(const char * devref);
	~DtaDevPyrite1();
        /** return the communication ID to be used with this device */
	uint16_t comID();
	
};