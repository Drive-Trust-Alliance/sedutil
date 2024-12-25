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
/** Base device class.
 * An OS port must create subclasses of this class
 * implementing the SWG SSCs and holding an instance
 * of a "drive" object that implements sendcmd and identify
 * specific to the IO requirements of that OS
 */
#include "log.h"
#include <iostream>
#include <algorithm>
#include <time.h>
#include "DtaDev.h"
#include "DtaHexDump.h"
#include "DtaSession.h"
#include "DtaCommand.h"
#include "DtaJson.h"

using namespace std;

/** Device Class (Base) represents a single disk device.
 *  This is the functionality that is common to all OS's and SSC's
 */
DtaDev::DtaDev()
{
}

uint8_t DtaDev::isRuby()
{
  LOG(D2) << "Entering DtaDev::isRuby " << (uint16_t)device_info.RUBY;
  return device_info.RUBY;
}

uint8_t DtaDev::isFIPS()
{
  LOG(D2) << "Entering DtaDev::isFIPS " << (uint16_t)device_info.fips;
  return device_info.fips;
}

uint8_t DtaDev::isOpalite()
{
  LOG(D2) << "Entering DtaDev::isOpalite " << (uint16_t) device_info.OPALITE;
  return device_info.OPALITE;
}
uint8_t DtaDev::isPyrite2()
{
  LOG(D2) << "Entering DtaDev::isPyrite2 " << (uint16_t)device_info.PYRITE2;
  return device_info.PYRITE2;
}
uint8_t DtaDev::isPyrite()
{
  LOG(D2) << "Entering DtaDev::isPyrite " << (uint16_t) device_info.PYRITE;
  return device_info.PYRITE;
}
uint8_t DtaDev::isOpal2_minor_v()
{
  LOG(D2) << "Entering DtaDev::isOpal2_minor " << (uint16_t)device_info.OPAL20_minor_v;
  return device_info.OPAL20_minor_v;
}
uint8_t DtaDev::isOpal2_version()
{
  LOG(D2) << "Entering DtaDev::isOpal2_version " << (uint16_t)device_info.OPAL20_version;
  return device_info.OPAL20_version;
}
uint8_t DtaDev::isOpal2()
{
  LOG(D2) << "Entering DtaDev::isOpal2 " << (uint16_t) device_info.OPAL20;
  return device_info.OPAL20;
}
uint8_t DtaDev::isOpal1()
{
  LOG(D2) << "Entering DtaDev::isOpal1() " << (uint16_t)device_info.OPAL10;
  return device_info.OPAL10;
}
uint8_t DtaDev::isEprise()
{
  LOG(D2) << "Entering DtaDev::isEprise " << (uint16_t) device_info.Enterprise;
  return device_info.Enterprise;
}
uint8_t DtaDev::isAnySSC()
{
  LOG(D2) << "Entering DtaDev::isAnySSC " << (uint16_t)device_info.ANY_OPAL_SSC;
  return device_info.ANY_OPAL_SSC;
}
uint8_t DtaDev::isPresent()
{
  LOG(D2) << "Entering DtaDev::isPresent() " << (uint16_t) isOpen;
  return isOpen;
}
uint8_t DtaDev::isNVMEbus()
{
  LOG(D2) << "Entering DtaDev::isNVMEbus() " << (uint16_t)isNVME;
  return isNVME;
}
uint8_t DtaDev::MBREnabled()
{
  LOG(D2) << "Entering DtaDev::MBRENabled" << (uint16_t)device_info.Locking_MBREnabled;
  return device_info.Locking_MBREnabled;
}
uint8_t DtaDev::MBRDone()
{
  LOG(D2) << "Entering DtaDev::MBRDone" << (uint16_t)device_info.Locking_MBRDone;
  return device_info.Locking_MBRDone;
}
uint8_t DtaDev::Locked()
{
  LOG(D2) << "Entering DtaDev::Locked" << (uint16_t)device_info.Locking_locked;
  return device_info.Locking_locked;
}
uint8_t DtaDev::LockingEnabled()
{
  LOG(D2) << "Entering DtaDev::LockingEnabled" << (uint16_t)device_info.Locking_lockingEnabled;
  return device_info.Locking_lockingEnabled;
}
char *DtaDev::getFirmwareRev()
{
  return (char *)&device_info.firmwareRev;
}
char *DtaDev::getModelNum()
{
  return (char *)&device_info.modelNum;
}
char *DtaDev::getSerialNum()
{
  return (char *)&device_info.serialNum;
}
vector<uint8_t>DtaDev::getPasswordSalt()
{
  const uint8_t * b=device_info.passwordSalt;
  return vector<uint8_t>(b,b+sizeof(device_info.passwordSalt));
}
DTA_DEVICE_TYPE DtaDev::getDevType()
{
  return device_info.devType;
}

char *DtaDev::getVendorID()
{
  return (char *)&device_info.vendorID;
}

char *DtaDev::getManufacturerName()
{
  return (char *)&device_info.manufacturerName;
}

vector<uint8_t>DtaDev::getWorldWideName()
{
  const uint8_t * b=device_info.worldWideName;
  return vector<uint8_t>(b,b+sizeof(device_info.worldWideName));
}

bool DtaDev::isWorldWideNameSynthetic()
{
    return 0!=device_info.worldWideNameIsSynthetic;
}


int DtaDev::TperReset()
{
  LOG(D2) << "Entering DtaDev::TperReset()";

  void * tpResponse = OS.alloc_aligned_MIN_BUFFER_LENGTH_buffer();
  memset(tpResponse, 0, MIN_BUFFER_LENGTH);

  // TperReset ProtocolID=0x02 ComID=0x0004
  int lastRC = sendCmd(TRUSTED_SEND, 0x02, 0x0004, tpResponse, 512);

  if (0 != lastRC) {
    LOG(E) << "TperReset failed with error code: " << lastRC;
  } else {
    IFLOG(D2) DtaHexDump((char *)tpResponse,64);
  }

  OS.free_aligned_MIN_BUFFER_LENGTH_buffer (tpResponse);
  return lastRC;
}


void DtaDev::puke()
{
  LOG(D2) << "Entering DtaDev::puke";
    if (outputFormat==sedutilJSON) {
        cout << JS::serializeStruct(device_info) << endl ;
        return;
    }
    if (outputFormat==sedutilJSONCompact) {
        cout << JS::serializeStruct(device_info, JS::SerializerOptions(JS::SerializerOptions::Compact)) << endl ;
        return;
    }
  /* IDENTIFY */
  const char * devType =
      device_info.devType == DEVICE_TYPE_ATA  ? " ATA "
    : device_info.devType == DEVICE_TYPE_SAS  ? " SAS "
    : device_info.devType == DEVICE_TYPE_USB  ? " USB "
    : device_info.devType == DEVICE_TYPE_NVME ? " NVMe "
    :                                           " OTHER ";
  cout << dev << devType << "  " << device_info.modelNum << "  " << device_info.firmwareRev << "   " << device_info.serialNum;
  IFLOG(D) {
    char WWN[19]="                ";  // 16 blanks as placeholder if missing
    uint8_t (&wwn)[8] = device_info.worldWideName;
    if (__is_not_all_NULs(wwn, sizeof(wwn))) {
      snprintf(WWN, 19, "%02X%02X%02X%02X%02X%02X%02X%02X %c",
               wwn[0], wwn[1], wwn[2], wwn[3], wwn[4], wwn[5], wwn[6], wwn[7],
               device_info.worldWideNameIsSynthetic ? '*' : ' ');
    }
    cout << "  " << WWN
         << "  " << device_info.vendorID
         << "   " << device_info.manufacturerName;
  }
  cout << endl;

  /* TPer */
  if (device_info.TPer) {
    cout << "TPer function (" << HEXON(4) << FC_TPER << HEXOFF << ")" << endl;
    cout << "    ACKNAK = " << (device_info.TPer_ACKNACK ? "Y, " : "N, ")
         << "ASYNC = " << (device_info.TPer_async ? "Y, " : "N. ")
         << "BufferManagement = " << (device_info.TPer_bufferMgt ? "Y, " : "N, ")
         << "comIDManagement  = " << (device_info.TPer_comIDMgt ? "Y, " : "N, ")
         << "Streaming = " << (device_info.TPer_streaming ? "Y, " : "N, ")
         << "SYNC = " << (device_info.TPer_sync ? "Y" : "N")
         << endl;
  }
  if (device_info.Locking) {

    cout << "Locking function (" << HEXON(4) << FC_LOCKING << HEXOFF << ")" << endl;
    cout << "    Locked = " << (device_info.Locking_locked ? "Y, " : "N, ")
         << "LockingEnabled = " << (device_info.Locking_lockingEnabled ? "Y, " : "N, ")
         << "MBR shadowing Not Supported = " << (device_info.Locking_MBRshadowingNotSupported ? "Y, " : "N, ")
         << "MBRDone = " << (device_info.Locking_MBRDone ? "Y, " : "N, ")
         << "MBREnabled = " << (device_info.Locking_MBREnabled ? "Y, " : "N, ")
         << "MediaEncrypt = " << (device_info.Locking_mediaEncrypt ? "Y" : "N")
         << endl;
  }
  if (device_info.Geometry) {

    cout << "Geometry function (" << HEXON(4) << FC_GEOMETRY << HEXOFF << ")" << endl;
    cout << "    Align = " << (device_info.Geometry_align ? "Y, " : "N, ")
         << "Alignment Granularity = " << device_info.Geometry_alignmentGranularity
         << " (" << // display bytes
      (device_info.Geometry_alignmentGranularity *
       device_info.Geometry_logicalBlockSize)
         << ")"
         << ", Logical Block size = " << device_info.Geometry_logicalBlockSize
         << ", Lowest Aligned LBA = " << device_info.Geometry_lowestAlignedLBA
         << endl;
  }
  if (device_info.Enterprise) {
    cout << "Enterprise function (" << HEXON(4) << FC_ENTERPRISE << HEXOFF << ")" << endl;
    cout << "    Range crossing = " << (device_info.Enterprise_rangeCrossing ? "Y, " : "N, ")
         << "Base comID = " << HEXON(4) << device_info.Enterprise_basecomID
         << ", comIDs = " << device_info.Enterprise_numcomID << HEXOFF
         << endl;
  }
  if (device_info.OPAL10) {
    cout << "Opal V1.0 function (" << HEXON(4) << FC_OPALV100 << HEXOFF << ")" << endl;
    cout << "Base comID = " << HEXON(4) << device_info.OPAL10_basecomID << HEXOFF
         << ", comIDs = " << device_info.OPAL10_numcomIDs
         << endl;
  }
  if (device_info.SingleUser) {
    cout << "SingleUser function (" << HEXON(4) << FC_SINGLEUSER << HEXOFF << ")" << endl;
    cout << "    ALL = " << (device_info.SingleUser_all ? "Y, " : "N, ")
         << "ANY = " << (device_info.SingleUser_any ? "Y, " : "N, ")
         << "Policy = " << (device_info.SingleUser_policy ? "Y, " : "N, ")
         << "Locking Objects = " << (device_info.SingleUser_lockingObjects)
         << endl;
  }
  if (device_info.DataStore) {
    cout << "DataStore function (" << HEXON(4) << FC_DATASTORE << HEXOFF << ")" << endl;
    cout << "    Max Tables = " << device_info.DataStore_maxTables
         << ", Max Size Tables = " << device_info.DataStore_maxTableSize
         << ", Table size alignment = " << device_info.DataStore_alignment
         << endl;
  }

  if (device_info.OPAL20) {
    cout << "OPAL 2." << ((device_info.OPAL20_version -1) & 0xf) << " function (" << HEXON(4) << FC_OPALV200 << ")" << HEXOFF << endl;
    cout << "    Base comID = " << HEXON(4) << device_info.OPAL20_basecomID << HEXOFF;
    cout << ", Initial PIN = " << HEXON(2) << static_cast<uint32_t>(device_info.OPAL20_initialPIN) << HEXOFF;
    cout << ", Reverted PIN = " << HEXON(2) << static_cast<uint32_t>(device_info.OPAL20_revertedPIN) << HEXOFF;
    cout << ", comIDs = " << device_info.OPAL20_numcomIDs;
    cout << endl;
    cout << "    Locking Admins = " << device_info.OPAL20_numAdmins;
    cout << ", Locking Users = " << device_info.OPAL20_numUsers;
    cout << ", Range Crossing = " << (device_info.OPAL20_rangeCrossing ? "Y" : "N");
    cout << endl;
  }
  if (device_info.OPALITE) {
    cout << "OPALITE 1." << ((device_info.OPALITE_version & 0xf) - 1) << " function (" << HEXON(4) << FC_OPALITE << ")" << HEXOFF << endl;
    cout << "    Base comID = " << HEXON(4) << device_info.OPALITE_basecomID << HEXOFF;
    cout << ", Initial PIN = " << HEXON(2) << device_info.OPALITE_initialPIN << HEXOFF;
    cout << ", Reverted PIN = " << HEXON(2) << device_info.OPALITE_revertedPIN << HEXOFF;
    cout << ", comIDs = " << device_info.OPALITE_numcomIDs;
    cout << "    Locking Admins = " << device_info.OPAL20_numAdmins;
    cout << ", Locking Users = " << device_info.OPAL20_numUsers;
    cout << endl;
  }
  if (device_info.PYRITE) {
    cout << "PYRITE 1." << ((device_info.PYRITE_version & 0xf) -1) << " function (" << HEXON(4) << FC_PYRITE << ")" << HEXOFF << endl;
    cout << "    Base comID = " << HEXON(4) << device_info.PYRITE_basecomID << HEXOFF;
    cout << ", Initial PIN = " << HEXON(2) << device_info.PYRITE_initialPIN << HEXOFF;
    cout << ", Reverted PIN = " << HEXON(2) << device_info.PYRITE_revertedPIN << HEXOFF;
    cout << ", comIDs = " << device_info.PYRITE_numcomIDs;
    cout << "    Locking Admins = " << device_info.OPAL20_numAdmins;
    cout << ", Locking Users = " << device_info.OPAL20_numUsers;
    cout << endl;
  }
  if (device_info.PYRITE2) {
    cout << "PYRITE 2." << ((device_info.PYRITE2_version & 0xf) - 1) << " function (" << HEXON(4) << FC_PYRITE << ")" << HEXOFF << endl;
    cout << "    Base comID = " << HEXON(4) << device_info.PYRITE2_basecomID << HEXOFF;
    cout << ", Initial PIN = " << HEXON(2) << device_info.PYRITE2_initialPIN << HEXOFF;
    cout << ", Reverted PIN = " << HEXON(2) << device_info.PYRITE2_revertedPIN << HEXOFF;
    cout << ", comIDs = " << device_info.PYRITE2_numcomIDs;
    cout << "    Locking Admins = " << device_info.OPAL20_numAdmins;
    cout << ", Locking Users = " << device_info.OPAL20_numUsers;
    cout << endl;
  }
  if (device_info.RUBY) {
    cout << "RUBY 1." << ((device_info.RUBY_version & 0xf) - 1) << " function (" << HEXON(4) << FC_RUBY << ")" << HEXOFF << endl;
    cout << "    Base comID = " << HEXON(4) << device_info.RUBY_basecomID << HEXOFF;
    cout << ", Initial PIN = " << HEXON(2) << device_info.RUBY_initialPIN << HEXOFF;
    cout << ", Reverted PIN = " << HEXON(2) << device_info.RUBY_revertedPIN << HEXOFF;
    cout << ", comIDs = " << device_info.RUBY_numcomIDs;
    cout << "    Locking Admins = " << device_info.OPAL20_numAdmins;
    cout << ", Locking Users = " << device_info.OPAL20_numUsers;
    cout << endl;
  }
  if (device_info.BlockSID) {
    cout << "BlockSID function (" << HEXON(4) << FC_BlockSID << ")" << HEXOFF << endl;
    cout << "    BlockSIDState = " << (device_info.BlockSID_BlockSIDState ? "Y" : "N" );
    cout << ", SIDvalueState = " << (device_info.BlockSID_SIDvalueState? "1" : "0");
    cout << ", HardReset  = " << (device_info.BlockSID_HardReset? "1" : "0") ;
    cout << endl;
  }
  if (device_info.DataRemoval) {
    cout << "DataRemoval 1." << ((device_info.DataRemoval_version & 0xf) - 1) << " function (" << HEXON(4) << FC_DataRemoval << ")" << HEXOFF << endl;
    cout << "    DataRemoval OperationProcessing " << HEXON(2) << device_info.DataRemoval_OperationProcessing << HEXOFF;
    cout << ", DataRemoval Machanisim " << HEXON(2) << device_info.DataRemoval_Mechanism << HEXOFF << endl;
    cout << "    DataRemoval TimeFormat Bit 5 : " << HEXON(2) << device_info.DataRemoval_TimeFormat_Bit5 << " " << HEXON(4) << device_info.DataRemoval_Time_Bit5 << HEXOFF << endl;
    cout << "    DataRemoval TimeFormat Bit 4 : " << HEXON(2) << device_info.DataRemoval_TimeFormat_Bit4 << " " << HEXON(4) << device_info.DataRemoval_Time_Bit4 << HEXOFF << endl;
    cout << "    DataRemoval TimeFormat Bit 3 : " << HEXON(2) << device_info.DataRemoval_TimeFormat_Bit3 << " " << HEXON(4) << device_info.DataRemoval_Time_Bit3 << HEXOFF << endl;
    cout << "    DataRemoval TimeFormat Bit 2 : " << HEXON(2) << device_info.DataRemoval_TimeFormat_Bit2 << " " << HEXON(4) << device_info.DataRemoval_Time_Bit2 << HEXOFF << endl;
    cout << "    DataRemoval TimeFormat Bit 1 : " << HEXON(2) << device_info.DataRemoval_TimeFormat_Bit1 << " " << HEXON(4) << device_info.DataRemoval_Time_Bit1 << HEXOFF << endl;
    cout << "    DataRemoval TimeFormat Bit 0 : " << HEXON(2) << device_info.DataRemoval_TimeFormat_Bit0 << " " << HEXON(4) << device_info.DataRemoval_Time_Bit0 << HEXOFF << endl;
  }


  if (device_info.Unknown)
    cout << "**** " << (uint16_t)device_info.Unknown << " **** Unknown function codes IGNORED " << endl;

  LOG(D2) << "Exiting DtaDev::puke";
}


uint8_t DtaDev::WithSession(std::function<uint8_t(void)>startSessionFn,
                            std::function<uint8_t(void)>sessionBodyFn) {
  session = new DtaSession(this);
  if (NULL == session) {
    LOG(E) << "Unable to create session object " << dev;
    return DTAERROR_OBJECT_CREATE_FAILED;
  }


  uint8_t lastRC = startSessionFn();

  if (lastRC == 0) {
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




/** start an anonymous session
 * @param SP the Security Provider to start the session with */
uint8_t DtaDev::start(OPAL_UID SP){
  if (session == NULL)
    return DTAERROR_OBJECT_CREATE_FAILED;
  return session->start(SP);
}


/** Start an authenticated session (OPAL only)
 * @param SP the securitly provider to start the session with
 * @param password the password to start the session
 * @param SignAuthority the Signing authority (in a simple session this is the user)
 */
uint8_t DtaDev::start(OPAL_UID SP, char * password, OPAL_UID SignAuthority){
  if (session == NULL)
    return DTAERROR_OBJECT_CREATE_FAILED;
  return session->start(SP, password, SignAuthority);
}



/** Start an authenticated session (OPAL only)
 * @param SP the securitly provider to start the session with
 * @param HostChallenge the password to start the session
 * @param SignAuthority the Signing authority (in a simple session this is the user)
 */
uint8_t DtaDev::start(OPAL_UID SP, vector<uint8_t> HostChallenge, OPAL_UID SignAuthority){
  if (session == NULL)
    return DTAERROR_OBJECT_CREATE_FAILED;
  return session->start(SP, HostChallenge, SignAuthority);
}


/** Start an authenticated session (OPAL only)
 * @param SP the securitly provider to start the session with
 * @param password the password to start the session
 * @param SignAuthority the Signing authority (in a simple session this is the user)
 *  */
uint8_t DtaDev::start(OPAL_UID SP, char * password, vector<uint8_t> SignAuthority){
  if (session == NULL)
    return DTAERROR_OBJECT_CREATE_FAILED;
  return session->start(SP, password, SignAuthority);
}


/** Start an authenticated session (OPAL only)
 * @param SP the securitly provider to start the session with
 * @param HostChallenge the password to start the session
 * @param SignAuthority the Signing authority (in a simple session this is the user)
 *  */
uint8_t DtaDev::start(OPAL_UID SP, vector<uint8_t>  HostChallenge, vector<uint8_t> SignAuthority){
  if (session == NULL)
    return DTAERROR_OBJECT_CREATE_FAILED;
  return session->start(SP, HostChallenge, SignAuthority);
}








/** Factory method to produce instance of appropriate subclass
 *   Note that all of DtaDevGeneric, DtaDevEnterprise, DtaDevOpal, ... derive from DtaDev
 * @param devref             name of the device in the OS lexicon
 * @param dev                reference into which to store the address of the new instance
 * @param genericIfNotTPer   if true, store an instance of DtaDevGeneric for non-TPers;
 *                           if false, store NULL for non-TPers
 */
// static
uint8_t DtaDev::getDtaDev(const char * devref,
                          DtaDev * & dev,
                          bool genericIfNotTPer)
{
  // LOG(D4) << "DtaDev::getDtaDev(devref=\"" << devref << "\")";
  DTA_DEVICE_INFO di;
  memset(&di, 0, sizeof(di));

  bool accessDenied = false;

  DtaDrive * drive = DtaDrive::getDtaDrive(devref, di, accessDenied);
  if (drive == NULL || accessDenied) {
    dev = NULL;
    // LOG(D4) << "DtaDrive::getDtaDrive(\"" << devref <<  "\", di) returned NULL";
    if (!accessDenied && !genericIfNotTPer) {
      // LOG(E) << "(From DtaDev::getDtaDev)";
      // LOG(E) << "Invalid or unsupported device " << devref;
    }
    // LOG(D4) << "DtaDev::getDtaDev(devref=\"" << devref << "\") returning DTAERROR_COMMAND_ERROR";
    if (accessDenied) {
      LOG(D4) << "DtaDev::getDtaDev(devref=\"" << devref << "\") returning DTAERROR_DEVICE_ACCESS_DENIED";
      return DTAERROR_DEVICE_ACCESS_DENIED;
    } else {
      LOG(D4) << "DtaDev::getDtaDev(devref=\"" << devref << "\") returning DTAERROR_DEVICE_INVALID_OR_UNSUPPORTED";
      return DTAERROR_DEVICE_INVALID_OR_UNSUPPORTED;
    }
  }

  dev =  getDtaDev(devref, drive, di, genericIfNotTPer) ;

  if (dev == NULL) {

    delete drive;

    LOG(D4) << "getDtaDev(" << "\"" << devref <<  "\"" << ", "
            << "drive"                 << ", "
            << "disk_info"             << ", "
            << ( genericIfNotTPer ? "true" : "false" )
            <<  ")"
            << " returned NULL";

     if (accessDenied) {
      LOG(D4) << "DtaDev::getDtaDev(devref=\"" << devref << "\") returning DTAERROR_DEVICE_ACCESS_DENIED";
      return DTAERROR_DEVICE_ACCESS_DENIED;
    } else {
      LOG(D4) << "DtaDev::getDtaDev(devref=\"" << devref << "\") returning DTAERROR_DEVICE_INVALID_OR_UNSUPPORTED";
      return DTAERROR_DEVICE_INVALID_OR_UNSUPPORTED;
    }
  }


  LOG(D4) << "DtaDev::getDtaDev(devref=\"" << devref << "\") disk_info:";
  IFLOG(D4) DtaHexDump(&di, (int)sizeof(di));
  LOG(D4) << "DtaDev::getDtaDev(devref=\"" << devref << "\") returning DTAERROR_SUCCESS";
  return DTAERROR_SUCCESS;
}


/** The Device class represents a OS generic storage device.
 * At instantiation we determine if we create an instance of the NVMe or SATA or Scsi (SAS) derived class
 */

const unsigned long long DtaDev::getSize() { return device_info.devSize; }

uint8_t DtaDev::sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                          void * buffer, unsigned int bufferlen)
{
  if (!isOpen) return DTAERROR_DEVICE_NOT_OPEN; // drive open failed so this will too

  if (NULL == drive)
    {
      LOG(E) << "DtaDev::sendCmd ERROR - unknown drive type";
      return DTAERROR_DEVICE_TYPE_UNKNOWN;
    }

  return drive->sendCmd(cmd, protocol, comID, buffer, bufferlen);
}

bool DtaDev::identify(DTA_DEVICE_INFO& di)
{
  return drive->identify(di)
    &&   DTAERROR_SUCCESS == drive->discovery0(di);
}


typedef struct ddipair{
    ddipair(string dr, DTA_DEVICE_INFO di) : devref(dr), device_info(di) {};
    string devref;
    DTA_DEVICE_INFO device_info;
} ddipair;
JS_OBJ_EXT(ddipair, devref, device_info);

uint8_t DtaDev::diskScan()
{
  LOG(D1) << "Entering DtaDev:diskScan ";

  IFLOG(D1) {
    fprintf(Output2FILE::StreamStdout(), "Scanning for TCG SWG compliant disks (loglevel=%d)\n", CLog::Level());
  } else {
    fprintf(Output2FILE::StreamStdout(), "Scanning for Opal compliant disks\n");
  }

  bool accessDenied=false;
  vector<string> devRefs(OS.enumerateDtaDriveDevRefs(accessDenied));
  if (accessDenied) {
    OS.errorNoAccess(NULL);
    return DTAERROR_DEVICE_ACCESS_DENIED;
  }

  if (devRefs.size()!=0) {


      if (outputFormat == sedutilJSON || outputFormat == sedutilJSONCompact) {


          vector<ddipair>devs;

          for (string& devref : devRefs) {
              LOG(D4) << std::endl << "Scanning \"" << devref << "\" ...";
              DtaDev* dev = NULL;
              uint8_t result = getDtaDev(devref.c_str(), dev, true);
              if (DTAERROR_SUCCESS == result && dev != NULL) {
                  devs.push_back(ddipair(devref, dev->device_info));
                  delete dev;
              }
          }

          cout << JS::serializeStruct(devs,
                                      JS::SerializerOptions(outputFormat == sedutilJSON
                                                                ? JS::SerializerOptions::Pretty
                                                                : JS::SerializerOptions::Compact))
              << endl;

          return DTAERROR_SUCCESS;
      }




      // Deal with device names being of various sizes in various OSes.  E.g. in Windows, a devRef might be
    // as long as "\\.\PhysicalDrive123" while on linux they might all be as short as "/dev/sda"

      vector<string>::iterator longest_devRef{max_element(devRefs.begin(), devRefs.end(),
                                                          [](string a, string b){
                                                            return a.length() < b.length();
                                                          })};
    size_t const longest_devRef_length{longest_devRef->length()};
    string column_header{" device "};
    size_t column_header_width{column_header.length()};
    size_t device_column_width{max(column_header_width, longest_devRef_length)};
    size_t const left_pad{(device_column_width - column_header_width) / 2};
    size_t const right_pad{device_column_width - column_header_width - left_pad};
    string const padded_column_header{string(left_pad, ' ') + column_header + string(right_pad, ' ')};
    string const padded_column_underline{string(device_column_width, '-')};


    IFLOG(D1) {
      string const header1{" SSC        Model Number       Firmware Locn   World Wide Name        Serial Number     Vendor      Manufacturer Name\n"};
      string const header2{" --- ------------------------- -------- -----  ----------------   --------------------  -------  -----------------------\n"};

      string const padded_column_headers   {padded_column_header    + header1};
      string const padded_column_underlines{padded_column_underline + header2};

      fputs(padded_column_headers.c_str()    , Output2FILE::StreamStdout());
      fputs(padded_column_underlines.c_str() , Output2FILE::StreamStdout());
    }


    for (string & devref:devRefs) {
      LOG(D4) << std::endl << "Scanning \"" << devref << "\" ...";

      DtaDev * dev=NULL;

      uint8_t result=getDtaDev(devref.c_str(), dev,true);

      if (DTAERROR_SUCCESS == result && dev!=NULL) {

        DTA_DEVICE_TYPE deviceType = dev->getDevType();
        const char * deviceTypeName = DtaDevTypeName(deviceType);
        // LOG(E) << "DtaDev::diskScan: deviceType=" << deviceType
        //        << " "
        //        << "deviceTypeName=" << deviceTypeName
        //   ;

        fprintf(Output2FILE::StreamStdout(), "%-*s", (int)device_column_width, devref.c_str());
        if (dev->isAnySSC()) {
          fprintf(Output2FILE::StreamStdout(), " %s%s%s ",
                  (dev->isOpal1()  ? "1" : " "),
                  (dev->isOpal2()  ? "2" : " "),
                  (dev->isEprise() ? "E" : " "));
        } else {
          fprintf(Output2FILE::StreamStdout(), "%s", " No  ");
        }

        IFLOG(D1) {
          char WWN[19]="                  ";  // 18 blanks as placeholder if missing
          vector<uint8_t>wwn(dev->getWorldWideName());
          if (__is_not_all_NULs(wwn.data(), (unsigned int)wwn.size())) {
            snprintf(WWN, 19, "%02X%02X%02X%02X%02X%02X%02X%02X %c",
                     wwn[0], wwn[1], wwn[2], wwn[3], wwn[4], wwn[5], wwn[6], wwn[7],
                     dev->isWorldWideNameSynthetic() ? '*' : ' ');
          }
          fprintf(Output2FILE::StreamStdout(), "%-25.25s %-8.8s %-5.5s  %18s %-20.20s %-8.8s %-25.25s\n",
                  dev->getModelNum(),
                  dev->getFirmwareRev(),
                  deviceTypeName,
                  WWN,
                  dev->getSerialNum(),
                  dev->getVendorID(),
                  dev->getManufacturerName());

        } else {
          fprintf(Output2FILE::StreamStdout(), "%-25.25s %-8.8s %-5.5s\n",
                  dev->getModelNum(),
                  dev->getFirmwareRev(),
                  deviceTypeName);
        }

        delete dev;
        dev=NULL;
      }

      LOG(D4) << "... done scanning \"" << devref << "\"" << std::endl;

    }
  }
  fprintf(Output2FILE::StreamStdout(), "No more disks present -- ending scan\n");
  LOG(D1) << "Exiting DtaDev::diskScan";
  return DTAERROR_SUCCESS;
}

/** Delete its drive when this object is deleted. */
DtaDev::~DtaDev()
{
  LOG(D4) << "Destroying DtaDev";
  if (NULL != drive) {
    delete drive;
    drive = NULL;
  }
}
