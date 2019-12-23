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
/** Device class for Opal 2.0 SSC
 * also supports the Opal 1.0 SSC
 */
#include "os.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include<iomanip>
#include "DtaDevOpal.h"
#include "DtaHashPwd.h"
#include "DtaEndianFixup.h"
#include "DtaStructures.h"
#include "DtaCommand.h"
#include "DtaResponse.h"
#include "DtaSession.h"
#include "DtaHexDump.h"
#include "DtaAnnotatedDump.h"

using namespace std;

DtaDevOpal::DtaDevOpal()
{
}

DtaDevOpal::~DtaDevOpal()
{
}
void DtaDevOpal::init(const char * devref)
{
	uint8_t lastRC;
	DtaDevOS::init(devref);
	if((lastRC = properties()) != 0) { LOG(E) << "Properties exchange failed";}
}

uint8_t DtaDevOpal::initialSetup(char * password)
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
	if ((lastRC = configureLockingRange(0, DTA_DISABLELOCKING, password)) != 0) {
		LOG(E) << "Initial setup failed - unable to configure global locking range";
		return lastRC;
	}
	if ((lastRC = setLockingRange(0, OPAL_LOCKINGSTATE::READWRITE, password)) != 0) {
		LOG(E) << "Initial setup failed - unable to set global locking range RW";
		return lastRC;
	}
	if (!MBRAbsent()) {
		setMBREnable(1, password);
	}
	
	LOG(I) << "Initial setup of TPer complete on " << dev;
	LOG(D1) << "Exiting initialSetup()";
	return 0;
}

uint8_t DtaDevOpal::setup_SUM(uint8_t lockingrange, uint64_t start, uint64_t length, char *Admin1Password, char * password)
{
	LOG(D1) << "Entering setup_SUM()";
	uint8_t lastRC;
	char defaultPW[] = ""; //OPAL defines the default initial User password as 0x00
	std::string userId;
	userId.append("User");
	userId.append(std::to_string(lockingrange + 1)); //OPAL defines LR0 to User1, LR1 to User2, etc.

	//verify opal SUM support and status
	if (!disk_info.Locking || !disk_info.SingleUser)
	{
		LOG(E) << "Setup_SUM failed - this drive does not support LockingSP / SUM";
		return DTAERROR_INVALID_COMMAND;
	}
	if (disk_info.Locking_lockingEnabled && !disk_info.SingleUser_any)
	{
		LOG(E) << "Setup_SUM failed - LockingSP has already been configured in standard mode.";
		return DTAERROR_INVALID_COMMAND;
	}
	//If locking not enabled, run initial setup flow
	if (!disk_info.Locking_lockingEnabled)
	{
		LOG(D1) << "LockingSP not enabled. Beginning initial setup flow.";
		if ((lastRC = takeOwnership(Admin1Password)) != 0) {
			LOG(E) << "Setup_SUM failed - unable to take ownership";
			return lastRC;
		}
		if ((lastRC = activateLockingSP_SUM(lockingrange, Admin1Password)) != 0) {
			LOG(E) << "Setup_SUM failed - unable to activate LockingSP in SUM";
			return lastRC;
		}
		if ((lastRC = setupLockingRange_SUM(lockingrange, start, length, defaultPW)) != 0) {
			LOG(E) << "Setup_SUM failed - unable to setup locking range " << lockingrange << "(" << start << "," << length << ")";
			return lastRC;
		}
	}
	if ((lastRC = eraseLockingRange_SUM(lockingrange, Admin1Password)) != 0) {
		LOG(E) << "Setup_SUM failed - unable to erase locking range";
		return lastRC;
	}

	//verify that locking range covers correct LBAs
	lrStatus_t lrStatus;
	if ((lrStatus = getLockingRange_status(lockingrange, Admin1Password)).command_status != 0) {
		LOG(E) << "Setup_SUM failed - unable to query locking range start/size";
		return lrStatus.command_status;
	}
	if (start != lrStatus.start || length != lrStatus.size)
	{
		LOG(D1) << "Incorrect Locking Range " << lockingrange << " start/size. Attempting to correct...";
		if ((lastRC = setupLockingRange_SUM(lockingrange, start, length, defaultPW)) != 0) {
			LOG(E) << "Setup_SUM failed - unable to setup locking range " << lockingrange << "(" << start << "," << length << ")";
			return lastRC;
		}
		LOG(D1) << "Locking Range " << lockingrange << " start/size corrected.";
	}

	//enable and set new password for locking range
	if ((lastRC = setLockingRange_SUM(lockingrange, OPAL_LOCKINGSTATE::READWRITE, defaultPW)) != 0) {
		LOG(E) << "Setup_SUM failed - unable to enable locking range";
		return lastRC;
	}
	if ((lastRC = setNewPassword_SUM(defaultPW, (char *)userId.c_str(), password)) != 0) {
		LOG(E) << "Setup_SUM failed - unable to set new locking range password";
		return lastRC;
	}

	LOG(I) << "Setup of SUM complete on " << dev;
	LOG(D1) << "Exiting setup_SUM()";
	return 0;
}
DtaDevOpal::lrStatus_t DtaDevOpal::getLockingRange_status(uint8_t lockingrange, char * password)
{
	uint8_t lastRC;
	lrStatus_t lrStatus;
	LOG(D1) << "Entering DtaDevOpal:getLockingRange_status()";
	vector<uint8_t> LR;
	LR.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		LR.push_back(OPALUID[OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL][i]);
	}

	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		lrStatus.command_status = DTAERROR_OBJECT_CREATE_FAILED;
		return lrStatus;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete session;
		lrStatus.command_status = lastRC;
		return lrStatus;
	}
	if (0 != lockingrange) {
		LR[8] = lockingrange & 0xff;
		LR[6] = 0x03;  // non global ranges are 00000802000300nn 
	}
	if ((lastRC = getTable(LR, _OPAL_TOKEN::RANGESTART, _OPAL_TOKEN::WRITELOCKED)) != 0) {
		delete session;
		lrStatus.command_status = lastRC;
		return lrStatus;
	}
	if (response.getTokenCount() < 24)
	{
		LOG(E) << "locking range getTable command did not return enough data";
		delete session;
		lrStatus.command_status = DTAERROR_NO_LOCKING_INFO;
		return lrStatus;
	}
	lrStatus.command_status = 0;
	lrStatus.lockingrange_num = lockingrange;
	lrStatus.start = response.getUint64(4);
	lrStatus.size = response.getUint64(8);
	lrStatus.RLKEna = (response.getUint8(12) != 0);
	lrStatus.WLKEna = (response.getUint8(16) != 0);
	lrStatus.RLocked = (response.getUint8(20) != 0);
	lrStatus.WLocked = (response.getUint8(24) != 0);
	LOG(D1) << "Locking Range " << lockingrange << " Begin: " << lrStatus.start << " Length: "
		<< lrStatus.size << " RLKEna: " << lrStatus.RLKEna << " WLKEna: " << lrStatus.WLKEna
		<< " RLocked: " << lrStatus.RLocked << " WLocked: " << lrStatus.WLocked;
	delete session;
	LOG(D1) << "Exiting DtaDevOpal:getLockingRange_status()";
	return lrStatus;
}
uint8_t DtaDevOpal::listLockingRanges(char * password, int16_t rangeid)
{
	uint8_t lastRC;
	LOG(D1) << "Entering DtaDevOpal:listLockingRanges()" << rangeid;
	vector<uint8_t> LR;
	LR.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		LR.push_back(OPALUID[OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL][i]);
	}
	
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
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
	if (response.tokenIs(4) != _OPAL_TOKEN::DTA_TOKENID_UINT) {
		LOG(E) << "Unable to determine number of ranges ";
		delete session;
		return DTAERROR_NO_LOCKING_INFO;
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
	LOG(D1) << "Exiting DtaDevOpal:listLockingRanges()";
	return 0;
}
uint8_t DtaDevOpal::setupLockingRange(uint8_t lockingrange, uint64_t start,
	uint64_t length, char * password)
{
	uint8_t lastRC;
	LOG(D1) << "Entering DtaDevOpal:setupLockingRange()";
	if (lockingrange < 1) {
		LOG(E) << "global locking range cannot be changed";
		return DTAERROR_UNSUPORTED_LOCKING_RANGE;
	}
	vector<uint8_t> LR;
	LR.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		LR.push_back(OPALUID[OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL][i]);
	}
	LR[6] = 0x03;
	LR[8] = lockingrange;
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete session;
		return lastRC;
	}
	DtaCommand *set = new DtaCommand();
	if (NULL == set) {
		LOG(E) << "Unable to create command object ";
		delete session;
		return DTAERROR_OBJECT_CREATE_FAILED;
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
	LOG(D1) << "Exiting DtaDevOpal:setupLockingRange()";
	return 0;
}
uint8_t DtaDevOpal::setupLockingRange_SUM(uint8_t lockingrange, uint64_t start,
	uint64_t length, char * password)
{
	uint8_t lastRC;
	LOG(D1) << "Entering DtaDevOpal:setupLockingRange_SUM()";
	if (lockingrange < 1) {
		LOG(E) << "global locking range cannot be changed";
		return DTAERROR_UNSUPORTED_LOCKING_RANGE;
	}
	vector<uint8_t> LR;
	LR.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		LR.push_back(OPALUID[OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL][i]);
	}
	LR[6] = 0x03;
	LR[8] = lockingrange;
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	vector<uint8_t> auth;
	auth.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 7; i++) {
		auth.push_back(OPALUID[OPAL_UID::OPAL_USER1_UID][i]);
	}
	auth.push_back(lockingrange+1);
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, auth)) != 0) {
		LOG(E) << "Error starting session. Did you provide the correct user password? (GlobalRange = User1; Range1 = User2, etc.)";
		delete session;
		return lastRC;
	}
	DtaCommand *set = new DtaCommand();
	if (NULL == set) {
		LOG(E) << "Unable to create command object ";
		delete session;
		return DTAERROR_OBJECT_CREATE_FAILED;
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
	set->addToken(OPAL_TOKEN::OPAL_TRUE);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken(OPAL_TOKEN::WRITELOCKENABLED);
	set->addToken(OPAL_TOKEN::OPAL_TRUE);
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
	if ((lastRC = rekeyLockingRange_SUM(LR, auth, password)) != 0) {
		LOG(E) << "setupLockingRange Unable to reKey Locking range -- Possible security issue ";
		return lastRC;
	}
	LOG(I) << "LockingRange" << (uint16_t)lockingrange << " starting block " << start <<
		" for " << length << " blocks configured as LOCKED range";
	LOG(D1) << "Exiting DtaDevOpal:setupLockingRange_SUM()";
	return 0;
}
uint8_t DtaDevOpal::configureLockingRange(uint8_t lockingrange, uint8_t enabled, char * password)
{
	uint8_t lastRC;
	LOG(D1) << "Entering DtaDevOpal::configureLockingRange()";
	vector<uint8_t> LR;
	LR.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		LR.push_back(OPALUID[OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL][i]);
	}
	if (lockingrange != 0) {
		LR[6] = 0x03;
		LR[8] = lockingrange;
	}
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete session;
		return lastRC;
	}
	DtaCommand *set = new DtaCommand();
	if (NULL == set) {
		LOG(E) << "Unable to create command object ";
		delete session;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	set->reset(OPAL_UID::OPAL_AUTHORITY_TABLE, OPAL_METHOD::SET);
	set->changeInvokingUid(LR);
	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken(OPAL_TOKEN::VALUES);
	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken(OPAL_TOKEN::READLOCKENABLED);
	set->addToken((enabled & DTA_READLOCKINGENABLED) ? OPAL_TRUE : OPAL_FALSE);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken(OPAL_TOKEN::WRITELOCKENABLED);
	set->addToken((enabled & DTA_WRITELOCKINGENABLED) ? OPAL_TRUE : OPAL_FALSE);
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
		<< ((enabled & DTA_READLOCKINGENABLED) ? "ReadLocking" : "")
		<< ((enabled == (DTA_WRITELOCKINGENABLED | DTA_READLOCKINGENABLED)) ? "," : "")
		<< ((enabled & DTA_WRITELOCKINGENABLED) ? "WriteLocking" : "");
	LOG(D1) << "Exiting DtaDevOpal::configureLockingRange()";
	return 0;
}
uint8_t DtaDevOpal::rekeyLockingRange(uint8_t lockingrange, char * password)
{
	LOG(D1) << "Entering DtaDevOpal::rekeyLockingRange()";
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
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete session;
		return lastRC;
	}
	if ((lastRC = getTable(LR, OPAL_TOKEN::ACTIVEKEY, OPAL_TOKEN::ACTIVEKEY)) != 0) {
		delete session;
		return lastRC;
	}
	DtaCommand *rekey = new DtaCommand();
	if (NULL == rekey) {
		LOG(E) << "Unable to create command object ";
		delete session;
		return DTAERROR_OBJECT_CREATE_FAILED;
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
	LOG(D1) << "Exiting DtaDevOpal::rekeyLockingRange()";
	return 0;
}
uint8_t DtaDevOpal::rekeyLockingRange_SUM(vector<uint8_t> LR, vector<uint8_t>  UID, char * password)
{
	LOG(D1) << "Entering DtaDevOpal::rekeyLockingRange_SUM()";
	uint8_t lastRC;

	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, UID)) != 0) {
		delete session;
		return lastRC;
	}
	if ((lastRC = getTable(LR, OPAL_TOKEN::ACTIVEKEY, OPAL_TOKEN::ACTIVEKEY)) != 0) {
		delete session;
		return lastRC;
	}
	DtaCommand *rekey = new DtaCommand();
	if (NULL == rekey) {
		LOG(E) << "Unable to create command object ";
		delete session;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	rekey->reset(OPAL_UID::OPAL_AUTHORITY_TABLE, OPAL_METHOD::GENKEY);
	rekey->changeInvokingUid(response.getRawToken(4));
	rekey->addToken(OPAL_TOKEN::STARTLIST);
	rekey->addToken(OPAL_TOKEN::ENDLIST);
	rekey->complete();
	if ((lastRC = session->sendCommand(rekey, response)) != 0) {
		LOG(E) << "rekeyLockingRange_SUM Failed ";
		delete rekey;
		delete session;
		return lastRC;
	}
	delete rekey;
	delete session;
	LOG(I) << "LockingRange reKeyed ";
	LOG(D1) << "Exiting DtaDevOpal::rekeyLockingRange_SUM()";
	return 0;
}
uint8_t DtaDevOpal::setBandsEnabled(int16_t lockingrange, char * password)
{
	if (password == NULL) { LOG(D4) << "Password is NULL"; } // unreferenced formal paramater
	LOG(D1) << "Entering DtaDevOpal::setBandsEnabled()" << lockingrange << " " << dev;
	LOG(I) << "setBandsEnabled is not implemented.  It is not part of the Opal SSC ";
	LOG(D1) << "Exiting DtaDevOpal::setBandsEnabled()";
	return 0;
}
uint8_t DtaDevOpal::revertLockingSP(char * password, uint8_t keep)
{
	LOG(D1) << "Entering DtaDevOpal::revertLockingSP() keep = " << (uint16_t) keep;
	uint8_t lastRC;
	vector<uint8_t> keepGlobalLocking;
	keepGlobalLocking.push_back(0x83);
	keepGlobalLocking.push_back(0x06);
	keepGlobalLocking.push_back(0x00);
	keepGlobalLocking.push_back(0x00);
	DtaCommand *cmd = new DtaCommand();
	if (NULL == cmd) {
		LOG(E) << "Create session object failed";
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Create session object failed";
		delete cmd;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete cmd;
		delete session;
                LOG(E) << "Start session failed";
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
                LOG(E) << "Command failed";
		delete cmd;
		delete session;
		return lastRC;
	}
	// empty list returned so rely on method status
	LOG(I) << "Revert LockingSP complete";
	session->expectAbort();
	delete cmd;
	delete session;
	LOG(D1) << "Exiting DtaDevOpal::revertLockingSP()";
	return 0;
}
uint8_t DtaDevOpal::eraseLockingRange(uint8_t lockingrange, char * password)
{
	LOG(D1) << "Entering DtaDevOpal::eraseLockingRange()" << lockingrange << " " << dev;
	if (password == NULL) { LOG(D4) << "Referencing formal parameters " << lockingrange; }
	LOG(I) << "eraseLockingRange is not implemented.  It is not part of the Opal SSC ";
	LOG(D1) << "Exiting DtaDevOpal::eraseLockingRange()";
	return 0;
}
uint8_t DtaDevOpal::getAuth4User(char * userid, uint8_t uidorcpin, std::vector<uint8_t> &userData)
{
	LOG(D1) << "Entering DtaDevOpal::getAuth4User()";
	userData.clear();
	userData. push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	userData.push_back(0x00);
	userData.push_back(0x00);
	userData.push_back(0x00);
	if ((0 != uidorcpin) && (10 != uidorcpin)) {
		LOG(E) << "Invalid Userid data requested" << (uint16_t)uidorcpin;
		return DTAERROR_INVALID_PARAMETER;
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
			return DTAERROR_INVALID_PARAMETER;
		}
	}
	LOG(D1) << "Exiting DtaDevOpal::getAuth4User()";
	return 0;
}
uint8_t DtaDevOpal::setPassword(char * password, char * userid, char * newpassword)
{
	LOG(D1) << "Entering DtaDevOpal::setPassword" ;
	uint8_t lastRC;
	std::vector<uint8_t> userCPIN, hash;
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
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
	DtaHashPwd(hash, newpassword, this);
	if ((lastRC = setTable(userCPIN, OPAL_TOKEN::PIN, hash)) != 0) {
		LOG(E) << "Unable to set user " << userid << " new password ";
		delete session;
		return lastRC;
	}
	LOG(I) << userid << " password changed";
	delete session;
	LOG(D1) << "Exiting DtaDevOpal::setPassword()";
	return 0;
}
uint8_t DtaDevOpal::setNewPassword_SUM(char * password, char * userid, char * newpassword)
{
	LOG(D1) << "Entering DtaDevOpal::setNewPassword_SUM";
	uint8_t lastRC;
	std::vector<uint8_t> userCPIN, hash;
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	vector<uint8_t> auth;
	if (!memcmp("Admin", userid, 5))
	{

		auth.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
		for (int i = 0; i < 7; i++) {
			auth.push_back(OPALUID[OPAL_UID::OPAL_ADMIN1_UID][i]);
		}
		auth.push_back((uint8_t)atoi(&userid[5]));
	}
	else if (!memcmp("User", userid, 4))
	{
		auth.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
		for (int i = 0; i < 7; i++) {
			auth.push_back(OPALUID[OPAL_UID::OPAL_USER1_UID][i]);
		}
		auth.push_back((uint8_t)atoi(&userid[4]));
	}
	else
	{
		LOG(E) << "Invalid userid \"" << userid << "\"specified for setNewPassword_SUM";
		delete session;
		return DTAERROR_INVALID_PARAMETER;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, auth)) != 0) {
		delete session;
		return lastRC;
	}
	if ((lastRC = getAuth4User(userid, 10, userCPIN)) != 0) {
		LOG(E) << "Unable to find user " << userid << " in Authority Table";
		delete session;
		return lastRC;
	}
	DtaHashPwd(hash, newpassword, this);
	if ((lastRC = setTable(userCPIN, OPAL_TOKEN::PIN, hash)) != 0) {
		LOG(E) << "Unable to set user " << userid << " new password ";
		delete session;
		return lastRC;
	}
	LOG(I) << userid << " password changed";
	delete session;
	LOG(D1) << "Exiting DtaDevOpal::setNewPassword_SUM()";
	return 0;
}
uint8_t DtaDevOpal::setMBREnable(uint8_t mbrstate,	char * Admin1Password)
{
	LOG(D1) << "Entering DtaDevOpal::setMBREnable";
	uint8_t lastRC;
        // set MBRDone before changing MBREnable so the PBA isn't presented
        if ((lastRC = setMBRDone(1, Admin1Password)) != 0){
		LOG(E) << "unable to set MBRDone";
                return lastRC;
        }
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
	LOG(D1) << "Exiting DtaDevOpal::setMBREnable";
	return 0;
}
uint8_t DtaDevOpal::setMBRDone(uint8_t mbrstate, char * Admin1Password)
{
	LOG(D1) << "Entering DtaDevOpal::setMBRDone";
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
	LOG(D1) << "Exiting DtaDevOpal::setMBRDone";
	return 0;
}
uint8_t DtaDevOpal::setLockingRange(uint8_t lockingrange, uint8_t lockingstate,
	char * Admin1Password)
{
	uint8_t lastRC;
	uint8_t archiveuser = 0;
	OPAL_TOKEN readlocked, writelocked;
	const char *msg;

	LOG(D1) << "Entering DtaDevOpal::setLockingRange";
	switch (lockingstate) {
	case OPAL_LOCKINGSTATE::READWRITE:
		readlocked = writelocked = OPAL_TOKEN::OPAL_FALSE;
		msg = "RW";
		break;
	case OPAL_LOCKINGSTATE::ARCHIVEUNLOCKED:
		archiveuser = 1;
	case OPAL_LOCKINGSTATE::READONLY:
		readlocked = OPAL_TOKEN::OPAL_FALSE;
		writelocked = OPAL_TOKEN::OPAL_TRUE;
		msg = "RO";
		break;
	case OPAL_LOCKINGSTATE::ARCHIVELOCKED:
		archiveuser = 1;
	case OPAL_LOCKINGSTATE::LOCKED:
		readlocked = writelocked = OPAL_TOKEN::OPAL_TRUE;
		msg = "LK";
		break;
	default:
		LOG(E) << "Invalid locking state for setLockingRange";
		return DTAERROR_INVALID_PARAMETER;
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
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, Admin1Password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete session;
		return lastRC;
	}
	DtaCommand *set = new DtaCommand();
	if (NULL == set) {
		LOG(E) << "Unable to create command object ";
		delete session;
		return DTAERROR_OBJECT_CREATE_FAILED;
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
	if (!archiveuser) {
		set->addToken(OPAL_TOKEN::STARTNAME);
		set->addToken(OPAL_TOKEN::WRITELOCKED);
		set->addToken(writelocked);
		set->addToken(OPAL_TOKEN::ENDNAME);
	}
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
	LOG(D1) << "Exiting DtaDevOpal::setLockingRange";
	return 0;
}
uint8_t DtaDevOpal::setLockingRange_SUM(uint8_t lockingrange, uint8_t lockingstate,
	char * password)
{
	uint8_t lastRC;
	OPAL_TOKEN readlocked, writelocked;
	const char *msg;

	LOG(D1) << "Entering DtaDevOpal::setLockingRange_SUM";
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
		return DTAERROR_INVALID_PARAMETER;
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
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	vector<uint8_t> auth;
	auth.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 7; i++) {
		auth.push_back(OPALUID[OPAL_UID::OPAL_USER1_UID][i]);
	}
	auth.push_back(lockingrange+1);
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, auth)) != 0) {
		LOG(E) << "Error starting session. Did you provide the correct user password? (GlobalRange = User1; Range1 = User2, etc.)";
		delete session;
		return lastRC;
	}

	DtaCommand *set = new DtaCommand();
	if (NULL == set) {
		LOG(E) << "Unable to create command object ";
		delete session;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	set->reset(OPAL_UID::OPAL_AUTHORITY_TABLE, OPAL_METHOD::SET);
	set->changeInvokingUid(LR);
	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken(OPAL_TOKEN::VALUES);
	set->addToken(OPAL_TOKEN::STARTLIST);

	//enable locking on the range to enforce lock state
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken(OPAL_TOKEN::READLOCKENABLED);
	set->addToken(OPAL_TOKEN::OPAL_TRUE);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken(OPAL_TOKEN::WRITELOCKENABLED);
	set->addToken(OPAL_TOKEN::OPAL_TRUE);
	set->addToken(OPAL_TOKEN::ENDNAME);
	//set read/write locked
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
	LOG(D1) << "Exiting DtaDevOpal::setLockingRange_SUM";
	return 0;
}
uint8_t DtaDevOpal::setLockingSPvalue(OPAL_UID table_uid, OPAL_TOKEN name, 
	OPAL_TOKEN value,char * password, char * msg)
{
	LOG(D1) << "Entering DtaDevOpal::setLockingSPvalue";
	uint8_t lastRC;
	vector<uint8_t> table;
	table. push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		table.push_back(OPALUID[table_uid][i]);
	}
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
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
	LOG(D1) << "Exiting DtaDevOpal::setLockingSPvalue()";
	return 0;
}

uint8_t DtaDevOpal::enableUser(char * password, char * userid, OPAL_TOKEN status)
{
	LOG(D1) << "Entering DtaDevOpal::enableUser";
	uint8_t lastRC;
	vector<uint8_t> userUID;
	
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
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
	if ((lastRC = setTable(userUID, (OPAL_TOKEN)0x05, status)) != 0) {
		LOG(E) << "Unable to enable user " << userid;
		delete session;
		return lastRC;
	}
	LOG(I) << userid << " has been enabled ";
	delete session;
	LOG(D1) << "Exiting DtaDevOpal::enableUser()";
	return 0;
}
uint8_t DtaDevOpal::revertTPer(char * password, uint8_t PSID, uint8_t AdminSP)
{
	LOG(D1) << "Entering DtaDevOpal::revertTPer() " << AdminSP;
	uint8_t lastRC;
	DtaCommand *cmd = new DtaCommand();
	if (NULL == cmd) {
		LOG(E) << "Unable to create command object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		delete cmd;
		return DTAERROR_OBJECT_CREATE_FAILED;
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
	if ((lastRC = session->sendCommand(cmd, response)) != 0) {
		delete cmd;
		delete session;
		return lastRC;
	}
	LOG(I) << "revertTper completed successfully";
	session->expectAbort();
	delete cmd;
	delete session;
	LOG(D1) << "Exiting DtaDevOpal::revertTPer()";
	return 0;
}
uint8_t DtaDevOpal::loadPBA(char * password, char * filename) {
	LOG(D1) << "Entering DtaDevOpal::loadPBAimage()" << filename << " " << dev;
	uint8_t lastRC;
	uint32_t blockSize;
	uint32_t filepos = 0;
	uint32_t eofpos;
	ifstream pbafile;
	(MAX_BUFFER_LENGTH > tperMaxPacket) ? blockSize = tperMaxPacket : blockSize = MAX_BUFFER_LENGTH;
	if (blockSize > (tperMaxToken - 4)) blockSize = tperMaxToken - 4;
	vector <uint8_t> buffer, lengthtoken;
	blockSize -= sizeof(OPALHeader) + 50;  // packet overhead
	buffer.resize(blockSize);
	pbafile.open(filename, ios::in | ios::binary);
	if (!pbafile) {
		LOG(E) << "Unable to open PBA image file " << filename;
		return DTAERROR_OPEN_ERR;
	}
	pbafile.seekg(0, pbafile.end);
	eofpos = (uint32_t) pbafile.tellg(); 
	pbafile.seekg(0, pbafile.beg);

	DtaCommand *cmd = new DtaCommand();
	if (NULL == cmd) {
		LOG(E) << "Unable to create command object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
	}

	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete cmd;
		delete session;
		pbafile.close();
		return lastRC;
	}
	LOG(I) << "Writing PBA to " << dev;
	
	while (!pbafile.eof()) {
		if (eofpos == filepos) break;
		if ((eofpos - filepos) < blockSize) {
			blockSize = eofpos - filepos; // handle a short last block
			buffer.resize(blockSize);
		}
		lengthtoken.clear();
		lengthtoken.push_back(0xe2);
		lengthtoken.push_back((uint8_t) ((blockSize >> 16) & 0x000000ff));
		lengthtoken.push_back((uint8_t)((blockSize >> 8) & 0x000000ff));
		lengthtoken.push_back((uint8_t)(blockSize & 0x000000ff));
		pbafile.read((char *)buffer.data(), blockSize);
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
		filepos += blockSize;
		cout << filepos << " of " << eofpos << " " << (uint16_t) (((float)filepos/(float)eofpos) * 100) << "% blk=" << blockSize << " \r";
	}
	cout << "\n";
	delete cmd;
	delete session;
	pbafile.close();
	LOG(I) << "PBA image  " << filename << " written to " << dev;
	LOG(D1) << "Exiting DtaDevOpal::loadPBAimage()";
	return 0;
}

uint8_t DtaDevOpal::activateLockingSP(char * password)
{
	LOG(D1) << "Entering DtaDevOpal::activateLockingSP()";
	uint8_t lastRC;
	vector<uint8_t> table;
	table. push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		table.push_back(OPALUID[OPAL_UID::OPAL_LOCKINGSP_UID][i]);
	}
	DtaCommand *cmd = new DtaCommand();
	if (NULL == cmd) {
		LOG(E) << "Unable to create command object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
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
		return DTAERROR_INVALID_LIFECYCLE;
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
	LOG(D1) << "Exiting DtaDevOpal::activatLockingSP()";
	return 0;
}

uint8_t DtaDevOpal::activateLockingSP_SUM(uint8_t lockingrange, char * password)
{
	LOG(D1) << "Entering DtaDevOpal::activateLockingSP_SUM()";
	uint8_t lastRC;
	vector<uint8_t> table;
	table.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		table.push_back(OPALUID[OPAL_UID::OPAL_LOCKINGSP_UID][i]);
	}
	vector<uint8_t> LR;
	LR.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		LR.push_back(OPALUID[OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL][i]);
	}
	if (lockingrange > 0) {
		LR[6] = 0x03;
		LR[8] = lockingrange;
	}
	DtaCommand *cmd = new DtaCommand();
	if (NULL == cmd) {
		LOG(E) << "Unable to create command object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_ADMINSP_UID, password, OPAL_UID::OPAL_SID_UID)) != 0) {
		LOG(E) << "session->start failed with code " << lastRC;
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
		return DTAERROR_INVALID_LIFECYCLE;
	}
	/*if (!disk_info.SingleUser)
	{
		LOG(E) << "This Locking SP does not support Single User Mode";
		delete cmd;
		delete session;
		return DTAERROR_INVALID_COMMAND;
	}*/
	cmd->reset(OPAL_UID::OPAL_LOCKINGSP_UID, OPAL_METHOD::ACTIVATE);
	cmd->addToken(OPAL_TOKEN::STARTLIST);
		cmd->addToken(OPAL_TOKEN::STARTNAME);
			//SingleUserModeSelectionList parameter
			cmd->addToken(OPAL_SHORT_ATOM::UINT_3);
			cmd->addToken(OPAL_TINY_ATOM::UINT_06);
			cmd->addToken(OPAL_TINY_ATOM::UINT_00);
			cmd->addToken(OPAL_TINY_ATOM::UINT_00);
			cmd->addToken(OPAL_TOKEN::STARTLIST);
				cmd->addToken(LR);
			cmd->addToken(OPAL_TOKEN::ENDLIST);
		cmd->addToken(OPAL_TOKEN::ENDNAME);
	cmd->addToken(OPAL_TOKEN::ENDLIST);
	cmd->complete();
	if ((lastRC = session->sendCommand(cmd, response)) != 0) {
		LOG(E) << "session->sendCommand failed with code " << lastRC;
		delete cmd;
		delete session;
		return lastRC;
	}
	disk_info.Locking_lockingEnabled = 1;
	LOG(I) << "Locking SP Activate Complete for single User" << (lockingrange+1) << " on locking range " << (int)lockingrange;

	delete cmd;
	delete session;
	LOG(D1) << "Exiting DtaDevOpal::activateLockingSP_SUM()";
	return 0;
}

uint8_t DtaDevOpal::eraseLockingRange_SUM(uint8_t lockingrange, char * password)
{
	uint8_t lastRC;
	LOG(D1) << "Entering DtaDevOpal::eraseLockingRange_SUM";
	vector<uint8_t> LR;
	LR.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		LR.push_back(OPALUID[OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL][i]);
	}
	if (lockingrange != 0) {
		LR[6] = 0x03;
		LR[8] = lockingrange;
	}
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete session;
		return lastRC;
	}

	DtaCommand *cmd = new DtaCommand();
	if (NULL == cmd) {
		LOG(E) << "Unable to create command object ";
		delete session;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	cmd->reset(OPAL_UID::OPAL_AUTHORITY_TABLE, OPAL_METHOD::ERASE);
	cmd->changeInvokingUid(LR);
	cmd->addToken(OPAL_TOKEN::STARTLIST);
	cmd->addToken(OPAL_TOKEN::ENDLIST);
	cmd->complete();
	if ((lastRC = session->sendCommand(cmd, response)) != 0) {
		LOG(E) << "setLockingRange Failed ";
		delete cmd;
		delete session;
		return lastRC;
	}
	delete cmd;
	delete session;
	LOG(I) << "LockingRange" << (uint16_t)lockingrange << " erased";
	LOG(D1) << "Exiting DtaDevOpal::eraseLockingRange_SUM";
	return 0;
}

uint8_t DtaDevOpal::takeOwnership(char * newpassword)
{
	LOG(D1) << "Entering DtaDevOpal::takeOwnership()";
	uint8_t lastRC;
	if ((lastRC = getDefaultPassword()) != 0) {
		LOG(E) << "Unable to read MSID password ";
		return lastRC;
	}
	if ((lastRC = setSIDPassword((char *)response.getString(4).c_str(), newpassword, 0)) != 0) {
		LOG(E) << "takeOwnership failed";
		return lastRC;
	}
	LOG(I) << "takeOwnership complete";
	LOG(D1) << "Exiting takeOwnership()";
	return 0;
}
uint8_t DtaDevOpal::getDefaultPassword()
{
	LOG(D1) << "Entering DtaDevOpal::getDefaultPassword()";
	uint8_t lastRC;
	vector<uint8_t> hash;
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
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
uint8_t DtaDevOpal::printDefaultPassword()
{
    const uint8_t rc = getDefaultPassword();
	if (rc) {
		LOG(E) << "unable to read MSID password";
		return rc;
	}
	string defaultPassword = response.getString(4);
    fprintf(stdout, "MSID: %s\n", (char *)defaultPassword.c_str());
    return 0;
}
uint8_t DtaDevOpal::setSIDPassword(char * oldpassword, char * newpassword,
	uint8_t hasholdpwd, uint8_t hashnewpwd)
{
	vector<uint8_t> hash, table;
	LOG(D1) << "Entering DtaDevOpal::setSIDPassword()";
	uint8_t lastRC;
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
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
		DtaHashPwd(hash, newpassword, this);
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
	LOG(I) << "SID password changed";
	delete session;
	LOG(D1) << "Exiting DtaDevOpal::setSIDPassword()";
	return 0;
}

uint8_t DtaDevOpal::setTable(vector<uint8_t> table, OPAL_TOKEN name,
	OPAL_TOKEN value)
{
	vector <uint8_t> token;
	token.push_back((uint8_t) value);
	return(setTable(table, name, token));
}

uint8_t DtaDevOpal::setTable(vector<uint8_t> table, OPAL_TOKEN name, 
	vector<uint8_t> value)
{
	LOG(D1) << "Entering DtaDevOpal::setTable";
	uint8_t lastRC;
	DtaCommand *set = new DtaCommand();
	if (NULL == set) {
		LOG(E) << "Unable to create command object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
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
	LOG(D1) << "Leaving DtaDevOpal::setTable";
	return 0;
}
uint8_t DtaDevOpal::getTable(vector<uint8_t> table, uint16_t startcol, 
	uint16_t endcol)
{
	LOG(D1) << "Entering DtaDevOpal::getTable";
	uint8_t lastRC;
	DtaCommand *get = new DtaCommand();
	if (NULL == get) {
		LOG(E) << "Unable to create command object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
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
uint8_t DtaDevOpal::exec(DtaCommand * cmd, DtaResponse & resp, uint8_t protocol)
{
	uint8_t lastRC;
    OPALHeader * hdr = (OPALHeader *) cmd->getCmdBuffer();
    LOG(D3) << endl << "Dumping command buffer";
    IFLOG(D) DtaAnnotatedDump(IF_SEND, cmd->getCmdBuffer(), cmd->outputBufferSize());
    IFLOG(D3) DtaHexDump(cmd->getCmdBuffer(), SWAP32(hdr->cp.length) + sizeof (OPALComPacket));
    if((lastRC = sendCmd(IF_SEND, protocol, comID(), cmd->getCmdBuffer(), cmd->outputBufferSize())) != 0) {
		LOG(E) << "Command failed on send " << (uint16_t) lastRC;
        return lastRC;
    }
    hdr = (OPALHeader *) cmd->getRespBuffer();
    do {
        osmsSleep(25);
        memset(cmd->getRespBuffer(), 0, MIN_BUFFER_LENGTH);
        lastRC = sendCmd(IF_RECV, protocol, comID(), cmd->getRespBuffer(), MIN_BUFFER_LENGTH);

    }
    while ((0 != hdr->cp.outstandingData) && (0 == hdr->cp.minTransfer));
    LOG(D3) << std::endl << "Dumping reply buffer";
    IFLOG(D) DtaAnnotatedDump(IF_RECV, cmd->getRespBuffer(), SWAP32(hdr->cp.length) + sizeof (OPALComPacket));
    IFLOG(D3) DtaHexDump(cmd->getRespBuffer(), SWAP32(hdr->cp.length) + sizeof (OPALComPacket));
	if (0 != lastRC) {
        LOG(E) << "Command failed on recv" << (uint16_t) lastRC;
        return lastRC;
    }
    resp.init(cmd->getRespBuffer());
    return 0;
}


uint8_t DtaDevOpal::properties()
{
	LOG(D1) << "Entering DtaDevOpal::properties()";
	uint8_t lastRC;
	session = new DtaSession(this);  // use the session IO without starting a session
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	DtaCommand *props = new DtaCommand(OPAL_UID::OPAL_SMUID_UID, OPAL_METHOD::PROPERTIES);
	if (NULL == props) {
		LOG(E) << "Unable to create command object ";
		delete session;
		return DTAERROR_OBJECT_CREATE_FAILED;
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
	for (uint32_t i = 0; i < propertiesResponse.getTokenCount(); i++) {
		if (OPAL_TOKEN::STARTNAME == propertiesResponse.tokenIs(i)) {
			if (OPAL_TOKEN::DTA_TOKENID_BYTESTRING != propertiesResponse.tokenIs(i + 1))
				break;
			else
				if(!strcasecmp("MaxComPacketSize",propertiesResponse.getString(i + 1).c_str()))
					tperMaxPacket = propertiesResponse.getUint32(i + 2);
				else
					if (!strcasecmp("MaxIndTokenSize", propertiesResponse.getString(i + 1).c_str())) {
						tperMaxToken = propertiesResponse.getUint32(i + 2);
						break;
					}

			i += 2;
		}
	}
	LOG(D1) << "Leaving DtaDevOpal::properties()";
	return 0;
}
void DtaDevOpal::puke()
{
	LOG(D1) << "Entering DtaDevOpal::puke()";
	DtaDev::puke();
	if (disk_info.Properties) {
		cout << std::endl << "TPer Properties: ";
		for (uint32_t i = 0; i < propertiesResponse.getTokenCount(); i++) {
			if (OPAL_TOKEN::STARTNAME == propertiesResponse.tokenIs(i)) {
				if (OPAL_TOKEN::DTA_TOKENID_BYTESTRING != propertiesResponse.tokenIs(i + 1))
					cout << std::endl << "Host Properties: " << std::endl;
				else
					cout << "  " << propertiesResponse.getString(i + 1) << " = " << propertiesResponse.getUint64(i + 2);
				i += 2;
			}
			if (!(i % 6)) cout << std::endl;
		}
	}
}

uint8_t DtaDevOpal::objDump(char *sp, char * auth, char *pass,
	char * objID)
{

	LOG(D1) << "Entering DtaDevEnterprise::objDump";
	LOG(D1) << sp << " " << auth << " " << pass << " " << objID;
	uint8_t lastRC;
	DtaCommand *get = new DtaCommand();
	if (NULL == get) {
		LOG(E) << "Unable to create command object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	vector<uint8_t> authority, object;
	uint8_t work;
	if (16 != strnlen(auth, 32)) {
		LOG(E) << "Authority must be 16 byte ascii string of hex authority uid";
		return DTAERROR_INVALID_PARAMETER;
	}
	if (16 != strnlen(objID, 32)) {
		LOG(E) << "ObjectID must be 16 byte ascii string of hex object uid";
		return DTAERROR_INVALID_PARAMETER;
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
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		delete get;
		return DTAERROR_OBJECT_CREATE_FAILED;
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
	LOG(D1) << "Exiting DtaDevEnterprise::objDump";
	return 0;
}
uint8_t DtaDevOpal::rawCmd(char *sp, char * hexauth, char *pass,
	char *hexinvokingUID, char *hexmethod, char *hexparms) {
	LOG(D1) << "Entering DtaDevEnterprise::rawCmd";
	LOG(D1) << sp << " " << hexauth << " " << pass << " ";
	LOG(D1) << hexinvokingUID << " " << hexmethod << " " << hexparms;
	uint8_t lastRC;
	vector<uint8_t> authority, object, invokingUID, method, parms;
	uint8_t work;
	if (16 != strnlen(hexauth, 32)) {
		LOG(E) << "Authority must be 16 byte ascii string of hex authority uid";
		return DTAERROR_INVALID_PARAMETER;
	}
	authority.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (uint32_t i = 0; i < 16; i += 2) {
		work = hexauth[i] & 0x40 ? 16 * ((hexauth[i] & 0xf) + 9) : 16 * (hexauth[i] & 0x0f);
		work += hexauth[i + 1] & 0x40 ? (hexauth[i + 1] & 0xf) + 9 : hexauth[i + 1] & 0x0f;
		authority.push_back(work);
	}
	if (16 != strnlen(hexinvokingUID, 32)) {
		LOG(E) << "invoker must be 16 byte ascii string of invoking uid";
		return DTAERROR_INVALID_PARAMETER;
	}
	invokingUID.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (uint32_t i = 0; i < 16; i += 2) {
		work = hexinvokingUID[i] & 0x40 ? 16 * ((hexinvokingUID[i] & 0xf) + 9) : 16 * (hexinvokingUID[i] & 0x0f);
		work += hexinvokingUID[i + 1] & 0x40 ? (hexinvokingUID[i + 1] & 0xf) + 9 : hexinvokingUID[i + 1] & 0x0f;
		invokingUID.push_back(work);
	}
	if (16 != strnlen(hexmethod, 32)) {
		LOG(E) << "invoker must be 16 byte ascii string of method uid";
		return DTAERROR_INVALID_PARAMETER;
	}
	method.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (uint32_t i = 0; i < 16; i += 2) {
		work = hexmethod[i] & 0x40 ? 16 * ((hexmethod[i] & 0xf) + 9) : 16 * (hexmethod[i] & 0x0f);
		work += hexmethod[i + 1] & 0x40 ? (hexmethod[i + 1] & 0xf) + 9 : hexmethod[i + 1] & 0x0f;
		method.push_back(work);
	}
	if (1020 < strnlen(hexparms, 1024)) {
		LOG(E) << "Parmlist limited to 1020 characters";
		return DTAERROR_INVALID_PARAMETER;
	}
	if (strnlen(hexparms, 1024) % 2) {
		LOG(E) << "Parmlist must be even number of bytes";
		return DTAERROR_INVALID_PARAMETER;
	}

	for (uint32_t i = 0; i < strnlen(hexparms, 1024); i += 2) {
		work = hexparms[i] & 0x40 ? 16 * ((hexparms[i] & 0xf) + 9) : 16 * (hexparms[i] & 0x0f);
		work += hexparms[i + 1] & 0x40 ? (hexparms[i + 1] & 0xf) + 9 : hexparms[i + 1] & 0x0f;
		parms.push_back(work);
	}
	DtaCommand *cmd = new DtaCommand();
	if (NULL == cmd) {
		LOG(E) << "Unable to create command object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	cmd->reset(OPAL_UID::OPAL_AUTHORITY_TABLE, method);
	cmd->changeInvokingUid(invokingUID);
	cmd->addToken(parms);
	cmd->complete();
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		delete cmd;
		return DTAERROR_OBJECT_CREATE_FAILED;
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
	LOG(D1) << "Exiting DtaDevEnterprise::rawCmd";
	return 0;
}