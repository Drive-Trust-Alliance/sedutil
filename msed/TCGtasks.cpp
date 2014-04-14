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
#include "os.h"
#include <stdio.h>
#include <iostream>
#include "TCGdev.h"
#include "hexDump.h"
#include "TCGcommand.h"
#include "TCGsession.h"
#include "endianfixup.h"
#include "TCGstructures.h"
#include "TCGresponse.h"
#include "TCGtasks.h"

using namespace std;
int diskQuery(char * devref)
{
    LOG(D4) << "Entering diskQuery()" << devref;
    TCGdev * dev = new TCGdev(devref);
    if (!dev->isPresent()) {
        LOG(E) << "Device not present" << devref;
        return 1;
    }
    if (!dev->isOpal2()) {
        return 1;
    }
    dev->puke();
    delete dev;
    return 0;
}

int diskScan()
{
/** Brute force disk scan.
 * loops through the physical devices until
 * there is an open error.Creates a Device
 * and reports OPAL support.
 */
	char devname[25];
	int i = 0;
	uint8_t FirmwareRev[8];
	uint8_t ModelNum[40];
	TCGdev * d;
	LOG(D4) << "Creating diskList";
	printf("\nScanning for Opal 2.0 compliant disks\n");
	while (TRUE) {
		SNPRINTF(devname, 23, DEVICEMASK, i);
		//		sprintf_s(devname, 23, "\\\\.\\PhysicalDrive3", i);
		d = new TCGdev(devname);
		if (d->isPresent()) {
			d->getFirmwareRev(FirmwareRev);
			d->getModelNum(ModelNum);
			printf("%s %s", devname, (d->isOpal2() ? " Yes " : " No "));
			for (int x = 0; x < sizeof(ModelNum); x++) {
				//if (0x20 == ModelNum[x]) break;
				cout << ModelNum[x];
			}
			cout << " ";
			for (int x = 0; x < sizeof(FirmwareRev); x++) {
				//if (0x20 == FirmwareRev[x]) break;
				cout << FirmwareRev[x];
			}
			cout << std::endl;
			if (MAX_DISKS == i) {
				LOG(I) << MAX_DISKS << " disks, really?";
				delete d;
				break;
			}
		}
		else break;
		delete d;
		i += 1;
	}
	delete d;
	printf("No more disks present ending scan\n");
	return 0;
}

int takeOwnership(char * devref, char * newpassword)
{
    LOG(D4) << "Entering takeOwnership(char * devref, char * newpassword)";
    TCGresponse * response;
    TCGdev *device = new TCGdev(devref);
    if (!(device->isOpal2())) {
        delete device;
        return 0xff;
    }
    //	Start Session
    TCGsession * session = new TCGsession(device);
    if (session->start(TCG_UID::TCG_ADMINSP_UID)) {
        delete session;
        delete device;
        return 0xff;
    }
    // Get the default password
    // session[TSN:HSN] -> C_PIN_MSID_UID.Get[Cellblock : [startColumn = PIN,
    //                       endColumn = PIN]]
    TCGcommand *cmd = new TCGcommand();
    cmd->reset(TCG_UID::TCG_C_PIN_MSID, TCG_METHOD::GET);
    cmd->addToken(TCG_TOKEN::STARTLIST);
    cmd->addToken(TCG_TOKEN::STARTLIST);
    cmd->addToken(TCG_TOKEN::STARTNAME);
    cmd->addToken(TCG_TOKEN::STARTCOLUMN);
    cmd->addToken(TCG_TINY_ATOM::UINT_03); // column 3 is the PIN
    cmd->addToken(TCG_TOKEN::ENDNAME);
    cmd->addToken(TCG_TOKEN::STARTNAME);
    cmd->addToken(TCG_TOKEN::ENDCOLUMN);
    cmd->addToken(TCG_TINY_ATOM::UINT_03); // column 3 is the PIN
    cmd->addToken(TCG_TOKEN::ENDNAME);
    cmd->addToken(TCG_TOKEN::ENDLIST);
    cmd->addToken(TCG_TOKEN::ENDLIST);
    cmd->complete();
    if (session->sendCommand(cmd)) {
        delete cmd;
        delete session;
        delete device;
        return 0xff;
    }

    response = new TCGresponse(cmd->getRespBuffer());
    // SL,SL,SN,NAME,Value
    //  0  1  2   3    4

    // session[TSN:HSN] <- EOS
    delete session;
    /*
     * We now have the PIN, sign on and take ownership
     */
    //	Start Session
    session = new TCGsession(device);
    if (session->start(TCG_UID::TCG_ADMINSP_UID, (char *) response->getString(4).c_str(), TCG_UID::TCG_SID_UID)) {
        delete response;
        delete cmd;
        delete session;
        delete device;
        return 0xff;
    }
    delete response;
    // session[TSN:HSN] -> C_PIN_SID_UID.Set[Values = [PIN = <new_SID_password>]]
    /*
     * Change the password --- Yikes!!!
     */
    cmd->reset(TCG_UID::TCG_C_PIN_SID, TCG_METHOD::SET);
    cmd->addToken(TCG_TOKEN::STARTLIST);
    cmd->addToken(TCG_TOKEN::STARTNAME);
    cmd->addToken(TCG_TINY_ATOM::UINT_01); // Values
    cmd->addToken(TCG_TOKEN::STARTLIST);
    cmd->addToken(TCG_TOKEN::STARTNAME);
    cmd->addToken(TCG_TINY_ATOM::UINT_03); // PIN
    cmd->addToken(newpassword);
    cmd->addToken(TCG_TOKEN::ENDNAME);
    cmd->addToken(TCG_TOKEN::ENDLIST);
    cmd->addToken(TCG_TOKEN::ENDNAME);
    cmd->addToken(TCG_TOKEN::ENDLIST);
    cmd->complete();
    if (session->sendCommand(cmd)) {
        delete cmd;
        delete session;
        delete device;
        return 0xff;
    }
    LOG(I) << "takeownership complete new SID password = " << newpassword;
    // session[TSN:HSN] <- EOS
    delete cmd;
    delete session;
    delete device;
    LOG(D4) << "Exiting changeInitialPassword()";
    return 0;
}

int revertTPer(char * devref, char * password, uint8_t PSID)
{
    LOG(D4) << "Entering revertTPer(char * devref, char * password)";
    TCGdev *device = new TCGdev(devref);
    if (!(device->isOpal2())) {
        delete device;
        return 0xff;
    }
    TCGcommand *cmd = new TCGcommand();
    TCGsession * session = new TCGsession(device);
    if (PSID) {
        if (session->start(TCG_UID::TCG_ADMINSP_UID, password, TCG_UID::TCG_PSID_UID)) {
            delete cmd;
            delete session;
            delete device;
            return 0xff;
        }
    }
    else {
        if (session->start(TCG_UID::TCG_ADMINSP_UID, password, TCG_UID::TCG_SID_UID)) {
            delete cmd;
            delete session;
            delete device;
            return 0xff;
        }
    }
    //	session[TSN:HSN]->AdminSP_UID.Revert[]
    cmd->reset(TCG_UID::TCG_ADMINSP_UID, TCG_METHOD::REVERT);
    cmd->addToken(TCG_TOKEN::STARTLIST);
    cmd->addToken(TCG_TOKEN::ENDLIST);
    cmd->complete();
    session->expectAbort();
    if (session->sendCommand(cmd)) {
        delete cmd;
        delete session;
        delete device;
        return 0xff;
    }
    LOG(I) << "revertTper completed successfully";
    delete cmd;
    delete session;
    delete device;
    LOG(D4) << "Exiting RevertTperevertTPer()";
    return 0;
}

int activateLockingSP(char * devref, char * password)
{
    LOG(D4) << "Entering activateLockingSP()";
    TCGresponse * response;
    /*
     * Activate the Locking SP
     */

    TCGdev *device = new TCGdev(devref);
    if (!(device->isOpal2())) {
        delete device;
        return 0xff;
    }
    TCGcommand *cmd = new TCGcommand();
    TCGsession * session = new TCGsession(device);
    if (session->start(TCG_UID::TCG_ADMINSP_UID, password, TCG_UID::TCG_SID_UID)) {
        delete cmd;
        delete session;
        delete device;
        return 0xff;
    }
    //session[TSN:HSN]->LockingSP_UID.Get[Cellblock:[startColumn = LifeCycle,
    //                                               endColumn = LifeCycle]]
    cmd->reset(TCG_UID::TCG_LOCKINGSP_UID, TCG_METHOD::GET);
    cmd->addToken(TCG_TOKEN::STARTLIST);
    cmd->addToken(TCG_TOKEN::STARTLIST);
    cmd->addToken(TCG_TOKEN::STARTNAME);
    cmd->addToken(TCG_TOKEN::STARTCOLUMN);
    cmd->addToken(TCG_TINY_ATOM::UINT_06); // LifeCycle
    cmd->addToken(TCG_TOKEN::ENDNAME);
    cmd->addToken(TCG_TOKEN::STARTNAME);
    cmd->addToken(TCG_TOKEN::ENDCOLUMN);
    cmd->addToken(TCG_TINY_ATOM::UINT_06); // LifeCycle
    cmd->addToken(TCG_TOKEN::ENDNAME);
    cmd->addToken(TCG_TOKEN::ENDLIST);
    cmd->addToken(TCG_TOKEN::ENDLIST);
    cmd->complete();
    if (session->sendCommand(cmd)) {
        delete cmd;
        delete session;
        delete device;
        return 0xff;
    }
    response = new TCGresponse(cmd->getRespBuffer());
    // SL,SL,SN,NAME,Value
    //  0  1  2   3    4
    // verify response
    if ((0x06 != response->getUint8(3)) | // getlifecycle
        (0x08 != response->getUint8(4))) // Manufactured-Inactive
    {
        LOG(E) << "Locking SP lifecycle is not Manufactured-Inactive";
        delete cmd;
        delete response;
        delete session;
        delete device;
        return 0xff;
    }
    delete response;
    // session[TSN:HSN] -> LockingSP_UID.Activate[]
    cmd->reset(TCG_UID::TCG_LOCKINGSP_UID, TCG_METHOD::ACTIVATE);
    cmd->addToken(TCG_TOKEN::STARTLIST);
    cmd->addToken(TCG_TOKEN::ENDLIST);
    cmd->complete();
    if (session->sendCommand(cmd)) {
        delete cmd;
        delete session;
        delete device;
        return 0xff;
    }
    // method response code 00
    LOG(I) << "Locking SP Activate Complete";

    delete cmd;
    delete session;
    delete device;
    LOG(D4) << "Exiting activatLockingSP()";
    return 0;
}

int revertLockingSP(char * devref, char * password, uint8_t keep)
{
    LOG(D4) << "Entering revert LockingSP() keep = " << keep;
    uint8_t keepgloballockingrange[] = {0xa3, 0x06, 0x00, 0x00};
    /*
     * revert the Locking SP
     */

    TCGdev *device = new TCGdev(devref);
    if (!(device->isOpal2())) {
        delete device;
        return 0xff;
    }
    TCGcommand *cmd = new TCGcommand();
    TCGsession * session = new TCGsession(device);
    // session[0:0]->SMUID.StartSession[HostSessionID:HSN, SPID : LockingSP_UID, Write : TRUE,
    //                   HostChallenge = <Admin1_password>, HostSigningAuthority = Admin1_UID]
    if (session->start(TCG_UID::TCG_LOCKINGSP_UID, password, TCG_UID::TCG_ADMIN1_UID)) {
        delete cmd;
        delete session;
        delete device;
        return 0xff;
    }
    // session[TSN:HSN]->ThisSP.RevertSP[]
    cmd->reset(TCG_UID::TCG_THISSP_UID, TCG_METHOD::REVERTSP);
    cmd->addToken(TCG_TOKEN::STARTLIST);
    if (keep) {
        cmd->addToken(TCG_TOKEN::STARTNAME);
        //KeepGlobalRangeKey SHALL be 0x060000  ????????
        cmd->addToken(keepgloballockingrange, 3);
        cmd->addToken(TCG_TINY_ATOM::UINT_01); // KeepGlobalRangeKey = TRUE
        cmd->addToken(TCG_TOKEN::ENDNAME);
    }
    cmd->addToken(TCG_TOKEN::ENDLIST);
    cmd->complete();
    if (session->sendCommand(cmd)) {
        delete cmd;
        delete session;
        delete device;
        return 0xff;
    }
    // empty list returned so rely on method status
    LOG(I) << "Revert LockingSP complete";
    session->expectAbort();

    delete session;
    delete device;
    LOG(D4) << "Exiting activateLockingSP()";
    return 0;
}

int setNewPassword(char * password, char * userid, char * newpassword, char * devref)
{
    LOG(D4) << "Entering setNewPassword" <<
            " Admin1 password = " << password << " user = " << userid <<
            " newpassword = " << newpassword << " device = " << devref;

	std::vector<uint8_t> userCPIN;
    TCGdev *device = new TCGdev(devref);
    if (!(device->isOpal2())) {
        delete device;
        return 0xff;
    }
    TCGcommand *cmd = new TCGcommand();
    TCGsession * session = new TCGsession(device);
    // session[0:0]->SMUID.StartSession[HostSessionID:HSN, SPID : LockingSP_UID, Write : TRUE,
    //               HostChallenge = <new_SID_password>, HostSigningAuthority = Admin1_UID]
    if (session->start(TCG_UID::TCG_LOCKINGSP_UID, password, TCG_UID::TCG_ADMIN1_UID)) {
        delete cmd;
        delete session;
        delete device;
        return 0xff;
    }
	// Get C_PIN ID  for user from Authority table
	if (getAuth4User(userid, 10, userCPIN, session)) {
		LOG(E) << "Unable to find user " << userid << " in Authority Table";
		delete cmd;
		delete session;
		delete device;
		return 0xff;
	}
    // session[TSN:HSN] -> C_PIN_user_UID.Set[Values = [PIN = <new_password>]]
    cmd->reset(TCG_UID::TCG_C_PIN_ADMIN1, TCG_METHOD::SET);
	cmd->changeInvokingUid(userCPIN);
    cmd->addToken(TCG_TOKEN::STARTLIST);
    cmd->addToken(TCG_TOKEN::STARTNAME);
    cmd->addToken(TCG_TINY_ATOM::UINT_01); // Values
    cmd->addToken(TCG_TOKEN::STARTLIST);
    cmd->addToken(TCG_TOKEN::STARTNAME);
    cmd->addToken(TCG_TINY_ATOM::UINT_03); // PIN
    cmd->addToken(newpassword);
    cmd->addToken(TCG_TOKEN::ENDNAME);
    cmd->addToken(TCG_TOKEN::ENDLIST);
    cmd->addToken(TCG_TOKEN::ENDNAME);
    cmd->addToken(TCG_TOKEN::ENDLIST);
    cmd->complete();
    if (session->sendCommand(cmd)) {
        delete cmd;
        delete session;
        delete device;
        return 0xff;
    }
    LOG(I) << userid << " password changed to " << newpassword;
    // session[TSN:HSN] <- EOS
    delete session;
    delete device;
    LOG(D4) << "Exiting setNewPassword()";
    return 0;
}

int enableUser(char * password, char * userid, char * devref)
{
    LOG(D4) << "Enable User()" <<
            " Admin1 password = " << password << " user = " << userid << " on" << devref;
    /*
     * Enable a user in the lockingSP
     */
	std::vector<uint8_t> userUID;
    TCGdev *device = new TCGdev(devref);
    if (!(device->isOpal2())) return 0xff;
    TCGcommand *cmd = new TCGcommand();
    TCGsession * session = new TCGsession(device);
    // session[0:0]->SMUID.StartSession[HostSessionID:HSN, SPID : LockingSP_UID, Write : TRUE,
    //               HostChallenge = <new_SID_password>, HostSigningAuthority = Admin1_UID]
	if (session->start(TCG_UID::TCG_LOCKINGSP_UID, password, TCG_UID::TCG_ADMIN1_UID)) {
		delete cmd;
		delete session;
		delete device;
		return 0xff;
	}
	// Get UID for user from Authority table
	if (getAuth4User(userid, 0, userUID, session)) {
		LOG(E) << "Unable to find user " << userid << " in Authority Table";
		delete cmd;
		delete session;
		delete device;
		return 0xff;
	}
    // session[TSN:HSN] -> User1_UID.Set[Values = [Enabled = TRUE]]
    cmd->reset(TCG_UID::TCG_USER1_UID, TCG_METHOD::SET);
	cmd->changeInvokingUid(userUID);
    cmd->addToken(TCG_TOKEN::STARTLIST);
    cmd->addToken(TCG_TOKEN::STARTNAME);
    cmd->addToken(TCG_TINY_ATOM::UINT_01); // Values
    cmd->addToken(TCG_TOKEN::STARTLIST);
    cmd->addToken(TCG_TOKEN::STARTNAME);
    cmd->addToken(TCG_TINY_ATOM::UINT_05); // Enabled
    cmd->addToken(TCG_TINY_ATOM::UINT_01); // TRUE
    cmd->addToken(TCG_TOKEN::ENDNAME);
    cmd->addToken(TCG_TOKEN::ENDLIST);
    cmd->addToken(TCG_TOKEN::ENDNAME);
    cmd->addToken(TCG_TOKEN::ENDLIST);
    cmd->complete();
    if (session->sendCommand(cmd)) {
        delete cmd;
        delete session;
        delete device;
        return 0xff;
    }
    LOG(I) << userid << " has been enabled ";
    // session[TSN:HSN] <- EOS
    delete cmd;
    delete session;
    delete device;
    LOG(D4) << "Exiting enable user()";
    return 0;
}

int getAuth4User(char * userid, uint8_t column, std::vector<uint8_t> &userData, TCGsession * session)
{
   LOG(D4) << "Entering getAuth4User()";
	std::vector<uint8_t> table, key, nextkey;
	TCGresponse response;
	// build a token for the authority table
	table.push_back(0xa8);
	for (int i = 0; i < 8; i++){
		table.push_back(TCGUID[TCG_UID::TCG_AUTHORITY_TABLE][i]);
	}
 	key.clear();
	while (true) {
// Get the next UID
		if (nextTable(session, table, key, response)) {
			return 0xff;
		}
		key = response.getRawToken(2);
		nextkey = response.getRawToken(3);

		//AUTHORITY_TABLE.Get[Cellblock : [startColumn = 0, endColumn = 10]]
		if (getTable(session, key, 1, 1, response)){
			return 0xff;
		}
		LOG(D1) << "Dumping Response";
		IFLOG(D1) hexDump(response.getRawToken(4).data(), response.getRawToken(4).size());
		if (!(strcmp(userid, response.getString(4).c_str()))) {
			if (getTable(session, key, column, column, response)){
				return 0xff;
			}
			userData = response.getRawToken(4);
			return 0x00;
		}
		
		if (9 != nextkey.size()) break; // no next row so bail
		key = nextkey;
	}
    return 0xff;
}
int dumpTable()
{
	CLog::Level() = CLog::FromInt(4);
	LOG(D4) << "Entering getTable()";
	std::vector<uint8_t> table, key, nextkey;
	table.push_back(0xa8);
	for (int i = 0; i < 8; i++){
		table.push_back(TCGUID[TCG_UID::TCG_AUTHORITY_TABLE][i]);
	}
	TCGdev *device = new TCGdev("\\\\.\\PhysicalDrive3");
	if (!(device->isOpal2())) {
		delete device;
		return 0xff;
	}
	TCGsession * session = new TCGsession(device);
	TCGresponse response;
	// session[0:0]->SMUID.StartSession[HostSessionID:HSN, SPID : LockingSP_UID, Write : TRUE,
	//                   HostChallenge = <Admin1_password>, HostSigningAuthority = Admin1_UID]
	//if (session->start(TCG_UID::TCG_ADMINSP_UID, (char *) "password", TCG_UID::TCG_SID_UID)) {
	if (session->start(TCG_UID::TCG_LOCKINGSP_UID, "password", TCG_UID::TCG_ADMIN1_UID)) {
		delete session;
		delete device;
		return 0xff;
	}

	key.clear();
	while (true) {
		// Get the next UID
		if (nextTable(session, table, key, response)) {
			delete session;
			delete device;
			return 0xff;
		}
		key = response.getRawToken(2);
		nextkey = response.getRawToken(3);

		//AUTHORITY_TABLE.Get[Cellblock : [startColumn = 0, endColumn = 10]]

		if (getTable(session, key, 1, 10, response)){
			delete session;
			delete device;
			return 0xff;
		}
		std::vector<uint8_t> temp = response.getRawToken(4);
		LOG(D1) << "Dumping Response";
		hexDump(temp.data(), temp.size());
		temp = response.getRawToken(20);
		hexDump(temp.data(), temp.size());

		//if (getTable(session, key, 5, 5, response)){
		//	delete session;
		//	delete device;
		//	return 0xff;
		//}
		//temp = response.getRawToken(4);
		//hexDump(temp.data(), temp.size());

		//if (getTable(session, key, 10, 10, response)){
		//	delete session;
		//	delete device;
		//	return 0xff;
		//}
		//temp = response.getRawToken(4);
		//hexDump(temp.data(), temp.size());

		if (9 != nextkey.size()) break; // no next row so bail
		key = nextkey;
	}
	delete session;
	delete device;
	return 0;
}
int nextTable(TCGsession * session, std::vector<uint8_t> table,
	std::vector<uint8_t> startkey, TCGresponse & response) {
	LOG(D4) << "Entering nextTable";
	TCGcommand *next = new TCGcommand();
	next->reset(TCG_UID::TCG_AUTHORITY_TABLE, TCG_METHOD::NEXT);
	next->changeInvokingUid(table);
	next->addToken(TCG_TOKEN::STARTLIST);
	if (0 != startkey.size()) {
		next->addToken(TCG_TOKEN::STARTNAME);
		next->addToken(TCG_TINY_ATOM::UINT_00);
		//startkey[0] = 0x00;
		next->addToken(startkey);
		next->addToken(TCG_TOKEN::ENDNAME);
	}
	next->addToken(TCG_TOKEN::STARTNAME);
	next->addToken(TCG_TINY_ATOM::UINT_01);
	next->addToken(TCG_TINY_ATOM::UINT_02);
	next->addToken(TCG_TOKEN::ENDNAME);
	next->addToken(TCG_TOKEN::ENDLIST);
	next->complete();
	if (session->sendCommand(next)) {
		delete next;
		return 0xff;
	}
	response.init(next->getRespBuffer());
	delete next;
	return 0;
}
int getTable(TCGsession * session, std::vector<uint8_t> table,
	uint16_t startcol, uint16_t endcol, TCGresponse & response) {
	LOG(D4) << "Entering getTable";
	TCGcommand *get = new TCGcommand();
	get->reset(TCG_UID::TCG_AUTHORITY_TABLE, TCG_METHOD::GET);
	get->changeInvokingUid(table);
	get->addToken(TCG_TOKEN::STARTLIST);
	get->addToken(TCG_TOKEN::STARTLIST);
	get->addToken(TCG_TOKEN::STARTNAME);
	get->addToken(TCG_TOKEN::STARTCOLUMN);
	get->addToken(startcol);
	get->addToken(TCG_TOKEN::ENDNAME);
	get->addToken(TCG_TOKEN::STARTNAME);
	get->addToken(TCG_TOKEN::ENDCOLUMN);
	get->addToken(endcol);
	get->addToken(TCG_TOKEN::ENDNAME);
	get->addToken(TCG_TOKEN::ENDLIST);
	get->addToken(TCG_TOKEN::ENDLIST);
	get->complete();
	if (session->sendCommand(get)) {
		delete get;
		return 0xff;
	}
	response.init(get->getRespBuffer());
	delete get;
	return 0;
}
