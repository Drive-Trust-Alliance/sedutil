/* C:B**************************************************************************
This software is Copyright 2016 Alexander Motin <mav@FreeBSD.org>

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
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fstream>
#include "DtaDevFreeBSDNvme.h"
#include "DtaHexDump.h"

using namespace std;

/** The Device class represents a single disk device.
 *  FreeBSD specific implementation using the NVMe interface
 */
DtaDevFreeBSDNvme::DtaDevFreeBSDNvme() {}

bool DtaDevFreeBSDNvme::init(const char * devref)
{
	LOG(D1) << "Creating DtaDevFreeBSDNvme::DtaDev() " << devref;

	if ((fd = open(devref, O_RDWR)) < 0) {
		// This is a D1 because diskscan looks for open fail to end scan
		LOG(D1) << "Error opening device " << devref << " " << errno;
		return (FALSE);
	}
	return (TRUE);
}

/** Send an ioctl to the device using nvme admin commands. */
uint8_t DtaDevFreeBSDNvme::sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                         void * buffer, uint16_t bufferlen)
{
	struct nvme_pt_command	pt;
	int err;

	LOG(D1) << "Entering DtaDevFreeBSDNvme::sendCmd";

	memset(&pt, 0, sizeof(pt));
	if (IF_RECV == cmd) {
		LOG(D3) << "Security Receive Command";
		pt.cmd.opc = NVME_OPC_SECURITY_RECEIVE;
	} else {
		LOG(D3) << "Security Send Command";
		pt.cmd.opc = NVME_OPC_SECURITY_SEND;
	}
	pt.cmd.cdw10 = protocol << 24 | comID << 8;
	pt.cmd.cdw11 = bufferlen;
	pt.len = bufferlen;
	pt.buf = buffer;

	err = ioctl(fd, NVME_PASSTHROUGH_CMD, &pt);
	if (err < 0)
		return (errno);
	else if (err != 0) {
		fprintf(stderr, "NVME Security Command Error:%d\n", err);
	} else
		LOG(D3) << "NVME Security Command Success";
	return (err);
}

void DtaDevFreeBSDNvme::identify(OPAL_DiskInfo& disk_info)
{
	struct nvme_pt_command	pt;
	struct nvme_controller_data cdata;

	memset(&pt, 0, sizeof(pt));
	pt.cmd.opc = NVME_OPC_IDENTIFY;
	pt.cmd.cdw10 = 1;
	pt.buf = &cdata;
	pt.len = sizeof(cdata);
	pt.is_read = 1;

	if (ioctl(fd, NVME_PASSTHROUGH_CMD, &pt) < 0) {
		LOG(E) << "Identify error " << errno;
		disk_info.devType = DEVICE_TYPE_OTHER;
		return;
	}

	if (nvme_completion_is_error(&pt.cpl)) {
		LOG(E) << "identify request returned error";
		disk_info.devType = DEVICE_TYPE_OTHER;
		return;
	}

	disk_info.devType = DEVICE_TYPE_NVME;
	memcpy(disk_info.serialNum, cdata.sn, sizeof (disk_info.serialNum));
	memcpy(disk_info.firmwareRev, cdata.fr, sizeof(disk_info.firmwareRev));
	memcpy(disk_info.modelNum, cdata.mn, sizeof(disk_info.modelNum));
}

/** Close the device reference so this object can be delete. */
DtaDevFreeBSDNvme::~DtaDevFreeBSDNvme()
{
	LOG(D1) << "Destroying DtaDevFreeBSDNvme";
	close(fd);
}
