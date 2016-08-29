/* C:B**************************************************************************
This software is Copyright 2016 Alexander Motin <mav@FreeBSD.org>
This software is Copyright 2014-2016 Bright Plaza Inc. <drivetrust@drivetrust.com>

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
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <vector>
#include <fstream>
#include "DtaDevOS.h"
#include "DtaHexDump.h"
#include "DtaDevFreeBSDNvme.h"
#include "DtaDevFreeBSDSata.h"

using namespace std;

/** The Device class represents a FreeBSD generic storage device.
  * At initialization we determine if we map to the NVMe or SATA derived class
 */
unsigned long long DtaDevOS::getSize()
{
	return 0;
}

DtaDevOS::DtaDevOS()
{
	drive = NULL;
}

/* Determine which type of drive we're using and instantiate a derived class of that type */
void DtaDevOS::init(const char * devref)
{
	LOG(D1) << "DtaDevOS::init " << devref;

	memset(&disk_info, 0, sizeof(OPAL_DiskInfo));
	dev = devref;

	if (!strncmp(devref, "/dev/nvme", 9) || !strncmp(devref, "/dev/nvd", 8))
	{
		drive = new DtaDevFreeBSDNvme();
	}
	else 
	if (!strncmp(devref, "/dev/da", 7) ||
	    !strncmp(devref, "/dev/ada", 8))
	{
		drive = new DtaDevFreeBSDSata();
	}
	else {
		LOG(E) << "DtaDevOS::init ERROR - unknown drive type";
		return;
	}

	if (drive->init(devref))
	{
		isOpen = TRUE;
		drive->identify(disk_info);
		if (disk_info.devType != DEVICE_TYPE_OTHER)
			discovery0();
	}
	else
		isOpen = FALSE;

	return;
}

uint8_t DtaDevOS::sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
	void * buffer, uint16_t bufferlen)
{
	if (!isOpen) return 0xfe; //disk open failed so this will too

	if (NULL == drive)
	{
		LOG(E) << "DtaDevOS::sendCmd ERROR - unknown drive type";
		return 0xff;
	}

	return drive->sendCmd(cmd, protocol, comID, buffer, bufferlen);
}

void DtaDevOS::identify(OPAL_DiskInfo& disk_info)
{
	if (!isOpen) return; //disk open failed so this will too
	if (NULL == drive)
	{
		LOG(E) << "DtaDevOS::identify ERROR - unknown disk type";
		return;
	}

	drive->identify(disk_info);
}

void DtaDevOS::osmsSleep(uint32_t ms)
{
	usleep(ms * 1000); //convert to microseconds
	return;
}

/** Close the device reference so this object can be delete. */
DtaDevOS::~DtaDevOS()
{
	LOG(D1) << "Destroying DtaDevOS";
	if (NULL != drive)
		delete drive;
}
