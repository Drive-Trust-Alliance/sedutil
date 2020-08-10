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
#include "os.h"
#include "UnlockSEDs.h"
#include "DtaDevGeneric.h"
#include "DtaDevOpal1.h"
#include "DtaDevOpal2.h"
#include "DtaDevOpalite.h"
#include "DtaDevPyrite1.h"
#include "DtaDevPyrite2.h"
#include "DtaDevRuby1.h"

#include <dirent.h>
#include <fnmatch.h>
#include <algorithm>

using namespace std;

uint8_t UnlockSEDs(char * password) {
/* Loop through drives */
    char devref[25];
    int failed = 0;
    DtaDev *tempDev;
    DtaDev *d;
    DIR *dir;
    struct dirent *dirent;
    vector<string> devices;
     string tempstring;
    LOG(D4) << "Enter UnlockSEDs";
    dir = opendir("/dev");
    if(dir!=NULL)
    {
        while((dirent=readdir(dir))!=NULL) {
            if((!fnmatch("sd[a-z]",dirent->d_name,0)) || 
                    (!fnmatch("nvme[0-9]",dirent->d_name,0)) ||
                    (!fnmatch("nvme[0-9][0-9]",dirent->d_name,0))
                    ) {
                tempstring = dirent->d_name;
                devices.push_back(tempstring);
            }
        }
        closedir(dir);
    }
    std::sort(devices.begin(),devices.end());
    printf("\nScanning....\n");
    for(uint16_t i = 0; i < devices.size(); i++) {
                snprintf(devref,23,"/dev/%s",devices[i].c_str());
        tempDev = new DtaDevGeneric(devref);
        if (!tempDev->isPresent()) {
            break;
        }
        if ((!tempDev->isOpal1()) && (!tempDev->isOpal2()) &&
            (!tempDev->isOpalite()) && (!tempDev->isPyrite1()) &&
            (!tempDev->isPyrite2()) && (!tempDev->isRuby1())) {
            printf("Drive %-10s %-40s not OPAL  \n", devref, tempDev->getModelNum());

            delete tempDev;
            continue;
        }
        if (tempDev->isRuby1())
            d = new DtaDevRuby1(devref);
        else if (tempDev->isOpal2())
            d = new DtaDevOpal2(devref);
        else if (tempDev->isOpalite())
            d = new DtaDevOpalite(devref);
        else if (tempDev->isPyrite2())
            d = new DtaDevPyrite2(devref);
        else if (tempDev->isPyrite1())
            d = new DtaDevPyrite1(devref);
        else
            d = new DtaDevOpal1(devref);
        delete tempDev;
        d->no_hash_passwords = false;
        failed = 0;
        if (d->Locked()) {
            if (d->MBREnabled()) {
                if (d->setMBRDone(1, password)) {
                    failed = 1;
                }
            }
            if (d->setLockingRange(0, OPAL_LOCKINGSTATE::READWRITE, password)) {
                failed = 1;
            }
            failed ? printf("Drive %-10s %-40s is OPAL Failed  \n", devref, d->getModelNum()) :
                    printf("Drive %-10s %-40s is OPAL Unlocked   \n", devref, d->getModelNum());
            delete d;
        }
        else {
            printf("Drive %-10s %-40s is OPAL NOT LOCKED   \n", devref, d->getModelNum());
            delete d;
        }

    }
    return 0x00;
};
