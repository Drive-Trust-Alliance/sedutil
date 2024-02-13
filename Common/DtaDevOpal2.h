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
#pragma once
#include "os.h"
#include "DtaDevOpal.h"

using namespace std;
/** Class representing a disk device, this class is represents a disk that conforms
* to the OPAL 2.0 SSC
*
* testing so far indicates that the functions implemented in this program
* function the same in OPAL 1.0 and Opal 2.0
*/
class DtaDevOpal2 : public DtaDevOpal {
public:
    DtaDevOpal2(const char * devref);

#if defined(__APPLE__) && defined(__MACH__)
    /* Apple OSX and iOS (Darwin). ------------------------------ */
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR == 1
    /* iOS in Xcode simulator */

#elif TARGET_OS_IPHONE == 1
    /* iOS on iPhone, iPad, etc. */

#elif TARGET_OS_MAC == 1
    /* OSX */

    DtaDevOpal2(const char * devref,
              io_registry_entry_t driverService,
              io_connect_t connect);

#endif
#endif  // defined(__APPLE__) && defined(__MACH__)
	~DtaDevOpal2();
        /** return the communication ID to be used with this device */
	uint16_t comID();

  // /** Short-circuit routine re-uses initialized drive and disk_info */
  DtaDevOpal2(const char * devref, DtaDevLinuxDrive * drive, DTA_DEVICE_INFO& di)
    : DtaDevOpal(devref, drive, di)
  {} ;

};
