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
#include <stdlib.h>
#include <stdio.h>
#include "TCGdev.h"
#include "diskList.h"

/** Brute force disk scan.
 * loops through the physical devices until
 * there is an open error. Creates a Device
 * and reports OPAL support.
 */

diskList::diskList()
{
    int i = 0;
    TCGdev * d;
    LOG(D4) << "Creating diskList";
    printf("\nScanning for Opal 2.0 compliant disks\n");
    while (TRUE) {
        SNPRINTF(devname, 23, DEVICEMASK, i);
        //		sprintf_s(devname, 23, "\\\\.\\PhysicalDrive3", i);
        d = new TCGdev(devname);
        if (d->isPresent()) {
            printf("%s %s", devname, (d->isOpal2() ? " Yes\n" : " No \n"));
            if (MAX_DISKS == i) {
                LOG(I) << MAX_DISKS << "% disks, really?";
                delete d;
                break;
            }
        }
        else break;

        delete d;
        i += 1;
    }
    delete d;
    printf("\n No more disks present ending scan\n");
}

diskList::~diskList()
{
    LOG(D4) << "Destroying  diskList";
}
