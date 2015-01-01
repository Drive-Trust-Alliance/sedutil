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
#include "MsedBaseDev.h"
/** Device Class (win32) represents a single disk device.
*  This class is stored in a vector so to avoid writing a
*  copy operator and an assignment operator no custom destructor
*  is used leading to this unfortunate class method structure
*/
class MsedDev : public MsedBaseDev {
public:
	MsedDev(const char * devref);
	~MsedDev();
	/** Send an ioctl to the device using pass through. */
	uint8_t	sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
		void * buffer, uint16_t bufferlen);
private:
	void osmsSleep(uint32_t milliseconds);
	void identify();
	HANDLE hDev;
	void *ataPointer; 
};
