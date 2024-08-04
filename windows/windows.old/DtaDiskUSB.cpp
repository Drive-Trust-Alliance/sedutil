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
#pragma once
#include "os.h"
#include <stdio.h>
#include <iostream>
#pragma warning(push)
#pragma warning(disable : 4091)
#include <Ntddscsi.h>
#pragma warning(pop)
#include <winioctl.h>
#include <vector>
#include "DtaDiskUSB.h"
#include "DtaEndianFixup.h"
#include "DtaStructures.h"
#include "DtaHexDump.h"

using namespace std;
DtaDiskUSB::DtaDiskUSB() {};
void DtaDiskUSB::init(const char * devref)
{
    LOG(D1) << "Creating DtaDiskUSB::DtaDiskUSB() " << devref;
	physicalDriveId = atoi(devref + 17);
    SDWB * scsi =
            (SDWB *) _aligned_malloc((sizeof (SDWB)), 4096);
    scsiPointer = (void *) scsi;
     osDeviceHandle = CreateFile(devref,
                      GENERIC_WRITE | GENERIC_READ,
                      FILE_SHARE_WRITE | FILE_SHARE_READ,
                      NULL,
                      OPEN_EXISTING,
                      0,
                      NULL);
    if (INVALID_HANDLE_VALUE == osDeviceHandle)
		return;
    else
        isOpen = TRUE;
}

uint8_t DtaDiskUSB::sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                        void * buffer, uint32_t bufferlen)
{
	LOG(D1) << "Entering DtaDiskUSB::sendCmd";
    DWORD bytesReturned = 0; // data returned
    if (!isOpen) {
        LOG(D1) << "Device open failed";
		return DTAERROR_OPEN_ERR; //disk open failed so this will too
    }
    /*
     * Initialize the SCSI_PASS_THROUGH_DIRECT structures
     * per windows DOC with the SCSI Command set reference
     */
    SDWB * scsi = (SDWB *) scsiPointer;
    memset(scsi, 0, sizeof (SDWB));
	scsi->sd.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
	scsi->sd.SenseInfoOffset = offsetof(SDWB, sensebytes);
	scsi->sd.ScsiStatus = 0;
	scsi->sd.PathId = 0;
	scsi->sd.TargetId = 0;
	scsi->sd.Lun = 0;
	scsi->sd.DataBuffer = buffer;
	scsi->sd.DataTransferLength = bufferlen;
	scsi->sd.TimeOutValue = 20;
	scsi->sd.SenseInfoLength = 32;
	scsi->sd.CdbLength = 12;
	scsi->sd.Cdb[0] = 0xa1;
	scsi->sd.Cdb[9] = (UCHAR) cmd;

	if (IF_RECV == cmd) {
		/* security protocol in */
		scsi->sd.DataIn = SCSI_IOCTL_DATA_IN;
		scsi->sd.Cdb[1] = 4 << 1;  // PIO IN
		scsi->sd.Cdb[2] = 0x0e;
		scsi->sd.Cdb[4] = (UCHAR) (bufferlen / 512);

	}
	else if (IDENTIFY == cmd) {

		/* Inquiry command */
		scsi->sd.DataTransferLength = 512;
		scsi->sd.DataIn = SCSI_IOCTL_DATA_IN;
		scsi->sd.Cdb[1] = 4 << 1;  // PIO IN
		scsi->sd.Cdb[2] = 0x0e;
		scsi->sd.Cdb[4] = 1;
	}
	else {
		/* security protocol out */
		scsi->sd.DataIn = SCSI_IOCTL_DATA_OUT;
		scsi->sd.Cdb[1] = 5 << 1;  // PIO OUT
		scsi->sd.Cdb[2] = 0x06;
		scsi->sd.Cdb[4] = (UCHAR)(bufferlen / 512);
	}


	scsi->sd.Cdb[3] = protocol;
	scsi->sd.Cdb[7] = ((comID & 0xff00) >> 8); // Commid MSB
	scsi->sd.Cdb[6] = (comID & 0x00ff); // Commid LSB

	//printf("comID=%04Xh;   protocol=%04Xh\n", comID, protocol);

    DeviceIoControl(osDeviceHandle, // device to be queried
                    IOCTL_SCSI_PASS_THROUGH_DIRECT, // operation to perform
                    scsi, sizeof (SDWB),
                    scsi, sizeof (SDWB),
                    &bytesReturned, // # bytes returned
                    (LPOVERLAPPED) NULL); // synchronous I/O
     return (scsi->sd.ScsiStatus);
}

/** adds the IDENTIFY information to the disk_info structure */

typedef struct _UASP_INQUIRY_RESPONSE {
    uint8_t fill1[20];
    char ProductSerial[20];
    uint8_t fill2[6];
    char ProductRev[8];
    char ProductID[40];
} UASP_INQUIRY_RESPONSE;

bool  DtaDiskUSB::identify(DTA_DEVICE_INFO& disk_info)
{
    LOG(D1) << "Entering DtaDiskUSB::identify()";
	vector<uint8_t> nullz(512, 0x00);
    void * identifyResp = NULL;
	identifyResp = _aligned_malloc(MIN_BUFFER_LENGTH, IO_BUFFER_ALIGNMENT);
    if (NULL == identifyResp) return;
    memset(identifyResp, 0, MIN_BUFFER_LENGTH);
    uint8_t iorc = sendCmd(IDENTIFY, 0x00, 0x0000, identifyResp, MIN_BUFFER_LENGTH);

    if (0x00 != iorc) {
        LOG(D) << "USB IDENTIFY Failed " << (uint16_t) iorc;
		//RTS_SATA = 0; // not SATA
    }
	if (!(memcmp(identifyResp, nullz.data(), 512))) {
		disk_info.devType = DEVICE_TYPE_OTHER;
		/////////////////////////////////////////
		_aligned_free(identifyResp);
		return false;
	}
	UASP_INQUIRY_RESPONSE * id = (UASP_INQUIRY_RESPONSE *) identifyResp;
    disk_info.devType = DEVICE_TYPE_USB;
	uint8_t non_ascii = 0;
    for (int i = 0; i < sizeof (disk_info.serialNum); i += 2) {
        disk_info.serialNum[i] = id->ProductSerial[i + 1];
        disk_info.serialNum[i + 1] = id->ProductSerial[i];
		if (!isprint(disk_info.serialNum[i])) { non_ascii = 1; ; break; };
		if (!isprint(disk_info.serialNum[i+1])) { non_ascii = 1; ; break; };
    }
	if (non_ascii) {
		for (int i = 0; i < sizeof(disk_info.serialNum); i += 1) {
			disk_info.serialNum[i] = ' ';
		}
	}
	//memcpy(disk_info.serialNum, id->ProductSerial, sizeof(disk_info.serialNum));
	non_ascii = 0;
    for (int i = 0; i < sizeof (disk_info.firmwareRev); i += 2) {
        disk_info.firmwareRev[i] = id->ProductRev[i + 1];
        disk_info.firmwareRev[i + 1] = id->ProductRev[i];
		if (!isprint(disk_info.firmwareRev[i])) { non_ascii = 1; ; break; }
		if (!isprint(disk_info.firmwareRev[i + 1])) { non_ascii = 1; ; break; }
    }
	if (non_ascii) {
		for (int i = 0; i < sizeof(disk_info.firmwareRev); i += 1) {
			disk_info.firmwareRev[i] = ' ';
		}
	}
	//memcpy(disk_info.firmwareRev, id->ProductRev, sizeof(disk_info.firmwareRev));
	non_ascii = 0;
    for (int i = 0; i < sizeof (disk_info.modelNum); i += 2) {
        disk_info.modelNum[i] = id->ProductID[i + 1];
        disk_info.modelNum[i + 1] = id->ProductID[i];
		if (!isprint(disk_info.modelNum[i])) { non_ascii = 1; ; break; };
		if (!isprint(disk_info.modelNum[i + 1])) { non_ascii = 1; ; break; };
    }
	if (non_ascii) {
		for (int i = 0; i < sizeof(disk_info.modelNum); i += 1) {
			disk_info.modelNum[i] = ' ';
		}
	}
	disk_info.fips = *(((uint8_t *)identifyResp) + 506) & 0x02; // Byte 506 bit 1
	//printf("disk_info.fips=%02X\n", disk_info.fips);

	//DtaHexDump(disk_info.serialNum, sizeof(disk_info.serialNum));
	//DtaHexDump(disk_info.firmwareRev, sizeof(disk_info.firmwareRev));
	//DtaHexDump(disk_info.modelNum, sizeof(disk_info.modelNum));
	//memcpy(disk_info.modelNum, id->ProductID, sizeof(disk_info.modelNum));
	_aligned_free(identifyResp);
    return true;
}

/** Close the filehandle so this object can be delete. */
DtaDiskUSB::~DtaDiskUSB()
{
    LOG(D1) << "Destroying DtaDiskUSB";
    CloseHandle(osDeviceHandle);
    _aligned_free(scsiPointer);
}
