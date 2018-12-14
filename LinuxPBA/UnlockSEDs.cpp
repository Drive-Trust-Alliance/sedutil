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

#include <dirent.h>
#include <fnmatch.h>
#include <algorithm>

using namespace std;

uint8_t UnlockSEDs(char * password) {
/* Loop through drives */
    char devref[25];
    OPALSTATUSCODE unlock_state = OPALSTATUSCODE::FAIL;
    DtaDev *tempDev;
    DtaDev *d;
    DIR *dir;
    struct dirent *dirent;
    vector<string> devices;
     string tempstring;
    LOG(D4) << "Enter UnlockSEDs";
    dir = opendir("/dev");
    const char* users[] = {"User1", "Admin1"};
    const uint8_t nb_users = 2;
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
        if ((!tempDev->isOpal1()) && (!tempDev->isOpal2())) {
            printf("Drive %-10s %-40s not OPAL  \n", devref, tempDev->getModelNum());

            delete tempDev;
            continue;
        }
        if (tempDev->isOpal2())
            d = new DtaDevOpal2(devref);
        else
            d = new DtaDevOpal1(devref);
        delete tempDev;
        d->no_hash_passwords = false;
        unlock_state = OPALSTATUSCODE::FAIL;
        if (d->Locked()) {
            uint8_t j;
            for (j = 0; unlock_state != OPALSTATUSCODE::SUCCESS && j < nb_users; j++) {
                unlock_state = (OPALSTATUSCODE) d->setLockingRange(0, OPAL_LOCKINGSTATE::READWRITE, users[j], password);
                if (unlock_state == OPALSTATUSCODE::SUCCESS && d->MBREnabled()) {
                    unlock_state = (OPALSTATUSCODE) d->setMBRDone(1, users[j], password);
                }
            }
            (unlock_state == OPALSTATUSCODE::SUCCESS) ? printf("Drive %-10s %-40s is OPAL Unlocked\n", devref, d->getModelNum()):
                printf("Drive %-10s %-40s is OPAL Failed  \n", devref, d->getModelNum()); 
            delete d;
        }
        else {
            printf("Drive %-10s %-40s is OPAL NOT LOCKED   \n", devref, d->getModelNum());
            delete d;
        }
    }
    return 0x00;
};
