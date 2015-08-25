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
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#include "os.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include<iomanip>
#include "MsedDevGeneric.h"
#include "MsedHashPwd.h"
#include "MsedEndianFixup.h"
#include "MsedStructures.h"
#include "MsedCommand.h"
#include "MsedResponse.h"
#include "MsedSession.h"
#include "MsedHexDump.h"

using namespace std;

/** Class representing a disk device, this class is intended to be used when
 * it is not yet known if the device is OPAL compliant
 */

#define voidNOCODE(name, ...) void MsedDevGeneric::name(##__VA_ARGS__) { \
LOG(E) << "Generic Device class does not support function " << #name << std::endl; \
}
#define uint8NOCODE(name, ...) uint8_t MsedDevGeneric::name(__VA_ARGS__) { \
LOG(E) << "Generic Device class does not support function " << #name << std::endl; \
return 0xff; \
}

MsedDevGeneric::MsedDevGeneric(const char * devref)
{
	MsedDevOS::init(devref);
}

MsedDevGeneric::~MsedDevGeneric()
{
}
void MsedDevGeneric::init(const char * devref)
{
}
uint8NOCODE(initialsetup, char *password)
uint8NOCODE(configureLockingRange,uint8_t lockingrange, 
	uint8_t enabled, char * password)
uint8NOCODE(revertLockingSP,char * password, uint8_t keep)
uint8NOCODE(setNewPassword,char * password, char * userid, char * newpassword)
uint8NOCODE(setMBREnable,uint8_t mbrstate, char * Admin1Password)
uint8NOCODE(setMBRDone,uint8_t mbrstate, char * Admin1Password)
uint8NOCODE(setLockingRange,uint8_t lockingrange, uint8_t lockingstate,
	char * Admin1Password)
uint8NOCODE(setupLockingRange,uint8_t lockingrange, uint64_t start,
	uint64_t length, char * password)
uint8NOCODE(listLockingRanges, char * password)
uint8NOCODE(enableUser,char * password, char * userid)
uint8NOCODE(revertTPer,char * password, uint8_t PSID)
uint8NOCODE(loadPBA,char * password, char * filename)
uint8NOCODE(activateLockingSP,char * password)
uint8NOCODE(takeOwnership, char * newpassword)
uint8NOCODE(setSIDPassword,char * oldpassword, char * newpassword,
	uint8_t hasholdpwd, uint8_t hashnewpwd)
uint16_t MsedDevGeneric::comID()
{
	LOG(E) << "Generic Device class does not support function " << "comID" << std::endl; 
		return 0xff; 
}

uint8NOCODE(exec,MsedCommand * cmd, MsedResponse & resp, uint8_t protocol)
uint8NOCODE(objDump,char *sp, char * auth, char *pass,char * objID)
uint8NOCODE(rawCmd,char *sp, char * auth, char *pass,char *invoker, char *method, char *plist)
#ifdef _MSC_VER
#pragma warning(pop)
#endif
