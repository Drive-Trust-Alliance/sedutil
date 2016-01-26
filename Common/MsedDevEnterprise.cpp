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
#include "MsedAnnotatedDump.h"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif
using namespace std;


////////////////////////////////////////////////////////////////////////////////
static const bool is_NULL_UID(std::vector<uint8_t> & v)
////////////////////////////////////////////////////////////////////////////////
{
    return
        v.size() == 9
        && v[0] == 0xa8
        && v[1] == 0x00
        && v[2] == 0x00
        && v[3] == 0x00
        && v[4] == 0x00
        && v[5] == 0x00
        && v[6] == 0x00
        && v[7] == 0x00
        && v[8] == 0x00
        ;
}

////////////////////////////////////////////////////////////////////////////////
static void set8(vector<uint8_t> & v, const uint8_t value[8])
////////////////////////////////////////////////////////////////////////////////
{
	v.clear();
	v.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
	for (int i = 0; i < 8; i++)
    {
		v.push_back(value[i]);
	}
}

////////////////////////////////////////////////////////////////////////////////
static void setband(vector<uint8_t> & v, uint16_t i)
////////////////////////////////////////////////////////////////////////////////
{
    const uint16_t j = i+1;
    v[1+6] = (uint8_t(j >> 8) | v[1+6]) & 0xF0;
    v[1+7] = uint8_t(j);
}

////////////////////////////////////////////////////////////////////////////////
static void user2cpin(vector<uint8_t> & dst, vector<uint8_t> & src)
////////////////////////////////////////////////////////////////////////////////
{
    // this works for BandMasterN and EraseMaster
    // Table 29 Locking C_PIN table, p. 72 of Enterprise SSC rev 3.00
    dst = src;
    dst[1+3] = 0x0B;
}

////////////////////////////////////////////////////////////////////////////////
uint16_t MsedDevEnterprise::getMaxRanges(char * password)
////////////////////////////////////////////////////////////////////////////////
{
    // create session
	session = new MsedSession(this);
	if (session->start(OPAL_UID::ENTERPRISE_LOCKINGSP_UID))
    {
		delete session;
		return 0xffff;
	}

    //** Table 36 "LockingInfo table", p. 72 of Enterprise SSC rev 3.00
	vector<uint8_t> table;
    set8(table, OPALUID[ENTERPRISE_LOCKING_INFO_TABLE]);

    // query row 1 of LockingInfo table
	if (getTable(table, "MaxRanges", "MaxRanges"))
    {
		delete session;
		return getMaxRangesOpal(password);
	}
	delete session;

    // "MaxRanges" is token 5 of response
	const uint16_t MaxRanges = response.getUint16(5);
    return MaxRanges;
}

////////////////////////////////////////////////////////////////////////////////
uint16_t MsedDevEnterprise::getMaxRangesOpal(char * password)
////////////////////////////////////////////////////////////////////////////////
{
    // create session
	session = new MsedSession(this);
	if (session->start(OPAL_UID::ENTERPRISE_LOCKINGSP_UID))
    {
		delete session;
		return 0xffff;
	}

    //** Table 36 "LockingInfo table", p. 72 of Enterprise SSC rev 3.00
	vector<uint8_t> table;
    set8(table, OPALUID[OPAL_LOCKING_INFO_TABLE]);

    // query row 1 of LockingInfo table
	if (getTable(table, "MaxRanges", "MaxRanges"))
    {
		LOG(E) << "Unable to get MaxRanges from LockingInfo table";
		delete session;
		return 0xffff;
	}
	delete session;

    // "MaxRanges" is token 5 of response
	const uint16_t MaxRanges = response.getUint16(5);
    return MaxRanges;
}

MsedDevEnterprise::MsedDevEnterprise(const char * devref)
{
	MsedDevOS::init(devref);
	assert(isEprise());
	if (properties()) { LOG(E) << "Properties exchange failed"; }
}
MsedDevEnterprise::~MsedDevEnterprise()
{
}
uint8_t MsedDevEnterprise::initialSetup(char * password)
{
	LOG(D1) << "Entering initialSetup()";
	if (takeOwnership(password)) {
		LOG(E) << "Initial setup failed - unable to take ownership";
		return 0xff;
	}
	if (setLockingRange(0,
        OPAL_LOCKINGSTATE::READWRITE, password)) {
		LOG(E) << "Initial setup failed - unable to unlock for read/write";
		return 0xff;
	}
	
	if (configureLockingRange(0,
		(MSED_READLOCKINGENABLED | MSED_WRITELOCKINGENABLED), password)) {
		LOG(E) << "Initial setup failed - unable to enable read/write locking";
		return 0xff;
	}
	
	LOG(I) << "Initial setup of TPer complete on " << dev;
	LOG(D1) << "Exiting initialSetup()";
	return 0;
}
uint8_t MsedDevEnterprise::setup_SUM(uint8_t lockingrange, uint64_t start, uint64_t length, char *Admin1Password, char * password)
{
	LOG(D1) << "Entering MsedDevEnterprise::setup_SUM";
	LOG(I) << "setup_SUM not supported on MsedDevEnterprise";
	return 1;
}
uint8_t MsedDevEnterprise::configureLockingRange(uint8_t lockingrange, uint8_t enabled, char * password)
{
	LOG(D1) << "Entering MsedDevEnterprise::configureLockingRange()";

    //** BandMaster0 UID of Table 28 Locking SP Authority table, p. 70 of Enterprise SSC rev 3.00
	std::vector<uint8_t> user;
	set8(user, OPALUID[OPAL_UID::ENTERPRISE_BANDMASTER0_UID]);
	setband(user, lockingrange);

    //** Global_Range UID of Table 33 Locking SP Locking table, p. 84 of Enterprise SSC rev 3.00
	vector<uint8_t> object;
    set8(object, OPALUID[OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL]);
    setband(object, lockingrange);

	session = new MsedSession(this);
	if (session->start(OPAL_UID::ENTERPRISE_LOCKINGSP_UID, password, user)) {
		delete session;
		return 0xff;
	}

	/* can't use settable because the segate drives require that both the 
	 * read & write lockenabled be changed at the same time.  I can find no
	 * written doc on such a restriction but .....
	 */
    vector<uint8_t> method;
    set8(method, OPALMETHOD[OPAL_METHOD::ESET]);

	MsedCommand *set = new MsedCommand();
	set->reset(object, method);
	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::ENDLIST);
	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken("ReadLockEnabled");
	set->addToken((enabled & MSED_READLOCKINGENABLED) ? OPAL_TRUE : OPAL_FALSE);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken("WriteLockEnabled");
	set->addToken((enabled & MSED_WRITELOCKINGENABLED) ? OPAL_TRUE : OPAL_FALSE);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken("LockOnReset");
	set->addToken(OPAL_TOKEN::STARTLIST);
    set->addToken(UINT_00);
	set->addToken(OPAL_TOKEN::ENDLIST);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::ENDLIST);
	set->addToken(OPAL_TOKEN::ENDLIST);
	set->addToken(OPAL_TOKEN::ENDLIST);
	set->complete();

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
uint8_t MsedDevEnterprise::rekeyLockingRange(uint8_t lockingrange, char * password)
{
	LOG(D1) << "Entering MsedDevEnterprise::rekeyLockingRange()";
	uint8_t lastRC;

    //** BandMaster0 UID of Table 28 Locking SP Authority table, p. 70 of Enterprise SSC rev 3.00
	vector<uint8_t> user;
    set8(user, OPALUID[ENTERPRISE_BANDMASTER0_UID]);
    setband(user, lockingrange);

    //** Global_Range UID of Table 33 Locking SP Locking table, p. 84 of Enterprise SSC rev 3.00
	vector<uint8_t> table;
    set8(table, OPALUID[OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL]);
    setband(table, lockingrange);

	session = new MsedSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::ENTERPRISE_LOCKINGSP_UID, password, user)) != 0) {
		delete session;
		return lastRC;
	}
	if ((lastRC = getTable(table, "ActiveKey", "ActiveKey")) != 0) {
		delete session;
		return lastRC;
	}
	std::vector<uint8_t> ActiveKey = response.getRawToken(5);
    if (is_NULL_UID(ActiveKey))
    {
	    LOG(I) << "LockingRange" << (uint16_t)lockingrange << " remains in plaintext ";
		delete session;
        return 0;
    }
	MsedCommand *rekey = new MsedCommand();
	if (NULL == rekey) {
		LOG(E) << "Unable to create command object ";
		delete session;
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	rekey->reset(OPAL_UID::OPAL_AUTHORITY_TABLE, OPAL_METHOD::GENKEY);
	rekey->changeInvokingUid(ActiveKey);
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
	LOG(D1) << "Exiting MsedDevEnterprise::rekeyLockingRange()";
	return 0;
}
uint8_t MsedDevEnterprise::revertLockingSP(char * password, uint8_t keep)
{
	LOG(D1) << "Entering MsedDevEnterprise::revertLockingSP()";
	if(password == NULL) { LOG(D4) << "Referencing formal parameters " << keep; }
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
	if ((lastRC = session->start(OPAL_UID::OPAL_ADMINSP_UID, password, uid)) != 0) {
		delete cmd;
		delete session;
		return lastRC;
	}
	cmd->reset(OPAL_UID::OPAL_THISSP_UID, OPAL_METHOD::REVERTSP);
	cmd->addToken(OPAL_TOKEN::STARTLIST);
	cmd->addToken(OPAL_TOKEN::STARTNAME);
	cmd->addToken("KeepGlobalRangeKey");
	cmd->addToken(keep);
	cmd->addToken(OPAL_TOKEN::ENDNAME);
	cmd->addToken(OPAL_TOKEN::ENDLIST);
	cmd->complete();
	session->expectAbort();
	if ((lastRC = session->sendCommand(cmd, response)) != 0) {
		delete cmd;
		delete session;
		return lastRC;
	}
	LOG(I) << "revertLockingSP completed successfully";
	delete cmd;
	delete session;
	LOG(D1) << "Exiting MsedDevEnterprise::revertLockingSP()";
	return 0;
}
uint8_t MsedDevEnterprise::setPassword(char * password, char * userid, char * newpassword)
{
	LOG(D1) << "Entering MsedDevEnterprise::setPassword" ;
	if (11 > strnlen(userid, 15)) {
		LOG(E) << "Invalid Userid " << userid;
		return 0xff;
	}

	std::vector<uint8_t> user;
	if (!memcmp("BandMaster", userid, 10)) {
		int band = atoi(&userid[10]);
		if (1023 < band) {
			LOG(E) << "Invalid Userid " << userid;
			return 0xff;
		}
		set8(user, OPALUID[OPAL_UID::ENTERPRISE_BANDMASTER0_UID]);
		setband(user, band);
	}
	else if (!memcmp("EraseMaster", userid, 11)) {
		set8(user, OPALUID[OPAL_UID::ENTERPRISE_ERASEMASTER_UID]);
	}
	else {
		LOG(E) << "Invalid Userid " << userid;
		return 0xff;
	}
	std::vector<uint8_t> usercpin;
	user2cpin(usercpin, user);
	if (*password == '\0')
	{
		if (getDefaultPassword())
		{
			LOG(E) << "setPassword failed to retrieve MSID";
			return 0xff;
		}
		string defaultPassword = response.getString(5);
		session = new MsedSession(this);
		session->dontHashPwd();
		if (session->start(OPAL_UID::ENTERPRISE_LOCKINGSP_UID, (char *)defaultPassword.c_str(), user))
		{
			delete session;
			return 0xff;
		}
	}
	else
	{
		session = new MsedSession(this);
		if (session->start(OPAL_UID::ENTERPRISE_LOCKINGSP_UID, password, user))
		{
			delete session;
			return 0xff;
		}
	}

	std::vector<uint8_t> hash;
	MsedHashPwd(hash, newpassword, this);
	if (setTable(usercpin, "PIN", hash)) {
		LOG(E) << "Unable to set user " << userid << " new password ";
		delete session;
		return 0xff;
	}
	LOG(I) << userid << " password changed";
	delete session;
	LOG(D1) << "Exiting MsedDevEnterprise::setPassword()";
	return 0;
}
uint8_t MsedDevEnterprise::setNewPassword_SUM(char * password, char * userid, char * newpassword)
{
	LOG(D1) << "Entering MsedDevEnterprise::setNewPassword_SUM()";
	LOG(I) << "setNewPassword_SUM is not in the Enterprise SSC and not supported";
	LOG(D1) << "Exiting MsedDevEnterprise::setNewPassword_SUM()";
	return 0;
}
uint8_t MsedDevEnterprise::setMBREnable(uint8_t mbrstate,	char * Admin1Password)
{
	LOG(D1) << "Entering MsedDevEnterprise::setMBREnable";
	if (NULL == Admin1Password) { LOG(E) << "This shouldn't happen " << mbrstate; }
	LOG(I) << "MBR shadowing is optional in the Enterprise SSC and not supported";
	LOG(D1) << "Exiting MsedDevEnterprise::setMBREnable";
	return 0;
}
uint8_t MsedDevEnterprise::setMBRDone(uint8_t mbrstate, char * Admin1Password)
{
	LOG(D1) << "Entering MsedDevEnterprise::setMBRDone";
	if (NULL == Admin1Password) { LOG(E) << "This shouldn't happen " << mbrstate; }
	LOG(I) << "MBR shadowing is optional in the Enterprise SSC and not supported";
	LOG(D1) << "Exiting MsedDevEnterprise::setMBRDone";
	return 0;
}

uint8_t MsedDevEnterprise::setupLockingRange(uint8_t lockingrange, uint64_t start, uint64_t length, char * password)
{
	uint8_t lastRC;
	LOG(D1) << "Entering MsedDevEnterprise::setupLockingRange";
    // look up MaxRanges
	const uint16_t MaxRanges = getMaxRanges(password);
    if (MaxRanges == 0 || MaxRanges >= 1024) return 0xff;

    // make sure lockingrange is in bounds
    if (lockingrange > MaxRanges)
    {
        LOG(E) << "Requested locking range " << lockingrange << " greater than MaxRanges " << MaxRanges;
        return 0xff;
    }

    //** BandMaster0 UID of Table 28 Locking SP Authority table, p. 70 of Enterprise SSC rev 3.00
    vector<uint8_t> user;
    set8(user, OPALUID[OPAL_UID::ENTERPRISE_BANDMASTER0_UID]);
    setband(user, lockingrange);

    //** Global_Range UID of Table 33 Locking SP Locking table, p. 84 of Enterprise SSC rev 3.00
    vector<uint8_t> object;
    set8(object, OPALUID[OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL]);
    setband(object, lockingrange);

    vector<uint8_t> method;
    set8(method, OPALMETHOD[OPAL_METHOD::ESET]);

	MsedCommand *set = new MsedCommand();
	set->reset(object, method);
	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::ENDLIST);
	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken("RangeStart");
	set->addToken(start);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken("RangeLength");
	set->addToken(length);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken("ReadLockEnabled");
	set->addToken(OPAL_TOKEN::OPAL_FALSE);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken("WriteLockEnabled");
	set->addToken(OPAL_TOKEN::OPAL_FALSE);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken("ReadLocked");
	set->addToken(OPAL_TOKEN::OPAL_FALSE);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken("WriteLocked");
	set->addToken(OPAL_TOKEN::OPAL_FALSE);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::STARTNAME);
	set->addToken("LockOnReset");
	set->addToken(OPAL_TOKEN::STARTLIST);
	set->addToken(OPAL_TOKEN::ENDLIST);
	set->addToken(OPAL_TOKEN::ENDNAME);
	set->addToken(OPAL_TOKEN::ENDLIST);
	set->addToken(OPAL_TOKEN::ENDLIST);
	set->addToken(OPAL_TOKEN::ENDLIST);
	set->complete();

	session = new MsedSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::ENTERPRISE_LOCKINGSP_UID, password, user)) != 0) {
		delete session;
		return lastRC;
	}
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
	LOG(D1) << "Exiting MsedDevEnterprise::setupLockingRange";
	return 0;
}
uint8_t MsedDevEnterprise::setupLockingRange_SUM(uint8_t lockingrange, uint64_t start,
	uint64_t length, char * password) {
	LOG(D1) << "Entering MsedDevEnterprise::setupLockingRange_SUM";
	if (0 == lockingrange) { LOG(E) << start << length << password; }
	LOG(D1) << "Exiting MsedDevEnterprise::setupLockingRange_SUM";
	return 0;
}
////////////////////////////////////////////////////////////////////////////////
uint8_t MsedDevEnterprise::listLockingRanges(char * password, int rangeid)
////////////////////////////////////////////////////////////////////////////////
{
	LOG(D) << "Entering MsedDevEnterprise::listLockingRanges";
	if (NULL == password) { LOG(E) << "password NULL"; }

    // look up MaxRanges
	const uint16_t MaxRanges = (rangeid == -1) ? getMaxRanges(password): (rangeid + 1);

    if (MaxRanges == 0 || MaxRanges >= 1024) return 0xff;

    if(rangeid == -1) {
        LOG(I) << "Maximum ranges supported " << MaxRanges;
    }

    //** BandMaster0 UID of Table 28 Locking SP Authority table, p. 70 of Enterprise SSC rev 3.00
    vector<uint8_t> user;
    user.clear();
    set8(user, OPALUID[ENTERPRISE_BANDMASTER0_UID]);

    //** Global_Range UID of Table 33 Locking SP Locking table, p. 84 of Enterprise SSC rev 3.00
    vector<uint8_t> table;
    table.clear();
    set8(table, OPALUID[OPAL_LOCKINGRANGE_GLOBAL]);

    int start = (rangeid == -1)? 0: rangeid;
	for (int i = start; i < MaxRanges; i++)
    {
        setband(user, i);
        setband(table, i);

        LOG(I) << "Band[" << i << "]";

		session = new MsedSession(this);
		if (session->start(OPAL_UID::ENTERPRISE_LOCKINGSP_UID, password, user))
        {
			LOG(I) << "    could not establish session for row[" << i << "]";
			delete session;
			continue;
		}
		if (getTable(table, "Name", "LockOnReset"))
        {
			LOG(I) << "    row[" << i << "] not found in LOCKING table";
			delete session;
			continue;
		}
		// 00	1   ( F0 ) Start_List
		// 01	1   ( F0 ) Start_List
		// 02	1   ( F0 ) Start_List
		// 03	1   ( F2 ) Start_Name
		// 04	5   ( A4 ) 4E 61 6D 65 ("Name")
		// 05*	6   ( A5 ) 42 61 6E 64 31 ("Band1")
		// 06	1   ( F3 ) End_Name
		// 07	1   ( F2 ) Start_Name
		// 08	11  ( AA ) 43 6F 6D 6D 6F 6E 4E 61 6D 65 ("CommonName")
		// 09*	8   ( A7 ) 4C 6F 63 6B 69 6E 67 ("Locking")
		// 10	1   ( F3 ) End_Name
		// 11	1   ( F2 ) Start_Name
		// 12	11  ( AA ) 52 61 6E 67 65 53 74 61 72 74 ("RangeStart")
		// 13*	1   ( 00 ) 0 (0h)
		// 14	1   ( F3 ) End_Name
		// 15	1   ( F2 ) Start_Name
		// 16	12  ( AB ) 52 61 6E 67 65 4C 65 6E 67 74 68 ("RangeLength")
		// 17*	1   ( 00 ) 0 (0h)
		// 18	1   ( F3 ) End_Name
		// 19	1   ( F2 ) Start_Name
		// 20	16  ( AF ) 52 65 61 64 4C 6F 63 6B 45 6E 61 62 6C 65 64 ("ReadLockEnabled")
		// 21*	1   ( 00 ) 0 (0h)
		// 22	1   ( F3 ) End_Name
		// 23	1   ( F2 ) Start_Name
		// 24	18  ( D0 10 ) 57 72 69 74 65 4C 6F 63 6B 45 6E 61 62 6C 65 64 ("WriteLockEnabled")
		// 25*	1   ( 00 ) 0 (0h)
		// 26	1   ( F3 ) End_Name
		// 27	1   ( F2 ) Start_Name
		// 28	11  ( AA ) 52 65 61 64 4C 6F 63 6B 65 64 ("ReadLocked")
		// 29*	1   ( 01 ) 1 (1h)
		// 30	1   ( F3 ) End_Name
		// 31	1   ( F2 ) Start_Name
		// 32	12  ( AB ) 57 72 69 74 65 4C 6F 63 6B 65 64 ("WriteLocked")
		// 33*	1   ( 01 ) 1 (1h)
		// 34	1   ( F3 ) End_Name
		// 35	1   ( F2 ) Start_Name
		// 36	12  ( AB ) 4C 6F 63 6B 4F 6E 52 65 73 65 74 ("LockOnReset")
		// 37	1   ( F0 ) Start_List
		// 38	1   ( 00 ) 0 (0h)
		// 39	1   ( F1 ) End_List
		// 40	1   ( F3 ) End_Name
        const std::string Name          = response.getString(5+4*0);
        const std::string CommonName    = response.getString(5+4*1);
        const uint64_t RangeStart       = response.getUint64(5+4*2);
        const uint64_t RangeLength      = response.getUint64(5+4*3);
        const bool ReadLockEnabled      = response.getUint8(5+4*4) != 0;
        const bool WriteLockEnabled     = response.getUint8(5+4*5) != 0;
        const bool ReadLocked           = response.getUint8(5+4*6) != 0;
        const bool WriteLocked          = response.getUint8(5+4*7) != 0;
        // LockOnReset list has at least one element
        const bool LockOnReset          = response.tokenIs(5+4*8) == STARTLIST
                                        && response.tokenIs(5+4*8+1) == MSED_TOKENID_UINT;
        delete session;

        LOG(I) << "    Name             " << Name;
        LOG(I) << "    CommonName       " << CommonName;
        LOG(I) << "    RangeStart       " << RangeStart;
        LOG(I) << "    RangeLength      " << RangeLength;
        LOG(I) << "    ReadLockEnabled  " << ReadLockEnabled;
        LOG(I) << "    WriteLockEnabled " << WriteLockEnabled;
        LOG(I) << "    ReadLocked       " << ReadLocked;
        LOG(I) << "    WriteLocked      " << WriteLocked;
        LOG(I) << "    LockOnReset      " << LockOnReset;
	}
	LOG(D1) << "Exiting MsedDevEnterprise::listLockingRanges";
	return 0;
}

uint8_t MsedDevEnterprise::setLockingRange(uint8_t lockingrange, uint8_t lockingstate,
	char * password)
{
	LOG(D1) << "Entering MsedDevEnterprise::setLockingRange";

    // convert Opal lockingstate to boolean	
    OPAL_TOKEN locked;
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
    	LOG(E) << "Invalid locking state for setLockingRange (RW=1, LOCKED=3)";
    	return 0xff;
	}
	
    // look up MaxRanges
	const uint16_t MaxRanges = getMaxRanges(password);
    if (MaxRanges == 0 || MaxRanges >= 1024) return 0xff;

    // make sure lockingrange is in bounds
    if (lockingrange > MaxRanges)
    {
        LOG(E) << "Requested locking range " << lockingrange << " greater than MaxRanges " << MaxRanges;
        return 0xff;
    }

    //** BandMaster0 UID of Table 28 Locking SP Authority table, p. 70 of Enterprise SSC rev 3.00
    vector<uint8_t> user;
    set8(user, OPALUID[OPAL_UID::ENTERPRISE_BANDMASTER0_UID]);
    setband(user, lockingrange);

    //** Band0 UID of Table 33 Locking SP Locking table, p. 84 of Enterprise SSC rev 3.00
    vector<uint8_t> object;
    set8(object, OPALUID[OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL]);
    setband(object, lockingrange);

    vector<uint8_t> method;
    set8(method, OPALMETHOD[OPAL_METHOD::ESET]);

	MsedCommand *set = new MsedCommand();
	set->reset(object, method);
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
	if (session->start(OPAL_UID::ENTERPRISE_LOCKINGSP_UID, password, user))
    {
		delete session;
		return 0xff;
	}
	if (session->sendCommand(set, response))
    {
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
uint8_t MsedDevEnterprise::setLockingRange_SUM(uint8_t lockingrange, uint8_t lockingstate,
	char * password) {
		LOG(D1) << "Entering MsedDevEnterprise::setLockingRange_SUM()";
        LOG(E) << "setLockingRange_SUM not implemented";
        LOG(D1) << "Exiting MsedDevEnterprise::setLockingRange_SUM()";
	return 0;
}
uint8_t MsedDevEnterprise::enableUser(char * password, char * userid)
{
	LOG(D1) << "Entering MsedDevEnterprise::enableUser";
	LOG(E) << "enableUser not implemented";
	if (!password && !userid) { LOG(E) << "Formal Parameters"; }
	LOG(D1) << "Exiting MsedDevEnterprise::enableUser()";
	return 0xff;
}
uint8_t MsedDevEnterprise::revertTPer(char * password, uint8_t PSID)
{
	LOG(D1) << "Entering MsedDevEnterprise::revertTPer()";
	if (password == NULL) { LOG(D4) << "Referencing formal parameters " << PSID; }
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
	LOG(D1) << "Exiting MsedDevEnterprise::revertTPer()";
	return 0;
}
uint8_t MsedDevEnterprise::eraseLockingRange(uint8_t lockingrange, char * password)
{
	uint8_t lastRC;
	LOG(D1) << "Entering MsedDevEnterprise::eraseLockingRange";

    // look up MaxRanges
	const uint16_t MaxRanges = getMaxRanges(password);
    if (MaxRanges == 0 || MaxRanges >= 1024) return 0xff;

    // make sure lockingrange is in bounds
    if (lockingrange > MaxRanges)
    {
        LOG(E) << "Requested locking range " << lockingrange << " greater than MaxRanges " << MaxRanges;
        return 0xff;
    }

    //** EraseMaster UID of Table 28 Locking SP Authority table, p. 70 of Enterprise SSC rev 3.00
    vector<uint8_t> user;
    set8(user, OPALUID[OPAL_UID::ENTERPRISE_ERASEMASTER_UID]);

    //** Band0 UID of Table 33 Locking SP Locking table, p. 84 of Enterprise SSC rev 3.00
    vector<uint8_t> object;
    set8(object, OPALUID[OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL]);
    setband(object, lockingrange);

    vector<uint8_t> method;
    set8(method, OPALMETHOD[OPAL_METHOD::ERASE]);

	MsedCommand *erase = new MsedCommand();
	erase->reset(object, method);
	erase->addToken(OPAL_TOKEN::STARTLIST);
	erase->addToken(OPAL_TOKEN::ENDLIST);
	erase->complete();

	session = new MsedSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return MSEDERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::ENTERPRISE_LOCKINGSP_UID, password, user)) != 0) {
		delete session;
		return lastRC;
	}
	if ((lastRC = session->sendCommand(erase, response)) != 0) {
		LOG(E) << "eraseLockingRange Failed ";
		delete erase;
		delete session;
		return lastRC;
	}
	delete erase;
	delete session;
	LOG(I) << "LockingRange" << (uint16_t)lockingrange << " erased";
	LOG(D1) << "Exiting MsedDevEnterprise::eraseLockingRange";
	return 0;
}
uint8_t MsedDevEnterprise::loadPBA(char * password, char * filename) {
	LOG(D1) << "Entering MsedDevEnterprise::loadPBAimage()" << filename << " " << dev;
	if (password == NULL) { LOG(D4) << "Referencing formal parameters " << filename; }
	LOG(I) << "loadPBA is not implemented.  It is not a mandatory part of  ";
	LOG(I) << "the enterprise SSC ";
	LOG(D1) << "Exiting MsedDevEnterprise::loadPBAimage()";
	return 0;
}
uint8_t MsedDevEnterprise::activateLockingSP(char * password)
{
	LOG(D1) << "Entering MsedDevEnterprise::activateLockingSP()";
	if (password == NULL) { LOG(D4) << "Referencing formal parameters "; }
	LOG(E) << "activate Locking SP is not a part of the Enterprise SSC ";
	LOG(D1) << "Exiting MsedDevEnterprise::activatLockingSP()";
	return 0;
}
uint8_t MsedDevEnterprise::activateLockingSP_SUM(uint8_t lockingrange, char * password)
{
	LOG(D1) << "Entering MsedDevEnterprise::activateLockingSP_SUM()";
	if (password == NULL) { LOG(D4) << "Referencing formal parameters "; }
	LOG(E) << "activate Locking SP SUM is not a part of the Enterprise SSC ";
	LOG(D1) << "Exiting MsedDevEnterprise::activateLockingSP_SUM()";
	return 0;
}
uint8_t MsedDevEnterprise::eraseLockingRange_SUM(uint8_t lockingrange, char * password)
{
	LOG(D1) << "Entering MsedDevEnterprise::eraseLockingRange_SUM()";
	if (password == NULL) { LOG(D4) << "Referencing formal parameters "; }
	LOG(E) << "Erase Locking Range SUM is not a part of the Enterprise SSC ";
	LOG(D1) << "Exiting MsedDevEnterprise::eraseLockingRange_SUM()";
	return 0;
}
uint8_t MsedDevEnterprise::takeOwnership(char * newpassword)
{
	string defaultPassword;
	LOG(D1) << "Entering MsedDevEnterprise::takeOwnership()";
	if (getDefaultPassword()) {
		LOG(E) << "takeOwnership failed unable to retrieve MSID";
		return 0xff;
	}
	defaultPassword = response.getString(5);
	if (setSIDPassword((char *)defaultPassword.c_str(), newpassword, 0)) {
		LOG(E) << "takeOwnership failed unable to set new SID password";
		return 0xff;
	}
	if (initLSPUsers((char *)defaultPassword.c_str(), newpassword)) {
		LOG(E) << "takeOwnership failed unable to set Locking SP user passwords";
		return 0xff;
	}
	LOG(I) << "takeOwnership complete";
	LOG(D1) << "Exiting takeOwnership()";
	return 0;
}
uint8_t MsedDevEnterprise::initLSPUsers(char * defaultPassword, char * newPassword)
{
    vector<uint8_t> user, usercpin, hash, erasemaster, table;
	LOG(D1) << "Entering MsedDevEnterprise::initLSPUsers()";

// do erasemaster
	session = new MsedSession(this);
	session->dontHashPwd();
    set8(erasemaster, OPALUID[OPAL_UID::ENTERPRISE_ERASEMASTER_UID]);
	if (session->start(OPAL_UID::ENTERPRISE_LOCKINGSP_UID, defaultPassword, erasemaster)) {
		delete session;
		return 0xff;
	}
	MsedHashPwd(hash, newPassword, this);
    user2cpin(usercpin, erasemaster);
	if (setTable(usercpin, "PIN", hash)) {
		LOG(E) << "Unable to set new EraseMaster password ";
		delete session;
		return 0xff;
	}
	LOG(I) << "EraseMaster  password set";
	delete session;
    // look up MaxRanges
	const uint16_t MaxRanges = getMaxRanges((char *)NULL);
    if (MaxRanges == 0 || MaxRanges >= 1024) return 0xff;
	LOG(I) << "Maximum ranges supported " << MaxRanges;
// do bandmasters
    set8(user, OPALUID[ENTERPRISE_BANDMASTER0_UID]);
	for (int i = 0; i < MaxRanges; i++) {
        setband(user, i);
		LOG(D3) << "initializing BandMaster" << (uint16_t) i;
		session = new MsedSession(this);
		session->dontHashPwd();
		if (session->start(OPAL_UID::ENTERPRISE_LOCKINGSP_UID, defaultPassword, user)) {
			delete session;
			return 0xff;
		}
		MsedHashPwd(hash, newPassword, this);
        user2cpin(usercpin, user);
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
uint8_t MsedDevEnterprise::printDefaultPassword()
{
    const uint8_t rc = getDefaultPassword();
	if (rc) {
		LOG(E) << "unable to retrieve MSID";
		return rc;
	}
	string defaultPassword = response.getString(5);
    fprintf(stdout, "MSID: %s\n", (char *)defaultPassword.c_str());
    return 0;
}
uint8_t MsedDevEnterprise::setSIDPassword(char * oldpassword, char * newpassword,
	uint8_t hasholdpwd, uint8_t hashnewpwd)
{
	LOG(D1) << "Entering MsedDevEnterprise::setSIDPassword()";

	vector<uint8_t> user;
    set8(user, OPALUID[OPAL_SID_UID]);

    vector<uint8_t> usercpin;
    set8(usercpin, OPALUID[OPAL_C_PIN_SID]);

	if (*oldpassword == '\0')
	{
		if (getDefaultPassword())
		{
			LOG(E) << "setPassword failed to retrieve MSID";
			return 0xff;
		}
		string defaultPassword = response.getString(5);
		session = new MsedSession(this);
		session->dontHashPwd();
		if (session->start(OPAL_UID::OPAL_ADMINSP_UID, (char *)defaultPassword.c_str(), user))
		{
			delete session;
			return 0xff;
		}
	}
	else
	{
		session = new MsedSession(this);
		if (!hasholdpwd) session->dontHashPwd();
		if (session->start(OPAL_UID::OPAL_ADMINSP_UID, oldpassword, user))
        {
			delete session;
			return 0xff;
		}
	}
	vector<uint8_t> hash;
	if (hashnewpwd)
    {
		MsedHashPwd(hash, newpassword, this);
	}
	else
    {
		hash.push_back(0xd0);
		hash.push_back((uint8_t)strnlen(newpassword, 255));
		for (uint16_t i = 0; i < strnlen(newpassword, 255); i++)
        {
			hash.push_back(newpassword[i]);
		}
	}
	if (setTable(usercpin, "PIN", hash))
    {
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
uint8_t MsedDevEnterprise::exec(MsedCommand * cmd, MsedResponse & resp, uint8_t protocol)
{
    uint8_t rc = 0;
    OPALHeader * hdr = (OPALHeader *) cmd->getCmdBuffer();
    LOG(D3) << endl << "Dumping command buffer";
    IFLOG(D) MsedAnnotatedDump(IF_SEND, cmd->getCmdBuffer(), IO_BUFFER_LENGTH);
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
    IFLOG(D) MsedAnnotatedDump(IF_RECV, cmd->getRespBuffer(), SWAP32(hdr->cp.length) + sizeof (OPALComPacket));
    IFLOG(D3) MsedHexDump(cmd->getRespBuffer(), SWAP32(hdr->cp.length) + sizeof (OPALComPacket));
    if (0 != rc) {
        LOG(E) << "Command failed on recv" << (uint16_t) rc;
        return rc;
    }
    resp.init(cmd->getRespBuffer());
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
#ifdef _MSC_VER
#pragma warning(pop)
#endif
