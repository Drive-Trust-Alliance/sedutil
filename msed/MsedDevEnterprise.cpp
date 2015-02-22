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
#include "os.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include<iomanip>
#include "MsedDevEnterprise.h"
#include "MsedHashPwd.h"
#include "MsedEndianFixup.h"
#include "MsedStructures.h"
#include "MsedCommand.h"
#include "MsedResponse.h"
#include "MsedSession.h"
#include "MsedHexDump.h"

using namespace std;


MsedDevEnterprise::MsedDevEnterprise(const char * devref)
{
	MsedDevOS::init(devref);
	assert(isEprise());
	if (properties())
		LOG(E) << "Properties exchange failed";
}
MsedDevEnterprise::~MsedDevEnterprise()
{
}
uint8_t MsedDevEnterprise::initialsetup(char * password)
{
	LOG(D1) << "Entering initialSetup()";
	if (takeOwnership(password)) {
		LOG(E) << "Initial setup failed - unable to take ownership";
		return 0xff;
	}
	if (setLockingRange(OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL,
		OPAL_TOKEN::OPAL_FALSE, password)) {
		LOG(E) << "Initial setup failed - unable to unlock for read/write";
		return 0xff;
	}
	
	if (configureLockingRange(OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL,
		OPAL_TOKEN::OPAL_TRUE, password)) {
		LOG(E) << "Initial setup failed - unable to enable read/write locking";
		return 0xff;
	}
	
	LOG(I) << "Initial setup of TPer complete on " << dev;
	LOG(D1) << "Exiting initialSetup()";
	return 0;
}
uint8_t MsedDevEnterprise::configureLockingRange(uint8_t lockingrange, OPAL_TOKEN enabled, char * password)
{
	LOG(D1) << "Entering MsedDevEnterprise::configureLockingRange()";
	if (lockingrange) {
		LOG(E) << "Only global locking range is currently supported";
		return 0xff;
	}
	/* can't use settable because the segate drives require that both the 
	 * read & write lockenabled be changed at the same time.  I can find no
	 * written doc on such a restriction but .....
	 */
	MsedCommand *set = new MsedCommand();
	set->reset(OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL, OPAL_METHOD::ESET);
	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::ENDLIST);
	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken("WriteLockEnabled");
	set->addToken(enabled);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken("ReadLockEnabled");
	set->addToken(enabled);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::ENDLIST);
	set->addToken(OPAL_TOKEN::ENDLIST);
	set->addToken(OPAL_TOKEN::ENDLIST);
	set->complete();
	session = new MsedSession(this);
	if (session->start(OPAL_UID::ENTERPRISE_LOCKINGSP_UID, password,
		OPAL_UID::ENTERPRISE_BANDMASTER0_UID)) {
		delete session;
		return 0xff;
	}
	if (session->sendCommand(set, response)) {
		LOG(E) << "Set Failed ";
		delete session;
		delete set;
		return 0xff;
	}
	delete set;
	delete session;
	LOG(I) << "Locking range configured " << (uint16_t) enabled;
	LOG(D1) << "Exiting MsedDevEnterprise::configureLockingRange()";
	return 0;
}
uint8_t MsedDevEnterprise::revertLockingSP(char * password, uint8_t keep)
{
	LOG(D1) << "Entering revert MsedEnterpriseDev::LockingSP()";
	if(password == NULL) LOG(D4) << "Referencing formal parameters " << keep;
	LOG(E) << "Revert is not implemented at this time ";
	LOG(E) << "I can find no documentation and numerous web searched haven't helped ";
	LOG(D1) << "Exiting revert MsedEnterpriseDev::LockingSP()";
	return 0;
}
uint8_t MsedDevEnterprise::setNewPassword(char * password, char * userid, char * newpassword)
{
	LOG(D1) << "Entering MsedDevEnterprise::setNewPassword" ;
	std::vector<uint8_t> user, usercpin, hash;
	if (11 > strnlen(userid, 15)) {
		LOG(E) << "Invalid Userid " << userid;
		return 0xff;
	}
	user.clear();
	user.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	user.push_back(0x00);
	user.push_back(0x00);
	user.push_back(0x00);
	user.push_back(0x09);
	user.push_back(0x00);
	user.push_back(0x00);
//	user.push_back(0x80);
//	user.push_back(0x01);

	if (!memcmp("BandMaster", userid, 10)) {
		int band = atoi(&userid[11]);
		band += 1;
		if (1023 < band) {
			LOG(E) << "Invalid Userid " << userid;
			return 0xff;
		}
		user.push_back((((band >> 8) & 0xff) | 0x80));
		user.push_back(band & 0xff);
	}
	else {
		if (!memcmp("EraseMaster", userid, 11)) {
			user.push_back(0x84);
			user.push_back(0x01);
		}
		else {
			LOG(E) << "Invalid Userid " << userid;
			return 0xff;
		}
	}
	usercpin = user;
	usercpin[4] = 0x0b;
	session = new MsedSession(this);
	if (session->start(OPAL_UID::ENTERPRISE_LOCKINGSP_UID, password, user)) {
		delete session;
		return 0xff;
	}

	MsedHashPwd(hash, newpassword, this);
	if (setTable(usercpin, "PIN", hash)) {
		LOG(E) << "Unable to set user " << userid << " new password ";
		delete session;
		return 0xff;
	}
	LOG(I) << userid << " password changed";
	delete session;
	LOG(D1) << "Exiting MsedDevEnterprise::setNewPassword()";
	return 0;
}
uint8_t MsedDevEnterprise::setMBREnable(uint8_t mbrstate,	char * Admin1Password)
{
	LOG(D1) << "Entering MsedDevEnterprise::setMBREnable";
	if (NULL == Admin1Password) LOG(E) << "This shouldn't happen " << mbrstate;
	LOG(I) << "MBR shadowing is optional in the Enterprise SSC and not supported";
	LOG(D1) << "Exiting MsedDevEnterprise::setMBREnable";
	return 0;
}
uint8_t MsedDevEnterprise::setMBRDone(uint8_t mbrstate, char * Admin1Password)
{
	LOG(D1) << "Entering MsedDevEnterprise::setMBRDone";
	if (NULL == Admin1Password) LOG(E) << "This shouldn't happen " << mbrstate;
	LOG(I) << "MBR shadowing is optional in the Enterprise SSC and not supported";
	LOG(D1) << "Exiting MsedDevEnterprise::setMBRDone";
	return 0;
}

uint8_t MsedDevEnterprise::setupLockingRange(uint8_t lockingrange, uint64_t start,
	uint64_t length, char * password) {
	LOG(D1) << "Entering MsedDevEnterprise::setupLockingRange";
	if (0 == lockingrange) LOG(E) << start << length << password;
	LOG(D1) << "Exiting MsedDevEnterprise::setupLockingRange";
	return 0;
}
uint8_t MsedDevEnterprise::setLockingRange(uint8_t lockingrange, uint8_t lockingstate,
	char * password)
{
	LOG(D1) << "Entering MsedDevEnterprise::setLockingRange";
	OPAL_TOKEN locked;
	
	if (lockingrange) {
		LOG(E) << "Only global locking range is currently supported";
		return 0xff;
	}
	switch (lockingstate) {
	case OPAL_LOCKINGSTATE::READWRITE:
		locked = OPAL_TOKEN::OPAL_FALSE;
		break;
	case OPAL_LOCKINGSTATE::READONLY:
		LOG(E) << "Read Only locking state is unsupported in Enterprise SSC";
		return 0xff;
	case OPAL_LOCKINGSTATE::LOCKED:
		locked = OPAL_TOKEN::OPAL_TRUE;
		break;
	default:
		LOG(E) << "Invalid locking state for setLockingRange";
		return 0xff;
	}
	MsedCommand *set = new MsedCommand();
	set->reset(OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL, OPAL_METHOD::ESET);
	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::ENDLIST);
	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken("WriteLocked");
	set->addToken(locked);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken("ReadLocked");
	set->addToken(locked);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::ENDLIST);
	set->addToken(OPAL_TOKEN::ENDLIST);
	set->addToken(OPAL_TOKEN::ENDLIST);
	set->complete();
	session = new MsedSession(this);
	if (session->start(OPAL_UID::ENTERPRISE_LOCKINGSP_UID, password,
		OPAL_UID::ENTERPRISE_BANDMASTER0_UID)) {
		delete session;
		return 0xff;
	}
	if (session->sendCommand(set, response)) {
		LOG(E) << "Set Failed ";
		delete session;
		delete set;
		return 0xff;
	}
	delete set;
	delete session;
	LOG(I) << "Locking range Read/Write set " << (uint16_t)locked;
	LOG(D1) << "Exiting MsedDevEnterprise::setLockingRange";
	return 0;
}
uint8_t MsedDevEnterprise::enableUser(char * password, char * userid)
{
	LOG(D1) << "Entering MsedDevEnterprise::enableUser";
	LOG(E) << "enableUser not implemented";
	if (!password && !userid) LOG(E) << "Formal Parameters";
	LOG(D1) << "Exiting MsedDevEnterprise::enableUser()";
	return 0xff;
}
uint8_t MsedDevEnterprise::revertTPer(char * password, uint8_t PSID)
{
	LOG(D1) << "Entering MsedDevEnterprise::revertTPer()";
	if (password == NULL) LOG(D4) << "Referencing formal parameters " << PSID;
	LOG(E) << "Revert is not implemented at this time ";
	LOG(E) << "I can find no documentation and numerous web searched haven't helped ";
	LOG(D1) << "Exiting MsedDevEnterprise::RevertTperevertTPer()";
	return 0;
}
uint8_t MsedDevEnterprise::loadPBA(char * password, char * filename) {
	LOG(D1) << "Entering MsedDevEnterprise::loadPBAimage()" << filename << " " << dev;
	if (password == NULL) LOG(D4) << "Referencing formal parameters " << filename;
	LOG(I) << "loadPBA is not implemented.  It is not a mandatory part of  ";
	LOG(I) << "the enterprise SSC ";
	LOG(D1) << "Exiting MsedDevEnterprise::loadPBAimage()";
	return 0;
}
uint8_t MsedDevEnterprise::activateLockingSP(char * password)
{
	LOG(D1) << "Entering MsedDevEnterprise::activateLockingSP()";
	if (password == NULL) LOG(D4) << "Referencing formal parameters ";
	LOG(E) << "activate Locking SP is not a part of the Enterprise SSC ";
	LOG(D1) << "Exiting MsedDevEnterprise::activatLockingSP()";
	return 0;
}
uint8_t MsedDevEnterprise::takeOwnership(char * newpassword)
{
	string defaultPassword;
	LOG(D1) << "Entering MsedDevEnterprise::takeOwnership()";
	if (getDefaultPassword()) {
		LOG(E) << "takeownership failed unable to retrieve MSID";
		return 0xff;
	}
	defaultPassword = response.getString(5);
	if (setSIDPassword((char *)defaultPassword.c_str(), newpassword, 0)) {
		LOG(E) << "takeownership failed unable to set new SID password";
		return 0xff;
	}
	if (initLSPUsers((char *)defaultPassword.c_str(), newpassword)) {
		LOG(E) << "takeownership failed unable to set Locking SP user passwords";
		return 0xff;
	}
	LOG(I) << "takeownership complete";
	LOG(D1) << "Exiting takeOwnership()";
	return 0;
}
uint8_t MsedDevEnterprise::initLSPUsers(char * defaultPassword, char * newPassword)
{
	vector<uint8_t> user, usercpin, hash, erasemaster, table;
	int maxRanges;

	LOG(D1) << "Entering MsedDevEnterprise::initLSPUsers()";
	user.clear();
	user.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	user.push_back(0x00);
	user.push_back(0x00);
	user.push_back(0x00);
	user.push_back(0x09);
	user.push_back(0x00);
	user.push_back(0x00);
	user.push_back(0x84);
	user.push_back(0x01);

// do erasemaster
	usercpin = erasemaster = user;
	usercpin[4] = 0x0b;
	session = new MsedSession(this);
	session->dontHashPwd();
	if (session->start(OPAL_UID::ENTERPRISE_LOCKINGSP_UID, defaultPassword, user)) {
		delete session;
		return 0xff;
	}
	MsedHashPwd(hash, newPassword, this);
	if (setTable(usercpin, "PIN", hash)) {
		LOG(E) << "Unable to set new EraseMaster password ";
		delete session;
		return 0xff;
	}
	LOG(I) << "EraseMaster  password set";
	delete session;
// use erasemaster to determine number of ranges
	session = new MsedSession(this);
	if (session->start(OPAL_UID::ENTERPRISE_LOCKINGSP_UID, newPassword, erasemaster)) {
		delete session;
		return 0xff;
	}
	table.clear();
	table.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		table.push_back(OPALUID[ENTERPRISE_LOCKING_INFO_TABLE][i]);
	}
	if (getTable(table, "MaxRanges", "MaxRanges")) {
		LOG(E) << "Unable to get max Ranges from Locking Info table ";
		delete session;
		return 0xff;
	}
	delete session;
	maxRanges = (int) response.getUint16(5);
	LOG(I) << "Maximum ranges supported " << maxRanges;
// do bandmasters
	table.clear();
	table.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		table.push_back(OPALUID[ENTERPRISE_BANDMASTER0_UID][i]);
	}
	for (int i = 0; i < maxRanges+1; i++) {
		LOG(D3) << "initializing BandMaster" << (uint16_t) i;
		user[7] = table[7] = (((((i + 1) >> 8) & 0xff) | 0x80));
		user[8] = table[8] = ((i + 1) & 0xff);

		session = new MsedSession(this);
		if (session->start(OPAL_UID::ENTERPRISE_LOCKINGSP_UID, newPassword, erasemaster)) {
			delete session;
			return 0xff;
		}
		if (getTable(table, "Enabled", "Enabled")) {
			LOG(E) << "Unable determine if band" << i << " is enabled";
			delete session;
			return 0xff;
		}
		delete session;

		if (!response.getUint8(5)) {
			LOG(I) << "Bandmaster" << i << " is disabled ... skipping";
			continue;
		}
		usercpin = user;
		usercpin[4] = 0x0b;
		session = new MsedSession(this);
		session->dontHashPwd();
		if (session->start(OPAL_UID::ENTERPRISE_LOCKINGSP_UID, defaultPassword, user)) {
			delete session;
			return 0xff;
		}
		MsedHashPwd(hash, newPassword, this);
		if (setTable(usercpin, "PIN", hash)) {
			LOG(E) << "Unable to set BandMaster" << (uint16_t) i << " new password ";
			delete session;
			return 0xff;
		}
		LOG(I) << "BandMaster" << (uint16_t) i << " password set";
		delete session;

	}
	LOG(D1) << "Exiting MsedDevEnterprise::initLSPUsers()";
	return 0;
}
uint8_t MsedDevEnterprise::getDefaultPassword()
{
	LOG(D1) << "Entering MsedDevEnterprise::getDefaultPassword()";
	vector<uint8_t> hash;
	session = new MsedSession(this);
	if (session->start(OPAL_UID::OPAL_ADMINSP_UID)) {
		LOG(E) << "Unable to start Unauthenticated session " << dev;
		delete session;
		return 0xff;
	}
	vector<uint8_t> table;
	table. push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		table.push_back(OPALUID[OPAL_UID::OPAL_C_PIN_MSID][i]);
	}
	if (getTable(table, "PIN", "PIN")) {
		delete session;
		return 0xff;
	}
	delete session;
	LOG(D1) << "Exiting getDefaultPassword()";
	return 0;
}
uint8_t MsedDevEnterprise::setSIDPassword(char * oldpassword, char * newpassword,
	uint8_t hasholdpwd, uint8_t hashnewpwd)
{
	vector<uint8_t> hash, table;
	LOG(D1) << "Entering MsedDevEnterprise::setSIDPassword()";
	session = new MsedSession(this);
	if (!hasholdpwd) session->dontHashPwd();
	if (session->start(OPAL_UID::OPAL_ADMINSP_UID,
		oldpassword, OPAL_UID::OPAL_SID_UID)) {
		delete session;
		return 0xff;
	}
	table.clear();
	table. push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		table.push_back(OPALUID[OPAL_UID::OPAL_C_PIN_SID][i]);
	}
	hash.clear();
	if (hashnewpwd) {
		MsedHashPwd(hash, newpassword, this);
	}
	else {
		hash.push_back(0xd0);
		hash.push_back((uint8_t)strnlen(newpassword, 255));
		for (uint16_t i = 0; i < strnlen(newpassword, 255); i++) {
			hash.push_back(newpassword[i]);
		}
	}
	if (setTable(table, "PIN", hash)) {
		LOG(E) << "Unable to set new SID password ";
		delete session;
		return 0xff;
	}
	delete session;
	LOG(D1) << "Exiting MsedDevEnterprise::setSIDPassword()";
	return 0;
}
uint8_t MsedDevEnterprise::setTable(vector<uint8_t> table, const char *name,
	OPAL_TOKEN value)
{
	vector <uint8_t> token;
	token.push_back((uint8_t) value);
	return(setTable(table, name, token));
}
uint8_t MsedDevEnterprise::setTable(vector<uint8_t> table, const char *name, 
	vector<uint8_t> value)
{
	LOG(D1) << "Entering MsedDevEnterprise::setTable";
	MsedCommand *set = new MsedCommand();
	set->reset(OPAL_UID::OPAL_AUTHORITY_TABLE, OPAL_METHOD::ESET);
	set->changeInvokingUid(table);
	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::ENDLIST);
 	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken(name);
	set->addToken(value);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::ENDLIST);
	set->addToken(OPAL_TOKEN::ENDLIST);
	set->addToken(OPAL_TOKEN::ENDLIST);
	set->complete();
	if (session->sendCommand(set, response)) {
		LOG(E) << "Set Failed ";
		delete set;
		return 0xff;
	}
	delete set;
	LOG(D1) << "Leaving MsedDevEnterprise::setTable";
	return 0;
}
uint8_t MsedDevEnterprise::getTable(vector<uint8_t> table, const char * startcol, 
	const char * endcol)
{
	LOG(D1) << "Entering MsedDevEnterprise::getTable";
	MsedCommand *get = new MsedCommand();
	get->reset(OPAL_UID::OPAL_AUTHORITY_TABLE, OPAL_METHOD::EGET);
	get->changeInvokingUid(table);
	get->addToken(OPAL_TOKEN::STARTLIST);
	get->addToken(OPAL_TOKEN::STARTLIST);
	get->addToken(OPAL_TOKEN::STARTNAME);
	get->addToken("startColumn");
	get->addToken(startcol);
	get->addToken(OPAL_TOKEN::ENDNAME);
	get->addToken(OPAL_TOKEN::STARTNAME);
	get->addToken("endColumn");
	get->addToken(endcol);
	get->addToken(OPAL_TOKEN::ENDNAME);
	get->addToken(OPAL_TOKEN::ENDLIST);
	get->addToken(OPAL_TOKEN::ENDLIST);
	get->complete();
	if (session->sendCommand(get, response)) {
		delete get;
		return 0xff;
	}
	delete get;
	return 0;
}
uint16_t MsedDevEnterprise::comID()
{
    LOG(D1) << "Entering MsedDevEnterprise::comID()";
    return disk_info.Enterprise_basecomID;
}
uint8_t MsedDevEnterprise::exec(MsedCommand * cmd, MsedResponse &response, uint8_t protocol)
{
    uint8_t rc = 0;
    OPALHeader * hdr = (OPALHeader *) cmd->getCmdBuffer();
    LOG(D3) << endl << "Dumping command buffer";
    IFLOG(D3) MsedHexDump(cmd->getCmdBuffer(), SWAP32(hdr->cp.length) + sizeof (OPALComPacket));
    rc = sendCmd(IF_SEND, protocol, comID(), cmd->getCmdBuffer(), IO_BUFFER_LENGTH);
    if (0 != rc) {
        LOG(E) << "Command failed on send " << (uint16_t) rc;
        return rc;
    }
    hdr = (OPALHeader *) cmd->getRespBuffer();
    do {
        //LOG(I) << "read loop";
        osmsSleep(25);
        memset(cmd->getRespBuffer(), 0, IO_BUFFER_LENGTH);
        rc = sendCmd(IF_RECV, protocol, comID(), cmd->getRespBuffer(), IO_BUFFER_LENGTH);

    }
    while ((0 != hdr->cp.outstandingData) && (0 == hdr->cp.minTransfer));
    LOG(D3) << std::endl << "Dumping reply buffer";
    IFLOG(D3) MsedHexDump(cmd->getRespBuffer(), SWAP32(hdr->cp.length) + sizeof (OPALComPacket));
    if (0 != rc) {
        LOG(E) << "Command failed on recv" << (uint16_t) rc;
        return rc;
    }
    response.init(cmd->getRespBuffer());
    return 0;
}
uint8_t MsedDevEnterprise::properties()
{
	LOG(D1) << "Entering MsedDevEnterprise::properties()";
	session = new MsedSession(this);  // use the session IO without starting a session
	MsedCommand *props = new MsedCommand(OPAL_UID::OPAL_SMUID_UID, OPAL_METHOD::PROPERTIES);
	props->addToken(OPAL_TOKEN::STARTLIST);
	props->addToken(OPAL_TOKEN::STARTNAME);
	props->addToken("HostProperties");	
	props->addToken(OPAL_TOKEN::STARTLIST);
	props->addToken(OPAL_TOKEN::STARTNAME);
	props->addToken("MaxComPacketSize");
	props->addToken(2048);
	props->addToken(OPAL_TOKEN::ENDNAME);
	props->addToken(OPAL_TOKEN::STARTNAME);
	props->addToken("MaxPacketSize");
	props->addToken(2028);
	props->addToken(OPAL_TOKEN::ENDNAME);
	props->addToken(OPAL_TOKEN::STARTNAME);
	props->addToken("MaxIndTokenSize");
	props->addToken(1992);
	props->addToken(OPAL_TOKEN::ENDNAME);
	props->addToken(OPAL_TOKEN::STARTNAME);
	props->addToken("MaxPackets");
	props->addToken(1);
	props->addToken(OPAL_TOKEN::ENDNAME);
	props->addToken(OPAL_TOKEN::STARTNAME);
	props->addToken("MaxSubpackets");
	props->addToken(1);
	props->addToken(OPAL_TOKEN::ENDNAME);
	props->addToken(OPAL_TOKEN::STARTNAME);
	props->addToken("MaxMethods");
	props->addToken(1);
	props->addToken(OPAL_TOKEN::ENDNAME);
	props->addToken(OPAL_TOKEN::ENDLIST);
	props->addToken(OPAL_TOKEN::ENDNAME);
	props->addToken(OPAL_TOKEN::ENDLIST);
	props->complete();
	if (session->sendCommand(props, propertiesResponse)) {
		delete props;
		return 0xff;
	}
	disk_info.Properties = 1;
	delete props;
	LOG(D1) << "Leaving MsedDevEnterprise::properties()";
	return 0;
}
void MsedDevEnterprise::puke()
{
	LOG(D1) << "Entering MsedDevEnterprise::puke()";
	MsedDev::puke();
	if (disk_info.Properties) {
		cout << std::endl << "TPer Properties: " << std::endl;
		for (uint32_t i = 0, j = 1; i < propertiesResponse.getTokenCount(); i++) {
			if (OPAL_TOKEN::ENDLIST == propertiesResponse.tokenIs(i)) {
				if (OPAL_TOKEN::STARTNAME == propertiesResponse.tokenIs(i + 1)) {
					cout << std::endl << "Host Properties: " << std::endl;
					i += 2;
					j = 1;
					continue;
				}
				else {
					break;
				}
			}
			if (OPAL_TOKEN::STARTNAME == propertiesResponse.tokenIs(i)) {
				if (OPAL_TOKEN::MSED_TOKENID_BYTESTRING == propertiesResponse.tokenIs(i + 1)) {
					cout << "  " << propertiesResponse.getString(i + 1) << " = " << propertiesResponse.getUint64(i + 2);
					i += 2;
					j++;
					if (!(j % 3)) cout << std::endl;
				}
			}	
		}
	}
}
uint8_t MsedDevEnterprise::rawCmd(char *sp, char *hexauth, char *pass,
	char *hexinvokingUID, char *hexmethod,char *hexparms)
{
	LOG(D1) << "Entering MsedDevEnterprise::rawCmd";
	LOG(D1) << sp << " " << hexauth << " " << pass << " " ;
	LOG(D1) << hexinvokingUID << " " << hexmethod << " " << hexparms;

	vector<uint8_t> authority, object, invokingUID, method, parms;
	uint8_t work;
	if (16 != strnlen(hexauth, 32)) {
		LOG(E) << "Authority must be 16 byte ascii string of hex authority uid";
		return 0xff;
	}
	authority.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (uint32_t i = 0; i < 16; i += 2) {
		work = hexauth[i] & 0x40 ? 16 * ((hexauth[i] & 0xf) + 9) : 16 * (hexauth[i] & 0x0f);
		work += hexauth[i + 1] & 0x40 ? (hexauth[i + 1] & 0xf) + 9 : hexauth[i + 1] & 0x0f;
		authority.push_back(work);
	}
	if (16 != strnlen(hexinvokingUID, 32)) {
		LOG(E) << "invoker must be 16 byte ascii string of invoking uid";
		return 0xff;
	}
	invokingUID.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (uint32_t i = 0; i < 16; i += 2) {
		work = hexinvokingUID[i] & 0x40 ? 16 * ((hexinvokingUID[i] & 0xf) + 9) : 16 * (hexinvokingUID[i] & 0x0f);
		work += hexinvokingUID[i + 1] & 0x40 ? (hexinvokingUID[i + 1] & 0xf) + 9 : hexinvokingUID[i + 1] & 0x0f;
		invokingUID.push_back(work);
	}
	if (16 != strnlen(hexmethod, 32)) {
		LOG(E) << "invoker must be 16 byte ascii string of method uid";
		return 0xff;
	}
	method.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (uint32_t i = 0; i < 16; i += 2) {
		work = hexmethod[i] & 0x40 ? 16 * ((hexmethod[i] & 0xf) + 9) : 16 * (hexmethod[i] & 0x0f);
		work += hexmethod[i + 1] & 0x40 ? (hexmethod[i + 1] & 0xf) + 9 : hexmethod[i + 1] & 0x0f;
		method.push_back(work);
	}
	if (1020 < strnlen(hexparms, 1024)) {
		LOG(E) << "Parmlist limited to 1020 characters";
		return 0xff;
	}
	if (strnlen(hexparms, 1024) % 2) {
		LOG(E) << "Parmlist must be even number of bytes";
		return 0xff;
	}

	for (uint32_t i = 0; i < strnlen(hexparms, 1024); i += 2) {
		work = hexparms[i] & 0x40 ? 16 * ((hexparms[i] & 0xf) + 9) : 16 * (hexparms[i] & 0x0f);
		work += hexparms[i + 1] & 0x40 ? (hexparms[i + 1] & 0xf) + 9 : hexparms[i + 1] & 0x0f;
		parms.push_back(work);
	}
	MsedCommand *cmd = new MsedCommand();
	cmd->reset(OPAL_UID::OPAL_AUTHORITY_TABLE, method);
	cmd->changeInvokingUid(invokingUID);
	cmd->addToken(parms);
	cmd->complete();
	session = new MsedSession(this);
	if (session->start((OPAL_UID) atoi(sp), pass, authority)) {
		delete cmd;
		delete session;
		return 0xff;
	}
	LOG(I) << "Command:";
	cmd->dumpCommand();
	if (session->sendCommand(cmd, response)) {
		delete cmd;
		delete session;
		return 0xff;
	}
	LOG(I) << "Response:";
	cmd->dumpResponse();
	delete cmd;
	delete session;
	LOG(D1) << "Exiting MsedDevEnterprise::rawCmd";
	return 0;
}
uint8_t MsedDevEnterprise::objDump(char *sp, char * auth, char *pass,
	char * objID)
{
	LOG(D1) << "Entering MsedDevEnterprise::objDump";
	LOG(D1) << sp << " " << auth << " " << pass << " " << objID;
	MsedCommand *get = new MsedCommand();
	vector<uint8_t> authority, object;
	uint8_t work;
	if (16 != strnlen(auth, 32)) {
		LOG(E) << "Authority must be 16 byte ascii string of hex authority uid";
		return 0xff;
	}
	if (16 != strnlen(objID, 32)) {
		LOG(E) << "ObjectID must be 16 byte ascii string of hex object uid";
		return 0xff;
	}
	authority.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (uint32_t i = 0; i < 16; i += 2) {
		work = auth[i] & 0x40 ? 16 * ((auth[i] & 0xf) + 9) : 16 * (auth[i] & 0x0f);
		work += auth[i + 1] & 0x40 ? (auth[i + 1] & 0xf) + 9 : auth[i + 1] & 0x0f;
		authority.push_back(work);
	}
	object.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (uint32_t i = 0; i < 16; i += 2) {
		work = objID[i] & 0x40 ? 16 * ((objID[i] & 0xf) + 9) : 16 * (objID[i] & 0x0f);
		work += objID[i + 1] & 0x40 ? (objID[i + 1] & 0xf) + 9 : objID[i + 1] & 0x0f;
		object.push_back(work);
	}
	get->reset(OPAL_UID::OPAL_AUTHORITY_TABLE, OPAL_METHOD::EGET);
	get->changeInvokingUid(object);
	get->addToken(OPAL_TOKEN::STARTLIST);
	get->addToken(OPAL_TOKEN::STARTLIST);
	get->addToken(OPAL_TOKEN::ENDLIST);
	get->addToken(OPAL_TOKEN::ENDLIST);
	get->complete();
	LOG(I) << "Command:";
	get->dumpCommand();
	session = new MsedSession(this);
	if (session->start((OPAL_UID)atoi(sp), pass, authority)) {
		delete get;
		delete session;
		return 0xff;
	}
	if (session->sendCommand(get, response)) {
		delete get;
		delete session;
		return 0xff;
	}
	LOG(I) << "Response:";
	get->dumpResponse();
	delete get;
	delete session;
	LOG(D1) << "Exiting MsedDevEnterprise::objDump";
	return 0;
}

