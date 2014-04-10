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
#include "diskList.h"
#include "TCGdev.h"
#include "hexDump.h"
#include "TCGcommand.h"
#include "TCGsession.h"
#include "endianfixup.h"
#include "TCGstructures.h"
#include "noparser.h"

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
    diskList * dl = new diskList();
    delete dl;
    return 0;
}

int takeOwnership(char * devref, char * newpassword)
{
    LOG(D4) << "Entering takeOwnership(char * devref, char * newpassword)";
    
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
    /* The pin is the ever so original "micron" so
     * I'll just use that instead of pretending
     * I'm parsing the reply
     */
    // session[TSN:HSN] <- EOS
    delete session;
    /*
     * We now have the PIN to sign on and take ownership
     * so lets give it a shot
     */
    //	Start Session
    session = new TCGsession(device);
    if (session->start(TCG_UID::TCG_ADMINSP_UID, (char *) "micron", TCG_UID::TCG_SID_UID))
        return 0xff;
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
	if (PSID){
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
    int rc = 0;
    GenericResponse * reply;
    /*
     * Activate the Locking SP
     */

    TCGdev *device = new TCGdev(devref);
	if (!(device->isOpal2())){
		delete device;
		return 0xff;
	}
    TCGcommand *cmd = new TCGcommand();
    TCGsession * session = new TCGsession(device);
	if (session->start(TCG_UID::TCG_ADMINSP_UID, password, TCG_UID::TCG_SID_UID)){
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
    if (session->sendCommand(cmd)) return 0xff;
    // verify response
    reply = (GenericResponse *) cmd->getRespBuffer();
    //if ((0x34 != SWAP32(reply->h.cp.length)) |
    // *BUG* micron/crucial m500 length field does not include padding
    if ((0x06 != reply->payload[3]) |
        (0x08 != reply->payload[4])
        ) {
        LOG(E) << "Get lifecycle Failed";
        goto exit;
    }
    // session[TSN:HSN] -> LockingSP_UID.Activate[]
    cmd->reset(TCG_UID::TCG_LOCKINGSP_UID, TCG_METHOD::ACTIVATE);
    cmd->addToken(TCG_TOKEN::STARTLIST);
    cmd->addToken(TCG_TOKEN::ENDLIST);
    cmd->complete();
    if (session->sendCommand(cmd)) return 0xff;
    // verify response
    reply = (GenericResponse *) cmd->getRespBuffer();
    // reply is empty list
    if ((0x2c != SWAP32(reply->h.cp.length)) |
        (0xf0 != reply->payload[0]) |
        (0xf1 != reply->payload[1])
        ) {
        LOG(E) << "LockingSP Activate Failed";
        goto exit;
    }
    LOG(I) << "Locking SP Activate Complete";
exit:
    /*  ******************  */
    /*  CLEANUP LEAVE HERE  */
    /*  ******************  */
	delete cmd;
    delete session;
	delete device;
    LOG(D4) << "Exiting activatLockingSP()";
    return rc;
}

int revertLockingSP(char * devref, char * password, uint8_t keep)
{
    LOG(D4) << "Entering revert LockingSP() keep = " << keep;
    int rc = 0;
	uint8_t keepgloballockingrange[] = { 0xa3, 0x06, 0x00, 0x00 };
    GenericResponse * reply;
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
    // verify response
    reply = (GenericResponse *) cmd->getRespBuffer();
    /* should return an empty list */
    if ((0xf0 != reply->payload[0]) |
        (0xf1 != reply->payload[1]) |
        (0xf9 != reply->payload[2])
        ) {
        LOG(E) << "revertSP Failed";
        goto exit;
    }
    LOG(I) << "Revert LockingSP complete";
	session->expectAbort();
exit:
    /*  ******************  */
    /*  CLEANUP LEAVE HERE  */
    /*  ******************  */
    delete session;
    delete device;
    LOG(D4) << "Exiting activateLockingSP()";
    return rc;
}
int setNewPassword(char * password, char * userid, char * newpassword, char * devref)
{
	LOG(D4) << "Entering setNewPassword" <<
		" ADMIN1 password = " << password << " user = " << userid <<
		" newpassword = " << newpassword <<	" device = " << devref;
//	int rc = 0;
//	GenericResponse * reply;
	/*
	* Set new password
	*/

	TCGdev *device = new TCGdev(devref);
	if (!(device->isOpal2())) {
		delete device;
		return 0xff;
	}
	TCGcommand *cmd = new TCGcommand();
	TCGsession * session = new TCGsession(device);
	// session[0:0]->SMUID.StartSession[HostSessionID:HSN, SPID : LockingSP_UID, Write : TRUE, 
	//               HostChallenge = <new_SID_password>, HostSigningAuthority = Admin1_UID]
	if (session->start(TCG_UID::TCG_LOCKINGSP_UID, password, TCG_UID::TCG_ADMIN1_UID)){
		delete cmd;
		delete session;
		delete device;
		return 0xff;
	}
	// session[TSN:HSN] -> C_PIN_user_UID.Set[Values = [PIN = <new_password>]]
		cmd->reset(TCG_UID::TCG_C_PIN_ADMIN1, TCG_METHOD::SET);
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
	LOG(I) << "ADMIN1 password changed to " << newpassword;
	// session[TSN:HSN] <- EOS
	delete session;
	delete device;
	LOG(D4) << "Exiting setNewPassword()";
	return 0;
}
int enableUser(char * password, char * userid, char * devref)
{
	LOG(D4) << "Enable User()" <<
		" ADMIN1 password = " << password << " user = " << userid << " on" << devref;
	/*
	* Eanable a user in the lockingSP
	*/
	TCGdev *device = new TCGdev(devref);
	if (!(device->isOpal2())) return 0xff;
	TCGcommand *cmd = new TCGcommand();
	TCGsession * session = new TCGsession(device);
	// session[0:0]->SMUID.StartSession[HostSessionID:HSN, SPID : LockingSP_UID, Write : TRUE, 
	//               HostChallenge = <new_SID_password>, HostSigningAuthority = Admin1_UID]
	if (session->start(TCG_UID::TCG_LOCKINGSP_UID, password, TCG_UID::TCG_ADMIN1_UID)) return 0xff;
	// session[TSN:HSN] -> User1_UID.Set[Values = [Enabled = TRUE]]
	cmd->reset(TCG_UID::TCG_USER1_UID, TCG_METHOD::SET);
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
	LOG(I) << "USER1 has been enabled ";
	// session[TSN:HSN] <- EOS
	delete cmd;
	delete session;
	delete device;
	LOG(D4) << "Exiting enable user()";
	return 0;
}
int getTable()
{
	CLog::Level() = CLog::FromInt(4);
	LOG(D4) << "Entering getTable()";
	TCGdev *device = new TCGdev("\\\\.\\PhysicalDrive3");
	if (!(device->isOpal2())) {
		delete device;
		return 0xff;
	}
	TCGcommand *get = new TCGcommand();
	TCGcommand *next = new TCGcommand();
	TCGsession * session = new TCGsession(device);
	// session[0:0]->SMUID.StartSession[HostSessionID:HSN, SPID : LockingSP_UID, Write : TRUE,
	//                   HostChallenge = <Admin1_password>, HostSigningAuthority = Admin1_UID]
	if (session->start(TCG_UID::TCG_ADMINSP_UID, "password", TCG_UID::TCG_SID_UID)) {
	//if (session->start(TCG_UID::TCG_LOCKINGSP_UID, "password", TCG_UID::TCG_ADMIN1_UID)) {
		delete get;
		delete next;
		delete session;
		delete device;
		return 0xff;
	}
	
	//AUTHORITY_TABLE.Get[Cellblock : [startColumn = 0, endColumn = 2]]
	get->reset(TCG_UID::TCG_AUTHORITY_TABLE, TCG_METHOD::GET);
	get->addToken(TCG_TOKEN::STARTLIST);
	get->addToken(TCG_TOKEN::STARTLIST);
	get->addToken(TCG_TOKEN::STARTNAME);
	get->addToken(TCG_TOKEN::STARTCOLUMN);
	get->addToken(TCG_TINY_ATOM::UINT_01);
	get->addToken(TCG_TOKEN::ENDNAME);
	get->addToken(TCG_TOKEN::STARTNAME);
	get->addToken(TCG_TOKEN::ENDCOLUMN);
	get->addToken(TCG_TINY_ATOM::UINT_10);
	get->addToken(TCG_TOKEN::ENDNAME);
	get->addToken(TCG_TOKEN::ENDLIST);
	get->addToken(TCG_TOKEN::ENDLIST);
	get->complete();
	next->reset(TCG_UID::TCG_AUTHORITY_TABLE, TCG_METHOD::NEXT);
	next->addToken(TCG_TOKEN::STARTLIST);
	//next->addToken(TCG_TOKEN::STARTNAME);
	//next->addToken(TCG_TINY_ATOM::UINT_00);
	//next->addToken(TCGUID[TCG_ADMIN1_UID],8);
	//next->addToken(TCG_TOKEN::ENDNAME);
	next->addToken(TCG_TOKEN::STARTNAME);
	next->addToken(TCG_TINY_ATOM::UINT_01);
	next->addToken(TCG_TINY_ATOM::UINT_02);
	next->addToken(TCG_TOKEN::ENDNAME);
	next->addToken(TCG_TOKEN::ENDLIST);
	next->complete();
	LOG(D1) << "Intitial Next";
	if (session->sendCommand(next)) {
		delete get;
		delete next;
		delete session;
		delete device;
		return 0xff;
	}
	while (true)
	{
		uint8_t nextuid[10];
		uint8_t a8[] = { 0xa8, 00 };
		GenericResponse * reply;
		// Get the next UID
		reply = (GenericResponse *)next->getRespBuffer();
		memcpy(&nextuid[0], &reply->payload[3], 8);
		IFLOG(D1) hexDump(nextuid, 8);
		LOG(D1) << "****LOOP**** get";
		get->changeInvokingUid(nextuid);
		if (session->sendCommand(get)) {
			delete get;
			delete next;
			delete session;
			delete device;
			return 0xff;
		}
		reply = (GenericResponse *)get->getRespBuffer();
		IFLOG(D1) hexDump(reply->payload, 128);
		next->reset(TCG_UID::TCG_AUTHORITY_TABLE, TCG_METHOD::NEXT);
		next->addToken(TCG_TOKEN::STARTLIST);
		next->addToken(TCG_TOKEN::STARTNAME);
		next->addToken(TCG_TINY_ATOM::UINT_00);
		reply = (GenericResponse *)next->getRespBuffer();
		if (0xa8 != reply->payload[11]) break; // no next row so bail
		memcpy(&nextuid[0], &reply->payload[12], 8);
		next->addToken(a8, 1);
		next->addToken(nextuid, 8);
		next->addToken(TCG_TOKEN::ENDNAME);
		next->addToken(TCG_TOKEN::STARTNAME);
		next->addToken(TCG_TINY_ATOM::UINT_01);
		next->addToken(TCG_TINY_ATOM::UINT_02);
		next->addToken(TCG_TOKEN::ENDNAME);
		next->addToken(TCG_TOKEN::ENDLIST);
		next->complete();
		LOG(D1) << "**NEXT***Next";
		if (session->sendCommand(next)) {
			delete get;
			delete next;
			delete session;
			delete device;
			return 0xff;
		}

	}
	delete get;
	delete next;
	delete session;
	delete device;
	return 0;
}