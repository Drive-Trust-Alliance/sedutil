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
#include "nvme.h"
#include "nvmeIoctl.h"
using namespace std;
DtaDiskNVME::DtaDiskNVME() { LOG(D1) << "DtaDiskNVME Constructor"; };
void DtaDiskNVME::init(const char * devref)
{/*
 	UCHAR                   myIoctlBuf[sizeof(NVME_PASS_THROUGH_IOCTL) +
	   	                               sizeof(ADMIN_IDENTIFY_CONTROLLER)];

    PNVME_PASS_THROUGH_IOCTL     pMyIoctl = (PNVME_PASS_THROUGH_IOCTL)myIoctlBuf;

	SDWB * scsi =
	(SDWB *) _aligned_malloc((sizeof (SDWB)), 4096);
	scsiPointer = (void *) scsi;
 */
	// didn't allocate memory for iocontrol, why blue screen after iocontrol
	// same as original code 
	PNVME_PASS_THROUGH_IOCTL nvme = (PNVME_PASS_THROUGH_IOCTL) _aligned_malloc((sizeof(NVME_PASS_THROUGH_IOCTL)+ IO_BUFFER_LENGTH), IO_BUFFER_ALIGNMENT);
	LOG(D1) << "Creating DtaDiskNVME::DtaDiskNVME() " << devref;
	nvmePointer = (void *)nvme;

	// convert \\.\PhysicalDriveXX to ScsiXX
	LPCTSTR	pszFormat = "%s%d:";
	TCHAR str2[64] ;
	LPTSTR pszTxt = "\\\\.\\Scsi";
	int scsiPort = int(devref[17]) - 0x30;
	//DtaHexDump(pszTxt, 64);
	wsprintf(str2, pszFormat, pszTxt, scsiPort); // ":" append to str2 make it work ?
	//LOG(D1) << "after wsprintf" << endl;
	//DtaHexDump(str2, 64); 
	//LOG(D1) << "str2=" << str2 << endl;


	LOG(D1) << "devref : " << devref << endl;
	LOG(D1) << "size of devref : " << sizeof(devref) << " size str2 : " << sizeof(str2) ;
	LOG(D1) << "new SCSI string  : " << str2 << endl ;
	//DtaHexDump(str2, 64);
	
    hDev = CreateFile(str2,
                      GENERIC_WRITE | GENERIC_READ,
                      FILE_SHARE_WRITE | FILE_SHARE_READ,
                      NULL,
                      OPEN_EXISTING,
					  FILE_ATTRIBUTE_NORMAL,  // 0,
                      NULL);
	
	if (INVALID_HANDLE_VALUE == hDev) {
		LOG(D) << "Open SCSI port fail. hDev = " << hex << hDev << endl;
		return;
	}
    else {
		LOG(D) << "Open SCSI port OK. hDev = " << hex << hDev << endl;
        isOpen = TRUE;
	}
}
uint8_t DtaDiskNVME::sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                        void * buffer, uint16_t bufferlen)
{
	PNVMe_COMMAND  pCmd;
	DWORD dw;
	BOOL b;
	NVME_PASS_THROUGH_IOCTL * nvme = (NVME_PASS_THROUGH_IOCTL *)nvmePointer;
	//PADMIN_SECURITY_RECEIVE_COMMAND_DW10 dw10;
	//PADMIN_SECURITY_RECEIVE_COMMAND_DW11 dw11;

    LOG(D1) << "Entering DtaDiskNVME::sendCmd";
	//getchar();
    DWORD bytesReturned = 0; // data returned
    if (!isOpen) {
        LOG(D1) << "Device open failed";
		return DTAERROR_OPEN_ERR; //disk open failed so this will too
    }
    /*
     * Initialize the NVME_PASS_THROUGH_DIRECT structures
     * per windows DOC with the special sauce from the
     * NVME Command set reference (protocol and comID)
     */
	
	//NVME_PASS_THROUGH_IOCTL * nvme = (NVME_PASS_THROUGH_IOCTL *) nvmePointer;
	//LOG(D1) << "before memset\n"; //ok
	//getchar();
	memset(nvme, 0, sizeof (NVME_PASS_THROUGH_IOCTL)+ IO_BUFFER_LENGTH);
	//LOG(D1) << "after memset\n"; //ok
	//getchar();

    ///////////////////////////////////////////////////////////////////////
	nvme->SrbIoCtrl.ControlCode = (ULONG)NVME_PASS_THROUGH_SRB_IO_CODE;
	memcpy(nvme->SrbIoCtrl.Signature, NVME_SIG_STR, sizeof(NVME_SIG_STR));
	nvme->SrbIoCtrl.HeaderLength = (ULONG) sizeof(SRB_IO_CONTROL);
	nvme->SrbIoCtrl.Timeout = 30;
	nvme->SrbIoCtrl.Length = sizeof(NVME_PASS_THROUGH_IOCTL) - sizeof(SRB_IO_CONTROL);

	// Set up the NVMe pass through IOCTL buffer
	// memcpy(&(pMyIoctl->NVMeCmd), &nvmeCmd, sizeof(nvmeCmd));
	//(nvme->DataBuffer) = buffer;  // void * buffer , nvme->DataBuffer[0]
	// DataBuffer must follow DataBuffer[0] location.
	// after IOCTRL we can move DataBuffer to buffer where the caller's buffer 

	pCmd = (PNVMe_COMMAND)nvme->NVMeCmd; //PNVMe_COMMAND

	if (IF_RECV == cmd) {
		LOG(D1) << "cmd = Security Receive" << endl;
		pCmd->CDW0.OPC = ADMIN_SECURITY_RECEIVE;
		nvme->Direction = NVME_FROM_DEV_TO_HOST;
		//pCmd->NSID = 1; // NSID
		//dw10 = (PADMIN_SECURITY_RECEIVE_COMMAND_DW10)&(pCmd->CDW10);
		//dw10->SECP = 1; // Protocol located LSB3
		//dw10->SPSP = 1; // only for level 0 discovery ; comID 2-byte 
						// data pointer to data buffer pMyIoctl->DataBuffer shall be at RPR0
		pCmd->PRP1 = (ULONGLONG)(nvme->DataBuffer); // set the return data buffer pointer in CDW command
		pCmd->CDW9 = bufferlen;
		pCmd->CDW10 = protocol << 24 | comID << 8;
		pCmd->CDW11 = bufferlen;
		//pCmd->PRP1 = (ULONGLONG)buffer; //cdw6,7
		memset(nvme->DataBuffer, 0x55, 4096);
		nvme->DataBufferLen = 0; // ????? maybe it doesn't matter for data in command, but for data out, it should be the length of the data out ????

	}
	else if (IDENTIFY == cmd) {
		/* IDENTIFY only need program 3 place
		pCmd->CDW0.OPC = ADMIN_IDENTIFY;
		dw10 = (PADMIN_IDENTIFY_COMMAND_DW10)&(pCmd->CDW10);
		dw10->CNS = 1;
		*/
		//===============================
		LOG(D1) << "cmd = IDENTIFY" << endl;
		pCmd->CDW0.OPC = ADMIN_IDENTIFY;
		nvme->Direction = NVME_FROM_DEV_TO_HOST;
		//pCmd->NSID = 1; // cdw1 is nsid, 1=identify ????? 
		pCmd->CDW10 = 1; // controller name space 
		//pCmd->PRP1 = (ULONGLONG)(nvme->DataBuffer); // set the return data buffer pointer in CDW command // JERRY
		memset(nvme->DataBuffer, 0x55, 4096);
		nvme->DataBufferLen = 0; // 4096;NG // 0 NG; // ????? maybe it doesn't matter for data in command, but for data out, it should be the length of the data out ????

	}
	else {
		// security send command
		LOG(D1) << "cmd = Security Send" << endl;
		pCmd->CDW0.OPC = ADMIN_SECURITY_SEND;
		nvme->Direction = NVME_FROM_HOST_TO_DEV;
		pCmd->PRP1 = (ULONGLONG)(nvme->DataBuffer); // set the data buffer pointer in CDW command
		pCmd->CDW9 = bufferlen; // 0x800;ok, try bufferlen // see which bufferlen work // bufferlen;
		pCmd->CDW10 = protocol << 24 | comID << 8;
		pCmd->CDW11 = bufferlen; //  0x800 ok; // bufferlen;
		memcpy(nvme->DataBuffer, buffer, bufferlen); //0x800 ok ); // move data from caller's buffer to deviceiocontrol buffer ???? temp 4096 may change to 8192
		nvme->DataBufferLen = bufferlen ; // ????? maybe it doesn't matter for data in command, but for data out, it should be the length of the data out ????
	}
	
	nvme->QueueId = 0; // Admin queue
	//nvme->DataBufferLen = 0; // ????? maybe it doesn't matter for data in command, but for data out, it should be the length of the data out ????
	nvme->ReturnBufferLen = sizeof(NVME_PASS_THROUGH_IOCTL) + bufferlen ;  //sizeof(ADMIN_IDENTIFY_CONTROLLER)
		
	nvme->VendorSpecific[0] = (DWORD)0;
	nvme->VendorSpecific[1] = (DWORD)0;
	LOG(D1) << "*****Dump NVMe command buffer*****" << endl;
	IFLOG(D4) DtaHexDump(pCmd, sizeof(nvme->NVMeCmd));
	//LOG(D1) << "b4 memset DataBuffer" << endl;
	//getchar();//ok
	
	//////////////////////////////////////////////////////////////////////

	//LOG(D1) << "after memset DataBuffer" << endl;
	//getchar(); //ok
    //LOG(D4) << "nvme before";
    //IFLOG(D4) hexDump(nvme, sizeof (NVME_PASS_THROUGH_DIRECT));
	/*
		b = DeviceIoControl(hand,
		IOCTL_SCSI_MINIPORT,
		pMyIoctl,
		sizeof(myIoctlBuf), 
		pMyIoctl,
		sizeof(myIoctlBuf),
		&dwlen,
		NULL);
	*/
    b = DeviceIoControl(hDev, // device to be queried
					IOCTL_SCSI_MINIPORT, // operation to perform
                    nvme,  sizeof (NVME_PASS_THROUGH_IOCTL)+ IO_BUFFER_LENGTH, //bufferlen+
                    nvme,  sizeof (NVME_PASS_THROUGH_IOCTL)+ IO_BUFFER_LENGTH, //bufferlen,
                    &bytesReturned, // # bytes returned
                    (LPOVERLAPPED) NULL); // synchronous I/O
	LOG(D1) << "after iocontrol" << endl; // dead here 
	//getchar();
	if (FALSE == b) {
		switch (cmd) {
			case IDENTIFY:
				LOG(D1) << "IdentifyController: NL_IOCTL_IDENTIFY: FAIL\n";
				break;
			case IF_RECV :
				LOG(D1) << "Security Receive Command FAIL\n";
				break;
			default: 
				LOG(D1) << "Security Send Command FAIL\n";
		}
		dw = GetLastError();
		LOG(D1) << "Did find err " << dw << endl;
		return DTAERROR_COMMAND_ERROR;
	}
	else {
		switch (cmd) {
			case IDENTIFY:
				LOG(D1) << "IdentifyController: NL_IOCTL_IDENTIFY: SUCCESS!!!\n";
				LOG(D1) << "IDENTIFY Data Buffer nvme->DataBuffer: " << endl;
				IFLOG(D4) DtaHexDump(nvme->DataBuffer, 64); //DEBUG
				
				memcpy(buffer, nvme->DataBuffer, 4096); // move data to caller's buffer ??? 
				LOG(D1) << "IDENTIFY Data Buffer after move to caller buffer: " << endl;
				IFLOG(D4) DtaHexDump(buffer, 64); //DEBUG
				//DtaHexDump(buffer, 1024); //DEBUG
				break;
			case IF_RECV:
				LOG(D1) << "Security Receive Command SUCCESS!!!\n";
				memcpy(buffer, nvme->DataBuffer, 4096); // move data to caller's buffer ??? 
				break;
			default:
				LOG(D1) << "Security Send Command SUCCESS!!!\n";
		}
		return 0;
	}


}

/** adds the IDENTIFY information to the disk_info structure */

void DtaDiskNVME::identify(OPAL_DiskInfo& disk_info)
{
    LOG(D1) << "Entering DtaDiskNVME::identify()";
	vector<uint8_t> nullz(4096, 0x00);
    void * identifyResp = NULL;
/*
// redefined in COnstants.h to multiple of 4096
#ifdef IO_BUFFER_LENGTH
#undef IO_BUFFER_LENGTH
#endif

#ifdef IO_BUFFER_ALIGNMENT
#undef IO_BUFFER_ALIGNMENT
#endif

#define IO_BUFFER_LENGTH 4096
#define IO_BUFFER_ALIGNMENT 4096
*/

	/*
	** Response returned by ATA Identify //
	typedef struct _IDENTIFY_RESPONSE {
	uint8_t reserved0;
	uint8_t reserved1 : 7;
	uint8_t devType : 1;
	uint8_t reserved2[18];
	uint8_t serialNum[20];
	uint8_t reserved3[6];
	uint8_t firmwareRev[8];
	uint8_t modelNum[40];
	} IDENTIFY_RESPONSE;

	*/

	/* NVME identify response

	4096-byte ADMIN_IDENTIFY_CONTROLLER

	*/

#ifdef IDENTIFY_RESPONSE 
#undef IDENTIFY_RESPONSE 
#endif

#define IDENTIFY_RESPONSE ADMIN_IDENTIFY_CONTROLLER

	/* short discription of struct 
	* Identify Controller Data Structure, Section 5.11, Figure 65 
	typedef struct _ADMIN_IDENTIFY_CONTROLLER
	{
		USHORT  VID;
		USHORT  SSVID;
		UCHAR   SN[20];
		UCHAR   MN[40];
		UCHAR   FR[8];
		UCHAR   RAB;
		.......
	}
	*/

	identifyResp = _aligned_malloc(IO_BUFFER_LENGTH, IO_BUFFER_ALIGNMENT);
    if (NULL == identifyResp) return;
    memset(identifyResp, 0, IO_BUFFER_LENGTH);
    uint8_t iorc = sendCmd(IDENTIFY, 0x00, 0x0000, identifyResp, IO_BUFFER_LENGTH);
	LOG(D1) << "iorc=" << hex <<  iorc << " disk_info.devType=" << disk_info.devType << endl;
    // TODO: figure out why iorc = 4
    if ((0x00 != iorc) && (0x04 != iorc)) {
        LOG(D1) << "IDENTIFY Failed " << (uint16_t) iorc;
        //ALIGNED_FREE(identifyResp);
        //return;
    }
	if (!(memcmp(identifyResp, nullz.data(), 512))) {
		disk_info.devType = DEVICE_TYPE_OTHER;
		return;
	}
	ADMIN_IDENTIFY_CONTROLLER * id = (ADMIN_IDENTIFY_CONTROLLER *) identifyResp;
    disk_info.devType = DEVICE_TYPE_NVME;
    for (int i = 0; i < sizeof (disk_info.serialNum); i += 1) {
		disk_info.serialNum[i] = id->serialNum[i];
     }
    for (int i = 0; i < sizeof (disk_info.firmwareRev); i += 1) {
		disk_info.firmwareRev[i] = id->firmwareRev[i];
    }
    for (int i = 0; i < sizeof (disk_info.modelNum); i += 1) {
		disk_info.modelNum[i] = id->modelNum[i];
    }
	_aligned_free(identifyResp);
    return;
}

/** Close the filehandle so this object can be delete. */
DtaDiskNVME::~DtaDiskNVME()
{
    LOG(D1) << "Destroying DtaDiskNVME";
    CloseHandle(hDev);
    _aligned_free(nvmePointer);
}