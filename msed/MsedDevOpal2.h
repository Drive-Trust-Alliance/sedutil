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
#include "os.h"
#include "MsedDevOpal.h"

using namespace std;
/** Class representing a disk device, this class is represents a disk that conforms
* to the OPAL 2.0 SSC
*
* testing so far indicates that the functions implemented in this program
* function the same in OPAL 1.0 and Opal 2.0
*/
class MsedDevOpal2 : public MsedDevOpal {
public:
	MsedDevOpal2(const char * devref);
	~MsedDevOpal2();
        /** return the communication ID to be used with this device */
	uint16_t comID();
	
};