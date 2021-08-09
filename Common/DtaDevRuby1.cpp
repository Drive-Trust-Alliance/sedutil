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

#include "DtaDevRuby1.h"

using namespace std;


DtaDevRuby1::DtaDevRuby1 (const char * devref)
{
	DtaDevOpal::init(devref);
	assert(isRuby1());
}

DtaDevRuby1::~DtaDevRuby1()
{
}
uint16_t DtaDevRuby1::comID() { return disk_info.Ruby10_basecomID; }
