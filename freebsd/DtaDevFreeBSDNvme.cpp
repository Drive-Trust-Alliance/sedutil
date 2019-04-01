/* C:B**************************************************************************
This software is Copyright 2016-2018 Alexander Motin <mav@FreeBSD.org>

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
                         void * buffer, uint32_t bufferlen)
{
	struct nvme_pt_command	pt;
	int err;

	LOG(D1) << "Entering DtaDevFreeBSDNvme::sendCmd";

	memset(&pt, 0, sizeof(pt));
	if (IF_RECV == cmd) {
		LOG(D3) << "Security Receive Command";
#if __FreeBSD_version >= 1200058 && __FreeBSD_version < 1200081
		pt.cmd.opc_fuse = NVME_CMD_SET_OPC(NVME_OPC_SECURITY_RECEIVE);
#else
		pt.cmd.opc = NVME_OPC_SECURITY_RECEIVE;
#endif
	} else {
		LOG(D3) << "Security Send Command";
#if __FreeBSD_version >= 1200058 && __FreeBSD_version < 1200081
		pt.cmd.opc_fuse = NVME_CMD_SET_OPC(NVME_OPC_SECURITY_SEND);
#else
		pt.cmd.opc = NVME_OPC_SECURITY_SEND;
#endif
	}
	pt.cmd.cdw10 = protocol << 24 | comID << 8;
	pt.cmd.cdw11 = bufferlen;
	pt.len = bufferlen;
	pt.buf = buffer;

	err = ioctl(fd, NVME_PASSTHROUGH_CMD, &pt);
	if (err < 0) {
		LOG(D4) << "NVME_PASSTHROUGH_CMD failed";
		return (errno);
	} else if (nvme_completion_is_error(&pt.cpl)) {
		LOG(D4) << "NVME Security Command Error: " <<
		    std::hex << pt.cpl.status;
		return (0xff);
	} else
		LOG(D4) << "NVME Security Command Success";
	return (err);
}

void DtaDevFreeBSDNvme::identify(OPAL_DiskInfo& disk_info)
{
	struct nvme_pt_command	pt;
	struct nvme_controller_data cdata;

	LOG(D4) << "Entering DtaDevFreeBSDNvme::identify()";

	memset(&pt, 0, sizeof(pt));
#if __FreeBSD_version >= 1200058 && __FreeBSD_version < 1200081
	pt.cmd.opc_fuse = NVME_CMD_SET_OPC(NVME_OPC_IDENTIFY);
#else
	pt.cmd.opc = NVME_OPC_IDENTIFY;
#endif
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

	memcpy(disk_info.serialNum, cdata.sn, sizeof (disk_info.serialNum));
	memcpy(disk_info.firmwareRev, cdata.fr, sizeof(disk_info.firmwareRev));
	memcpy(disk_info.modelNum, cdata.mn, sizeof(disk_info.modelNum));

#if __FreeBSD_version >= 1200058
	if ((cdata.oacs >> NVME_CTRLR_DATA_OACS_SECURITY_SHIFT) &
	    NVME_CTRLR_DATA_OACS_SECURITY_MASK) {
#else
	if (cdata.oacs.security) {
#endif
		LOG(D4) << "Security Send/Receive are supported";
		disk_info.devType = DEVICE_TYPE_NVME;
	} else {
		LOG(D4) << "Security Send/Receive are not supported";
		disk_info.devType = DEVICE_TYPE_OTHER;
	}
}

/** Close the device reference so this object can be delete. */
DtaDevFreeBSDNvme::~DtaDevFreeBSDNvme()
{
	LOG(D1) << "Destroying DtaDevFreeBSDNvme";
	close(fd);
}
