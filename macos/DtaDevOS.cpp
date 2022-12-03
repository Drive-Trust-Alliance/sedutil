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
#include "DtaHexDump.h"
#include "DtaDevGeneric.h"
#include "DtaStructures.h"
#include "DtaDevMacOSTPer.h"


using namespace std;

/** The Device class represents a MacOS generic storage device.
  * At initialization we determine if we map to the NVMe or SATA derived class
 */
const unsigned long long DtaDevOS::getSize() {
    return disk_info.devSize;
}


DtaDevOS::DtaDevOS()
{
    dev = NULL;
    tPer = NULL;
}


bool DtaDevOS::__init(const char *devref) {
    dev = strdup(devref);
    memset(&disk_info, 0, sizeof(DTA_DEVICE_INFO));
    DtaDevMacOSBlockStorageDevice * device = DtaDevMacOSBlockStorageDevice::getBlockStorageDevice(devref, &disk_info);
    tPer = dynamic_cast <DtaDevMacOSTPer *> (device);
    return (tPer != NULL);
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

uint8_t DtaDevOS::sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
	void * buffer, uint32_t bufferlen)
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

    (void)(tPer -> identify(disk_info));
}

void DtaDevOS::osmsSleep(uint32_t ms)
{
	usleep(ms * 1000); //convert to microseconds
}


int  DtaDevOS::diskScan()
{
    LOG(D1) << "Entering DtaDevOS:diskScan ";

    printf("Scanning for Opal compliant disks\n");

    vector<DtaDevMacOSBlockStorageDevice *> blockStorageDevices=DtaDevMacOSBlockStorageDevice::enumerateBlockStorageDevices();

    for (DtaDevMacOSBlockStorageDevice * device : blockStorageDevices){
        printf("%-11s", device->getDevName().c_str());

        if (device->isAnySSC()) {
            printf(" %s%s%s ",
                   (device->isOpal1()  ? "1" : " "),
                   (device->isOpal2()  ? "2" : " "),
                   (device->isEprise() ? "E" : " "));
        } else {
            printf(" No  ");
        }
        const char * devType;
        switch (device->getDevType()) {
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
        printf("%-30s %-8s  %-7s  %-8s %-30s\n",
               device->getModelNum(),
               device->getFirmwareRev(),
               devType,
               device->getVendorID(),
               device->getManufacturerName());
#else // DEBUG
        printf("%-30s %-8s  %-7s\n",
               device->getModelNum(),
               device->getFirmwareRev(),
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
