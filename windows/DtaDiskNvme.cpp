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
#include <Ntddstor.h>
#pragma warning(pop)
#include <vector>
#include "DtaDiskNVME.h"
#include "DtaEndianFixup.h"
#include "DtaStructures.h"
#include "DtaHexDump.h"
//#include "nvme.h"
//#include "nvmeIoctl.h"

DWORD GetDSN(HANDLE hDev, char * strDSN);
DWORD GetDSN(HANDLE hDev, char * strDSN)
{
	DWORD dwRet = NO_ERROR;
	// Set the input data structure
	STORAGE_PROPERTY_QUERY storagePropertyQuery;
	ZeroMemory(&storagePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY));
	storagePropertyQuery.PropertyId = StorageDeviceProperty;
	storagePropertyQuery.QueryType = PropertyStandardQuery;

	// Get the necessary output buffer size
	STORAGE_DESCRIPTOR_HEADER storageDescriptorHeader = { 0 };
	DWORD dwBytesReturned = 0;
	if (!DeviceIoControl(hDev, IOCTL_STORAGE_QUERY_PROPERTY,
		&storagePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
		&storageDescriptorHeader, sizeof(STORAGE_DESCRIPTOR_HEADER),
		&dwBytesReturned, NULL))
	{
		dwRet = GetLastError();
		LOG(E) << "IOCTL_STORAGE_QUERY_PROPERTY error";
		//CloseHandle(hDev);
		return dwRet;
	}

	// Alloc the output buffer
	const DWORD dwOutBufferSize = storageDescriptorHeader.Size;
	BYTE* pOutBuffer = new BYTE[dwOutBufferSize];
	ZeroMemory(pOutBuffer, dwOutBufferSize);

	// Get the storage device descriptor
	if (!DeviceIoControl(hDev, IOCTL_STORAGE_QUERY_PROPERTY,
		&storagePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
		pOutBuffer, dwOutBufferSize,
		&dwBytesReturned, NULL))
	{
		dwRet = GetLastError();
		delete[]pOutBuffer;
		//CloseHandle(hDev);
		LOG(E) << "IOCTL_STORAGE_QUERY_PROPERTY error";
		return dwRet;
	}

	// Now, the output buffer points to a STORAGE_DEVICE_DESCRIPTOR structure
	// followed by additional info like vendor ID, product ID, serial number, and so on.
	STORAGE_DEVICE_DESCRIPTOR* pDeviceDescriptor = (STORAGE_DEVICE_DESCRIPTOR*)pOutBuffer;
	const DWORD dwSerialNumberOffset = pDeviceDescriptor->SerialNumberOffset;
	char * p = strDSN;
	if (dwSerialNumberOffset != 0)
	{
		// Finally, get the serial number
		for (int i=0; i< 20; i++)
			*(p + i)  = *(pOutBuffer + dwSerialNumberOffset+i);
		IFLOG(D4) {
			LOG(I) << "Drive Series Number :";
			DtaHexDump(strDSN, 20);
		}
	}
	else {
		IFLOG(D4)
			LOG(E) << "no series number found";
	}

	// Do cleanup and return
	delete[]pOutBuffer;
	//CloseHandle(hDev);
	return dwRet;
}

using namespace std;
DtaDiskNVME::DtaDiskNVME() { LOG(D1) << "DtaDiskNVME Constructor"; };

void DtaDiskNVME::init(const char * devref)
{
	LOG(D1) << "Creating DtaDiskNVME::DtaDiskNVME() " << devref;
	//DtaHexDump((void *)devref, 23);
	SDWA * scsi =
		(SDWA *)_aligned_malloc((sizeof(SDWA)), 4096);
	scsiPointer = (void *)scsi;
	hDev = CreateFile(devref,
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if (INVALID_HANDLE_VALUE == hDev) {
		LOG(D1) << "Open Nvme fail. hDev = " << hex << hDev << endl;
		return;
	}
    else {
		LOG(D1) << "Open Nvme OK. hDev = " << hex << hDev << endl;
        isOpen = TRUE;
	}
	// seems to be system vol series number NOT IDFY command series number
	//  Drive Series Number :0025_385B_61B0_02FA.
}

uint8_t DtaDiskNVME::sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                        void * buffer, uint16_t bufferlen)
{
    LOG(D1) << "Entering DtaDiskNVME::sendCmd";
    DWORD bytesReturned = 0; // data returned
    if (!isOpen) {
        LOG(D1) << "Device open failed";
		return DTAERROR_OPEN_ERR; //disk open failed so this will too
    }
	SDWA * scsi = (SDWA *)scsiPointer;
	memset(scsi, 0, sizeof(SDWA));
	// same for every command
	scsi->scsiDetails.DataBuffer = buffer;
	scsi->scsiDetails.DataTransferLength = bufferlen;
	scsi->scsiDetails.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
	scsi->scsiDetails.TimeOutValue = 20;
	scsi->scsiDetails.SenseInfoLength = 32;
	scsi->scsiDetails.SenseInfoOffset = offsetof(SDWA, sensebytes);
	scsi->scsiDetails.ScsiStatus = 0;
	scsi->scsiDetails.PathId = 0;
	scsi->scsiDetails.TargetId = 0;
	scsi->scsiDetails.Lun = 0;


	if (IF_RECV == cmd) {
		LOG(D1) << "cmd = Security Receive";
		scsi->scsiDetails.DataIn = SCSI_IOCTL_DATA_IN;
		//memset(scsi.scsiDetails.Cdb, 0x00, sizeof(scsi.scsiDetails.Cdb));
		scsi->scsiDetails.CdbLength = 12;
		scsi->scsiDetails.Cdb[0] = 0xa2; // 0xa2 or 0xb5;
		scsi->scsiDetails.Cdb[1] = protocol;
		scsi->scsiDetails.Cdb[2] = (comID >> 8) & 0xff;
		scsi->scsiDetails.Cdb[3] = comID & 0xff;
		scsi->scsiDetails.Cdb[6] = 0; //  (bufferlen >> 24) & 0xff;
		scsi->scsiDetails.Cdb[7] = 0; // (bufferlen >> 16) & 0xff; 
		scsi->scsiDetails.Cdb[8] = (bufferlen >> 8) & 0xff;
		scsi->scsiDetails.Cdb[9] = bufferlen & 0xff;
		scsi->scsiDetails.Cdb[10] = 0;
		scsi->scsiDetails.Cdb[11] = 0;
	}
	else if (IDENTIFY == cmd) {
		//===============================
		LOG(D1) << "cmd = IDENTIFY";
		//printf("pagecode = %02Xh\n", pagecode);
		scsi->scsiDetails.DataIn = SCSI_IOCTL_DATA_IN;
		scsi->scsiDetails.CdbLength = 6;
		scsi->scsiDetails.Cdb[0] = 0x12; // 0xa2 or 0xa5;
		scsi->scsiDetails.Cdb[1] = 0; // evpd;
		scsi->scsiDetails.Cdb[2] = 0; // pagecode; //  
		scsi->scsiDetails.Cdb[3] = (bufferlen >> 8) & 0xff;
		scsi->scsiDetails.Cdb[4] = bufferlen & 0xff;
	}
	else {
		// security send command
		LOG(D1) << "cmd = Security Send" ;
		scsi->scsiDetails.DataIn = SCSI_IOCTL_DATA_OUT;
		scsi->scsiDetails.CdbLength = 12;
		scsi->scsiDetails.Cdb[0] = 0xb5; // 0xa2 or 0xb5;
		scsi->scsiDetails.Cdb[1] = protocol;
		scsi->scsiDetails.Cdb[2] = (comID >> 8) & 0xff;
		scsi->scsiDetails.Cdb[3] = comID & 0xff;
		scsi->scsiDetails.Cdb[6] = 0; //  (bufferlen >> 24) & 0xff;
		scsi->scsiDetails.Cdb[7] = 0; // (bufferlen >> 16) & 0xff; 
		scsi->scsiDetails.Cdb[8] = (bufferlen >> 8) & 0xff;
		scsi->scsiDetails.Cdb[9] = bufferlen & 0xff;
		scsi->scsiDetails.Cdb[10] = 0;
		scsi->scsiDetails.Cdb[11] = 0;
	}

	DWORD n = 0;
	int status = DeviceIoControl(hDev,
		IOCTL_SCSI_PASS_THROUGH_DIRECT,
		scsi, sizeof(SDWA),
		scsi, sizeof(SDWA),
		&n,
		(LPOVERLAPPED)NULL);

	/*
	If the operation completes successfully, the return value is nonzero
	*/
	if (status)
	{
		LOG(D1) << "DeviceIoControl return non-zero status OK " << status;
		//if (scsi->scsiDetails.ScsiStatus) {
			// it only make sense to look sense info if scsi get error
			//printf("scsi->scsiDetails.ScsiStatus = %d and sense info : \n", scsi->scsiDetails.ScsiStatus);
			//DtaHexDump(scsi->sensebytes, 32);
		//}
	}
	else
	{
		IFLOG(D1) {
			printf("*************************************************\n");
			printf("***** DeviceIoControl return zero status NG *****\n");
			printf("*************************************************\n");
		}
		DWORD err = 0;
		err = GetLastError();
		LOG(D1) << "GetLastError " <<  err;
		return DTAERROR_COMMAND_ERROR;
	}
	IFLOG(D4) {
		printf("cdb : after DeviceIoControl\n");
		DtaHexDump(scsi->scsiDetails.Cdb, 16);

		printf("SDBW buffer content : after DeviceIoControl \n");
		DtaHexDump(scsi, sizeof(SDWA));

		printf("data buffer content : after DeviceIoControl ; returned data length = %ld \n", n);
		DtaHexDump(scsi->scsiDetails.DataBuffer, 256);
	}
	return 0;
}


/** adds the IDENTIFY information to the disk_info structure */

void DtaDiskNVME::identify(OPAL_DiskInfo& disk_info)
{
    LOG(D1) << "Entering DtaDiskNVME::identify()";
	vector<uint8_t> nullz(4096, 0x00);
    void * identifyResp = NULL;
	identifyResp = _aligned_malloc(IO_BUFFER_LENGTH, IO_BUFFER_ALIGNMENT);
    if (NULL == identifyResp) return;
    memset(identifyResp, 0, IO_BUFFER_LENGTH);
    uint8_t iorc = sendCmd(IDENTIFY, 0x00, 0x0000, identifyResp, IO_BUFFER_LENGTH);
	LOG(D1) << "iorc=" << hex <<  iorc << " disk_info.devType=" << disk_info.devType;
    // TODO: figure out why iorc = 4
    if ((0x00 != iorc) && (0x04 != iorc)) {
        LOG(D1) << "IDENTIFY Failed " << (uint16_t) iorc;
    }
	if (!(memcmp(identifyResp, nullz.data(), 512))) {
		disk_info.devType = DEVICE_TYPE_OTHER;
		return;
	}
	CScsiCmdInquiry_StandardData * id = (CScsiCmdInquiry_StandardData *) identifyResp;
    disk_info.devType = DEVICE_TYPE_NVME;
	char * strDSN = (char *) malloc(20);
	memset(strDSN, 0x41,20);
	
	if (GetDSN(hDev, strDSN)) {
		LOG(D1) << "GetDSN error";
		return; // error 
	}
	for (int i = 0; i < sizeof(disk_info.serialNum); i += 1) {
		disk_info.serialNum[i] = strDSN[i];
	}

	for (int i = 0; i < sizeof(disk_info.firmwareRev); i += 1) {
		disk_info.firmwareRev[i] = id->m_ProductRevisionLevel[i]; 
	}
	memset(disk_info.modelNum, 0x20, sizeof(disk_info.modelNum));
	for (int i = 0; i < sizeof(id->m_T10VendorId); i += 1) {
		disk_info.modelNum[i] = id->m_T10VendorId[i]; 
	}
	for (int i = 0; i < sizeof(id->m_ProductId); i += 1) {
		disk_info.modelNum[i + sizeof(id->m_T10VendorId)] = id->m_ProductId[i]; 
	}
	_aligned_free(identifyResp);
    return;
}

/** Close the filehandle so this object can be delete. */
DtaDiskNVME::~DtaDiskNVME()
{
    LOG(D1) << "Destroying DtaDiskNVME";
    CloseHandle(hDev);
    _aligned_free(scsiPointer);
}