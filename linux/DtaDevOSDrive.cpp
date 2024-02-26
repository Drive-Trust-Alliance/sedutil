/* C:B**************************************************************************
   This software is Copyright (c) 2014-2024 Bright Plaza Inc. <drivetrust@drivetrust.com>

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

#include <cstdint>
#include <cstring>
#include "os.h"
#include "DtaEndianFixup.h"
#include "DtaHexDump.h"
#include "DtaDevLinuxDrive.h"

static
void parseDiscovery0Features(const uint8_t * d0Response, DTA_DEVICE_INFO & di)
{
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
      LOG(D) << "Level 0 Discovery header length abnormal " << std::hex << length;
      return;
    }
  IFLOG(D3) DtaHexDump(hdr, length);

  uint8_t *cpos = (uint8_t *) d0Response + 48; // TODO: check header version
  uint8_t *epos = (uint8_t *) d0Response + length;

  do {
    Discovery0Features * body = (Discovery0Features *) cpos;
    uint16_t featureCode = SWAP16(body->TPer.featureCode);
    LOG(D2) << "Discovery0 FeatureCode: " << std::hex << featureCode;
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

uint8_t DtaDevLinuxDrive::discovery0(DTA_DEVICE_INFO & disk_info) {
  uint8_t d0Response[MIN_BUFFER_LENGTH]; // TODO: ALIGNMENT?
  memset(d0Response, 0, MIN_BUFFER_LENGTH);

  uint8_t lastRC = sendCmd(IF_RECV, 0x01, 0x0001, d0Response, MIN_BUFFER_LENGTH);
  if ((lastRC ) != 0) {
    LOG(D) << "Acquiring Discovery 0 response failed " << (uint16_t)lastRC;
    return DTAERROR_COMMAND_ERROR;
  }
  parseDiscovery0Features(d0Response, disk_info);
  return DTAERROR_SUCCESS;
}


int DtaDevLinuxDrive::fdopen(const char * devref)
{
  if (access(devref, R_OK | W_OK)) {
    LOG(E) << "You do not have permission to access the raw device in write mode";
    LOG(E) << "Perhaps you might try sudo to run as root";
  }

  int fd = open(devref, O_RDWR);

  if (fd < 0) {
    LOG(E) << "Error opening device " << devref << " " << (int32_t) fd;
    //        if (-EPERM == fd) {
    //            LOG(E) << "You do not have permission to access the raw disk in write mode";
    //            LOG(E) << "Perhaps you might try sudo to run as root";
    //        }
  }
  return fd;
}

void DtaDevLinuxDrive::fdclose()
{
  if (0 <= fd) {
    LOG(D4) << "Closing device file handle " << (int32_t) fd;
    close(fd);
  }
}
