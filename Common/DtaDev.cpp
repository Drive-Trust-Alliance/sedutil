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
/** Base device class.
 * An OS port must create a subclass of this class
 * implementing sendcmd, osmsSleep and identify
 * specific to the IO requirements of that OS
 */
#include "os.h"
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include "DtaOptions.h"
#include "DtaDev.h"
#include "DtaStructures.h"
#include "DtaEndianFixup.h"
#include "DtaHexDump.h"
#include "DtaSession.h"
#include "DtaCommand.h"

#include "ob.h"

void setlic(unsigned char * lic_level, const char * LicenseLevel)
{
    unsigned char sbnk[16] = { ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ', ' ', ' ', ' ', };
    obfs ob;

    if (!memcmp("0:", LicenseLevel, 2)) { // correct feature set
        ob.lic(atoi(&LicenseLevel[2]), lic_level);
    }
    else {
        memcpy(lic_level, sbnk, 16);
        printf("no license = %s\n", lic_level);
    }
}



void auditpass(unsigned char * apass)
{
    obfs ob;
    ob.setaudpass(apass);
}


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

uint8_t DtaDev::isFIPS()
{
	LOG(D1) << "Entering DtaDev::isFIPS " << (uint16_t)disk_info.fips;
	return disk_info.fips;
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
uint8_t DtaDev::isOpal2_minor_v()
{
	LOG(D1) << "Entering DtaDev::isOpal2_minor " << (uint16_t)disk_info.OPAL20_minor_v;
	return disk_info.OPAL20_minor_v;
}
uint8_t DtaDev::isOpal2_version()
{
	LOG(D1) << "Entering DtaDev::isOpal2_version " << (uint16_t)disk_info.OPAL20_version;
	return disk_info.OPAL20_version;
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
DTA_DEVICE_TYPE DtaDev::getDevType()
{
    return disk_info.devType;
}

uint8_t DtaDev::acquireDiscovery0Response(uint8_t * d0Response)
{
    return sendCmd(IF_RECV, 0x01, 0x0001, d0Response, MIN_BUFFER_LENGTH);
}

void DtaDev::parseDiscovery0Features(const uint8_t * d0Response, DTA_DEVICE_INFO & di)
{
    LOG(D1) << "Entering DtaDev::discovery0()";
    Discovery0Header * hdr = (Discovery0Header *) d0Response;
    uint32_t length = SWAP32(hdr->length);
    if (0 == length)
    {
        LOG(D) << "Level 0 Discovery returned no response";
        return;
    }
    LOG(D3) << "Dumping D0Response";
    if ( (length > 8192) || (length < 48) )
    {
	  LOG(D) << "Level 0 Discovery header length abnormal " << hex << length;
	  return;
    }
    IFLOG(D3) DtaHexDump(hdr, length);

    uint8_t *cpos = (uint8_t *) d0Response + 48; // TODO: check header version
    uint8_t *epos = (uint8_t *) d0Response + length;

    do {
        Discovery0Features * body = (Discovery0Features *) cpos;
        uint16_t featureCode = SWAP16(body->TPer.featureCode);
        LOG(D2) << "Discovery0 FeatureCode: " << hex << featureCode;
        switch (featureCode) { /* could use of the structures here is a common field */
        case FC_TPER: /* TPer */
            LOG(D2) << "TPer Feature";
            di.TPer = 1;
            di.TPer_ACKNACK = body->TPer.acknack;
            di.TPer_async = body->TPer.async;
            di.TPer_bufferMgt = body->TPer.bufferManagement;
            di.TPer_comIDMgt = body->TPer.comIDManagement;
            di.TPer_streaming = body->TPer.streaming;
            di.TPer_sync = body->TPer.sync;
            break;
        case FC_LOCKING: /* Locking*/
            LOG(D2) << "Locking Feature";
            di.Locking = 1;
            di.Locking_locked = body->locking.locked;
            di.Locking_lockingEnabled = body->locking.lockingEnabled;
            di.Locking_lockingSupported = body->locking.lockingSupported;
            di.Locking_MBRDone = body->locking.MBRDone;
            di.Locking_MBREnabled = body->locking.MBREnabled;
            di.Locking_mediaEncrypt = body->locking.mediaEncryption;
            break;
        case FC_GEOMETRY: /* Geometry Features */
            LOG(D2) << "Geometry Feature";
            di.Geometry = 1;
            di.Geometry_align = body->geometry.align;
            di.Geometry_alignmentGranularity = SWAP64(body->geometry.alignmentGranularity);
            di.Geometry_logicalBlockSize = SWAP32(body->geometry.logicalBlockSize);
            di.Geometry_lowestAlignedLBA = SWAP64(body->geometry.lowestAlighedLBA);
            break;
        case FC_ENTERPRISE: /* Enterprise SSC */
            LOG(D2) << "Enterprise SSC Feature";
            di.Enterprise = 1;
			di.ANY_OPAL_SSC = 1;
	        di.Enterprise_rangeCrossing = body->enterpriseSSC.rangeCrossing;
            di.Enterprise_basecomID = SWAP16(body->enterpriseSSC.baseComID);
            di.Enterprise_numcomID = SWAP16(body->enterpriseSSC.numberComIDs);
            break;
        case FC_OPALV100: /* Opal V1 */
            LOG(D2) << "Opal v1.0 SSC Feature";
            di.OPAL10 = 1;
			di.ANY_OPAL_SSC = 1;
	        di.OPAL10_basecomID = SWAP16(body->opalv100.baseComID);
            di.OPAL10_numcomIDs = SWAP16(body->opalv100.numberComIDs);
            break;
        case FC_SINGLEUSER: /* Single User Mode */
            LOG(D2) << "Single User Mode Feature";
            di.SingleUser = 1;
            di.SingleUser_all = body->singleUserMode.all;
            di.SingleUser_any = body->singleUserMode.any;
            di.SingleUser_policy = body->singleUserMode.policy;
            di.SingleUser_lockingObjects = SWAP32(body->singleUserMode.numberLockingObjects);
            break;
        case FC_DATASTORE: /* Datastore Tables */
            LOG(D2) << "Datastore Feature";
            di.DataStore = 1;
            di.DataStore_maxTables = SWAP16(body->datastore.maxTables);
            di.DataStore_maxTableSize = SWAP32(body->datastore.maxSizeTables);
            di.DataStore_alignment = SWAP32(body->datastore.tableSizeAlignment);
            break;
        case FC_OPALV200: /* OPAL V200 */
            LOG(D2) << "Opal v2.0 SSC Feature";
            di.OPAL20 = 1;
			di.ANY_OPAL_SSC = 1;
		    di.OPAL20_basecomID = SWAP16(body->opalv200.baseCommID);
            di.OPAL20_initialPIN = body->opalv200.initialPIN;
            di.OPAL20_revertedPIN = body->opalv200.revertedPIN;
            di.OPAL20_numcomIDs = SWAP16(body->opalv200.numCommIDs);
            di.OPAL20_numAdmins = SWAP16(body->opalv200.numlockingAdminAuth);
            di.OPAL20_numUsers = SWAP16(body->opalv200.numlockingUserAuth);
            di.OPAL20_rangeCrossing = body->opalv200.rangeCrossing;
            break;
        case FC_PYRITE: /* PYRITE 0x302 */
            LOG(D2) << "Pyrite SSC Feature";
            di.PYRITE= 1;
			di.ANY_OPAL_SSC = 1;
			di.PYRITE_version = body->opalv200.version;
		    di.PYRITE_basecomID = SWAP16(body->opalv200.baseCommID);
            di.PYRITE_initialPIN = body->opalv200.initialPIN;
            di.PYRITE_revertedPIN = body->opalv200.revertedPIN;
            di.PYRITE_numcomIDs = SWAP16(body->opalv200.numCommIDs);
			// temp patch ; use OPAL2 diskinfo if needed; need create pyrite class in the future
			di.OPAL20_basecomID = SWAP16(body->opalv200.baseCommID);
			di.OPAL20_initialPIN = body->opalv200.initialPIN;
			di.OPAL20_revertedPIN = body->opalv200.revertedPIN;
			di.OPAL20_numcomIDs = SWAP16(body->opalv200.numCommIDs);
			di.OPAL20_numAdmins = 1; // SWAP16(body->opalv200.numlockingAdminAuth);
			di.OPAL20_numUsers = 2; // SWAP16(body->opalv200.numlockingUserAuth);
			di.OPAL20_rangeCrossing = body->opalv200.rangeCrossing;
			di.OPAL20_version = body->opalv200.version;
			// does pyrite has data store. no feature set for data store default vaule 128K
			di.DataStore = 1;
			di.DataStore_maxTables = 1; //  SWAP16(body->datastore.maxTables);
			di.DataStore_maxTableSize = 131072; //  10485760 (OPAL2); // SWAP32(body->datastore.maxSizeTables);
			di.DataStore_alignment = 1; //  SWAP32(body->datastore.tableSizeAlignment);
            break;
		case FC_PYRITE2: /* PYRITE 2 0x303 */
            LOG(D2) << "Pyrite 2 SSC Feature";
			di.PYRITE2 = 1;
			di.ANY_OPAL_SSC = 1;
			di.PYRITE2_version = body->opalv200.version;
			di.PYRITE2_basecomID = SWAP16(body->opalv200.baseCommID);
			di.PYRITE2_initialPIN = body->opalv200.initialPIN;
			di.PYRITE2_revertedPIN = body->opalv200.revertedPIN;
			di.PYRITE2_numcomIDs = SWAP16(body->opalv200.numCommIDs);
			// temp patch ; use OPAL2 diskinfo if needed; need create pyrite class in the future
			di.OPAL20_basecomID = SWAP16(body->opalv200.baseCommID);
			di.OPAL20_initialPIN = body->opalv200.initialPIN;
			di.OPAL20_revertedPIN = body->opalv200.revertedPIN;
			di.OPAL20_numcomIDs = SWAP16(body->opalv200.numCommIDs);
			di.OPAL20_numAdmins = 1; // SWAP16(body->opalv200.numlockingAdminAuth);
			di.OPAL20_numUsers = 2; // SWAP16(body->opalv200.numlockingUserAuth);
			di.OPAL20_rangeCrossing = body->opalv200.rangeCrossing;
			di.OPAL20_version = body->opalv200.version;
			// does pyrite has data store. no feature set for data store default vaule 128K
			di.DataStore = 1;
			di.DataStore_maxTables = 1; //  SWAP16(body->datastore.maxTables);
			di.DataStore_maxTableSize = 131072; //  10485760 (OPAL2); // SWAP32(body->datastore.maxSizeTables);
			di.DataStore_alignment = 1; //  SWAP32(body->datastore.tableSizeAlignment);
			break;
		case FC_RUBY: /* RUBY 0x304 */
            LOG(D2) << "Ruby SSC Feature";
			di.RUBY = 1;
			di.ANY_OPAL_SSC = 1;
			di.RUBY_version = body->opalv200.version;
			di.RUBY_basecomID = SWAP16(body->opalv200.baseCommID);
			di.RUBY_initialPIN = body->opalv200.initialPIN;
			di.RUBY_revertedPIN = body->opalv200.revertedPIN;
			di.RUBY_numcomIDs = SWAP16(body->opalv200.numCommIDs);
			di.RUBY_numAdmins = SWAP16(body->opalv200.numlockingAdminAuth);
			di.RUBY_numUsers = SWAP16(body->opalv200.numlockingUserAuth);
			// temp patch ; use OPAL2 diskinfo if needed; need create pyrite class in the future
			di.OPAL20_basecomID = SWAP16(body->opalv200.baseCommID);
			di.OPAL20_initialPIN = body->opalv200.initialPIN;
			di.OPAL20_revertedPIN = body->opalv200.revertedPIN;
			di.OPAL20_numcomIDs = SWAP16(body->opalv200.numCommIDs);
			di.OPAL20_numAdmins = 1; // SWAP16(body->opalv200.numlockingAdminAuth);
			di.OPAL20_numUsers = 2; // SWAP16(body->opalv200.numlockingUserAuth);
			di.OPAL20_rangeCrossing = body->opalv200.rangeCrossing;
			di.OPAL20_version = body->opalv200.version;
			// does pyrite has data store. no feature set for data store default vaule 128K
			di.DataStore = 1;
			di.DataStore_maxTables = 1; //  SWAP16(body->datastore.maxTables);
			di.DataStore_maxTableSize = 131072; //  10485760 (OPAL2); // SWAP32(body->datastore.maxSizeTables);
			di.DataStore_alignment = 1; //  SWAP32(body->datastore.tableSizeAlignment);
			break;
		case FC_BlockSID: /* Block SID 0x402 */
            LOG(D2) << "Block SID Feature";
			di.BlockSID = 1;
			di.BlockSID_BlockSIDState = body->blocksidauth.BlockSIDState;
			di.BlockSID_SIDvalueState = body->blocksidauth.SIDvalueState;
			di.BlockSID_HardReset = body->blocksidauth.HardReset;
			break;
		case FC_NSLocking:
            LOG(D2) << "Namespace Locking Feature";
			di.NSLocking = 1;
			di.NSLocking_version = body->Configurable_Namespace_LockingFeature.version;
			di.Max_Key_Count = body->Configurable_Namespace_LockingFeature.Max_Key_Count;
			di.Unused_Key_Count = body->Configurable_Namespace_LockingFeature.Unused_Key_Count;
			di.Max_Range_Per_NS = body->Configurable_Namespace_LockingFeature.Max_Range_Per_NS;
			break;
		case FC_DataRemoval: /* Data Removal mechanism 0x404 */
            LOG(D2) << "Data Removal Feature";
			di.DataRemoval = 1;
			di.DataRemoval_version = body->dataremoval.version;
			di.DataRemoval_Mechanism = body->dataremoval.DataRemoval_Mechanism;
			di.DataRemoval_TimeFormat_Bit5 = body->dataremoval.DataRemoval_TimeFormat_Bit5;
			di.DataRemoval_Time_Bit5 = body->dataremoval.DataRemoval_Time_Bit5;
			di.DataRemoval_TimeFormat_Bit5 = body->dataremoval.DataRemoval_TimeFormat_Bit4;
			di.DataRemoval_Time_Bit5 = body->dataremoval.DataRemoval_Time_Bit4;
			di.DataRemoval_TimeFormat_Bit5 = body->dataremoval.DataRemoval_TimeFormat_Bit3;
			di.DataRemoval_Time_Bit5 = body->dataremoval.DataRemoval_Time_Bit3;
			di.DataRemoval_TimeFormat_Bit5 = body->dataremoval.DataRemoval_TimeFormat_Bit2;
			di.DataRemoval_Time_Bit5 = body->dataremoval.DataRemoval_Time_Bit2;
			di.DataRemoval_TimeFormat_Bit5 = body->dataremoval.DataRemoval_TimeFormat_Bit1;
			di.DataRemoval_Time_Bit5 = body->dataremoval.DataRemoval_Time_Bit1;
			di.DataRemoval_TimeFormat_Bit5 = body->dataremoval.DataRemoval_TimeFormat_Bit0;
			di.DataRemoval_Time_Bit5 = body->dataremoval.DataRemoval_Time_Bit0;
			break;
        default:
            if (FC_Min_Vendor_Specific <= featureCode) {
				// silently ignore vendor specific segments as there is no public doc on them
				di.VendorSpecific += 1;
                LOG(D2) << "Vendor Specfic Feature Code " << std::hex << featureCode << std::dec;
			} else {
				di.Unknown += 1;
				LOG(D) << "Unknown Feature Code " << std::hex << featureCode << std::dec << "in Discovery0 response";
				/* should do something here */
			}
            break;
        }
        cpos = cpos + (body->TPer.length + 4);
    }
    while (cpos < epos);
	// do adjustment for No Additional data store case
	if (!di.DataStore  || !di.DataStore_maxTables || !di.DataStore_maxTableSize) {
		di.DataStore_maxTableSize = 10 * 1024 * 1024;
	}
}


uint8_t DtaDev::TperReset()
{
	LOG(D1) << "Entering DtaDev::TperReset()";
	uint8_t lastRC;
	void * tpResponse = (void *)((((uintptr_t)discovery0buffer) +
                                   (uintptr_t)IO_BUFFER_ALIGNMENT) &      //  0x00002000 if e.g. 16384
                                 (uintptr_t)~(IO_BUFFER_ALIGNMENT - 1));  // ~0x00001FFF will be
                                                                          //  0xFFFFE000 masking to alignment
	memset(tpResponse, 0, MIN_BUFFER_LENGTH);
	// TperReset ProtocolID=0x02 ComID=0x0004
	if ((lastRC = sendCmd(IF_SEND, 0x02, 0x0004, tpResponse, 512)) != 0) { // 2048->512
		LOG(D1) << "Send TperReset to device failed " << (uint16_t)lastRC;
		return lastRC;
	}
	IFLOG(D2) DtaHexDump((char *)tpResponse,64);
	return 0;
}

/*
uint8_t DtaDev::STACK_Reset()
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


void DtaDev::discovery0() {
    uint8_t * d0Response = (uint8_t *)((uintptr_t)(discovery0buffer + IO_BUFFER_ALIGNMENT) &
                                 (uintptr_t)~(IO_BUFFER_ALIGNMENT - 1));
    memset(d0Response, 0, MIN_BUFFER_LENGTH);

    uint8_t lastRC = acquireDiscovery0Response(d0Response);
    if ((lastRC ) != 0) {
        LOG(D) << "Acquiring D0 response failed " << (uint16_t)lastRC;
        return;
    }
    parseDiscovery0Features(d0Response, disk_info);
}


void DtaDev::puke()
{
	LOG(D1) << "Entering DtaDev::puke()";
	/* IDENTIFY */
	cout << endl << dev << (disk_info.devType == DEVICE_TYPE_ATA ? " ATA " :
            disk_info.devType == DEVICE_TYPE_SAS ? " SAS " :
            disk_info.devType == DEVICE_TYPE_USB ? " USB " :
            disk_info.devType == DEVICE_TYPE_NVME ? " NVMe " :
                    " OTHER ");
	cout << disk_info.modelNum << " " << disk_info.firmwareRev << " " << disk_info.serialNum << endl;
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
		cout << ", Initial PIN = " << HEXON(2) << static_cast<uint32_t>(disk_info.OPAL20_initialPIN) << HEXOFF;
		cout << ", Reverted PIN = " << HEXON(2) << static_cast<uint32_t>(disk_info.OPAL20_revertedPIN) << HEXOFF;
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

uint8_t DtaDev::WithSession(std::function<uint8_t(void)>startSessionFn,
                            std::function<uint8_t(void)>sessionBodyFn) {
    session = new DtaSession(this);
    if (NULL == session) {
        LOG(E) << "Unable to create session object " << dev;
        return DTAERROR_OBJECT_CREATE_FAILED;
    }
    
    
    uint8_t lastRC;

    if ((lastRC = startSessionFn()) == 0) {
        lastRC = sessionBodyFn();
    }
            
    delete session;
    return lastRC;
    
}

uint8_t DtaDev::WithSessionCommand(std::function<uint8_t(void)>startSessionFn,
                                   std::function<void(DtaCommand * command)>commandWriterFn) {
   return WithSession(startSessionFn, [this, commandWriterFn]()->uint8_t{
        DtaCommand *command = new DtaCommand();
        if (NULL == command) {
            LOG(E) << "Unable to create command object " << dev;
            return DTAERROR_OBJECT_CREATE_FAILED;
        }
        commandWriterFn(command);
        uint8_t rc = session->sendCommand(command, response);
        delete command;
        return rc;
    });
}
