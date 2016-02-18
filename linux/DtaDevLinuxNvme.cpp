/* C:B**************************************************************************
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

using namespace std;

/** The Device class represents a single disk device.
 *  Linux specific implementation using the NVMe interface
 */
DtaDevLinuxNvme::DtaDevLinuxNvme() {}

bool DtaDevLinuxNvme::init(const char * devref)
{
    LOG(D1) << "Creating DtaDevLinuxNvme::DtaDev() " << devref;
    ifstream kopts;
    bool isOpen = FALSE;

    if ((fd = open(devref, O_RDWR)) < 0) {
        isOpen = FALSE;
        // This is a D1 because diskscan looks for open fail to end scan
        LOG(D1) << "Error opening device " << devref << " " << (int32_t) fd;
        //        if (-EPERM == fd) {
        //            LOG(E) << "You do not have permission to access the raw disk in write mode";
        //            LOG(E) << "Perhaps you might try sudo to run as root";
        //        }
    }
    else {
        isOpen = TRUE;
    }
	return isOpen;
}

/** Send an ioctl to the device using nvme admin commands. */
uint8_t DtaDevLinuxNvme::sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                         void * buffer, uint16_t bufferlen)
{
    struct nvme_admin_cmd nvme_cmd;
	int err;

    LOG(D1) << "Entering DtaDevLinuxNvme::sendCmd";

	memset(&nvme_cmd, 0, sizeof(nvme_cmd));

	if (IF_RECV == cmd) {
		LOG(D3) << "Security Receive Command";
		nvme_cmd.opcode = nvme_admin_security_recv;
		nvme_cmd.cdw10 = protocol << 24 | comID << 8;
		nvme_cmd.cdw11 = bufferlen;
		nvme_cmd.data_len = bufferlen;
		nvme_cmd.addr = (__u64)buffer;
	}
	else {
		LOG(D3) << "Security Send Command";
		nvme_cmd.opcode = nvme_admin_security_send;
		nvme_cmd.cdw10 = protocol << 24 | comID << 8;
		nvme_cmd.cdw11 = bufferlen;
		nvme_cmd.data_len = bufferlen;
		nvme_cmd.addr = (__u64)buffer;
	}

	err = ioctl(fd, NVME_IOCTL_ADMIN_CMD, &nvme_cmd);
	if (err < 0)
		return errno;
	else if (err != 0) {
		fprintf(stderr, "NVME Security Command Error:%d\n", err);
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

void DtaDevLinuxNvme::identify(OPAL_DiskInfo& disk_info)
{
	LOG(D4) << "Entering DtaDevLinuxNvme::identify()";

	struct nvme_admin_cmd cmd;
	struct nvme_id_ctrl ctrl;
	int err;

	memset(&cmd, 0, sizeof(cmd));
	cmd.opcode = nvme_admin_identify;
	cmd.nsid = 0;
	cmd.addr = (unsigned long)&ctrl;
	cmd.data_len = 4096;
	cmd.cdw10 = 1;
	err = ioctl(fd, NVME_IOCTL_ADMIN_CMD, &cmd);

	if (err) {
		LOG(E) << "Identify error. NVMe status " << err;
		disk_info.devType = DEVICE_TYPE_OTHER;
		IFLOG(D4) DtaHexDump(&cmd, sizeof(cmd));
		IFLOG(D4) DtaHexDump(&ctrl, sizeof(ctrl));
		return;
	}

	disk_info.devType = DEVICE_TYPE_NVME;
	memcpy(disk_info.serialNum, ctrl.sn, sizeof (disk_info.serialNum));
	memcpy(disk_info.firmwareRev, ctrl.fr, sizeof(disk_info.firmwareRev));
	memcpy(disk_info.modelNum, ctrl.mn, sizeof(disk_info.modelNum));

    return;
}

/** Close the device reference so this object can be delete. */
DtaDevLinuxNvme::~DtaDevLinuxNvme()
{
    LOG(D1) << "Destroying DtaDevLinuxNvme";
    close(fd);
}
