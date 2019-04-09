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
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fstream>
#include <camlib.h>
#include <cam/scsi/scsi_message.h>
#include <cam/scsi/scsi_pass.h>
#include "DtaDevFreeBSDCAM.h"
#include "DtaHexDump.h"

using namespace std;

/** The Device class represents a single disk device.
 *  FreeBSD specific implementation using the CAM pass interface
 */

DtaDevFreeBSDCAM::DtaDevFreeBSDCAM()
{
	isSCSI = 0;
	isNVMe = 0;
}

bool DtaDevFreeBSDCAM::init(const char * devref)
{
	LOG(D1) << "Creating DtaDevFreeBSDCAM::DtaDev() " << devref;

	if ((camdev = cam_open_device(devref, O_RDWR)) == NULL) {
		// This is a D1 because diskscan looks for open fail to end scan
		LOG(D1) << "Error opening device " << devref;
		return (FALSE);
	}
	return (TRUE);
}

/** Send an ioctl to the device using pass through. */
uint8_t DtaDevFreeBSDCAM::sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                         void * buffer, uint32_t bufferlen)
{
	union ccb ccb;

	LOG(D1) << "Entering DtaDevFreeBSDCAM::sendCmd";
	bzero(&ccb, sizeof(ccb));
	if(isSCSI) {
		cam_fill_csio(&ccb.csio, 1, NULL,
		    (cmd == IF_RECV) ? CAM_DIR_IN : CAM_DIR_OUT,
		    MSG_SIMPLE_Q_TAG, (u_int8_t*)buffer, bufferlen,
		    SSD_FULL_SIZE, 12, 60 * 1000);

		ccb.csio.cdb_io.cdb_bytes[0] = (cmd == IF_RECV) ? 0xa2 : 0xb5;
		ccb.csio.cdb_io.cdb_bytes[1] = protocol;
		ccb.csio.cdb_io.cdb_bytes[2] = comID >> 8;
		ccb.csio.cdb_io.cdb_bytes[3] = comID;
		ccb.csio.cdb_io.cdb_bytes[4] = 0x80;
		ccb.csio.cdb_io.cdb_bytes[6] = (bufferlen/512) >> 24;
		ccb.csio.cdb_io.cdb_bytes[7] = (bufferlen/512) >> 16;
		ccb.csio.cdb_io.cdb_bytes[8] = (bufferlen/512) >> 8;
		ccb.csio.cdb_io.cdb_bytes[9] = (bufferlen/512);
	} else if(isNVMe) {
		cam_fill_nvmeadmin(&ccb.nvmeio, 0, NULL,
		    (cmd == IF_RECV) ? CAM_DIR_IN : CAM_DIR_OUT,
		    (u_int8_t*)buffer, bufferlen, 60 * 1000);
		if (IF_RECV == cmd) {
			LOG(D3) << "Security Receive Command";
			ccb.nvmeio.cmd.opc = NVME_OPC_SECURITY_RECEIVE;
		} else {
			LOG(D3) << "Security Send Command";
			ccb.nvmeio.cmd.opc = NVME_OPC_SECURITY_SEND;
		}
		ccb.nvmeio.cmd.cdw10 = htole32(protocol << 24 | comID << 8);
		ccb.nvmeio.cmd.cdw11 = htole32(bufferlen);
	} else {
		cam_fill_ataio(&ccb.ataio, 0, NULL,
		    (cmd == IF_RECV) ? CAM_DIR_IN : CAM_DIR_OUT,
		    MSG_SIMPLE_Q_TAG, (u_int8_t*)buffer, bufferlen, 60 * 1000);

		ccb.ataio.cmd.flags = 0;
		ccb.ataio.cmd.command = cmd;
		ccb.ataio.cmd.features = protocol;
		ccb.ataio.cmd.lba_low = (bufferlen / 512) >> 8;
		ccb.ataio.cmd.lba_mid = (comID & 0x00ff);
		ccb.ataio.cmd.lba_high = (comID & 0xff00) >> 8;
		ccb.ataio.cmd.device = 0x40;
		ccb.ataio.cmd.sector_count = bufferlen / 512;
	}

	ccb.ccb_h.flags |= CAM_PASS_ERR_RECOVER;

	if (cam_send_ccb(camdev, &ccb) < 0) {
		LOG(D4) << "cam_send_ccb failed";
		return (0xff);
	}

	if ((ccb.ccb_h.status & CAM_STATUS_MASK) != CAM_REQ_CMP) {
		IFLOG(D4)
			cam_error_print(camdev, &ccb, CAM_ESF_ALL, CAM_EPF_ALL, stderr);
		return (0xff);
	}
	return (0);
}

static void safecopy(uint8_t * dst, size_t dstsize, uint8_t * src, size_t srcsize)
{
	const size_t size = min(dstsize, srcsize);
	if (size > 0) memcpy(dst, src, size);
	if (size < dstsize) memset(dst+size, '\0', dstsize-size);
}

static int
nvme_get_cdata(struct cam_device *dev, struct nvme_controller_data *cdata)
{
	union ccb *ccb;
	struct ccb_dev_advinfo *advi;

	ccb = cam_getccb(dev);
	if (ccb == NULL) {
		LOG(D4) << "couldn't allocate CCB";
		return (1);
	}

	advi = &ccb->cdai;
	advi->ccb_h.flags = CAM_DIR_IN;
	advi->ccb_h.func_code = XPT_DEV_ADVINFO;
	advi->flags = CDAI_FLAG_NONE;
	advi->buftype = CDAI_TYPE_NVME_CNTRL;
	advi->bufsiz = sizeof(struct nvme_controller_data);
	advi->buf = (uint8_t *)cdata;

	if (cam_send_ccb(dev, ccb) < 0) {
		LOG(D4) << "error sending CAMIOCOMMAND ioctl";
		cam_freeccb(ccb);
		return (1);
	}
	if (advi->ccb_h.status != CAM_REQ_CMP) {
		LOG(D4) << "got CAM error " << advi->ccb_h.status;
		cam_freeccb(ccb);
		return (1);
	}
	cam_freeccb(ccb);
	return (0);
}

void DtaDevFreeBSDCAM::identify(OPAL_DiskInfo& disk_info)
{
	union ccb ccb;
	struct nvme_controller_data cdata;

	LOG(D4) << "Entering DtaDevFreeBSDCAM::identify()";

	bzero(&ccb, sizeof(union ccb));
	ccb.ccb_h.func_code = XPT_GDEV_TYPE;
	if (cam_send_ccb(camdev, &ccb) < 0) {
		LOG(D4) << "cam_send_ccb failed";
		disk_info.devType = DEVICE_TYPE_OTHER;
		return;
	}

	if ((ccb.ccb_h.status & CAM_STATUS_MASK) != CAM_REQ_CMP) {
		LOG(D4) << "cam_send_ccb error" << ccb.ccb_h.status;
		disk_info.devType = DEVICE_TYPE_OTHER;
		return;
	}

	if (ccb.cgd.protocol == PROTO_SCSI) {
		isSCSI = 1;
		disk_info.devType = DEVICE_TYPE_SAS;
		safecopy(disk_info.serialNum, sizeof(disk_info.serialNum),
		    (uint8_t *)ccb.cgd.serial_num, ccb.cgd.serial_num_len);
		safecopy(disk_info.firmwareRev, sizeof(disk_info.firmwareRev),
		    (uint8_t *)ccb.cgd.inq_data.revision, sizeof(ccb.cgd.inq_data.revision));
		safecopy(disk_info.modelNum, sizeof(disk_info.modelNum),
		    (uint8_t *)ccb.cgd.inq_data.vendor, sizeof(ccb.cgd.inq_data.vendor) + sizeof(ccb.cgd.inq_data.product));
	} else if (ccb.cgd.protocol == PROTO_NVME) {
		isNVMe = 1;
		if (nvme_get_cdata(camdev, &cdata)) {
			LOG(D4) << "nvme_get_cdata failed";
			disk_info.devType = DEVICE_TYPE_OTHER;
			return;
		}
		safecopy(disk_info.serialNum, sizeof(disk_info.serialNum),
		    (uint8_t *)cdata.sn, sizeof(cdata.sn));
		safecopy(disk_info.firmwareRev, sizeof(disk_info.firmwareRev),
		    (uint8_t *)cdata.fr, sizeof(cdata.fr));
		safecopy(disk_info.modelNum, sizeof(disk_info.modelNum),
		    (uint8_t *)cdata.mn, sizeof(cdata.mn));
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
	} else if (ccb.cgd.protocol == PROTO_ATA) {
		safecopy(disk_info.serialNum, sizeof(disk_info.serialNum),
		    (uint8_t *)ccb.cgd.serial_num, ccb.cgd.serial_num_len);
		memcpy(disk_info.firmwareRev, ccb.cgd.ident_data.revision,
		    sizeof(disk_info.firmwareRev));
		memcpy(disk_info.modelNum, ccb.cgd.ident_data.model,
		    sizeof(disk_info.modelNum));
		if ((ccb.cgd.ident_data.usedmovsd & 0xc001) == 0x4001) {
			LOG(D4) << "Trusted Computing feature set is supported "
			    << std::hex << ccb.cgd.ident_data.usedmovsd;
			disk_info.devType = DEVICE_TYPE_ATA;
		} else {
			LOG(D4) << "Trusted Computing feature set is not supported "
			    << std::hex << ccb.cgd.ident_data.usedmovsd;
			disk_info.devType = DEVICE_TYPE_OTHER;
		}
	} else {
		disk_info.devType = DEVICE_TYPE_OTHER;
	}
}

/** Close the device reference so this object can be delete. */
DtaDevFreeBSDCAM::~DtaDevFreeBSDCAM()
{
	LOG(D1) << "Destroying DtaDevFreeBSDCAM";
	cam_close_device(camdev);
}
