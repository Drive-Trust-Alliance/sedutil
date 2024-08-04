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
#include "DtaDiskType.h"
#include "IntelRST.h"

// specific ////////////////////////////////////////////////////
//From DDK

#define	FILE_DEVICE_SCSI							0x0000001b
#define	IOCTL_SCSI_MINIPORT_IDENTIFY				((FILE_DEVICE_SCSI << 16) + 0x0501)
#define	IOCTL_SCSI_MINIPORT_READ_SMART_ATTRIBS		((FILE_DEVICE_SCSI << 16) + 0x0502)
#define IOCTL_SCSI_MINIPORT_READ_SMART_THRESHOLDS	((FILE_DEVICE_SCSI << 16) + 0x0503)
#define IOCTL_SCSI_MINIPORT_ENABLE_SMART			((FILE_DEVICE_SCSI << 16) + 0x0504)
#define IOCTL_SCSI_MINIPORT_DISABLE_SMART			((FILE_DEVICE_SCSI << 16) + 0x0505)

#define IOCTL_SCSI_BASE                 FILE_DEVICE_CONTROLLER
#define IOCTL_SCSI_PASS_THROUGH         CTL_CODE(IOCTL_SCSI_BASE, 0x0401, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//
// Define values for pass-through DataIn field.
//
#define SCSI_IOCTL_DATA_OUT          0
#define SCSI_IOCTL_DATA_IN           1
#define SCSI_IOCTL_DATA_UNSPECIFIED  2

//
// Define the SCSI pass through structure.
//
/*
typedef struct _SCSI_PASS_THROUGH {
	USHORT Length;
	UCHAR ScsiStatus;
	UCHAR PathId;
	UCHAR TargetId;
	UCHAR Lun;
	UCHAR CdbLength;
	UCHAR SenseInfoLength;
	UCHAR DataIn;
	ULONG DataTransferLength;
	ULONG TimeOutValue;
	ULONG_PTR DataBufferOffset;
	ULONG SenseInfoOffset;
	UCHAR Cdb[16];
}SCSI_PASS_THROUGH, *PSCSI_PASS_THROUGH;
*/
typedef struct _SCSI_PASS_THROUGH_WITH_BUFFERS {
	SCSI_PASS_THROUGH Spt;
	ULONG             Filler;      // realign buffers to double word boundary
	UCHAR             SenseBuf[32];
	UCHAR             DataBuf[4096];
} SCSI_PASS_THROUGH_WITH_BUFFERS, *PSCSI_PASS_THROUGH_WITH_BUFFERS;

typedef struct _SCSI_PASS_THROUGH_WITH_BUFFERS24 {
	SCSI_PASS_THROUGH Spt;
	UCHAR             SenseBuf[24];
	UCHAR             DataBuf[4096];
} SCSI_PASS_THROUGH_WITH_BUFFERS24, *PSCSI_PASS_THROUGH_WITH_BUFFERS24;


/** Device specific implementation of disk access functions. */
typedef struct _SDWB {
	SCSI_PASS_THROUGH_DIRECT sd;
	WORD filler;
	char sensebytes[32];

} SDWB;


typedef struct _USB_INQUIRY_DATA_NVME {
	uint8_t fill1[4];
	char ProductSerial[20];
	char ProductID[40];
	char ProductRev[8];

} USB_INQUIRY_DATA_NVME;

class DtaDiskUSB : public DtaDiskType {
public:
	DtaDiskUSB();
	~DtaDiskUSB();
	/** device specific initialization.
	* This function should perform the necessary authority and environment checking
	* to allow proper functioning of the program, open the device, perform an
	* identify, add the fields from the identify response to the disk info structure
	* and if the device is an ATA device perform a call to Discovery0() to complete
	* the disk_info structure
	* @param devref character representation of the device is standard OS lexicon
	*/
	void init(const char * devref);
	/** OS specific method to send an ATA command to the device
	* @param cmd command to be sent to the device
	* @param protocol security protocol to be used in the command
	* @param comID communications ID to be used
	* @param buffer input/output buffer
	* @param bufferlen length of the input/output buffer
	*/
	uint8_t	sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
		void * buffer, uint32_t bufferlen);
	/** OS specific routine to send an ATA identify to the device */
	bool identify(DTA_DEVICE_INFO& disk_info);
	void identifyPd(DTA_DEVICE_INFO& disk_info);
	void identifyNVMeASMedia(DTA_DEVICE_INFO& disk_info);
	void identifyNVMeRealtek(DTA_DEVICE_INFO& disk_info);
	BOOL DoIdentifyDevicePd(INT physicalDriveId, BYTE target, IDENTIFY_DEVICE * data);
	BOOL DoIdentifyDeviceNVMeASMedia(INT physicalDriveId, INT scsiPort, INT scsiTargetId, IDENTIFY_DEVICE* data);
	BOOL DoIdentifyDeviceNVMeRealtek(INT physicalDriveId, INT scsiPort, INT scsiTargetId, IDENTIFY_DEVICE* data);

	HANDLE GetIoCtrlHandle(BYTE index);
	BOOL SendAtaCommandPd(INT physicalDriveId, BYTE target, BYTE main, BYTE sub, BYTE param, PBYTE data, DWORD dataSize);
private:
	void * scsiPointer;
	OSDEVICEHANDLE osDeviceHandle; /**< Windows device handle */
	uint8_t isOpen = FALSE;
public:
	BYTE physicalDriveId = 0;
	BYTE scsiTargetId;
	BYTE scsiPort;
};
