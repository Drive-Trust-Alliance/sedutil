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
#include <stdlib.h>

#include "DtaHexDump.h"
#include "DtaDevGeneric.h"
#include "DtaStructures.h"
#include "DtaDevMacOSTPer.h"
#include "DtaConstants.h"

#include "DtaDevEnterprise.h"
#include "DtaDevOpal1.h"
#include "DtaDevOpal2.h"

using namespace std;

/** The Device class represents a MacOS generic storage device.
  * At initialization we determine if we map to the NVMe or SATA derived class
 */


DtaDevOS::DtaDevOS()
{
    dev = NULL;
    blockStorageDevice = NULL;
    tPer = NULL;
}


bool DtaDevOS::__init(const char *devref) {
    dev = const_cast<const char *>(strdup(devref));
    memset(&disk_info, 0, sizeof(DTA_DEVICE_INFO));
    blockStorageDevice = DtaDevMacOSBlockStorageDevice::getBlockStorageDevice(devref, &disk_info);
    tPer = dynamic_cast <DtaDevMacOSTPer *> (blockStorageDevice);
    return (tPer != NULL);
}

bool DtaDevOS::__init(const char * devref, DTA_DEVICE_INFO & di) {  // TODO: ???
    memcpy(&disk_info, &di, sizeof(DTA_DEVICE_INFO));
    return __init(devref);
}

/* Determine which type of drive we're using and instantiate a derived class of that type */
void DtaDevOS::init(const char * devref)
{
    if (__init(devref)) {
        isOpen = tPer->init(devref, true);
    };
}


/* Determine which type of drive we're using and instantiate a derived class of that type */
void DtaDevOS::init(const char * devref,
                    io_registry_entry_t driverService,
                    io_connect_t connect)
{
    if (__init(devref)) {
        tPer->init(driverService, connect);
        isOpen = true;
    };
}


/** OS specific method to initialize an object to a pre-existing connection
 *  @param di  reference to already-initialized DTA_DEVICE_INFO
 */
void DtaDevOS::init(const char * devref, DTA_DEVICE_INFO &di) {
    if (__init(devref, di)) {
        isOpen = tPer->init(devref, true);
    };
}


uint8_t DtaDevOS::sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
	void * buffer, size_t bufferlen)
{
	if (!isOpen) return 0xfe; //disk open failed so this will too

	if (NULL == tPer)
	{
		LOG(E) << "DtaDevOS::sendCmd ERROR - unknown drive type";
		return 0xff;
	}

	return tPer->sendCmd(cmd, protocol, comID, buffer, (uint16_t)bufferlen);
}


void DtaDevOS::identify()
{
    if (!isOpen) return; //disk open failed so this will too

    if (NULL == tPer)
    {
        LOG(E) << "DtaDevOS::identify ERROR - unknown drive type";
        return;
    }
    kern_return_t ret = tPer -> identify(disk_info);
    if (kIOReturnSuccess != ret) {
        LOG(E) << "DtaDevOS::identify ERROR - " << HEXON(8) << ret;
        return;
    }
}

void DtaDevOS::osmsSleep(uint32_t ms)
{
	usleep(ms * 1000); //convert to microseconds
}


int  DtaDevOS::diskScan()
{
    LOG(D1) << "Entering DtaDevOS:diskScan ";

#if !DEBUG
    printf("Scanning for Opal compliant disks\n");
#endif // DEBUG

    vector<DtaDevMacOSBlockStorageDevice *> blockStorageDevices =
        DtaDevMacOSBlockStorageDevice::enumerateBlockStorageDevices();

#if DEBUG
    if (blockStorageDevices.size()!=0) {
        printf(" device    SSC         Model Number       Firmware  Locn     World Wide Name    Serial Number       Vendor      Manufacturer Name\n");
        printf("---------- ---  ------------ ------------ --------  -----    ----- ---- -----  ------- ---------    -------  --------------- -------\n");
    }
#endif // DEBUG

    for (DtaDevMacOSBlockStorageDevice * blockStorageDevice : blockStorageDevices){
        printf("%-11s", blockStorageDevice->getDevPath().c_str());

        if (blockStorageDevice->isAnySSC()) {
            DtaDevMacOSTPer * t = dynamic_cast<DtaDevMacOSTPer *>(blockStorageDevice);
            printf(" %s%s%s ",
                   (t->isOpal1()  ? "1" : " "),
                   (t->isOpal2()  ? "2" : " "),
                   (t->isEprise() ? "E" : " "));
        } else {
            printf(" No  ");
        }
        const char * devType = NULL;
        switch (blockStorageDevice->getDevType()) {
            case DEVICE_TYPE_ATA:
                devType = "ATA";
                break;
            case DEVICE_TYPE_SAS:
                devType = "SAS";
                break;
            case DEVICE_TYPE_NVME:
                devType = "NVME";
                break;
            case DEVICE_TYPE_USB:
                devType = "USB";
                break;
            case DEVICE_TYPE_OTHER:
                devType = "OTHER";
                break;
            default:
                devType = "UNKNOWN";
        }
#if DEBUG
        char WWN[17]="                ";  // 16 blanks as placeholder if missing
        vector<uint8_t>wwn(blockStorageDevice->getWorldWideName());
        if (__is_not_all_NULs(wwn.data(), wwn.size())) {
            snprintf(WWN, 17, "%02X%02X%02X%02X%02X%02X%02X%02X",
                     wwn[0], wwn[1], wwn[2], wwn[3], wwn[4], wwn[5], wwn[6], wwn[7]);
        }
        printf("%-25s %-8s  %-7s  %16s  %-20s %-8s %-30s\n",
               blockStorageDevice->getModelNum(),
               blockStorageDevice->getFirmwareRev(),
               devType,
               WWN,
               blockStorageDevice->getSerialNum(),
               blockStorageDevice->getVendorID(),
               blockStorageDevice->getManufacturerName());
#else // DEBUG
        printf("%-25s %-8s  %-7s\n",
               blockStorageDevice->getModelNum(),
               blockStorageDevice->getFirmwareRev(),
               devType);
#endif // DEBUG
    }

    LOG(D1) << "Exiting DtaDevOS::diskScan ";
	return 0;
}

/** Close the device reference so this object can be delete. */
DtaDevOS::~DtaDevOS()
{
    LOG(D1) << "Destroying DtaDevOS";

    if (NULL != dev) {
        free((void *)dev);
    }
    if (NULL != tPer) {
        delete tPer;
    }
    
    dev = NULL;
    tPer = NULL;
}
