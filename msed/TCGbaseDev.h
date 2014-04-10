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

#include "TCGstructures.h"

class TCGbaseDev {
public:
    TCGbaseDev();
    ~TCGbaseDev();
    virtual uint8_t 
		sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
            void * buffer, uint16_t bufferlen) = 0;
    uint8_t isOpal2();
    uint8_t isPresent();
    uint16_t comID();
    void puke();
protected:
    void discovery0();
    const char * dev;
    uint8_t isOpen = FALSE;
    TCG_DiskInfo disk_info;
};