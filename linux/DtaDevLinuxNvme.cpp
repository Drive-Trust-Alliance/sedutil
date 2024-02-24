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
#include "DtaDevLinuxNvme.h"
#include "DtaHexDump.h"
#include <fnmatch.h>

#define  NVME_SECURITY_SEND 0x81
#define  NVME_SECURITY_RECV 0x82
#define  NVME_IDENTIFY 0x06

using namespace std;

/** The Device class represents a single disk device.
 *  Linux specific implementation using the NVMe interface
 */

DtaDevLinuxNvme * DtaDevLinuxNvme::getDtaDevLinuxNvme(const char * devref,
                                                      DTA_DEVICE_INFO & disk_info) {
  if (!(0 == fnmatch("/dev/nvme[0-9]",      devref, 0) ||
        0 == fnmatch("/dev/nvme[1-9][0-9]", devref, 0) ))
    return NULL;

  int fd_=fdopen(devref);
  if (fd_ < 0) {
    // This is a D1 because diskscan looks for open fail to end scan
    LOG(D1) << "Error opening device " << devref << " " << (int32_t) fd_;
    //        if (-EPERM == fd) {
    //            LOG(E) << "You do not have permission to access the raw disk in write mode";
    //            LOG(E) << "Perhaps you might try sudo to run as root";
    //        }
    return NULL;
  }


  LOG(D4) << "Success opening device " << devref << " as file handle " << (int32_t) fd_;


  DtaDevLinuxNvme * drive = new DtaDevLinuxNvme((int)fd_);

  if (!drive->identify(disk_info)) {
    disk_info.devType = DEVICE_TYPE_OTHER;
    LOG(E) << " Device "<< devref <<" is NOT Nvme?! -- file handle " << (int32_t) fd_;
    delete drive; // => close(fd)
    drive = NULL ;
  } else {
    disk_info.devType = DEVICE_TYPE_NVME;
    LOG(D4) << " Device "<< devref <<" is Nvme" ;
  }
  return drive;
}

/** Send an ioctl to the device using nvme admin commands. */
uint8_t DtaDevLinuxNvme::sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                                 void * buffer, uint32_t bufferlen)
{
  LOG(D1) << "Entering DtaDevLinuxNvme::sendCmd";

  struct nvme_admin_cmd nvme_cmd;
  memset(&nvme_cmd, 0, sizeof(nvme_cmd));

  if (IF_RECV == cmd) {
    LOG(D3) << "Nvme Security Receive Command";
    nvme_cmd.opcode = NVME_SECURITY_RECV;
  }
  else {
    LOG(D3) << "Nvme Security Send Command";
    nvme_cmd.opcode = NVME_SECURITY_SEND;
  }

  nvme_cmd.cdw10 = protocol << 24 | comID << 8;
  nvme_cmd.cdw11 = bufferlen;
  nvme_cmd.data_len = bufferlen;
  nvme_cmd.addr = (__u64)buffer;

  int err = ioctl(fd, NVME_IOCTL_ADMIN_CMD, &nvme_cmd);

  if (err < 0)
    return errno;
  else if (err != 0) {
    //fprintf(stderr, "NVME Security Command Error:%d\n", err);
    LOG(D3) << "NVME Security Command Error: " << err ;
    IFLOG(D4) DtaHexDump(&nvme_cmd, sizeof(nvme_cmd));
  }
  else
    LOG(D3) << "NVME Security Command Success:" << nvme_cmd.result;
  //LOG(D4) << "NVMe command:";
  //IFLOG(D4) DtaHexDump(&nvme_cmd, sizeof(nvme_cmd));
  //LOG(D4) << "NVMe buffer @ " << buffer;
  //IFLOG(D4) DtaHexDump(buffer, bufferlen);
  return err;
}

bool DtaDevLinuxNvme::identifyUsingNvmeIdentify(int fd,
                                                InterfaceDeviceID &,  // Currently unused
                                                DTA_DEVICE_INFO & disk_info)
{
  LOG(D4) << "Entering DtaDevLinuxNvme::identifyUsingNvmeIdentify";

  uint8_t ctrl[4096];

  struct nvme_admin_cmd cmd;
  memset(&cmd, 0, sizeof(cmd));

  LOG(D3) << "Nvme Identify Command";
  cmd.opcode = NVME_IDENTIFY;
  cmd.nsid = 0;
  cmd.addr = (unsigned long)&ctrl;
  cmd.data_len = 4096;
  cmd.cdw10 = 1;

  int err = ioctl(fd, NVME_IOCTL_ADMIN_CMD, &cmd);

  if (err) {
    disk_info.devType = DEVICE_TYPE_OTHER;
    LOG(D4) << "Nvme Identify error. NVMe status " << err;
    IFLOG(D4) DtaHexDump(&cmd, sizeof(cmd));
    IFLOG(D4) DtaHexDump(&ctrl, sizeof(ctrl));
    return false;
  }

  disk_info.devType = DEVICE_TYPE_NVME;
  uint8_t *results = ctrl;
  results += 4;
  safecopy(disk_info.serialNum, sizeof(disk_info.serialNum), results, sizeof(disk_info.serialNum));
  results += sizeof(disk_info.serialNum);
  safecopy(disk_info.modelNum, sizeof(disk_info.modelNum), results, sizeof(disk_info.modelNum));
  results += sizeof(disk_info.modelNum);
  safecopy(disk_info.firmwareRev, sizeof(disk_info.firmwareRev), results, sizeof(disk_info.firmwareRev));

  return true;
}
