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
#if defined(__APPLE__) && defined(__MACH__)
    /* Apple OSX and iOS (Darwin). ------------------------------ */
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR == 1
    /* iOS in Xcode simulator */

#elif TARGET_OS_IPHONE == 1
    /* iOS on iPhone, iPad, etc. */

#elif TARGET_OS_MAC == 1
    /* OSX */


#include <log/log.h>

#include "DtaHexDump.h"
#include "DtaStructures.h"
#include "DtaDevMacOSTPer.h"
#include "DtaConstants.h"

#include "DtaDevOS.h"
#include "DtaDevGeneric.h"
#include "DtaDevEnterprise.h"
#include "DtaDevOpal1.h"
#include "DtaDevOpal2.h"

using namespace std;



static DtaDevOS* getDtaDevOSSubclassInstance(DtaDevMacOSTPer * t,
                                             const char * devref,
                                             bool genericIfNotTPer) {
    if (t->isOpal2())     return new DtaDevOpal2(devref);
    if (t->isOpal1())     return new DtaDevOpal1(devref);
    if (t->isEprise())    return new DtaDevEnterprise(devref);
//  if (t->isRuby()) ...  etc.

    if (genericIfNotTPer) return new DtaDevGeneric(devref);

    LOG(E) << "Unknown OPAL SSC ";
    return NULL;
}

uint8_t DtaDevOS::getDtaDevOS(const char * devref, DtaDevOS * & device, bool genericIfNotTPer)
{
    DTA_DEVICE_INFO di;
    bzero(&di,sizeof(di));

    DtaDevMacOSBlockStorageDevice * bsdBlockStorageDevice =
        DtaDevMacOSBlockStorageDevice::getBlockStorageDevice(devref, &di);
    if (bsdBlockStorageDevice==NULL) {
        LOG(E) << "Unrecognized device '" << devref << "'";
        return DTAERROR_OBJECT_CREATE_FAILED;
    }

    if (!bsdBlockStorageDevice->isAnySSC()) {
        if (genericIfNotTPer) {
            device = new DtaDevGeneric(devref);
            return DTAERROR_SUCCESS;
        }
        LOG(E) << "Invalid or unsupported device " << devref;
        return DTAERROR_COMMAND_ERROR;
    }

    DtaDevMacOSTPer * t = dynamic_cast<DtaDevMacOSTPer *>(bsdBlockStorageDevice);
    if (NULL == t) {
        if (genericIfNotTPer) {
            device = new DtaDevGeneric(devref);
            return DTAERROR_SUCCESS;
        }
        LOG(E) << "Create DtaDevMacOSTPer object failed?!";
        return DTAERROR_OBJECT_CREATE_FAILED;
    }

    device = getDtaDevOSSubclassInstance(t, devref, genericIfNotTPer);

    if (NULL == device) {
        LOG(E) << "Create DtaDevOS object failed?!";
        return DTAERROR_OBJECT_CREATE_FAILED;
    }

    return DTAERROR_SUCCESS;
}

uint8_t DtaDev::getDtaDev(const char * devref, DtaDev * & device, bool genericIfNotTPer)
{
    DtaDevOS * d;
    uint8_t result = DtaDevOS::getDtaDevOS(devref, d, genericIfNotTPer);
    if (result == DTAERROR_SUCCESS) {
        device = static_cast<DtaDev *>(d);
    }
    return result;
}

#endif
#endif // defined(__APPLE__) && defined(__MACH__)
