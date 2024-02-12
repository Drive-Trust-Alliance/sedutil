/* C:B**************************************************************************
This software is Copyright 2014, 2022 Bright Plaza Inc. <drivetrust@drivetrust.com>

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
#include <log/log.h>
#include <iostream>
#include <iomanip>
#include "DtaOptions.h"
#include "DtaDev.h"
#include "DtaStructures.h"
#include "DtaEndianFixup.h"
#include "DtaHexDump.h"
#include "DtaSession.h"
#include "DtaCommand.h"


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
vector<uint8_t>DtaDev::getPasswordSalt()
{
    const uint8_t * b=disk_info.passwordSalt;
    return vector<uint8_t>(b,b+sizeof(disk_info.passwordSalt));
}
DTA_DEVICE_TYPE DtaDev::getDevType()
{
    return disk_info.devType;
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

void DtaDev::puke()
{
	LOG(D1) << "Entering DtaDev::puke()";
	/* IDENTIFY */
    const char * devType =
       disk_info.devType == DEVICE_TYPE_ATA  ? " ATA "
     : disk_info.devType == DEVICE_TYPE_SAS  ? " SAS "
     : disk_info.devType == DEVICE_TYPE_USB  ? " USB "
     : disk_info.devType == DEVICE_TYPE_NVME ? " NVMe "
     :                                         " OTHER ";
    cout << endl << dev << devType << "  " << disk_info.modelNum << "  " << disk_info.firmwareRev << "   " << disk_info.serialNum;
    IFLOG(D) {
        char WWN[19]="                ";  // 16 blanks as placeholder if missing
        uint8_t (&wwn)[8] = disk_info.worldWideName;
        if (__is_not_all_NULs(wwn, sizeof(wwn))) {
            snprintf(WWN, 19, "%02X%02X%02X%02X%02X%02X%02X%02X %c",
                     wwn[0], wwn[1], wwn[2], wwn[3], wwn[4], wwn[5], wwn[6], wwn[7],
                     disk_info.worldWideNameIsSynthetic ? '*' : ' ');
        }
        cout << "  " << WWN
             << "  " << disk_info.vendorID
             << "   " << disk_info.manufacturerName;
    }
    cout << endl;

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
