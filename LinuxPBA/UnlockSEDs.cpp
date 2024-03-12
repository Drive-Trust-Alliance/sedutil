/* C:B**************************************************************************
This software is Copyright (c) 2014-2024 Bright Plaza Inc. <drivetrust@drivetrust.com>

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
#include "DtaDevOS.h"


using namespace std;

uint8_t UnlockSEDs(char * password) {
/* Loop through drives */

  for (string & device:DtaDevOSDrive::enumerateDtaDevOSDriveDevRefs()) {

    DtaDevOS * d=NULL;
    const char * devref = (const char *)device.c_str();
    if (DTAERROR_SUCCESS == DtaDevOS::getDtaDevOS(devref, &d, true)  &&  d!=NULL) {

        if ((!d->isOpal1()) && (!d->isOpal2())) {
            printf("Drive %-10s %-40s not OPAL  \n", devref, d->getModelNum());
            delete d;
            continue;
        }

        d->no_hash_passwords = false;
        bool failed = false;
        if (d->Locked()) {
            if (d->MBREnabled()) {
                if (d->setMBRDone(1, password)) {
                    failed = true;
                }
            }
            if (d->setLockingRange(0, OPAL_LOCKINGSTATE::READWRITE, password)) {
                failed = true;
            }
            if (failed)
              printf("Drive %-10s %-40s is OPAL Failed  \n", devref, d->getModelNum());
            else
              printf("Drive %-10s %-40s is OPAL Unlocked   \n", devref, d->getModelNum());

        }
        else {
            printf("Drive %-10s %-40s is OPAL NOT LOCKED   \n", devref, d->getModelNum());
        }
        delete d;

    }
  }

  return 0x00;
}
