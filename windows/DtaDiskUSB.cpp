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
     hDev = CreateFile(devref,
                      GENERIC_WRITE | GENERIC_READ,
                      FILE_SHARE_WRITE | FILE_SHARE_READ,
                      NULL,
                      OPEN_EXISTING,
                      0,
                      NULL);
    if (INVALID_HANDLE_VALUE == hDev) 
		return;
    else 
        isOpen = TRUE;
}

uint8_t DtaDiskUSB::sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID, void * buffer, uint32_t bufferlen)
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
 
    DeviceIoControl(hDev, // device to be queried
                    IOCTL_SCSI_PASS_THROUGH_DIRECT, // operation to perform
                    scsi, sizeof (SDWB),
                    scsi, sizeof (SDWB),
                    &bytesReturned, // # bytes returned
                    (LPOVERLAPPED) NULL); // synchronous I/O
     return (scsi->sd.ScsiStatus);
}

/** adds the IDENTIFY information to the disk_info structure */

void DtaDiskUSB::identify(OPAL_DiskInfo& disk_info)
{
    LOG(D1) << "Entering DtaDiskUSB::identify()";
	vector<uint8_t> nullz(512, 0x00);
    void * identifyResp = NULL;
	identifyResp = _aligned_malloc(IO_BUFFER_LENGTH, IO_BUFFER_ALIGNMENT);
    if (NULL == identifyResp) return;
    memset(identifyResp, 0, IO_BUFFER_LENGTH);
    uint8_t iorc = sendCmd(IDENTIFY, 0x00, 0x0000, identifyResp, IO_BUFFER_LENGTH);

    if (0x00 != iorc) {
        LOG(D) << "USB IDENTIFY Failed " << (uint16_t) iorc;
		//RTS_SATA = 0; // not SATA
    }
	if (!(memcmp(identifyResp, nullz.data(), 512))) {
		disk_info.devType = DEVICE_TYPE_OTHER;
		/////////////////////////////////////////
		_aligned_free(identifyResp);
		return;
	}
	USB_INQUIRY_DATA * id = (USB_INQUIRY_DATA *) identifyResp;
	IFLOG(D1) DtaHexDump(id, 512); // shall remove DEBUG
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

	//DtaHexDump(disk_info.serialNum, sizeof(disk_info.serialNum));
	//DtaHexDump(disk_info.firmwareRev, sizeof(disk_info.firmwareRev));
	//DtaHexDump(disk_info.modelNum, sizeof(disk_info.modelNum));
	//memcpy(disk_info.modelNum, id->ProductID, sizeof(disk_info.modelNum));
	_aligned_free(identifyResp);
    return;
}

/** Close the filehandle so this object can be delete. */
DtaDiskUSB::~DtaDiskUSB()
{
    LOG(D1) << "Destroying DtaDiskUSB";
    CloseHandle(hDev);
    _aligned_free(scsiPointer);
}

HANDLE DtaDiskUSB::GetIoCtrlHandle(BYTE index)
{
	char strDevice[64] = { 0 };

	sprintf_s(strDevice, "\\\\.\\PhysicalDrive%d",index);

	return ::CreateFile(strDevice, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, 0, NULL);
}

/////////////////////////////////////////////

void DtaDiskUSB::identifyPd(OPAL_DiskInfo& disk_info) {
	LOG(D1) << "Entering DtaDiskUSB::identifyPd(OPAL_DiskInfo& disk_info)";
	vector<uint8_t> nullz(512, 0x00);
	void * identifyResp = NULL;
	identifyResp = _aligned_malloc(IO_BUFFER_LENGTH, IO_BUFFER_ALIGNMENT);
	if (NULL == identifyResp) return;
	memset(identifyResp, 0, IO_BUFFER_LENGTH);

	// before give up, try identifyDevicePd
	////////////////////////////////////////
	IDENTIFY_DEVICE identify = { 0 };
	if (DoIdentifyDevicePd(physicalDriveId, 0xA0, &identify)) {
		// success
		LOG(D) << "USB IDENTIFYPD OK possible with Nvme ";
		USB_INQUIRY_DATA * id = (USB_INQUIRY_DATA *)&identify;
		IFLOG(D1) DtaHexDump(id, 512);
		disk_info.devType = DEVICE_TYPE_NVME;
		uint8_t non_ascii = 0;
		memset(disk_info.serialNum, 32, sizeof(disk_info.serialNum));
		for (int i = 0; i < sizeof(disk_info.serialNum); i += 2) {
			disk_info.serialNum[i] = (id->ProductSerial[i + 1] == 0) ? 32 : id->ProductSerial[i + 1];
			disk_info.serialNum[i + 1] = (id->ProductSerial[i] == 0) ? 32 : id->ProductSerial[i];
			if (!isprint(disk_info.serialNum[i])) { non_ascii = 1; ; break; };
			if (!isprint(disk_info.serialNum[i + 1])) { non_ascii = 1; ; break; };
		}
		//if (non_ascii) {
		//	for (int i = 0; i < sizeof(disk_info.serialNum); i += 1) {
		//		disk_info.serialNum[i] = ' ';
		//	}
		//}
		non_ascii = 0;
		memset(disk_info.firmwareRev, 32, sizeof(disk_info.firmwareRev));
		for (int i = 0; i < sizeof(disk_info.firmwareRev); i += 2) {
			disk_info.firmwareRev[i] = (id->ProductRev[i + 1] == 0) ? 32 : id->ProductRev[i + 1];
			disk_info.firmwareRev[i + 1] = (id->ProductRev[i] == 0) ? 32 : id->ProductRev[i]; //id->ProductRev[i];
			if (!isprint(disk_info.firmwareRev[i])) { non_ascii = 1; ; break; }
			if (!isprint(disk_info.firmwareRev[i + 1])) { non_ascii = 1; ; break; }
		}
		//if (non_ascii) {
		//	for (int i = 0; i < sizeof(disk_info.firmwareRev); i += 1) {
		//		disk_info.firmwareRev[i] = ' ';
		//	}
		//}
		non_ascii = 0;
		memset(disk_info.modelNum, 32, sizeof(disk_info.modelNum));
		for (int i = 0; i < sizeof(disk_info.modelNum); i += 2) {
			disk_info.modelNum[i] = (id->ProductID[i + 1] == 0)? 32 : id->ProductID[i + 1];
			disk_info.modelNum[i + 1] = (id->ProductID[i] == 0) ? 32 : id->ProductID[i]; // id->ProductID[i];
			if (!isprint(disk_info.modelNum[i])) { non_ascii = 1; ; break; };
			if (!isprint(disk_info.modelNum[i + 1])) { non_ascii = 1; ; break; };
		}
		//if (non_ascii) {
		//	for (int i = 0; i < sizeof(disk_info.modelNum); i += 1) {
		//		disk_info.modelNum[i] = ' ';
		//	}
		//}

	}
	//else {
		// fail again 
	//}
	_aligned_free(identifyResp);
	return;
}

BOOL DtaDiskUSB::DoIdentifyDevicePd(INT physicalDriveId, BYTE target, IDENTIFY_DEVICE * data)
{
	BOOL	bRet = FALSE;
	HANDLE	hIoCtrl;
	DWORD	dwReturned;
	CHAR * cstr;

	IDENTIFY_DEVICE_OUTDATA	sendCmdOutParam;
	SENDCMDINPARAMS	sendCmd;
	LOG(D1) << "Enetring DoIdentifyDevicePd";
	if (data == NULL)
	{
		LOG(D1) << "Enetring DoIdentifyDevicePd data == Null return FALSE";
		return	FALSE;
	}

	LOG(D1) << "SendAtaCommandPd - IDENTIFY_DEVICE (ATA_PASS_THROUGH) - EC";
	// SendAtaCommandPd(INT physicalDriveId, target, main, sub,  param, PBYTE data, DWORD dataSize)
	bRet = SendAtaCommandPd(physicalDriveId, target, 0xEC, 0x00, 0x00, (PBYTE)data, sizeof(IDENTIFY_DEVICE));
	cstr = data->A.Model;

	if (bRet == FALSE || strlen(cstr) == 0 )
	{
		LOG(D1) << "SendAtaCommandPd - IDENTIFY_DEVICE (ATA_PASS_THROUGH) - EC return FALSE or no mdoel number";
		::ZeroMemory(data, sizeof(IDENTIFY_DEVICE));
		hIoCtrl = GetIoCtrlHandle(physicalDriveId);
		if (hIoCtrl == INVALID_HANDLE_VALUE)
		{
			return	FALSE;
		}
		::ZeroMemory(&sendCmdOutParam, sizeof(IDENTIFY_DEVICE_OUTDATA));
		::ZeroMemory(&sendCmd, sizeof(SENDCMDINPARAMS));

		sendCmd.irDriveRegs.bCommandReg = ID_CMD;
		sendCmd.irDriveRegs.bSectorCountReg = 1;
		sendCmd.irDriveRegs.bSectorNumberReg = 1;
		sendCmd.irDriveRegs.bDriveHeadReg = target;
		sendCmd.cBufferSize = IDENTIFY_BUFFER_SIZE;

		LOG(D1) << "SendAtaCommandPd - IDENTIFY_DEVICE - DFP_RECEIVE_DRIVE_DATA";
		bRet = ::DeviceIoControl(hIoCtrl, DFP_RECEIVE_DRIVE_DATA,
			&sendCmd, sizeof(SENDCMDINPARAMS),
			&sendCmdOutParam, sizeof(IDENTIFY_DEVICE_OUTDATA),
			&dwReturned, NULL);

		::CloseHandle(hIoCtrl);

		if (bRet == FALSE || dwReturned != sizeof(IDENTIFY_DEVICE_OUTDATA))
		{
			LOG(D1) << "SendAtaCommandPd - IDENTIFY_DEVICE - DFP_RECEIVE_DRIVE_DATA return FALSE";
			return	FALSE;
		}
		else {
			LOG(D1) << "SendAtaCommandPd - IDENTIFY_DEVICE - DFP_RECEIVE_DRIVE_DATA return TRUE";
		}

		memcpy_s(data, sizeof(IDENTIFY_DEVICE), sendCmdOutParam.SendCmdOutParam.bBuffer, sizeof(IDENTIFY_DEVICE));
	}

	return	TRUE;
}


BOOL m_bAtaPassThrough = TRUE;

BOOL DtaDiskUSB::SendAtaCommandPd(INT physicalDriveId, BYTE target, BYTE main, BYTE sub, BYTE param, PBYTE data, DWORD dataSize)
{
	BOOL	bRet;
	HANDLE	hIoCtrl;
	DWORD	dwReturned;
	LOG(D1) << "Entering SendAtaCommandPd(INT physicalDriveId, BYTE target, BYTE main, BYTE sub, BYTE param, PBYTE data, DWORD dataSize)";
	hIoCtrl = GetIoCtrlHandle(physicalDriveId);
	if (hIoCtrl == INVALID_HANDLE_VALUE)
	{
		return	FALSE;
	}

	if (m_bAtaPassThrough)
	{
		ATA_PASS_THROUGH_EX_WITH_BUFFERS ab;
		::ZeroMemory(&ab, sizeof(ab));
		ab.Apt.Length = sizeof(ATA_PASS_THROUGH_EX);
		ab.Apt.TimeOutValue = 2;
		DWORD size = offsetof(ATA_PASS_THROUGH_EX_WITH_BUFFERS, Buf);
		ab.Apt.DataBufferOffset = size;

		if (dataSize > 0)
		{
			if (dataSize > sizeof(ab.Buf))
			{
				return FALSE;
			}
			ab.Apt.AtaFlags = ATA_FLAGS_DATA_IN;
			ab.Apt.DataTransferLength = dataSize;
			ab.Buf[0] = 0xCF; // magic number
			size += dataSize;
		}

		ab.Apt.CurrentTaskFile.bFeaturesReg = sub;
		ab.Apt.CurrentTaskFile.bSectorCountReg = param;
		ab.Apt.CurrentTaskFile.bDriveHeadReg = target;
		ab.Apt.CurrentTaskFile.bCommandReg = main;

		if (main == SMART_CMD)
		{
			ab.Apt.CurrentTaskFile.bCylLowReg = SMART_CYL_LOW;
			ab.Apt.CurrentTaskFile.bCylHighReg = SMART_CYL_HI;
			ab.Apt.CurrentTaskFile.bSectorCountReg = 1;
			ab.Apt.CurrentTaskFile.bSectorNumberReg = 1;
		}

		bRet = ::DeviceIoControl(hIoCtrl, IOCTL_ATA_PASS_THROUGH,
			&ab, size, &ab, size, &dwReturned, NULL);
		::CloseHandle(hIoCtrl);
		// If the operation completes successfully, the return value is nonzero.
		if (bRet && dataSize && data != NULL)
		{
			LOG(D1) << "Entering SendAtaCommandPd(INT physicalDriveId, BYTE target, BYTE main, BYTE sub, BYTE param, PBYTE data, DWORD dataSize) - ::DeviceIoControl(hIoCtrl, IOCTL_ATA_PASS_THROUGH OK";
			memcpy_s(data, dataSize, ab.Buf, dataSize);
			//LOG(D) << "data = ";
			//DtaHexDump(data, 512);
			//LOG(D) << "ab.Buf = "; 
			//DtaHexDump(ab.Buf, 512);
		}
		else { 
			LOG(D1) << "Entering SendAtaCommandPd(INT physicalDriveId, BYTE target, BYTE main, BYTE sub, BYTE param, PBYTE data, DWORD dataSize) - ::DeviceIoControl(hIoCtrl, IOCTL_ATA_PASS_THROUGH FAIL"; 
		}

	}
	//else if (m_Os.dwMajorVersion <= 4)
	//{
	//	return FALSE;
	//}
	else
	{
		DWORD size = sizeof(CMD_IDE_PATH_THROUGH) - 1 + dataSize;
		CMD_IDE_PATH_THROUGH* buf = (CMD_IDE_PATH_THROUGH*)VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE);

		buf->reg.bFeaturesReg = sub;
		buf->reg.bSectorCountReg = param;
		buf->reg.bSectorNumberReg = 0;
		buf->reg.bCylLowReg = 0;
		buf->reg.bCylHighReg = 0;
		buf->reg.bDriveHeadReg = target;
		buf->reg.bCommandReg = main;
		buf->reg.bReserved = 0;
		buf->length = dataSize;

		bRet = ::DeviceIoControl(hIoCtrl, IOCTL_IDE_PASS_THROUGH,
			buf, size, buf, size, &dwReturned, NULL);
		::CloseHandle(hIoCtrl);
		if (bRet && dataSize && data != NULL)
		{
			LOG(D1) << "Entering SendAtaCommandPd(INT physicalDriveId, BYTE target, BYTE main, BYTE sub, BYTE param, PBYTE data, DWORD dataSize) - DeviceIoControl(hIoCtrl, IOCTL_IDE_PASS_THROUGH, - OK";
			memcpy_s(data, dataSize, buf->buffer, dataSize);
		}
		else { LOG(D1) << "Entering SendAtaCommandPd(INT physicalDriveId, BYTE target, BYTE main, BYTE sub, BYTE param, PBYTE data, DWORD dataSize) - DeviceIoControl(hIoCtrl, IOCTL_IDE_PASS_THROUGH, - FAIL"; }
		VirtualFree(buf, 0, MEM_RELEASE);
	}

	LOG(D1) << "Exiting SendAtaCommandPd(INT physicalDriveId, BYTE target, BYTE main, BYTE sub, BYTE param, PBYTE data, DWORD dataSize) ";
	return	bRet;
}


/*---------------------------------------------------------------------------*/
//  NVMe ASMedia
/*---------------------------------------------------------------------------*/

//BOOL DtaDiskUSB::DoIdentifyDevicePd(INT physicalDriveId, BYTE target, IDENTIFY_DEVICE * data);
void DtaDiskUSB::identifyNVMeASMedia(OPAL_DiskInfo& disk_info) {
	LOG(D1) << "Entering DtaDiskUSB::identifyNVMeASMedia()";
	vector<uint8_t> nullz(512, 0x00);
	void * identifyResp = NULL;
	identifyResp = _aligned_malloc(IO_BUFFER_LENGTH, IO_BUFFER_ALIGNMENT);
	if (NULL == identifyResp) return;
	memset(identifyResp, 0, IO_BUFFER_LENGTH);

	// before give up, try identifyDevicePd
	////////////////////////////////////////
	IDENTIFY_DEVICE identify = { 0 };
	//BOOL DtaDiskUSB::DoIdentifyDeviceNVMeASMedia(INT physicalDriveId, INT scsiPort, INT scsiTargetId, IDENTIFY_DEVICE* data)
	if (DoIdentifyDeviceNVMeASMedia(physicalDriveId, 0xA0,0, &identify)) {
		// success
		LOG(D) << "USB IDENTIFY OK possible with NVMeASMedia ";
		USB_INQUIRY_DATA * id = (USB_INQUIRY_DATA *)&identify;
		DtaHexDump(id, 512);
		disk_info.devType = DEVICE_TYPE_NVME;
		uint8_t non_ascii = 0;
		for (int i = 0; i < sizeof(disk_info.serialNum); i += 2) {
			disk_info.serialNum[i] = id->ProductSerial[i ];
			disk_info.serialNum[i + 1] = id->ProductSerial[i + 1];
			if (!isprint(disk_info.serialNum[i])) { non_ascii = 1; ; break; };
			if (!isprint(disk_info.serialNum[i + 1])) { non_ascii = 1; ; break; };
		}
		for (int i = 0; i < sizeof(disk_info.firmwareRev); i += 2) {
			disk_info.firmwareRev[i] = id->ProductRev[i ];
			disk_info.firmwareRev[i + 1] = id->ProductRev[i + 1];
			if (!isprint(disk_info.firmwareRev[i])) { non_ascii = 1; ; break; }
			if (!isprint(disk_info.firmwareRev[i + 1])) { non_ascii = 1; ; break; }
		}
		for (int i = 0; i < sizeof(disk_info.modelNum); i += 2) {
			disk_info.modelNum[i] = id->ProductID[i];
			disk_info.modelNum[i + 1] = id->ProductID[i + 1];
			if (!isprint(disk_info.modelNum[i])) { non_ascii = 1; ; break; };
			if (!isprint(disk_info.modelNum[i + 1])) { non_ascii = 1; ; break; };
		}

	}
	//else {
	// fail again 
	//}
	_aligned_free(identifyResp);
	return;
}

BOOL GetScsiAddressASMedia(const TCHAR* Path, BYTE* PortNumber, BYTE* PathId, BYTE* TargetId, BYTE* Lun)
{
	HANDLE hDevice = CreateFile(Path,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);
	DWORD dwReturned;
	SCSI_ADDRESS ScsiAddr;
	BOOL bRet = DeviceIoControl(hDevice, IOCTL_SCSI_GET_ADDRESS,
		nullptr, 0, &ScsiAddr, sizeof(ScsiAddr), &dwReturned, NULL);

	CloseHandle(hDevice);

	*PortNumber = ScsiAddr.PortNumber;
	*PathId = ScsiAddr.PathId;
	*TargetId = ScsiAddr.TargetId;
	*Lun = ScsiAddr.Lun;

	return bRet == TRUE;
}

BOOL DtaDiskUSB::DoIdentifyDeviceNVMeASMedia(INT physicalDriveId, INT scsiPort, INT scsiTargetId, IDENTIFY_DEVICE* data)
{
	HANDLE	hIoCtrl;
	printf("Entering DoIdentifyDeviceNVMeASmedia(physicalDriveId=%d,scsiPort=%d,scsiTargetId=%d)\n", physicalDriveId, scsiPort, scsiTargetId);
	BYTE portNumber, pathId, targetId, lun;
	TCHAR path[64] = TEXT("\0");
	sprintf_s(path, "\\\\.\\PhysicalDrive%d", physicalDriveId);
	// translate physical address to scsi port number,
	GetScsiAddressASMedia(path, &portNumber, &pathId, &targetId, &lun);

	TCHAR drive[64] = TEXT("\0");
	sprintf_s(drive, TEXT("\\\\.\\Scsi%d:"), portNumber);
	hIoCtrl = CreateFile(drive,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
		OPEN_EXISTING, 0, nullptr);


	BOOL	bRet;
	
	DWORD	dwReturned;
	DWORD	length;

	SCSI_PASS_THROUGH_WITH_BUFFERS sptwb;

	if (data == NULL)
	{
		printf("data == NULL Leaving DoIdentifyDeviceNVMeASmedia(physicalDriveId=%d,scsiPort=%d,scsiTargetId=%d)", physicalDriveId, scsiPort, scsiTargetId);

		return	FALSE;
	}

	::ZeroMemory(data, sizeof(IDENTIFY_DEVICE));

	hIoCtrl = GetIoCtrlHandle(physicalDriveId);

	if (hIoCtrl == INVALID_HANDLE_VALUE)
	{
		//debug.Format(L"INVALID_HANDLE_VALUE Leaving DoIdentifyDeviceNVMeASmedia(physicalDriveId=%d,scsiPort=%d,scsiTargetId=%d)", physicalDriveId, scsiPort, scsiTargetId);
		//DebugPrint(debug);
		return	FALSE;
	}

	::ZeroMemory(&sptwb, sizeof(SCSI_PASS_THROUGH_WITH_BUFFERS));
	sptwb.Spt.Length = sizeof(SCSI_PASS_THROUGH);
	sptwb.Spt.PathId = 0;
	sptwb.Spt.TargetId = 0;
	sptwb.Spt.Lun = 0;
	sptwb.Spt.SenseInfoLength = 24;
	sptwb.Spt.DataTransferLength = 4096;
	sptwb.Spt.TimeOutValue = 2;
	sptwb.Spt.DataBufferOffset = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, DataBuf);
	sptwb.Spt.SenseInfoOffset = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, SenseBuf);

	sptwb.Spt.CdbLength = 16;
	sptwb.Spt.Cdb[0] = 0xE6; // NVME PASS THROUGH
	sptwb.Spt.Cdb[1] = 0x06; // IDENTIFY
	sptwb.Spt.Cdb[3] = 0x01;

	sptwb.Spt.DataIn = SCSI_IOCTL_DATA_IN;

	length = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, DataBuf) + sptwb.Spt.DataTransferLength;

	bRet = ::DeviceIoControl(hIoCtrl, IOCTL_SCSI_PASS_THROUGH,
		&sptwb, length,
		&sptwb, length, &dwReturned, NULL);

	if (bRet == FALSE)
	{
		::CloseHandle(hIoCtrl);
		printf("IOCTL_SCSI_PASS_THROUGH FAIL Leaving DoIdentifyDeviceNVMeASmedia(physicalDriveId=%d,scsiPort=%d,scsiTargetId=%d)", physicalDriveId, scsiPort, scsiTargetId);
		return	FALSE;
	}

	DWORD count = 0;
	for (int i = 0; i < 512; i++)
	{
		count += sptwb.DataBuf[i];
	}
	if (count == 0)
	{
		::CloseHandle(hIoCtrl);
		printf("counter == %d Leaving DoIdentifyDeviceNVMeASmedia(physicalDriveId=%d,scsiPort=%d,scsiTargetId=%d)", physicalDriveId, scsiPort, scsiTargetId);
		return	FALSE;
	}

	memcpy_s(data, sizeof(IDENTIFY_DEVICE), sptwb.DataBuf, sizeof(IDENTIFY_DEVICE));
	DtaHexDump((BYTE*)data, 512 );
	::CloseHandle(hIoCtrl);

	printf("Leaving DoIdentifyDeviceNVMeASmedia(physicalDriveId=%d,scsiPort=%d,scsiTargetId=%d)", physicalDriveId, scsiPort, scsiTargetId);
	return TRUE;
}