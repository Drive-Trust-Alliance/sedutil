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
#include "os.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fnmatch.h>
#include <fcntl.h>
#include <sys/ioctl.h>
// #include <scsi/sg.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
// #include <linux/hdreg.h>
#include <errno.h>
#include <map>
#include <vector>
#include <fstream>
#include "DtaDevOS.h"
#include "DtaHexDump.h"
#include "DtaDevMacOS_UAS_SAT.h"
#include "DtaDevGeneric.h"
#include "DtaOptions.h"
#include "DtaStructures.h"
#include <IOKit/IOKitLib.h>
#include <IOKit/storage/IOBlockStorageDevice.h>
#include <IOKit/storage/IOBlockStorageDriver.h>
#include <IOKit/storage/IOMedia.h>
#include "DtaBlockStorageDevice.h"


using namespace std;

/** The Device class represents a MacOS generic storage device.
  * At initialization we determine if we map to the NVMe or SATA derived class
 */
unsigned long long DtaDevOS::getSize()
{ return 0;
}
DtaDevOS::DtaDevOS()
{
    dev = NULL;
	drive = NULL;
}

template<typename Callable>  // Let the compiler figure out the type of the lambda
void DtaDevOS::_init (Callable driveFn, const char * devref)
{
    LOG(D1) << "DtaDevOS::init " << devref;
    isOpen = FALSE;

    if (0 != strncmp(devref, "disk", 4)) {
        LOG(E) << "DtaDevOS::init ERROR - unknown drive type";
        return;
    }

    DtaDevMacOSDrive * driveUSB = driveFn();
    bool connected = driveUSB->init(devref,true);
    if (!connected) {
        delete driveUSB;
        return;
    }

    dev = strndup(devref, 25);
    drive = driveUSB;
    isOpen = TRUE;
    drive->identify(disk_info);
    discovery0();
}


/* Determine which type of drive we're using and instantiate a derived class of that type */
void DtaDevOS::init(const char * devref)
{
    memset(&disk_info, 0, sizeof(OPAL_DiskInfo));
    auto driveUSBfn = [=](){return (DtaDevMacOSDrive *)new DtaDevMacOS_UAS_SAT();};
    _init(driveUSBfn, devref);
}

/* Determine which type of drive we're using and instantiate a derived class of that type */
void DtaDevOS::init(const char * devref,
                    io_registry_entry_t driverService,
                    io_connect_t connect,
                    const OPAL_DiskInfo& diskInfo)  // TODO: see below, either make this sufficient or leave it out
{
    disk_info=diskInfo;
    auto driveUSBfn = [=](){return (DtaDevMacOSDrive *)new DtaDevMacOS_UAS_SAT(driverService, connect);};
    _init(driveUSBfn, devref);
}

uint8_t DtaDevOS::sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
	void * buffer, uint32_t bufferlen)
{
	if (!isOpen) return 0xfe; //disk open failed so this will too

	if (NULL == drive)
	{
		LOG(E) << "DtaDevOS::sendCmd ERROR - unknown drive type";
		return 0xff;
	}

	return drive->sendCmd(cmd, protocol, comID, buffer, (uint16_t)bufferlen);
}

void DtaDevOS::identify(OPAL_DiskInfo& disk_info)
{
	if (!isOpen)
        return; //disk open failed so this will too
	if (NULL == drive)
	{
		LOG(E) << "DtaDevOS::identify ERROR - unknown disk type";
		return;
	}

	drive->identify(disk_info);
}

uint8_t DtaDevOS::acquireDiscovery0Response(uint8_t * d0Response)
{
    if (!isOpen)
        return KERN_FAILURE; //disk open failed so this will too
    if (NULL == drive)
    {
        LOG(E) << "DtaDevOS::acquireDiscovery0Response ERROR - unknown disk type";
        return  KERN_FAILURE;
    }

    return drive->discovery0(d0Response);
}


void DtaDevOS::osmsSleep(uint32_t ms)
{
	usleep(ms * 1000); //convert to microseconds
}


int  DtaDevOS::diskScan()
{
    LOG(D1) << "Entering DtaDevOS:diskScan ";

    printf("Scanning for Opal compliant disks\n");

    vector<DtaBlockStorageDevice *> blockStorageDevices=DtaBlockStorageDevice::enumerateBlockStorageDevices();

    for (DtaBlockStorageDevice * device : blockStorageDevices)
    {
        char devname[25];
        snprintf(devname,25,"/dev/%s", device->bsdName.c_str());
        printf("%-10s", devname);

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

        printf("%40s %8s   %-7s\n",
               device->getModelNum(),
               device->getFirmwareRev(),
               devType);
    }

    LOG(D1) << "Exiting DtaDevOS::diskScan ";
	return 0;
}

/** Close the device reference so this object can be delete. */
DtaDevOS::~DtaDevOS()
{
    LOG(D1) << "Destroying DtaDevOS";

    if (NULL != dev) {
        std::free((void *)dev);
        dev = NULL;
    }

	if (NULL != drive) {
		delete drive;
        drive = NULL;
    }
}
