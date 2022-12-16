/* C:B**************************************************************************
This software is Copyright 2014-2022 Bright Plaza Inc. <drivetrust@drivetrust.com>

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

#include <log/log.h>

#include "DtaHexDump.h"
#include "DtaStructures.h"
#include "DtaDevMacOSTPer.h"
#include "DtaConstants.h"

#include "DtaDevEnterprise.h"
#include "DtaDevOpal1.h"
#include "DtaDevOpal2.h"

using namespace std;


uint8_t DtaDev::getDtaDev(const char * devref, DtaDev * * pdev)
{
    DTA_DEVICE_INFO di;
    bzero(&di,sizeof(di));
    DtaDevMacOSBlockStorageDevice * blockStorageDevice =
        DtaDevMacOSBlockStorageDevice::getBlockStorageDevice(devref, &di);
    if (blockStorageDevice==NULL) {
        LOG(E) << "Create DtaDevMacOSBlockStorageDevice object for " << devref << " failed";
        return DTAERROR_OBJECT_CREATE_FAILED;
    }
    if (!blockStorageDevice->isAnySSC()) {
        LOG(E) << "Invalid or unsupported device " << devref;
        return DTAERROR_COMMAND_ERROR;
    }

    DtaDevMacOSTPer * t = dynamic_cast<DtaDevMacOSTPer *>(blockStorageDevice);
    if (NULL == t) {
       LOG(E) << "Create DtaDevMacOSTPer object failed";
       return DTAERROR_OBJECT_CREATE_FAILED;
    }

    if (t->isOpal2()) {
       * pdev = new DtaDevOpal2(devref);
    } else if (t->isOpal1()) {
       * pdev = new DtaDevOpal1(devref);
    } else if (t->isEprise()) {
       * pdev = new DtaDevEnterprise(devref);
    } else {
       LOG(E) << "Unknown OPAL SSC ";
       return DTAERROR_INVALID_COMMAND;
    }
    
    if (NULL == * pdev) {
       LOG(E) << "Create DtaDev object failed";
       return DTAERROR_OBJECT_CREATE_FAILED;
    }
    
    return DTAERROR_SUCCESS;
}

