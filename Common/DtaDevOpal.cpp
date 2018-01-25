/* C:B**************************************************************************
This software is Copyright 2014-2016 Bright Plaza Inc. <drivetrust@drivetrust.com>

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
#if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__)
#else
#include <Windows.h>
#include "compressapi-8.1.h"
#endif
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <iomanip>

#include "DtaDevOpal.h"
#include "DtaHashPwd.h"
#include "DtaEndianFixup.h"
#include "DtaStructures.h"
#include "DtaCommand.h"
#include "DtaResponse.h"
#include "DtaSession.h"
#include "DtaHexDump.h"
#include <signal.h>
#include "sedsize.h" 

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
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, getusermode() ? OPAL_UID::OPAL_USER1_UID : OPAL_UID::OPAL_ADMIN1_UID)) != 0) { // NG JERRY
	//if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
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
	// JERRY dump raw token info 
	uint32_t tc = response.getTokenCount();
	printf("***** getTokenCount()=%ld\n", tc);
	for (uint32_t i = 0; i < tc; i++) {
		printf("token %ld = ", i);
		for (uint32_t j = 0; j < response.getRawToken(i).size(); j++)
			printf("%02X ", response.getRawToken(i)[j]);
		cout << endl;
	}

	// JERRY
	if (response.tokenIs(4) != _OPAL_TOKEN::DTA_TOKENID_UINT) {
		LOG(E) << "Unable to determine number of ranges ";
		delete session;
		return DTAERROR_NO_LOCKING_INFO;
	}
	LOG(I) << "Locking Range Configuration for " << dev;
	uint32_t numRanges = response.getUint32(4) + 1;
	for (uint32_t i = 0; i < numRanges; i++){
		if(0 != i) LR[8] = i & 0xff;
		// JERRY 
		if (0) {
			for (uint8_t k = 0; k < LR.size(); k++) printf("%02X ", LR[k]);
			cout << endl;
		}
		// JERRY
		if ((lastRC = getTable(LR, _OPAL_TOKEN::RANGESTART, _OPAL_TOKEN::WRITELOCKED)) != 0) {
			delete session;
			return lastRC;
		}
		// JERRY dump raw token info 
		uint32_t tc = response.getTokenCount();
		if (0) {
			printf("***** getTokenCount()=%ld\n", tc);
			for (uint32_t i = 0; i < tc; i++) {
				printf("token %ld = ", i);
				for (uint32_t j = 0; j < response.getRawToken(i).size(); j++)
					printf("%02X ", response.getRawToken(i)[j]);
				cout << endl;
			}
		}
		if (tc != 34) { // why ?????
			cout << endl;
			LOG(E) << "token count is wrong. Exit loop";
			break;
		}
		// JERRY

		LR[6] = 0x03;  // non global ranges are 00000802000300nn 
		//LOG(I) << "LR" << i << " Begin " << response.getUint64(4) <<
		cout << "LR" << i << " Begin " << response.getUint64(4) <<
			" for " << response.getUint64(8);
		//LOG(I)	<< "            RLKEna =" << (response.getUint8(12) ? " Y " : " N ") <<
        // 12 blank space --> 1 space
		cout	<< " RLKEna =" << (response.getUint8(12) ? " Y " : " N ") <<
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
	//if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, getusermode() ? OPAL_UID::OPAL_USER1_UID : OPAL_UID::OPAL_ADMIN1_UID)) != 0) { // NG : JERRY 
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
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, getusermode() ? OPAL_UID::OPAL_USER1_UID : OPAL_UID::OPAL_ADMIN1_UID)) != 0) { // NG : JERRY 
	//if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
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
	LOG(D1) << "Entering revert DtaDevOpal::revertLockingSP() keep = " << (uint16_t) keep;
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
	LOG(D1) << "Exiting revert DtaDev:LockingSP()";
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
	// if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, getusermode() ? OPAL_UID::OPAL_USER1_UID : OPAL_UID::OPAL_ADMIN1_UID)) != 0) { // ok work : JERRY can user set its own password ?????
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
	//auditRec(newpassword, memcmp(userid, "Admin", 5) ? (uint8_t)evt_PasswordChangedUser: (uint8_t)evt_PasswordChangedAdmin);
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
	OPAL_TOKEN readlocked, writelocked;
	const char *msg;

	LOG(D1) << "Entering DtaDevOpal::setLockingRange";
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
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, Admin1Password, getusermode() ? OPAL_UID::OPAL_USER1_UID : OPAL_UID::OPAL_ADMIN1_UID)) != 0) { // JERRY can User1 set Lockingrange RW ??????
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
	
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, getusermode()? OPAL_UID::OPAL_USER1_UID : OPAL_UID::OPAL_ADMIN1_UID)) != 0) { // JERRY ADMIN1 or USER1 ?????
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

uint8_t DtaDevOpal::enableUser(uint8_t mbrstate, char * password, char * userid)
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
	if ((lastRC = setTable(userUID, (OPAL_TOKEN)0x05, mbrstate ? OPAL_TOKEN::OPAL_TRUE : OPAL_TOKEN::OPAL_FALSE)) != 0) {
		LOG(E) << "Unable to enable user " << userid;
		delete session;
		return lastRC;
	}
	/*
	if ((lastRC = getAuth4User("User2", 0, userUID)) != 0) {
		LOG(E) << "Unable to find user " << "User2" << " in Authority Table";
		delete session;
		return lastRC;
	}
	if ((lastRC = setTable(userUID, (OPAL_TOKEN)0x05, mbrstate ? OPAL_TOKEN::OPAL_TRUE : OPAL_TOKEN::OPAL_FALSE)) != 0) {
		LOG(E) << "Unable to enable user " << "User2";
		delete session;
		return lastRC;
	}
	*/
	if (mbrstate) 
		LOG(I) << userid << " has been enabled ";
	else 
		LOG(I) << userid << " has been disabled";

	delete session;
	LOG(D1) << "Exiting DtaDevOpal::enableUser()";
	return 0;
}


OPAL_UID getUIDtoken(char * userid)
{
	// translate UserN AdminN into <int8_t 
	vector<uint8_t> auth;
	uint8_t id;
	if (!memcmp("User", userid, 4)) {// UserI UID
		id = (uint8_t)(OPAL_UID::OPAL_USER1_UID) + atoi(&userid[4]) -1; 
		printf("UserN=%s enum=%d\n", userid, id);
		return  (OPAL_UID)id; 
	}
	else
	{
		id = (uint8_t)(OPAL_UID::OPAL_ADMIN1_UID) + atoi(&userid[4]) -1 ;
		printf("AdminN=%s enum=%d\n", userid, id);
		return  (OPAL_UID)id;
	}
}

vector<uint8_t> getUID(char * userid, vector<uint8_t> &auth2)
{
	// translate UserN AdminN into <int8_t 
	vector<uint8_t> auth;
	;
	uint8_t id;
	auth.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	auth2.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 7; i++) {
		if (!memcmp("User", userid, 4)) {// UserI UID
			//printf("UserN : %s", userid);
			auth.push_back(OPALUID[OPAL_UID::OPAL_USER1_UID][i]);
			id = (uint8_t)atoi(&userid[4]); // (uint8_t)atoi(argv[opts.dsnum])
			auth2.push_back(OPALUID[OPAL_UID::OPAL_ADMIN1_UID][i]); 
		}
		else { // "Admin"
			//printf("AdminN %s\n", userid);
			auth.push_back(OPALUID[OPAL_UID::OPAL_ADMIN1_UID][i]);
			id = (uint8_t)atoi(&userid[5]);
			auth2.push_back(OPALUID[OPAL_UID::OPAL_USER1_UID][i]);
		}
	}
	auth.push_back(id);
	auth2.push_back(id);
	return auth;
}


uint8_t DtaDevOpal::userAcccessEnable(uint8_t mbrstate, OPAL_UID UID, char * userid)
{
	uint8_t lastRC;
	// Give UserN read access to the DataStore table
	DtaCommand *cmd = new DtaCommand();
	if (NULL == cmd) {
		LOG(E) << "Unable to create command object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
	}

	cmd->reset(UID, OPAL_METHOD::SET);
	cmd->addToken(OPAL_TOKEN::STARTLIST);
	cmd->addToken(OPAL_TOKEN::STARTNAME);
	cmd->addToken(OPAL_TOKEN::VALUES);
	cmd->addToken(OPAL_TOKEN::STARTLIST);
	cmd->addToken(OPAL_TOKEN::STARTNAME);
	cmd->addToken(OPAL_TOKEN::OPAL_BOOLEAN_EXPR);
	cmd->addToken(OPAL_TOKEN::STARTLIST);
	// User1
	cmd->addToken(OPAL_TOKEN::STARTNAME);
	cmd->addToken(OPAL_UID::OPAL_HALF_UID_AUTHORITY_OBJ_REF, 4); //????? how to insert 4-byte here, addToken will insert BYTESTRING4 token
	// translate UserN AdminN into <int8_t>
	vector<uint8_t> auth, auth2;
	auth = getUID(userid, auth2);
	cmd->addToken(auth);
	//for (int i = 0; i < 9; i++) printf("%02X, ", auth[i]);  printf("\n"); 

	cmd->addToken(OPAL_TOKEN::ENDNAME);
	// User2 
	cmd->addToken(OPAL_TOKEN::STARTNAME);
	cmd->addToken(OPAL_UID::OPAL_HALF_UID_AUTHORITY_OBJ_REF, 4); //????? how to insert 4-byte here, addToken will insert BYTESTRING4 token

	////// auth.at(8) = auth.at(8) + 1;
	////// for (int i = 0; i < 9; i++) printf("%02X, ", auth[i]);  printf("\n");
	cmd->addToken(auth2);  
	cmd->addToken(OPAL_TOKEN::ENDNAME);
	//
	cmd->addToken(OPAL_TOKEN::STARTNAME);
	cmd->addToken(OPAL_UID::OPAL_HALF_UID_BOOLEAN_ACE, 4);
	cmd->addToken(mbrstate ? OPAL_TOKEN::VALUES : OPAL_TOKEN::WHERE);
	cmd->addToken(OPAL_TOKEN::ENDNAME);
	cmd->addToken(OPAL_TOKEN::ENDLIST);
	cmd->addToken(OPAL_TOKEN::ENDNAME);
	cmd->addToken(OPAL_TOKEN::ENDLIST);
	cmd->addToken(OPAL_TOKEN::ENDNAME);
	cmd->addToken(OPAL_TOKEN::ENDLIST);
	cmd->complete();

	//LOG(I) << "Dump enable user access cmd buffer";
	//IFLOG(D1) DtaHexDump(cmd->cmdbuf, 512);

	if ((lastRC = session->sendCommand(cmd, response)) != 0) {
		LOG(E) << "***** send enable/disable user access command fail";
		delete cmd;
		return lastRC;
	}
	LOG(I) << "***** " << (mbrstate ? "enable" : "disable") << " user access command OK";

	delete cmd;
	LOG(D1) << "***** end of enable/disable user access command ";
	return 0;
}
uint8_t DtaDevOpal::enableUserRead(uint8_t mbrstate, char * password, char * userid)
{
	LOG(D1) << "Entering DtaDevOpal::enableUserRead";
	uint8_t lastRC;
	uint8_t error;
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
	/*	OPAL_ACE_DataStore_Get_All,
		OPAL_ACE_MBRControl_Set_Done,
	    OPAL_ACE_LOCKINGRANGE_RDLOCKED,
		OPAL_ACE_LOCKINGRANGE_WRLOCKED,
	*/
	error = 0;
	LOG(D1) << "***** " << (mbrstate ? "enable " : "disbale ") << "OPAL_ACE_DataStore_Get_All for " << userid; 
	error = userAcccessEnable(mbrstate, OPAL_UID::OPAL_ACE_DataStore_Get_All,userid);
	LOG(D1) << "***** " << (mbrstate ? "enable " : "disbale ") << "enable OPAL_ACE_DataStore_Set_All for " << userid;
	error = userAcccessEnable(mbrstate, OPAL_UID::OPAL_ACE_DataStore_Set_All, userid);
	LOG(D1) << "***** " << (mbrstate ? "enable " : "disbale ") << "enable OPAL_ACE_MBRControl_Set_Done for " << userid;
	error |= userAcccessEnable(mbrstate, OPAL_UID::OPAL_ACE_MBRControl_Set_Done, userid);
	// DO NOT turn on lockingrange 1
	//LOG(D1) << "***** " << (mbrstate ? "enable " : "disbale ") << "enable OPAL_ACE_LOCKINGRANGE1_RDLOCKED for " << userid;
	//error |= userAcccessEnable(mbrstate, OPAL_UID::OPAL_ACE_LOCKINGRANGE1_RDLOCKED, userid);
	//LOG(D1) << "***** " << (mbrstate ? "enable " : "disbale ") << "enable OPAL_ACE_LOCKINGRANGE1_WRLOCKED for " << userid;
	//error |= userAcccessEnable(mbrstate, OPAL_UID::OPAL_ACE_LOCKINGRANGE1_WRLOCKED, userid);
	/*
	OPAL_ACE_MBRControl_Set_Enable,
	ACE_Locking_GlobalRange_Get_RangeStartToActiveKey,
	ACE_Locking_GlobalRange_Set_ReadLocked,
	ACE_Locking_GlobalRange_Set_WriteLocked,
	ACE_Locking_GlobalRange_Admin_Set,	// allow to set/reset
	ACE_Locking_GlobalRange_Admin_Start, // allow to set/reset range start and length
	*/
	LOG(D1) << "***** " << (mbrstate ? "enable " : "disbale ") << "OPAL_ACE_MBRControl_Set_Enable for " << userid;
	error |= userAcccessEnable(mbrstate, OPAL_UID::OPAL_ACE_MBRControl_Set_Enable, userid); // NG6
	LOG(D1) << "***** " << (mbrstate ? "enable " : "disbale ") << "OPAL_ACE_Locking_GlobalRange_Get_RangeStartToActiveKey for " << userid;
	error |= userAcccessEnable(mbrstate, OPAL_UID::OPAL_ACE_Locking_GlobalRange_Get_RangeStartToActiveKey, userid);
	LOG(D1) << "***** " << (mbrstate ? "enable " : "disbale ") << "OPAL_ACE_Locking_GlobalRange_Get_RangeStartToActiveKey for " << userid;
	error |= userAcccessEnable(mbrstate, OPAL_UID::OPAL_ACE_Locking_GlobalRange_Set_ReadLocked, userid);
	LOG(D1) << "***** " << (mbrstate ? "enable " : "disbale ") << "OPAL_ACE_Locking_GlobalRange_Set_WriteLocked for " << userid;
	error |= userAcccessEnable(mbrstate, OPAL_UID::OPAL_ACE_Locking_GlobalRange_Set_WriteLocked, userid);
	LOG(D1) << "***** " << (mbrstate ? "enable " : "disbale ") << "OPAL_ACE_Locking_GlobalRange_Admin_Set for " << userid;
	error |= userAcccessEnable(mbrstate, OPAL_UID::OPAL_ACE_Locking_GlobalRange_Admin_Set, userid); // NG10
	LOG(D1) << "***** " << (mbrstate ? "enable " : "disbale ") << "OPAL_ACE_Locking_GlobalRange_Admin_Start for " << userid;
	error |= userAcccessEnable(mbrstate, OPAL_UID::OPAL_ACE_Locking_GlobalRange_Admin_Start, userid); // NG11

	if (error) {
		LOG(E) << (mbrstate ? "enable " : "disbale ") << "one of user accese fail";
		delete session;
		return error;
	}

	delete session;
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
	session->expectAbort();
	if ((lastRC = session->sendCommand(cmd, response)) != 0) {
		delete cmd;
		delete session;
		return lastRC;
	}
	LOG(I) << "revertTper completed successfully";
	delete cmd;
	delete session;

	//auditRec(password, evt_Revert);
	LOG(D1) << "Exiting DtaDevOpal::revertTPer()";
	return 0;
}



uint8_t DtaDevOpal::pbaValid(char * password)
{
	// check if boot sector exist 55AA(offset 510-511)  FAT16(
	// get PBA version offset 512 to (up to 32 bytes)
	#if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__)
	LOG(D1) << "DtaDevOpal::pbaValid() isn't supported in Linux";
	return 0;
	#else
	
	uint8_t lastRC;
	uint8_t boot[512];
	uint8_t pbaver[512];
	uint8_t bootsig[2] = { 0x55,0xAA };
	uint8_t pat[16] = { 0xEB,0x3C, 0x90, 0x6D,  0x6B, 0x66, 0x73, 0x2E,  0x66, 0x61, 0x74, };
	lastRC = MBRRead(password, 512, 512, (char *)pbaver);
	if (lastRC) {
		LOG(D1) << "MBRRead error";
		return lastRC;
	}
	IFLOG(D4) DtaHexDump(pbaver, 64);
	printf("PBA image version : %s", (char *)pbaver);

	lastRC = MBRRead(password, 1048576,512,(char *)boot);
	if (lastRC) {
		LOG(D1) << "MBRRead error";
		return lastRC;
	}
	lastRC = (uint8_t)memcmp(boot, pat, 11);
	if (lastRC) {
		LOG(D1) << "boot sector header incorrect";
		return lastRC;
	}
	lastRC = (uint8_t)memcmp(boot + 0x36, "FAT16", 5);
	if (lastRC) {
		LOG(D1) << "boot sector FAT16 signature incorrect";
		return lastRC;
	}
	lastRC = (uint8_t)memcmp(boot + 510, (char *)bootsig, 5);
	if (lastRC) {
		LOG(D1) << "boot sector signature 55AA incorrect";
		return lastRC;
	}
	printf("PBA image valid");
	/*
	// audit log header version
	char * buffer;

	buffer = (char *)malloc(8 * MAX_ENTRY + gethdrsize());
	memset(buffer, 0, (MAX_ENTRY * 8) + gethdrsize());
	lastRC = auditlogrd(password, 0, (MAX_ENTRY * 8) + gethdrsize(), buffer);
	if (lastRC) {
		LOG(D1) << "Audit read error : " << lastRC;
		return lastRC;
	};
	
	audit_hdr * phdr = (audit_hdr *)buffer;
	printf("audit log version : %d.%d", phdr->ver_major, phdr->ver_minor);
	*/
	return 0;
	#endif
}


uint8_t DtaDevOpal::MBRRead(char * password, uint32_t startpos, uint32_t len,char * buffer)
{
	LOG(D1) << "Entering DtaDevOpal::MBRRead() with buffer";
	uint8_t lastRC;
	vector<uint8_t> LR;
	uint32_t filepos = 0; // startpos;
	uint32_t blocksize = len;

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
		return lastRC;
	}

	LOG(D1) << "***** read shadow mbr";
	cmd->reset(OPAL_UID::OPAL_MBR, OPAL_METHOD::GET);
	cmd->addToken(OPAL_TOKEN::STARTLIST);
	cmd->addToken(OPAL_TOKEN::STARTLIST);
	cmd->addToken(OPAL_TOKEN::STARTNAME);
	cmd->addToken(OPAL_TOKEN::STARTROW);
	cmd->addToken(filepos + startpos);
	cmd->addToken(OPAL_TOKEN::ENDNAME);
	cmd->addToken(OPAL_TOKEN::STARTNAME);
	cmd->addToken(OPAL_TOKEN::ENDROW);
	cmd->addToken(filepos + startpos + blocksize - 1);
	cmd->addToken(OPAL_TOKEN::ENDNAME);
	cmd->addToken(OPAL_TOKEN::ENDLIST);
	cmd->addToken(OPAL_TOKEN::ENDLIST);
	cmd->complete();
	LOG(D1) << "***** send read shadow mbr command ";
	if ((lastRC = session->sendCommand(cmd, response)) != 0) {
		delete cmd;
		delete session;
		return lastRC;
	}
	response.getBytes(1, (uint8_t *)buffer); // data is token 1
	LOG(D1) << "***** end of send read shadow mbr command ";
	delete cmd;
	delete session;

	return 0;

}

uint8_t DtaDevOpal::DataRead(char * password, uint32_t startpos, uint32_t len, char * buffer, char * userid)
{
	LOG(D1) << "Entering DtaDevOpal::DataRead()";
	uint8_t lastRC;
	vector<uint8_t> LR;
	uint32_t filepos = startpos;
	uint32_t blocksize = len;

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
	LOG(D1) << "start lockingSP session";
	// ???????????????????????????????????????????????????????????????????????????????????
	// experiement start lockingSP with User1 password"
	// ????????????????????????????????????????????????????????????????????????????????????
		// translate UserN AdminN into <int8_t 
		//printf(" ***** start LOCKINGSP with %s  Token = %d\n", userid, getUIDtoken(userid));
		vector<uint8_t> auth,auth2;
		auth = getUID(userid,auth2); // pass vector directly, not enum index of vector table
		//for (int i = 0; i < 9; i++) {
		//	printf("%02X ", auth[i]);
		//} 
		//printf("\n");
		if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, auth) != 0)) { // JERRY OPAL_UID::OPAL_ADMIN1_UID ->OK getUID() --> OK too; getUIDtoken-->NG ????
			delete cmd;
			delete session;
			return lastRC;
		}

	LOG(D1) << "***** start read data store";

	cmd->reset(OPAL_UID::OPAL_DATA_STORE, OPAL_METHOD::GET);
	cmd->addToken(OPAL_TOKEN::STARTLIST);
	cmd->addToken(OPAL_TOKEN::STARTLIST);
	cmd->addToken(OPAL_TOKEN::STARTNAME);
	cmd->addToken(OPAL_TOKEN::STARTROW);
	cmd->addToken(filepos);
	cmd->addToken(OPAL_TOKEN::ENDNAME);
	cmd->addToken(OPAL_TOKEN::STARTNAME);
	cmd->addToken(OPAL_TOKEN::ENDROW);
	cmd->addToken(blocksize);
	cmd->addToken(OPAL_TOKEN::ENDNAME);
	cmd->addToken(OPAL_TOKEN::ENDLIST);
	cmd->addToken(OPAL_TOKEN::ENDLIST);
	cmd->complete();
	LOG(D1) << "***** send read data store command ";
	if ((lastRC = session->sendCommand(cmd, response)) != 0) {
	delete cmd;
	delete session;
	return lastRC;
	}
	
	response.getBytes(1, (uint8_t *) buffer); // data is token 1
	LOG(D1) << "raw data after data store read response data";

	IFLOG(D4) DtaHexDump(buffer, gethdrsize()); // contain header and entries
	LOG(D1) << "***** end of send read data store command ";
	delete cmd;
	delete session;
	return 0;
	
}

uint8_t DtaDevOpal::DataWrite(char * password, uint32_t startpos, uint32_t len, char * buffer, char * userid)
{
	LOG(D1) << "Entering DtaDevOpal::DataWrite()";
	uint8_t lastRC;
	vector<uint8_t> LR;
	vector <uint8_t> bufferA; // (8192, 0x66); // 0 buffer  (57344, 0x00),
	vector <uint8_t> lengthtoken;
	uint32_t filepos = startpos;

	bufferA.insert(bufferA.begin(), buffer, buffer + len); 
	//////////////////////////////////////////////
	LOG(D1) << "bufferA contents after copy passed buffer content";
	IFLOG(D4) DtaHexDump(bufferA.data(), 256);
	//////////////////////////////////////////////
	lengthtoken.clear();
	lengthtoken.push_back(0xe2); // 8k = 8192 (0x2000)
	lengthtoken.push_back(0x00);
	lengthtoken.push_back((len>>8) & 0x000000ff);
	lengthtoken.push_back(len & 0x000000ff);

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
	LOG(D1) << "start lockingSP session";
	vector<uint8_t> auth,auth2;
	auth = getUID(userid,auth2);
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, auth)) != 0) { // OPAL_UID::OPAL_ADMIN1_UID
	delete cmd;
	delete session;
	return lastRC;
	}
	LOG(D1) << "Start transaction";
	cmd->reset();
	cmd->addToken(OPAL_TOKEN::STARTTRANSACTON);
	cmd->addToken(OPAL_TOKEN::WHERE);
	cmd->complete();
	// cmd buffer before write 
	LOG(D1) << "bufferA before send write data command";
	IFLOG(D4) DtaHexDump(bufferA.data(), 256);
	// //////////////////////////
	if ((lastRC = session->sendCommand(cmd, response)) != 0) {
	delete cmd;
	delete session;
	return lastRC;
	}
	LOG(D1) << "Writing to data store 0" << dev;

	cmd->reset(OPAL_UID::OPAL_DATA_STORE, OPAL_METHOD::SET);
	cmd->addToken(OPAL_TOKEN::STARTLIST);
	cmd->addToken(OPAL_TOKEN::STARTNAME);
	cmd->addToken(OPAL_TOKEN::WHERE);
	cmd->addToken(filepos);
	cmd->addToken(OPAL_TOKEN::ENDNAME);
	cmd->addToken(OPAL_TOKEN::STARTNAME);
	cmd->addToken(OPAL_TOKEN::VALUES);
	cmd->addToken(lengthtoken);
	cmd->addToken(bufferA);
	cmd->addToken(OPAL_TOKEN::ENDNAME);
	cmd->addToken(OPAL_TOKEN::ENDLIST);
	cmd->complete();
	if ((lastRC = session->sendCommand(cmd, response)) != 0) {
	delete cmd;
	delete session;
	return lastRC;
	}
	LOG(D1) << "***** command buffer of write data store buffer:";
	IFLOG(D4) cmd->dumpCommand();
	
	// end write
	LOG(D1) << "end transaction";
	cmd->reset();
	cmd->addToken(OPAL_TOKEN::ENDTRANSACTON);
	cmd->addToken(OPAL_TOKEN::WHERE);
	cmd->complete();
	if ((lastRC = session->sendCommand(cmd, response)) != 0) {
	delete cmd;
	delete session;
	return lastRC;
	}

	delete cmd;
	delete session;
	LOG(D1) << "Exiting DtaDevOpal::DataWrite()";
	return 0;
}



uint8_t DtaDevOpal::auditlogwr(char * password, uint32_t startpos, uint32_t len, char * buffer, entry_t * pent, char * userid) // add event ID and write audit log to Data Store
{
        #if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__)
        return 0;
        LOG(D1) << "DtaDevOpal::auditlogwr() isn't supported in Linux";
        #else
	audit_t * ptr;
	entry_t * ptrent;
	vector <entry_t> entryA;
	uint8_t lastRC;
	SYSTEMTIME st, lt;
	uint32_t MAX_ENTRY;

	MAX_ENTRY = 1000; // default size
	if (disk_info.DataStore_maxTableSize < 10485760) {
		MAX_ENTRY = 100;
	}

	LOG(D1) << "***** Entering DtaDevOpal::auditlogwr *****";

	ptr = (audit_t *)buffer;
	LOG(D1) << "copy passing buffer to entryA ";

	ptrent = (entry_t *) ptr->buffer;
	entryA.insert(entryA.begin(), ptrent, ptrent + MAX_ENTRY );
	LOG(D1) << "after memcpy to entryA[0]";
	char zero[8];
	memset(zero, 0, 8);
	if (!memcmp(pent, zero, 8))
	{
		LOG(D1) << "passing empty entry";
		GetSystemTime(&st);
		GetLocalTime(&lt);
		//entry_t ent; passing from caller
		pent->yy = (uint8_t)(lt.wYear - 2000);
		pent->mm = (uint8_t)lt.wMonth;
		pent->dd = (uint8_t)lt.wDay;
		pent->hh = (uint8_t)lt.wHour;
		pent->min = (uint8_t)lt.wMinute;
		pent->sec = (uint8_t)lt.wSecond;
		//pent->event = ID;
	} // if empty entry past, fill up the entry with system time
	// otherwise, past entry already has time stamp and eventID
	entryA.insert(entryA.begin(), *pent); // push_back -> insert
	if (entryA.size() > MAX_ENTRY * 8) {
		LOG(D1) << "Next execute erase()";
		entryA.erase(entryA.begin() + MAX_ENTRY);
	}
	//printf("size of entryA = %zd\n", entryA.size() * 8);
	memcpy(ptr->buffer, &entryA[0], MAX_ENTRY * 8);
 	ptr->header.num_entry = (ptr->header.num_entry + 1) % MAX_ENTRY;
	ptr->header.tail = (ptr->header.tail + 1) % MAX_ENTRY;
	wrtchksum(buffer, genchksum(buffer));
	if (0)
	{
		printf("entryA[0].yy=%d ", entryA[0].yy);
		printf("entryA[0].mm=%d ", entryA[0].mm);
		printf("entryA[0].dd=%d ", entryA[0].dd);
		printf("entryA[0].hh=%d ", entryA[0].hh);
		printf("entryA[0].min=%d ", entryA[0].min);
		printf("entryA[0].sec=%d ", entryA[0].sec);
		printf("entryA[0].event=%d\n", entryA[0].event);
		//printf("entryA[i].reserved=%d\n", entryA[i].reserved);
	}
	if (0) 
	{
		printf("ptr->header.hdr=%s ", ptr->header.hdr);
		printf("ptr->header.ver_major=%d ", ptr->header.ver_major);
		printf("ptr->header.ver_minor=%d ", ptr->header.ver_minor);
		printf("ptr->header.head=%d ", ptr->header.head);
		printf("ptr->header.tail=%d ", ptr->header.tail);
		printf("ptr->header.num_entry=%d\n\n", ptr->header.num_entry);
	}
	//memcpy(ptr->buffer, &entryA[0].yy, auditL.header.num_entry * 8);
	//memcpy(auditL.buffer, &entryA[0].yy, auditL.header.num_entry * 8);
	//uint8_t DtaDevOpal::DataWrite(char * password, uint32_t startpos, uint32_t len,char * buffer)
	// dummy for startpos and len
	len = len + 2;	startpos = startpos + 2;
	len = len - 2;	startpos = startpos - 2 ; // LAME......

	lastRC = DataWrite(password, startpos, len, buffer, userid);
	if (lastRC == 0)
	{
		LOG(D1) << "write data store success";
		//DtaHexDump(&auditL, (int)(entryA.size()) * 8 + sizeof(_audit_hdr));
		IFLOG(D4) DtaHexDump(buffer, (int)ptr->header.num_entry * 8 + sizeof(_audit_hdr));
		return 0;
	}
	else {
		LOG(E) << "write data store Error";
		return lastRC;
	}
	#endif
}

uint8_t DtaDevOpal::auditlogrd(char * password, uint32_t startpos, uint32_t len, char * buffer, char * userid) // read audit log to Data Store
{
        #if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__)
        LOG(D1) << "DtaDevOpal::auditlogrd() isn't supported in Linux";
	return 0;
        #else
	LOG(D1) << "entering DtaDevOpal::auditlogrd";
	//uint8_t DtaDevOpal::DataRead(char * password, uint32_t startpos, uint32_t len, char * buffer)
	uint8_t lastRC;

	lastRC = DataRead(password, startpos, len, buffer, userid);
	if (lastRC == 0)
	{
		audit_t * A = (audit_t *)buffer;
		entry_t * ent;
		char str1[] = SIGNATURE;
		
		if ((lastRC = (uint8_t)memcmp(A->header.signature, str1, strlen(str1))) != 0)
		{
			printf("Invalid Audit Signature or No Audit Entry log\n");
			IFLOG(D4) DtaHexDump(buffer, gethdrsize());
			return ERRCHKSUM;
		}
		if (getchksum(buffer) != genchksum(buffer))
		{
			LOG(D1) << "Audit log header check sum error";
			return ERRCHKSUM; // checksum error
		}
		printf("Fidelity Audit Log Version %d.%d :\n",A->header.ver_major,A->header.ver_minor);
		IFLOG(D4) DtaHexDump(buffer + gethdrsize(), A->header.num_entry * 8);

		ent = (entry_t *)A->buffer;
		printf("Total Number of Audit Entries: %d\n", A->header.num_entry);
		for (int i = 0; i < A->header.num_entry; i++)
		{
			printf("%02d/%02d/%02d %02d:%02d:%02d %3d\n", ((ent->yy) & 0xff)+2000, ent->mm, ent->dd,ent->hh, ent->min, ent->sec, ent->event);
			ent += 1;
		}
		return 0;
	}
	else
	{
		LOG(E) << "read data store Error";
		return lastRC;
	}
	#endif
}

uint16_t genchksum(char * buffer)
{
	audit_hdr * hdr;
	hdr = (audit_hdr *)buffer;
	uint16_t sum ;
	//printf("sizeof(audit_hdr)=0x%Xh; sizeof(hdr->chksum)=0x%Xh \n", (int)sizeof(audit_hdr), (int)sizeof(hdr->chksum));
	sum = 0;
	for (unsigned int i=0; i < (gethdrsize() - sizeof(hdr->chksum)) ;i++ ) 
	{
		sum += buffer[i];
	}
	//printf("generated checksum = 0x%Xh\n",sum);
	return sum;

}

uint16_t getchksum(char * buffer)
{
	audit_hdr * hdr;
	hdr = (audit_hdr *)buffer;
	//printf("header checksum location value = 0x%Xh\n", hdr->chksum);
	return hdr->chksum;
}

void wrtchksum(char * buffer,uint16_t sum)
{
	audit_hdr * hdr;
	hdr = (audit_hdr *)buffer;
	hdr->chksum=sum;
	//printf("write this value(0x%Xh) to header checksum\n", sum);
}


uint16_t gethdrsize() {
	return (sizeof(audit_hdr));
}

// For internal use to record the activities of all password required command

//uint8_t DtaDevOpal::auditRec(char * password, uint8_t id)
uint8_t DtaDevOpal::auditRec(char * password, entry_t * pent, char * userid)
{
	
        #if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__)
        LOG(D1) << "DtaDevOpal::auditRec() isn't supported in Linux";
        return 0;
        #else
	char * buffer;
	uint8_t lastRC;

	uint32_t MAX_ENTRY;

	MAX_ENTRY = 1000; // default size
	if (disk_info.DataStore_maxTableSize < 10485760) {
		MAX_ENTRY = 100;
	}

	//printf("auditlog header size=%d", gethdrsize());
	buffer = (char *)malloc(8 * MAX_ENTRY + gethdrsize());
	//lastRC = auditlogrd(password, 0, (MAX_ENTRY * 8) + gethdrsize(), buffer);

	lastRC = DataRead(password, 0, 8 * MAX_ENTRY + gethdrsize(), buffer,userid);

	if (lastRC != 0)
	{
		LOG(E) << "auditlogrd() returned error"; 
		return lastRC;
	}
	LOG(D4) << "raw data of data store read";
	IFLOG(D4)  DtaHexDump(buffer, gethdrsize());
	audit_t * ptr = (audit_t *)buffer;
	audit_hdr hdrtmp;
	char str1[] = SIGNATURE; // strlen() return 23 char but the array length is 24
	//lastRC = (uint8_t)memcmp(ptr->header.signature, str1, strlen(str1));
	//printf("ptr->header.signature=%s string length = %d\n", ptr->header.signature, (int) strlen(str1));
	//printf("***** lastRC = %d ; ptr->header.num_entry= %d*****\n", lastRC, ptr->header.num_entry);
	if ((lastRC = (uint8_t)memcmp(ptr->header.signature, str1, strlen(str1))) !=0)
	{
		SYSTEMTIME st, lt;
		LOG(D1) << "Invalid Audit signature : lastRC = " << lastRC << " or num_entry is zero : " << hex << ptr->header.num_entry;
		GetSystemTime(&st);
		GetLocalTime(&lt);
		//entry_t ent;
		hdrtmp.date_created.yy = (uint8_t)((lt.wYear-2000) & 0xff);
		hdrtmp.date_created.mm = (uint8_t)lt.wMonth;
		hdrtmp.date_created.dd = (uint8_t)lt.wDay;
		hdrtmp.date_created.hh = (uint8_t)lt.wHour;
		hdrtmp.date_created.min = (uint8_t)lt.wMinute;
		hdrtmp.date_created.sec = (uint8_t)lt.wSecond;
			
		memset(buffer, 0, MAX_ENTRY * 8 + gethdrsize());
		memcpy(buffer, (char *)&hdrtmp, gethdrsize());
		wrtchksum(buffer, genchksum(buffer)); 
		IFLOG(D4) DtaHexDump(buffer, gethdrsize());
	}
	if (getchksum(buffer) != genchksum(buffer))
	{
		LOG(E) << "Read Audit log header check sum error, stop writing audit log";
		//printf("getchksum(buffer) = 0x%Xh ; genchksum(buffer)= 0x%Xh\n", getchksum(buffer), genchksum(buffer));
		//DtaHexDump(buffer, gethdrsize());
		return ERRCHKSUM; // checksum error
	}
	LOG(D1) << "hdrtmp header info :";
	IFLOG(D4) DtaHexDump((char*)&hdrtmp, sizeof(audit_hdr));
	LOG(D1) << "buffer header info : ";
	IFLOG(D4) DtaHexDump(buffer, sizeof(audit_hdr));

	// only for testing purpose
	//srand((uint16_t)time(NULL));
	//id = 1 + rand() % ((uint8_t)evt::evt_lastID);
	printf("***** (uint8_t )evt::evt_lastID=%d event  %d  ***** \n", (uint8_t)evt::evt_lastID, pent->event);
	lastRC = auditlogwr(password, 0, (MAX_ENTRY * 8) + gethdrsize(), buffer, pent, userid); // use rand id for test
	if (lastRC)
	{
		LOG(E) << "audit write error : " << hex << lastRC;
		return lastRC;
	}
	else
	{
		LOG(D1) << "audit write success";
		return 0;
	}
	#endif
}

uint8_t DtaDevOpal::auditErase(char * password, char * userid)
{
        #if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__)
        LOG(D1) << "DtaDevOpal::auditErase() isn't supported in Linux";
	return 0;
        #else
	char * buffer;
	uint8_t lastRC;
	audit_hdr hdr;
	uint32_t MAX_ENTRY;

	MAX_ENTRY = 1000; // default size
	if (disk_info.DataStore_maxTableSize < 10485760) {
		MAX_ENTRY = 100;
	}

	buffer = (char *)malloc(8 * MAX_ENTRY + gethdrsize());
	memset(buffer, 0, (MAX_ENTRY * 8) + gethdrsize());
	/*
	memcpy(buffer, &hdr, gethdrsize());
	wrtchksum(buffer,genchksum(buffer));
	*/
	lastRC = DataWrite(password, 0, (MAX_ENTRY * 8) + gethdrsize(), buffer,userid); 
	return lastRC;
	#endif
}

uint8_t DtaDevOpal::auditRead(char * password, char * userid)
{
        #if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__)
        LOG(D1) << "DtaDevOpal::auditread() isn't supported in Linux";
	return 0;
        #else
	char * buffer;
	uint8_t lastRC;
	uint32_t MAX_ENTRY;

	MAX_ENTRY = 1000; // default size
	if (disk_info.DataStore_maxTableSize < 10485760) {
		MAX_ENTRY = 100;
	}

	buffer = (char *)malloc(8 * MAX_ENTRY + gethdrsize());
	memset(buffer, 0, (MAX_ENTRY * 8) + gethdrsize());
	lastRC = auditlogrd(password, 0, (MAX_ENTRY * 8) + gethdrsize(), buffer,userid);
	return lastRC;
	#endif
}

uint8_t DtaDevOpal::auditWrite(char * password, char * idstr, char * userid)
{
        #if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__)
        LOG(D1) << "DtaDevOpal::auditWrite() isn't supported in Linux";
	return 0;
        #else
	//char * buffer;
	uint8_t lastRC;

	//buffer = (char *)malloc(8 * MAX_ENTRY + gethdrsize());
	//memset(buffer, 0, (MAX_ENTRY * 8) + gethdrsize());
	// proc char * id
	// 

	entry_t ent;
	memset(&ent, 0, sizeof(entry_t));

	char t[2];
	memcpy(t, idstr, 2);
	ent.event = (uint8_t)atoi(t);
	memcpy(t, idstr+2, 2);
	ent.yy = (uint8_t)atoi(t);
	memcpy(t, idstr+4, 2);
	ent.mm = (uint8_t)atoi(t);
	memcpy(t, idstr+6, 2);
	ent.dd = (uint8_t)atoi(t);
	memcpy(t, idstr+8, 2);
	ent.hh = (uint8_t)atoi(t);
	memcpy(t, idstr+10, 2);
	ent.min = (uint8_t)atoi(t);
	memcpy(t, idstr+12, 2);
	ent.sec = (uint8_t)atoi(t);
	ent.reserved = 0;

	lastRC = auditRec(password, &ent, userid);
	return lastRC;
	#endif
}

uint8_t DtaDevOpal::activate(char * password)
{
	uint8_t lastRC;

	if ((lastRC = getDefaultPassword()) != 0) {
		LOG(E) << "Unable to read MSID password ";
		return lastRC;
	}
	char * newpassword = password; 
	if ((lastRC = setSIDPassword((char *)response.getString(4).c_str(), newpassword, 0)) != 0) {
		LOG(E) << "takeOwnership failed";
		return lastRC;
	}

	if ((lastRC = activateLockingSP(password)) != 0) {
		LOG(E) << "Unable to activate LockingSP with default MSID";
		return lastRC;
	}
	return lastRC;
}

uint8_t DtaDevOpal::getmfgstate()
{
	LOG(D1) << "Entering DtaDevOpal::getmfgstate()";
	uint8_t lastRC;
	vector<uint8_t> LR;

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

	LR.clear();
	LR.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		LR.push_back(OPALUID[OPAL_UID::OPAL_ADMINSP_UID][i]); // OPAL_LOCKINGSP_UID or OPAL_ADMINSP_UID? 
	}
	if ((lastRC = getTable(LR, 0x06, 0x07)) != 0) {
		LOG(E) << "Unable to determine LockingSP Lifecycle state";
		delete session;
		return lastRC;
	}

	//uint8_t col_lifecycle_adminSP = response.getUint8(3); // column lifecycle
	uint8_t val_lifecycle_adminSP = response.getUint8(4); // value
	(val_lifecycle_adminSP == 9) ? printf("adminSP life cycle state :  manufactured\n") : printf("adminSP life cycle state :  manufactured-inactive\n");
	//printf("col_lifecycle_adminSP 0x%0Xh val_lifecycle_adminSP 0x%0Xh\n", col_lifecycle_adminSP, val_lifecycle_adminSP);

	//uint8_t col_frozen_adminSP = response.getUint8(7); // column frozen 
	//uint8_t val_frozen_adminSP = response.getUint8(8); // value
	//printf("col_frozen_adminSP 0x%0Xh val_frozen_adminSP 0x%0Xh\n", col_frozen_adminSP, val_frozen_adminSP);

	// for locking SP

	LR.clear();
	LR.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		LR.push_back(OPALUID[OPAL_UID::OPAL_LOCKINGSP_UID][i]); // OPAL_LOCKINGSP_UID or OPAL_ADMINSP_UID? 
	}
	if ((lastRC = getTable(LR, 0x06, 0x07)) != 0) {
		LOG(E) << "Unable to determine LockingSP Lifecycle state";
		delete session;
		return lastRC;
	}

	//uint8_t col_lifecycle_lockingSP = response.getUint8(3); // column lifecycle
	uint8_t val_lifecycle_lockingSP = response.getUint8(4); // value
	(val_lifecycle_lockingSP == 9) ? printf("lockingSP life cycle state :  manufactured\n") : printf("lockingSP life cycle state :  manufactured-inactive\n");
	//printf("col_lifecycle_lockingSP 0x%0Xh val_lifecycle_lockingSP 0x%0Xh\n", col_lifecycle_lockingSP, val_lifecycle_lockingSP);
	//uint8_t col_frozen_lockingSP = response.getUint8(7); // column frozen 
	//uint8_t val_frozen_lockingSP = response.getUint8(8); // value
	//printf("col_frozen_lockingSP 0x%0Xh val_frozen_lockingSP 0x%0Xh\n", col_frozen_lockingSP, val_frozen_lockingSP);

	delete session;




	LOG(D1) << "Exiting DtaDevOpal::getmfgstate()";
	return 0;
}

uint8_t DtaDevOpal::DataStoreWrite(char * password, char * filename, uint8_t dsnum, uint32_t startpos, uint32_t len)
{
        #if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__)
        LOG(D1) << "DtaDevOpal::DataStoreWrite() isn't supported in Linux";
	return 0;
        #else
	LOG(D1) << "Entering DtaDevOpal::DataStoreWrite()";

	ifstream datafile;
	vector <uint8_t> bufferA(57344, 0x00); // (8192, 0x66); // 0 buffer  (57344, 0x00),
	vector <uint8_t> lengthtoken;
	uint8_t lastRC;
	uint64_t fivepercent = 0;
	int complete = 4;
	typedef struct { uint8_t  i : 2; } spinnertik;
	spinnertik spinnertick;
	spinnertick.i = 0;
	char star[] = "*";
	char spinner[] = "|/-\\";
	char progress_bar[] = "   [                     ]";
	uint32_t blockSize = 57344; // 57344=512*112=E000h 1950=0x79E;
	uint32_t filepos = 0;
	uint64_t imgsize;
	uint32_t newSize;

	//printf("dsnum = %d startpos=%ld len=%ld\n", dsnum, startpos, len);

	if (dsnum > disk_info.DataStore_maxTables)
	{
		LOG(E) << "Data Store number must be " << disk_info.DataStore_maxTables << " or less";
		return 1;
	}
	if (len > disk_info.DataStore_maxTableSize)
	{
		LOG(E) << "Data Store Length must be " << disk_info.DataStore_maxTableSize << " or less";
		return 1;
	}
	if (startpos > disk_info.DataStore_maxTableSize)
	{
		LOG(E) << "Data Store Startpos must be " << disk_info.DataStore_maxTableSize << " or less";
		return 1;
	}

	if ((startpos + len) > disk_info.DataStore_maxTableSize)
	{
		LOG(E) << "Data Store write exceed maxTableSize " << disk_info.DataStore_maxTableSize;
		return 1;
	}

	//buffer = (char *)malloc(blockSize);
	datafile.open(filename, ios::in | ios::binary);
	if (!datafile) {
		LOG(E) << "Unable to open Data file " << filename;
		return DTAERROR_OPEN_ERR;
	}
	datafile.seekg(0, datafile.end);
	imgsize = datafile.tellg();
	printf("datafile size %I64Xh\n", imgsize);
	if (imgsize < len)
	{
		len = (uint32_t)imgsize; // Min(imgsize,len)
		LOG(I) << "File truncated to " << len;
	}
	else
	{
		imgsize = len; // cap len to imgsize // 
		LOG(I) << "Write Length truncated " << len;
	}
	if ((startpos + imgsize) > disk_info.DataStore_maxTableSize)
	{
		LOG(E) << "startpos + File size exceed max Data Store Table Size " << disk_info.DataStore_maxTableSize;
		return 1;
	}


	if (imgsize < len)
		fivepercent = (uint64_t)((imgsize / 20) / blockSize) * blockSize;
	else
		fivepercent = (uint64_t)((len / 20) / blockSize) * blockSize;
	if (0 == fivepercent) fivepercent++;
	datafile.seekg(0, datafile.beg);
	if (imgsize > disk_info.DataStore_maxTableSize)
	{
		LOG(E) << "Data file size exceed maximum Data Store Table size";
		datafile.close();
		return 1; // general error 
	}

	LOG(I) << "Writing datafile to " << dev;

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
	LOG(D1) << "start lockingSP session";
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete cmd;
		delete session;
		return lastRC;
	}
	LOG(D1) << "Start transaction";
	cmd->reset();
	cmd->addToken(OPAL_TOKEN::STARTTRANSACTON);
	cmd->addToken(OPAL_TOKEN::WHERE);
	cmd->complete();
	if ((lastRC = session->sendCommand(cmd, response)) != 0) {
		delete cmd;
		delete session;
		return lastRC;
	}
	// 3 boundary
	// (1) eof (2) len (3) data store size
	while (!datafile.eof()  && filepos < (len) ) {
		datafile.read((char *)bufferA.data(), blockSize);
		if (!(filepos % fivepercent)) {
			if (complete <24)
				progress_bar[complete++] = star[0];
			else
				progress_bar[complete] = star[0];
		}
		if (!(filepos % (blockSize * 5))) {
			progress_bar[1] = spinner[spinnertick.i++];
			printf("\r%s %i", progress_bar, filepos);
			fflush(stdout);
		}
		// Max Tables = disk_info.DataStore_maxTables
		// Max Size Tables = " disk_info.DataStore_maxTableSize
		// 
		if (filepos + blockSize + startpos > disk_info.DataStore_maxTableSize)
		{
			newSize = disk_info.DataStore_maxTableSize - filepos - startpos;
			//printf("***** filepos=%ld - startpos = %ld *****\n", filepos, startpos);
			//printf("***** newSize = disk_info.DataStore_maxTableSize - filepos - startpos = %ld ; bufferA.size()=%I64d *****\n", newSize, bufferA.size());
			vector <uint8_t> tmpbuf;
			
			// bufferA.insert(bufferA.begin(), buffer, buffer + len);
			tmpbuf.insert(tmpbuf.begin(), bufferA.begin(), bufferA.begin() + newSize);
			bufferA.erase(bufferA.begin(), bufferA.end());
			bufferA.insert(bufferA.begin(), tmpbuf.begin(), tmpbuf.end());
			//printf("***** newSize = disk_info.DataStore_maxTableSize - filepos - startpos = %ld ; bufferA.size()=%I64d tmpbuf.size()=%I64d *****\n", newSize, bufferA.size(), tmpbuf.size());
		}
		else {
			newSize = blockSize;
		}

		//bufferA.insert(bufferA.begin(), buffer, buffer + len);
		//////////////////////////////////////////////
		LOG(D1) << "bufferA contents after copy passed buffer content";
		IFLOG(D4) DtaHexDump(bufferA.data(), 256);
		//////////////////////////////////////////////
		lengthtoken.clear();
		lengthtoken.push_back(0xe2); // 8k = 8192 (0x2000)
		lengthtoken.push_back(0x00);
		lengthtoken.push_back((newSize >> 8) & 0x000000ff);
		lengthtoken.push_back(newSize & 0x000000ff);

		// cmd buffer before write 
		LOG(D1) << "bufferA before send write data command";
		IFLOG(D4) DtaHexDump(bufferA.data(), 256);

		LOG(D1) << "Writing to data store" << hex << dsnum << " of " << dev;
		//printf(" ***** dsnum = %d *****\n", dsnum);
		switch (dsnum)
		{
		case 0:
		case 1:
			cmd->reset(OPAL_UID::OPAL_DATA_STORE, OPAL_METHOD::SET);
			break;
		case 2:
			cmd->reset(OPAL_UID::OPAL_DATA_STORE2, OPAL_METHOD::SET);
			break;
		case 3:
			cmd->reset(OPAL_UID::OPAL_DATA_STORE3, OPAL_METHOD::SET);
			break;
		case 4:
			cmd->reset(OPAL_UID::OPAL_DATA_STORE4, OPAL_METHOD::SET);
			break;
		case 5:
			cmd->reset(OPAL_UID::OPAL_DATA_STORE5, OPAL_METHOD::SET);
			break;
		case 6:
			cmd->reset(OPAL_UID::OPAL_DATA_STORE6, OPAL_METHOD::SET);
			break;
		case 7:
			cmd->reset(OPAL_UID::OPAL_DATA_STORE7, OPAL_METHOD::SET);
			break;
		case 8:
			cmd->reset(OPAL_UID::OPAL_DATA_STORE8, OPAL_METHOD::SET);
			break;
		case 9:
			cmd->reset(OPAL_UID::OPAL_DATA_STORE9, OPAL_METHOD::SET);
			break;
		}
		cmd->addToken(OPAL_TOKEN::STARTLIST);
		cmd->addToken(OPAL_TOKEN::STARTNAME);
		cmd->addToken(OPAL_TOKEN::WHERE);
		cmd->addToken(filepos+startpos);
		cmd->addToken(OPAL_TOKEN::ENDNAME);
		cmd->addToken(OPAL_TOKEN::STARTNAME);
		cmd->addToken(OPAL_TOKEN::VALUES);
		cmd->addToken(lengthtoken);
		cmd->addToken(bufferA);
		cmd->addToken(OPAL_TOKEN::ENDNAME);
		cmd->addToken(OPAL_TOKEN::ENDLIST);
		cmd->complete();
		if ((lastRC = session->sendCommand(cmd, response)) != 0) {
			delete cmd;
			delete session;
			return lastRC;
		}
		LOG(D1) << "***** command buffer of write data store buffer:";
		IFLOG(D4) cmd->dumpCommand();

		if (lastRC) {
			LOG(E) << "DataStoreWrite Error";
			datafile.close();
			return lastRC;
		}
		filepos += newSize;
	}
	printf("\r%s %i bytes written \n", progress_bar, filepos);

	LOG(D1) << "end transaction";
	cmd->reset();
	cmd->addToken(OPAL_TOKEN::ENDTRANSACTON);
	cmd->addToken(OPAL_TOKEN::WHERE);
	cmd->complete();
	if ((lastRC = session->sendCommand(cmd, response)) != 0) {
		delete cmd;
		delete session;
		return lastRC;
	}
	datafile.close();
	delete cmd;
	delete session;
	LOG(I) << "Data Store file  " << filename << " written to " << dev;
	LOG(D1) << "Exiting DtaDevOpal::DataStoreWrite()";
	return 0;
	#endif

}

uint8_t DtaDevOpal::DataStoreRead(char * password, char * filename, uint8_t dsnum, uint32_t startpos, uint32_t len)
{
        #if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__)
        LOG(D1) << "DtaDevOpal::DataStoreRead() isn't supported in Linux";
	return 0;
        #else
	LOG(D1) << "Entering DtaDevOpal::DataStoreRead()";

	ofstream datafile;
	char * buffer;
	uint8_t lastRC;
	uint64_t fivepercent = 0;
	int complete = 4;
	typedef struct { uint8_t  i : 2; } spinnertik;
	spinnertik spinnertick;
	spinnertick.i = 0;
	char star[] = "*";
	char spinner[] = "|/-\\";
	char progress_bar[] = "   [                     ]";
	uint32_t blockSize = 57344; // 4096;// 57344; // 57344=512*112=E000h 1950=0x79E;
	uint32_t filepos = 0;
	uint32_t newSize;

	//printf("dsnum = %d startpos=%ld len=%ld\n", dsnum, startpos, len);

	if (dsnum > disk_info.DataStore_maxTables)
	{
		LOG(E) << "Data Store number must be " << disk_info.DataStore_maxTables << " or less";
		return 1;
	}
	if (len > disk_info.DataStore_maxTableSize)
	{
		LOG(E) << "Data Store Length must be " << disk_info.DataStore_maxTableSize << " or less";
		return 1;
	}
	if (startpos > disk_info.DataStore_maxTableSize)
	{
		LOG(E) << "Data Store Startpos must be " << disk_info.DataStore_maxTableSize << " or less";
		return 1;
	}

	if ((startpos + len) > disk_info.DataStore_maxTableSize)
	{
		len = disk_info.DataStore_maxTableSize - startpos - len;
		LOG(I) << "Data Store read exceed Data Store max Table Size " << disk_info.DataStore_maxTableSize << " truncated Len to " << len;

	}

	buffer = (char *)malloc(1024*58);
	datafile.open(filename, ios::out | ios::binary);
	if (!datafile) {
		LOG(E) << "Unable to open Data file " << filename;
		return DTAERROR_OPEN_ERR;
	}
	//datafile.seekg(0, datafile.end);
	//imgsize = datafile.tellg();
	//printf("datafile size %I64Xh", imgsize);
	fivepercent = (uint64_t)((disk_info.DataStore_maxTableSize / 20) / blockSize) * blockSize;
	if (0 == fivepercent) fivepercent++;

	vector<uint8_t> LR;

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
	LOG(D1) << "start lockingSP session";
	//if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, getusermode() ? OPAL_UID::OPAL_USER1_UID : OPAL_UID::OPAL_ADMIN1_UID)) != 0) { // JERRY TEST User 1 
		delete cmd;
		delete session;
		LOG(E) << "DataStore Read Error";
		datafile.close();
		free(buffer);
		return lastRC;
	}

	LOG(I) << "Reading Data store from " << dev << " to " << filename;
	while ((filepos + startpos) < disk_info.DataStore_maxTableSize && (filepos < len)) {
		
		if (!(filepos % fivepercent)) {
			progress_bar[complete++] = star[0];
		}
		if (!(filepos % (blockSize * 5))) {
			progress_bar[1] = spinner[spinnertick.i++];
			printf("\r%s %i", progress_bar, filepos);
			fflush(stdout);
		}
		// Max Tables = disk_info.DataStore_maxTables
		// Max Size Tables = " disk_info.DataStore_maxTableSize
		newSize = ((filepos + startpos + blockSize) <= disk_info.DataStore_maxTableSize) ? 
			blockSize : (disk_info.DataStore_maxTableSize - filepos - startpos);
		if (len < newSize)
		{
			newSize = len;
		}
		//printf("newSize=%d filepos=%d\n", newSize,filepos); 
		memset(buffer, 0, blockSize);

		LOG(D1) << "***** start read data store";
		switch (dsnum)
		{
		case 0:
		case 1:
			cmd->reset(OPAL_UID::OPAL_DATA_STORE, OPAL_METHOD::GET);
			break;
		case 2:
			cmd->reset(OPAL_UID::OPAL_DATA_STORE2, OPAL_METHOD::GET);
			break;
		case 3:
			cmd->reset(OPAL_UID::OPAL_DATA_STORE3, OPAL_METHOD::GET);
			break;
		case 4:
			cmd->reset(OPAL_UID::OPAL_DATA_STORE4, OPAL_METHOD::GET);
			break;
		case 5:
			cmd->reset(OPAL_UID::OPAL_DATA_STORE5, OPAL_METHOD::GET);
			break;
		case 6:
			cmd->reset(OPAL_UID::OPAL_DATA_STORE6, OPAL_METHOD::GET);
			break;
		case 7:
			cmd->reset(OPAL_UID::OPAL_DATA_STORE7, OPAL_METHOD::GET);
			break;
		case 8:
			cmd->reset(OPAL_UID::OPAL_DATA_STORE8, OPAL_METHOD::GET);
			break;
		case 9:
			cmd->reset(OPAL_UID::OPAL_DATA_STORE9, OPAL_METHOD::GET);
			break;
		}
		cmd->addToken(OPAL_TOKEN::STARTLIST);
		cmd->addToken(OPAL_TOKEN::STARTLIST);
		cmd->addToken(OPAL_TOKEN::STARTNAME);
		cmd->addToken(OPAL_TOKEN::STARTROW);
		cmd->addToken(filepos+startpos);
		cmd->addToken(OPAL_TOKEN::ENDNAME);
		cmd->addToken(OPAL_TOKEN::STARTNAME);
		cmd->addToken(OPAL_TOKEN::ENDROW);
		cmd->addToken(filepos+startpos+newSize-1);// end row is size -1
		cmd->addToken(OPAL_TOKEN::ENDNAME);
		cmd->addToken(OPAL_TOKEN::ENDLIST);
		cmd->addToken(OPAL_TOKEN::ENDLIST);
		cmd->complete();
		LOG(D1) << "***** send read data store command ";
		if ((lastRC = session->sendCommand(cmd, response)) != 0) {
			delete cmd;
			delete session;
			LOG(E) << "DataStore Read Error";
			datafile.close();
			free(buffer);
			return lastRC;
		}

		// DtaResponse::getBytes(uint32_t tokenNum, uint8_t bytearray[])
		response.getBytes(1, (uint8_t *)buffer); // data is token 1
												 //IFLOG(D4) DtaHexDump(buffer, 8192); // contain header and entries
		LOG(D1) << "raw data after data store read response data";

		IFLOG(D4) DtaHexDump(buffer, gethdrsize()); // contain header and entries
		LOG(D1) << "***** end of send read data store command ";

		if (lastRC) {
			LOG(E) << "DataStore Read Error";
			datafile.close();
			free(buffer);
			delete cmd;
			delete session;
			return lastRC;
		}
		datafile.write(buffer, newSize);
		if (datafile.fail())
		{
			LOG(E) << "Saving datafile error";
			delete cmd;
			delete session;
			datafile.close();
			free(buffer);
			return lastRC;
		}
		filepos += blockSize;
	}
	if (newSize != blockSize)
	{
		printf("\r%s %i bytes read \n", progress_bar, filepos - blockSize + newSize);
	} else {
		printf("\r%s %i bytes read \n", progress_bar, filepos);
	}

	delete cmd;
	delete session;
	datafile.close();
	free(buffer);

	LOG(I) << "Read Data Store from "<< dev << " to " << filename ;
	LOG(D1) << "Exiting DtaDevOpal::DataStoreRead()";
	return 0;
	#endif
}

uint8_t DtaDevOpal::MBRRead(char * password, char * filename, uint32_t startpos, uint32_t len)
{
	LOG(D1) << "Entering DtaDevOpal::MBRRead() with filename"; // D1->I
	ofstream datafile;
	char * buffer;
	uint8_t lastRC;
	uint64_t fivepercent = 0;
	int complete = 4;
	typedef struct { uint8_t  i : 2; } spinnertik;
	spinnertik spinnertick;
	spinnertick.i = 0;
	char star[] = "*";
	char spinner[] = "|/-\\";
	char progress_bar[] = "   [                     ]";
	uint32_t blockSize = 57344; // 4096;// 57344; // 57344=512*112=E000h 1950=0x79E;
	uint32_t filepos = 0;
	uint32_t newSize;
	uint32_t maxMBRSize;

#if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__)
#else
	printf("startpos=%ld len=%ld\n", startpos, len); // linux has error
#endif
	if ((lastRC = getMBRsize(password, &maxMBRSize))!=0)
	{
		LOG(E) << " Can not get MBR table size";
		return lastRC;
	}


	if (len > maxMBRSize)
	{
		LOG(E) << "MBR Length must be " << maxMBRSize << " or less";
		return 1;
	}
	if (startpos > maxMBRSize)
	{
		LOG(E) << "MBR Startpos must be " << maxMBRSize << " or less";
		return 1;
	}

	if ((startpos + len) > maxMBRSize)
	{
		len = maxMBRSize - startpos - len;
		LOG(I) << "MBR read exceed MBR max Table Size " << maxMBRSize << " truncated Len to " << len;

	}

	buffer = (char *)malloc(1024 * 58);
	datafile.open(filename, ios::out | ios::binary);
	if (!datafile) {
		LOG(E) << "Unable to open Data file " << filename;
		return DTAERROR_OPEN_ERR;
	}
	//datafile.seekg(0, datafile.end);
	//imgsize = datafile.tellg();
	//printf("datafile size %I64Xh", imgsize);
	fivepercent = (uint64_t)((maxMBRSize / 20) / blockSize) * blockSize;
	if (0 == fivepercent) fivepercent++;

	vector<uint8_t> LR;

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
	LOG(D1) << "start lockingSP session";
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete cmd;
		delete session;
		LOG(E) << "DataStore Read Error";
		datafile.close();
		free(buffer);
		return lastRC;
	}

	LOG(I) << "Reading MBR from " << dev << " to " << filename;
	while ((filepos + startpos) < maxMBRSize && (filepos < len)) {

		if (!(filepos % fivepercent)) {
			progress_bar[complete++] = star[0];
		}
		if (!(filepos % (blockSize * 5))) {
			progress_bar[1] = spinner[spinnertick.i++];
			printf("\r%s %i", progress_bar, filepos);
			fflush(stdout);
		}
		// Max Tables = disk_info.DataStore_maxTables
		// Max Size Tables = " maxMBRSize
		newSize = ((filepos + startpos + blockSize) <= maxMBRSize) ?
			blockSize : (maxMBRSize - filepos - startpos);
		if (len < newSize)
		{
			newSize = len;
		}
		//printf("newSize=%d filepos=%d\n", newSize, filepos); 
		memset(buffer, 0, blockSize);

		LOG(D1) << "***** start Read MBR";
		cmd->reset(OPAL_UID::OPAL_MBR, OPAL_METHOD::GET);
		cmd->addToken(OPAL_TOKEN::STARTLIST);
		cmd->addToken(OPAL_TOKEN::STARTLIST);
		cmd->addToken(OPAL_TOKEN::STARTNAME);
		cmd->addToken(OPAL_TOKEN::STARTROW);
		cmd->addToken(filepos + startpos);
		cmd->addToken(OPAL_TOKEN::ENDNAME);
		cmd->addToken(OPAL_TOKEN::STARTNAME);
		cmd->addToken(OPAL_TOKEN::ENDROW);
		cmd->addToken(filepos + startpos + newSize - 1);// end row is size -1
		cmd->addToken(OPAL_TOKEN::ENDNAME);
		cmd->addToken(OPAL_TOKEN::ENDLIST);
		cmd->addToken(OPAL_TOKEN::ENDLIST);
		cmd->complete();
		LOG(D1) << "***** send Read MBR command ";
		if ((lastRC = session->sendCommand(cmd, response)) != 0) {
			delete cmd;
			delete session;
			LOG(E) << "DataStore Read Error";
			datafile.close();
			free(buffer);
			return lastRC;
		}

		// DtaResponse::getBytes(uint32_t tokenNum, uint8_t bytearray[])
		response.getBytes(1, (uint8_t *)buffer); // data is token 1
												 //IFLOG(D4) DtaHexDump(buffer, 8192); // contain header and entries
		LOG(D1) << "raw data after MBR read response data";

		IFLOG(D4) DtaHexDump(buffer, gethdrsize()); // contain header and entries
		LOG(D1) << "***** end of send Read MBR command ";

		if (lastRC) {
			LOG(E) << "DataStore Read Error";
			datafile.close();
			free(buffer);
			delete cmd;
			delete session;
			return lastRC;
		}
		datafile.write(buffer, newSize);
		if (datafile.fail())
		{
			LOG(E) << "Saving datafile error";
			delete cmd;
			delete session;
			datafile.close();
			free(buffer);
			return lastRC;
		}
		filepos += blockSize;
	}
	if (newSize != blockSize)
	{
		printf("\r%s %i bytes read \n", progress_bar, filepos - blockSize + newSize);
	}
	else {
		printf("\r%s %i bytes read \n", progress_bar, filepos);
	}

	delete cmd;
	delete session;
	datafile.close();
	free(buffer);

	LOG(I) << "Read MBR from " << dev << " to " << filename;
	LOG(D1) << "Exiting DtaDevOpal::MBRRead()";
	return 0;

}


uint8_t DtaDevOpal::getMBRsize(char * password, uint32_t * msize)
{
	LOG(D1) << "Entering DtaDevOpal::getMBRsize()";

	uint8_t lastRC;
	vector<uint8_t> LR;

	LR.clear();
	LR.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		LR.push_back(OPALUID[OPAL_UID::OPAL_MBR_SZ][i]);
	}

	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	//if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, getusermode() ? OPAL_UID::OPAL_USER1_UID : OPAL_UID::OPAL_ADMIN1_UID)) != 0) { // NG : JERRY 
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete session;
		return lastRC;
	}
	if ((lastRC = getTable(LR, 0x07, 0x07)) != 0) {
		delete session;
		return lastRC;
	}

	uint32_t MBRsz = response.getUint32(4);
	* msize = MBRsz;
	//printf("Shadow MBR size 0x%lX\n", MBRsz);
	cout << "Shadow MBR size 0x" << hex << MBRsz << endl;
	delete session;
	return 0;
}








uint8_t DtaDevOpal::getMBRsize(char * password)
{
	LOG(D1) << "Entering DtaDevOpal::getMBRsize()";

	uint8_t lastRC;
	vector<uint8_t> LR;

	LR.clear();
	LR.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		LR.push_back(OPALUID[OPAL_UID::OPAL_MBR_SZ][i]);
	}

	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	//if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, getusermode() ? OPAL_UID::OPAL_USER1_UID : OPAL_UID::OPAL_ADMIN1_UID)) != 0) { // NG : JERRY 
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete session;
		return lastRC;
	}
	if ((lastRC = getTable(LR, 0x07, 0x07)) != 0) {
		delete session;
		return lastRC;
	}

	uint32_t MBRsz = response.getUint32(4);
	//printf("Shadow MBR size 0x%lX\n", MBRsz);
	cout << "Shadow MBR size 0x" << hex << MBRsz << endl;

	if ((lastRC = getTable(LR, 0x0D, 0x0E)) != 0) {
		delete session;
		return lastRC;
	}
	uint32_t MandatoryWriteGranularity = response.getUint32(4);
	printf("MandatoryWriteGranularity 0x%X\n", MandatoryWriteGranularity);
	uint32_t RecommandedAccessGranularity = response.getUint32(8);
	printf("RecommandedAccessGranularity 0x%X\n", RecommandedAccessGranularity);

	//
	// adminN userN enabled state
	//
	for (uint8_t usr = 0; usr < disk_info.OPAL20_numUsers; usr++)
	{
		LR.clear();
		LR.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
		for (int i = 0; i < 7; i++) {
			LR.push_back(OPALUID[OPAL_UID::OPAL_USER1_UID][i]); // OPAL_USERX_UID or OPAL_ADMINX_UID
		}
		//LR.push_back(OPALUID[OPAL_UID::OPAL_USER1_UID][7])=usr+1;
		LR.push_back(usr+1);
		if ((lastRC = getTable(LR, 0x05, 0x05)) != 0) {
			printf("***** lastRC=%d *****\n", lastRC);
			LOG(E) << "Unable to determine User1 enabled state";
			delete session;
			return lastRC;
		}

		//uint8_t col_lifecycle_adminSP = response.getUint8(3); // column lifecycle
		uint8_t userenabled = response.getUint8(4); // value
		printf("User%d enabled = %d\n", usr+1, userenabled);
		userEnabledTab[usr] = userenabled;
	}

	for (uint8_t usr = 0; usr < disk_info.OPAL20_numAdmins; usr++)
	{
		LR.clear();
		LR.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
		for (int i = 0; i < 7; i++) {
			LR.push_back(OPALUID[OPAL_UID::OPAL_ADMIN1_UID][i]); // OPAL_USERX_UID or OPAL_ADMINX_UID
		}
		//LR.push_back(OPALUID[OPAL_UID::OPAL_USER1_UID][7])=usr+1;
		LR.push_back(usr + 1);
		if ((lastRC = getTable(LR, 0x05, 0x05)) != 0) {
			printf("***** lastRC=%d *****\n", lastRC);
			LOG(E) << "Unable to determine Admin enabled state";
			delete session;
			return lastRC;
		}
		
		uint8_t userenabled = response.getUint8(4); // value
		printf("Admin%d enabled = %d\n", usr + 1, userenabled);
		adminEnabledTab[usr] = userenabled;
	}
	//delete session;

	//
	// adminSP GUDID
	//
	//OPAL_UID uid = OPAL_UID::OPAL_SID_UID;
	//if ((lastRC = session->start(OPAL_UID::OPAL_ADMINSP_UID, password, uid)) != 0) {
	//	delete session;
	//	return lastRC;
	//}
	/* 
	LR.clear();
	LR.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		LR.push_back(OPALUID[OPAL_UID::OPAL_GUDID_UID][i]); // OPAL_GUDID_UID
	}

	if ((lastRC = getTable(LR, 0x02, 0x02)) != 0) {
		printf("***** lastRC=%d *****\n", lastRC);
		LOG(E) << "Unable to get adminSP GUDID";
		delete session;
		return lastRC;
	}

	// DtaResponse::getBytes(uint32_t tokenNum, uint8_t bytearray[])
	uint8_t * gudid = (uint8_t *)malloc(12);
	
	response.getBytes(1, gudid); // data is token 1
	printf("GUDID = ");
	uint8_t * p = gudid;
	for (int i = 0; i < 12; i++)
	{
		printf("%02X", *p & 0x00ff);
		p++;
	}
	printf("\n");
	free(gudid);

	*/

	delete session;
	// 
	//srand((uint16_t)time(NULL));
	//auditRec(password, 1 + rand() % evt_lastID); // for testing purpose
	LOG(D1) << "Exiting DtaDevOpal::getMBRsize()";
	return 0;
}
///////////////////////////////////////////////////////////////////////////
void SignalHandler(int signal)
{
	printf("Signal %d\n", signal);
	throw "!Access Violation!";
}
//////////////////////////////////////////////////////////////////////////////////////////////
//createUSB
uint8_t DtaDevOpal::createUSB(char * filename) {
#if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__)
	LOG(D1) << "DtaDevOpal::createUSB() isn't supported in Linux";
	return 0;
#else
	LOG(D1) << "Entering DtaDevOpal::createUSB()" << filename << " " << dev;
#endif
	LOG(D1) << "Exiting DtaDevOpal::createUSB()";
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//  above code is for createUSB 
//////////////////////////////////////////////////////////////////////////////////////////////


uint8_t DtaDevOpal::loadPBA(char * password, char * filename) {
        #if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__)
        LOG(D1) << "DtaDevOpal::loadPBAimage() isn't supported in Linux";
	return 0;
        #else
	LOG(D1) << "Entering DtaDevOpal::loadPBAimage()" << filename << " " << dev;
	uint8_t embed = 1;
	uint8_t lastRC;
	uint64_t fivepercent = 0;
	uint64_t imgsize;
	int complete = 4;
	typedef struct { uint8_t  i : 2; } spinnertik;
	spinnertik spinnertick;
	spinnertick.i = 0;
	char star[] = "*";
	char spinner[] = "|/-\\";
	char progress_bar[] = "   [                     ]";
	uint32_t blockSize = 57344 ; // 57344=512*112=E000h 1950=0x79E;
	uint32_t filepos = 0;
	uint64_t sz;
	ifstream pbafile;
	// for decompression
	PBYTE DecompressedBuffer = NULL;
	uint64_t DecompressedBufferSize = NULL;
	PBYTE CompressedBuffer = NULL;
	uint64_t CompressedBufferSize = 0;
	DECOMPRESSOR_HANDLE Decompressor = NULL;
	DecompressedBuffer = NULL;
	BOOL Success;
	SIZE_T  DecompressedDataSize;
	void * somebuf;

	vector <uint8_t> buffer; // 0 buffer  (57344, 0x00),
	vector <uint8_t> lengthtoken;

	lengthtoken.clear();
	lengthtoken.push_back(0xe2); // E2 is byte string which mean the followind data is byte-stream, but for read, there is no byte string so it should be E0
	lengthtoken.push_back(0x00);
	lengthtoken.push_back(0xE0);
	lengthtoken.push_back(0x00);
	if (embed == 0) {
		pbafile.open(filename, ios::in | ios::binary);
		if (!pbafile) {
			LOG(E) << "Unable to open PBA image file " << filename;
			return DTAERROR_OPEN_ERR;
		}
		pbafile.seekg(0, pbafile.end);
		fivepercent = (uint64_t)((pbafile.tellg() / 20) / blockSize) * blockSize;
	}
	else {
		const char * fname[] = { "sedutil-cli.exe" }; // , "..\\rc\\sedutil.exe", "..\\rc\\UEFI.img"	};
		#include "sedsize.h" 
		pbafile.open(fname[0], ios::in | ios::binary);
		pbafile.seekg(0, pbafile.end);
		imgsize = pbafile.tellg();
		pbafile.seekg(0, pbafile.beg);
		pbafile.seekg(sedsize);
		//LOG(I) << "read pointer=" << pbafile.tellg();
		int comprss = 1;

		if (comprss) { 
			CompressedBufferSize = imgsize - sedsize;
			CompressedBuffer = (PBYTE)malloc(CompressedBufferSize);
			if (!CompressedBuffer)
			{
				LOG(E) << "Cannot allocate memory for compressed buffer.";
				return DTAERROR_OPEN_ERR;
			}
			pbafile.read((char *)CompressedBuffer, CompressedBufferSize); // read all img data
            
			Success = CreateDecompressor(
				COMPRESS_ALGORITHM_XPRESS_HUFF, //  Compression Algorithm
				NULL,                           //  Optional allocation routine
				&Decompressor);                 //  Handle
			if (!Success)
			{
				LOG(E) << "Cannot create a decompressor: " << GetLastError();
				goto done;
			}
			//  Query decompressed buffer size.
			Success = Decompress(
				Decompressor,                //  Compressor Handle
				CompressedBuffer,            //  Compressed data
				CompressedBufferSize,        //  Compressed data size
				NULL,                        //  Buffer set to NULL
				0,                           //  Buffer size set to 0
				&DecompressedBufferSize);    //  Decompressed Data size
											 // Allocate memory for decompressed buffer.
			if (!Success)
			{
				DWORD ErrorCode = GetLastError();
				// Note that the original size returned by the function is extracted 
				// from the buffer itself and should be treated as untrusted and tested
				// against reasonable limits.
				if (ErrorCode != ERROR_INSUFFICIENT_BUFFER)
				{
					LOG(E) << "Cannot query decompress data: " << ErrorCode ;
					//printf("DecompressedBufferSize=%I64d \n", DecompressedBufferSize);
					goto done;
				}
				DecompressedBuffer = (PBYTE)malloc(DecompressedBufferSize);
				if (!DecompressedBuffer)
				{
					LOG(E) << "Cannot allocate memory for decompressed buffer";
					goto done;
				}
				somebuf = malloc(DecompressedBufferSize);
				if (!somebuf) {
					LOG(E) << "Cannot allocate memory for somebuf buffer";
					goto done;
				} 
				memset(DecompressedBuffer, 0, DecompressedBufferSize);
			}
			//  Decompress data 
			Success = Decompress(
				Decompressor,               //  Decompressor handle
				CompressedBuffer,           //  Compressed data
				CompressedBufferSize,       //  Compressed data size
				DecompressedBuffer,         //  Decompressed buffer
				DecompressedBufferSize,     //  Decompressed buffer size
				&DecompressedDataSize);     //  Decompressed data size
			if (!Success)
			{
				LOG(E) << "Cannot really decompress data: " << GetLastError();
				//LOG(I) << ("DecompressedBufferSize=%I64d DecompressedDataSize=%I64d\n", DecompressedBufferSize, DecompressedDataSize);
				goto done;
			}
				
			/*
			printf("CompressedBuffer size: %I64d; DecompressedBufferSize:%I64d; DecompressedDataSize: %I64d\n",
				CompressedBufferSize,
				DecompressedBufferSize,
				DecompressedDataSize);
			printf("File decompressed.\n");
			*/
		done:
			// house keeping buffer and file handler
			if (Decompressor != NULL)
			{
				LOG(D1) << "free Decompressor" << endl;
				CloseDecompressor(Decompressor);
			}
			if (CompressedBuffer)
			{
				LOG(D1) << "free CompressBuffer" << endl;
				free(CompressedBuffer);
			}
		} // end cmprss
		fivepercent = (uint64_t)((DecompressedBufferSize / 20) / blockSize) * blockSize;
	}
	// embedded info to MBR
	bool saved_flag = no_hash_passwords;
	no_hash_passwords = false;
	char * model = getModelNum();
	char * firmware = getFirmwareRev();
	char * sernum = getSerialNum();
	vector<uint8_t> hash;
	IFLOG(D1) printf("model : %s ", model);
	IFLOG(D1) printf("firmware : %s ", firmware);
	IFLOG(D1) printf("serial : %s\n", sernum);
	hash.clear();
	LOG(D1) << "start hashing";
	DtaHashPwd(hash, sernum, this);
	LOG(D1) << "end hashing";
	IFLOG(D1) printf("hashed size = %zd\n", hash.size());
	IFLOG(D1) printf("hashed serial number is ");
	IFLOG(D1)
	for (int i = 0; i < hash.size(); i++)
	{
		printf("%02X", hash.at(i));
	}
	printf("\n");
	// try dump decompressed buffer of sector 0 , 1 
	//DtaHexDump(DecompressedBuffer + 512, 512);
	// write 32-byte date into buffer 
	for (int i = 2; i < hash.size(); i++)
	{
		DecompressedBuffer[512 + 64 + i - 2] = hash.at(i);
	}

	hash.clear();
	LOG(D1) << "start hashing";
	char mbrstr[16] = "FidelityLockMBR";
	IFLOG(D4) DtaHashPwd(hash, mbrstr, this);
	for (int i = 2; i < hash.size(); i++)
	{
		DecompressedBuffer[512 + 96 + i - 2] = hash.at(i);
	}

	IFLOG(D4) DtaHexDump(DecompressedBuffer + 512, 512);
	no_hash_passwords = saved_flag;

	if (0 == fivepercent) fivepercent++;
	if (embed ==0) 
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
	LOG(D1) << "Start transaction";
	cmd->reset();
	cmd->addToken(OPAL_TOKEN::STARTTRANSACTON);
	cmd->addToken(OPAL_TOKEN::WHERE);
	cmd->complete();
	if ((lastRC = session->sendCommand(cmd, response)) != 0) {
		delete cmd;
		delete session;
		pbafile.close();
		return lastRC;
	}
	LOG(I) << "Writing PBA to " << dev;
	
	while (!pbafile.eof() || (filepos <= DecompressedBufferSize)) {
		if (embed == 0) {
			pbafile.read((char *)buffer.data(), blockSize);
		}
		else {
			
				signal(SIGSEGV, SignalHandler);
				try { 
					//memcpy(&buffer[0], &(DecompressedBuffer[filepos]), ((filepos + blockSize) <= DecompressedBufferSize) ? blockSize: DecompressedBufferSize - filepos);
					buffer.erase(buffer.begin(), buffer.end());
					sz = (((filepos + blockSize) <= DecompressedBufferSize) ? blockSize : DecompressedBufferSize - filepos);
					
					if (sz < blockSize) {
						somebuf = (PBYTE)malloc(blockSize);
						if (!somebuf)
						{
							LOG(E) << "Cannot allocate memory for last block buffer.";
							return DTAERROR_OPEN_ERR;
						}
						memset(somebuf, 0, blockSize);
						memcpy(somebuf, DecompressedBuffer + filepos,sz);
						buffer.insert(buffer.begin(), (PBYTE)somebuf, (PBYTE)somebuf + blockSize );
						free(somebuf);
					}
					else
					{
						buffer.insert(buffer.begin(), DecompressedBuffer + filepos, DecompressedBuffer + filepos + sz);
						//LOG(I) << "buffer.size()=" << buffer.size(); 
					}
					//printf("DecompressedBuffer + filepos=%I64d; DecompressedBuffer + filepos + sz=%I64d; filepos=%ld; sz=%I64d\n", (uint64_t)DecompressedBuffer + filepos, (uint64_t)DecompressedBuffer + filepos + sz, filepos, sz);
				}
				catch (char *e) {
					LOG(E) << "Exception Caught: " << e;
					break;
				}
		}

		if (!(filepos % fivepercent))
			progress_bar[complete++] = star[0];

		if (!(filepos % (blockSize * 5))) {
			progress_bar[1] = spinner[spinnertick.i++];
			printf("\r%s %i(%I64d)", progress_bar,filepos, DecompressedBufferSize);
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
		filepos += blockSize;
		if (filepos > DecompressedBufferSize)
		{
			break;
		}
	} // end of while 

	printf("\r%s %i(%I64d) bytes written \n", progress_bar, filepos, DecompressedBufferSize);
	LOG(D1) << "end transaction";
	cmd->reset();
	cmd->addToken(OPAL_TOKEN::ENDTRANSACTON);
	cmd->addToken(OPAL_TOKEN::WHERE);
	cmd->complete(); 
	if ((lastRC = session->sendCommand(cmd, response)) != 0) {
		delete cmd;
		delete session;
		pbafile.close();
		return lastRC;
	}
	delete cmd;
	delete session;
	pbafile.close();
	if (embed ==0)
		LOG(I) << "PBA image  " << filename << " written to " << dev;
	else {
		LOG(I) << "PBA image written to " << dev;

	}
        #endif
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
	delete session;
	//auditRec(newpassword, evt_PasswordChangedSID);
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
	LOG(D1) << "Entering DtaDevOpal::exec";
    LOG(D3) << endl << "Dumping command buffer";
    IFLOG(D3) DtaHexDump(cmd->getCmdBuffer(), SWAP32(hdr->cp.length) + sizeof (OPALComPacket));
	LOG(D1) << "Entering DtaDevOpal::exec sendCmd(IF_SEND, IO_BUFFER_LENGTH)";
    if((lastRC = sendCmd(IF_SEND, protocol, comID(), cmd->getCmdBuffer(), IO_BUFFER_LENGTH)) != 0) {
		LOG(E) << "Command failed on send " << (uint16_t) lastRC;
        return lastRC;
    }
    hdr = (OPALHeader *) cmd->getRespBuffer();
    do {
        osmsSleep(25);
        memset(cmd->getRespBuffer(), 0, IO_BUFFER_LENGTH);
		LOG(D1) << "Entering DtaDevOpal::exec sendCmd(IF_RECV, IO_BUFFER_LENGTH)";
        lastRC = sendCmd(IF_RECV, protocol, comID(), cmd->getRespBuffer(), IO_BUFFER_LENGTH);

    }
    while ((0 != hdr->cp.outstandingData) && (0 == hdr->cp.minTransfer));
    LOG(D3) << std::endl << "Dumping reply buffer";
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
	props->addToken(61440);
	props->addToken(OPAL_TOKEN::ENDNAME);
	props->addToken(OPAL_TOKEN::STARTNAME);
	props->addToken("MaxResponseComPacketSize");
	props->addToken(61440);
	props->addToken(OPAL_TOKEN::ENDNAME);
	props->addToken(OPAL_TOKEN::STARTNAME);
	props->addToken("MaxPacketSize");
	props->addToken(61420);
	props->addToken(OPAL_TOKEN::ENDNAME);
	props->addToken(OPAL_TOKEN::STARTNAME);
	props->addToken("MaxIndTokenSize");
	props->addToken(61384);
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
