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
/** Base device class.
 * An OS port must create a subclass of this class
 * implementing sendcmd, osmsSleep and identify 
 * specific to the IO requirements of that OS
 */
#include "os.h"
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include "DtaDev.h"
#include "DtaStructures.h"
#include "DtaConstants.h"
#include "DtaEndianFixup.h"
#include "DtaHexDump.h"

using namespace std;

/** Device Class (Base) represents a single disk device.
 *  This is the functionality that is common to all OS's and SSC's
 */
DtaDev::DtaDev()
{
}
DtaDev::~DtaDev()
{
}

uint8_t DtaDev::isRuby()
{
	LOG(D1) << "Entering DtaDev::isRuby " << (uint16_t)disk_info.RUBY;
	return disk_info.RUBY;
}

uint8_t DtaDev::isOpalite()
{
	LOG(D1) << "Entering DtaDev::isOpalite " << (uint16_t) disk_info.OPALITE;
	return disk_info.OPALITE;
}
uint8_t DtaDev::isPyrite2()
{
	LOG(D1) << "Entering DtaDev::isPyrite2 " << (uint16_t)disk_info.PYRITE2;
	return disk_info.PYRITE2;
}
uint8_t DtaDev::isPyrite()
{
	LOG(D1) << "Entering DtaDev::isPyrite " << (uint16_t) disk_info.PYRITE;
	return disk_info.PYRITE;
}
uint8_t DtaDev::isOpal2()
{
	LOG(D1) << "Entering DtaDev::isOpal2 " << (uint16_t) disk_info.OPAL20;
	return disk_info.OPAL20;
}
uint8_t DtaDev::isOpal1()
{
	LOG(D1) << "Entering DtaDev::isOpal1() " << (uint16_t)disk_info.OPAL10;
    return disk_info.OPAL10;
}
uint8_t DtaDev::isEprise()
{
    LOG(D1) << "Entering DtaDev::isEprise " << (uint16_t) disk_info.Enterprise;
    return disk_info.Enterprise;
}

uint8_t DtaDev::isAnySSC()
{
	LOG(D1) << "Entering DtaDev::isAnySSC " << (uint16_t)disk_info.ANY_OPAL_SSC;
	return disk_info.ANY_OPAL_SSC;
}
uint8_t DtaDev::isPresent()
{
	LOG(D1) << "Entering DtaDev::isPresent() " << (uint16_t) isOpen;
    return isOpen;
}
uint8_t DtaDev::isNVMEbus()
{
	LOG(D1) << "Entering DtaDev::isNVMEbus() " << (uint16_t)isNVME;
	return isNVME;
}
uint8_t DtaDev::MBREnabled()
{
	LOG(D1) << "Entering DtaDev::MBRENabled" << (uint16_t)disk_info.Locking_MBREnabled;
	return disk_info.Locking_MBREnabled;
}
uint8_t DtaDev::MBRDone()
{
	LOG(D1) << "Entering DtaDev::MBRDone" << (uint16_t)disk_info.Locking_MBRDone;
	return disk_info.Locking_MBRDone;
}
uint8_t DtaDev::Locked()
{
	LOG(D1) << "Entering DtaDev::Locked" << (uint16_t)disk_info.Locking_locked;
	return disk_info.Locking_locked;
}
uint8_t DtaDev::LockingEnabled()
{
	LOG(D1) << "Entering DtaDev::LockingEnabled" << (uint16_t)disk_info.Locking_lockingEnabled;
	return disk_info.Locking_lockingEnabled;
}
char *DtaDev::getFirmwareRev()
{
	return (char *)&disk_info.firmwareRev;
}
char *DtaDev::getModelNum()
{
	return (char *)&disk_info.modelNum;
}
char *DtaDev::getSerialNum()
{
	return (char *)&disk_info.serialNum;
}
void DtaDev::discovery0()
{
    LOG(D1) << "Entering DtaDev::discovery0()";
	uint8_t lastRC;
    void * d0Response = NULL;
    uint8_t * epos, *cpos;
    Discovery0Header * hdr;
    Discovery0Features * body;
	d0Response = discovery0buffer + IO_BUFFER_ALIGNMENT;
	// wrong memory alignment cause a huge difference. if correct, it seems run well 
	d0Response = (void *)((uintptr_t)d0Response & (uintptr_t)~(IO_BUFFER_ALIGNMENT - 1));
	//d0Response = (void *)(((uintptr_t)d0Response + IO_BUFFER_ALIGNMENT - 1) & (uintptr_t)~(IO_BUFFER_ALIGNMENT - 1));

	memset(d0Response, 0, IO_BUFFER_LENGTH);
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
	try {
		lastRC = sendCmd(IF_RECV, 0x01, 0x0001, d0Response, 2048); // IO_BUFFER_LENGTH); // 1024 * 14); // 12k->ok sometimes  10k->NG, 8k->NG 4096->NG); // IO_BUFFER_LENGTH->OK); // 2048->NG);
	}
	catch (char *error) // doesnt seem to catch any memory violation
	{
		printf("sendCmd Error : %s\n", error);
		//ExitProcess(0);
	}
    if ((lastRC) != 0) { 
		LOG(D1) << "sendCmd failed; lastRC= " << lastRC;

    #endif
    #if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__)
    if ((lastRC = sendCmd(IF_RECV, 0x01, 0x0001, d0Response, IO_BUFFER_LENGTH)) != 0) { 
        LOG(D1) << "Send D0 request(IO_BUFFER_LENGTH) to device failed " << (uint16_t)lastRC;
        if ((lastRC = sendCmd(IF_RECV, 0x01, 0x0001, d0Response, 2048)) == 0) {
            LOG(D1) << "Send D0 request(2048) to device OK " << (uint16_t)lastRC;
            goto OK101;
        }
    #endif
        LOG(D1) << "Send D0 request(2048) to device failed " << (uint16_t)lastRC;
        return;
    }

    else 
        { LOG(D1) << "Send D0 request(IO_BUFFER_LENGTH) to device OK " << (uint16_t)lastRC; }

    #if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__)
OK101:
    #endif
    epos = cpos = (uint8_t *) d0Response;
    hdr = (Discovery0Header *) d0Response;
    LOG(D3) << "Dumping D0Response";
    if ( (SWAP32(hdr->length) > 8192) || (SWAP32(hdr->length) < 48) )
    {
	LOG(E) << "Level 0 Discovery header length abnormal " << hex << SWAP32(hdr->length); 
	return;
    }
    IFLOG(D3) DtaHexDump(hdr, SWAP32(hdr->length));

    epos = epos + SWAP32(hdr->length);
    cpos = cpos + 48; // TODO: check header version

    do {
        body = (Discovery0Features *) cpos;
        LOG(D2) << "Discover0FeatureCode: " << hex << SWAP16(body->TPer.featureCode);
        switch (SWAP16(body->TPer.featureCode)) { /* could use of the structures here is a common field */
        case FC_TPER: /* TPer 0x001 */
            disk_info.TPer = 1;
            disk_info.TPer_ACKNACK = body->TPer.acknack;
            disk_info.TPer_async = body->TPer.async;
            disk_info.TPer_bufferMgt = body->TPer.bufferManagement;
            disk_info.TPer_comIDMgt = body->TPer.comIDManagement;
            disk_info.TPer_streaming = body->TPer.streaming;
            disk_info.TPer_sync = body->TPer.sync;
            break;
        case FC_LOCKING: /* Locking 0x002 */
            disk_info.Locking = 1;
            disk_info.Locking_locked = body->locking.locked;
            disk_info.Locking_lockingEnabled = body->locking.lockingEnabled;
            disk_info.Locking_lockingSupported = body->locking.lockingSupported;
            disk_info.Locking_MBRDone = body->locking.MBRDone;
            disk_info.Locking_MBREnabled = body->locking.MBREnabled;
            disk_info.Locking_mediaEncrypt = body->locking.mediaEncryption;
			disk_info.Locking_MBRshadowingNotSupported = body->locking.MBRshadowingNotSupported; // 
            break;
        case FC_GEOMETRY: /* Geometry Features 0x003 */
            disk_info.Geometry = 1;
            disk_info.Geometry_align = body->geometry.align;
            disk_info.Geometry_alignmentGranularity = SWAP64(body->geometry.alignmentGranularity);
            disk_info.Geometry_logicalBlockSize = SWAP32(body->geometry.logicalBlockSize);
            disk_info.Geometry_lowestAlignedLBA = SWAP64(body->geometry.lowestAlighedLBA);
            break;
        case FC_ENTERPRISE: /* Enterprise SSC 0x100 */
            disk_info.Enterprise = 1;
			disk_info.ANY_OPAL_SSC = 1;
	        disk_info.Enterprise_rangeCrossing = body->enterpriseSSC.rangeCrossing;
            disk_info.Enterprise_basecomID = SWAP16(body->enterpriseSSC.baseComID);
            disk_info.Enterprise_numcomID = SWAP16(body->enterpriseSSC.numberComIDs);
            break;
        case FC_OPALV100: /* Opal V1 0x200 */
            disk_info.OPAL10 = 1;
			disk_info.ANY_OPAL_SSC = 1;
	        disk_info.OPAL10_basecomID = SWAP16(body->opalv100.baseComID);
            disk_info.OPAL10_numcomIDs = SWAP16(body->opalv100.numberComIDs);
            break;
        case FC_SINGLEUSER: /* Single User Mode 0x201 */
            disk_info.SingleUser = 1;
            disk_info.SingleUser_all = body->singleUserMode.all;
            disk_info.SingleUser_any = body->singleUserMode.any;
            disk_info.SingleUser_policy = body->singleUserMode.policy;
            disk_info.SingleUser_lockingObjects = SWAP32(body->singleUserMode.numberLockingObjects);
            break;
        case FC_DATASTORE: /* Datastore Tables 0x202 */
            disk_info.DataStore = 1;
            disk_info.DataStore_maxTables = SWAP16(body->datastore.maxTables);
            disk_info.DataStore_maxTableSize = SWAP32(body->datastore.maxSizeTables);
            disk_info.DataStore_alignment = SWAP32(body->datastore.tableSizeAlignment);
            break;
        case FC_OPALV200: /* OPAL V200 0x203 */
            disk_info.OPAL20 = 1;
			disk_info.ANY_OPAL_SSC = 1;
		    disk_info.OPAL20_basecomID = SWAP16(body->opalv200.baseCommID);
            disk_info.OPAL20_initialPIN = body->opalv200.initialPIN;
            disk_info.OPAL20_revertedPIN = body->opalv200.revertedPIN;
            disk_info.OPAL20_numcomIDs = SWAP16(body->opalv200.numCommIDs);
            disk_info.OPAL20_numAdmins = SWAP16(body->opalv200.numlockingAdminAuth);
            disk_info.OPAL20_numUsers = SWAP16(body->opalv200.numlockingUserAuth);
            disk_info.OPAL20_rangeCrossing = body->opalv200.rangeCrossing;
			disk_info.OPAL20_version = body->opalv200.version;
            break;
        case FC_OPALITE: /* OPALITE 0x301 */
            disk_info.OPALITE = 1;
			disk_info.ANY_OPAL_SSC = 1;
		    disk_info.OPALITE_basecomID = SWAP16(body->opalv200.baseCommID);
            disk_info.OPALITE_initialPIN = body->opalv200.initialPIN;
            disk_info.OPALITE_revertedPIN = body->opalv200.revertedPIN;
            disk_info.OPALITE_numcomIDs = SWAP16(body->opalv200.numCommIDs);
			disk_info.OPALITE_version = body->opalv200.version;
			// temp patch ; use OPAL2 diskinfo if needed; need create pyrite class in the future
			disk_info.OPAL20_basecomID = SWAP16(body->opalv200.baseCommID);
			disk_info.OPAL20_initialPIN = body->opalv200.initialPIN;
			disk_info.OPAL20_revertedPIN = body->opalv200.revertedPIN;
			disk_info.OPAL20_numcomIDs = SWAP16(body->opalv200.numCommIDs);
			disk_info.OPAL20_numAdmins = 1; // SWAP16(body->opalv200.numlockingAdminAuth);
			disk_info.OPAL20_numUsers =  2; // SWAP16(body->opalv200.numlockingUserAuth);
			disk_info.OPAL20_rangeCrossing = body->opalv200.rangeCrossing;
			disk_info.OPAL20_version = body->opalv200.version;
			// does pyrite has data store. no feature set for data store default vaule 128K 
			disk_info.DataStore = 1;
			disk_info.DataStore_maxTables = 1; //  SWAP16(body->datastore.maxTables);
			disk_info.DataStore_maxTableSize = 131072; //  10485760 (OPAL2); // SWAP32(body->datastore.maxSizeTables);
			disk_info.DataStore_alignment = 1; //  SWAP32(body->datastore.tableSizeAlignment);
            break;
        case FC_PYRITE: /* PYRITE 0x302 */
            disk_info.PYRITE= 1;
			disk_info.ANY_OPAL_SSC = 1;
			disk_info.PYRITE_version = body->opalv200.version;
		    disk_info.PYRITE_basecomID = SWAP16(body->opalv200.baseCommID);
            disk_info.PYRITE_initialPIN = body->opalv200.initialPIN;
            disk_info.PYRITE_revertedPIN = body->opalv200.revertedPIN;
            disk_info.PYRITE_numcomIDs = SWAP16(body->opalv200.numCommIDs);
			// temp patch ; use OPAL2 diskinfo if needed; need create pyrite class in the future
			disk_info.OPAL20_basecomID = SWAP16(body->opalv200.baseCommID);
			disk_info.OPAL20_initialPIN = body->opalv200.initialPIN;
			disk_info.OPAL20_revertedPIN = body->opalv200.revertedPIN;
			disk_info.OPAL20_numcomIDs = SWAP16(body->opalv200.numCommIDs);
			disk_info.OPAL20_numAdmins = 1; // SWAP16(body->opalv200.numlockingAdminAuth);
			disk_info.OPAL20_numUsers = 2; // SWAP16(body->opalv200.numlockingUserAuth);
			disk_info.OPAL20_rangeCrossing = body->opalv200.rangeCrossing;
			disk_info.OPAL20_version = body->opalv200.version;
			// does pyrite has data store. no feature set for data store default vaule 128K 
			disk_info.DataStore = 1;
			disk_info.DataStore_maxTables = 1; //  SWAP16(body->datastore.maxTables);
			disk_info.DataStore_maxTableSize = 131072; //  10485760 (OPAL2); // SWAP32(body->datastore.maxSizeTables);
			disk_info.DataStore_alignment = 1; //  SWAP32(body->datastore.tableSizeAlignment);
            break; 
		case FC_PYRITE2: /* PYRITE 2 0x303 */
			disk_info.PYRITE2 = 1;
			disk_info.ANY_OPAL_SSC = 1;
			disk_info.PYRITE2_version = body->opalv200.version;
			disk_info.PYRITE2_basecomID = SWAP16(body->opalv200.baseCommID);
			disk_info.PYRITE2_initialPIN = body->opalv200.initialPIN;
			disk_info.PYRITE2_revertedPIN = body->opalv200.revertedPIN;
			disk_info.PYRITE2_numcomIDs = SWAP16(body->opalv200.numCommIDs);
			// temp patch ; use OPAL2 diskinfo if needed; need create pyrite class in the future
			disk_info.OPAL20_basecomID = SWAP16(body->opalv200.baseCommID);
			disk_info.OPAL20_initialPIN = body->opalv200.initialPIN;
			disk_info.OPAL20_revertedPIN = body->opalv200.revertedPIN;
			disk_info.OPAL20_numcomIDs = SWAP16(body->opalv200.numCommIDs);
			disk_info.OPAL20_numAdmins = 1; // SWAP16(body->opalv200.numlockingAdminAuth);
			disk_info.OPAL20_numUsers = 2; // SWAP16(body->opalv200.numlockingUserAuth);
			disk_info.OPAL20_rangeCrossing = body->opalv200.rangeCrossing;
			disk_info.OPAL20_version = body->opalv200.version;
			// does pyrite has data store. no feature set for data store default vaule 128K 
			disk_info.DataStore = 1;
			disk_info.DataStore_maxTables = 1; //  SWAP16(body->datastore.maxTables);
			disk_info.DataStore_maxTableSize = 131072; //  10485760 (OPAL2); // SWAP32(body->datastore.maxSizeTables);
			disk_info.DataStore_alignment = 1; //  SWAP32(body->datastore.tableSizeAlignment);
			break;
		case FC_RUBY: /* RUBY 0x304 */
			disk_info.RUBY = 1;
			disk_info.ANY_OPAL_SSC = 1;
			disk_info.RUBY_version = body->opalv200.version;
			disk_info.RUBY_basecomID = SWAP16(body->opalv200.baseCommID);
			disk_info.RUBY_initialPIN = body->opalv200.initialPIN;
			disk_info.RUBY_revertedPIN = body->opalv200.revertedPIN;
			disk_info.RUBY_numcomIDs = SWAP16(body->opalv200.numCommIDs);
			disk_info.RUBY_numAdmins = SWAP16(body->opalv200.numlockingAdminAuth);
			disk_info.RUBY_numUsers = SWAP16(body->opalv200.numlockingUserAuth);
			// temp patch ; use OPAL2 diskinfo if needed; need create pyrite class in the future
			disk_info.OPAL20_basecomID = SWAP16(body->opalv200.baseCommID);
			disk_info.OPAL20_initialPIN = body->opalv200.initialPIN;
			disk_info.OPAL20_revertedPIN = body->opalv200.revertedPIN;
			disk_info.OPAL20_numcomIDs = SWAP16(body->opalv200.numCommIDs);
			disk_info.OPAL20_numAdmins = 1; // SWAP16(body->opalv200.numlockingAdminAuth);
			disk_info.OPAL20_numUsers = 2; // SWAP16(body->opalv200.numlockingUserAuth);
			disk_info.OPAL20_rangeCrossing = body->opalv200.rangeCrossing;
			disk_info.OPAL20_version = body->opalv200.version;
			// does pyrite has data store. no feature set for data store default vaule 128K 
			disk_info.DataStore = 1;
			disk_info.DataStore_maxTables = 1; //  SWAP16(body->datastore.maxTables);
			disk_info.DataStore_maxTableSize = 131072; //  10485760 (OPAL2); // SWAP32(body->datastore.maxSizeTables);
			disk_info.DataStore_alignment = 1; //  SWAP32(body->datastore.tableSizeAlignment);

			break;
		case FC_BlockSID: /* Block SID 0x402 */
			disk_info.BlockSID = 1;
			disk_info.BlockSID_BlockSIDState = body->blocksidauth.BlockSIDState;
			disk_info.BlockSID_SIDvalueState = body->blocksidauth.SIDvalueState;
			disk_info.BlockSID_HardReset = body->blocksidauth.HardReset;
#if 0
			LOG(I) << "BLockSID buffer dump";
			DtaHexDump(body, 64);
			printf("body->blocksidauth.BlockSIDState= %d ", body->blocksidauth.BlockSIDState);
			printf("disk_info.BlockSID_BlockSIDState= %d ", disk_info.BlockSID_BlockSIDState);
			printf("body->blocksidauth.SIDvalueState= %d ", body->blocksidauth.SIDvalueState);
			printf("disk_info.BlockSID_SIDvalueState= %d ", disk_info.BlockSID_SIDvalueState);
			printf("body->blocksidauth.HardReset= %d ", body->blocksidauth.HardReset);
			printf("disk_info.BlockSID_HardReset= %d\n", disk_info.BlockSID_HardReset);
#endif
			break;
		case FC_NSLocking:
			disk_info.NSLocking = 1;
			disk_info.NSLocking_version = body->Configurable_Namespace_LockingFeature.version;
			disk_info.Max_Key_Count = body->Configurable_Namespace_LockingFeature.Max_Key_Count;
			disk_info.Unused_Key_Count = body->Configurable_Namespace_LockingFeature.Unused_Key_Count;
			disk_info.Max_Range_Per_NS = body->Configurable_Namespace_LockingFeature.Max_Range_Per_NS;
			break;
		case FC_DataRemoval: /* Data Remove mechanism 0x404 */
			disk_info.DataRemoval = 1;
			disk_info.DataRemoval_version = body->dataremoval.version;
			disk_info.DataRemoval_Mechanism = body->dataremoval.DataRemoval_Mechanism;
			disk_info.DataRemoval_TimeFormat_Bit5 = body->dataremoval.DataRemoval_TimeFormat_Bit5;
			disk_info.DataRemoval_Time_Bit5 = body->dataremoval.DataRemoval_Time_Bit5;
			disk_info.DataRemoval_TimeFormat_Bit5 = body->dataremoval.DataRemoval_TimeFormat_Bit4;
			disk_info.DataRemoval_Time_Bit5 = body->dataremoval.DataRemoval_Time_Bit4;
			disk_info.DataRemoval_TimeFormat_Bit5 = body->dataremoval.DataRemoval_TimeFormat_Bit3;
			disk_info.DataRemoval_Time_Bit5 = body->dataremoval.DataRemoval_Time_Bit3;
			disk_info.DataRemoval_TimeFormat_Bit5 = body->dataremoval.DataRemoval_TimeFormat_Bit2;
			disk_info.DataRemoval_Time_Bit5 = body->dataremoval.DataRemoval_Time_Bit2;
			disk_info.DataRemoval_TimeFormat_Bit5 = body->dataremoval.DataRemoval_TimeFormat_Bit1;
			disk_info.DataRemoval_Time_Bit5 = body->dataremoval.DataRemoval_Time_Bit1;
			disk_info.DataRemoval_TimeFormat_Bit5 = body->dataremoval.DataRemoval_TimeFormat_Bit0;
			disk_info.DataRemoval_Time_Bit5 = body->dataremoval.DataRemoval_Time_Bit0;
			break;
        default:
			if (0xbfff < (SWAP16(body->TPer.featureCode))) {
				// silently ignore vendor specific segments as there is no public doc on them
				disk_info.VendorSpecific += 1;
			}
			else {
				disk_info.Unknown += 1;
				LOG(D) << "Unknown Feature in Discovery 0 response " << std::hex << SWAP16(body->TPer.featureCode) << std::dec;
				/* should do something here */
			}
            break;
        }
        cpos = cpos + (body->TPer.length + 4);
    }
    while (cpos < epos);

}
uint8_t DtaDev::TperReset()
{
	LOG(D1) << "Entering DtaDev::TperReset()";
	uint8_t lastRC;
	void * tpResponse = NULL;
	tpResponse = discovery0buffer + IO_BUFFER_ALIGNMENT;
	tpResponse = (void *)((uintptr_t)tpResponse & (uintptr_t)~(IO_BUFFER_ALIGNMENT - 1));
	memset(tpResponse, 0, IO_BUFFER_LENGTH);
	// TperReset ProtocolID=0x02 ComID=0x0004
	if ((lastRC = sendCmd(IF_SEND, 0x02, 0x0004, tpResponse, 512)) != 0) { // 2048->512
		LOG(D1) << "Send TperReset to device failed " << (uint16_t)lastRC;
		return lastRC;
	}
	DtaHexDump((char *)tpResponse,64);
	return 0;
}

/*
uint8_t DtaDev::SATCK_Reset()
{
	LOG(D1) << "Entering DtaDev::STACK_Reset()";
	uint8_t lastRC;
	void * STACKResponse = NULL;
	STACKResponse = discovery0buffer + IO_BUFFER_ALIGNMENT;
	STACKResponse = (void *)((uintptr_t)STACKResponse & (uintptr_t)~(IO_BUFFER_ALIGNMENT - 1));
	memset(STACKResponse, 0, IO_BUFFER_LENGTH);
	// STACK_RESET 
	if ((lastRC = sendCmd(IF_SEND, 0x02, 0x0004, STACKResponse, 512)) != 0) { // 2048->512
		LOG(D1) << "Send STACK_Reset to device failed " << (uint16_t)lastRC;
		return lastRC;
	}
	DtaHexDump((char *)STACKResponse, 64);
	return 0;
}
*/

void DtaDev::puke()
{
	LOG(D1) << "Entering DtaDev::puke()";
	/* IDENTIFY */
	cout << endl << dev << (disk_info.devType == DEVICE_TYPE_ATA ? " ATA " : disk_info.devType == DEVICE_TYPE_SAS ? " SAS " : disk_info.devType == DEVICE_TYPE_USB ? " USB " : disk_info.devType == DEVICE_TYPE_NVME ? "NVMe" : " OTHER ");
	cout << disk_info.modelNum << ":" << disk_info.firmwareRev << " " << disk_info.serialNum << endl;
	/* TPer */
	if (disk_info.TPer) {
		cout << "TPer function (" << HEXON(4) << FC_TPER << HEXOFF << ")" << std::endl;
		cout << "    ACKNAK = " << (disk_info.TPer_ACKNACK ? "Y, " : "N, ")
			<< "ASYNC = " << (disk_info.TPer_async ? "Y, " : "N. ")
			<< "BufferManagement = " << (disk_info.TPer_bufferMgt ? "Y, " : "N, ")
			<< "comIDManagement  = " << (disk_info.TPer_comIDMgt ? "Y, " : "N, ")
			<< "Streaming = " << (disk_info.TPer_streaming ? "Y, " : "N, ")
			<< "SYNC = " << (disk_info.TPer_sync ? "Y" : "N")
			<< std::endl;
	}
	if (disk_info.Locking) {

		cout << "Locking function (" << HEXON(4) << FC_LOCKING << HEXOFF << ")" << std::endl;
		cout << "    Locked = " << (disk_info.Locking_locked ? "Y, " : "N, ")
			<< "LockingEnabled = " << (disk_info.Locking_lockingEnabled ? "Y, " : "N, ")
			<< "MBR shadowing Not Supported = " << (disk_info.Locking_MBRshadowingNotSupported ? "Y, " : "N, ")
			<< "MBRDone = " << (disk_info.Locking_MBRDone ? "Y, " : "N, ")
			<< "MBREnabled = " << (disk_info.Locking_MBREnabled ? "Y, " : "N, ")
			<< "MediaEncrypt = " << (disk_info.Locking_mediaEncrypt ? "Y" : "N")
			<< std::endl;
	}
	if (disk_info.Geometry) {

		cout << "Geometry function (" << HEXON(4) << FC_GEOMETRY << HEXOFF << ")" << std::endl;
		cout << "    Align = " << (disk_info.Geometry_align ? "Y, " : "N, ")
			<< "Alignment Granularity = " << disk_info.Geometry_alignmentGranularity
			<< " (" << // display bytes
			(disk_info.Geometry_alignmentGranularity *
			disk_info.Geometry_logicalBlockSize)
			<< ")"
			<< ", Logical Block size = " << disk_info.Geometry_logicalBlockSize
			<< ", Lowest Aligned LBA = " << disk_info.Geometry_lowestAlignedLBA
			<< std::endl;
	}
	if (disk_info.Enterprise) {
		cout << "Enterprise function (" << HEXON(4) << FC_ENTERPRISE << HEXOFF << ")" << std::endl;
		cout << "    Range crossing = " << (disk_info.Enterprise_rangeCrossing ? "Y, " : "N, ")
			<< "Base comID = " << HEXON(4) << disk_info.Enterprise_basecomID
			<< ", comIDs = " << disk_info.Enterprise_numcomID << HEXOFF
			<< std::endl;
	}
	if (disk_info.OPAL10) {
		cout << "Opal V1.0 function (" << HEXON(4) << FC_OPALV100 << HEXOFF << ")" << std::endl;
		cout << "Base comID = " << HEXON(4) << disk_info.OPAL10_basecomID << HEXOFF
			<< ", comIDs = " << disk_info.OPAL10_numcomIDs
			<< std::endl;
	}
	if (disk_info.SingleUser) {
		cout << "SingleUser function (" << HEXON(4) << FC_SINGLEUSER << HEXOFF << ")" << std::endl;
		cout << "    ALL = " << (disk_info.SingleUser_all ? "Y, " : "N, ")
			<< "ANY = " << (disk_info.SingleUser_any ? "Y, " : "N, ")
			<< "Policy = " << (disk_info.SingleUser_policy ? "Y, " : "N, ")
			<< "Locking Objects = " << (disk_info.SingleUser_lockingObjects)
			<< std::endl;
	}
	if (disk_info.DataStore) {
		cout << "DataStore function (" << HEXON(4) << FC_DATASTORE << HEXOFF << ")" << std::endl;
		cout << "    Max Tables = " << disk_info.DataStore_maxTables
			<< ", Max Size Tables = " << disk_info.DataStore_maxTableSize
			<< ", Table size alignment = " << disk_info.DataStore_alignment
			<< std::endl;
	}

	if (disk_info.OPAL20) {
		cout << "OPAL 2." << ((disk_info.OPAL20_version -1) & 0xf) << " function (" << HEXON(4) << FC_OPALV200 << ")" << HEXOFF << std::endl;
		cout << "    Base comID = " << HEXON(4) << disk_info.OPAL20_basecomID << HEXOFF;
		cout << ", Initial PIN = " << HEXON(2) << disk_info.OPAL20_initialPIN << HEXOFF;
		cout << ", Reverted PIN = " << HEXON(2) << disk_info.OPAL20_revertedPIN << HEXOFF;
		cout << ", comIDs = " << disk_info.OPAL20_numcomIDs;
		cout << std::endl;
		cout << "    Locking Admins = " << disk_info.OPAL20_numAdmins;
		cout << ", Locking Users = " << disk_info.OPAL20_numUsers;
		cout << ", Range Crossing = " << (disk_info.OPAL20_rangeCrossing ? "Y" : "N");
		cout << std::endl;
	}
	if (disk_info.OPALITE) {
		cout << "OPALITE 1." << ((disk_info.OPALITE_version & 0xf) - 1) << " function (" << HEXON(4) << FC_OPALITE << ")" << HEXOFF << std::endl;
		cout << "    Base comID = " << HEXON(4) << disk_info.OPALITE_basecomID << HEXOFF;
		cout << ", Initial PIN = " << HEXON(2) << disk_info.OPALITE_initialPIN << HEXOFF;
		cout << ", Reverted PIN = " << HEXON(2) << disk_info.OPALITE_revertedPIN << HEXOFF;
		cout << ", comIDs = " << disk_info.OPALITE_numcomIDs;
		cout << "    Locking Admins = " << disk_info.OPAL20_numAdmins;
		cout << ", Locking Users = " << disk_info.OPAL20_numUsers;
		cout << std::endl;
	}
	if (disk_info.PYRITE) {
		cout << "PYRITE 1." << ((disk_info.PYRITE_version & 0xf) -1) << " function (" << HEXON(4) << FC_PYRITE << ")" << HEXOFF << std::endl;
		cout << "    Base comID = " << HEXON(4) << disk_info.PYRITE_basecomID << HEXOFF;
		cout << ", Initial PIN = " << HEXON(2) << disk_info.PYRITE_initialPIN << HEXOFF;
		cout << ", Reverted PIN = " << HEXON(2) << disk_info.PYRITE_revertedPIN << HEXOFF;
		cout << ", comIDs = " << disk_info.PYRITE_numcomIDs;
		cout << "    Locking Admins = " << disk_info.OPAL20_numAdmins;
		cout << ", Locking Users = " << disk_info.OPAL20_numUsers;
		cout << std::endl;
	}
	if (disk_info.PYRITE2) {
		cout << "PYRITE 2." << ((disk_info.PYRITE2_version & 0xf) - 1) << " function (" << HEXON(4) << FC_PYRITE << ")" << HEXOFF << std::endl;
		cout << "    Base comID = " << HEXON(4) << disk_info.PYRITE2_basecomID << HEXOFF;
		cout << ", Initial PIN = " << HEXON(2) << disk_info.PYRITE2_initialPIN << HEXOFF;
		cout << ", Reverted PIN = " << HEXON(2) << disk_info.PYRITE2_revertedPIN << HEXOFF;
		cout << ", comIDs = " << disk_info.PYRITE2_numcomIDs;
		cout << "    Locking Admins = " << disk_info.OPAL20_numAdmins;
		cout << ", Locking Users = " << disk_info.OPAL20_numUsers;
		cout << std::endl;
	}
	if (disk_info.RUBY) {
		cout << "RUBY 1." << ((disk_info.RUBY_version & 0xf) - 1) << " function (" << HEXON(4) << FC_RUBY << ")" << HEXOFF << std::endl;
		cout << "    Base comID = " << HEXON(4) << disk_info.RUBY_basecomID << HEXOFF;
		cout << ", Initial PIN = " << HEXON(2) << disk_info.RUBY_initialPIN << HEXOFF;
		cout << ", Reverted PIN = " << HEXON(2) << disk_info.RUBY_revertedPIN << HEXOFF;
		cout << ", comIDs = " << disk_info.RUBY_numcomIDs;
		cout << "    Locking Admins = " << disk_info.OPAL20_numAdmins;
		cout << ", Locking Users = " << disk_info.OPAL20_numUsers;
		cout << std::endl;
	}
	if (disk_info.BlockSID) {
		cout << "BlockSID function (" << HEXON(4) << FC_BlockSID << ")" << HEXOFF << std::endl;
		cout << "    BlockSIDState = " << (disk_info.BlockSID_BlockSIDState ? "Y" : "N" );
		cout << ", SIDvalueState = " << (disk_info.BlockSID_SIDvalueState? "1" : "0");
		cout << ", HardReset  = " << (disk_info.BlockSID_HardReset? "1" : "0") ;
		cout << std::endl;
	}
	if (disk_info.DataRemoval) {
		cout << "DataRemoval 1." << ((disk_info.DataRemoval_version & 0xf) - 1) << " function (" << HEXON(4) << FC_DataRemoval << ")" << HEXOFF << std::endl;
		cout << "    DataRemoval OperationProcessing " << HEXON(2) << disk_info.DataRemoval_OperationProcessing << HEXOFF;
		cout << ", DataRemoval Machanisim " << HEXON(2) << disk_info.DataRemoval_Mechanism << HEXOFF << std::endl;
		cout << "    DataRemoval TimeFormat Bit 5 : " << HEXON(2) << disk_info.DataRemoval_TimeFormat_Bit5 << " " << HEXON(4) << disk_info.DataRemoval_Time_Bit5 << HEXOFF << std::endl;
		cout << "    DataRemoval TimeFormat Bit 4 : " << HEXON(2) << disk_info.DataRemoval_TimeFormat_Bit4 << " " << HEXON(4) << disk_info.DataRemoval_Time_Bit4 << HEXOFF << std::endl;
		cout << "    DataRemoval TimeFormat Bit 3 : " << HEXON(2) << disk_info.DataRemoval_TimeFormat_Bit3 << " " << HEXON(4) << disk_info.DataRemoval_Time_Bit3 << HEXOFF << std::endl;
		cout << "    DataRemoval TimeFormat Bit 2 : " << HEXON(2) << disk_info.DataRemoval_TimeFormat_Bit2 << " " << HEXON(4) << disk_info.DataRemoval_Time_Bit2 << HEXOFF << std::endl;
		cout << "    DataRemoval TimeFormat Bit 1 : " << HEXON(2) << disk_info.DataRemoval_TimeFormat_Bit1 << " " << HEXON(4) << disk_info.DataRemoval_Time_Bit1 << HEXOFF << std::endl;
		cout << "    DataRemoval TimeFormat Bit 0 : " << HEXON(2) << disk_info.DataRemoval_TimeFormat_Bit0 << " " << HEXON(4) << disk_info.DataRemoval_Time_Bit0 << HEXOFF << std::endl;
	}



	if (disk_info.Unknown)
		cout << "**** " << (uint16_t)disk_info.Unknown << " **** Unknown function codes IGNORED " << std::endl;
}
