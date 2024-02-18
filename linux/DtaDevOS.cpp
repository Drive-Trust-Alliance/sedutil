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
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fnmatch.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <scsi/sg.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <linux/hdreg.h>
#include <errno.h>
#include <vector>
#include <fstream>
#include "DtaDevOS.h"
#include "DtaHexDump.h"
#include "DtaDevLinuxSata.h"
#include "DtaDevLinuxNvme.h"
#include "DtaDevGeneric.h"
#include "DtaDevEnterprise.h"
#include "DtaDevOpal1.h"
#include "DtaDevOpal2.h"

using namespace std;




/** Factory method to produce instance of appropriate subclass
 *   Note that all of DtaDevGeneric, DtaDevEnterprise, DtaDevOpal, ... derive from DtaDevOS
 * @param devref             name of the device in the OS lexicon
 * @param dev                reference into which to store the address of the new instance
 * @param genericIfNotTPer   if true, store an instance of DtaDevGeneric for non-TPers;
 *                           if false, store NULL for non-TPers
 */
// static
uint8_t DtaDevOS::getDtaDevOS(const char * devref,
                              DtaDevOS * & dev, bool genericIfNotTPer)
{
  LOG(D1) << "DtaDevOS::getDtaDevOS(devref=\"" << devref << "\")";
  DTA_DEVICE_INFO disk_info;
  bzero(&disk_info, sizeof(disk_info));

  DtaDevLinuxDrive * drive = DtaDevLinuxDrive::getDtaDevLinuxDrive(devref, &disk_info);
  if (drive == NULL) {
    dev = NULL;
    LOG(E) << "Invalid or unsupported device " << devref;
    LOG(D1) << "DtaDevOS::getDtaDevOS(devref=\"" << devref << "\") returning DTAERROR_COMMAND_ERROR";
    return DTAERROR_COMMAND_ERROR;
  }

  dev =  getDtaDevOS(devref, drive, disk_info, genericIfNotTPer) ;
  if (dev == NULL) {
    delete drive;
    LOG(E) << "Invalid or unsupported device " << devref;
    LOG(D1) << "DtaDevOS::getDtaDevOS(devref=\"" << devref << "\") returning DTAERROR_COMMAND_ERROR";
    return DTAERROR_COMMAND_ERROR;
  }


  LOG(D1) << "DtaDevOS::getDtaDevOS(devref=\"" << devref << "\") disk_info:";
  DtaHexDump(&disk_info, (int)sizeof(disk_info));
  LOG(D1) << "DtaDevOS::getDtaDevOS(devref=\"" << devref << "\") returning DTAERROR_SUCCESS";
  return DTAERROR_SUCCESS;
}


/** The Device class represents a Linux generic storage device.
 * At instantiation we determine if we create an instance of the NVMe or SATA or Scsi (SAS) derived class
 */

const unsigned long long DtaDevOS::getSize()
{ return 0;
}

uint8_t DtaDevOS::sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                          void * buffer, size_t bufferlen)
{
  if (!isOpen) return 0xfe; //disk open failed so this will too

  if (NULL == drive)
    {
      LOG(E) << "DtaDevOS::sendCmd ERROR - unknown drive type";
      return 0xff;
    }

  return drive->sendCmd(cmd, protocol, comID, buffer, bufferlen);
}

bool DtaDevOS::identify(DTA_DEVICE_INFO& disk_info)
{
  return drive->identify(disk_info)
    &&   DTAERROR_SUCCESS == drive->discovery0(disk_info);
}

void DtaDevOS::osmsSleep(uint32_t ms)
{
  usleep(ms * 1000); //convert to microseconds
  return;
}


int  DtaDevOS::diskScan()
{
  LOG(D1) << "Entering DtaDevOS:diskScan ";

  DIR *dir = opendir("/dev");
  if (dir==NULL) {
    LOG(E) < "Can't read /dev ?!";
    return 0xff;
  }

  vector<string> devices;

  struct dirent *dirent;
  while (dirent=readdir(dir))
    devices.push_back(string(dirent->d_name));
  closedir(dir);


  std::sort(devices.begin(),devices.end());


  printf("Scanning for Opal compliant disks\n");
  for (string & device:devices) {

    char devname[256];
    snprintf(devname,sizeof(devname),"/dev/%s",device.c_str());

    DtaDevOS * d;
    if (DTAERROR_SUCCESS == getDtaDevOS(devname,d,true)) {

      printf("%-10s", devname);
      if (d->isAnySSC()) {
        printf(" %s%s%s ",
               (d->isOpal1()  ? "1" : " "),
               (d->isOpal2()  ? "2" : " "),
               (d->isEprise() ? "E" : " "));
      } else {
        printf("%s", " No  ");
      }
      printf("%s %s\n",d->getModelNum(),d->getFirmwareRev());

      delete d;
    }

  }

  printf("No more disks present -- ending scan\n");
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
