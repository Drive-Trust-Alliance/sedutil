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

#pragma warning(disable: 4224) //C2224: conversion from int to char , possible loss of data
#pragma warning(disable: 4244) //C4244: 'argument' : conversion from 'uint16_t' to 'uint8_t', possible loss of data
#pragma warning(disable: 4996)
#pragma comment(lib, "rpcrt4.lib")  // UuidCreate - Minimum supported OS Win 2000

#include "os.h"
#if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__)
#else
#include <Windows.h>
#include "compressapi-8.1.h"
#include "sedsize.h" 
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
//#include "ob.h"
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include "uuid.h"
#endif


void setlic(char * lic_level, const char * LicenseLevel);
void auditpass(char * apass);

using namespace std;

DtaDevOpal::DtaDevOpal()
{
}

DtaDevOpal::~DtaDevOpal()
{
	#if 0
	LOG(I) << "Exit DtaDevOpal()";
	if (adj_host == 1)
	{
		LOG(I) << "adj_host == 1, readjust host propeties back to smaller packet size";
		adj_host = 0;
		properties();
	}
	else {
		LOG(I) << "adj_host == 0, no need readjust host propeties back to smaller packet size";
	}
	LOG(I) << "Exit DtaDevOpal()";
	#endif
}
void DtaDevOpal::init(const char * devref)
{
	uint8_t lastRC;
	DtaDevOS::init(devref);
	adj_host = 0; 
	if((lastRC = properties()) != 0) { LOG(E) << "Properties exchange failed " << dev;}
}

// create an audit user UserN disk_info.OPAL20_numUsers
//char * DtaDevOpal::gethuser(void);


void DtaDevOpal::gethuser(char * buf)
{	string hUser = "User" + to_string(disk_info.OPAL20_numUsers);
	for (int ii = 0; ii < (int)hUser.size(); ii++)
		buf[ii] = hUser.at(ii);
}

 uint8_t DtaDevOpal::setTperResetEnable(bool enable,char * password)
{
		LOG(I) << "Entering DtaDevOpal::setTPerResetEnable()";
		uint8_t lastRC;
		/*
		vector<uint8_t> LR;
		LR.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
		for (int i = 0; i < 8; i++) {
			LR.push_back(OPALUID[OPAL_UID::OPAL_ACE_TperInfo_Set_ProgrammaticResetEnable][i]);
		}
		*/
		session = new DtaSession(this);
		if (NULL == session) {
			LOG(E) << "Unable to create session object " << dev;
			return DTAERROR_OBJECT_CREATE_FAILED;
		}// OPAL_ADMINSP_UID
		//if ((lastRC = session->start(OPAL_UID::OPAL_ADMINSP_UID, password, OPAL_UID::OPAL_SID_UID)) != 0) {
		if ((lastRC = session->start(OPAL_UID::OPAL_ADMINSP_UID, password, OPAL_UID::OPAL_SID_UID)) != 0) {
			delete session;
			return lastRC;
		}
		//if ((lastRC = getTable(LR, OPAL_TOKEN::ACTIVEKEY, OPAL_TOKEN::ACTIVEKEY)) != 0) {
		//	delete session;
		//	return lastRC;
		//}
		DtaCommand *rekey = new DtaCommand();
		if (NULL == rekey) {
			LOG(E) << "Unable to create command object " << dev;
			delete session;
			return DTAERROR_OBJECT_CREATE_FAILED;
		}
		rekey->reset(OPAL_UID::OPAL_ACE_TperInfo_Set_ProgrammaticResetEnable, OPAL_METHOD::SET);
		//rekey->changeInvokingUid(LR);
		rekey->addToken(OPAL_TOKEN::STARTLIST);
		rekey->addToken(OPAL_TOKEN::STARTNAME);
		rekey->addToken(OPAL_TOKEN::VALUES);

		rekey->addToken(OPAL_TOKEN::STARTLIST);
		rekey->addToken(OPAL_TOKEN::STARTNAME);
		rekey->addToken(OPAL_TOKEN::TPERRESETENABLECOLUMN);

		rekey->addToken(OPAL_TOKEN::STARTLIST);
		rekey->addToken(OPAL_TOKEN::WHERE);
		if (enable) 
			rekey->addToken(OPAL_TOKEN::TPERRESETENABLEVALUE);
		else
			rekey->addToken(OPAL_TOKEN::WHERE); // Do I need WHERE ?
		rekey->addToken(OPAL_TOKEN::ENDLIST);

		rekey->addToken(OPAL_TOKEN::ENDNAME);
		rekey->addToken(OPAL_TOKEN::ENDLIST);
		rekey->addToken(OPAL_TOKEN::ENDNAME);
		rekey->addToken(OPAL_TOKEN::ENDLIST);
		rekey->complete();
		rekey->dumpCommand(); // JERRY 

		if ((lastRC = session->sendCommand(rekey, response)) != 0) {
			LOG(E) << "setTPerResetEnable  " << (enable ? "ON" : "OFF") << " Failed " << dev;
			delete rekey;
			delete session;
			return lastRC;
		}
		delete rekey;
		delete session;
		
		LOG(D1) << "Exiting DtaDevOpal::setTPerResetEnable() " << dev;
		return 0;
}
 uint8_t DtaDevOpal::setLockonReset(uint8_t lockingrange, bool enable,char * password)
{

		LOG(D1) << "Entering DtaDevOpal::setLockonReset()";
		uint8_t lastRC;
		vector<uint8_t> LR;
		LR.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
		for (int i = 0; i < 8; i++) {
			LR.push_back(OPALUID[OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL][i]);
		}
		if (lockingrange != 0) {
			LOG(D2) << "lockingrange = " << lockingrange;
			LR[6] = 0x03;
			LR[8] = lockingrange;
		}
		session = new DtaSession(this);
		if (NULL == session) {
			LOG(E) << "Unable to create session object " << dev;
			return DTAERROR_OBJECT_CREATE_FAILED;
		}
		if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
			delete session;
			return lastRC;
		}
		//if ((lastRC = getTable(LR, OPAL_TOKEN::ACTIVEKEY, OPAL_TOKEN::ACTIVEKEY)) != 0) {
		//	delete session;
		//	return lastRC;
		//}
		DtaCommand *rekey = new DtaCommand();
		if (NULL == rekey) {
			LOG(E) << "Unable to create command object " << dev;
			delete session;
			return DTAERROR_OBJECT_CREATE_FAILED;
		}
		rekey->reset(OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL, OPAL_METHOD::SET);
		rekey->changeInvokingUid(LR);
		rekey->addToken(OPAL_TOKEN::STARTLIST);
		rekey->addToken(OPAL_TOKEN::STARTNAME);
		rekey->addToken(OPAL_TOKEN::VALUES);

		rekey->addToken(OPAL_TOKEN::STARTLIST);
		rekey->addToken(OPAL_TOKEN::STARTNAME);
		rekey->addToken(OPAL_TOKEN::LOCKONRESETCOLUMN);

		rekey->addToken(OPAL_TOKEN::STARTLIST);
		rekey->addToken(OPAL_TOKEN::WHERE);
		if (enable) 
			rekey->addToken(OPAL_TOKEN::LOCKONRESETVALUE); 
		rekey->addToken(OPAL_TOKEN::ENDLIST);

		rekey->addToken(OPAL_TOKEN::ENDNAME);
		rekey->addToken(OPAL_TOKEN::ENDLIST);
		rekey->addToken(OPAL_TOKEN::ENDNAME);
		rekey->addToken(OPAL_TOKEN::ENDLIST);
		rekey->complete();
		if ((lastRC = session->sendCommand(rekey, response)) != 0) {
			LOG(E) << "LockOnReset LockingRange " << lockingrange << " Failed " << dev;
			delete rekey;
			delete session;
			return lastRC;
		}
		delete rekey;
		delete session;
		LOG(I) << "LockOnReset LockingRange : " << (uint16_t)lockingrange << " *** " << dev;
		LOG(D1) << "Exiting DtaDevOpal::SetLockonReset() " << dev;
		return 0;
}
uint8_t DtaDevOpal::initialSetup(char * password)
{
	LOG(D1) << "Entering initialSetup() " << dev;
	uint8_t lastRC;

	if (!skip_activate) {
		LOG(I) << "skip activateLockingSP"; 
		if ((lastRC = takeOwnership(password)) != 0) {
			LOG(E) << "Initial setup failed - unable to take ownership " << dev;
			return lastRC;
		}

		if ((lastRC = activateLockingSP(password)) != 0) {
			LOG(E) << "Initial setup failed - unable to activate LockingSP " << dev;
			return lastRC;
		}
	}
	if ((lastRC = configureLockingRange(0, DTA_DISABLELOCKING, password)) != 0) {
		LOG(E) << "Initial setup failed - unable to configure global locking range " << dev;
		return lastRC;
	}
	if ((lastRC = setLockingRange(0, OPAL_LOCKINGSTATE::READWRITE, password)) != 0) {
		LOG(E) << "Initial setup failed - unable to set global locking range RW " << dev;
		return lastRC;
	}
	if ((lastRC = setMBRDone(1, password)) != 0){
		LOG(E) << "Initial setup failed - unable to Enable MBR shadow " << dev;
		return lastRC;
	}
	if ((lastRC = setMBREnable(1, password)) != 0){
		LOG(E) << "Initial setup failed - unable to Enable MBR shadow " << dev;
		return lastRC;
	}
	if ((lastRC = setLockonReset(0, TRUE, password)) != 0) { // enable LOCKING RANGE 0 LOCKonRESET 
		LOG(E) << "Initial setup failed - unable to set LOCKONRESET " << dev;
		return lastRC;
	}
	LOG(D1) << "Initial setup of TPer complete on " << dev;
	LOG(I) << "setuphuser() " << dev;
	if ((lastRC = setuphuser(password)) != 0) {
		LOG(E) << "setup audit user failed " << dev;
		return lastRC;
	}
	LOG(I) << "setup normal user" << dev;
	/*
		enableUser(true, password, buf); // true : enable user; false: disable user
	enableUserRead(true, password, buf);
	
	*/
	char buf[5] = { 'U','s','e','r','1' };

	if ((lastRC = enableUser(true, password, buf)) != 0) {
		LOG(E) << "enable user failed " << dev;
		return lastRC;
	}
	if ((lastRC = enableUserRead(true, password, buf)) != 0) {
		LOG(E) << "enable user read failed " << dev;
		return lastRC;
	}
        char strname[20];
	memset (strname,0,20);
	strncpy(strname, "USER1", 255);
	if ((lastRC = setPassword(password, buf, strname)) != 0) { // set User1 password as USER1 default
		LOG(E) << "set user password failed " << dev;
		return lastRC;
	}

	LOG(D1) << "Exiting initialSetup() " << dev;
	return 0;
}

uint8_t DtaDevOpal::setuphuser(char * password)
{
	LOG(I) << "Entering setuphuser() " << dev;
	uint8_t lastRC;
	char * buf = (char *)malloc(20);
	memset(buf, 0, 20);
	gethuser(buf);
	if (enableUser(true, password, buf)) { LOG(E) << "enable audit User fail"; }; // true : enable user; false: disable user
	if (enableUserRead(true, password, buf)) { LOG(E) << "enable User access fail"; };
	//char p1[64] = "F0iD2eli81Ty"; //20->12 "pFa0isDs2ewloir81Tdy";
	char p1[80]; // = { 'F','0','i','D','2','e','l','i','8','1','T','y',NULL };
	memset(p1, 0, 80); // zero out pass
	auditpass(p1);

	#if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__)
	strcat(p1, getSerialNum());
	#else
	strcat_s(p1, getSerialNum());
	#endif
	//LOG(I) << p1;  // JERRY 
	//DtaHexDump(p1, 80); // JERRY 
	// setpassword has flag -n -t from GUI
	if (no_hash_passwords) { // do it only when -n is set
		bool saved_flag = no_hash_passwords;
		no_hash_passwords = false; // want to hash audit user password
		bool saved_t_flag = translate_req;
		translate_req = false; // do not want to do translate user password
		vector <uint8_t> hash;
		DtaHashPwd(hash, (char *)p1, this);
		memset(p1, 0, 80); 
		for (int ii = 0; ii < (int)(hash.size() -2); ii += 1) { // first 2 byte of hash vector is header
			//p1[ii] = hash.at(ii+2); // p1 is binary data of hashed password
			//itoa(hash.at(ii + 2) >> 4,  p1 + (ii*2),   16);
			//itoa(hash.at(ii + 2) & 0xf, p1 + (ii*2) + 1 , 16);
			snprintf(p1 + (ii * 2), 4, "%02x", hash.at(ii + 2)); // itoa is not standard lib and linux doesn't like it
		}
		if (0) { // should never reveal the hashed password
			LOG(I) << "setuphuser() : after hash p1, User9 new hashed password = ";
			for (int ii = 0; ii < (int)(hash.size() - 2); ii += 1) { // first 2 byte of hash vector is header
				printf("%02X", hash[ii+2]);
			}
			printf("  %s \n",  dev);
		} 
		//LOG(I) << "setuphuser() : new hash size = " << (uint16_t)hash.size(); 
		//DtaHexDump(p1, 80);
		translate_req = saved_t_flag;
		no_hash_passwords = saved_flag ;
	}
	// 
	if ((lastRC = setPassword(password, buf, (char *)p1) != 0))
	{
		LOG(E) << "setup h user failed " << dev;
		return lastRC;
	}
	//translate_req = saved_t_flag;
	//no_hash_passwords = saved_flag;
	LOG(I) << "Exiting setuphuser() " << dev;
	return 0;

}


uint8_t DtaDevOpal::setup_SUM(uint8_t lockingrange, uint64_t start, uint64_t length, char *Admin1Password, char * password)
{
	LOG(D1) << "Entering setup_SUM() " << dev;;
	uint8_t lastRC;
	char defaultPW[] = ""; //OPAL defines the default initial User password as 0x00
	std::string userId;
	userId.append("User");
	userId.append(std::to_string(lockingrange + 1)); //OPAL defines LR0 to User1, LR1 to User2, etc.

	//verify opal SUM support and status
	if (!disk_info.Locking || !disk_info.SingleUser)
	{
		LOG(E) << "Setup_SUM failed - this drive does not support LockingSP / SUM " << dev;
		return DTAERROR_INVALID_COMMAND;
	}
	if (disk_info.Locking_lockingEnabled && !disk_info.SingleUser_any)
	{
		LOG(E) << "Setup_SUM failed - LockingSP has already been configured in standard mode. " << dev;  
		return DTAERROR_INVALID_COMMAND;
	}
	//If locking not enabled, run initial setup flow
	if (!disk_info.Locking_lockingEnabled)
	{
		LOG(D1) << "LockingSP not enabled. Beginning initial setup flow. " << dev;
		if ((lastRC = takeOwnership(Admin1Password)) != 0) {
			LOG(E) << "Setup_SUM failed - unable to take ownership " << dev;
			return lastRC;
		}
		if ((lastRC = activateLockingSP_SUM(lockingrange, Admin1Password)) != 0) {
			LOG(E) << "Setup_SUM failed - unable to activate LockingSP in SUM " << dev;
			return lastRC;
		}
		if ((lastRC = setupLockingRange_SUM(lockingrange, start, length, defaultPW)) != 0) {
			LOG(E) << "Setup_SUM failed - unable to setup locking range " << lockingrange << "(" << start << "," << length << ") " << dev;
			return lastRC;
		}
	}
	if ((lastRC = eraseLockingRange_SUM(lockingrange, Admin1Password)) != 0) {
		LOG(E) << "Setup_SUM failed - unable to erase locking range " << dev;
		return lastRC;
	}

	//verify that locking range covers correct LBAs
	lrStatus_t lrStatus;
	if ((lrStatus = getLockingRange_status(lockingrange, Admin1Password)).command_status != 0) {
		LOG(E) << "Setup_SUM failed - unable to query locking range start/size " << dev;
		return lrStatus.command_status;
	}
	if (start != lrStatus.start || length != lrStatus.size)
	{
		LOG(D1) << "Incorrect Locking Range " << lockingrange << " start/size. Attempting to correct..." << dev;
		if ((lastRC = setupLockingRange_SUM(lockingrange, start, length, defaultPW)) != 0) {
			LOG(E) << "Setup_SUM failed - unable to setup locking range " << lockingrange << "(" << start << "," << length << ") " << dev;
			return lastRC;
		}
		LOG(D1) << "Locking Range " << lockingrange << " start/size corrected. " << dev;
	}

	//enable and set new password for locking range
	if ((lastRC = setLockingRange_SUM(lockingrange, OPAL_LOCKINGSTATE::READWRITE, defaultPW)) != 0) {
		LOG(E) << "Setup_SUM failed - unable to enable locking range " << dev;
		return lastRC;
	}
	if ((lastRC = setNewPassword_SUM(defaultPW, (char *)userId.c_str(), password)) != 0) {
		LOG(E) << "Setup_SUM failed - unable to set new locking range password " << dev;
		return lastRC;
	}

	LOG(I) << "Setup of SUM complete on " << dev;
	LOG(D1) << "Exiting setup_SUM() " << dev;
	return 0;
}
DtaDevOpal::lrStatus_t DtaDevOpal::getLockingRange_status(uint8_t lockingrange, char * password)
{
	uint8_t lastRC;
	lrStatus_t lrStatus;
	LOG(D1) << "Entering DtaDevOpal:getLockingRange_status() " << dev;
	vector<uint8_t> LR;
	LR.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		LR.push_back(OPALUID[OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL][i]);
	}

	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object " << dev;
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
		LOG(E) << "locking range getTable command did not return enough data " << dev;
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
	LOG(D1) << "Exiting DtaDevOpal:getLockingRange_status() " << dev;
	return lrStatus;
}
uint8_t DtaDevOpal::listLockingRanges(char * password, int16_t rangeid)
{
	uint8_t lastRC;
	LOG(D1) << "Entering DtaDevOpal:listLockingRanges() " << rangeid << " " << dev;
	vector<uint8_t> LR;
	LR.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		LR.push_back(OPALUID[OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL][i]);
	}
	
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object " << dev;
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
	//if (0) {
	//	printf("***** getTokenCount()=%ld\n", (long)tc);
	//	for (uint32_t i = 0; i < tc; i++) {
	//		printf("token %ld = ", (long)i);
	//		for (uint32_t j = 0; j < response.getRawToken(i).size(); j++)
	//			printf("%02X ", response.getRawToken(i)[j]);
	//		cout << endl;
	//	}
	//}

	// JERRY
	if (response.tokenIs(4) != _OPAL_TOKEN::DTA_TOKENID_UINT) {
		LOG(E) << "Unable to determine number of ranges " << dev;
		delete session;
		return DTAERROR_NO_LOCKING_INFO;
	}
	LOG(I) << "Locking Range Configuration for " << dev;
	uint32_t numRanges = response.getUint32(4) + 1;
	for (uint32_t i = 0; i < numRanges; i++){
		if(0 != i) LR[8] = i & 0xff;
		// JERRY 
		//if (0) {
		//	for (uint8_t k = 0; k < LR.size(); k++) printf("%02X ", LR[k]);
		//	cout << endl;
		//}
		// JERRY
		if ((lastRC = getTable(LR, _OPAL_TOKEN::RANGESTART, _OPAL_TOKEN::WRITELOCKED)) != 0) {
			delete session;
			return lastRC;
		}
		// JERRY dump raw token info 
		tc = response.getTokenCount();
		//if (0) {
		//	printf("***** getTokenCount()=%ld\n", (long)tc);
		//	for (uint32_t i = 0; i < tc; i++) {
		//		printf("token %ld = ", (long)i);
		//		for (uint32_t j = 0; j < response.getRawToken(i).size(); j++)
		//			printf("%02X ", response.getRawToken(i)[j]);
		//		cout << endl;
		//	}
		//}
		if (tc != 34) { // why ?????
			cout << endl;
			LOG(E) << "token count is wrong. Exit loop " << dev;
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
	LOG(D1) << "Exiting DtaDevOpal:listLockingRanges() " << dev;
	return 0;
}
uint8_t DtaDevOpal::setupLockingRange(uint8_t lockingrange, uint64_t start,
	uint64_t length, char * password)
{
	uint8_t lastRC;
	LOG(D1) << "Entering DtaDevOpal:setupLockingRange()" << dev;
	if (lockingrange < 1) {
		LOG(E) << "global locking range cannot be changed " << dev;
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
		LOG(E) << "Unable to create session object " << dev;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	//if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, getusermode() ? OPAL_UID::OPAL_USER1_UID : OPAL_UID::OPAL_ADMIN1_UID)) != 0) { // NG : JERRY 
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete session;
		return lastRC;
	}
	DtaCommand *set = new DtaCommand();
	if (NULL == set) {
		LOG(E) << "Unable to create command object " << dev;
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
		LOG(E) << "setupLockingRange Failed " << dev;
		delete set;
		delete session;
		return lastRC;
	}
	delete set;
	delete session;
	if ((lastRC = rekeyLockingRange(lockingrange, password)) != 0) {
		LOG(E) << "setupLockingRange Unable to reKey Locking range -- Possible security issue " << dev;
		return lastRC;
	}
	LOG(I) << "LockingRange" << (uint16_t)lockingrange << " starting block " << start <<
		" for " << length << " blocks configured as unlocked range";
	
	LOG(D1) << "Exiting DtaDevOpal:setupLockingRange() " << dev;
	return 0;
}
uint8_t DtaDevOpal::setupLockingRange_SUM(uint8_t lockingrange, uint64_t start,
	uint64_t length, char * password)
{
	uint8_t lastRC;
	LOG(D1) << "Entering DtaDevOpal:setupLockingRange_SUM() " << dev;
	if (lockingrange < 1) {
		LOG(E) << "global locking range cannot be changed " << dev;
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
		LOG(E) << "Unable to create session object " << dev ;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	vector<uint8_t> auth;
	auth.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 7; i++) {
		auth.push_back(OPALUID[OPAL_UID::OPAL_USER1_UID][i]);
	}
	auth.push_back(lockingrange+1);
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, auth)) != 0) {
		LOG(E) << "Error starting session. Did you provide the correct user password? (GlobalRange = User1; Range1 = User2, etc.) " << dev;
		delete session;
		return lastRC;
	}
	DtaCommand *set = new DtaCommand();
	if (NULL == set) {
		LOG(E) << "Unable to create command object " << dev;
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
		LOG(E) << "setupLockingRange Failed " << dev;
		delete set;
		delete session;
		return lastRC;
	}
	delete set;
	delete session;
	if ((lastRC = rekeyLockingRange_SUM(LR, auth, password)) != 0) {
		LOG(E) << "setupLockingRange Unable to reKey Locking range -- Possible security issue " << dev;
		return lastRC;
	}
	LOG(I) << "LockingRange" << (uint16_t)lockingrange << " starting block " << start <<
		" for " << length << " blocks configured as LOCKED range";
	LOG(D1) << "Exiting DtaDevOpal:setupLockingRange_SUM() " << dev;
	return 0;
}
uint8_t DtaDevOpal::configureLockingRange(uint8_t lockingrange, uint8_t enabled, char * password)
{
	uint8_t lastRC;
	LOG(D1) << "Entering DtaDevOpal::configureLockingRange() " << dev;
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
		LOG(E) << "Unable to create session object " << dev;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, getusermode() ? OPAL_UID::OPAL_USER1_UID : OPAL_UID::OPAL_ADMIN1_UID)) != 0) { // NG : JERRY 
	//if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete session;
		return lastRC;
	}
	DtaCommand *set = new DtaCommand();
	if (NULL == set) {
		LOG(E) << "Unable to create command object " << dev;
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
		LOG(E) << "configureLockingRange Failed " << dev;
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
		<< ((enabled & DTA_WRITELOCKINGENABLED) ? "WriteLocking" : "")
		<< " " << dev ;
	LOG(D1) << "Exiting DtaDevOpal::configureLockingRange() " << dev;
	return 0;
}
uint8_t DtaDevOpal::rekeyLockingRange(uint8_t lockingrange, char * password)
{
	LOG(D1) << "Entering DtaDevOpal::rekeyLockingRange() " << dev;
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
		LOG(E) << "Unable to create session object " << dev;
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
		LOG(E) << "Unable to create command object " << dev;
		delete session;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	rekey->reset(OPAL_UID::OPAL_AUTHORITY_TABLE, OPAL_METHOD::GENKEY);
	rekey->changeInvokingUid(response.getRawToken(4));
	rekey->addToken(OPAL_TOKEN::STARTLIST);
	rekey->addToken(OPAL_TOKEN::ENDLIST);
	rekey->complete();
	if ((lastRC = session->sendCommand(rekey, response)) != 0) {
		LOG(E) << "rekeyLockingRange Failed " << dev;
		delete rekey;
		delete session;
		return lastRC;
	}
	delete rekey;
	delete session;
	LOG(I) << "LockingRange" << (uint16_t)lockingrange << " reKeyed " << dev;
	LOG(D1) << "Exiting DtaDevOpal::rekeyLockingRange() " << dev;
	return 0;
}
uint8_t DtaDevOpal::rekeyLockingRange_SUM(vector<uint8_t> LR, vector<uint8_t>  UID, char * password)
{
	LOG(D1) << "Entering DtaDevOpal::rekeyLockingRange_SUM() " << dev;
	uint8_t lastRC;

	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object " << dev;
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
		LOG(E) << "Unable to create command object " << dev;
		delete session;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	rekey->reset(OPAL_UID::OPAL_AUTHORITY_TABLE, OPAL_METHOD::GENKEY);
	rekey->changeInvokingUid(response.getRawToken(4));
	rekey->addToken(OPAL_TOKEN::STARTLIST);
	rekey->addToken(OPAL_TOKEN::ENDLIST);
	rekey->complete();
	if ((lastRC = session->sendCommand(rekey, response)) != 0) {
		LOG(E) << "rekeyLockingRange_SUM Failed " << dev;
		delete rekey;
		delete session;
		return lastRC;
	}
	delete rekey;
	delete session;
	LOG(I) << "LockingRange reKeyed " << dev;
	LOG(D1) << "Exiting DtaDevOpal::rekeyLockingRange_SUM() " << dev;
	return 0;
}
uint8_t DtaDevOpal::setBandsEnabled(int16_t lockingrange, char * password)
{
	if (password == NULL) { LOG(D4) << "Password is NULL"; } // unreferenced formal paramater
	LOG(D1) << "Entering DtaDevOpal::setBandsEnabled()" << lockingrange << " " << dev;
	LOG(I) << "setBandsEnabled is not implemented.  It is not part of the Opal SSC ";
	LOG(D1) << "Exiting DtaDevOpal::setBandsEnabled() " << dev;
	return 0;
}
uint8_t DtaDevOpal::revertLockingSP(char * password, uint8_t keep)
{
	LOG(D1) << "Entering revert DtaDevOpal::revertLockingSP() keep = " << (uint16_t) keep << " " << dev;
	uint8_t lastRC;
	vector<uint8_t> keepGlobalLocking;
	keepGlobalLocking.push_back(0x83);
	keepGlobalLocking.push_back(0x06);
	keepGlobalLocking.push_back(0x00);
	keepGlobalLocking.push_back(0x00);
	DtaCommand *cmd = new DtaCommand();
	if (NULL == cmd) {
		LOG(E) << "Create session object failed " << dev;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Create session object failed " << dev;
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
	LOG(I) << "Revert LockingSP complete " << dev;
	session->expectAbort();
	delete session;
	LOG(D1) << "Exiting revert DtaDev:LockingSP() " << dev;
	return 0;
}
uint8_t DtaDevOpal::eraseLockingRange(uint8_t lockingrange, char * password)
{
	LOG(D1) << "Entering DtaDevOpal::eraseLockingRange()" << lockingrange << " " << dev;
	if (password == NULL) { LOG(D4) << "Referencing formal parameters " << lockingrange; }
	LOG(I) << "eraseLockingRange is not implemented.  It is not part of the Opal SSC ";
	LOG(D1) << "Exiting DtaDevOpal::eraseLockingRange() " << dev;
	return 0;
}
uint8_t DtaDevOpal::getAuth4User(char * userid, uint8_t uidorcpin, std::vector<uint8_t> &userData)
{
	LOG(D1) << "Entering DtaDevOpal::getAuth4User() " << dev;
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
		userData.push_back(atoi(&userid[4]) & 0xff);
	} 
	else {
		if (!memcmp("Admin", userid, 5)) {
			userData.push_back(0x00);
			userData.push_back(0x01);
			userData.push_back(0x00);
			userData.push_back(atoi(&userid[5]) & 0xff );
		}
		else {
			LOG(E) << "Invalid Userid "; // JERRY gabble data << userid;
			//for (int ii=0; ii < 5; ii++) { printf("%02X", userid[ii]); } 
			userData.clear();
			return DTAERROR_INVALID_PARAMETER;
		}
	}
	LOG(D1) << "Exiting DtaDevOpal::getAuth4User() " << dev;
	return 0;
}
uint8_t DtaDevOpal::setPassword(char * password, char * userid, char * newpassword)
{
	LOG(D1) << "Entering DtaDevOpal::setPassword " << dev;
	uint8_t lastRC;
	std::vector<uint8_t> userCPIN, hash;
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object " << dev;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	char * buf = (char *)malloc(20);
	int idx=0;
	memset(buf, 0, 20);
	gethuser(buf);
	if (!memcmp(userid , buf, (disk_info.OPAL20_numUsers < 10) ? 5 : 6 ) ) idx = disk_info.OPAL20_numUsers -1 ;
	// if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, getusermode() ? (OPAL_UID)(OPAL_USER1_UID + idx) : OPAL_UID::OPAL_ADMIN1_UID)) != 0) { // ok work : JERRY can user set its own password ?????
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
		LOG(E) << "Unable to set user " << userid << " new password " << dev;
		delete session;
		return lastRC;
	}
	delete session;
	LOG(D1) << userid << " password changed " << dev;

	//auditRec(newpassword, memcmp(userid, "Admin", 5) ? (uint8_t)evt_PasswordChangedUser: (uint8_t)evt_PasswordChangedAdmin);
	if (!memcmp(userid, "Admin", 5)) { // if admin
		LOG(D1) << "Admin try set password ";
		if ((lastRC = setLockonReset(0, TRUE, newpassword)) != 0) { // enable LOCKING RANGE 0 LOCKonRESET 
			LOG(E) << "failed - unable to set LOCKONRESET " << dev;
			//delete session;
			return lastRC;
		}
		//setuphuser(newpassword); // do not setup audit user when set admin password 
	}
	//else {
	//	LOG(I) << "User try set password ";
	//}
	
	LOG(D1) << "Exiting DtaDevOpal::setPassword() " << dev;
	return 0;
}
uint8_t DtaDevOpal::setNewPassword_SUM(char * password, char * userid, char * newpassword)
{
	LOG(D1) << "Entering DtaDevOpal::setNewPassword_SUM " << dev;
	uint8_t lastRC; 
	std::vector<uint8_t> userCPIN, hash;
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object " << dev;
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
		LOG(E) << "Invalid userid \"" << userid << "\"specified for setNewPassword_SUM " << dev;
		delete session;
		return DTAERROR_INVALID_PARAMETER;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, auth)) != 0) {
		delete session;
		return lastRC;
	}
	if ((lastRC = getAuth4User(userid, 10, userCPIN)) != 0) {
		LOG(E) << "Unable to find user " << userid << " in Authority Table " << dev;
		delete session;
		return lastRC;
	}
	DtaHashPwd(hash, newpassword, this);
	if ((lastRC = setTable(userCPIN, OPAL_TOKEN::PIN, hash)) != 0) {
		LOG(E) << "Unable to set user " << userid << " new password " << dev;
		delete session;
		return lastRC;
	}
	LOG(I) << userid << " password changed " << dev;
	delete session;
	LOG(D1) << "Exiting DtaDevOpal::setNewPassword_SUM() " << dev;
	return 0; 
}
uint8_t DtaDevOpal::setMBREnable(uint8_t mbrstate,	char * Admin1Password)
{
	LOG(D1) << "Entering DtaDevOpal::setMBREnable " << dev;
	uint8_t lastRC;
        // set MBRDone before changing MBREnable so the PBA isn't presented
        if ((lastRC = setMBRDone(1, Admin1Password)) != 0) {
		LOG(E) << "unable to set MBRDone " << dev;
                return lastRC;
        }
	if (mbrstate) {
		if ((lastRC = setLockingSPvalue(OPAL_UID::OPAL_MBRCONTROL, OPAL_TOKEN::MBRENABLE,
			OPAL_TOKEN::OPAL_TRUE, Admin1Password, NULL)) != 0) {
			LOG(E) << "Unable to set setMBREnable on " << dev;
			return lastRC;
		}
		else {
			LOG(I) << "MBREnable set on " << dev ;
		}
	} 
	else {
		if ((lastRC = setLockingSPvalue(OPAL_UID::OPAL_MBRCONTROL, OPAL_TOKEN::MBRENABLE,
				OPAL_TOKEN::OPAL_FALSE, Admin1Password, NULL)) != 0) {
				LOG(E) << "Unable to set setMBREnable off " << dev;
				return lastRC;
			}
		else {
			LOG(I) << "MBREnable set off " << dev;
		}
	}
	LOG(D1) << "Exiting DtaDevOpal::setMBREnable " << dev;
	return 0;
}
uint8_t DtaDevOpal::setMBRDone(uint8_t mbrstate, char * Admin1Password)
{
	// hijack to setTperResetEnable
	/*
	LOG(I) << "Entering HIJACKED DtaDevOpal::setMBRDon " << dev;
	bool state;
	state = true;
	setTperResetEnable(state, Admin1Password);
	return 0;
	*/

	LOG(D1) << "Entering DtaDevOpal::setMBRDone " << dev;
	uint8_t lastRC;
	if (mbrstate) {
		if ((lastRC = setLockingSPvalue(OPAL_UID::OPAL_MBRCONTROL, OPAL_TOKEN::MBRDONE,
			OPAL_TOKEN::OPAL_TRUE, Admin1Password, NULL)) != 0) {
			LOG(E) << "Unable to set setMBRDone on " << dev;
			return lastRC;
		}
		else {
			LOG(I) << "MBRDone set on " << dev;
		}
	}
	else {
		if ((lastRC = setLockingSPvalue(OPAL_UID::OPAL_MBRCONTROL, OPAL_TOKEN::MBRDONE,
			OPAL_TOKEN::OPAL_FALSE, Admin1Password, NULL)) != 0) {
			LOG(E) << "Unable to set setMBRDone off " << dev;
			return lastRC;
		}
		else {
			LOG(I) << "MBRDone set off " << dev;
		}
	}
	LOG(D1) << "Exiting DtaDevOpal::setMBRDone " << dev;

	return 0;
}
uint8_t DtaDevOpal::TCGreset(uint8_t mbrstate)
{
	LOG(I) << "Entering DtaDevOpal::TCGreset " << dev;
	uint8_t lastRC;
	switch (mbrstate) { 
		case 1 : // mbrstate=1 , Tper Reset;  mbrstate=2, STACK_RESET
			LOG(I) << "TPerReset";
			DtaDev::TperReset();
			break;
		case 2 : 
			LOG(I) << "STACK_RESET";
			STACK_RESET();
			break;
		default : 
		// default do nothing 
			break;
	}
	return 0;
}
uint8_t DtaDevOpal::STACK_RESET()
{
	LOG(D1) << "Entering DtaDevOpal::STACK_RESET " << dev;
	uint8_t lastRC;
	DtaCommand *set = new DtaCommand();
	if (NULL == set) {
		LOG(E) << "Unable to create command object " << dev;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	set->reset();
	set->setcomID(comID());
	set->addToken((uint32_t)0x02);
	LOG(I) << "1st dump";
	set->dumpCommand();

	// 8 - transfer len == 0 
	set->complete();
	LOG(I) << "2nd dump";
	set->dumpCommand();

	if ((lastRC = session->sendCommand(set, response)) != 0) {
		LOG(E) << "StackReset Fail " << dev;
		delete set;
		return lastRC;
	}
	delete set;
	LOG(D1) << "Exiting DtaDevOpal::STACK_RESET " << dev;
	return 0;
}
uint8_t DtaDevOpal::setLockingRange(uint8_t lockingrange, uint8_t lockingstate,
	char * Admin1Password)
{
	uint8_t lastRC;
	OPAL_TOKEN readlocked, writelocked;
	const char *msg;

	LOG(D1) << "Entering DtaDevOpal::setLockingRange " << dev;
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
		LOG(E) << "Invalid locking state for setLockingRange " << dev;
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
		LOG(E) << "Unable to create session object " << dev;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, Admin1Password, getusermode() ? OPAL_UID::OPAL_USER1_UID : OPAL_UID::OPAL_ADMIN1_UID)) != 0) { // JERRY can User1 set Lockingrange RW ??????
		delete session;
		return lastRC;
	}
	DtaCommand *set = new DtaCommand();
	if (NULL == set) {
		LOG(E) << "Unable to create command object " << dev;
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
		LOG(E) << "setLockingRange Failed " << dev;
		delete set;
		delete session;
		return lastRC;
	}
	delete set;
	delete session;
	LOG(I) << "LockingRange" << (uint16_t)lockingrange << " set to " << msg << " " << dev;
	LOG(D1) << "Exiting DtaDevOpal::setLockingRange " << dev;
	return 0;
}
uint8_t DtaDevOpal::setLockingRange_SUM(uint8_t lockingrange, uint8_t lockingstate,
	char * password)
{
	uint8_t lastRC;
	OPAL_TOKEN readlocked, writelocked;
	const char *msg;

	LOG(D1) << "Entering DtaDevOpal::setLockingRange_SUM " << dev;
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
		LOG(E) << "Invalid locking state for setLockingRange " << dev;
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
		LOG(E) << "Unable to create session object " << dev;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	vector<uint8_t> auth;
	auth.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 7; i++) {
		auth.push_back(OPALUID[OPAL_UID::OPAL_USER1_UID][i]);
	}
	auth.push_back(lockingrange+1);
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, auth)) != 0) {
		LOG(E) << "Error starting session. Did you provide the correct user password? (GlobalRange = User1; Range1 = User2, etc.)" << dev;
		delete session;
		return lastRC;
	}

	DtaCommand *set = new DtaCommand();
	if (NULL == set) {
		LOG(E) << "Unable to create command object " << dev;
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
		LOG(E) << "setLockingRange Failed " << dev;
		delete set;
		delete session;
		return lastRC;
	}
	delete set;
	delete session;
	LOG(I) << "LockingRange" << (uint16_t)lockingrange << " set to " << msg << " " << dev;
	LOG(D1) << "Exiting DtaDevOpal::setLockingRange_SUM " << dev;
	return 0;
}
uint8_t DtaDevOpal::setLockingSPvalue(OPAL_UID table_uid, OPAL_TOKEN name, 
	OPAL_TOKEN value,char * password, char * msg)
{
	LOG(D1) << "Entering DtaDevOpal::setLockingSPvalue " << dev;
	uint8_t lastRC;
	vector<uint8_t> table;
	table. push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		table.push_back(OPALUID[table_uid][i]);
	}
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object " << dev;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, getusermode()? OPAL_UID::OPAL_USER1_UID : OPAL_UID::OPAL_ADMIN1_UID)) != 0) { // JERRY ADMIN1 or USER1 ?????
		delete session;
		return lastRC;
	}
	if ((lastRC = setTable(table, name, value)) != 0) {
		LOG(E) << "Unable to update table " << dev;
		delete session;
		return lastRC;
	}
	if (NULL != msg) {
		LOG(I) << msg;
	}
	
	delete session;
	LOG(D1) << "Exiting DtaDevOpal::setLockingSPvalue() " << dev;
	return 0;
}

uint8_t DtaDevOpal::enableUser(uint8_t mbrstate, char * password, char * userid)
{
	LOG(D1) << "Entering DtaDevOpal::enableUser " << dev;
	uint8_t lastRC;
	vector<uint8_t> userUID;
	
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object " << dev;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete session;
		return lastRC;
	}
	if ((lastRC = getAuth4User(userid, 0, userUID)) != 0) {
		LOG(E) << "Unable to find user " << userid << " in Authority Table " << dev;
		delete session;
		return lastRC;
	}
	if ((lastRC = setTable(userUID, (OPAL_TOKEN)0x05, mbrstate ? OPAL_TOKEN::OPAL_TRUE : OPAL_TOKEN::OPAL_FALSE)) != 0) {
		LOG(E) << "Unable to enable user " << userid << " " << dev;
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
		LOG(D2) << userid << " has been enabled " << dev;
	else 
		LOG(D2) << userid << " has been disabled" << dev;

	delete session;
	LOG(D1) << "Exiting DtaDevOpal::enableUser() " << dev;
	return 0;
}

/*
OPAL_UID getUIDtoken(char * userid)
{
	// translate UserN AdminN into <int8_t 
	uint8_t id;

	if (!memcmp("User", userid, 4)) {// UserI UID
		id = (uint8_t)(OPAL_UID::OPAL_USER1_UID) + atoi(&userid[4]) - 1;
		IFLOG(D4) printf("UserN=%s enum=%d\n", userid, id);
		return  (OPAL_UID)id; 
	}
	else
	{
		id = (uint8_t)(OPAL_UID::OPAL_ADMIN1_UID) + atoi(&userid[4]) -1 ;
		printf("AdminN=%s enum=%d\n", userid, id);
		return  (OPAL_UID)id;
	}
}
*/

vector<uint8_t> getUID(char * userid, vector<uint8_t> &auth2, vector<uint8_t> &auth3, uint8_t hu)
{
	// translate UserN AdminN into <int8_t 
	vector<uint8_t> auth;
	;
	uint8_t id = 1;
	auth.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	auth2.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	auth3.push_back(OPAL_SHORT_ATOM::BYTESTRING8);

	
	if (!memcmp("User", userid, 4)) {// UserI UID
		if (strnlen(userid, 6) == 5) {
			id = (uint8_t)atoi(&userid[4]); // (uint8_t)atoi(argv[opts.dsnum])
		}
		else if (strnlen(userid, 6) == 6) {
			id = ((uint8_t)atoi(&userid[4]));
		}

		//IFLOG(D4) printf("UserN : %s traslated id = %d\n", userid,id);
		for (int i = 0; i < 7; i++) {
			auth.push_back(OPALUID[OPAL_UID::OPAL_USER1_UID][i]);
			auth2.push_back(OPALUID[OPAL_UID::OPAL_ADMIN1_UID][i]);
			auth3.push_back(OPALUID[OPAL_UID::OPAL_USER1_UID + (hu - 1)][i]);
			//auth3.push_back(OPALUID[OPAL_UID::OPAL_USER1_UID][i]); // first 7-byte is all the same as OPAL_USER1_UID, the only difference is the 8th byte 
		}
		auth.push_back(id);
		auth2.push_back(1); // always admin1 or user1 
	}
	else { // "Admin"
		//IFLOG(D4) printf("AdminN %s\n", userid); 
		id = (uint8_t)atoi(&userid[5]);
		for (int i = 0; i < 7; i++) {
			auth.push_back(OPALUID[OPAL_UID::OPAL_ADMIN1_UID][i]);
			auth2.push_back(OPALUID[OPAL_UID::OPAL_USER1_UID][i]);
			auth3.push_back(OPALUID[OPAL_UID::OPAL_USER1_UID+(hu-1)][i]);
			//auth3.push_back(OPALUID[OPAL_UID::OPAL_USER1_UID][i]);
		}
		auth.push_back(id); // AdminN 
		auth2.push_back(1); // always admin1
	}
	//auth.push_back(id);
	//auth2.push_back(1); // always admin1 or user1 
	auth3.push_back(hu); // always audit user 
	return auth;
}


uint8_t DtaDevOpal::userAcccessEnable(uint8_t mbrstate, OPAL_UID UID, char * userid)
{
	uint8_t lastRC;
	// Give UserN read access to the DataStore table
	DtaCommand *cmd = new DtaCommand();
	if (NULL == cmd) {
		LOG(E) << "Unable to create command object " << dev;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}

	// translate UserN AdminN into <int8_t>
	vector<uint8_t> auth, auth2, auth3;
	auth = getUID(userid, auth2, auth3, disk_info.OPAL20_numUsers); // always add audit user to auth3. audit user is added first, the following userid will preserve the audit userid
	LOG(D4) << "auth";  IFLOG(D4) { for (int i = 0; i < 9; i++) printf("%02X, ", auth[i]);  printf("\n"); }
	LOG(D4) << "auth2"; IFLOG(D4) { for (int i = 0; i < 9; i++) printf("%02X, ", auth2[i]);  printf("\n"); }
	LOG(D4) << "auth3"; IFLOG(D4) { for (int i = 0; i < 9; i++) printf("%02X, ", auth3[i]);  printf("\n"); }

	cmd->reset(UID, OPAL_METHOD::SET);
	//
	cmd->addToken(OPAL_TOKEN::STARTLIST);
	cmd->addToken(OPAL_TOKEN::STARTNAME);
	cmd->addToken(OPAL_TOKEN::VALUES);
	cmd->addToken(OPAL_TOKEN::STARTLIST);
	cmd->addToken(OPAL_TOKEN::STARTNAME);
	cmd->addToken(OPAL_TOKEN::OPAL_BOOLEAN_EXPR);
	//
	cmd->addToken(OPAL_TOKEN::STARTLIST);
	// User1 /////////////////////////////////////////////////////
	cmd->addToken(OPAL_TOKEN::STARTNAME);
	cmd->addToken(OPAL_UID::OPAL_HALF_UID_AUTHORITY_OBJ_REF, 4); //????? how to insert 4-byte here, addToken will insert BYTESTRING4 token
	cmd->addToken(auth);
	cmd->addToken(OPAL_TOKEN::ENDNAME);
	// /////////////////////////////////////////////////////////////
	// User2 
	cmd->addToken(OPAL_TOKEN::STARTNAME);
	cmd->addToken(OPAL_UID::OPAL_HALF_UID_AUTHORITY_OBJ_REF, 4); //????? how to insert 4-byte here, addToken will insert BYTESTRING4 token
	cmd->addToken(auth2);  
	cmd->addToken(OPAL_TOKEN::ENDNAME);
	//
	cmd->addToken(OPAL_TOKEN::STARTNAME);
	cmd->addToken(OPAL_UID::OPAL_HALF_UID_BOOLEAN_ACE, 4);
	cmd->addToken(mbrstate ? OPAL_TOKEN::VALUES : OPAL_TOKEN::WHERE);
	cmd->addToken(OPAL_TOKEN::ENDNAME);
	//
	// always add this audit use, if request user id is not audit id
	// !memcmp("User", userid, 4) 
	if (1) { // ((uint8_t)atoi(&userid[4]) != disk_info.OPAL20_numUsers) { 
		LOG(D1) << "addition audit user added " << dev;
		//
		cmd->addToken(OPAL_TOKEN::STARTNAME);
		cmd->addToken(OPAL_UID::OPAL_HALF_UID_AUTHORITY_OBJ_REF, 4);
		cmd->addToken(auth3);
		cmd->addToken(OPAL_TOKEN::ENDNAME); // can only add single half-uid ?????
		//
		cmd->addToken(OPAL_TOKEN::STARTNAME);
		cmd->addToken(OPAL_UID::OPAL_HALF_UID_BOOLEAN_ACE, 4);
		cmd->addToken(mbrstate ? OPAL_TOKEN::VALUES : OPAL_TOKEN::WHERE);
		cmd->addToken(OPAL_TOKEN::ENDNAME);

		//
		//cmd->addToken(OPAL_TOKEN::STARTNAME);
		//cmd->addToken(OPAL_UID::OPAL_HALF_UID_AUTHORITY_OBJ_REF, 4);
		//cmd->addToken(auth3);
		//cmd->addToken(OPAL_TOKEN::ENDNAME); // can only add even number of user ?????
	}
	else {
		LOG(I) << "no addition audit user added " << dev;
	}
	//
	// above is audit user 
	//
	//

	//
	cmd->addToken(OPAL_TOKEN::ENDLIST);
	cmd->addToken(OPAL_TOKEN::ENDNAME);
	cmd->addToken(OPAL_TOKEN::ENDLIST);
	cmd->addToken(OPAL_TOKEN::ENDNAME);
	cmd->addToken(OPAL_TOKEN::ENDLIST);

	cmd->complete();

	LOG(D4) << "Dump enable user access cmd buffe r" << dev;
	IFLOG(D4) DtaHexDump(cmd->cmdbuf, 176 ); 

	if ((lastRC = session->sendCommand(cmd, response)) != 0) {
		LOG(E) << "***** send enable/disable user access command fail " << dev;
		delete cmd;
		return lastRC;
	}
	LOG(D2) << "***** " << (mbrstate ? "enable" : "disable") << " user access command OK " << dev;

	delete cmd;
	LOG(D1) << "***** end of enable/disable user access command " << dev;
	return 0;
}
uint8_t DtaDevOpal::enableUserRead(uint8_t mbrstate, char * password, char * userid)
{
	LOG(D1) << "Entering DtaDevOpal::enableUserRead " << dev;
	uint8_t lastRC;
	uint8_t error;
	vector<uint8_t> userUID;

	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object " << dev;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete session;
		return lastRC;
	}
	if ((lastRC = getAuth4User(userid, 0, userUID)) != 0) {
		LOG(E) << "Unable to find user " << userid << " in Authority Table " << dev;
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
	//LOG(D1) << "***** " << (mbrstate ? "enable " : "disbale ") << "OPAL_ACE_MBRControl_Set_Enable for " << userid;
	//error |= userAcccessEnable(mbrstate, OPAL_UID::OPAL_ACE_MBRControl_Set_Enable, userid); // NG6
	LOG(D1) << "***** " << (mbrstate ? "enable " : "disbale ") << "OPAL_ACE_Locking_GlobalRange_Get_RangeStartToActiveKey for " << userid;
	error |= userAcccessEnable(mbrstate, OPAL_UID::OPAL_ACE_Locking_GlobalRange_Get_RangeStartToActiveKey, userid);
	LOG(D1) << "***** " << (mbrstate ? "enable " : "disbale ") << "OPAL_ACE_Locking_GlobalRange_Get_RangeStartToActiveKey for " << userid;
	error |= userAcccessEnable(mbrstate, OPAL_UID::OPAL_ACE_Locking_GlobalRange_Set_ReadLocked, userid);
	LOG(D1) << "***** " << (mbrstate ? "enable " : "disbale ") << "OPAL_ACE_Locking_GlobalRange_Set_WriteLocked for " << userid;
	error |= userAcccessEnable(mbrstate, OPAL_UID::OPAL_ACE_Locking_GlobalRange_Set_WriteLocked, userid);
	//LOG(D1) << "***** " << (mbrstate ? "enable " : "disbale ") << "OPAL_ACE_Locking_GlobalRange_Admin_Set for " << userid;
	//error |= userAcccessEnable(mbrstate, OPAL_UID::OPAL_ACE_Locking_GlobalRange_Admin_Set, userid); // NG10
	//LOG(D1) << "***** " << (mbrstate ? "enable " : "disbale ") << "OPAL_ACE_Locking_GlobalRange_Admin_Start for " << userid;
	//error |= userAcccessEnable(mbrstate, OPAL_UID::OPAL_ACE_Locking_GlobalRange_Admin_Start, userid); // NG11

	if (error) {
		LOG(E) << (mbrstate ? "enable " : "disbale ") << "one of user accese fail" << dev;
		delete session;
		return error;
	}

	delete session;
	return 0;
}


uint8_t DtaDevOpal::revertTPer(char * password, uint8_t PSID, uint8_t AdminSP)
{
	LOG(D1) << "Entering DtaDevOpal::revertTPer() " << AdminSP << " " << dev;
	uint8_t lastRC;
	DtaCommand *cmd = new DtaCommand();
	if (NULL == cmd) {
		LOG(E) << "Unable to create command object " << dev;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object " << dev;
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
	LOG(I) << "revertTper completed successfully " << dev;
	delete cmd;
	delete session;

	//auditRec(password, evt_Revert);
	LOG(D1) << "Exiting DtaDevOpal::revertTPer() " << dev;
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
	uint8_t pbaver[512];	
	uint8_t boot[512];
	uint8_t bootsig[2] = { 0x55,0xAA };
	uint8_t pat[16] = { 0xEB,0x3C, 0x90, 0x6D,  0x6B, 0x66, 0x73, 0x2E,  0x66, 0x61, 0x74, };
	lastRC = MBRRead(password, 512, 512, (char *)pbaver);
	if (lastRC) {
		LOG(D1) << "MBRRead error " << dev;
		return lastRC;
	}

	LOG(I) << "DtaDevOpal::pbaValid() show license level only in Linux " << dev;
	/*IFLOG(D4) */ DtaHexDump(pbaver+128, 32);
	printf("PBA image license level :");
	for (uint8_t i = 0; i < 32; i++) {
		printf("%02X", pbaver [ 128 + i]);
	}
	printf(":\n");


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
	printf("PBA image valid %s" , dev);
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
	LOG(D1) << "Entering DtaDevOpal::MBRRead() with buffer " << dev;
	uint8_t lastRC;
	vector<uint8_t> LR;
	uint32_t filepos = 0; // startpos;
	uint32_t blocksize = len;

	DtaCommand *cmd = new DtaCommand();
	if (NULL == cmd) {
		LOG(E) << "Unable to create command object " << dev;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}

	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object " << dev;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete cmd;
		delete session;
		return lastRC;
	}

	LOG(D1) << "***** read shadow mbr " << dev;
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
	LOG(D1) << "***** send read shadow mbr command " << dev;
	if ((lastRC = session->sendCommand(cmd, response)) != 0) {
		delete cmd;
		delete session;
		return lastRC;
	}
	response.getBytes(1, (uint8_t *)buffer); // data is token 1
	LOG(D1) << "***** end of send read shadow mbr command " << dev;
	delete cmd;
	delete session;

	return 0;

}

uint8_t DtaDevOpal::DataRead(char * password, uint32_t startpos, uint32_t len, char * buffer, char * userid)
{
	LOG(D1) << "Entering DtaDevOpal::DataRead() " << dev;
	uint8_t lastRC;
	vector<uint8_t> LR;
	uint32_t filepos = startpos;
	uint32_t blocksize = len;

	DtaCommand *cmd = new DtaCommand();
	if (NULL == cmd) {
	LOG(E) << "Unable to create command object " << dev;
	return DTAERROR_OBJECT_CREATE_FAILED;
	}

	session = new DtaSession(this);
	if (NULL == session) {
	LOG(E) << "Unable to create session object " << dev;
	return DTAERROR_OBJECT_CREATE_FAILED;
	}
	LOG(D1) << "start lockingSP session " << dev;
	// ???????????????????????????????????????????????????????????????????????????????????
	// experiement start lockingSP with User1 password"
	// ????????????????????????????????????????????????????????????????????????????????????
		// translate UserN AdminN into <int8_t 
		//printf(" ***** start LOCKINGSP with %s  Token = %d\n", userid, getUIDtoken(userid));
		vector<uint8_t> auth,auth2,auth3;
		auth = getUID(userid,auth2,auth3,disk_info.OPAL20_numUsers); // pass vector directly, not enum index of vector table
		//for (int i = 0; i < 9; i++) {
		//	printf("%02X ", auth[i]);
		//} 
		//printf("\n");
		//LOG(I) << "audit data read password" << password; // JERRY
		//for (int i = 0; i < auth.size(); i++) printf("%02", auth.at(i)); printf("\n"); // JERRY

		if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, auth)) != 0) { // JERRY OPAL_UID::OPAL_ADMIN1_UID ->OK getUID() --> OK too; getUIDtoken-->NG ????
			delete cmd;
			delete session;
			return lastRC;
		}

	LOG(D1) << "***** start read data store " << dev;

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
	LOG(D1) << "***** send read data store command " << dev;
	if ((lastRC = session->sendCommand(cmd, response)) != 0) {
	delete cmd;
	delete session;
	return lastRC;
	}
	
	response.getBytes(1, (uint8_t *) buffer); // data is token 1
	LOG(D1) << "raw data after data store read response data " << dev;

	IFLOG(D4) DtaHexDump(buffer, gethdrsize()); // contain header and entries
	LOG(D1) << "***** end of send read data store command " << dev;
	delete cmd;
	delete session;
	return 0;
	
}

uint8_t DtaDevOpal::DataWrite(char * password, uint32_t startpos, uint32_t len, char * buffer, char * userid)
{
	LOG(D1) << "Entering DtaDevOpal::DataWrite() " << dev;
	uint8_t lastRC;
	vector<uint8_t> LR;
	vector <uint8_t> bufferA; // (8192, 0x66); // 0 buffer  (57344, 0x00),
	vector <uint8_t> lengthtoken;
	uint32_t filepos = startpos;

	bufferA.insert(bufferA.begin(), buffer, buffer + len ); 
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
		LOG(E) << "Unable to create command object " << dev;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}

	session = new DtaSession(this);
	if (NULL == session) {
	LOG(E) << "Unable to create session object " << dev;
	return DTAERROR_OBJECT_CREATE_FAILED;
	}
	LOG(D1) << "start lockingSP session " << dev;
	vector<uint8_t> auth,auth2,auth3;
	auth = getUID(userid,auth2, auth3, disk_info.OPAL20_numUsers);
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, auth)) != 0) { // OPAL_UID::OPAL_ADMIN1_UID
	delete cmd;
	delete session;
	return lastRC;
	}
	/* temp out due to MX300 limitation
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
	}*/ 
	LOG(D1) << "Writing to data store 0 " << dev;
	LOG(I) << "filepos= " << filepos << " length= " << len << " " << dev;

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
	
	/* temp out due to MX300 limitation
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
	}*/

	delete cmd;
	delete session;
	LOG(D1) << "Exiting DtaDevOpal::DataWrite() " << dev;
	return 0;
}



uint8_t DtaDevOpal::auditlogwr(char * password, uint32_t startpos, uint32_t len, char * buffer, entry_t * pent, char * userid) // add event ID and write audit log to Data Store
{
	audit_t * ptr;
	entry_t * ptrent;
	vector <entry_t> entryA;
	uint8_t lastRC;

	uint32_t MAX_ENTRY;

	MAX_ENTRY = 1000; // default size
	if (disk_info.DataStore_maxTableSize < 10485760) {
		MAX_ENTRY = 100;
	}

	LOG(D1) << "***** Entering DtaDevOpal::auditlogwr ***** " << dev;

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
		// linux 
        #if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__)
		time_t rawtime;
  		struct tm * lt;
  		/* get current timeinfo */
  		time ( &rawtime );
  		lt = localtime ( &rawtime );
		//entry_t ent; passing from caller
		pent->yy = (uint8_t)(lt->tm_year + 1900 - 2000);
		pent->mm = (uint8_t)lt->tm_mon + 1;
		pent->dd = (uint8_t)lt->tm_mday;
		pent->hh = (uint8_t)lt->tm_hour;
		pent->min = (uint8_t)lt->tm_min;
		pent->sec = (uint8_t)lt->tm_sec;
		//pent->event = ID;
	#else
		// windows
		SYSTEMTIME st, lt;
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
	#endif


	} // if empty entry past, fill up the entry with system time
	// otherwise, past entry already has time stamp and eventID
	entryA.insert(entryA.begin(), *pent); // push_back -> insert
	//printf("entryA.size()=%u\n", entryA.size());
	//for (int kkk = 0; kkk < entryA.size(); kkk++) printf("%02X", entryA.at(kkk));
	//printf("\n");
	//if (entryA.size() > MAX_ENTRY * 8) {
	//printf("size of entryA * 8 = %zd\n", entryA.size() * 8);
	ptr->header.num_entry = (ptr->header.num_entry + 1); // inc number of entry if less than MAX_ENTRY, otherwise maintain the MAX_ENTRY 
	ptr->header.tail = (ptr->header.tail + 1);
	if (ptr->header.num_entry > MAX_ENTRY) {

		LOG(D1) << "num of entry exceed MAX_ENT ; erase() the last entry ";
		ptr->header.num_entry = MAX_ENTRY;
		ptr->header.tail = (ptr->header.tail - 1);
		try {
			entryA.erase(entryA.end()-1);
		}
		catch (char *e) {
			printf("Exception Caught: %s\n", e);
		}
	}

	memcpy(ptr->buffer, &entryA[0], MAX_ENTRY * 8);
	/*
	if (entryA.size() > MAX_ENTRY ) {
		LOG(I) << "erase() the last entry ";
		entryA.erase(entryA.end());
	}
	else {
		memcpy(ptr->buffer, &entryA[0], MAX_ENTRY * 8);
		if (ptr->header.num_entry < MAX_ENTRY) {
			ptr->header.num_entry = (ptr->header.num_entry + 1); // inc number of entry if less than MAX_ENTRY, otherwise maintain the MAX_ENTRY 
			ptr->header.tail = (ptr->header.tail + 1);
		}
	}
	*/
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
		LOG(D1) << "write data store success " << dev;
		//DtaHexDump(&auditL, (int)(entryA.size()) * 8 + sizeof(_audit_hdr));
		IFLOG(D4) DtaHexDump(buffer, (int)ptr->header.num_entry * 8 + sizeof(_audit_hdr));
		return 0;
	}
	else {
		LOG(E) << "write data store Error " << dev;
		return lastRC;
	}
}

uint8_t DtaDevOpal::auditlogrd(char * password, uint32_t startpos, uint32_t len, char * buffer, char * userid) // read audit log to Data Store
{
	LOG(D1) << "entering DtaDevOpal::auditlogrd " << dev;
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
			return 0; // ERRCHKSUM;
		}
		if (getchksum(buffer) != genchksum(buffer))
		{
			LOG(D1) << "Audit log header check sum error " << dev;
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
		LOG(E) << "read data store Error " << dev;
		return lastRC;
	}
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
		LOG(E) << "auditlogrd() returned error " << dev;
		return lastRC;
	}
	LOG(D4) << "raw data of data store read " << dev;
	IFLOG(D4)  DtaHexDump(buffer, gethdrsize());
	audit_t * ptr = (audit_t *)buffer;
	audit_hdr hdrtmp;
	char str1[] = SIGNATURE; // strlen() return 23 char but the array length is 24
	//lastRC = (uint8_t)memcmp(ptr->header.signature, str1, strlen(str1));
	//printf("ptr->header.signature=%s string length = %d\n", ptr->header.signature, (int) strlen(str1));
	//printf("***** lastRC = %d ; ptr->header.num_entry= %d*****\n", lastRC, ptr->header.num_entry);
	if ((lastRC = (uint8_t)memcmp(ptr->header.signature, str1, strlen(str1))) !=0)
	{
		LOG(D1) << "Invalid Audit signature : lastRC = " << lastRC << " or num_entry is zero : " << hex << ptr->header.num_entry;
        #if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__)
		time_t rawtime;
  		struct tm * lt;
  		/* get current timeinfo */
  		time ( &rawtime );
  		lt = localtime ( &rawtime );
		//entry_t ent; passing from caller
		hdrtmp.date_created.yy = (uint8_t)((lt->tm_year+1900-2000) & 0xff);
		hdrtmp.date_created.mm = (uint8_t)lt->tm_mon+1;
		hdrtmp.date_created.dd = (uint8_t)lt->tm_mday;
		hdrtmp.date_created.hh = (uint8_t)lt->tm_hour;
		hdrtmp.date_created.min = (uint8_t)lt->tm_min;
		hdrtmp.date_created.sec = (uint8_t)lt->tm_sec;
		//pent->event = ID;
	#else
		SYSTEMTIME st, lt;
		GetSystemTime(&st);
		GetLocalTime(&lt);
		//entry_t ent;
		hdrtmp.date_created.yy = (uint8_t)((lt.wYear-2000) & 0xff);
		hdrtmp.date_created.mm = (uint8_t)lt.wMonth;
		hdrtmp.date_created.dd = (uint8_t)lt.wDay;
		hdrtmp.date_created.hh = (uint8_t)lt.wHour;
		hdrtmp.date_created.min = (uint8_t)lt.wMinute;
		hdrtmp.date_created.sec = (uint8_t)lt.wSecond;
	#endif	
		memset(buffer, 0, MAX_ENTRY * 8 + gethdrsize());
		memcpy(buffer, (char *)&hdrtmp, gethdrsize());
		wrtchksum(buffer, genchksum(buffer)); 
		IFLOG(D4) DtaHexDump(buffer, gethdrsize());
	}
	if (getchksum(buffer) != genchksum(buffer))
	{
		LOG(E) << "Read Audit log header check sum error, stop writing audit log " << dev;
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
	printf("***** (uint8_t )evt::evt_lastID=%d event  %d  %s ***** \n", (uint8_t)evt::evt_lastID, pent->event, dev);
	lastRC = auditlogwr(password, 0, (MAX_ENTRY * 8) + gethdrsize(), buffer, pent, userid); // use rand id for test
	if (lastRC)
	{
		LOG(E) << "audit write error : " << hex << lastRC << " " << dev;
		return lastRC;
	}
	else
	{
		LOG(D1) << "audit write success " << dev;
		return 0;
	}
}

uint8_t DtaDevOpal::auditErase(char * password, char * userid)
{
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
}

uint8_t DtaDevOpal::auditRead(char * password, char * userid)
{
	char * buffer;
	uint8_t lastRC;
	uint32_t MAX_ENTRY;
	LOG(I) << "***** enter audit read " << dev;
	MAX_ENTRY = 1000; // default size
	if (disk_info.DataStore_maxTableSize < 10485760) {
		MAX_ENTRY = 100;
	}

	buffer = (char *)malloc(8 * MAX_ENTRY + gethdrsize());
	memset(buffer, 0, (MAX_ENTRY * 8) + gethdrsize());
	lastRC = auditlogrd(password, 0, (MAX_ENTRY * 8) + gethdrsize(), buffer,userid);
	return lastRC;
}

// duplicate from DtaHashPwd.cpp 
// credit
// https://www.codeproject.com/articles/99547/hex-strings-to-raw-data-and-back
//

inline unsigned char hex_digit_to_nybble(char ch)
{
	switch (ch)
	{
	case '0': return 0x0;
	case '1': return 0x1;
	case '2': return 0x2;
	case '3': return 0x3;
	case '4': return 0x4;
	case '5': return 0x5;
	case '6': return 0x6;
	case '7': return 0x7;
	case '8': return 0x8;
	case '9': return 0x9;
	case 'a': return 0xa;
	case 'A': return 0xa;
	case 'b': return 0xb;
	case 'B': return 0xb;
	case 'c': return 0xc;
	case 'C': return 0xc;
	case 'd': return 0xd;
	case 'D': return 0xd;
	case 'e': return 0xe;
	case 'E': return 0xe;
	case 'f': return 0xf;
	case 'F': return 0xf;
	default: return 0xff;  // throw std::invalid_argument();
	}
}

vector<uint8_t> hex2data_a(char * password)
{
	vector<uint8_t> h;
	h.clear();
	if (false)
		printf("strlen(password)=%d\n", (int)strlen(password));
	/*
	if (strlen(password) != 16)
	{
		//LOG(I) << "Hashed Password length isn't 64-byte, no translation";
		h.clear();
		for (uint16_t i = 0; i < (uint16_t)strnlen(password, 32); i++)
			h.push_back(password[i]);
		return h;
	}
	*/
	
	//printf("GUI hashed password=");
	for (uint16_t i = 0; i < (uint16_t)strlen(password); i += 2)
	{
		h.push_back(
			(hex_digit_to_nybble(password[i])) * 10 +  // high 4-bit
			(hex_digit_to_nybble(password[i + 1]) & 0x0f)); // lo 4-bit
	}
	//for (uint16_t i = 0; i < (uint16_t)h.size(); i++)
	//	printf("%02x", h[i]);
	//printf("\n");
	return h;
}
uint8_t DtaDevOpal::auditWrite(char * password, char * idstr, char * userid)
{
	uint8_t lastRC;
	entry_t ent;
	uint8_t * pent; 
	pent = (uint8_t *)&ent;
	memset(&ent, 0, sizeof(entry_t));
	char t[16];
	memset(t, 0, 16);
	//DtaHexDump(idstr, 16);
	vector<uint8_t> h;
	h.clear();
	h = hex2data_a(idstr);
	uint16_t len = 0; 
	if (h.size() < 8) {
		for (int jj = 0; jj < (8 - h.size()); jj++ ) h.push_back(0); // fill up h to 8 bytes
	}

	for (uint8_t ii = 0; ii < 8; ii++) {
		pent[ii] = h.at(ii);
	}
	//DtaHexDump(pent, 16);
	//DtaHexDump(&ent, 16);

	/*
	memcpy(t, idstr, 2);
	DtaHexDump(t, 3);
	ent.event = (uint8_t)atoi(t);
	memset(t, 0, 3);
	memcpy(t, idstr+2, 2);
	ent.yy = (uint8_t)atoi(t);
	memset(t, 0, 3);
	memcpy(t, idstr+4, 2);
	ent.mm = (uint8_t)atoi(t);
	memset(t, 0, 3);
	memcpy(t, idstr+6, 2);
	ent.dd = (uint8_t)atoi(t);
	memset(t, 0, 3);
	memcpy(t, idstr+8, 2);
	ent.hh = (uint8_t)atoi(t);
	memset(t, 0, 3);
	memcpy(t, idstr+10, 2);
	ent.min = (uint8_t)atoi(t);
	memset(t, 0, 3);
	memcpy(t, idstr+12, 2);
	ent.sec = (uint8_t)atoi(t);
	ent.reserved = 0;
	*/

	lastRC = auditRec(password, &ent, userid);
	return lastRC;
}

uint8_t DtaDevOpal::activate(char * password)
{
	uint8_t lastRC;

	if ((lastRC = getDefaultPassword()) != 0) {
		LOG(E) << "Unable to read MSID password " << dev;
		return lastRC;
	}
	char * newpassword = password; 
	if ((lastRC = setSIDPassword((char *)response.getString(4).c_str(), newpassword, 0)) != 0) {
		LOG(E) << "takeOwnership failed " << dev;
		return lastRC;
	}

	if ((lastRC = activateLockingSP(password)) != 0) {
		LOG(E) << "Unable to activate LockingSP with default MSID " << dev;
		return lastRC;
	}
	return lastRC;
}

uint8_t DtaDevOpal::getmfgstate()
{
	LOG(D1) << "Entering DtaDevOpal::getmfgstate() " << dev;
	uint8_t lastRC;
	vector<uint8_t> LR;

	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object " << dev;
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
		LOG(E) << "Unable to determine LockingSP Lifecycle state " << dev;
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
		LOG(E) << "Unable to determine LockingSP Lifecycle state " << dev;
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
	LOG(D1) << "Exiting DtaDevOpal::getmfgstate() " << dev;
	return 0;
}

uint8_t DtaDevOpal::DataStoreWrite(char * password, char * userid, char * filename, uint8_t dsnum, uint32_t startpos, uint32_t len)
{
    //#if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__)
    //LOG(D1) << "DtaDevOpal::DataStoreWrite() isn't supported in Linux";
	//return 0;
    //#else
	LOG(D1) << "Entering DtaDevOpal::DataStoreWrite() " << dev;

	ifstream datafile;
	vector <uint8_t> bufferA(14336,0); // (16384, 0x00); // (8192, 0x66); // 0 buffer  (57344, 0x00),
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
	uint32_t blockSize = 14336;//  15360; // 16384;  // 57344; // 57344=512*112=E000h 1950=0x79E; 16384=512*32 for 17K MaxComPacketSize, 15360=512*30
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
		LOG(E) << "Unable to create command object " << dev;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}

	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object " << dev;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	LOG(D1) << "start lockingSP session";
	//if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
	vector<uint8_t> auth,auth2,auth3;
	auth = getUID(userid,auth2,auth3,disk_info.OPAL20_numUsers); // pass vector directly, not enum index of vector table

	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, auth)) != 0) {
		delete cmd;
		delete session;
		return lastRC;
	}
	/* temp out due to MX300 limitation
	LOG(D1) << "Start transaction";
	cmd->reset();
	cmd->addToken(OPAL_TOKEN::STARTTRANSACTON);
	cmd->addToken(OPAL_TOKEN::WHERE);
	cmd->complete();
	if ((lastRC = session->sendCommand(cmd, response)) != 0) {
		delete cmd;
		delete session;
		return lastRC;
	} */
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
		vector<uint8_t> DstoreUid;
		DstoreUid.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
		for (uint8_t i = 0; i<7; i++) {
			DstoreUid.push_back(OPALUID[OPAL_UID::OPAL_DATA_STORE][i]);
		}
		if (dsnum < 2)
			DstoreUid.push_back(0);
		else
			DstoreUid.push_back(dsnum);
		//printf("DstoreUid= ");
		//for (uint8_t i = 0; i < 8; i++) {
		//	printf("%02X ", DstoreUid.at(i));
		//} printf("\n");

		cmd->reset(OPAL_UID::OPAL_DATA_STORE, OPAL_METHOD::SET);
		if (dsnum > 1)
			cmd->changeInvokingUid(DstoreUid);

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
			LOG(E) << "DataStoreWrite Error " << dev;
			datafile.close();
			return lastRC;
		}
		filepos += newSize;
	}
	printf("\r%s %i bytes written \n", progress_bar, filepos);
	/* temp out due to MX300 limitation
	LOG(D1) << "end transaction";
	cmd->reset();
	cmd->addToken(OPAL_TOKEN::ENDTRANSACTON);
	cmd->addToken(OPAL_TOKEN::WHERE);
	cmd->complete();
	if ((lastRC = session->sendCommand(cmd, response)) != 0) {
		delete cmd;
		delete session;
		return lastRC;
	}*/
	datafile.close();
	delete cmd;
	delete session;
	LOG(I) << "Data Store file  " << filename << " written to " << dev;
	LOG(D1) << "Exiting DtaDevOpal::DataStoreWrite() " << dev;
	return 0;
	//#endif

}

uint8_t DtaDevOpal::DataStoreRead(char * password, char * userid, char * filename, uint8_t dsnum, uint32_t startpos, uint32_t len)
{
        //#if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__)
        //LOG(D1) << "DtaDevOpal::DataStoreRead() isn't supported in Linux";
	//return 0;
        //#else
	LOG(D1) << "Entering DtaDevOpal::DataStoreRead() " << dev;

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
	uint32_t blockSize = 14336; // 15360; //16384; // 57344; // 4096;// 57344; // 57344=512*112=E000h 1950=0x79E;
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
		LOG(E) << "Unable to open Data file " << filename << dev ;
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
		LOG(E) << "Unable to create command object " << dev;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}

	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object " << dev;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	LOG(D1) << "start lockingSP session";
	//if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
	//if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, getusermode() ? OPAL_UID::OPAL_USER1_UID : OPAL_UID::OPAL_ADMIN1_UID)) != 0) { // JERRY TEST User 1 
	vector<uint8_t> auth,auth2,auth3;
	auth = getUID(userid,auth2,auth3,disk_info.OPAL20_numUsers); // pass vector directly, not enum index of vector table
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, auth)) != 0) { 
		delete cmd;
		delete session;
		LOG(E) << "DataStore Read Unable to start session Error" << dev;
		datafile.close();
		free(buffer);
		return lastRC;
	}

	//LOG(I) << "Read Data store from " << dev << " to " << filename;
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

		LOG(D1) << "***** start read datastore";
		vector<uint8_t> DstoreUid, getUid;
		DstoreUid.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
		for (uint8_t i=0;i<7;i++) {
			DstoreUid.push_back(OPALUID[OPAL_UID::OPAL_DATA_STORE][i]);
		}
		if (dsnum < 2)
			DstoreUid.push_back(0);
		else
			DstoreUid.push_back(dsnum);
		//printf("DstoreUid= ");
		//for (uint8_t i = 0; i < 8; i++) {
		//	printf("%02X ", DstoreUid.at(i));
		//} printf("\n");

		cmd->reset(OPAL_UID::OPAL_DATA_STORE, OPAL_METHOD::GET);
		if (dsnum > 1)
			cmd->changeInvokingUid(DstoreUid);

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
			LOG(E) << "DataStore Read Error " << dev;
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
			LOG(E) << "DataStore Read Error " << dev;
			datafile.close();
			free(buffer);
			delete cmd;
			delete session;
			return lastRC;
		}
		datafile.write(buffer, newSize);
		if (datafile.fail())
		{
			LOG(E) << "Saving datafile error " << dev;
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

	LOG(I) << "Read Data Store from "<< dev << " to " << filename << " OK";
	LOG(D1) << "Exiting DtaDevOpal::DataStoreRead() " << dev;
	return 0;
	//#endif
}

uint8_t DtaDevOpal::MBRRead(char * password, char * filename, uint32_t startpos, uint32_t len)
{
	LOG(D1) << "Entering DtaDevOpal::MBRRead() with filename " << dev;
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
	uint32_t blockSize = 14336; // 15360; // 16384;  // 57344; // 4096;// 57344; // 57344=512*112=E000h 1950=0x79E;
	uint32_t filepos = 0;
	uint32_t newSize;
	uint32_t maxMBRSize;

#if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__)
#else
	printf("startpos=%ld len=%ld\n", startpos, len); // linux has error
#endif
	if ((lastRC = getMBRsize(password, &maxMBRSize))!=0)
	{
		LOG(E) << " Can not get MBR table size " << dev;
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
		LOG(E) << "Unable to create command object " << dev;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}

	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object " << dev;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	LOG(D1) << "start lockingSP session";
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete cmd;
		delete session;
		LOG(E) << "DataStore Read Error" << dev;
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
			LOG(E) << "MBR Read Error " << dev;
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
			LOG(E) << "MBR Read Error " << dev;
			datafile.close();
			free(buffer);
			delete cmd;
			delete session;
			return lastRC;
		}
		datafile.write(buffer, newSize);
		if (datafile.fail())
		{
			LOG(E) << "Saving datafile error " << dev;
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

	LOG(I) << "Read MBR from " << dev << " to " << filename << " " << dev;
	LOG(D1) << "Exiting DtaDevOpal::MBRRead() " << dev;
	return 0;

}


uint8_t DtaDevOpal::getMBRsize(char * password, uint32_t * msize)
{
	LOG(D1) << "Entering DtaDevOpal::getMBRsize() " << dev;

	uint8_t lastRC;
	vector<uint8_t> LR;

	LR.clear();
	LR.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		LR.push_back(OPALUID[OPAL_UID::OPAL_MBR_SZ][i]);
	}

	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object " << dev;
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
	LOG(D1) << "Entering DtaDevOpal::getMBRsize() " << dev;

	uint8_t lastRC;
	vector<uint8_t> LR;

	LR.clear();
	LR.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		LR.push_back(OPALUID[OPAL_UID::OPAL_MBR_SZ][i]);
	}

	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object " << dev;
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
	uint32_t RecommendedAccessGranularity = response.getUint32(8);
	printf("RecommendedAccessGranularity 0x%X\n", RecommendedAccessGranularity);

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
	////////////////////////////////////////////////////////////////////////

/*
	uint32_t n;
	OPAL_TOKEN tkn;
	uint16_t col1,col2;

	col1 = 5; // TryLimit column
	col2 = 6; // Tries colume
	lastRC = getTryLimit(col1,col2,password);
	if (lastRC) {
		LOG(E) << "unable to read TryLimit";
		return lastRC;
	}

	n = response.getTokenCount();
	printf("OPAL_TOKEN count is %d\n", n);

	for (uint32_t i = 0; i < n; i++)
	{
		tkn = response.tokenIs(i);
		printf("OPAL_TOKEN %i is %d(%02Xh)\n", i, tkn, tkn);
	}

	uint8_t trLmt = response.getUint8(4);
	uint8_t tys = response.getUint8(8);
	fprintf(stdout, "TryLimit: %d ;  trys: %d\n",trLmt, tys );
*/	

	getTryLimit(5, 6, password); // list all TryLimit Tries


	////////////////////////////////////////////////////////////////////////////////

	//delete session;
	// 
	//srand((uint16_t)time(NULL));
	//auditRec(password, 1 + rand() % evt_lastID); // for testing purpose
	LOG(D1) << "Exiting DtaDevOpal::getMBRsize()" << dev;
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
	LOG(D1) << "Exiting DtaDevOpal::createUSB() " << dev;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//  above code is for createUSB 
//////////////////////////////////////////////////////////////////////////////////////////////


uint8_t DtaDevOpal::loadPBA(char * password, char * filename) {
    #if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__) || (WINDOWS7)
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
	uint32_t blockSize; // = 16384; // 57344; // 57344=512*112=E000h 1950=0x79E;  16384=512*32=0x4000
	uint32_t filepos = 0;
	uint64_t sz;
	ifstream pbafile;
	ofstream progfile;
	char progbuf[50];
	// for decompression
	PBYTE DecompressedBuffer = NULL;
	//uint64_t DecompressedBufferSize = NULL;
	SIZE_T DecompressedBufferSize = NULL;
	PBYTE CompressedBuffer = NULL;
	uint64_t CompressedBufferSize = 0;
	DECOMPRESSOR_HANDLE Decompressor = NULL;
	DecompressedBuffer = NULL;
	BOOL Success;
	SIZE_T  DecompressedDataSize;
	void * somebuf = NULL;

	vector <uint8_t> buffer; // 0 buffer  (57344, 0x00),
	vector <uint8_t> lengthtoken;

	adj_host_prop(1);
	if (adj_host == 1)
		blockSize = 57344; // 57344=512*112=E000h 1950=0x79E;  16384=512*32=0x4000
	else
		blockSize = 14336;//  15360; // 16384;
	lengthtoken.clear();
	lengthtoken.push_back(0xe2); // E2 is byte string which mean the followind data is byte-stream, but for read, there is no byte string so it should be E0
	lengthtoken.push_back(0x00);
	lengthtoken.push_back((uint8_t)(blockSize >> 8)); // (0xE0); // 
	lengthtoken.push_back((uint8_t)(blockSize & 0xFF));// (0x00); // 
	if (embed == 0) {
		pbafile.open(filename, ios::in | ios::binary);
		if (!pbafile) {
			adj_host_prop(2); // reset host properties to smaller size
			LOG(E) << "Unable to open PBA image file " << filename;
			return DTAERROR_OPEN_ERR;
		}
		pbafile.seekg(0, pbafile.end);
		fivepercent = (uint64_t)((pbafile.tellg() / 20) / blockSize) * blockSize;
	}
	else {
		const char * fname[] = { "sedutil-cli.exe" , "prog"}; // , "..\\rc\\sedutil.exe", "..\\rc\\UEFI.img"	};
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
				adj_host_prop(2); // reset host properties to smaller size
				LOG(E) << "Cannot allocate memory for compressed buffer.";
				return DTAERROR_OPEN_ERR;
			}
			pbafile.read((char *)CompressedBuffer, CompressedBufferSize); // read all img data
            
			Success = CreateDecompressor( // nozero = OK; 0 : NG
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

		if (!Success || !somebuf)
		{
			adj_host_prop(2); // reset host properties to smaller size
			return DTAERROR_OPEN_ERR;
		}

		fivepercent = (uint64_t)((DecompressedBufferSize / 20) / blockSize) * blockSize;
	}
	// change FAT uuid and disk label
	UUID uuid;
	uint8_t struuid[64];
	vector <uint8_t> uu = ugenv(uuid, struuid);
	for (uint8_t i = 0; i < 4; i++) { DecompressedBuffer[0x100027 + i] = uu.at(i); DecompressedBuffer[0x27 + i] = uu.at(i);
		DecompressedBuffer[0x1b8 + i] = uu.at(i);
	}
	for (uint8_t i = 0; i < 11; i++) { DecompressedBuffer[0x10002b + i] = struuid[i]; DecompressedBuffer[0x2b + i] = struuid[i];
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
	DtaHashPwd(hash, sernum, this,1000);
	LOG(D1) << "end hashing";
	//////IFLOG(D1) printf("hashed size = %zd\n", hash.size());
	//////IFLOG(D1) printf("hashed serial number is ");
	//////IFLOG(D1) // should never expose the hashed series , need to comment out when release
	//////for (uint8_t i = 0; i < hash.size(); i++)
	//////{
	//////	printf("%02X", hash.at(i));
	//////}
	//////printf("\n"); // end of IFLOG(D1)
	// try dump decompressed buffer of sector 0 , 1 
	//DtaHexDump(DecompressedBuffer + 512, 512);
	// write 32-byte date into buffer 
	for (uint8_t i = 2; i < hash.size(); i++)
	{
		DecompressedBuffer[512 + 64 + i - 2] = hash.at(i);
	}

	hash.clear();
	LOG(D1) << "start hashing";
	char mbrstr[16] = { 'F','i','d','e','l','i','t','y','L','o','c','k','M', 'B', 'R', }; // "FidelityLockMBR";
	DtaHashPwd(hash, mbrstr, this,1000); // why IFLOG(D4)
	for (uint8_t i = 2; i < hash.size(); i++)
	{
		DecompressedBuffer[512 + 96 + i - 2] = hash.at(i);
	}

	// write license level 
	hash.clear();
	//LOG(D1) << "start hashing license level";
	//uint8_t idx[16];
	//char st1[16];

	char sbnk[16] = { ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ', ' ', ' ', ' ', };
	char lic_level[18];
	memset(lic_level, 0, 18);

	if (!memcmp("0:", LicenseLevel, 2)) { // correct feature set
		setlic(lic_level, LicenseLevel);
	}
	else {
		memcpy(lic_level, sbnk, 16);
		printf("no license = %s\n", lic_level);
	}

	//IFLOG(D4) 
	//	for (uint8_t i = 0; i < 16; i++) { printf("%02X", lic_level[i]); };
	hash.clear();
	DtaHashPwd(hash, lic_level, this,1000);
	for (uint8_t i = 2; i < hash.size(); i++)
	{
		DecompressedBuffer[512 + 128 + i - 2] = hash.at(i);
	}
	IFLOG(D4)  // remove hexdump when release 
		DtaHexDump(DecompressedBuffer + 512, 512);

	no_hash_passwords = saved_flag;

	if (0 == fivepercent) fivepercent++;
	if (embed ==0) 
		pbafile.seekg(0, pbafile.beg);

	DtaCommand *cmd = new DtaCommand();
	if (NULL == cmd) {
		LOG(E) << "Unable to create command object " << dev;
		adj_host_prop(2); // reset host properties to smaller size
		return DTAERROR_OBJECT_CREATE_FAILED;
	}

	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object " << dev;
		adj_host_prop(2); // reset host properties to smaller size
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete cmd;
		delete session;
		pbafile.close();
		adj_host_prop(2); // reset host properties to smaller size
		return lastRC;
	}
	/* 
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
	} */

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
							adj_host_prop(2); // reset host properties to smaller size
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
			printf("\r%s %i(%I64d) %s", progress_bar,filepos, DecompressedBufferSize, dev);
			fflush(stdout);
			// open progress output file
			progfile.open(sernum, ios::out);
			memset(progbuf, 0,50);
			sprintf(progbuf,"\r%i(%I64d)", filepos, DecompressedBufferSize);
			progfile.write(progbuf, strlen(progbuf));
			progfile.close();
		}
		int rty;
		rty = 0;
	rty1:
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
			rty += 1;
			if (rty < 3) goto rty1;
			delete cmd;
			delete session;
			pbafile.close();
			adj_host_prop(2); // reset host properties to smaller size
			return lastRC;
		}

		// do retry here 

		filepos += blockSize;
		if (filepos > DecompressedBufferSize)
		{
			break;
		}
	} // end of while 

	printf("\r%s %i(%I64d) bytes written \n", progress_bar, filepos, DecompressedBufferSize);
	// open progress output file
	progfile.open(sernum, ios::out);
	memset(progbuf, 0, 50);
	sprintf(progbuf, "\r%i(%I64d) Complete PBA write", filepos, DecompressedBufferSize);
	progfile.write(progbuf, strlen(progbuf));
	progfile.close();

/*	LOG(D1) << "end transaction";
	cmd->reset();
	cmd->addToken(OPAL_TOKEN::ENDTRANSACTON);
	cmd->addToken(OPAL_TOKEN::WHERE);
	cmd->complete(); 
	if ((lastRC = session->sendCommand(cmd, response)) != 0) {
		delete cmd;
		delete session;
		pbafile.close();
		adj_host_prop(2); // reset host properties to smaller size
		return lastRC;
	} */
	delete cmd;
	delete session;
	pbafile.close();
	if (embed ==0)
		LOG(I) << "PBA image  " << filename << " written to " << dev;
	else {
		LOG(I) << "PBA image written to " << dev;

	}
        #endif
	adj_host_prop(2); // reset host properties to smaller size
	LOG(D1) << "Exiting DtaDevOpal::loadPBAimage() " << dev;

	return 0;
}

uint8_t DtaDevOpal::activateLockingSP(char * password)
{
	LOG(D1) << "Entering DtaDevOpal::activateLockingSP() " << dev;
	uint8_t lastRC;
	vector<uint8_t> table;
	table. push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++) {
		table.push_back(OPALUID[OPAL_UID::OPAL_LOCKINGSP_UID][i]);
	}
	DtaCommand *cmd = new DtaCommand();
	if (NULL == cmd) {
		LOG(E) << "Unable to create command object " << dev;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object " << dev;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_ADMINSP_UID, password, OPAL_UID::OPAL_SID_UID)) != 0) {
		delete cmd;
		delete session;
		return lastRC;
	}
	if ((lastRC = getTable(table, 0x06, 0x06)) != 0) {
		LOG(E) << "Unable to determine LockingSP Lifecycle state " << dev;
		delete cmd;
		delete session;
		return lastRC;
	}
	if ((0x06 != response.getUint8(3)) || // getlifecycle
		(0x08 != response.getUint8(4))) // Manufactured-Inactive
	{
		LOG(E) << "Locking SP lifecycle is not Manufactured-Inactive " << dev;
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
	LOG(I) << "Locking SP Activate Complete " << dev;

	delete cmd;
	delete session;
	LOG(D1) << "Exiting DtaDevOpal::activatLockingSP() " << dev;
	return 0;
}

uint8_t DtaDevOpal::activateLockingSP_SUM(uint8_t lockingrange, char * password)
{
	LOG(D1) << "Entering DtaDevOpal::activateLockingSP_SUM() " << dev;
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
		LOG(E) << "Unable to create command object " << dev;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object " << dev;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_ADMINSP_UID, password, OPAL_UID::OPAL_SID_UID)) != 0) {
		LOG(E) << "session->start failed with code " << lastRC << dev;
		delete cmd;
		delete session;
		return lastRC;
	}
	if ((lastRC = getTable(table, 0x06, 0x06)) != 0) {
		LOG(E) << "Unable to determine LockingSP Lifecycle state" << dev;
		delete cmd;
		delete session;
		return lastRC;
	}
	if ((0x06 != response.getUint8(3)) || // getlifecycle
		(0x08 != response.getUint8(4))) // Manufactured-Inactive
	{
		LOG(E) << "Locking SP lifecycle is not Manufactured-Inactive" << dev;
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
		LOG(E) << "session->sendCommand failed with code " << lastRC << dev;
		delete cmd;
		delete session;
		return lastRC;
	}
	disk_info.Locking_lockingEnabled = 1;
	LOG(I) << "Locking SP Activate Complete for single User" << (lockingrange+1) << " on locking range " << (int)lockingrange;

	delete cmd;
	delete session;
	LOG(D1) << "Exiting DtaDevOpal::activateLockingSP_SUM() " << dev;
	return 0;
}

uint8_t DtaDevOpal::eraseLockingRange_SUM(uint8_t lockingrange, char * password)
{
	uint8_t lastRC;
	LOG(D1) << "Entering DtaDevOpal::eraseLockingRange_SUM " << dev;
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
		LOG(E) << "Unable to create session object " << dev;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete session;
		return lastRC;
	}

	DtaCommand *cmd = new DtaCommand();
	if (NULL == cmd) {
		LOG(E) << "Unable to create command object " << dev;
		delete session;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	cmd->reset(OPAL_UID::OPAL_AUTHORITY_TABLE, OPAL_METHOD::ERASE);
	cmd->changeInvokingUid(LR);
	cmd->addToken(OPAL_TOKEN::STARTLIST);
	cmd->addToken(OPAL_TOKEN::ENDLIST);
	cmd->complete();
	if ((lastRC = session->sendCommand(cmd, response)) != 0) {
		LOG(E) << "setLockingRange Failed " << dev;
		delete cmd;
		delete session;
		return lastRC;
	}
	delete cmd;
	delete session;
	LOG(I) << "LockingRange" << (uint16_t)lockingrange << " erased";
	LOG(D1) << "Exiting DtaDevOpal::eraseLockingRange_SUM" << dev;
	return 0;
}

uint8_t DtaDevOpal::takeOwnership(char * newpassword)
{
	LOG(D1) << "Entering DtaDevOpal::takeOwnership() " << dev;
	uint8_t lastRC;
	if ((lastRC = getDefaultPassword()) != 0) {
		LOG(E) << "Unable to read MSID password " << dev;
		return lastRC;
	}
	if ((lastRC = setSIDPassword((char *)response.getString(4).c_str(), newpassword, 0)) != 0) {
		LOG(E) << "takeOwnership failed " << dev;
		return lastRC;
	}
	LOG(I) << "takeOwnership complete " << dev;
	LOG(D1) << "Exiting takeOwnership() " << dev;
	return 0;
}
uint8_t DtaDevOpal::getDefaultPassword()
{
	LOG(D1) << "Entering DtaDevOpal::getDefaultPassword() " << dev;
	uint8_t lastRC;
	vector<uint8_t> hash;
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object " << dev;
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
	LOG(D1) << "Exiting getDefaultPassword() " << dev;
	return 0;
}
/////////////////////////////////////////////////////
// get TryLimit and Tries

uint8_t DtaDevOpal::getTryLimit(uint16_t col1,uint16_t col2, char * password)
{
	LOG(D1) << "Entering DtaDevOpal::getTryLimit()" << dev;
	uint8_t lastRC;
	vector<uint8_t> hash;
	vector<uint8_t> table;
	//uint32_t n;
	//OPAL_TOKEN tkn;
	char s[16];
	uint8_t trLmt ;
	uint8_t tys;

	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object " << dev;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	//if ((lastRC = session->start(OPAL_UID::OPAL_ADMINSP_UID)) != 0) {
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		LOG(E) << "Unable to start Unauthenticated session " << dev;
		delete session;
		return lastRC;
	}

	uint8_t lmt; 
	// print TryLimit and Tries for OPAL_C_PIN_ADMIN
	for (uint8_t u = 0; u < 3; u ++) // u=0=>admin  u=1 => user u=2 SID ---> DBG start u=2
	{ 
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
		strcpy_s(s,"Admin");
#else
		strcpy(s, "Admin");
#endif
		switch (u) {
		case 0 :
			lmt = disk_info.OPAL20_numAdmins;
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
			strcpy_s(s,"Admin");
#else
			strcpy(s, "Admin");
#endif
			break;
		case 1 :
			lmt = disk_info.OPAL20_numUsers; 
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
			strcpy_s(s, "User");
#else
			strcpy(s, "User");
#endif
			break;
		case 2 :
			lmt = 1; 
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
			strcpy_s(s, "SID");
#else
			strcpy(s, "SID");
#endif
			break;
		}
		for (uint16_t admin = 1; admin <= lmt; admin++)
		{ 
			if (u == 2) {
				delete session;
				session = new DtaSession(this);
				if (NULL == session) {
					LOG(E) << "Unable to create session object " << dev;
					return DTAERROR_OBJECT_CREATE_FAILED;
				}
				//if ((lastRC = session->start(OPAL_UID::OPAL_ADMINSP_UID)) != 0) {
				//if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {

				if ((lastRC = session->start(OPAL_UID::OPAL_ADMINSP_UID, password, OPAL_UID::OPAL_SID_UID)) != 0) { // for SID TryLimit, it need to start its own session 
				// OPAL_SID_UID 
					LOG(E) << "Unable to start Unauthenticated session " << dev;
					delete session;
					return lastRC;
				}
			}
			//printf("C_PIN_%s%d start column = %d ; end column = %d\n", s, admin, col1, col2);
			table.clear();
			table.push_back(OPAL_SHORT_ATOM::BYTESTRING8);

			for (int i = 0; i < 7; i++) { //A3
				switch (u)
				{ 
				case 0 :
					table.push_back(OPALUID[(OPAL_UID::OPAL_C_PIN_ADMIN1)][i]); //OPAL_C_PIN_SID->NG OPAL_C_PIN_ADMIN1->NG
					break;
				case 1 :
					table.push_back(OPALUID[(OPAL_UID::OPAL_C_PIN_USER1)][i]);
					break;
				case 2 :
					table.push_back(OPALUID[(OPAL_UID::OPAL_C_PIN_SID)][i]); //OPAL_C_PIN_SID->NG OPAL_C_PIN_ADMIN1->NG
					break;
				}
			} 
			if (u < 2 ) table.push_back(admin); else table.push_back(OPALUID[(OPAL_UID::OPAL_C_PIN_SID)][7]);
			if ((lastRC = getTable(table, col1, col2)) != 0) { // TryLimit, TryLimit
				delete session;
				return lastRC;
			}
		/*
		n = response.getTokenCount();
		printf("OPAL_TOKEN count is %d\n", n);

		for (uint32_t i = 0; i < n; i++)
		{
			tkn = response.tokenIs(i);
			printf("OPAL_TOKEN %i is %d(%02Xh)\n", i, tkn, tkn);
		}
		*/
			trLmt = response.getUint8(4);
			tys = response.getUint8(8);
			if (u < 2)
				fprintf(stdout, "%s%d TryLimit = %d : Tries = %d\n", s, admin, trLmt, tys);
			else
				fprintf(stdout, "%s TryLimit = %d : Tries = %d\n", s, trLmt, tys);
		} 
	} 

	delete session;
	LOG(D1) << "Exiting getTryLimit() " << dev;
	return 0;
}

uint8_t DtaDevOpal::printDefaultPassword()
{
    /*const*/ uint8_t rc = getDefaultPassword();
	if (rc) {
		LOG(E) << "unable to read MSID password " << dev;
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
	LOG(D1) << "Entering DtaDevOpal::setSIDPassword() " << dev;
	uint8_t lastRC;
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object " << dev;
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
		LOG(E) << "Unable to set new SID password " << dev;
		delete session;
		return lastRC;
	}
	delete session;
	//auditRec(newpassword, evt_PasswordChangedSID);
	LOG(I) << "set SID password completed " << dev;
	LOG(D1) << "Exiting DtaDevOpal::setSIDPassword() " << dev;
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
	LOG(D1) << "Entering DtaDevOpal::setTable" << dev;
	uint8_t lastRC;
	DtaCommand *set = new DtaCommand();
	if (NULL == set) {
		LOG(E) << "Unable to create command object " << dev;
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
		LOG(E) << "Set table Failed " << dev;
		delete set;
		return lastRC;
	}
	delete set;
	LOG(D1) << "Leaving DtaDevOpal::setTable " << dev;
	return 0;
}
uint8_t DtaDevOpal::getTable(vector<uint8_t> table, uint16_t startcol, 
	uint16_t endcol)
{
	LOG(D1) << "Entering DtaDevOpal::getTable " << dev;
	uint8_t lastRC;
	DtaCommand *get = new DtaCommand();
	if (NULL == get) {
		LOG(E) << "Unable to create command object " << dev;
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
	LOG(D1) << "Entering DtaDevOpal::exec" << dev;
    LOG(D3) << endl << "Dumping command buffer";
    IFLOG(D3) DtaHexDump(cmd->getCmdBuffer(), SWAP32(hdr->cp.length) + sizeof (OPALComPacket));
	LOG(D1) << "Entering DtaDevOpal::exec sendCmd(IF_SEND, IO_BUFFER_LENGTH)";
    //if((lastRC = sendCmd(IF_SEND, protocol, comID(), cmd->getCmdBuffer(), IO_BUFFER_LENGTH)) != 0) {
	#if 0
	if (adj_host == 1) {
		LOG(I) << "adj_host = 1, use Host_sz_MaxComPacketSize";
		printf("Host_sz_MaxComPacketSize = %ld\n", Host_sz_MaxComPacketSize);
	}
	#endif
	if ((lastRC = sendCmd(IF_SEND, protocol, comID(), cmd->getCmdBuffer(), (adj_host == 1)? Host_sz_MaxComPacketSize : IO_BUFFER_LENGTH)) != 0) { // JERRY
		LOG(E) << "Command failed on send " << (uint16_t) lastRC << dev;
        return lastRC;
    }

	/* if TperReset, no response cmd to send */
	if ((protocol == 0x02) && (comID() == 0x0004)) {
		LOG(I) << "TperReset, No reponse command after all";
		return 0;
	}


    hdr = (OPALHeader *) cmd->getRespBuffer();

    do {
        osmsSleep(100); // could it be too fast if multiple drive situation ?????, 25->250 does not help; 25->50 better, ->100
        memset(cmd->getRespBuffer(), 0, IO_BUFFER_LENGTH);
		LOG(D1) << "Entering DtaDevOpal::exec sendCmd(IF_RECV, IO_BUFFER_LENGTH) " << dev ; 
        lastRC = sendCmd(IF_RECV, protocol, comID(), cmd->getRespBuffer(), IO_BUFFER_LENGTH);
		//LOG(I) << "hdr->cp.outstandingData)=" << hdr->cp.outstandingData << " hdr->cp.minTransfer=" << hdr->cp.minTransfer << dev;
	}
    while ((0 != hdr->cp.outstandingData) && (0 == hdr->cp.minTransfer));  // add timer --> advice from Joe
    LOG(D3) << std::endl << "Dumping reply buffer";
    IFLOG(D3) DtaHexDump(cmd->getRespBuffer(), SWAP32(hdr->cp.length) + sizeof (OPALComPacket));
	if (0 != lastRC) {
        LOG(E) << "Command failed on recv" << (uint16_t) lastRC << dev;
        return lastRC;
    }
    resp.init(cmd->getRespBuffer());
    return 0;
}


	
void DtaDevOpal::set_prop(DtaCommand *props ,uint16_t sz_MaxComPacketSize, uint16_t sz_MaxResponseComPacketSize, uint16_t sz_MaxPacketSize, uint16_t sz_MaxIndTokenSize)
{
	props->addToken(OPAL_TOKEN::STARTLIST);
	props->addToken(OPAL_TOKEN::STARTNAME);
	props->addToken(OPAL_TOKEN::HOSTPROPERTIES);
	props->addToken(OPAL_TOKEN::STARTLIST);
	props->addToken(OPAL_TOKEN::STARTNAME);
	props->addToken("MaxComPacketSize");

	props->addToken(sz_MaxComPacketSize);
	//props->addToken(61440);
	props->addToken(OPAL_TOKEN::ENDNAME);
	props->addToken(OPAL_TOKEN::STARTNAME);
	props->addToken("MaxResponseComPacketSize");

	props->addToken(sz_MaxResponseComPacketSize);
	//props->addToken(61440);
	props->addToken(OPAL_TOKEN::ENDNAME);
	props->addToken(OPAL_TOKEN::STARTNAME);
	props->addToken("MaxPacketSize");

	props->addToken(sz_MaxPacketSize);
	//props->addToken(61420);
	props->addToken(OPAL_TOKEN::ENDNAME);
	props->addToken(OPAL_TOKEN::STARTNAME);
	props->addToken("MaxIndTokenSize");
	
	props->addToken(sz_MaxIndTokenSize);
	//props->addToken(61384);
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
}

uint8_t DtaDevOpal::properties()
{
	uint16_t sz_MaxComPacketSize; // = 17408; // 61440;
	uint16_t sz_MaxResponseComPacketSize; // = 17108; //  61440;
	uint16_t sz_MaxPacketSize; // = 17180; // 61440;
	uint16_t sz_MaxIndTokenSize; // = 16384; // 61384;

	LOG(D1) << "Entering DtaDevOpal::properties() " << dev;
	uint8_t lastRC;
	session = new DtaSession(this);  // use the session IO without starting a session
	if (NULL == session) {
		LOG(E) << "Unable to create session object " << dev;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	DtaCommand *props = new DtaCommand(OPAL_UID::OPAL_SMUID_UID, OPAL_METHOD::PROPERTIES);
	if (NULL == props) {
		LOG(E) << "Unable to create command object " << dev;
		delete session;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	if (adj_host == 1) {
		sz_MaxComPacketSize = 61440;
		sz_MaxResponseComPacketSize = 61440;
		sz_MaxPacketSize = 61440;
		sz_MaxIndTokenSize = 61384;
		adj_io_buffer_length = 61440;
	}
	else {
		sz_MaxComPacketSize = 17408; 
		sz_MaxResponseComPacketSize = 17108; 
		sz_MaxPacketSize = 17180; 
		sz_MaxIndTokenSize = 16384;
		adj_io_buffer_length = 17408;
	}
	set_prop(props, sz_MaxComPacketSize, sz_MaxResponseComPacketSize, sz_MaxPacketSize, sz_MaxIndTokenSize);

	//props->complete();
	if ((lastRC = session->sendCommand(props, propertiesResponse)) != 0) {
		delete props;
		return lastRC;
	}
	disk_info.Properties = 1;
	delete props;
	LOG(D1) << "Leaving DtaDevOpal::properties() " << dev;
	return 0;
}

// fill  property 
void DtaDevOpal::fill_prop(uint8_t show)
{
		if (show) cout << std::endl << "TPer Properties: " << dev;
		uint8_t tper_flag;
		tper_flag = 1;
		for (uint32_t i = 0; i < propertiesResponse.getTokenCount(); i++) {
			if (OPAL_TOKEN::STARTNAME == propertiesResponse.tokenIs(i)) {
				if (OPAL_TOKEN::DTA_TOKENID_BYTESTRING != propertiesResponse.tokenIs(i + 1))
				{
					if (show) cout << std::endl << "Host Properties: " << std::endl;
					tper_flag = 0;
				}
				else //
				{//
					if (show) cout << "  " << propertiesResponse.getString(i + 1) << " = " << propertiesResponse.getUint64(i + 2);

					if (!memcmp((propertiesResponse.getString(i + 1)).c_str(), "MaxComPacketSize", sizeof("MaxComPacketSize"))) {
						//LOG(I) << "match MaxComPacketSize";
						if (tper_flag) { // Tper size
							//LOG(I) << "Tper";
							Tper_sz_MaxComPacketSize = propertiesResponse.getUint64(i + 2);
						}
						else { // Host size
							//LOG(I) << "Host";
							Host_sz_MaxComPacketSize = propertiesResponse.getUint64(i + 2);
						}
					}
					else if (!memcmp((propertiesResponse.getString(i + 1)).c_str(), "MaxResponseComPacketSize", sizeof("MaxResponseComPacketSize"))) {
						//LOG(I) << "match MaxResponseComPacketSize";
						if (tper_flag) { // Tper size
							//LOG(I) << "Tper";
							Tper_sz_MaxResponseComPacketSize = propertiesResponse.getUint64(i + 2);
						}
						else { // Host size
							//LOG(I) << "Host";
							Host_sz_MaxResponseComPacketSize = propertiesResponse.getUint64(i + 2);
						}
					}
					else if (!memcmp((propertiesResponse.getString(i + 1)).c_str(), "MaxPacketSize", sizeof("MaxPacketSize"))) {
						//LOG(I) << "match MaxPacketSize";
						if (tper_flag) { // Tper size
							//LOG(I) << "Tper";
							Tper_sz_MaxPacketSize = propertiesResponse.getUint64(i + 2);
						}
						else { // Host size
							//LOG(I) << "Host";
							Host_sz_MaxPacketSize = propertiesResponse.getUint64(i + 2);
						}
					}
					else if (!memcmp((propertiesResponse.getString(i + 1)).c_str(), "MaxIndTokenSize", sizeof("MaxIndTokenSize"))) {
						//LOG(I) << "match MaxIndTokenSize";
						if (tper_flag) { // Tper size
							//LOG(I) << "Tper";
							Tper_sz_MaxIndTokenSize = propertiesResponse.getUint64(i + 2);
						}
						else { // Host size
							//LOG(I) << "Host";
							Host_sz_MaxIndTokenSize = propertiesResponse.getUint64(i + 2);
						}
					}
				} // 
				i += 2;
			}
			if (show) if (!(i % 6)) cout << std::endl;
		}
		#if 0
			printf("Tper_sz_MaxComPacketSize=%ld\n", Tper_sz_MaxComPacketSize);
			printf("Tper_sz_MaxResponseComPacketSize=%ld\n", Tper_sz_MaxResponseComPacketSize);
			printf("Tper_sz_MaxPacketSize=%ld\n", Tper_sz_MaxPacketSize);
			printf("Tper_sz_MaxIndTokenSize=%ld\n", Tper_sz_MaxIndTokenSize);
			printf("Host_sz_MaxComPacketSize=%ld\n", Host_sz_MaxComPacketSize);
			printf("Host_sz_MaxResponseComPacketSize=%ld\n", Host_sz_MaxResponseComPacketSize);
			printf("Host_sz_MaxPacketSize=%ld\n", Host_sz_MaxPacketSize);
			printf("Host_sz_MaxIndTokenSize=%ld\n", Host_sz_MaxIndTokenSize);
		#endif
}


void DtaDevOpal::puke()
{
	LOG(D1) << "Entering DtaDevOpal::puke() " << dev;
	DtaDev::puke();
	if (disk_info.Properties) {
		fill_prop(TRUE); // fill and display Tper Host property
	} // diskinfo.propery
}

// adjust host property 
//act :0
//act :1 : adjust host property if MaxComPacket > 64 K 
//act :2 : reset host property ; regardless if it has been adjust
void DtaDevOpal::adj_host_prop(uint8_t act)
{
	//LOG(I) << "Enter adj_host_prop";
	fill_prop(FALSE); // JERRY 
	//printf("act =  %d\n", act);
	switch (act) {
	case 0 :
	case 1 :
		if (Tper_sz_MaxComPacketSize > 64 * 1024) {
			//LOG(I) << "adj_host set to 1";
			adj_host = 1;
			properties();
		}
		break;
	case 2 :
		//LOG(I) << "adj_host set to 0";
		adj_host = 0;
		properties();
		break;
	} // switch 
	fill_prop(FALSE); // JERRY must re-stuff the host property because properties() only exchange property with Tper but not set host_sz_Maxxxxxxxx
	//LOG(I) << "Exit adj_host_prop";
}

uint8_t DtaDevOpal::objDump(char *sp, char * auth, char *pass,
	char * objID)
{

	LOG(D1) << "Entering DtaDevEnterprise::objDump";
	LOG(D1) << sp << " " << auth << " " << pass << " " << objID;
	uint8_t lastRC;
	DtaCommand *get = new DtaCommand();
	if (NULL == get) {
		LOG(E) << "Unable to create command object " << dev;
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
		LOG(E) << "Unable to create session object " << dev;
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
		LOG(E) << "Unable to create command object " << dev;
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	cmd->reset(OPAL_UID::OPAL_AUTHORITY_TABLE, method);
	cmd->changeInvokingUid(invokingUID);
	cmd->addToken(parms);
	cmd->complete();
	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object " << dev;
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
