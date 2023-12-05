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
/** Device class for Opal 2.0 SSC
 * also supports the Opal 1.0 SSC
 */

#if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__) || defined(__APPLE__)
#else // Windows
#pragma warning(disable: 4224) //C2224: conversion from int to char , possible loss of data
#pragma warning(disable: 4244) //C4244: 'argument' : conversion from 'uint16_t' to 'uint8_t', possible loss of data
#pragma warning(disable: 4996)
#pragma comment(lib, "rpcrt4.lib")  // UuidCreate - Minimum supported OS Win 2000
#endif

#include "os.h"
#include <log/log.h>

#if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__) || defined(__APPLE__)
#else // Windows
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


using namespace std;

DtaDevOpal::DtaDevOpal()
{
}

DtaDevOpal::~DtaDevOpal()
{
	#if 0
	LOG(D) << "Exit DtaDevOpal()";
	if (adj_host == 1)
	{
		LOG(D) << "adj_host == 1, readjust host propeties back to smaller packet size";
		adj_host = 0;
		properties();
	}
	else {
		LOG(D) << "adj_host == 0, no need readjust host propeties back to smaller packet size";
	}
	LOG(D) << "Exit DtaDevOpal()";
	#endif
}
void DtaDevOpal::init(const char * devref)
{
	DtaDevOS::init(devref);
	adj_host = 0; 
	if(properties() != 0) { LOG(E) << "Properties exchange failed " << dev;}
	else fill_prop(false);
}

#if defined(__APPLE__) && defined(__MACH__)
    /* Apple OSX and iOS (Darwin). ------------------------------ */
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR == 1
    /* iOS in Xcode simulator */

#elif TARGET_OS_IPHONE == 1
    /* iOS on iPhone, iPad, etc. */

#elif TARGET_OS_MAC == 1
    /* OSX */

void DtaDevOpal::init(const char * devref,
          io_registry_entry_t driverService,
          io_connect_t connect)
{
    DtaDevOS::init(devref, driverService, connect);
    adj_host = 0;
    if(properties() != 0) { LOG(E) << "Properties exchange failed " << dev;}
    else fill_prop(false);
}

#endif
#endif  // defined(__APPLE__) && defined(__MACH__)


// create an audit user UserN disk_info.OPAL20_numUsers
//char * DtaDevOpal::gethuser(void);


void DtaDevOpal::gethuser(char * buf)
{	string hUser = "User" + to_string(disk_info.OPAL20_numUsers);
	for (int ii = 0; ii < (int)hUser.size(); ii++)
		buf[ii] = hUser.at(ii);
}

 uint8_t DtaDevOpal::setTperResetEnable(bool enable,char * password)
{
		LOG(D) << "Entering DtaDevOpal::setTPerResetEnable()";
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
		rekey->dumpCommand(); 

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



uint8_t DtaDevOpal::setLockonReset(uint8_t lockingrange, bool enable,
                                   char * password)
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
       //    delete session;
       //    return lastRC;
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
       LOG(D) << "LockOnReset LockingRange : " << (uint16_t)lockingrange << " *** " << dev;
       LOG(D1) << "Exiting DtaDevOpal::SetLockonReset() " << dev;
       return 0;
}


uint8_t DtaDevOpal::setLockonReset(uint8_t lockingrange, bool enable,
                                   vector<uint8_t>HostChallenge)
{

    LOG(D1) << "Entering DtaDevOpal::setLockonReset()";
    
    vector<uint8_t> LR = vUID(OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL);
    if (lockingrange != 0) {
       LOG(D2) << "lockingrange = " << lockingrange;
       LR[6] = 0x03;
       LR[8] = lockingrange;
    }
    
    uint8_t lastRC = WithSimpleSessionCommand(OPAL_UID::OPAL_LOCKINGSP_UID,
                                              HostChallenge,
                                              OPAL_UID::OPAL_ADMIN1_UID,
                                              [LR, enable](DtaCommand * rekey){
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
    });
    
    if (lastRC != 0) {
       LOG(E) << "LockOnReset LockingRange " << lockingrange << " Failed " << dev;
    } else {
        LOG(D) << "LockOnReset LockingRange : " << (uint16_t)lockingrange << " *** " << dev;
        LOG(D1) << "Exiting DtaDevOpal::SetLockonReset() " << dev;
    }
    return lastRC;
}



uint8_t DtaDevOpal::initialSetup(char * password)
{
    LOG(D1) << "Entering initialSetup() " << dev;
    uint8_t lastRC;

    if (!skip_activate) {
        LOG(D) << "skip activateLockingSP";
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
    if ( isOpal2() || isOpal1()  ||  (disk_info.PYRITE_version > 1 ) ) { // Opal2, pyrite 2 support mbr
        if ((lastRC = setMBRDone(1, password)) != 0) {
            LOG(E) << "Initial setup failed - unable to Enable MBR shadow " << dev;
            return lastRC;
        }
        if ((lastRC = setMBREnable(1, password)) != 0) {
            LOG(E) << "Initial setup failed - unable to Enable MBR shadow " << dev;
            return lastRC;
        }
    }
    if ((lastRC = setLockonReset(0, TRUE, password)) != 0) { // enable LOCKING RANGE 0 LOCKonRESET
        LOG(E) << "Initial setup failed - unable to set LOCKONRESET " << dev;
        return lastRC;
    }
    LOG(D1) << "Initial setup of TPer complete on " << dev;
    if (isAnySSC()) {  // ( isOpal1() | isOpal2() || isPyrite() || isOpalite()) { // Opal2 support users, Pyrite support 2 User
        LOG(D) << "setuphuser() " << dev;
        if ((lastRC = setuphuser(password)) != 0) {
            LOG(E) << "setup audit user failed " << dev;
            return lastRC;
        }
    }

    
    LOG(D) << "setup normal user" << dev;
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
    memset(strname, 0, 20);
    strncpy(strname, "USER1", 5);
    if ((lastRC = setPassword(password, buf, strname)) != 0) { // set User1 password as USER1 default
        LOG(E) << "set user password failed " << dev;
        return lastRC;
    }
    LOG(D1) << "Exiting initialSetup() " << dev;
    return 0;
}


uint8_t DtaDevOpal::initialSetup(vector<uint8_t> HostChallenge)
{
    LOG(D1) << "Entering initialSetup() " << dev;
    uint8_t lastRC;

    if (!skip_activate) {
        LOG(D) << "skip activateLockingSP";
        if ((lastRC = takeOwnership(HostChallenge)) != 0) {
            LOG(E) << "Initial setup failed - unable to take ownership " << dev;
            return lastRC;
        }

        if ((lastRC = activateLockingSP(HostChallenge)) != 0) {
            LOG(E) << "Initial setup failed - unable to activate LockingSP " << dev;
            return lastRC;
        }
    }

    if ((lastRC = configureLockingRange(0, DTA_DISABLELOCKING, HostChallenge)) != 0) {
        LOG(E) << "Initial setup failed - unable to configure global locking range " << dev;
        return lastRC;
    }
    if ((lastRC = setLockingRange(0, OPAL_LOCKINGSTATE::READWRITE, HostChallenge)) != 0) {
        LOG(E) << "Initial setup failed - unable to set global locking range RW " << dev;
        return lastRC;
    }
    if ( isOpal2() || isOpal1()  ||  (disk_info.PYRITE_version > 1 ) ) { // Opal2, pyrite 2 support mbr
        if ((lastRC = setMBRDone(1, HostChallenge)) != 0) {
            LOG(E) << "Initial setup failed - unable to Enable MBR shadow " << dev;
            return lastRC;
        }
        if ((lastRC = setMBREnable(1, HostChallenge)) != 0) {
            LOG(E) << "Initial setup failed - unable to Enable MBR shadow " << dev;
            return lastRC;
        }
    }
    if ((lastRC = setLockonReset(0, TRUE, HostChallenge)) != 0) { // enable LOCKING RANGE 0 LOCKonRESET
        LOG(E) << "Initial setup failed - unable to set LOCKONRESET " << dev;
        return lastRC;
    }
    LOG(D1) << "Initial setup of TPer complete on " << dev;
    if (isAnySSC()) {  // ( isOpal1() | isOpal2() || isPyrite() || isOpalite()) { // Opal2 support users, Pyrite support 2 User
        LOG(D) << "setuphuser() " << dev;
        if ((lastRC = setuphuser(HostChallenge)) != 0) {
            LOG(E) << "setup audit user failed " << dev;
            return lastRC;
        }
    }

    
    LOG(D) << "setup normal user" << dev;
    /*
        enableUser(true, password, buf); // true : enable user; false: disable user
        enableUserRead(true, password, buf);

    */
    char buf[5] = { 'U','s','e','r','1' };

    if ((lastRC = enableUser(true, HostChallenge, buf)) != 0) {
        LOG(E) << "enable user failed " << dev;
        return lastRC;
    }
    if ((lastRC = enableUserRead(true, HostChallenge, buf)) != 0) {
        LOG(E) << "enable user read failed " << dev;
        return lastRC;
    }
    char strname[] = "USER1";
    vector<uint8_t> newHostChallenge(strname, strname+strlen(strname));
    
    if ((lastRC = setHostChallenge(HostChallenge, buf, newHostChallenge)) != 0) { // set User1 password as USER1 default
        LOG(E) << "set user host challenge failed " << dev;
        return lastRC;
    }
    LOG(D1) << "Exiting initialSetup() " << dev;
    return 0;
}

uint8_t DtaDevOpal::setuphuser(char * password)
{
    LOG(D) << "Entering setuphuser() " << dev;
    uint8_t lastRC;
    char * buf = (char *)malloc(20);
    memset(buf, 0, 20);
    if (isPyrite() || isOpalite() || isRuby() ) {
        //buf = "User2"; // User2 is the last user of Pyrite
        strncpy(buf, "User2", 5);
    }
    else {
        gethuser(buf);
    }
    if (enableUser(true, password, buf)) { LOG(E) << "enable audit User fail"; }; // true : enable user; false: disable user
    if (enableUserRead(true, password, buf)) { LOG(E) << "enable User access fail"; };
    //char p1[64] = "F0iD2eli81Ty"; //20->12 "pFa0isDs2ewloir81Tdy";
    char p1[80]; // = { 'F','0','i','D','2','e','l','i','8','1','T','y',NULL };
    memset(p1, 0, 80); // zero out pass
    auditpass((unsigned char *)p1);

    #if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__) || defined(__APPLE__)
    strncat((char *)p1, getSerialNum(), sizeof(disk_info.serialNum));
    #else
    strncat_s(p1, getSerialNum(), sizeof(disk_info.serialNum));
    #endif
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
            LOG(D) << "setuphuser() : after hash p1, User9 new hashed password = ";
            for (int ii = 0; ii < (int)(hash.size() - 2); ii += 1) { // first 2 byte of hash vector is header
                printf("%02X", hash[ii+2]);
            }
            printf("  %s \n",  dev);
        }
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
    LOG(D) << "Exiting setuphuser() " << dev;
    return 0;

}




uint8_t DtaDevOpal::setuphuser(vector<uint8_t>HostChallenge)
{
    LOG(D) << "Entering setuphuser() " << dev;
    uint8_t lastRC;
    char buf[20];
    memset(buf, 0, 20);
    if (isPyrite() || isOpalite() || isRuby() ) {
        //buf = "User2"; // User2 is the last user of Pyrite
        strncpy(buf, "User2", 5);
    }
    else {
        gethuser(buf);
    }
    if (enableUser(true, HostChallenge, buf)) { LOG(E) << "enable audit User fail"; }; // true : enable user; false: disable user
    if (enableUserRead(true, HostChallenge, buf)) { LOG(E) << "enable User access fail"; };
    //char p1[64] = "F0iD2eli81Ty"; //20->12 "pFa0isDs2ewloir81Tdy";
    char p1[80]; // = { 'F','0','i','D','2','e','l','i','8','1','T','y',NULL };
    memset(p1, 0, 80); // zero out pass
    auditpass((unsigned char *)p1);

    #if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__) || defined(__APPLE__)
    strncat((char *)p1, getSerialNum(), sizeof(disk_info.serialNum));
    #else
    strncat_s(p1, getSerialNum(), sizeof(disk_info.serialNum));
    #endif
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
            LOG(D) << "setuphuser() : after hash p1, User9 new hashed password = ";
            for (int ii = 0; ii < (int)(hash.size() - 2); ii += 1) { // first 2 byte of hash vector is header
                printf("%02X", hash[ii+2]);
            }
            printf("  %s \n",  dev);
        }
        translate_req = saved_t_flag;
        no_hash_passwords = saved_flag ;
    }
    //
    vector<uint8_t> newHostChallenge(p1, p1+strlen(p1));
    if ((lastRC = setHostChallenge(HostChallenge, buf, newHostChallenge) != 0))
    {
        LOG(E) << "setup h user failed " << dev;
        return lastRC;
    }
    //translate_req = saved_t_flag;
    //no_hash_passwords = saved_flag;
    LOG(D) << "Exiting setuphuser() " << dev;
    return 0;

}





uint8_t DtaDevOpal::setup_SUM(uint8_t lockingrange, uint64_t start, uint64_t length, char *Admin1Password, char * password)
{
	LOG(D1) << "Entering setup_SUM() " << dev;;
	uint8_t lastRC;
	char defaultPW[] = ""; //OPAL defines the default initial User password as 0x00
	string userId;
	userId.append("User");
	userId.append(to_string(lockingrange + 1)); //OPAL defines LR0 to User1, LR1 to User2, etc.

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

	LOG(D) << "Setup of SUM complete on " << dev;
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
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, getusermode() ? OPAL_UID::OPAL_USER1_UID : OPAL_UID::OPAL_ADMIN1_UID)) != 0) { 
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
    uint32_t tc;
	// dump raw token info 
	//tc = response.getTokenCount();
	//if (0) {
	//	printf("***** getTokenCount()=%ld\n", (long)tc);
	//	for (uint32_t i = 0; i < tc; i++) {
	//		printf("token %ld = ", (long)i);
	//		for (uint32_t j = 0; j < response.getRawToken(i).size(); j++)
	//			printf("%02X ", response.getRawToken(i)[j]);
	//		cout << endl;
	//	}
	//}

	if (response.tokenIs(4) != _OPAL_TOKEN::DTA_TOKENID_UINT) {
		LOG(E) << "Unable to determine number of ranges " << dev;
		delete session;
		return DTAERROR_NO_LOCKING_INFO;
	}
	LOG(D) << "Locking Range Configuration for " << dev;
	uint32_t numRanges = response.getUint32(4) + 1;
	for (uint32_t i = 0; i < numRanges; i++){
		if(0 != i) LR[8] = i & 0xff;

		if ((lastRC = getTable(LR, _OPAL_TOKEN::RANGESTART, _OPAL_TOKEN::WRITELOCKED)) != 0) {
			delete session;
			return lastRC;
		}
		
		tc = response.getTokenCount();

		if (tc != 34) { // why ?????
			cout << endl;
			LOG(E) << "token count is wrong. Exit loop " << dev;
			break;
		}

		LR[6] = 0x03;  // non global ranges are 00000802000300nn 
		//LOG(D) << "LR" << i << " Begin " << response.getUint64(4) <<
		cout << "LR" << i << " Begin " << response.getUint64(4) <<
			" for " << response.getUint64(8);
		//LOG(D)	<< "            RLKEna =" << (response.getUint8(12) ? " Y " : " N ") <<
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
	LOG(D) << "LockingRange" << (uint16_t)lockingrange << " starting block " << start <<
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
	LOG(D) << "LockingRange" << (uint16_t)lockingrange << " starting block " << start <<
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
    if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, getusermode() ? OPAL_UID::OPAL_USER1_UID : OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
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
    LOG(D) << "LockingRange" << (uint16_t) lockingrange
        << (enabled ? " enabled " : " disabled ")
        << ((enabled & DTA_READLOCKINGENABLED) ? "ReadLocking" : "")
        << ((enabled == (DTA_WRITELOCKINGENABLED | DTA_READLOCKINGENABLED)) ? "," : "")
        << ((enabled & DTA_WRITELOCKINGENABLED) ? "WriteLocking" : "")
        << " " << dev ;
    LOG(D1) << "Exiting DtaDevOpal::configureLockingRange() " << dev;
    return 0;
}

uint8_t DtaDevOpal::configureLockingRange(uint8_t lockingrange, uint8_t enabled, vector<uint8_t> HostChallenge)
{
    LOG(D1) << "Entering DtaDevOpal::configureLockingRange() " << dev;
    vector<uint8_t> LR = vUID(OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL);
    if (lockingrange != 0) {
        LR[6] = 0x03;
        LR[8] = lockingrange;
    }
    
    uint8_t lastRC = WithSimpleSessionCommand(OPAL_UID::OPAL_LOCKINGSP_UID,
                                              HostChallenge,
                                              (getusermode()
                                                ? OPAL_UID::OPAL_USER1_UID
                                                : OPAL_UID::OPAL_ADMIN1_UID),
                                              [LR, enabled](DtaCommand * set){
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
    });
    if (lastRC  != 0) {
        LOG(E) << "configureLockingRange Failed " << dev;
        return lastRC;
    }

    LOG(D) << "LockingRange" << (uint16_t) lockingrange
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
	LOG(D) << "LockingRange" << (uint16_t)lockingrange << " reKeyed " << dev;
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
	LOG(D) << "LockingRange reKeyed " << dev;
	LOG(D1) << "Exiting DtaDevOpal::rekeyLockingRange_SUM() " << dev;
	return 0;
}
uint8_t DtaDevOpal::setBandsEnabled(int16_t lockingrange, char * password)
{
	if (password == NULL) { LOG(D4) << "Password is NULL"; } // unreferenced formal paramater
	LOG(D1) << "Entering DtaDevOpal::setBandsEnabled()" << lockingrange << " " << dev;
	LOG(D) << "setBandsEnabled is not implemented.  It is not part of the Opal SSC ";
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
    LOG(D) << "Revert LockingSP complete " << dev;
    session->expectAbort();
    delete session;
    LOG(D1) << "Exiting revert DtaDev:LockingSP() " << dev;
    return 0;
}



uint8_t DtaDevOpal::revertLockingSP(vector<uint8_t> HostChallenge, uint8_t keep)
{
    LOG(D1) << "Entering revert DtaDevOpal::revertLockingSP() keep = " << (uint16_t) keep << " " << dev;
    uint8_t lastRC = WithSimpleSessionCommand(OPAL_UID::OPAL_LOCKINGSP_UID,
                                              HostChallenge,
                                              OPAL_UID::OPAL_ADMIN1_UID,
                                              [this, keep](DtaCommand *cmd){
        cmd->reset(OPAL_UID::OPAL_THISSP_UID, OPAL_METHOD::REVERTSP);
        cmd->addToken(OPAL_TOKEN::STARTLIST);
        if (keep) {
            cmd->addToken(OPAL_TOKEN::STARTNAME);
            vector<uint8_t> keepGlobalLocking{0x83,0x06,0x00,0x00};
            cmd->addToken(keepGlobalLocking);
            cmd->addToken(OPAL_TOKEN::OPAL_TRUE);
            cmd->addToken(OPAL_TOKEN::ENDNAME);
        }
        cmd->addToken(OPAL_TOKEN::ENDLIST);
        cmd->complete();
        session->expectAbort();  // TODO: check this
    });
    if (lastRC == 0) {
        // empty list returned so rely on method status
        LOG(D) << "Revert LockingSP complete " << dev;
    }
    LOG(D1) << "Exiting revert DtaDev:LockingSP() " << dev;
    return lastRC;
}


uint8_t DtaDevOpal::eraseLockingRange(uint8_t lockingrange, char * password)
{
	LOG(D1) << "Entering DtaDevOpal::eraseLockingRange()" << lockingrange << " " << dev;
	if (password == NULL) { LOG(D4) << "Referencing formal parameters " << lockingrange; }
	LOG(D) << "eraseLockingRange is not implemented.  It is not part of the Opal SSC ";
	LOG(D1) << "Exiting DtaDevOpal::eraseLockingRange() " << dev;
	return 0;
}
uint8_t DtaDevOpal::getAuth4User(char * userid, uint8_t uidorcpin, vector<uint8_t> &userData)
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
			LOG(E) << "Invalid Userid "; // gabble data << userid;
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
    vector<uint8_t> userCPIN, hash;
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
    free(buf);
    // if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
    if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, getusermode() ? (OPAL_UID)(OPAL_USER1_UID + idx) : OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
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
    //    LOG(D) << "User try set password ";
    //}
    
    LOG(D1) << "Exiting DtaDevOpal::setPassword() " << dev;
    return 0;
}



uint8_t DtaDevOpal::setHostChallenge(vector<uint8_t> currentHostChallenge, char * userid, vector<uint8_t> newHostChallenge)
{
    LOG(D1) << "Entering DtaDevOpal::setHostChallenge " << dev;
    char buf[20];
    bzero(buf, sizeof(buf));
    gethuser(buf);
    int idx=0;
    if (!memcmp(userid , buf, (disk_info.OPAL20_numUsers < 10) ? 5 : 6 ) ) {
        idx = disk_info.OPAL20_numUsers -1 ;
    }
    
    uint8_t lastRC = WithSession([this, currentHostChallenge, idx](){
        return session->start(OPAL_UID::OPAL_LOCKINGSP_UID,
                              currentHostChallenge,
                              getusermode() ? (OPAL_UID)(OPAL_USER1_UID + idx) : OPAL_UID::OPAL_ADMIN1_UID);
    },
                                 [this, userid, newHostChallenge](){
        uint8_t rc;
        vector<uint8_t> userCPIN;
        if ((rc = getAuth4User(userid, 10, userCPIN)) != 0) {
            LOG(E) << "Unable to find user " << userid << " in Authority Table";
            return rc;
        }
        if ((rc = setTable(userCPIN, OPAL_TOKEN::PIN, newHostChallenge)) != 0) {
            LOG(E) << "Unable to set user " << userid << " new host challenge " << dev;
            return rc;
        }
        if (0 == memcmp(userid, "Admin", 5)) { // if admin
            LOG(D1) << "Admin try set password ";
            if ((rc = setLockonReset(0, TRUE, newHostChallenge)) != 0) { // enable LOCKING RANGE 0 LOCKonRESET
                LOG(E) << "failed - unable to set LOCKONRESET " << dev;
                return rc;
            }
        }
        return rc;
    });

    if (lastRC == 0) {
        LOG(D1) << userid << " host challenge changed " << dev;
    }
    LOG(D1) << "Exiting DtaDevOpal::setHostChallenge() " << dev;
    return lastRC;
}






/** Set the host challenge of a locking SP user.
 *   Note that the version above of this method is called setPassword
 * @param currentHostChallenge  current host challenge
 * @param userid the userid whose host challenge is to be changed
 * @param newHostChallenge  value  host challenge is to be changed to
 */
uint8_t setHostChallenge(vector<uint8_t> currentHostChallenge, char * userid, vector<uint8_t> newHostChallenge);






uint8_t DtaDevOpal::setNewPassword_SUM(char * password, char * userid, char * newpassword)
{
	LOG(D1) << "Entering DtaDevOpal::setNewPassword_SUM " << dev;
	uint8_t lastRC; 
	vector<uint8_t> userCPIN, hash;
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
	LOG(D) << userid << " password changed " << dev;
	delete session;
	LOG(D1) << "Exiting DtaDevOpal::setNewPassword_SUM() " << dev;
	return 0; 
}
uint8_t DtaDevOpal::setMBREnable(uint8_t mbrstate,    char * Admin1Password)
{
    LOG(D1) << "Entering DtaDevOpal::setMBREnable " << dev;
    if (disk_info.Locking_MBRshadowingNotSupported) {
        LOG(E) << "SSC device does not support shadow MBR";
        return DTAERROR_INVALID_COMMAND;
    }
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
            LOG(D) << "MBREnable set on " << dev ;
        }
    }
    else {
        if ((lastRC = setLockingSPvalue(OPAL_UID::OPAL_MBRCONTROL, OPAL_TOKEN::MBRENABLE,
                OPAL_TOKEN::OPAL_FALSE, Admin1Password, NULL)) != 0) {
                LOG(E) << "Unable to set setMBREnable off " << dev;
                return lastRC;
            }
        else {
            LOG(D) << "MBREnable set off " << dev;
        }
    }
    LOG(D1) << "Exiting DtaDevOpal::setMBREnable " << dev;
    return 0;
}



uint8_t DtaDevOpal::setMBREnable(uint8_t mbrstate, vector<uint8_t> Admin1HostChallenge)
{
    LOG(D1) << "Entering DtaDevOpal::setMBREnable " << dev;
    if (disk_info.Locking_MBRshadowingNotSupported) {
        LOG(E) << "SSC device does not support shadow MBR";
        return DTAERROR_INVALID_COMMAND;
    }
    uint8_t lastRC;
    // set MBRDone before changing MBREnable so the PBA isn't presented
    if ((lastRC = setMBRDone(1, Admin1HostChallenge)) != 0) {
    LOG(E) << "unable to set MBRDone " << dev;
            return lastRC;
    }
    if (mbrstate) {
        if ((lastRC = setLockingSPvalue(OPAL_UID::OPAL_MBRCONTROL, OPAL_TOKEN::MBRENABLE,
            OPAL_TOKEN::OPAL_TRUE, Admin1HostChallenge, NULL)) != 0) {
            LOG(E) << "Unable to set setMBREnable on " << dev;
            return lastRC;
        }
        else {
            LOG(D) << "MBREnable set on " << dev ;
        }
    }
    else {
        if ((lastRC = setLockingSPvalue(OPAL_UID::OPAL_MBRCONTROL, OPAL_TOKEN::MBRENABLE,
                OPAL_TOKEN::OPAL_FALSE, Admin1HostChallenge, NULL)) != 0) {
                LOG(E) << "Unable to set setMBREnable off " << dev;
                return lastRC;
            }
        else {
            LOG(D) << "MBREnable set off " << dev;
        }
    }
    LOG(D1) << "Exiting DtaDevOpal::setMBREnable " << dev;
    return 0;
}

uint8_t DtaDevOpal::setMBRDone(uint8_t mbrstate, char * Admin1Password)
{
    LOG(D1) << "Entering DtaDevOpal::setMBRDone " << dev;
    if (disk_info.Locking_MBRshadowingNotSupported) {
        LOG(E) << "SSC device does not support shadow MBR";
        return DTAERROR_INVALID_COMMAND;
    }
    uint8_t lastRC;
    if (mbrstate) {
        if ((lastRC = setLockingSPvalue(OPAL_UID::OPAL_MBRCONTROL, OPAL_TOKEN::MBRDONE,
            OPAL_TOKEN::OPAL_TRUE, Admin1Password, NULL)) != 0) {
            LOG(E) << "Unable to set setMBRDone on " << dev;
            return lastRC;
        }
        else {
            LOG(D) << "MBRDone set on " << dev;
        }
    }
    else {
        if ((lastRC = setLockingSPvalue(OPAL_UID::OPAL_MBRCONTROL, OPAL_TOKEN::MBRDONE,
            OPAL_TOKEN::OPAL_FALSE, Admin1Password, NULL)) != 0) {
            LOG(E) << "Unable to set setMBRDone off " << dev;
            return lastRC;
        }
        else {
            LOG(D) << "MBRDone set off " << dev;
        }
    }
    LOG(D1) << "Exiting DtaDevOpal::setMBRDone " << dev;

    return 0;
}



uint8_t DtaDevOpal::setMBRDone(uint8_t state, vector<uint8_t> Admin1HostChallenge)
{
    LOG(D1) << "Entering DtaDevOpal::setMBRDone " << dev;
    if (disk_info.Locking_MBRshadowingNotSupported) {
        LOG(E) << "SSC device does not support shadow MBR";
        return DTAERROR_INVALID_COMMAND;
    }
    uint8_t lastRC;
    if (0!=state) {
        if ((lastRC = setLockingSPvalue(OPAL_UID::OPAL_MBRCONTROL, OPAL_TOKEN::MBRDONE,
            OPAL_TOKEN::OPAL_TRUE, Admin1HostChallenge, NULL)) != 0) {
            LOG(E) << "Unable to set setMBRDone on " << dev;
            return lastRC;
        }
        else {
            LOG(D) << "MBRDone set on " << dev;
        }
    }
    else {
        if ((lastRC = setLockingSPvalue(OPAL_UID::OPAL_MBRCONTROL, OPAL_TOKEN::MBRDONE,
            OPAL_TOKEN::OPAL_FALSE, Admin1HostChallenge, NULL)) != 0) {
            LOG(E) << "Unable to set setMBRDone off " << dev;
            return lastRC;
        }
        else {
            LOG(D) << "MBRDone set off " << dev;
        }
    }
    LOG(D1) << "Exiting DtaDevOpal::setMBRDone " << dev;

    return 0;
}




uint8_t DtaDevOpal::TCGreset(uint8_t mbrstate)
{
	LOG(D) << "Entering DtaDevOpal::TCGreset " << dev;
	switch (mbrstate) {
		case 1 : // mbrstate=1 , Tper Reset;  mbrstate=2, STACK_RESET
			LOG(D) << "TPerReset";
			DtaDev::TperReset();
			break;
		case 2 : 
			LOG(D) << "STACK_RESET";
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
	LOG(D) << "1st dump";
	set->dumpCommand();

	// 8 - transfer len == 0 
	set->complete();
	LOG(D) << "2nd dump";
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

uint8_t DtaDevOpal::__setLockingRange(uint8_t lockingrange, uint8_t lockingstate,
                                      std::function<uint8_t(void)>startSessionFn)
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
    
    vector<uint8_t> LR = vUID(OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL);
    if (lockingrange != 0) {
        LR[6] = 0x03;
        LR[8] = lockingrange;
    }
    
    lastRC = WithSessionCommand(startSessionFn,
                                [LR, readlocked, writelocked]
                                (DtaCommand * set){
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
        });
    
    if (lastRC != 0) {
        LOG(E) << "setLockingRange Failed " << dev;
    } else {
        LOG(D) << "LockingRange" << (uint16_t)lockingrange << " set to " << msg << " " << dev;
        LOG(D1) << "Exiting DtaDevOpal::setLockingRange " << dev;
    }
    return lastRC;
}


uint8_t DtaDevOpal::setLockingRange(uint8_t lockingrange, uint8_t lockingstate,
    char * Admin1Password)
{
    return __setLockingRange(lockingrange, lockingstate,
                             [this, Admin1Password](){
        return session->start(OPAL_UID::OPAL_LOCKINGSP_UID,
                              Admin1Password,
                              getusermode() ? OPAL_UID::OPAL_USER1_UID : OPAL_UID::OPAL_ADMIN1_UID);
        
    });

}


uint8_t DtaDevOpal::setLockingRange(uint8_t lockingrange, uint8_t lockingstate,
                                    vector<uint8_t> Admin1HostChallenge)
{
    return __setLockingRange(lockingrange, lockingstate,
                             [this, Admin1HostChallenge](){
        return session->start(OPAL_UID::OPAL_LOCKINGSP_UID,
                              Admin1HostChallenge,
                              getusermode() ? OPAL_UID::OPAL_USER1_UID : OPAL_UID::OPAL_ADMIN1_UID);
        
    });
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
	LOG(D) << "LockingRange" << (uint16_t)lockingrange << " set to " << msg << " " << dev;
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
    
    if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, getusermode()? OPAL_UID::OPAL_USER1_UID : OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
        delete session;
        return lastRC;
    }
    if ((lastRC = setTable(table, name, value)) != 0) {
        LOG(E) << "Unable to update table " << dev;
        delete session;
        return lastRC;
    }
    if (NULL != msg) {
        LOG(D) << msg;
    }
    
    delete session;
    LOG(D1) << "Exiting DtaDevOpal::setLockingSPvalue() " << dev;
    return 0;
}



uint8_t DtaDevOpal::setLockingSPvalue(OPAL_UID table_uid, OPAL_TOKEN name,
    OPAL_TOKEN value, vector<uint8_t>HostChallenge, char * msg)
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
    
    if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, HostChallenge, getusermode()? OPAL_UID::OPAL_USER1_UID : OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
        delete session;
        return lastRC;
    }
    if ((lastRC = setTable(table, name, value)) != 0) {
        LOG(E) << "Unable to update table " << dev;
        delete session;
        return lastRC;
    }
    if (NULL != msg) {
        LOG(D) << msg;
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

uint8_t DtaDevOpal::enableUser(uint8_t mbrstate, vector<uint8_t> HostChallenge, char * userid)
{
    LOG(D1) << "Entering DtaDevOpal::enableUser " << dev;
    
    uint8_t lastRC = WithSession([this, HostChallenge](){
        return session->start(OPAL_UID::OPAL_LOCKINGSP_UID, HostChallenge, OPAL_UID::OPAL_ADMIN1_UID);
    },
                                 [this, mbrstate, userid](){
        vector<uint8_t> userUID;
        uint8_t rc;
        if ((rc = getAuth4User(userid, 0, userUID)) != 0) {
            LOG(E) << "Unable to find user " << userid << " in Authority Table " << dev;
            return rc;
        }
        if ((rc = setTable(userUID, (OPAL_TOKEN)0x05, mbrstate ? OPAL_TOKEN::OPAL_TRUE : OPAL_TOKEN::OPAL_FALSE)) != 0) {
            LOG(E) << "Unable to enable user " << userid << " " << dev;
            return rc;
        }
        return rc;
    });
    
    if (lastRC == 0) {
        if (mbrstate)
            LOG(D2) << userid << " has been enabled " << dev;
        else
            LOG(D2) << userid << " has been disabled" << dev;
    }
    LOG(D1) << "Exiting DtaDevOpal::enableUser() " << dev;
    return lastRC;
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


uint8_t DtaDevOpal::userAccessEnable(uint8_t mbrstate, OPAL_UID UID, char * userid)
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
	auth = getUID(userid, auth2, auth3, (uint8_t)disk_info.OPAL20_numUsers); // always add audit user to auth3. audit user is added first, the following userid will preserve the audit userid
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
	// Admin1
	cmd->addToken(OPAL_TOKEN::STARTNAME);
	cmd->addToken(OPAL_UID::OPAL_HALF_UID_AUTHORITY_OBJ_REF, 4); //????? how to insert 4-byte here, addToken will insert BYTESTRING4 token
	cmd->addToken(auth2);  
	cmd->addToken(OPAL_TOKEN::ENDNAME);
	//
	cmd->addToken(OPAL_TOKEN::STARTNAME);
	cmd->addToken(OPAL_UID::OPAL_HALF_UID_BOOLEAN_ACE, 4);
	cmd->addToken(mbrstate ? OPAL_TOKEN::VALUES : OPAL_TOKEN::WHERE);
	cmd->addToken(OPAL_TOKEN::ENDNAME);
	// User2 
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
		LOG(D) << "no addition audit user added " << dev;
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
    /*    OPAL_ACE_DataStore_Get_All,
        OPAL_ACE_MBRControl_Set_Done,
        OPAL_ACE_LOCKINGRANGE_RDLOCKED,
        OPAL_ACE_LOCKINGRANGE_WRLOCKED,
    */
    error = 0;
    
    LOG(D1) << "***** " << (mbrstate ? "enable " : "disable ") << "OPAL_ACE_DataStore_Get_All for " << userid;
    error |= userAccessEnable(mbrstate, OPAL_UID::OPAL_ACE_DataStore_Get_All,userid);
    
    LOG(D1) << "***** " << (mbrstate ? "enable " : "disable ") << "OPAL_ACE_DataStore_Set_All for " << userid;
    error |= userAccessEnable(mbrstate, OPAL_UID::OPAL_ACE_DataStore_Set_All, userid);
    
    if (!disk_info.Locking_MBRshadowingNotSupported) {
        LOG(D1) << "***** " << (mbrstate ? "enable " : "disable ") << "OPAL_ACE_MBRControl_Set_Done for " << userid;
        error |= userAccessEnable(mbrstate, OPAL_UID::OPAL_ACE_MBRControl_Set_Done, userid);
    }
    // DO NOT turn on lockingrange 1
    //LOG(D1) << "***** " << (mbrstate ? "enable " : "disable ") << "enable OPAL_ACE_LOCKINGRANGE1_RDLOCKED for " << userid;
    //error |= userAccessEnable(mbrstate, OPAL_UID::OPAL_ACE_LOCKINGRANGE1_RDLOCKED, userid);
    //LOG(D1) << "***** " << (mbrstate ? "enable " : "disable ") << "enable OPAL_ACE_LOCKINGRANGE1_WRLOCKED for " << userid;
    //error |= userAccessEnable(mbrstate, OPAL_UID::OPAL_ACE_LOCKINGRANGE1_WRLOCKED, userid);
    /*
    OPAL_ACE_MBRControl_Set_Enable,
    ACE_Locking_GlobalRange_Get_RangeStartToActiveKey,
    ACE_Locking_GlobalRange_Set_ReadLocked,
    ACE_Locking_GlobalRange_Set_WriteLocked,
    ACE_Locking_GlobalRange_Admin_Set,    // allow to set/reset
    ACE_Locking_GlobalRange_Admin_Start, // allow to set/reset range start and length
    */
    //LOG(D1) << "***** " << (mbrstate ? "enable " : "disable ") << "OPAL_ACE_MBRControl_Set_Enable for " << userid;
    //error |= userAccessEnable(mbrstate, OPAL_UID::OPAL_ACE_MBRControl_Set_Enable, userid); // NG6
    if ( isOpal2() && !( isPyrite() || isOpalite() || isRuby()) ) {
        LOG(D1) << "***** " << (mbrstate ? "enable " : "disable ") << "OPAL_ACE_Locking_GlobalRange_Get_RangeStartToActiveKey for " << userid;
        error |= userAccessEnable(mbrstate, OPAL_UID::OPAL_ACE_Locking_GlobalRange_Get_RangeStartToActiveKey, userid);
    }
    LOG(D1) << "***** " << (mbrstate ? "enable " : "disable ") << "OPAL_ACE_Locking_GlobalRange_Set_ReadLocked for " << userid;
    error |= userAccessEnable(mbrstate, OPAL_UID::OPAL_ACE_Locking_GlobalRange_Set_ReadLocked, userid);
    LOG(D1) << "***** " << (mbrstate ? "enable " : "disable ") << "OPAL_ACE_Locking_GlobalRange_Set_WriteLocked for " << userid;
    error |= userAccessEnable(mbrstate, OPAL_UID::OPAL_ACE_Locking_GlobalRange_Set_WriteLocked, userid);
    //LOG(D1) << "***** " << (mbrstate ? "enable " : "disable ") << "OPAL_ACE_Locking_GlobalRange_Admin_Set for " << userid;
    //error |= userAccessEnable(mbrstate, OPAL_UID::OPAL_ACE_Locking_GlobalRange_Admin_Set, userid); // NG10
    //LOG(D1) << "***** " << (mbrstate ? "enable " : "disable ") << "OPAL_ACE_Locking_GlobalRange_Admin_Start for " << userid;
    //error |= userAccessEnable(mbrstate, OPAL_UID::OPAL_ACE_Locking_GlobalRange_Admin_Start, userid); // NG11

    if (error) {
        LOG(E) << (mbrstate ? "enable " : "disable ") << "one of user accesses fail" << dev;
        delete session;
        return error;
    }

    delete session;
    return 0;
}


uint8_t DtaDevOpal::enableUserRead(uint8_t mbrstate, vector<uint8_t> HostChallenge, char * userid)
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
    if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, HostChallenge, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
        delete session;
        return lastRC;
    }
    if ((lastRC = getAuth4User(userid, 0, userUID)) != 0) {
        LOG(E) << "Unable to find user " << userid << " in Authority Table " << dev;
        delete session;
        return lastRC;
    }
    /*    OPAL_ACE_DataStore_Get_All,
        OPAL_ACE_MBRControl_Set_Done,
        OPAL_ACE_LOCKINGRANGE_RDLOCKED,
        OPAL_ACE_LOCKINGRANGE_WRLOCKED,
    */
    error = 0;
    
    LOG(D1) << "***** " << (mbrstate ? "enable " : "disable ") << "OPAL_ACE_DataStore_Get_All for " << userid;
    error |= userAccessEnable(mbrstate, OPAL_UID::OPAL_ACE_DataStore_Get_All,userid);
    
    LOG(D1) << "***** " << (mbrstate ? "enable " : "disable ") << "OPAL_ACE_DataStore_Set_All for " << userid;
    error |= userAccessEnable(mbrstate, OPAL_UID::OPAL_ACE_DataStore_Set_All, userid);
    
    if (!disk_info.Locking_MBRshadowingNotSupported) {
        LOG(D1) << "***** " << (mbrstate ? "enable " : "disable ") << "OPAL_ACE_MBRControl_Set_Done for " << userid;
        error |= userAccessEnable(mbrstate, OPAL_UID::OPAL_ACE_MBRControl_Set_Done, userid);
    }
    // DO NOT turn on lockingrange 1
    //LOG(D1) << "***** " << (mbrstate ? "enable " : "disable ") << "enable OPAL_ACE_LOCKINGRANGE1_RDLOCKED for " << userid;
    //error |= userAccessEnable(mbrstate, OPAL_UID::OPAL_ACE_LOCKINGRANGE1_RDLOCKED, userid);
    //LOG(D1) << "***** " << (mbrstate ? "enable " : "disable ") << "enable OPAL_ACE_LOCKINGRANGE1_WRLOCKED for " << userid;
    //error |= userAccessEnable(mbrstate, OPAL_UID::OPAL_ACE_LOCKINGRANGE1_WRLOCKED, userid);
    /*
    OPAL_ACE_MBRControl_Set_Enable,
    ACE_Locking_GlobalRange_Get_RangeStartToActiveKey,
    ACE_Locking_GlobalRange_Set_ReadLocked,
    ACE_Locking_GlobalRange_Set_WriteLocked,
    ACE_Locking_GlobalRange_Admin_Set,    // allow to set/reset
    ACE_Locking_GlobalRange_Admin_Start, // allow to set/reset range start and length
    */
    //LOG(D1) << "***** " << (mbrstate ? "enable " : "disable ") << "OPAL_ACE_MBRControl_Set_Enable for " << userid;
    //error |= userAccessEnable(mbrstate, OPAL_UID::OPAL_ACE_MBRControl_Set_Enable, userid); // NG6
    if ( isOpal2() && !( isPyrite() || isOpalite() || isRuby()) ) {
        LOG(D1) << "***** " << (mbrstate ? "enable " : "disable ") << "OPAL_ACE_Locking_GlobalRange_Get_RangeStartToActiveKey for " << userid;
        error |= userAccessEnable(mbrstate, OPAL_UID::OPAL_ACE_Locking_GlobalRange_Get_RangeStartToActiveKey, userid);
    }
    LOG(D1) << "***** " << (mbrstate ? "enable " : "disable ") << "OPAL_ACE_Locking_GlobalRange_Set_ReadLocked for " << userid;
    error |= userAccessEnable(mbrstate, OPAL_UID::OPAL_ACE_Locking_GlobalRange_Set_ReadLocked, userid);
    LOG(D1) << "***** " << (mbrstate ? "enable " : "disable ") << "OPAL_ACE_Locking_GlobalRange_Set_WriteLocked for " << userid;
    error |= userAccessEnable(mbrstate, OPAL_UID::OPAL_ACE_Locking_GlobalRange_Set_WriteLocked, userid);
    //LOG(D1) << "***** " << (mbrstate ? "enable " : "disable ") << "OPAL_ACE_Locking_GlobalRange_Admin_Set for " << userid;
    //error |= userAccessEnable(mbrstate, OPAL_UID::OPAL_ACE_Locking_GlobalRange_Admin_Set, userid); // NG10
    //LOG(D1) << "***** " << (mbrstate ? "enable " : "disable ") << "OPAL_ACE_Locking_GlobalRange_Admin_Start for " << userid;
    //error |= userAccessEnable(mbrstate, OPAL_UID::OPAL_ACE_Locking_GlobalRange_Admin_Start, userid); // NG11

    if (error) {
        LOG(E) << (mbrstate ? "enable " : "disable ") << "one of user accesses fail" << dev;
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
    LOG(D) << "revertTper completed successfully " << dev;
    delete cmd;
    delete session;

    //auditRec(password, evt_Revert);
    LOG(D1) << "Exiting DtaDevOpal::revertTPer() " << dev;
    return 0;
}

uint8_t DtaDevOpal::revertTPer(vector<uint8_t> HostChallenge, uint8_t PSID, uint8_t AdminSP)
{
    LOG(D1) << "Entering DtaDevOpal::revertTPer() " << AdminSP << " " << dev;
    uint8_t lastRC = WithSimpleSessionCommand(OPAL_UID::OPAL_ADMINSP_UID,
                                              HostChallenge,
                                              (PSID ? OPAL_UID::OPAL_PSID_UID : OPAL_UID::OPAL_SID_UID),
                                              [this](DtaCommand * cmd){
        cmd->reset(OPAL_UID::OPAL_ADMINSP_UID, OPAL_METHOD::REVERT);
        cmd->addToken(OPAL_TOKEN::STARTLIST);
        cmd->addToken(OPAL_TOKEN::ENDLIST);
        cmd->complete();
        session->expectAbort();
    });
    if (lastRC == 0) {
        LOG(D) << "revertTper completed successfully " << dev;
    }
    //auditRec(password, evt_Revert);
    LOG(D1) << "Exiting DtaDevOpal::revertTPer() " << dev;
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
	default: return 0xff;  // throw invalid_argument();
	}
}

vector<uint8_t> hex2data_a(char * password)
{
	vector<uint8_t> h;
	h.clear();
	if ((false))
		printf("strlen(password)=%d\n", (int)strlen(password));
	/*
	if (strlen(password) != 16)
	{
		//LOG(D) << "Hashed Password length isn't 64-byte, no translation";
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


///////////////////////////////////////////////////////////////////////////
void SignalHandler(int signal)
{
	printf("Signal %d\n", signal);
	throw "!Access Violation!";
}

////////////////////////////////////////////////////////////////////




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
    LOG(D) << "Locking SP Activate Complete " << dev;

    delete cmd;
    delete session;
    LOG(D1) << "Exiting DtaDevOpal::activatLockingSP() " << dev;
    return 0;
}


uint8_t DtaDevOpal::activateLockingSP(vector<uint8_t> HostChallenge)
{
    LOG(D1) << "Entering DtaDevOpal::activateLockingSP() " << dev;
    vector<uint8_t> LockingSP= vUID(OPAL_UID::OPAL_LOCKINGSP_UID);
    
    uint8_t lastRC = WithSessionCommand(
                                        [this, HostChallenge, LockingSP](){
        uint8_t rc = session->start(OPAL_UID::OPAL_ADMINSP_UID, HostChallenge, OPAL_UID::OPAL_SID_UID);
        if (rc != 0)
            return rc;
        rc = getTable(LockingSP, 0x06, 0x06);
        if (rc != 0) {
            LOG(E) << "Unable to determine LockingSP Lifecycle state " << dev;
            return rc;
        }
        if ((0x06 != response.getUint8(3)) || // getlifecycle
            (0x08 != response.getUint8(4))) // Manufactured-Inactive
        {
            LOG(E) << "Locking SP lifecycle is not Manufactured-Inactive " << dev;
            return (uint8_t)DTAERROR_INVALID_LIFECYCLE;
        }
        return rc;
    },
                                        [](DtaCommand * cmd){
        cmd->reset(OPAL_UID::OPAL_LOCKINGSP_UID, OPAL_METHOD::ACTIVATE);
        cmd->addToken(OPAL_TOKEN::STARTLIST);
        cmd->addToken(OPAL_TOKEN::ENDLIST);
        cmd->complete();
    });
    if (lastRC  != 0) {
    } else {
        LOG(D) << "Locking SP Activate Complete " << dev;
        LOG(D1) << "Exiting DtaDevOpal::activatLockingSP() " << dev;
    }
    return lastRC;
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
	LOG(D) << "Locking SP Activate Complete for single User" << (lockingrange+1) << " on locking range " << (int)lockingrange;

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
	LOG(D) << "LockingRange" << (uint16_t)lockingrange << " erased";
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
    LOG(D) << "takeOwnership complete " << dev;
    LOG(D1) << "Exiting takeOwnership() " << dev;
    return 0;
}

uint8_t DtaDevOpal::takeOwnership(vector<uint8_t> HostChallenge)
{
    LOG(D1) << "Entering DtaDevOpal::takeOwnership() " << dev;
    uint8_t lastRC;
    if ((lastRC = getDefaultPassword()) != 0) {
        LOG(E) << "Unable to read MSID password " << dev;
        return lastRC;
    }
    string defaultPassword = response.getString(4);
    const char * dps = defaultPassword.c_str();
    vector<uint8_t> defaultHostChallenge(dps,dps+strlen(dps));
    if ((lastRC = setSIDHostChallenge(defaultHostChallenge, HostChallenge)) != 0) {
        LOG(E) << "takeOwnership failed " << dev;
        return lastRC;
    }
    LOG(D) << "takeOwnership complete " << dev;
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

	uint8_t lmt = 1;
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
			lmt = (uint8_t)disk_info.OPAL20_numAdmins;
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
			strcpy_s(s,"Admin");
#else
			strcpy(s, "Admin");
#endif
			break;
		case 1 :
			lmt = (uint8_t)disk_info.OPAL20_numUsers;
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
		for (uint8_t admin = 1; admin <= lmt; admin++)
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
    LOG(D1) << "Entering DtaDevOpal::setSIDHostChallenge() " << dev;
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
    LOG(D) << "set SID password completed " << dev;
    LOG(D1) << "Exiting DtaDevOpal::setSIDPassword() " << dev;
    return 0;
}

uint8_t DtaDevOpal::setSIDHostChallenge(vector<uint8_t> oldHostChallenge,
                                        vector<uint8_t> newHostChallenge)
{
    LOG(D1) << "Entering DtaDevOpal::setSIDHostChallenge() " << dev;
    uint8_t lastRC = WithSession([this, oldHostChallenge](){
        return session->start(OPAL_UID::OPAL_ADMINSP_UID,
                              oldHostChallenge, OPAL_UID::OPAL_SID_UID);
    },
                                 [this, newHostChallenge](){
        vector<uint8_t> table;
        table.clear();
        table. push_back(OPAL_SHORT_ATOM::BYTESTRING8);
        for (int i = 0; i < 8; i++) {
            table.push_back(OPALUID[OPAL_UID::OPAL_C_PIN_SID][i]);
        }
        return setTable(table, OPAL_TOKEN::PIN, newHostChallenge);
    });
    if (lastRC != 0) {
        LOG(E) << "Unable to set new SID password " << dev;
    } else {
        LOG(D) << "set SID password completed " << dev;
        LOG(D1) << "Exiting DtaDevOpal::setSIDHostChallenge() " << dev;
    }
    return lastRC;
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
    DTA_Header * hdr = (DTA_Header *) cmd->getCmdBuffer();
	LOG(D) << "Entering DtaDevOpal::exec " << dev;
    LOG(D3) << endl << "Dumping command buffer";
    IFLOG(D) DtaHexDump(cmd->getCmdBuffer(), SWAP32(hdr->cp.length) + sizeof (DTA_ComPacketHeader));
	LOG(D) << "Entering DtaDevOpal::exec sendCmd(IF_SEND, IO_BUFFER_LENGTH)";
    //if((lastRC = sendCmd(IF_SEND, protocol, comID(), cmd->getCmdBuffer(), IO_BUFFER_LENGTH)) != 0) {
	#if 0
	if (adj_host == 1) {
		LOG(D) << "adj_host = 1, use Host_sz_MaxComPacketSize";
		printf("Host_sz_MaxComPacketSize = %ld\n", Host_sz_MaxComPacketSize);
	}
	#endif
	
# if USING_OPER
	if (oper == 1)
		lastRC = sendCmd(IF_SEND, protocol, comID(), cmd->getCmdBuffer(), cmd->outputBufferSize());
	else
        lastRC = sendCmd(IF_SEND, protocol, comID(), cmd->getCmdBuffer(), adj_io_buffer_length);
#else
    lastRC = sendCmd(IF_SEND, protocol, comID(), cmd->getCmdBuffer(), cmd->outputBufferSize());
#endif
	if ((lastRC) != 0) {
        LOG(E) << "Command failed on send to " << dev << " --  result was 0x"
               << hex << setw(2) << setfill('0') << uppercase << (uint16_t)lastRC;
        return lastRC;
    }

	/* if TperReset, no response cmd to send */
	if ((protocol == 0x02) && (comID() == 0x0004)) {
		LOG(D) << "TperReset, No reponse command after all";
		return 0;
	}


    hdr = (DTA_Header *) cmd->getRespBuffer();

    do {
        osmsSleep(25); // could it be too fast if multiple drive situation ?????, 25->250 does not help; 25->50 better, ->100
        bzero(cmd->getRespBuffer(), cmd->getRespBufferSize());
# if USING_OPER
        if (oper == 1 )
            lastRC = sendCmd(IF_RECV, protocol, comID(), cmd->getRespBuffer(), 2048); //  IO_BUFFER_LENGTH);
        else
            lastRC = sendCmd(IF_RECV, protocol, comID(), cmd->getRespBuffer(), adj_io_buffer_length); //  IO_BUFFER_LENGTH);
#else
        lastRC = sendCmd(IF_RECV, protocol, comID(), cmd->getRespBuffer(), cmd->getRespBufferSize());
#endif

		//LOG(D) << "hdr->cp.outstandingData)=" << hdr->cp.outstandingData << " hdr->cp.minTransfer=" << hdr->cp.minTransfer << dev;
	}
    while ((0 != hdr->cp.outstandingData) && (0 == hdr->cp.minTransfer));  // add timer --> advice from Joe
    LOG(D3) << endl << "Dumping reply buffer";
    IFLOG(D3) DtaHexDump(cmd->getRespBuffer(), SWAP32(hdr->cp.length) + sizeof (DTA_ComPacketHeader));
	if (0 != lastRC) {
        LOG(E) << "Command failed on recv from " << dev << " --  result was 0x"
               << hex << setw(2) << setfill('0') << uppercase << (uint16_t)lastRC;
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
	uint16_t sz_MaxComPacketSize = 2048; // = 17408; // 61440;
	uint16_t sz_MaxResponseComPacketSize = 2048; // = 17108; //  61440;
	uint16_t sz_MaxPacketSize = 2028; // = 17180; // 61440;
	uint16_t sz_MaxIndTokenSize = 1992; // = 16384; // 61384;

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
	else if (adj_host ==0) {
		sz_MaxComPacketSize = 2048; // 10240; // 17408; 
		sz_MaxResponseComPacketSize = 2048; // 10240; // 17108;
		sz_MaxPacketSize = 2028; //  10220; // 17180;
		sz_MaxIndTokenSize = 1992; //  10184; //  16384;
		adj_io_buffer_length = 2048; // -> 2048 IO_BUFFER_LENGTH; // 10240; //  17408;
	}
	else if (adj_host == 3) {// T7 specific
//		uint16_t adjust_more_t7 = 512; // T7 adjust size , 256,  NG >= 512 OK
		sz_MaxComPacketSize = 30720  ; // 30K  - adjust_more_t7  ; // 10240; // 17408; 
		sz_MaxResponseComPacketSize = 30720; // 10240; // 17108;
		sz_MaxPacketSize = 30700 ; // - adjust_more_t7; //  10220; // 17180;
		sz_MaxIndTokenSize = 30700 - 56 ; // - adjust_more_t7; //  10184; //  16384;
		adj_io_buffer_length = 30720;   // Tper_sz_MaxComPacketSize - adjust_more_t7; // +IO_BUFFER_ALIGNMENT; //  17408;
																		  // adj_io_buffer_length must not exceed TperMaxComPacketSize 
	}
	else if (adj_host == 2) {// anything less than 64K but greater than 2K, will adjust according to TPer returned size
		uint16_t adjust_more_t7 = 512 ; // T7 adjust size , 256,  NG >= 512 OK 
		sz_MaxComPacketSize = (uint16_t)Tper_sz_MaxComPacketSize ; // - adjust_more_t7  ; // 10240; // 17408;
		sz_MaxResponseComPacketSize = (uint16_t)Tper_sz_MaxResponseComPacketSize; // 10240; // 17108;
		sz_MaxPacketSize = (uint16_t)Tper_sz_MaxPacketSize ; // - adjust_more_t7; //  10220; // 17180;
		sz_MaxIndTokenSize = (uint16_t)Tper_sz_MaxIndTokenSize; // - adjust_more_t7; //  10184; //  16384;
		adj_io_buffer_length = (uint16_t)(Tper_sz_MaxComPacketSize - adjust_more_t7); // +IO_BUFFER_ALIGNMENT; //  17408;
		// adj_io_buffer_length must not exceed TperMaxComPacketSize 
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
		if (show) cout << endl << "TPer Properties: " << dev;
		uint8_t tper_flag;
		tper_flag = 1;
		for (uint32_t i = 0; i < propertiesResponse.getTokenCount(); i++) {
			if (OPAL_TOKEN::STARTNAME == propertiesResponse.tokenIs(i)) {
				if (OPAL_TOKEN::DTA_TOKENID_BYTESTRING != propertiesResponse.tokenIs(i + 1))
				{
					if (show) cout << endl << "Host Properties: " << endl;
					tper_flag = 0;
				}
				else //
				{//
					if (show) cout << "  " << propertiesResponse.getString(i + 1) << " = " << propertiesResponse.getUint64(i + 2);

					if (!memcmp((propertiesResponse.getString(i + 1)).c_str(), "MaxComPacketSize", sizeof("MaxComPacketSize"))) {
						//LOG(D) << "match MaxComPacketSize";
						if (tper_flag) { // Tper size
							//LOG(D) << "Tper";
							Tper_sz_MaxComPacketSize = (uint32_t)propertiesResponse.getUint64(i + 2);
						}
						else { // Host size
							//LOG(D) << "Host";
							Host_sz_MaxComPacketSize = (uint32_t)propertiesResponse.getUint64(i + 2);
						}
					}
					else if (!memcmp((propertiesResponse.getString(i + 1)).c_str(), "MaxResponseComPacketSize", sizeof("MaxResponseComPacketSize"))) {
						//LOG(D) << "match MaxResponseComPacketSize";
						if (tper_flag) { // Tper size
							//LOG(D) << "Tper";
							Tper_sz_MaxResponseComPacketSize = (uint32_t)propertiesResponse.getUint64(i + 2);
						}
						else { // Host size
							//LOG(D) << "Host";
							Host_sz_MaxResponseComPacketSize = (uint32_t)propertiesResponse.getUint64(i + 2);
						}
					}
					else if (!memcmp((propertiesResponse.getString(i + 1)).c_str(), "MaxPacketSize", sizeof("MaxPacketSize"))) {
						//LOG(D) << "match MaxPacketSize";
						if (tper_flag) { // Tper size
							//LOG(D) << "Tper";
							Tper_sz_MaxPacketSize = (uint32_t)propertiesResponse.getUint64(i + 2);
						}
						else { // Host size
							//LOG(D) << "Host";
							Host_sz_MaxPacketSize = (uint32_t)propertiesResponse.getUint64(i + 2);
						}
					}
					else if (!memcmp((propertiesResponse.getString(i + 1)).c_str(), "MaxIndTokenSize", sizeof("MaxIndTokenSize"))) {
						//LOG(D) << "match MaxIndTokenSize";
						if (tper_flag) { // Tper size
							//LOG(D) << "Tper";
							Tper_sz_MaxIndTokenSize = (uint32_t)propertiesResponse.getUint64(i + 2);
						}
						else { // Host size
							//LOG(D) << "Host";
							Host_sz_MaxIndTokenSize = (uint32_t)propertiesResponse.getUint64(i + 2);
						}
					}
				} // 
				i += 2;
			}
			if (show) if (!(i % 6)) cout << endl;
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
	DtaDevOS::puke();
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
	LOG(D1) << "Enter adj_host_prop";
	//fill_prop(FALSE); // why there are two fill_property
	//printf("act =  %d\n", act);

	adj_host = act;
	properties();
	fill_prop(FALSE); // must re-stuff the host property because properties() only exchange property with Tper but not set host_sz_Maxxxxxxxx
	LOG(D1) << "Exit adj_host_prop";
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
        delete get;
		return DTAERROR_INVALID_PARAMETER;
	}
	if (16 != strnlen(objID, 32)) {
		LOG(E) << "ObjectID must be 16 byte ascii string of hex object uid";
        delete get;
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
	LOG(D) << "Command:";
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
	LOG(D) << "Response:";
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
	LOG(D) << "Command:";
	cmd->dumpCommand();
	if ((lastRC = session->sendCommand(cmd, response)) != 0) {
		delete cmd;
		delete session;
		return lastRC;
	}
	LOG(D) << "Response:";
	cmd->dumpResponse();
	delete cmd;
	delete session;
	LOG(D1) << "Exiting DtaDevEnterprise::rawCmd";
	return 0;
}


uint8_t DtaDevOpal::getMSID(string& MSID) {
    const uint8_t rc = getDefaultPassword();
    if (rc) {
        LOG(E) << "unable to read MSID password";
        return rc;
    }
    MSID = response.getString(4);
    LOG(D1) << "MSID=" << MSID;
    return 0;
}
