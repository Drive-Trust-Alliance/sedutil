/* C:B**************************************************************************
This software is Copyright 2014,2015 Michael Romeo <r0m30@r0m30.com>

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
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <scsi/sg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/hdreg.h>
#include <errno.h>
#include <vector>
#include <fstream>
#include "MsedDevOS.h"
#include "MsedHexDump.h"
#include "MsedDevLinuxSata.h"
#include "MsedDevLinuxNvme.h"

using namespace std;

/** The Device class represents a Linux generic storage device.
  * At initialization we determine if we map to the NVMe or SATA derived class
 */
MsedDevOS::MsedDevOS()
{
	drive = NULL;
}

/* Determine which type of drive we're using and instantiate a derived class of that type */
void MsedDevOS::init(const char * devref)
{
	LOG(D1) << "MsedDevOS::init " << devref;

	memset(&disk_info, 0, sizeof(OPAL_DiskInfo));
	dev = devref;

	if (!strncmp(devref, "/dev/nvme", 9))
	{
		MsedDevLinuxNvme *NvmeDrive = new MsedDevLinuxNvme();
		drive = NvmeDrive;
	}
	else if (!strncmp(devref, "/dev/s", 6))
	{
		MsedDevLinuxSata *SataDrive = new MsedDevLinuxSata();
		drive = SataDrive;
	}
	else
		LOG(E) << "MsedDevOS::init ERROR - unknown drive type";

	if (drive->init(devref))
	{
		isOpen = TRUE;
		drive->identify(&disk_info);
		if (!disk_info.devType)
			discovery0();
	}
	else
		isOpen = FALSE;

	return;
}

uint8_t MsedDevOS::sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
	void * buffer, uint16_t bufferlen)
{
	if (!isOpen) return 0xfe; //disk open failed so this will too

	if (NULL == drive)
	{
		LOG(E) << "MsedDevOS::init ERROR - unknown drive type";
		return 0xff;
	}
	
	return drive->sendCmd(cmd, protocol, comID, buffer, bufferlen);
}

void MsedDevOS::identify()
{
	if (!isOpen) return; //disk open failed so this will too
	if (NULL == drive)
	{
		LOG(E) << "MsedDevOS::identify ERROR - unknown disk type";
		return;
	}
	
	drive->identify(&disk_info);
}

void MsedDevOS::osmsSleep(uint32_t ms)
{
	usleep(ms * 1000); //convert to microseconds
    return;
}

/** Close the device reference so this object can be delete. */
MsedDevOS::~MsedDevOS()
{
    LOG(D1) << "Destroying MsedDev";
	if (NULL != drive)
		delete drive;
}