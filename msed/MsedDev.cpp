/* C:B**************************************************************************
This software is Copyright 2014,2015 Michael Romeo <r0m30@r0m30.com>

This file is part of msed.

msed is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

msed is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with msed.  If not, see <http://www.gnu.org/licenses/>.

 * C:E********************************************************************** */
/** Base device class.
 * An OS port must create a subclass of this class
 * implementing sendcmd, osmsSleep and identify 
 * specific to the IO requirements of that OS
 */
#include "os.h"
#include <stdio.h>
#include <iostream>
#include "MsedDev.h"
#include "MsedEndianFixup.h"
#include "MsedStructures.h"
#include "MsedCommand.h"
#include "MsedResponse.h"
#include "MsedSession.h"
#include "MsedHexDump.h"

using namespace std;

/** Device Class (Base) represents a single disk device.
 *  This is the functionality that is common to all OS's and SSC's
 */
MsedDev::MsedDev()
{
}
MsedDev::~MsedDev()
{
}
uint8_t MsedDev::isOpal2()
{
	LOG(D1) << "Entering MsedDev::isOpal2 " << (uint16_t) disk_info.OPAL20;
	return disk_info.OPAL20;
}
uint8_t MsedDev::isOpal1()
{
	LOG(D1) << "Entering MsedDev::isOpal1() " << (uint16_t)disk_info.OPAL10;
    return disk_info.OPAL10;
}
uint8_t MsedDev::isEprise()
{
    LOG(D1) << "Entering MsedDev::isEprise " << (uint16_t) disk_info.Enterprise;
    return disk_info.Enterprise;
}
uint8_t MsedDev::isSupportedSSC()
{
	LOG(D1) << "Entering MsedDev::isSupportedSSC " << (uint16_t)disk_info.SupportedSSC;
	return disk_info.SupportedSSC;
}
uint8_t MsedDev::isAnySSC()
{
	LOG(D1) << "Entering MsedDev::isAnySSC " << (uint16_t)disk_info.ANY_OPAL_SSC;
	return disk_info.ANY_OPAL_SSC;
}
uint8_t MsedDev::isPresent()
{
	LOG(D1) << "Entering MsedDev::isPresent() " << (uint16_t) isOpen;
    return isOpen;
}
char *MsedDev::getFirmwareRev()
{
	return (char *)&disk_info.firmwareRev;
}
char *MsedDev::getModelNum()
{
	return (char *)&disk_info.modelNum;
}
char *MsedDev::getSerialNum()
{
	return (char *)&disk_info.serialNum;
}
void MsedDev::discovery0()
{
    LOG(D1) << "Entering MsedDev::discovery0()";
    void * d0Response = NULL;
    uint8_t * epos, *cpos;
    Discovery0Header * hdr;
    Discovery0Features * body;
    d0Response = ALIGNED_ALLOC(4096, IO_BUFFER_LENGTH);
    if (NULL == d0Response) return;
    memset(d0Response, 0, IO_BUFFER_LENGTH);
    if (sendCmd(IF_RECV, 0x01, 0x0001, d0Response, IO_BUFFER_LENGTH)) {
        ALIGNED_FREE(d0Response);
        return;
    }

    epos = cpos = (uint8_t *) d0Response;
    hdr = (Discovery0Header *) d0Response;
    LOG(D3) << "Dumping D0Response";
    IFLOG(D3) MsedHexDump(hdr, SWAP32(hdr->length));
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
			disk_info.SupportedSSC = 1;
            disk_info.Enterprise_rangeCrossing = body->enterpriseSSC.rangeCrossing;
            disk_info.Enterprise_basecomID = SWAP16(body->enterpriseSSC.baseComID);
            disk_info.Enterprise_numcomID = SWAP16(body->enterpriseSSC.numberComIDs);
            break;
        case FC_OPALV100: /* Opal V1 */
            disk_info.OPAL10 = 1;
			disk_info.ANY_OPAL_SSC = 1;
			disk_info.SupportedSSC = 1;
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
			disk_info.SupportedSSC = 1;
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
    ALIGNED_FREE(d0Response);
}
