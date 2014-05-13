/* C:B**************************************************************************
This software is Copyright 2014 Michael Romeo <r0m30@r0m30.com>

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
#include <vector>
#include "MsedLexicon.h"
class MsedSession;
class MsedResponse;
using namespace std;

int initialsetup(char * password, char * devref);
int revertnoerase(char * SIDPassword, char * Admin1Password, char * devref);
int setSIDPassword(char * oldpassword, char * newpassword,
        char * devref, uint8_t hasholdpwd = 1, uint8_t hashnewpwd = 1);
int setLockingRange(uint8_t lockingrange, uint8_t lockingstate, char * Admin1Password, char * devref);
int getDefaultPassword(MsedResponse &response, char * devref);
int takeOwnership(char * devref, char * newpassword);
int activateLockingSP(char * devref, char * password);
int revertTPer(char * devref, char * password, uint8_t PSID = 0);
int revertLockingSP(char * devref, char * password, uint8_t keep = 0);
int diskScan();
int diskQuery(char * devref);
int setNewPassword(char * password, char * userid, char * newpassword, char * devref);
int enableUser(char * password, char * userid, char * devref);
int dumpTable(char * password, char * devref);
int getAuth4User(char * userid, uint8_t column, std::vector<uint8_t> &userData);
int getTable(MsedSession * session, vector<uint8_t> table,
        uint16_t startcol, uint16_t endcol, MsedResponse & response);
int setTable(MsedSession * session, vector<uint8_t> table,
        OPAL_TOKEN name, vector<uint8_t> value);
int nextTable(MsedSession * session, vector<uint8_t> table,
        vector<uint8_t> startkey, MsedResponse & response);
int MsedSetLSP(OPAL_UID table_uid, OPAL_TOKEN name, vector<uint8_t> value,
        char * password, char * devref, char * msg = (char *) "New Value Set");
