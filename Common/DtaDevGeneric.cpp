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
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#include "os.h"
#include <stdio.h>
#include <log/log.h>

#include <iostream>
#include <fstream>
#include<iomanip>
#include "DtaOptions.h"
#include "DtaDevGeneric.h"
#include "DtaHashPwd.h"
#include "DtaEndianFixup.h"
#include "DtaStructures.h"
#include "DtaCommand.h"
#include "DtaResponse.h"
#include "DtaSession.h"
#include "DtaHexDump.h"

using namespace std;

/** Class representing a disk device, this class is intended to be used when
 * it is not yet known if the device is a TPer
 */

#define voidNOCODE(name, ...) void DtaDevGeneric::name(##__VA_ARGS__) { \
LOG(E) << "Generic Device class does not support function " << #name << std::endl; \
}
#define uint8NOCODE(name, ...) uint8_t DtaDevGeneric::name(__VA_ARGS__) { \
LOG(E) << "Generic Device class does not support function " << #name << std::endl; \
return 0xff; \
}

DtaDevGeneric::DtaDevGeneric(const char * devref)
{
	DtaDevOS::init(devref);
}

DtaDevGeneric::~DtaDevGeneric()
{
}
void DtaDevGeneric::init(const char * devref)
{
}
uint8NOCODE(initialSetup, char *password)
uint8NOCODE(initialSetup, vector<uint8_t>HostChallenge)
uint8NOCODE(configureLockingRange,uint8_t lockingrange,
    uint8_t enabled, char * password, uint8_t idx)
uint8NOCODE(configureLockingRange,uint8_t lockingrange,
    uint8_t enabled, vector<uint8_t> HostChallenge, uint8_t idx)
uint8NOCODE(revertLockingSP,char * password, uint8_t keep)
uint8NOCODE(revertLockingSP,vector<uint8_t> HostChallenge, uint8_t keep)
uint8NOCODE(setup_SUM, uint8_t lockingrange, uint64_t start, uint64_t length, char *Admin1Password, char * password)
uint8NOCODE(setPassword,char * password, char * userid, char * newpassword, uint8_t idx)
uint8NOCODE(setHostChallenge,vector<uint8_t> oldHostChallenge, char * userid, vector<uint8_t> newHostChallenge, uint8_t idx)
uint8NOCODE(setNewPassword_SUM,char * password, char * userid, char * newpassword)
uint8NOCODE(setMBREnable,uint8_t mbrstate, char * Admin1Password)
uint8NOCODE(setMBREnable,uint8_t mbrstate, vector<uint8_t> Admin1HostChallenge)
uint8NOCODE(setMBRDone,uint8_t mbrstate, char * Admin1Password)
uint8NOCODE(setMBRDone,uint8_t mbrstate, vector<uint8_t> Admin1HostChallenge)
uint8NOCODE(TCGreset, uint8_t mbrstate)
uint8NOCODE(setLockingRange,uint8_t lockingrange, uint8_t lockingstate,
	char * Admin1Password, uint8_t idx)
uint8NOCODE(setLockingRange,uint8_t lockingrange, uint8_t lockingstate,
                        vector<uint8_t> Admin1HostChallenge, uint8_t idx)
uint8NOCODE(setLockingRange_SUM, uint8_t lockingrange, uint8_t lockingstate,
	char * password)
uint8NOCODE(setupLockingRange,uint8_t lockingrange, uint64_t start,
	uint64_t length, char * password)
uint8NOCODE(listLockingRanges, char * password, int16_t rangeid, uint8_t idx)
uint8NOCODE(setupLockingRange_SUM, uint8_t lockingrange, uint64_t start,
	uint64_t length, char * password)
uint8NOCODE(rekeyLockingRange, uint8_t lockingrange, char * password)
uint8NOCODE(setBandsEnabled, int16_t lockingrange, char * password)

//-0uint8NOCODE(enableUser,char * password, char * userid, OPAL_TOKEN status)
uint8NOCODE(enableUser, uint8_t state, char * password, char * userid)
uint8NOCODE(enableUser, uint8_t state, vector<uint8_t> HostChallenge, char * userid)
uint8NOCODE(enableUserRead, uint8_t state, char * password, char * userid)
uint8NOCODE(enableUserRead, uint8_t state, vector<uint8_t> HostChallenge, char * userid)
uint8NOCODE(revertTPer,char * password, uint8_t PSID, uint8_t AdminSP)
uint8NOCODE(revertTPer,vector<uint8_t> HostChallenge, uint8_t PSID, uint8_t AdminSP)
uint8NOCODE(eraseLockingRange,uint8_t lockingrange, char * password)
uint8NOCODE(getMSID,string& MSID)
uint8NOCODE(printDefaultPassword)
uint8NOCODE(activate, char * password)
uint8NOCODE(getmfgstate)
uint8NOCODE(getMBRsize, char * password)
uint8NOCODE(loadPBA,char * password, char * filename)
uint8NOCODE(activateLockingSP,char * password)
uint8NOCODE(activateLockingSP,vector<uint8_t>HostChallenge)
uint8NOCODE(activateLockingSP_SUM,uint8_t lockingrange, char * password)
uint8NOCODE(eraseLockingRange_SUM, uint8_t lockingrange, char * password)
uint8NOCODE(takeOwnership, char * newpassword)
uint8NOCODE(takeOwnership, vector<uint8_t> HostChallenge)
uint8NOCODE(setSIDPassword,char * oldpassword, char * newpassword,
    uint8_t hasholdpwd, uint8_t hashnewpwd)
uint8NOCODE(setSIDHostChallenge,vector<uint8_t> oldHostChallenge,
                                vector<uint8_t> newHostChallenge)

uint16_t DtaDevGeneric::comID()
{
	LOG(E) << "Generic Device class does not support function " << "comID" << std::endl;
		return 0xff;
}

uint8NOCODE(exec,DtaCommand * cmd, DtaResponse & resp, uint8_t protocol)
uint8NOCODE(objDump,char *sp, char * auth, char *pass,char * objID)
uint8NOCODE(rawCmd,char *sp, char * auth, char *pass,char *invoker, char *method, char *plist)
#ifdef _MSC_VER
#pragma warning(pop)
#endif
