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
/** Device class for Opal 2.0 SSC
 * also supports the Opal 1.0 SSC
 */
#include "os.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include<iomanip>
#include "MsedDevOpal.h"
#include "MsedHashPwd.h"
#include "MsedEndianFixup.h"
#include "MsedStructures.h"
#include "MsedCommand.h"
#include "MsedResponse.h"
#include "MsedSession.h"
#include "MsedHexDump.h"

using namespace std;

MsedDevOpal::MsedDevOpal()
{
}

MsedDevOpal::~MsedDevOpal()
{
}
void MsedDevOpal::init(const char * devref)
{
	uint8_t lastRC;
	MsedDevOS::init(devref);
	if((lastRC = properties()) != 0) 
		LOG(E) << "Properties exchange failed";
}

uint8_t MsedDevOpal::initialsetup(char * password)
{
	LOG(D1) << "Entering initialSetup()";
	uint8_t lastRC;
	if ((lastRC = takeOwnership(password)) != 0) {
		LOG(E) << "Initial setup failed - unable to take ownership";
		return lastRC;
	}
	if ((lastRC = activateLockingSP(password)) != 0) {
		LOG(E) << "Initial setup failed - unable to activate LockingSP";
		return lastRC;
	}
	if ((lastRC = configureLockingRange(0, MSED_DISABLELOCKING, password)) != 0) {
		LOG(E) << "Initial setup failed - unable to configure global locking range";
		return lastRC;
	}
	if ((lastRC = setLockingRange(0, OPAL_LOCKINGSTATE::READWRITE, password)) != 0) {
		LOG(E) << "Initial setup failed - unable to set global locking range RW";
		return lastRC;
	}
	if ((lastRC = setMBRDone(1, password)) != 0){
		LOG(E) << "Initial setup failed - unable to Enable MBR shadow";
		return lastRC;
	}
	if ((lastRC = setMBREnable(1, password)) != 0){
		LOG(E) << "Initial setup failed - unable to Enable MBR shadow";
		return lastRC;
	}
	
	LOG(I) << "Initial setup of TPer complete on " << dev;
	LOG(D1) << "Exiting initialSetup()";
	return 0;
}
uint8_t MsedDevOpal::listLockingRanges(char * password)
{
	uint8_t lastRC;
	LOG(D1) << "Entering MsedDevOpal:listLockingRanges()";
	vector<uint8_t> LR;
	LR.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		LR.push_back(OPALUID[OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL][i]);
	}
	
	session = new MsedSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete session;
		return lastRC;
	}
	vector<uint8_t> table;
	table.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		table.push_back(OPALUID[OPAL_UID::OPAL_LOCKING_INFO_TABLE][i]);
	}
	if ((lastRC = getTable(table, _OPAL_TOKEN::MAXRANGES, _OPAL_TOKEN::MAXRANGES)) != 0) {
		delete session;
		return lastRC;
	}
	if (response.tokenIs(4) != _OPAL_TOKEN::MSED_TOKENID_UINT) {
		LOG(E) << "Unable to determine number of ranges ";
		delete session;
		return MSEDERROR_NO_LOCKING_INFO;
	}
	LOG(I) << "Locking Range Configuration for " << dev;
	uint32_t numRanges = response.getUint32(4) + 1;
	for (uint32_t i = 0; i < numRanges; i++){
		if(0 != i) LR[8] = i & 0xff;
		if ((lastRC = getTable(LR, _OPAL_TOKEN::RANGESTART, _OPAL_TOKEN::WRITELOCKED)) != 0) {
			delete session;
			return lastRC;
		}
		LR[6] = 0x03;  // non global ranges are 00000802000300nn 
		LOG(I) << "LR" << i << " Begin " << response.getUint64(4) <<
			" for " << response.getUint64(8);
		LOG(I)	<< "            RLKEna =" << (response.getUint8(12) ? " Y " : " N ") <<
			" WLKEna =" << (response.getUint8(16) ? " Y " : " N ") <<
			" RLocked =" << (response.getUint8(20) ? " Y " : " N ") <<
			" WLocked =" << (response.getUint8(24) ? " Y " : " N ");
	}
	delete session;
	LOG(D1) << "Exiting MsedDevOpal:listLockingRanges()";
	return 0;
}
uint8_t MsedDevOpal::setupLockingRange(uint8_t lockingrange, uint64_t start,
	uint64_t length, char * password)
{
	uint8_t lastRC;
	LOG(D1) << "Entering MsedDevOpal:setupLockingRange()";
	if (lockingrange < 1) {
		LOG(E) << "global locking range cannot be changed";
		return MSEDERROR_UNSUPORTED_LOCKING_RANGE;
	}
	vector<uint8_t> LR;
	LR.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		LR.push_back(OPALUID[OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL][i]);
	}
	LR[6] = 0x03;
	LR[8] = lockingrange;
	session = new MsedSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete session;
		return lastRC;
	}
	MsedCommand *set = new MsedCommand();
	if (NULL == set) {
		LOG(E) << "Unable to create command object ";
		delete session;
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	set->reset(OPAL_UID::OPAL_AUTHORITY_TABLE, OPAL_METHOD::SET);
	set->changeInvokingUid(LR);
	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken(OPAL_TOKEN::VALUES); 
	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken(OPAL_TOKEN::RANGESTART);
	set->addToken(start);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken(OPAL_TOKEN::RANGELENGTH);
	set->addToken(length);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken(OPAL_TOKEN::READLOCKENABLED);
	set->addToken(OPAL_TOKEN::OPAL_FALSE);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken(OPAL_TOKEN::WRITELOCKENABLED);
	set->addToken(OPAL_TOKEN::OPAL_FALSE);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken(OPAL_TOKEN::READLOCKED);
	set->addToken(OPAL_TOKEN::OPAL_FALSE);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken(OPAL_TOKEN::WRITELOCKED);
	set->addToken(OPAL_TOKEN::OPAL_FALSE);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::ENDLIST);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::ENDLIST);
	set->complete();
	if ((lastRC = session->sendCommand(set, response)) != 0) {
		LOG(E) << "setupLockingRange Failed ";
		delete set;
		delete session;
		return lastRC;
	}
	delete set;
	delete session;
	if ((lastRC = rekeyLockingRange(lockingrange, password)) != 0) {
		LOG(E) << "setupLockingRange Unable to reKey Locking range -- Possible security issue ";
		return lastRC;
	}
	LOG(I) << "LockingRange" << (uint16_t)lockingrange << " starting block " << start <<
		" for " << length << " blocks configured as unlocked range";
	LOG(D1) << "Exiting MsedDevOpal:setupLockingRange()";
	return 0;
}
uint8_t MsedDevOpal::configureLockingRange(uint8_t lockingrange, uint8_t enabled, char * password)
{
	uint8_t lastRC;
	LOG(D1) << "Entering MsedDevOpal::configureLockingRange()";
	vector<uint8_t> LR;
	LR.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		LR.push_back(OPALUID[OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL][i]);
	}
	if (lockingrange != 0) {
		LR[6] = 0x03;
		LR[8] = lockingrange;
	}
	session = new MsedSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete session;
		return lastRC;
	}
	MsedCommand *set = new MsedCommand();
	if (NULL == set) {
		LOG(E) << "Unable to create command object ";
		delete session;
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	set->reset(OPAL_UID::OPAL_AUTHORITY_TABLE, OPAL_METHOD::SET);
	set->changeInvokingUid(LR);
	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken(OPAL_TOKEN::VALUES);
	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken(OPAL_TOKEN::READLOCKENABLED);
	set->addToken((enabled & MSED_READLOCKINGENABLED) ? OPAL_TRUE : OPAL_FALSE);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken(OPAL_TOKEN::WRITELOCKENABLED);
	set->addToken((enabled & MSED_WRITELOCKINGENABLED) ? OPAL_TRUE : OPAL_FALSE);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::ENDLIST);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::ENDLIST);
	set->complete();
	if ((lastRC = session->sendCommand(set, response)) != 0) {
		LOG(E) << "configureLockingRange Failed ";
		delete set;
		delete session;
		return lastRC;
	}
	delete set;
	delete session;
	LOG(I) << "LockingRange" << (uint16_t) lockingrange 
		<< (enabled ? " enabled " : " disabled ") 
		<< ((enabled & MSED_READLOCKINGENABLED) ? "ReadLocking" : "")
		<< ((enabled == (MSED_WRITELOCKINGENABLED | MSED_READLOCKINGENABLED)) ? "," : "")
		<< ((enabled & MSED_WRITELOCKINGENABLED) ? "WriteLocking" : "");
	LOG(D1) << "Exiting MsedDevOpal::configureLockingRange()";
	return 0;
}
uint8_t MsedDevOpal::rekeyLockingRange(uint8_t lockingrange, char * password)
{
	LOG(D1) << "Entering MsedDevOpal::rekeyLockingRange()";
	uint8_t lastRC;
	vector<uint8_t> LR;
	LR.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		LR.push_back(OPALUID[OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL][i]);
	}
	if (lockingrange != 0) {
		LR[6] = 0x03;
		LR[8] = lockingrange;
	}
	session = new MsedSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete session;
		return lastRC;
	}
	if ((lastRC = getTable(LR, OPAL_TOKEN::ACTIVEKEY, OPAL_TOKEN::ACTIVEKEY)) != 0) {
		delete session;
		return lastRC;
	}
	MsedCommand *rekey = new MsedCommand();
	if (NULL == rekey) {
		LOG(E) << "Unable to create command object ";
		delete session;
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	rekey->reset(OPAL_UID::OPAL_AUTHORITY_TABLE, OPAL_METHOD::GENKEY);
	rekey->changeInvokingUid(response.getRawToken(4));
	rekey->addToken(OPAL_TOKEN::STARTLIST);
	rekey->addToken(OPAL_TOKEN::ENDLIST);
	rekey->complete();
	if ((lastRC = session->sendCommand(rekey, response)) != 0) {
		LOG(E) << "rekeyLockingRange Failed ";
		delete rekey;
		delete session;
		return lastRC;
	}
	delete rekey;
	delete session;
	LOG(I) << "LockingRange" << (uint16_t)lockingrange << " reKeyed ";
	LOG(D1) << "Exiting MsedDevOpal::rekeyLockingRange()";
	return 0;
}
uint8_t MsedDevOpal::revertLockingSP(char * password, uint8_t keep)
{
	LOG(D1) << "Entering revert MsedDevOpal::revertLockingSP() keep = " << (uint16_t) keep;
	uint8_t lastRC;
	vector<uint8_t> keepGlobalLocking;
	keepGlobalLocking.push_back(0x83);
	keepGlobalLocking.push_back(0x06);
	keepGlobalLocking.push_back(0x00);
	keepGlobalLocking.push_back(0x00);
	MsedCommand *cmd = new MsedCommand();
	if (NULL == cmd) {
		LOG(E) << "Create session object failed";
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	session = new MsedSession(this);
	if (NULL == session) {
		LOG(E) << "Create session object failed";
		delete cmd;
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete cmd;
		delete session;
		return lastRC;
	}
	cmd->reset(OPAL_UID::OPAL_THISSP_UID, OPAL_METHOD::REVERTSP);
	cmd->addToken(OPAL_TOKEN::STARTLIST);
	if (keep) {
		cmd->addToken(OPAL_TOKEN::STARTNAME);
		cmd->addToken(keepGlobalLocking);
		cmd->addToken(OPAL_TOKEN::OPAL_TRUE);
		cmd->addToken(OPAL_TOKEN::ENDNAME);
	}
	cmd->addToken(OPAL_TOKEN::ENDLIST);
	cmd->complete();
	if ((lastRC = session->sendCommand(cmd, response)) != 0) {
		delete cmd;
		delete session;
		return lastRC;
	}
	// empty list returned so rely on method status
	LOG(I) << "Revert LockingSP complete";
	session->expectAbort();
	delete session;
	LOG(D1) << "Exiting revert MsedDev:LockingSP()";
	return 0;
}
uint8_t MsedDevOpal::getAuth4User(char * userid, uint8_t uidorcpin, std::vector<uint8_t> &userData)
{
	LOG(D1) << "Entering MsedDevOpal::getAuth4User()";
	userData.clear();
	userData. push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	userData.push_back(0x00);
	userData.push_back(0x00);
	userData.push_back(0x00);
	if ((0 != uidorcpin) && (10 != uidorcpin)) {
		LOG(E) << "Invalid Userid data requested" << (uint16_t)uidorcpin;
		return MSEDERROR_INVALID_PARAMETER;
	}
	if(uidorcpin) 
		userData.push_back(0x0b);
	else
		userData.push_back(0x09);
	if (!memcmp("User", userid, 4)) {
		userData.push_back(0x00);
		userData.push_back(0x03);
		userData.push_back(0x00);
		userData.push_back(atoi(&userid[4]) &0xff );
	} 
	else {
		if (!memcmp("Admin", userid, 5)) {
			userData.push_back(0x00);
			userData.push_back(0x01);
			userData.push_back(0x00);
			userData.push_back(atoi(&userid[5]) & 0xff );
		}
		else {
			LOG(E) << "Invalid Userid " << userid;
			userData.clear();
			return MSEDERROR_INVALID_PARAMETER;
		}
	}
	LOG(D1) << "Exiting MsedDevOpal::getAuth4User()";
	return 0;
}
uint8_t MsedDevOpal::setNewPassword(char * password, char * userid, char * newpassword)
{
	LOG(D1) << "Entering MsedDevOpal::setNewPassword" ;
	uint8_t lastRC;
	std::vector<uint8_t> userCPIN, hash;
	session = new MsedSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete session;
		return lastRC;
	}
	if ((lastRC = getAuth4User(userid, 10, userCPIN)) != 0) {
		LOG(E) << "Unable to find user " << userid << " in Authority Table";
		delete session;
		return lastRC;
	}
	MsedHashPwd(hash, newpassword, this);
	if ((lastRC = setTable(userCPIN, OPAL_TOKEN::PIN, hash)) != 0) {
		LOG(E) << "Unable to set user " << userid << " new password ";
		delete session;
		return lastRC;
	}
	LOG(I) << userid << " password changed";
	delete session;
	LOG(D1) << "Exiting MsedDevOpal::setNewPassword()";
	return 0;
}
uint8_t MsedDevOpal::setMBREnable(uint8_t mbrstate,	char * Admin1Password)
{
	LOG(D1) << "Entering MsedDevOpal::setMBREnable";
	uint8_t lastRC;
	if (mbrstate) {
		if ((lastRC = setLockingSPvalue(OPAL_UID::OPAL_MBRCONTROL, OPAL_TOKEN::MBRENABLE,
			OPAL_TOKEN::OPAL_TRUE, Admin1Password, NULL)) != 0) {
			LOG(E) << "Unable to set setMBREnable on";
			return lastRC;
		}
		else {
			LOG(I) << "MBREnable set on ";
		}
	} 
	else {
		if ((lastRC = setLockingSPvalue(OPAL_UID::OPAL_MBRCONTROL, OPAL_TOKEN::MBRENABLE,
				OPAL_TOKEN::OPAL_FALSE, Admin1Password, NULL)) != 0) {
				LOG(E) << "Unable to set setMBREnable off";
				return lastRC;
			}
		else {
			LOG(I) << "MBREnable set off ";
		}
	}
	LOG(D1) << "Exiting MsedDevOpal::setMBREnable";
	return 0;
}
uint8_t MsedDevOpal::setMBRDone(uint8_t mbrstate, char * Admin1Password)
{
	LOG(D1) << "Entering MsedDevOpal::setMBRDone";
	uint8_t lastRC;
	if (mbrstate) {
		if ((lastRC = setLockingSPvalue(OPAL_UID::OPAL_MBRCONTROL, OPAL_TOKEN::MBRDONE,
			OPAL_TOKEN::OPAL_TRUE, Admin1Password, NULL)) != 0) {
			LOG(E) << "Unable to set setMBRDone on";
			return lastRC;
		}
		else {
			LOG(I) << "MBRDone set on ";
		}
	}
	else {
		if ((lastRC = setLockingSPvalue(OPAL_UID::OPAL_MBRCONTROL, OPAL_TOKEN::MBRDONE,
			OPAL_TOKEN::OPAL_FALSE, Admin1Password, NULL)) != 0) {
			LOG(E) << "Unable to set setMBRDone off";
			return lastRC;
		}
		else {
			LOG(I) << "MBRDone set off ";
		}
	}
	LOG(D1) << "Exiting MsedDevOpal::setMBRDone";
	return 0;
}
uint8_t MsedDevOpal::setLockingRange(uint8_t lockingrange, uint8_t lockingstate,
	char * Admin1Password)
{
	uint8_t lastRC;
	OPAL_TOKEN readlocked, writelocked;
	const char *msg;

	LOG(D1) << "Entering MsedDevOpal::setLockingRange";
	switch (lockingstate) {
	case OPAL_LOCKINGSTATE::READWRITE:
		readlocked = writelocked = OPAL_TOKEN::OPAL_FALSE;
		msg = "RW";
		break;
	case OPAL_LOCKINGSTATE::READONLY:
		readlocked = OPAL_TOKEN::OPAL_FALSE;
		writelocked = OPAL_TOKEN::OPAL_TRUE;
		msg = "RO";
		break;
	case OPAL_LOCKINGSTATE::LOCKED:
		readlocked = writelocked = OPAL_TOKEN::OPAL_TRUE;
		msg = "LK";
		break;
	default:
		LOG(E) << "Invalid locking state for setLockingRange";
		return MSEDERROR_INVALID_PARAMETER;
	}
	vector<uint8_t> LR;
	LR.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		LR.push_back(OPALUID[OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL][i]);
	}
	if (lockingrange != 0) {
		LR[6] = 0x03;
		LR[8] = lockingrange;
	}
	session = new MsedSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, Admin1Password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete session;
		return lastRC;
	}
	MsedCommand *set = new MsedCommand();
	if (NULL == set) {
		LOG(E) << "Unable to create command object ";
		delete session;
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	set->reset(OPAL_UID::OPAL_AUTHORITY_TABLE, OPAL_METHOD::SET);
	set->changeInvokingUid(LR);
	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken(OPAL_TOKEN::VALUES);
	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken(OPAL_TOKEN::READLOCKED);
	set->addToken(readlocked);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken(OPAL_TOKEN::WRITELOCKED);
	set->addToken(writelocked);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::ENDLIST);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::ENDLIST);
	set->complete();
	if ((lastRC = session->sendCommand(set, response)) != 0) {
		LOG(E) << "setLockingRange Failed ";
		delete set;
		delete session;
		return lastRC;
	}
	delete set;
	delete session;
	LOG(I) << "LockingRange" << (uint16_t)lockingrange << " set to " << msg;
	LOG(D1) << "Exiting MsedDevOpal::setLockingRange";
	return 0;
}
uint8_t MsedDevOpal::setLockingSPvalue(OPAL_UID table_uid, OPAL_TOKEN name, 
	OPAL_TOKEN value,char * password, char * msg)
{
	LOG(D1) << "Entering MsedDevOpal::setLockingSPvalue";
	uint8_t lastRC;
	vector<uint8_t> table;
	table. push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		table.push_back(OPALUID[table_uid][i]);
	}
	session = new MsedSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete session;
		return lastRC;
	}
	if ((lastRC = setTable(table, name, value)) != 0) {
		LOG(E) << "Unable to update table";
		delete session;
		return lastRC;
	}
	if (NULL != msg) {
		LOG(I) << msg;
	}
	
	delete session;
	LOG(D1) << "Exiting MsedDevOpal::setLockingSPvalue()";
	return 0;
}

uint8_t MsedDevOpal::enableUser(char * password, char * userid)
{
	LOG(D1) << "Entering MsedDevOpal::enableUser";
	uint8_t lastRC;
	vector<uint8_t> userUID;
	
	session = new MsedSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete session;
		return lastRC;
	}
	if ((lastRC = getAuth4User(userid, 0, userUID)) != 0) {
		LOG(E) << "Unable to find user " << userid << " in Authority Table";
		delete session;
		return lastRC;
	}
	if ((lastRC = setTable(userUID, (OPAL_TOKEN)0x05, OPAL_TOKEN::OPAL_TRUE)) != 0) {
		LOG(E) << "Unable to enable user " << userid;
		delete session;
		return lastRC;
	}
	LOG(I) << userid << " has been enabled ";
	delete session;
	LOG(D1) << "Exiting MsedDevOpal::enableUser()";
	return 0;
}
uint8_t MsedDevOpal::revertTPer(char * password, uint8_t PSID)
{
	LOG(D1) << "Entering MsedDevOpal::revertTPer()";
	uint8_t lastRC;
	MsedCommand *cmd = new MsedCommand();
	if (NULL == cmd) {
		LOG(E) << "Unable to create command object ";
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	session = new MsedSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		delete cmd;
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	OPAL_UID uid = OPAL_UID::OPAL_SID_UID;
	if (PSID) {
		session->dontHashPwd(); // PSID pwd should be passed as entered
		uid = OPAL_UID::OPAL_PSID_UID;
		}
	if ((lastRC = session->start(OPAL_UID::OPAL_ADMINSP_UID, password, uid)) != 0) {
		delete cmd;
		delete session;
		return lastRC;
	}
	cmd->reset(OPAL_UID::OPAL_ADMINSP_UID, OPAL_METHOD::REVERT);
	cmd->addToken(OPAL_TOKEN::STARTLIST);
	cmd->addToken(OPAL_TOKEN::ENDLIST);
	cmd->complete();
	session->expectAbort();
	if ((lastRC = session->sendCommand(cmd, response)) != 0) {
		delete cmd;
		delete session;
		return lastRC;
	}
	LOG(I) << "revertTper completed successfully";
	delete cmd;
	delete session;
	LOG(D1) << "Exiting MsedDevOpal::revertTPer()";
	return 0;
}
uint8_t MsedDevOpal::loadPBA(char * password, char * filename) {
	LOG(D1) << "Entering MsedDevOpal::loadPBAimage()" << filename << " " << dev;
	uint8_t lastRC;
	uint64_t fivepercent = 0;
	int complete = 4;
	typedef struct { uint8_t  i : 2; } spinnertik;
	spinnertik spinnertick;
	spinnertick.i = 0;
	char star[] = "*";
	char spinner[] = "|/-\\";
	char progress_bar[] = "   [                     ]";

	uint32_t filepos = 0;
	ifstream pbafile;
	vector <uint8_t> buffer(1024,0x00), lengthtoken;
	//buffer.clear();
	//buffer.reserve(1024);
	//for (int i = 0; i < 1024; i++) {
	//	buffer.push_back(0x00);
	//}
	lengthtoken.clear();
	lengthtoken.push_back(0xd4);
	lengthtoken.push_back(0x00);
	pbafile.open(filename, ios::in | ios::binary);
	if (!pbafile) {
		LOG(E) << "Unable to open PBA image file " << filename;
		return MSEDERROR_OPEN_ERR;
	}
	pbafile.seekg(0, pbafile.end);
	fivepercent = ((pbafile.tellg() / 20) / 1024) * 1024;
	if (0 == fivepercent) fivepercent++;
	pbafile.seekg(0, pbafile.beg);

	MsedCommand *cmd = new MsedCommand();
	if (NULL == cmd) {
		LOG(E) << "Unable to create command object ";
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	session = new MsedSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete cmd;
		delete session;
		pbafile.close();
		return lastRC;
	}
	LOG(I) << "Writing PBA to " << dev;
	while (!pbafile.eof()) {
		pbafile.read((char *)buffer.data(), 1024);
		if (!(filepos % fivepercent)) progress_bar[complete++] = star[0];
		if (!(filepos % (1024 * 5))) {
			progress_bar[1] = spinner[spinnertick.i++];
			printf("\r%s", progress_bar);
			fflush(stdout);
		}
		cmd->reset(OPAL_UID::OPAL_MBR, OPAL_METHOD::SET);
		cmd->addToken(OPAL_TOKEN::STARTLIST);
		cmd->addToken(OPAL_TOKEN::STARTNAME);
		cmd->addToken(OPAL_TOKEN::WHERE);
		cmd->addToken(filepos);
		cmd->addToken(OPAL_TOKEN::ENDNAME);
		cmd->addToken(OPAL_TOKEN::STARTNAME);
		cmd->addToken(OPAL_TOKEN::VALUES);
		cmd->addToken(lengthtoken);
		cmd->addToken(buffer);
		cmd->addToken(OPAL_TOKEN::ENDNAME);
		cmd->addToken(OPAL_TOKEN::ENDLIST);
		cmd->complete();
		if ((lastRC = session->sendCommand(cmd, response)) != 0) {
			delete cmd;
			delete session;
			pbafile.close();
			return lastRC;
		}
		filepos += 1024;
	}
	printf("\n");
	delete cmd;
	delete session;
	pbafile.close();
	LOG(I) << "PBA image  " << filename << " written to " << dev;
	LOG(D1) << "Exiting MsedDevOpal::loadPBAimage()";
	return 0;
}

uint8_t MsedDevOpal::activateLockingSP(char * password)
{
	LOG(D1) << "Entering MsedDevOpal::activateLockingSP()";
	uint8_t lastRC;
	vector<uint8_t> table;
	table. push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		table.push_back(OPALUID[OPAL_UID::OPAL_LOCKINGSP_UID][i]);
	}
	MsedCommand *cmd = new MsedCommand();
	if (NULL == cmd) {
		LOG(E) << "Unable to create command object ";
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	session = new MsedSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_ADMINSP_UID, password, OPAL_UID::OPAL_SID_UID)) != 0) {
		delete cmd;
		delete session;
		return lastRC;
	}
	if ((lastRC = getTable(table, 0x06, 0x06)) != 0) {
		LOG(E) << "Unable to determine LockingSP Lifecycle state";
		delete cmd;
		delete session;
		return lastRC;
	}
	if ((0x06 != response.getUint8(3)) || // getlifecycle
		(0x08 != response.getUint8(4))) // Manufactured-Inactive
	{
		LOG(E) << "Locking SP lifecycle is not Manufactured-Inactive";
		delete cmd;
		delete session;
		return MSEDERROR_INVALID_LIFECYCLE;
	}
	cmd->reset(OPAL_UID::OPAL_LOCKINGSP_UID, OPAL_METHOD::ACTIVATE);
	cmd->addToken(OPAL_TOKEN::STARTLIST);
	cmd->addToken(OPAL_TOKEN::ENDLIST);
	cmd->complete();
	if ((lastRC = session->sendCommand(cmd, response)) != 0) {
		delete cmd;
		delete session;
		return lastRC;
	}
	LOG(I) << "Locking SP Activate Complete";

	delete cmd;
	delete session;
	LOG(D1) << "Exiting MsedDevOpal::activatLockingSP()";
	return 0;
}
uint8_t MsedDevOpal::takeOwnership(char * newpassword)
{
	LOG(D1) << "Entering MsedDevOpal::takeOwnership()";
	uint8_t lastRC;
	if ((lastRC = getDefaultPassword()) != 0) {
		LOG(E) << "Unable to read MSID password ";
		return lastRC;
	}
	if ((lastRC = setSIDPassword((char *)response.getString(4).c_str(), newpassword, 0)) != 0) {
		LOG(E) << "takeownership failed";
		return lastRC;
	}
	LOG(I) << "takeownership complete";
	LOG(D1) << "Exiting takeOwnership()";
	return 0;
}
uint8_t MsedDevOpal::getDefaultPassword()
{
	LOG(D1) << "Entering MsedDevOpal::getDefaultPassword()";
	uint8_t lastRC;
	vector<uint8_t> hash;
	session = new MsedSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_ADMINSP_UID)) != 0) {
		LOG(E) << "Unable to start Unauthenticated session " << dev;
		delete session;
		return lastRC;
	}
	vector<uint8_t> table;
	table. push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		table.push_back(OPALUID[OPAL_UID::OPAL_C_PIN_MSID][i]);
	}
	if ((lastRC = getTable(table, PIN, PIN)) != 0) {
		delete session;
		return lastRC;
	}
	delete session;
	LOG(D1) << "Exiting getDefaultPassword()";
	return 0;
}
uint8_t MsedDevOpal::setSIDPassword(char * oldpassword, char * newpassword,
	uint8_t hasholdpwd, uint8_t hashnewpwd)
{
	vector<uint8_t> hash, table;
	LOG(D1) << "Entering MsedDevOpal::setSIDPassword()";
	uint8_t lastRC;
	session = new MsedSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	if (!hasholdpwd) session->dontHashPwd();
	if ((lastRC = session->start(OPAL_UID::OPAL_ADMINSP_UID,
		oldpassword, OPAL_UID::OPAL_SID_UID)) != 0) {
		delete session;
		return lastRC;
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
	if ((lastRC = setTable(table, OPAL_TOKEN::PIN, hash)) != 0) {
		LOG(E) << "Unable to set new SID password ";
		delete session;
		return lastRC;
	}
	delete session;
	LOG(D1) << "Exiting MsedDevOpal::setSIDPassword()";
	return 0;
}

uint8_t MsedDevOpal::setTable(vector<uint8_t> table, OPAL_TOKEN name,
	OPAL_TOKEN value)
{
	vector <uint8_t> token;
	token.push_back((uint8_t) value);
	return(setTable(table, name, token));
}

uint8_t MsedDevOpal::setTable(vector<uint8_t> table, OPAL_TOKEN name, 
	vector<uint8_t> value)
{
	LOG(D1) << "Entering MsedDevOpal::setTable";
	uint8_t lastRC;
	MsedCommand *set = new MsedCommand();
	if (NULL == set) {
		LOG(E) << "Unable to create command object ";
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	set->reset(OPAL_UID::OPAL_AUTHORITY_TABLE, OPAL_METHOD::SET);
	set->changeInvokingUid(table);
	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken(OPAL_TOKEN::VALUES); // "values"
	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken(name);
	set->addToken(value);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::ENDLIST);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::ENDLIST);
	set->complete();
	if ((lastRC = session->sendCommand(set, response)) != 0) {
		LOG(E) << "Set Failed ";
		delete set;
		return lastRC;
	}
	delete set;
	LOG(D1) << "Leaving MsedDevOpal::setTable";
	return 0;
}
uint8_t MsedDevOpal::getTable(vector<uint8_t> table, uint16_t startcol, 
	uint16_t endcol)
{
	LOG(D1) << "Entering MsedDevOpal::getTable";
	uint8_t lastRC;
	MsedCommand *get = new MsedCommand();
	if (NULL == get) {
		LOG(E) << "Unable to create command object ";
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	get->reset(OPAL_UID::OPAL_AUTHORITY_TABLE, OPAL_METHOD::GET);
	get->changeInvokingUid(table);
	get->addToken(OPAL_TOKEN::STARTLIST);
	get->addToken(OPAL_TOKEN::STARTLIST);
	get->addToken(OPAL_TOKEN::STARTNAME);
	get->addToken(OPAL_TOKEN::STARTCOLUMN);
	get->addToken(startcol);
	get->addToken(OPAL_TOKEN::ENDNAME);
	get->addToken(OPAL_TOKEN::STARTNAME);
	get->addToken(OPAL_TOKEN::ENDCOLUMN);
	get->addToken(endcol);
	get->addToken(OPAL_TOKEN::ENDNAME);
	get->addToken(OPAL_TOKEN::ENDLIST);
	get->addToken(OPAL_TOKEN::ENDLIST);
	get->complete();
	if ((lastRC = session->sendCommand(get, response)) != 0) {
		delete get;
		return lastRC;
	}
	delete get;
	return 0;
}
uint8_t MsedDevOpal::exec(MsedCommand * cmd, MsedResponse & resp, uint8_t protocol)
{
	uint8_t lastRC;
    OPALHeader * hdr = (OPALHeader *) cmd->getCmdBuffer();
    LOG(D3) << endl << "Dumping command buffer";
    IFLOG(D3) MsedHexDump(cmd->getCmdBuffer(), SWAP32(hdr->cp.length) + sizeof (OPALComPacket));
    if((lastRC = sendCmd(IF_SEND, protocol, comID(), cmd->getCmdBuffer(), IO_BUFFER_LENGTH)) != 0) {
		LOG(E) << "Command failed on send " << (uint16_t) lastRC;
        return lastRC;
    }
    hdr = (OPALHeader *) cmd->getRespBuffer();
    do {
        osmsSleep(25);
        memset(cmd->getRespBuffer(), 0, IO_BUFFER_LENGTH);
        lastRC = sendCmd(IF_RECV, protocol, comID(), cmd->getRespBuffer(), IO_BUFFER_LENGTH);

    }
    while ((0 != hdr->cp.outstandingData) && (0 == hdr->cp.minTransfer));
    LOG(D3) << std::endl << "Dumping reply buffer";
    IFLOG(D3) MsedHexDump(cmd->getRespBuffer(), SWAP32(hdr->cp.length) + sizeof (OPALComPacket));
	if (0 != lastRC) {
        LOG(E) << "Command failed on recv" << (uint16_t) lastRC;
        return lastRC;
    }
    resp.init(cmd->getRespBuffer());
    return 0;
}


uint8_t MsedDevOpal::properties()
{
	LOG(D1) << "Entering MsedDevOpal::properties()";
	uint8_t lastRC;
	session = new MsedSession(this);  // use the session IO without starting a session
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	MsedCommand *props = new MsedCommand(OPAL_UID::OPAL_SMUID_UID, OPAL_METHOD::PROPERTIES);
	if (NULL == props) {
		LOG(E) << "Unable to create command object ";
		delete session;
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	props->addToken(OPAL_TOKEN::STARTLIST);
	props->addToken(OPAL_TOKEN::STARTNAME);
	props->addToken(OPAL_TOKEN::HOSTPROPERTIES);
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
	if ((lastRC = session->sendCommand(props, propertiesResponse)) != 0) {
		delete props;
		return lastRC;
	}
	disk_info.Properties = 1;
	delete props;
	LOG(D1) << "Leaving MsedDevOpal::properties()";
	return 0;
}
void MsedDevOpal::puke()
{
	LOG(D1) << "Entering MsedDevOpal::puke()";
	MsedDev::puke();
	if (disk_info.Properties) {
		cout << std::endl << "TPer Properties: ";
		for (uint32_t i = 0; i < propertiesResponse.getTokenCount(); i++) {
			if (OPAL_TOKEN::STARTNAME == propertiesResponse.tokenIs(i)) {
				if (OPAL_TOKEN::MSED_TOKENID_BYTESTRING != propertiesResponse.tokenIs(i + 1))
					cout << std::endl << "Host Properties: " << std::endl;
				else
					cout << "  " << propertiesResponse.getString(i + 1) << " = " << propertiesResponse.getUint64(i + 2);
				i += 2;
			}
			if (!(i % 6)) cout << std::endl;
		}
	}
}

uint8_t MsedDevOpal::objDump(char *sp, char * auth, char *pass,
	char * objID)
{

	LOG(D1) << "Entering MsedDevEnterprise::objDump";
	LOG(D1) << sp << " " << auth << " " << pass << " " << objID;
	uint8_t lastRC;
	MsedCommand *get = new MsedCommand();
	if (NULL == get) {
		LOG(E) << "Unable to create command object ";
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	vector<uint8_t> authority, object;
	uint8_t work;
	if (16 != strnlen(auth, 32)) {
		LOG(E) << "Authority must be 16 byte ascii string of hex authority uid";
		return MSEDERROR_INVALID_PARAMETER;
	}
	if (16 != strnlen(objID, 32)) {
		LOG(E) << "ObjectID must be 16 byte ascii string of hex object uid";
		return MSEDERROR_INVALID_PARAMETER;
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
	get->reset(OPAL_UID::OPAL_AUTHORITY_TABLE, OPAL_METHOD::GET);
	get->changeInvokingUid(object);
	get->addToken(OPAL_TOKEN::STARTLIST);
	get->addToken(OPAL_TOKEN::STARTLIST);
	get->addToken(OPAL_TOKEN::ENDLIST);
	get->addToken(OPAL_TOKEN::ENDLIST);
	get->complete();
	LOG(I) << "Command:";
	get->dumpCommand();
	session = new MsedSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		delete get;
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start((OPAL_UID)atoi(sp), pass, authority)) != 0) {
		delete get;
		delete session;
		return lastRC;
	}
	if ((lastRC = session->sendCommand(get, response)) != 0) {
		delete get;
		delete session;
		return lastRC;
	}
	LOG(I) << "Response:";
	get->dumpResponse();
	delete get;
	delete session;
	LOG(D1) << "Exiting MsedDevEnterprise::objDump";
	return 0;
}
uint8_t MsedDevOpal::rawCmd(char *sp, char * hexauth, char *pass,
	char *hexinvokingUID, char *hexmethod, char *hexparms) {
	LOG(D1) << "Entering MsedDevEnterprise::rawCmd";
	LOG(D1) << sp << " " << hexauth << " " << pass << " ";
	LOG(D1) << hexinvokingUID << " " << hexmethod << " " << hexparms;
	uint8_t lastRC;
	vector<uint8_t> authority, object, invokingUID, method, parms;
	uint8_t work;
	if (16 != strnlen(hexauth, 32)) {
		LOG(E) << "Authority must be 16 byte ascii string of hex authority uid";
		return MSEDERROR_INVALID_PARAMETER;
	}
	authority.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (uint32_t i = 0; i < 16; i += 2) {
		work = hexauth[i] & 0x40 ? 16 * ((hexauth[i] & 0xf) + 9) : 16 * (hexauth[i] & 0x0f);
		work += hexauth[i + 1] & 0x40 ? (hexauth[i + 1] & 0xf) + 9 : hexauth[i + 1] & 0x0f;
		authority.push_back(work);
	}
	if (16 != strnlen(hexinvokingUID, 32)) {
		LOG(E) << "invoker must be 16 byte ascii string of invoking uid";
		return MSEDERROR_INVALID_PARAMETER;
	}
	invokingUID.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (uint32_t i = 0; i < 16; i += 2) {
		work = hexinvokingUID[i] & 0x40 ? 16 * ((hexinvokingUID[i] & 0xf) + 9) : 16 * (hexinvokingUID[i] & 0x0f);
		work += hexinvokingUID[i + 1] & 0x40 ? (hexinvokingUID[i + 1] & 0xf) + 9 : hexinvokingUID[i + 1] & 0x0f;
		invokingUID.push_back(work);
	}
	if (16 != strnlen(hexmethod, 32)) {
		LOG(E) << "invoker must be 16 byte ascii string of method uid";
		return MSEDERROR_INVALID_PARAMETER;
	}
	method.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (uint32_t i = 0; i < 16; i += 2) {
		work = hexmethod[i] & 0x40 ? 16 * ((hexmethod[i] & 0xf) + 9) : 16 * (hexmethod[i] & 0x0f);
		work += hexmethod[i + 1] & 0x40 ? (hexmethod[i + 1] & 0xf) + 9 : hexmethod[i + 1] & 0x0f;
		method.push_back(work);
	}
	if (1020 < strnlen(hexparms, 1024)) {
		LOG(E) << "Parmlist limited to 1020 characters";
		return MSEDERROR_INVALID_PARAMETER;
	}
	if (strnlen(hexparms, 1024) % 2) {
		LOG(E) << "Parmlist must be even number of bytes";
		return MSEDERROR_INVALID_PARAMETER;
	}

	for (uint32_t i = 0; i < strnlen(hexparms, 1024); i += 2) {
		work = hexparms[i] & 0x40 ? 16 * ((hexparms[i] & 0xf) + 9) : 16 * (hexparms[i] & 0x0f);
		work += hexparms[i + 1] & 0x40 ? (hexparms[i + 1] & 0xf) + 9 : hexparms[i + 1] & 0x0f;
		parms.push_back(work);
	}
	MsedCommand *cmd = new MsedCommand();
	if (NULL == cmd) {
		LOG(E) << "Unable to create command object ";
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	cmd->reset(OPAL_UID::OPAL_AUTHORITY_TABLE, method);
	cmd->changeInvokingUid(invokingUID);
	cmd->addToken(parms);
	cmd->complete();
	session = new MsedSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		delete cmd;
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start((OPAL_UID)atoi(sp), pass, authority)) != 0) {
		delete cmd;
		delete session;
		return lastRC;
	}
	LOG(I) << "Command:";
	cmd->dumpCommand();
	if ((lastRC = session->sendCommand(cmd, response)) != 0) {
		delete cmd;
		delete session;
		return lastRC;
	}
	LOG(I) << "Response:";
	cmd->dumpResponse();
	delete cmd;
	delete session;
	LOG(D1) << "Exiting MsedDevEnterprise::rawCmd";
	return 0;
}
