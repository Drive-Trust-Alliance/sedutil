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
uint8_t DtaDev::isRuby1()
{
	LOG(D1) << "Entering DtaDev::isRuby1 " << (uint16_t) disk_info.Ruby10;
	return disk_info.Ruby10;
}
uint8_t DtaDev::isPyrite2()
{
	LOG(D1) << "Entering DtaDev::isPyrite2 " << (uint16_t) disk_info.Pyrite20;
	return disk_info.Pyrite20;
}
uint8_t DtaDev::isPyrite1()
{
	LOG(D1) << "Entering DtaDev::isPyrite1 " << (uint16_t) disk_info.Pyrite10;
	return disk_info.Pyrite10;
}
uint8_t DtaDev::isOpalite()
{
	LOG(D1) << "Entering DtaDev::isOpalite " << (uint16_t) disk_info.Opalite;
	return disk_info.Opalite;
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
uint8_t DtaDev::MBRAbsent()
{
	LOG(D1) << "Entering DtaDev::MBRAbsent" << (uint16_t)disk_info.Locking_MBRAbsent;
	return disk_info.Locking_MBRAbsent;
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
void DtaDev::discovery0()
{
    LOG(D1) << "Entering DtaDev::discovery0()";
	uint8_t lastRC;
    void * d0Response = NULL;
    uint8_t * epos, *cpos;
    Discovery0Header * hdr;
    Discovery0Features * body;
	uint32_t len;

	d0Response = discovery0buffer + IO_BUFFER_ALIGNMENT;
	d0Response = (void *)((uintptr_t)d0Response & (uintptr_t)~(IO_BUFFER_ALIGNMENT - 1));
	memset(d0Response, 0, MIN_BUFFER_LENGTH);
    if ((lastRC = sendCmd(IF_RECV, 0x01, 0x0001, d0Response, MIN_BUFFER_LENGTH)) != 0) {
        LOG(D) << "Send D0 request to device failed " << (uint16_t)lastRC;
        return;
    }

    epos = cpos = (uint8_t *) d0Response;
    hdr = (Discovery0Header *) d0Response;
    len = SWAP32(hdr->length);
    if (len > MIN_BUFFER_LENGTH) {
	LOG(D) << "Too long Discovery0 response: " << SWAP32(hdr->length);
	len = MIN_BUFFER_LENGTH;
    }
    LOG(D3) << "Dumping D0Response";
    IFLOG(D3) DtaHexDump(hdr, len);
    epos = epos + len;
    cpos = cpos + 48; // TODO: check header version

    do {
        body = (Discovery0Features *) cpos;
        switch (SWAP16(body->TPer.featureCode)) { /* could use of the structures here is a common field */
        case FC_TPER: /* TPer */
            disk_info.TPer = 1;
            disk_info.TPer_ACKNACK = body->TPer.acknack;
            disk_info.TPer_async = body->TPer.async;
            disk_info.TPer_bufferMgt = body->TPer.bufferManagement;
            disk_info.TPer_comIDMgt = body->TPer.comIDManagement;
            disk_info.TPer_streaming = body->TPer.streaming;
            disk_info.TPer_sync = body->TPer.sync;
            break;
        case FC_LOCKING: /* Locking*/
            disk_info.Locking = 1;
            disk_info.Locking_locked = body->locking.locked;
            disk_info.Locking_lockingEnabled = body->locking.lockingEnabled;
            disk_info.Locking_lockingSupported = body->locking.lockingSupported;
            disk_info.Locking_MBRDone = body->locking.MBRDone;
            disk_info.Locking_MBREnabled = body->locking.MBREnabled;
            disk_info.Locking_MBRAbsent = body->locking.MBRAbsent;
            disk_info.Locking_mediaEncrypt = body->locking.mediaEncryption;
            break;
        case FC_GEOMETRY: /* Geometry Features */
            disk_info.Geometry = 1;
            disk_info.Geometry_align = body->geometry.align;
            disk_info.Geometry_alignmentGranularity = SWAP64(body->geometry.alignmentGranularity);
            disk_info.Geometry_logicalBlockSize = SWAP32(body->geometry.logicalBlockSize);
            disk_info.Geometry_lowestAlignedLBA = SWAP64(body->geometry.lowestAlighedLBA);
            break;
        case FC_SECUREMSG: /* Secure Messaging */
            disk_info.SecureMsg = 1;
            disk_info.SecureMsg_activated = body->secureMsg.activated;
            disk_info.SecureMsg_numberOfSPs = SWAP16(body->secureMsg.numberOfSPs);
            break;
        case FC_ENTERPRISE: /* Enterprise SSC */
            disk_info.Enterprise = 1;
			disk_info.ANY_OPAL_SSC = 1;
	        disk_info.Enterprise_rangeCrossing = body->enterpriseSSC.rangeCrossing;
            disk_info.Enterprise_basecomID = SWAP16(body->enterpriseSSC.baseComID);
            disk_info.Enterprise_numcomID = SWAP16(body->enterpriseSSC.numberComIDs);
            break;
        case FC_OPALV100: /* Opal V1 */
            disk_info.OPAL10 = 1;
			disk_info.ANY_OPAL_SSC = 1;
	        disk_info.OPAL10_basecomID = SWAP16(body->opalv100.baseComID);
            disk_info.OPAL10_numcomIDs = SWAP16(body->opalv100.numberComIDs);
            disk_info.OPAL10_rangeCrossing = body->opalv100.rangeCrossing;
            break;
        case FC_SINGLEUSER: /* Single User Mode */
            disk_info.SingleUser = 1;
            disk_info.SingleUser_all = body->singleUserMode.all;
            disk_info.SingleUser_any = body->singleUserMode.any;
            disk_info.SingleUser_policy = body->singleUserMode.policy;
            disk_info.SingleUser_lockingObjects = SWAP32(body->singleUserMode.numberLockingObjects);
            break;
        case FC_DATASTORE: /* Datastore Tables */
            disk_info.DataStore = 1;
            disk_info.DataStore_maxTables = SWAP16(body->datastore.maxTables);
            disk_info.DataStore_maxTableSize = SWAP32(body->datastore.maxSizeTables);
            disk_info.DataStore_alignment = SWAP32(body->datastore.tableSizeAlignment);
            break;
        case FC_OPALV200: /* OPAL V200 */
            disk_info.OPAL20 = 1;
			disk_info.ANY_OPAL_SSC = 1;
		    disk_info.OPAL20_basecomID = SWAP16(body->opalv200.baseCommID);
            disk_info.OPAL20_initialPIN = body->opalv200.initialPIN;
            disk_info.OPAL20_revertedPIN = body->opalv200.revertedPIN;
            disk_info.OPAL20_numcomIDs = SWAP16(body->opalv200.numCommIDs);
            disk_info.OPAL20_numAdmins = SWAP16(body->opalv200.numlockingAdminAuth);
            disk_info.OPAL20_numUsers = SWAP16(body->opalv200.numlockingUserAuth);
            disk_info.OPAL20_rangeCrossing = body->opalv200.rangeCrossing;
            break;
		case FC_BLOCKSID: /* Block SID Authentication */
			disk_info.BlockSID = 1;
			disk_info.BlockSID_SIDBlockedState = body->blockSID.SIDBlockedState;
			disk_info.BlockSID_SIDValueState = body->blockSID.SIDValueState;
			disk_info.BlockSID_HardwareReset = body->blockSID.HardwareReset;
			break;
		case FC_NAMESPACE: /* Namespace */
			disk_info.Namespace = 1;
			disk_info.Namespace_MaximumKeyCount = SWAP32(body->ns.MaximumKeyCount);
			disk_info.Namespace_UnusedKeyCount = SWAP32(body->ns.UnusedKeyCount);
			disk_info.Namespace_MaximumRangesPerNamespace = SWAP32(body->ns.MaximumRangesPerNamespace);
			break;
		case FC_OPALITE: /* Opalite */
			disk_info.Opalite = 1;
			disk_info.ANY_OPAL_SSC = 1;
			disk_info.Opalite_basecomID = SWAP16(body->opalite.baseCommID);
			disk_info.Opalite_numcomIDs = SWAP16(body->opalite.numCommIDs);
			disk_info.Opalite_initialPIN = body->opalite.initialPIN;
			disk_info.Opalite_revertedPIN = body->opalite.revertedPIN;
			break;
		case FC_PYRITEV100: /* Pyrite V100 */
			disk_info.Pyrite10 = 1;
			disk_info.ANY_OPAL_SSC = 1;
			disk_info.Pyrite10_basecomID = SWAP16(body->pyrite10.baseCommID);
			disk_info.Pyrite10_numcomIDs = SWAP16(body->pyrite10.numCommIDs);
			disk_info.Pyrite10_initialPIN = body->pyrite10.initialPIN;
			disk_info.Pyrite10_revertedPIN = body->pyrite10.revertedPIN;
			break;
		case FC_PYRITEV200: /* Pyrite V200 */
			disk_info.Pyrite20 = 1;
			disk_info.ANY_OPAL_SSC = 1;
			disk_info.Pyrite20_basecomID = SWAP16(body->pyrite20.baseCommID);
			disk_info.Pyrite20_numcomIDs = SWAP16(body->pyrite20.numCommIDs);
			disk_info.Pyrite20_initialPIN = body->pyrite20.initialPIN;
			disk_info.Pyrite20_revertedPIN = body->pyrite20.revertedPIN;
			break;
		case FC_RUBYV100: /* Ruby V1.00 */
			disk_info.Ruby10 = 1;
			disk_info.ANY_OPAL_SSC = 1;
			disk_info.Ruby10_basecomID = SWAP16(body->ruby10.baseCommID);
			disk_info.Ruby10_numcomIDs = SWAP16(body->ruby10.numCommIDs);
			disk_info.Ruby10_rangeCrossing = body->ruby10.rangeCrossing;
			disk_info.Ruby10_numAdmins = SWAP16(body->ruby10.numlockingAdminAuth);
			disk_info.Ruby10_numUsers = SWAP16(body->ruby10.numlockingUserAuth);
			disk_info.Ruby10_initialPIN = body->ruby10.initialPIN;
			disk_info.Ruby10_revertedPIN = body->ruby10.revertedPIN;
			disk_info.Ruby10_PINonTPerRevert = body->ruby10.PINonTPerRevert;
			break;
		case FC_DATAREM: /* Supported Data Removal Mechanism */
			disk_info.DataRem = 1;
			disk_info.DataRem_processing = body->dataRem.processing;
			disk_info.DataRem_supported = body->dataRem.supported;
			disk_info.DataRem_format = body->dataRem.format;
			for (int i = 0; i < 6; i++)
				disk_info.DataRem_time[i] = SWAP16(body->dataRem.time[i]);
			break;
		case FC_NSGEOMETRY: /* Namespace Geometry Reporting */
			disk_info.NSGeometry = 1;
			disk_info.NSGeometry_align = body->nsgeometry.align;
			disk_info.NSGeometry_alignmentGranularity = SWAP64(body->nsgeometry.alignmentGranularity);
			disk_info.NSGeometry_logicalBlockSize = SWAP32(body->nsgeometry.logicalBlockSize);
			disk_info.NSGeometry_lowestAlignedLBA = SWAP64(body->nsgeometry.lowestAlighedLBA);
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
			<< "LockingSupported = " << (disk_info.Locking_lockingSupported ? "Y, " : "N, ");
		cout << "MBRDone = " << (disk_info.Locking_MBRDone ? "Y, " : "N, ")
			<< "MBREnabled = " << (disk_info.Locking_MBREnabled ? "Y, " : "N, ")
			<< "MBRAbsent = " << (disk_info.Locking_MBRAbsent ? "Y, " : "N, ")
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
	if (disk_info.SecureMsg) {

		cout << "Secure Messaging function (" << HEXON(4) << FC_SECUREMSG << HEXOFF << ")" << std::endl;
		cout << "    Activated = " << (disk_info.SecureMsg_activated ? "Y, " : "N, ")
			<< "Number of SPs = " << disk_info.SecureMsg_numberOfSPs
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
		cout << "    Base comID = " << HEXON(4) << disk_info.OPAL10_basecomID << HEXOFF
			<< ", comIDs = " << disk_info.OPAL10_numcomIDs
			<< ", Range Crossing = " << (disk_info.OPAL10_rangeCrossing ? "Y" : "N")
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
		cout << "OPAL 2.0 function (" << HEXON(4) << FC_OPALV200 << ")" << HEXOFF << std::endl;
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
	if (disk_info.BlockSID) {
		cout << "Block SID Authentication function (" << HEXON(4) << FC_BLOCKSID << ")" << HEXOFF << std::endl;
		cout << "    SID Blocked State = " << (disk_info.BlockSID_SIDBlockedState ? "Y" : "N");
		cout << ", SID Value State = " << (disk_info.BlockSID_SIDValueState ? "Y" : "N");
		cout << ", Hardware Reset = " << (disk_info.BlockSID_HardwareReset ? "Y" : "N");
		cout << std::endl;
	}
	if (disk_info.Namespace) {
		cout << "Namespace function (" << HEXON(4) << FC_NAMESPACE << ")" << HEXOFF << std::endl;
		cout << "    Maximum Key Count = " << disk_info.Namespace_MaximumKeyCount;
		cout << ", Unused Key Count = " << disk_info.Namespace_UnusedKeyCount;
		cout << ", Maximum Ranges Per Namespace = " << disk_info.Namespace_MaximumRangesPerNamespace;
		cout << std::endl;
	}
	if (disk_info.Opalite) {
		cout << "Opalite function (" << HEXON(4) << FC_OPALITE << ")" << HEXOFF << std::endl;
		cout << "    Base comID = " << HEXON(4) << disk_info.Opalite_basecomID << HEXOFF;
		cout << ", comIDs = " << disk_info.Opalite_numcomIDs;
		cout << ", Initial PIN = " << HEXON(2) << disk_info.Opalite_initialPIN << HEXOFF;
		cout << ", Reverted PIN = " << HEXON(2) << disk_info.Opalite_revertedPIN << HEXOFF;
		cout << std::endl;
	}
	if (disk_info.Pyrite10) {
		cout << "Pyrite 1.0 function (" << HEXON(4) << FC_PYRITEV100 << ")" << HEXOFF << std::endl;
		cout << "    Base comID = " << HEXON(4) << disk_info.Pyrite10_basecomID << HEXOFF;
		cout << ", comIDs = " << disk_info.Pyrite10_numcomIDs;
		cout << ", Initial PIN = " << HEXON(2) << disk_info.Pyrite10_initialPIN << HEXOFF;
		cout << ", Reverted PIN = " << HEXON(2) << disk_info.Pyrite10_revertedPIN << HEXOFF;
		cout << std::endl;
	}
	if (disk_info.Pyrite20) {
		cout << "Pyrite 2.0 function (" << HEXON(4) << FC_PYRITEV200 << ")" << HEXOFF << std::endl;
		cout << "    Base comID = " << HEXON(4) << disk_info.Pyrite20_basecomID << HEXOFF;
		cout << ", comIDs = " << disk_info.Pyrite20_numcomIDs;
		cout << ", Initial PIN = " << HEXON(2) << disk_info.Pyrite20_initialPIN << HEXOFF;
		cout << ", Reverted PIN = " << HEXON(2) << disk_info.Pyrite20_revertedPIN << HEXOFF;
		cout << std::endl;
	}
	if (disk_info.Ruby10) {
		cout << "Ruby 1.0 function (" << HEXON(4) << FC_RUBYV100 << ")" << HEXOFF << std::endl;
		cout << "    Base comID = " << HEXON(4) << disk_info.Ruby10_basecomID << HEXOFF;
		cout << ", comIDs = " << disk_info.Ruby10_numcomIDs;
		cout << ", Initial PIN = " << HEXON(2) << disk_info.Ruby10_initialPIN << HEXOFF;
		cout << ", Reverted PIN = " << HEXON(2) << disk_info.Ruby10_revertedPIN << HEXOFF;
		cout << ", PINonTPerRevert = " << HEXON(2)  << disk_info.Ruby10_PINonTPerRevert << HEXOFF;
		cout << std::endl;
		cout << "    Locking Admins = " << disk_info.Ruby10_numAdmins;
		cout << ", Locking Users = " << disk_info.Ruby10_numUsers;
		cout << ", Range Crossing = " << (disk_info.Ruby10_rangeCrossing ? "Y" : "N");
		cout << std::endl;
	}
	if (disk_info.DataRem) {
		cout << "Supported Data Removal Mechanism function (" << HEXON(4) << FC_DATAREM << ")" << HEXOFF << std::endl;
		cout << "    Processing = " << (disk_info.DataRem_processing ? "Y" : "N");
		string types[6] = { "Overwrite", "Block", "Crypto", "Unmap", "Reset Write Pointers", "Vendor Specific" };
		for (int i = 0; i < 6; i++) {
			if ((disk_info.DataRem_supported & (1 << i)) == 0)
				continue;
			cout << ", " << types[i];
			cout << " = " << (disk_info.DataRem_time[i] * 2) <<
			    (((disk_info.DataRem_format & (1 << i)) == 0) ? "s " : "m ");
		}
		cout << std::endl;
	}
	if (disk_info.NSGeometry) {
		cout << "Namespace Geometry function (" << HEXON(4) << FC_NSGEOMETRY << HEXOFF << ")" << std::endl;
		cout << "    Align = " << (disk_info.NSGeometry_align ? "Y, " : "N, ")
			<< "Alignment Granularity = " << disk_info.NSGeometry_alignmentGranularity
			<< " (" << // display bytes
			(disk_info.NSGeometry_alignmentGranularity *
			disk_info.NSGeometry_logicalBlockSize)
			<< ")"
			<< ", Logical Block size = " << disk_info.NSGeometry_logicalBlockSize
			<< ", Lowest Aligned LBA = " << disk_info.NSGeometry_lowestAlignedLBA
			<< std::endl;
	}
	if (disk_info.Unknown)
		cout << "**** " << (uint16_t)disk_info.Unknown << " **** Unknown function codes IGNORED " << std::endl;
}
