/* C:B**************************************************************************
This software is Copyright © 2014 Michael Romeo <r0m30@r0m30.com>

THIS SOFTWARE IS PROVIDED BY THE AUTHORS ''AS IS'' AND ANY EXPRESS
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

* C:E********************************************************************** */
#include "..\os.h"
#include <stdio.h>
#include <iostream>
#include <Ntddscsi.h>
#include "..\Device.h"
#include "..\Endianfixup.h"
#include "..\TCGStructures.h"
#include "..\HexDump.h"

using namespace std;
/** Device Class (win32) represents a single disk device.
 *  This class is stored in a vector so to avoid writing a 
 *  copy operator and an assignment operator no custom destructor
 *  is used leading to this unfortunate class method structure
 */
Device::Device(TCHAR * devref) {
	ATA_PASS_THROUGH_DIRECT * ata = 
		(ATA_PASS_THROUGH_DIRECT *)_aligned_malloc(sizeof(ATA_PASS_THROUGH_DIRECT),4096);
	ataPointer = (void *)ata;
	
	dev = devref;
	memset(&disk_info, 0, sizeof(TCG_DiskInfo));
	hDev = CreateFile((TCHAR *)dev,
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if (INVALID_HANDLE_VALUE == hDev) {
		DWORD err = GetLastError();
		//printf("\nerror opening device %s Error 0x%04x\n", dev, err);
	}
	else {
		isOpen = TRUE;
		Discovery0();
	}
}

/** Send an ioctl to the device using pass through. */
UINT8 Device::SendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,	
						void * buffer, uint16_t bufferlen) {
	DWORD bytesReturned = 0;                     // data returned
	if (!isOpen) return 0xff; //disk open failed so this will too
	/*
	* Initialize the ATA_PASS_THROUGH_DIRECT structures
	* per windows DOC with the secial sauce from the
	* ATA Command set reference (protocol and commID)
	*/
	ATA_PASS_THROUGH_DIRECT * ata = (ATA_PASS_THROUGH_DIRECT *)ataPointer;
	memset(ata, 0, sizeof(ATA_PASS_THROUGH_DIRECT));
	ata->Length = sizeof(ATA_PASS_THROUGH_DIRECT);
	if (IF_RECV == cmd)
		ata->AtaFlags = 0x00 | ATA_FLAGS_DRDY_REQUIRED | ATA_FLAGS_DATA_IN;
	else
		ata->AtaFlags = 0x00 | ATA_FLAGS_DRDY_REQUIRED | ATA_FLAGS_DATA_OUT;

	ata->DataBuffer = buffer;
	ata->DataTransferLength = bufferlen;
	ata->TimeOutValue = 300;
	/* these were a b**** to find  defined in TCG specs but location is defined
	 * in ATA spec */
	ata->CurrentTaskFile[0] = protocol;             // Protocol
	ata->CurrentTaskFile[1] = int(bufferlen / 512);             // Payload in number of 512 blocks
	// Damn self inflicted endian bugs
	// The comID is passed in host endian format in the taskfile
	// don't know why?? Translated later?
	ata->CurrentTaskFile[3] = (comID & 0x00ff);					// Commid LSB
	ata->CurrentTaskFile[4] = ((comID & 0xff00) >> 8);			// Commid MSB
	ata->CurrentTaskFile[6] = cmd;             // ata Command (0x5e or ox5c)
	//printf("\nata before \n");
	//HexDump(ata, sizeof(ATA_PASS_THROUGH_DIRECT));
	DeviceIoControl(hDev,							// device to be queried
		IOCTL_ATA_PASS_THROUGH_DIRECT,							// operation to perform
		ata, sizeof(ATA_PASS_THROUGH_DIRECT),
		ata, sizeof(ATA_PASS_THROUGH_DIRECT),
		&bytesReturned,											// # bytes returned
		(LPOVERLAPPED)NULL);									// synchronous I/O
	//printf("\nata after \n");
	//HexDump(ata, sizeof(ATA_PASS_THROUGH_DIRECT));
	return(ata->CurrentTaskFile[0]);
}
uint8_t Device::isOpal2() {
	return disk_info.OPAL20;
}
uint8_t Device::isPresent() {
	return isOpen;
}
uint16_t Device::comID() {
	if (disk_info.OPAL20)
		return disk_info.OPAL20_BasecomID;
	else
		return 0x0000;
}
/** Decode the Discovery 0 response.Scans the D0 response and creates structure
* that can be queried later as required.This code also takes care of
* the endianess conversions either via a bitswap in the structure or executing
* a macro when the input buffer is read.
* /
/* TODO: this should throw an exception if a request is made for a field that
* was not returned in the D0 response
*/
void Device::Discovery0()
{
	void * d0Response;
	uint8_t * epos, *cpos;
	Discovery0Header * hdr;
	Discovery0Features * body;
	d0Response = ALIGNED_ALLOC(4096,IO_BUFFER_LENGTH);
	if (NULL != d0Response) {
		memset(d0Response, 0, IO_BUFFER_LENGTH);
		uint8_t iorc = SendCmd(IF_RECV, 0x01, 0x0001, d0Response,
			IO_BUFFER_LENGTH);
		if (0x00 != iorc) {
			ALIGNED_FREE(d0Response);
			return;
		}
	}
	else 
		return;
	epos = cpos = (uint8_t *)d0Response;
	hdr = (Discovery0Header *)d0Response;
	//cout << "\nDumping D0Response" << std::endl;
	//HexDump(hdr, SWAP32(hdr->length));
	epos = epos + SWAP32(hdr->length);
	cpos = cpos + 48;  // TODO: check header version 

	do  {
		body = (Discovery0Features *)cpos;
		//SNPRINTF(fc, 8, "0x%04x", SWAP16(body->TPer.featureCode));
		//SNPRINTF(ver, 8, "0x%02x", body->TPer.version);
		switch (SWAP16(body->TPer.featureCode)) {         /* could use of the structures here is a common field */
		case FC_TPER:					/* TPer */
			disk_info.TPer = 1;
			disk_info.TPer_ACKNACK = body->TPer.acknack;
			disk_info.TPer_ASYNC = body->TPer.Async;
			disk_info.TPer_BufferMgt = body->TPer.bufferManagement;
			disk_info.TPer_comIDMgt = body->TPer.comIDManagement;
			disk_info.TPer_Streaming = body->TPer.streaming;
			disk_info.TPer_SYNC= body->TPer.Sync;
			break;
		case FC_LOCKING:					/* Locking*/
			disk_info.Locking = 1;
			disk_info.Locking_Locked = body->Locking.locked;
			disk_info.Locking_LockingEnabled = body->Locking.lockingEnabled;
			disk_info.Locking_LockingSupported = body->Locking.LockingSupported;
			disk_info.Locking_MBRDone = body->Locking.MBRDone;
			disk_info.Locking_MBREnabled = body->Locking.MBREnabled;
			disk_info.Locking_MediaEncrypt = body->Locking.mediaEncryption;
			break;
		case FC_GEOMETRY:					/* Geometry Features */
			disk_info.Geometry = 1;
			disk_info.Geometry_Align = body->Geometry.align;
			disk_info.Geometry_AlignmentGranularity = SWAP32(body->Geometry.alignmentGranularity);
			disk_info.Geometry_LogicalBlockSize = SWAP32(body->Geometry.logicalBlockSize);
			disk_info.Geometry_LowestAlignedLBA = SWAP32(body->Geometry.lowestAlighedLBA);
			break;
		case FC_ENTERPRISE:					/* Enterprise SSC */
			disk_info.Enterprise = 1;
			disk_info.Enterprise_RangeCrossing = body->EnterpriseSSC.rangeCrossing;
			disk_info.Enterprise_BasecomID = SWAP16(body->EnterpriseSSC.baseComID);
			disk_info.Enterprise_NumcomID = SWAP16(body->EnterpriseSSC.numberComIDs);
			break;
		case FC_SINGLEUSER:					/* Single User Mode */
			disk_info.SingleUser = 1;
			disk_info.SingleUser_ALL = body->SingleUserMode.all;
			disk_info.SingleUser_ANY = body->SingleUserMode.any;
			disk_info.SingleUser_Policy = body->SingleUserMode.policy;
			disk_info.SingleUser_LockingObjects = SWAP32(body->SingleUserMode.numberLockingObjects);
			break;
		case FC_DATASTORE:					/* Datastore Tables */
			disk_info.DataStore = 1;
			disk_info.DataStore_MaxTables = SWAP16(body->Datastore.maxTables);
			disk_info.DataStore_MaxTableSize = SWAP32(body->Datastore.maxSizeTables);
			disk_info.DataStore_Alignment = SWAP32(body->Datastore.tableSizeAlignment);
			break;
		case FC_OPALV200:					/* OPAL V200 */
			disk_info.OPAL20 = 1;
			disk_info.OPAL20_BasecomID = SWAP16(body->OPALv200.baseCommID);
			disk_info.OPAL20_InitialPIN = body->OPALv200.initialPin;
			disk_info.OPAL20_RevertedPIN = body->OPALv200.revertedPin;
			disk_info.OPAL20_NumcomIDs = SWAP16(body->OPALv200.numCommIDs);
			disk_info.OPAL20_NumAdmins = SWAP16(body->OPALv200.numlockingAdminAuth);
			disk_info.OPAL20_NumUsers = SWAP16(body->OPALv200.numlockingUserAuth);
			disk_info.OPAL20_RangeCrossing = body->OPALv200.rangeCrossing;
			break;
		default:
			disk_info.Unknown += 1;
			/* should do something here */
			break;
		}
		cpos = cpos + (body->TPer.length + 4);
	} while (cpos < epos);
	ALIGNED_FREE(d0Response);
}

/** Print out the Descovery 0 results */
void Device::Puke() {
	char scratch[25];
	/* TPer */
	if (disk_info.TPer) {
		printf("\nTPer funcion (0x%04x)\n", FC_TPER);
		cout << "\nTPer functions  (" << std::hex << FC_TPER << std::dec << ")" << std::endl;
		cout << "ACKNAK           = " << (disk_info.TPer_ACKNACK ? "Y" : "N") << std::endl;
		cout << "ASYNC            = " << (disk_info.TPer_ASYNC ? "Y" : "N") << std::endl;
		cout << "BufferManagement = " << (disk_info.TPer_BufferMgt ? "Y" : "N") << std::endl;
		cout << "comIDManagement  = " << (disk_info.TPer_comIDMgt ? "Y" : "N") << std::endl;
		cout << "Streaming        = " << (disk_info.TPer_Streaming ? "Y" : "N") << std::endl;
		cout << "SYNC             = " << (disk_info.TPer_SYNC ? "Y" : "N") << std::endl;
		cout << std::endl;
	}
	if (disk_info.Locking) {
		printf("\nLocking functions (0x%04x)\n", FC_LOCKING);
		cout << "Locked             = " << (disk_info.Locking_Locked ? "Y" : "N") << std::endl;
		cout << "LockingEnabled     = " << (disk_info.Locking_LockingEnabled ? "Y" : "N") << std::endl;
		cout << "LockingSupported   = " << (disk_info.Locking_LockingSupported ? "Y" : "N") << std::endl;
		cout << "MBRDone            = " << (disk_info.Locking_MBRDone ? "Y" : "N") << std::endl;
		cout << "MBREnabled         = " << (disk_info.Locking_MBREnabled ? "Y" : "N") << std::endl;
		cout << "MediaEncrypt       = " << (disk_info.Locking_MediaEncrypt ? "Y" : "N") << std::endl;
		cout << std::endl;
	}
	if (disk_info.Geometry) {
		printf("\nGeometry functions (0x%04x)\n", FC_GEOMETRY);
		cout << "Align                 = " << (disk_info.Geometry_Align ? "Y" : "N") << std::endl;
		cout << "Alignment Granularity = " << disk_info.Geometry_AlignmentGranularity << std::endl;
		cout << "Logical Block size    = " << disk_info.Geometry_LogicalBlockSize << std::endl;
		cout << "Lowest Aligned LBA    = " << disk_info.Geometry_LowestAlignedLBA << std::endl;
	}
	if (disk_info.Enterprise) {
		printf("\nEnterprise functions (0x%04x)\n", FC_ENTERPRISE);
		cout << "Range crossing      = " << (disk_info.Enterprise_RangeCrossing ? "Y" : "N") << std::endl;
		cout << "Base commID         = " << disk_info.Enterprise_BasecomID << std::endl;
		cout << "Number of commIDs   = " << disk_info.Enterprise_NumcomID << std::endl;
	}
	if (disk_info.SingleUser) {
		printf("\nSingleUser functions (0x%04x)\n", FC_SINGLEUSER);
		cout << "ALL                         = " << (disk_info.SingleUser_ALL ? "Y" : "N") << std::endl;
		cout << "ANY                         = " << (disk_info.SingleUser_ANY ? "Y" : "N") << std::endl;
		cout << "Policy                      = " << (disk_info.SingleUser_Policy ? "Y" : "N") << std::endl;
		cout << "Number of Locking Objects   = " << (disk_info.SingleUser_LockingObjects) << std::endl;
	}
	if (disk_info.DataStore) {
		printf("\nDataStore functions (0x%04x)\n", FC_DATASTORE);
		cout << "Max Tables             = " << disk_info.DataStore_MaxTables << std::endl;
		cout << "Max Size Tables        = " << disk_info.DataStore_MaxTableSize << std::endl;
		cout << "Table size alignment   = " << disk_info.DataStore_Alignment << std::endl;
	}
	if (disk_info.OPAL20) {
		printf("\nOPAL 2.0 functions (0x%04x)\n", FC_OPALV200);
		SNPRINTF(scratch, 8, "0x%04x", disk_info.OPAL20_BasecomID);
		cout << "Base commID                = " << scratch << std::endl;
		SNPRINTF(scratch, 8, "0x%02x", disk_info.OPAL20_InitialPIN);
		cout << "Initial PIN                = " << scratch << std::endl;
		SNPRINTF(scratch, 8, "0x%02x", disk_info.OPAL20_RevertedPIN);
		cout << "Reverted PIN               = " << scratch << std::endl;
		cout << "Number of commIDs          = " << disk_info.OPAL20_NumcomIDs << std::endl;
		cout << "Number of Locking Admins   = " << disk_info.OPAL20_NumAdmins << std::endl;
		cout << "Number of Locking Users    = " << disk_info.OPAL20_NumUsers << std::endl;
		cout << "Range Crossing             = " << (disk_info.OPAL20_RangeCrossing ? "Y" : "N") << std::endl;
	}
	if (disk_info.Unknown)
		cout << disk_info.Unknown << " unknown function codes IGNORED " << std::endl;
	}
/** Close the filehandle so this object can be delete. */
Device::~Device(){
	CloseHandle(hDev);
	_aligned_free(ataPointer);
}