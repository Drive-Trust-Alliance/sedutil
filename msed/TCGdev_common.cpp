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
#include "os.h"
#include <stdio.h>
#include <iostream>
#include "TCGdev.h"
#include "endianfixup.h"
#include "TCGstructures.h"
#include "hexDump.h"

using namespace std;

/** Device Class (Common) represents a single disk device.
 *  This is the functionality that is common to all OS's
 *  Later this should be made into a base class that the OD classes
 *  inherit from
 */
uint8_t TCGdev::isOpal2()
{
    LOG(D4) << "Entering TCGdev::isOpal2()";
    return disk_info.OPAL20;
}

uint8_t TCGdev::isPresent()
{
    LOG(D4) << "Entering TCGdev::isPresent()";
    return isOpen;
}

uint16_t TCGdev::comID()
{
    LOG(D4) << "Entering TCGdev::comID()";
    if (disk_info.OPAL20)
        return disk_info.OPAL20_basecomID;
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
void TCGdev::discovery0()
{
    LOG(D4) << "Entering TCGdev::discovery0()";
    void * d0Response = NULL;
    uint8_t * epos, *cpos;
    Discovery0Header * hdr;
    Discovery0Features * body;
    d0Response = ALIGNED_ALLOC(4096, IO_BUFFER_LENGTH);
    if (NULL == d0Response) return;
    memset(d0Response, 0, IO_BUFFER_LENGTH);
    uint8_t iorc = sendCmd(IF_RECV, 0x01, 0x0001, d0Response,
                           IO_BUFFER_LENGTH);
    if (0x00 != iorc) {
        ALIGNED_FREE(d0Response);
        return;
    }

    epos = cpos = (uint8_t *) d0Response;
    hdr = (Discovery0Header *) d0Response;
    LOG(D3) << "Dumping D0Response";
    IFLOG(D3) hexDump(hdr, SWAP32(hdr->length));
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
            disk_info.Geometry_alignmentGranularity = SWAP32(body->geometry.alignmentGranularity);
            disk_info.Geometry_logicalBlockSize = SWAP32(body->geometry.logicalBlockSize);
            disk_info.Geometry_lowestAlignedLBA = SWAP32(body->geometry.lowestAlighedLBA);
            break;
        case FC_ENTERPRISE: /* Enterprise SSC */
            disk_info.Enterprise = 1;
            disk_info.Enterprise_rangeCrossing = body->enterpriseSSC.rangeCrossing;
            disk_info.Enterprise_basecomID = SWAP16(body->enterpriseSSC.baseComID);
            disk_info.Enterprise_numcomID = SWAP16(body->enterpriseSSC.numberComIDs);
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
            disk_info.OPAL20_basecomID = SWAP16(body->opalv200.baseCommID);
            disk_info.OPAL20_initialPIN = body->opalv200.initialPIN;
            disk_info.OPAL20_revertedPIN = body->opalv200.revertedPIN;
            disk_info.OPAL20_numcomIDs = SWAP16(body->opalv200.numCommIDs);
            disk_info.OPAL20_numAdmins = SWAP16(body->opalv200.numlockingAdminAuth);
            disk_info.OPAL20_numUsers = SWAP16(body->opalv200.numlockingUserAuth);
            disk_info.OPAL20_rangeCrossing = body->opalv200.rangeCrossing;
            break;
        default:
            disk_info.Unknown += 1;
            LOG(D) << "Unknown Feature in Discovery 0 response " << std::hex << SWAP16(body->TPer.featureCode) << std::dec;
            /* should do something here */
            break;
        }
        cpos = cpos + (body->TPer.length + 4);
    }
    while (cpos < epos);
    ALIGNED_FREE(d0Response);
}

/** Print out the Discovery 0 results */
void TCGdev::puke()
{
    LOG(D4) << "Entering TCGdev::puke()";
    char scratch[25];
    /* TPer */
    if (disk_info.TPer) {
        printf("\nTPer function (0x%04x)\n", FC_TPER);
        cout << "ACKNAK           = " << (disk_info.TPer_ACKNACK ? "Y" : "N") << std::endl;
        cout << "ASYNC            = " << (disk_info.TPer_async ? "Y" : "N") << std::endl;
        cout << "BufferManagement = " << (disk_info.TPer_bufferMgt ? "Y" : "N") << std::endl;
        cout << "comIDManagement  = " << (disk_info.TPer_comIDMgt ? "Y" : "N") << std::endl;
        cout << "Streaming        = " << (disk_info.TPer_streaming ? "Y" : "N") << std::endl;
        cout << "SYNC             = " << (disk_info.TPer_sync ? "Y" : "N") << std::endl;
        cout << std::endl;
    }
    if (disk_info.Locking) {
        printf("\nLocking functions (0x%04x)\n", FC_LOCKING);
        cout << "Locked             = " << (disk_info.Locking_locked ? "Y" : "N") << std::endl;
        cout << "LockingEnabled     = " << (disk_info.Locking_lockingEnabled ? "Y" : "N") << std::endl;
        cout << "LockingSupported   = " << (disk_info.Locking_lockingSupported ? "Y" : "N") << std::endl;
        cout << "MBRDone            = " << (disk_info.Locking_MBRDone ? "Y" : "N") << std::endl;
        cout << "MBREnabled         = " << (disk_info.Locking_MBREnabled ? "Y" : "N") << std::endl;
        cout << "MediaEncrypt       = " << (disk_info.Locking_mediaEncrypt ? "Y" : "N") << std::endl;
        cout << std::endl;
    }
    if (disk_info.Geometry) {
        printf("\nGeometry functions (0x%04x)\n", FC_GEOMETRY);
        cout << "Align                 = " << (disk_info.Geometry_align ? "Y" : "N") << std::endl;
        cout << "Alignment Granularity = " << disk_info.Geometry_alignmentGranularity << std::endl;
        cout << "Logical Block size    = " << disk_info.Geometry_logicalBlockSize << std::endl;
        cout << "Lowest Aligned LBA    = " << disk_info.Geometry_lowestAlignedLBA << std::endl;
    }
    if (disk_info.Enterprise) {
        printf("\nEnterprise functions (0x%04x)\n", FC_ENTERPRISE);
        cout << "Range crossing      = " << (disk_info.Enterprise_rangeCrossing ? "Y" : "N") << std::endl;
        cout << "Base commID         = " << disk_info.Enterprise_basecomID << std::endl;
        cout << "Number of commIDs   = " << disk_info.Enterprise_numcomID << std::endl;
    }
    if (disk_info.SingleUser) {
        printf("\nSingleUser functions (0x%04x)\n", FC_SINGLEUSER);
        cout << "ALL                         = " << (disk_info.SingleUser_all ? "Y" : "N") << std::endl;
        cout << "ANY                         = " << (disk_info.SingleUser_any ? "Y" : "N") << std::endl;
        cout << "Policy                      = " << (disk_info.SingleUser_policy ? "Y" : "N") << std::endl;
        cout << "Number of Locking Objects   = " << (disk_info.SingleUser_lockingObjects) << std::endl;
    }
    if (disk_info.DataStore) {
        printf("\nDataStore functions (0x%04x)\n", FC_DATASTORE);
        cout << "Max Tables             = " << disk_info.DataStore_maxTables << std::endl;
        cout << "Max Size Tables        = " << disk_info.DataStore_maxTableSize << std::endl;
        cout << "Table size alignment   = " << disk_info.DataStore_alignment << std::endl;
    }
    if (disk_info.OPAL20) {
        printf("\nOPAL 2.0 functions (0x%04x)\n", FC_OPALV200);
        SNPRINTF(scratch, 8, "0x%04x", disk_info.OPAL20_basecomID);
        cout << "Base commID                = " << scratch << std::endl;
        SNPRINTF(scratch, 8, "0x%02x", disk_info.OPAL20_initialPIN);
        cout << "Initial PIN                = " << scratch << std::endl;
        SNPRINTF(scratch, 8, "0x%02x", disk_info.OPAL20_revertedPIN);
        cout << "Reverted PIN               = " << scratch << std::endl;
        cout << "Number of commIDs          = " << disk_info.OPAL20_numcomIDs << std::endl;
        cout << "Number of Locking Admins   = " << disk_info.OPAL20_numAdmins << std::endl;
        cout << "Number of Locking Users    = " << disk_info.OPAL20_numUsers << std::endl;
        cout << "Range Crossing             = " << (disk_info.OPAL20_rangeCrossing ? "Y" : "N") << std::endl;
    }
    if (disk_info.Unknown)
        cout << disk_info.Unknown << " unknown function codes IGNORED " << std::endl;
}
