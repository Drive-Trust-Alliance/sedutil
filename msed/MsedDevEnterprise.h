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
#pragma once
class MsedCommand;
class MsedSession;

#include "os.h"
#include "MsedDev.h"
#include "MsedDevOS.h"
#include "MsedStructures.h"
#include "MsedLexicon.h"
#include "MsedResponse.h"   // wouldn't take class
#include <vector>

using namespace std;
/** Device Class represents a disk device, conforming to the TCG Enterprise standard
*  This is the functionality that is common to all OS's
*/

class MsedDevEnterprise : public MsedDevOS {
public:
	MsedDevEnterprise(const char * devref);
	~MsedDevEnterprise();
	uint8_t properties();
	uint8_t exec(MsedCommand * cmd, MsedResponse &response, uint8_t protocol = 0x01);
	uint16_t comID();
	uint8_t takeOwnership(char * newpassword);
	uint8_t initLSPUsers(char * defaultPassword, char * newPassword);
	uint8_t getDefaultPassword();
	uint8_t getTable(vector<uint8_t> table, const char * startcol,
		const char * endcol);
	uint8_t setSIDPassword(char * oldpassword, char * newpassword,
		uint8_t hasholdpwd = 1, uint8_t hashnewpwd = 1);
	uint8_t setTable(vector<uint8_t> table, const char *name,
		vector<uint8_t> value);
	uint8_t setTable(vector<uint8_t> table, const char *name,
		OPAL_TOKEN value);
	uint8_t diskQuery();
	uint8_t activateLockingSP(char * password);
	uint8_t revertLockingSP(char * password, uint8_t keep = 0);
	uint8_t getAuth4User(char * userid, uint8_t column, std::vector<uint8_t> &userData);
	uint8_t enableUser(char * password, char * userid);
	uint8_t setMBRDone(uint8_t state, char * Admin1Password);
	uint8_t setMBREnable(uint8_t state, char * Admin1Password);
	uint8_t setNewPassword(char * password, char * userid, char * newpassword);
	uint8_t setLockingRange(uint8_t lockingrange, uint8_t lockingstate,
		char * assword);
	uint8_t configureLockingRange(uint8_t lockingrange, OPAL_TOKEN enabled, char * password);

	uint8_t revertTPer(char * password, uint8_t PSID = 0);
	uint8_t loadPBA(char * password, char * filename);
	uint8_t initialsetup(char * password);
	void puke();
	uint8_t objDump(char *sp, char * auth, char *pass, char * objID);
	uint8_t rawCmd(char *sp, char *hexauth, char *pass,
		char *hexinvokingUID, char *hexmethod, char *hexparms);

protected:
	
};