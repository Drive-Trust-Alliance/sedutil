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
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fnmatch.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/sysmacros.h>
#include <scsi/sg.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <linux/hdreg.h>
#include <linux/major.h>
#include <errno.h>
#include <vector>
#include <fstream>
#include "DtaDevOS.h"
#include "DtaHexDump.h"
#include "DtaDevLinuxSata.h"
#include "DtaDevLinuxNvme.h"
#include "DtaDevGeneric.h"

using namespace std;

/** The Device class represents a Linux generic storage device.
  * At initialization we determine if we map to the NVMe or SATA derived class
 */
unsigned long long DtaDevOS::getSize()
{ return 0;
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

	struct stat st;
	if (stat(devref, &st) < 0)
	{
		LOG(E) << "DtaDevOS::init ERROR - failed to stat " << devref;
		isOpen = FALSE;
		return;
	}
	switch (st.st_mode & S_IFMT)
	{
		case S_IFBLK: // block device
		{
			switch (major(st.st_rdev))
			{
				case SCSI_DISK0_MAJOR:
				case SCSI_DISK1_MAJOR:
				case SCSI_DISK2_MAJOR:
				case SCSI_DISK3_MAJOR:
				case SCSI_DISK4_MAJOR:
				case SCSI_DISK5_MAJOR:
				case SCSI_DISK6_MAJOR:
				case SCSI_DISK7_MAJOR:
				case SCSI_DISK8_MAJOR:
				case SCSI_DISK9_MAJOR:
				case SCSI_DISK10_MAJOR:
				case SCSI_DISK11_MAJOR:
				case SCSI_DISK12_MAJOR:
				case SCSI_DISK13_MAJOR:
				case SCSI_DISK14_MAJOR:
				case SCSI_DISK15_MAJOR:
				{
//					DtaDevLinuxSata *SataDrive = new DtaDevLinuxSata();
					drive = new DtaDevLinuxSata();
					goto init;
				}
			}
			break;
		}
		case S_IFCHR: // character device
		{
			char path[PATH_MAX];
			snprintf(path, sizeof path, "/sys/dev/char/%u:%u/device/driver", major(st.st_rdev), minor(st.st_rdev));
			ssize_t r;
			if ((r = readlink(path, path, sizeof path - 1)) < 0)
			{
				LOG(E) << "DtaDevOS::init ERROR - failed to readlink " << path;
				isOpen = FALSE;
				return;
			}
			path[r] = '\0'; // readlink does not append a null terminator
			if (strcmp(basename(path), "nvme") == 0)
			{
//				DtaDevLinuxNvme *NvmeDrive = new DtaDevLinuxNvme();
				drive = new DtaDevLinuxNvme();
				goto init;
			}
			break;
		}
	}
	LOG(E) << "DtaDevOS::init ERROR - unknown drive type";
	isOpen = FALSE;
	return;

init:
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
	void * buffer, uint32_t bufferlen)
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
int  DtaDevOS::diskScan()
{
    DIR *dir;
    struct dirent *dirent;
    DtaDev * d;
    char devname[25];
    vector<string> devices;
    string tempstring;
    
    LOG(D1) << "Entering DtaDevOS:diskScan ";
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
    printf("Scanning for Opal compliant disks\n");
    for(uint16_t i = 0; i < devices.size(); i++) {
                snprintf(devname,23,"/dev/%s",devices[i].c_str());
                printf("%-10s", devname);
                d = new DtaDevGeneric(devname);
		if (d->isAnySSC())
                    printf(" %s%s%s ", (d->isOpal1() ? "1" : " "),
			(d->isOpal2() ? "2" : " "), (d->isEprise() ? "E" : " "));
		else
                    printf("%s", " No  ");
                
                printf("%s %s\n",d->getModelNum(),d->getFirmwareRev());
                delete d;
          }
	printf("No more disks present ending scan\n");
        LOG(D1) << "Exiting DtaDevOS::scanDisk ";
	return 0;
}

/** Close the device reference so this object can be delete. */
DtaDevOS::~DtaDevOS()
{
    LOG(D1) << "Destroying DtaDevOS";
	if (NULL != drive)
		delete drive;
}
