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
#include "DtaHashPwd.h"

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
	d0Response = discovery0buffer + IO_BUFFER_ALIGNMENT;
	d0Response = (void *)((uintptr_t)d0Response & (uintptr_t)~(IO_BUFFER_ALIGNMENT - 1));
	memset(d0Response, 0, MIN_BUFFER_LENGTH);
    if ((lastRC = sendCmd(IF_RECV, 0x01, 0x0001, d0Response, MIN_BUFFER_LENGTH)) != 0) {
        LOG(D) << "Send D0 request to device failed " << (uint16_t)lastRC;
        return;
    }

    epos = cpos = (uint8_t *) d0Response;
    hdr = (Discovery0Header *) d0Response;
    LOG(D3) << "Dumping D0Response";
    IFLOG(D3) DtaHexDump(hdr, SWAP32(hdr->length));
    epos = epos + SWAP32(hdr->length);
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
            disk_info.Locking_mediaEncrypt = body->locking.mediaEncryption;
            break;
        case FC_GEOMETRY: /* Geometry Features */
            disk_info.Geometry = 1;
            disk_info.Geometry_align = body->geometry.align;
            disk_info.Geometry_alignmentGranularity = SWAP64(body->geometry.alignmentGranularity);
            disk_info.Geometry_logicalBlockSize = SWAP32(body->geometry.logicalBlockSize);
            disk_info.Geometry_lowestAlignedLBA = SWAP64(body->geometry.lowestAlighedLBA);
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

uint8_t DtaDev::printPasswordHash(char * password)
{
    LOG(D1) << "Entering DtaDev::printPasswordHash()";
    vector<uint8_t> hash;
    DtaHashPwd(hash, password, this);

    /* std::hex overwrites flags; save them, so we do not alter other output later */
    ios_base::fmtflags saved_flags = cout.flags();

    /* First two bytes are actually the opal header */
    for (size_t i = 2; i < hash.size(); ++i)
        cout << hex << setfill('0') << setw(2) << (int)hash[i];
    cout << endl;
    cout.flags(saved_flags);
    return 0;
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
	if (disk_info.Unknown)
		cout << "**** " << (uint16_t)disk_info.Unknown << " **** Unknown function codes IGNORED " << std::endl;
}

uint8_t DtaDev::prepareForS3Sleep(uint8_t lockingrange, char* password)
{
    LOG(E) << "S3 sleep not supported on this platform";
    return 1;
}
