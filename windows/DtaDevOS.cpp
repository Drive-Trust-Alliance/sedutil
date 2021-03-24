/* C:B**************************************************************************
This software is Copyright 2014-2017 Bright Plaza Inc. <drivetrust@drivetrust.com>

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
#include <vector>
#include "DtaDevOS.h"
#include "DtaEndianFixup.h"
#include "DtaStructures.h"
#include "DtaHexDump.h"
#include "DtaDevGeneric.h"
#include "DtaDiskATA.h"
#include "DtaDiskUSB.h"
#include "DtaDiskNVMe.h"

using namespace std;
DtaDevOS::DtaDevOS() {};
void DtaDevOS::init(const char * devref)
{
    LOG(D1) << "Creating DtaDevOS::DtaDevOS() " << devref;
    dev = devref;
    memset(&disk_info, 0, sizeof (OPAL_DiskInfo));
	/*  Open the drive to see if we have access */
	ATA_PASS_THROUGH_DIRECT * ata =
		(ATA_PASS_THROUGH_DIRECT *)_aligned_malloc(sizeof(ATA_PASS_THROUGH_DIRECT), 8);
	ataPointer = (void *)ata;
	hDev = CreateFile(devref,
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if (INVALID_HANDLE_VALUE == hDev) {
		DWORD err = GetLastError();
		// This is a D1 because diskscan looks for open fail to end scan
		LOG(D1) << "Error opening device " << devref << " Error " << err;
		if (ERROR_ACCESS_DENIED == err) {
			LOG(E) << "You do not have proper authority to access the raw disk";
			LOG(E) << "Try running as Administrator";
		}
	}
	else
	{
		isOpen = 1;
	}
	/*  determine the attachment type of the drive */
	STORAGE_PROPERTY_QUERY query;
	STORAGE_DEVICE_DESCRIPTOR descriptor;
	DWORD BytesReturned;
	query.PropertyId = StorageDeviceProperty;
	query.QueryType = PropertyStandardQuery;

	if (!DeviceIoControl(
		_In_(HANDLE)       hDev,									// handle to a partition
		_In_(DWORD) IOCTL_STORAGE_QUERY_PROPERTY,					// dwIoControlCode
		_In_(LPVOID)       &query,									// input buffer - STORAGE_PROPERTY_QUERY structure
		_In_(DWORD)        sizeof(STORAGE_PROPERTY_QUERY),			// size of input buffer
		_Out_opt_(LPVOID)   &descriptor,							// output buffer - see Remarks
		_In_(DWORD)        sizeof(STORAGE_DEVICE_DESCRIPTOR),		// size of output buffer
		_Out_opt_(LPDWORD)      &BytesReturned,						// number of bytes returned
		_Inout_opt_(LPOVERLAPPED) NULL)) {
		return;
	}
	// OVERLAPPED structure
	switch (descriptor.BusType) {
	case BusTypeAta:
	case BusTypeSata:
		disk = new DtaDiskATA();
		break;
	case BusTypeUsb:
		disk = new DtaDiskUSB();
		break;
	case BusTypeNvme:
		disk = new DtaDiskNVMe();
		break;
	default:
		return;
	}

	disk->init(dev);
    identify(disk_info);
	if (DEVICE_TYPE_OTHER != disk_info.devType) discovery0();
}

uint8_t DtaDevOS::sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                        void * buffer, uint32_t bufferlen)
{
    LOG(D1) << "Entering DtaDevOS::sendCmd";
	LOG(D1) << "Exiting DtaDevOS::sendCmd";
	return (disk->sendCmd(cmd, protocol, comID, buffer, bufferlen));
}

void DtaDevOS::osmsSleep(uint32_t milliseconds)
{
    Sleep(milliseconds);
}
unsigned long long DtaDevOS::getSize() {
	if (DeviceIoControl(
		(HANDLE)hDev,              // handle to device
		IOCTL_DISK_GET_LENGTH_INFO,    // dwIoControlCode
		NULL,                          // lpInBuffer
		0,                             // nInBufferSize
		(LPVOID)&lengthInfo,          // output buffer
		(DWORD)sizeof(GET_LENGTH_INFORMATION),        // size of output buffer
		(LPDWORD)&infoBytesReturned,     // number of bytes returned
		(LPOVERLAPPED)NULL    // OVERLAPPED structure
		)) return (lengthInfo.Length.QuadPart);
	return(0);
}

/** adds the IDENTIFY information to the disk_info structure */

void DtaDevOS::identify(OPAL_DiskInfo& di)
{
    LOG(D1) << "Entering DtaDevOS::identify()";
	LOG(D1) << "Exiting DtaDevOS::identify()";
	return(disk->identify(di));
}
/** Static member to scann for supported drives */
int DtaDevOS::diskScan()
{
	char devname[25];
	int i = 0;
	DtaDev * d;
	LOG(D1) << "Creating diskList";
	printf("\nScanning for Opal compliant disks\n");
	while (TRUE) {
		sprintf_s(devname, 23, "\\\\.\\PhysicalDrive%i", i);
		d = new DtaDevGeneric(devname);
		if (d->isPresent()) {
			printf("%s", devname);
			if (d->isAnySSC())
				printf(" %s%s%s%s%s%s%s ", (d->isOpal1() ? "1" : " "),
				(d->isOpal2() ? "2" : " "), (d->isEprise() ? "E" : " "),
				(d->isOpalite() ? "L" : " "), (d->isPyrite1() ? "p" : " "),
				(d->isPyrite2() ? "P" : " "), (d->isRuby1() ? "r" : " "));
			else
				printf("%s", " No      ");
			cout << d->getModelNum() << " " << d->getFirmwareRev() << std::endl;
			if (MAX_DISKS == i) {
				LOG(I) << MAX_DISKS << " disks, really?";
				delete d;
				return 1;
			}
		}
		else break;
		delete d;
		i += 1;
	}
	delete d;
	printf("No more disks present ending scan\n");
	return 0;
}
/** Close the filehandle so this object can be delete. */

DtaDevOS::~DtaDevOS()
{
    LOG(D1) << "Destroying DtaDevOS";
	delete disk;
	CloseHandle(hDev);
}
