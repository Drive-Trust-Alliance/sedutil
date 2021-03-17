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
#include <vector>
#include "DtaDevOS.h"
#include "DtaEndianFixup.h"
#include "DtaStructures.h"
#include "DtaHexDump.h"
#include "DtaDiskATA.h"
#include "DtaDiskUSB.h"
#include "DtaDiskNVME.h"

using namespace std;
DtaDevOS::DtaDevOS() { LOG(D1) << "Entering DtaDevOS Constructor"; };
void DtaDevOS::init(const char * devref)
{

	LOG(D1) << "Creating DtaDevOS::DtaDevOS() " << devref;
	dev = devref;
	memset(&disk_info, 0, sizeof(OPAL_DiskInfo));
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
		cout << endl;
		//LOG(E) << "Can not determine the device type";
		return;
	}
	LOG(D1) << "descriptor.BusType = " << descriptor.BusType << "\n";
	// OVERLAPPED structure
	switch (descriptor.BusType) {
	case BusTypeSata:
		LOG(D1) << "Enter Sata bus type case";
		disk = new DtaDiskATA();
		LOG(D1) << "Return from DtaDiskATA bus type case";
		break;
	case BusTypeUsb:
		LOG(D1) << "Enter USB bus type case";
		disk = new DtaDiskUSB();
		// now nvme is potentially on USB bus
		disk->init(dev);
		/////////////
		LOG(D) << "Entering USB DtaDevOS::identify()";
		identify(disk_info); // will come back either it is SATA or Nvme
		LOG(D) << "Exiting USB returning DtaDevOS::identify()";
		//uint8_t modelNum[40];
		if (strlen((char *)disk_info.modelNum) == 0) // only usb dongle or non-sata non-nvme will return nothing here
		{
			// Not SATA, Not Nvme
		}
		else {
			// do discovery0 
			disk_info.devType = DEVICE_TYPE_USB;
			discovery0(&disc0Sts);
			return;
		}
		// check  if nvme
		//DoIdentifyDeviceNVMeASMedia(INT physicalDriveId, INT scsiPort, INT scsiTargetId, IDENTIFY_DEVICE* data);
		LOG(D) << "Entering USB DtaDevOS::identifyNVMeASMedia()";
		
		identifyNVMeASMedia(disk_info);
		if (strlen((char *)disk_info.modelNum) == 0) // only usb dongle or non-sata non-nvme will return nothing here
		{
			// Not SATA, Not Nvme
		} else {
			// do discovery0 
			
			
			delete disk;
			disk = new DtaDiskNVME();
			disk->init(dev);

			identifyNVMeASMedia(disk_info);
			disk_info.devType = DEVICE_TYPE_NVME;
			discovery0(&disc0Sts);
			return;
		return;
		}
		
		// assume it is nvme
			/*
		delete disk;
		disk = new DtaDiskNVME();
		disk->init(dev);
		identify(disk_info);
		if (strlen((char *)disk_info.modelNum) == 0) // only usb dongle or non-sata non-nvme will return nothing here
		{
			// Not SATA, Not Nvme
		}
		else {
			// do discovery0 
			disk_info.devType = DEVICE_TYPE_NVME;
			discovery0(&disc0Sts);
			return;
		}*/
		// doesn't matter any more since no nvme no ata


		break;
	case BusTypeNvme:
		LOG(D1) << "Enter Nvme bus type case";
		disk = new DtaDiskNVME();
		break;
	case BusTypeRAID:
		LOG(D1) << "Enter RAID bus type case";
		disk = new DtaDiskUSB(); 
		disk->init(dev);
		/////////////
		LOG(D) << "Entering USB DtaDevOS::identify()";
		identify(disk_info); // will come back either it is SATA or Nvme
		LOG(D) << "Exiting USB returning DtaDevOS::identify()";
		//uint8_t modelNum[40];
		if (strlen((char *)disk_info.modelNum) == 0) // only usb dongle or non-sata non-nvme will return nothing here
		{
			// Not SATA, Not Nvme
		}
		else {
			// do discovery0 
			disk_info.devType = DEVICE_TYPE_ATA;
			discovery0(&disc0Sts);
			return;
		}
		// what the discovery0 say, invalid or non-TCG/TCG
		LOG(D) << "Entering USB DtaDevOS::identifyPd()";
		identifyPd(disk_info);
		LOG(D) << "Exiting USB DtaDevOS::identifyPd()";
		if (strlen((char *)disk_info.modelNum) == 0) // only usb dongle or non-sata non-nvme will return nothing here
		{
			// Not SATA, Not Nvme
			disk_info.devType = DEVICE_TYPE_OTHER;
		}
		else {
			// do discovery0 
			// Not SATA , potential Nvme
			disk_info.devType = DEVICE_TYPE_NVME;
			LOG(D) << "USB DtaDevOS::identifyPd() OK"; // need to be done in nvme
			// usb device discovery0 command OK?   
			delete disk;
			disk = new DtaDiskNVME();
			disk->init(dev);
			discovery0(&disc0Sts);
			LOG(D) << "end of USB DtaDevOS::discovery0()";
			return;
		}
		// otherwise set it as USB type
		delete disk;
		disk = new DtaDiskUSB();
		disk->init(dev);
		break; // not SATA , not nvme lease it open 





		/*
		if (disk_info.devType == DEVICE_TYPE_OTHER)
		{
			LOG(D) << "Device on RAID is not identified as USB SATA";
			delete disk;
			disk = new DtaDiskNVME();
			disk->init(dev);
			identify(disk_info);
			if (disk_info.devType == DEVICE_TYPE_OTHER)
			{
				LOG(D) << "Device on RAID is not identified as nvme";
				delete disk; 
				disk = new DtaDiskUSB; // assume USB bus even it can not be identified
				disk_info.devType = DEVICE_TYPE_USB;
				break; 
			}
		}
		else {
			LOG(D) << "Device on RAID is identified at first galance but it can be a SATA or Nvme";
			// find out if it is discovery work for SATA or Nvme
			// discovery zero send disk->sendCmd(cmd, protocol, comID, buffer, bufferlen
			// disk-> must point to either SATA or Nvme
			discovery0(&disc0Sts);
			if (!disc0Sts) {
				disk_info.devType = DEVICE_TYPE_USB;
				break; // OK
			}
			else { // try nvme again 
				delete disk;
				disk = new DtaDiskNVME();
				disk->init(dev);
				discovery0(&disc0Sts);
				disk_info.devType = DEVICE_TYPE_NVME;
			}
		}*/
		break;
	case BusTypeSas:
		LOG(D1) << "Enter Sas bus type case";
		disk = new DtaDiskUSB();
		break;
	default:
		LOG(D) << "Unknown bus Type on system storage";
		delete disk;
		return;
	}
	if (descriptor.BusType != BusTypeRAID) {
		LOG(D1) << "Before Entering disk->init";
		disk->init(dev);
		LOG(D1) << "Before Entering identify(disk_info)";
	}

	uint8_t geometry[256];
	uint32_t disksz;
	PDISK_GEOMETRY_EX DiskGeometry = (PDISK_GEOMETRY_EX)(void*)geometry; // defined in winioctl.h
	BOOL r = 0;																	 // double check if it is a usb thumb drive, ignore discovery0()
	if (!(r = DeviceIoControl(hDev, IOCTL_DISK_GET_DRIVE_GEOMETRY_EX,
		NULL, 0, geometry, sizeof(geometry), &BytesReturned, NULL)))
	{
		LOG(D1) << "IOCTL_DISK_GET_DRIVE_GEOMETRY_EX error"  ; 
		LOG(D1) << "Pyrite return error for some reason (when locked), ignore error for now"; 
		// return; // jerry pyrite return error for geometry info ????
	}
	/*
	printf("Disk size = %I64d\n", DiskGeometry->DiskSize.QuadPart);
	printf("Disk byte per sector = %ld\n",DiskGeometry->Geometry.BytesPerSector);
	printf("Disk sector per track = %ld\n", DiskGeometry->Geometry.SectorsPerTrack);
	printf("Disk track per cylinder = %ld\n", DiskGeometry->Geometry.TracksPerCylinder);
	printf("Disk media type = %ld\n", DiskGeometry->Geometry.MediaType);
	//LARGE_INTEGER ??
	printf("Disk total cylinder = %ld\n", DiskGeometry->Geometry.Cylinders);
	//printf("Calculate the disk size (MB) = %ld\n", disksz);
	*/
	disksz = (DiskGeometry->Geometry.Cylinders.LowPart *
		DiskGeometry->Geometry.TracksPerCylinder *
		DiskGeometry->Geometry.SectorsPerTrack) / 2000; // size in MB
	if ((disksz > (32 * 1000)) || (descriptor.BusType != BusTypeUsb)) { // only > 32GB and not usb-bus consider as SSD otherwise treat it as usb thumb
		identify(disk_info);
	}
	else {
		LOG(D) << "disksz is less than 32GB treat it as flash usb drive";
	}

	LOG(D) << "Before Entering disk->init";
	if (DEVICE_TYPE_OTHER != disk_info.devType)
	{
		if ((disksz > (32 * 1000)) || (descriptor.BusType != BusTypeUsb)) // only > 32GB consider as SSD otherwise treat it as usb thumb
			if(descriptor.BusType != BusTypeRAID) discovery0(&disc0Sts);
	}
}

uint8_t DtaDevOS::sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID, void * buffer, uint32_t bufferlen)
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
	return(disk->identify(di)); 
}
void DtaDevOS::identifyPd(OPAL_DiskInfo& di)
{
	return(disk->identifyPd(di));
}

void DtaDevOS::identifyNVMeASMedia(OPAL_DiskInfo& di)
{
	return(disk->identifyNVMeASMedia(di));
}

/** Close the filehandle so this object can be delete. */

DtaDevOS::~DtaDevOS()
{
    LOG(D1) << "Destroying DtaDevOS";
	delete disk;
	CloseHandle(hDev);
}