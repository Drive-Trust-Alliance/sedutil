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
#include "MsedStructures.h"
#include "MsedLexicon.h"
#include <vector>
#include "MsedResponse.h"
class MsedCommand;
class MsedSession;

using namespace std;

class MsedDev {
public:
    MsedDev();
    ~MsedDev();
	/* 
	functions implemented in this base class
	 * returns basic disk information from IDENTIFY and
	 * discovery 0 
	 */
    uint8_t isOpal2();
    uint8_t isOpal1();
    uint8_t isEprise();
    uint8_t isAnySSC();
     uint8_t MBREnabled();
    uint8_t isPresent();
	uint8_t isSupportedSSC();
	char *getFirmwareRev();
	char *getModelNum();
	char *getSerialNum();
	void puke();

	/** Decode the Discovery 0 response. Scans the D0 response and creates a structure
	* that can be queried later as required.This code also takes care of
	* the endianess conversions either via a bitswap in the structure or executing
	* a macro when the input buffer is read.
	*/
	void discovery0();

	/* 
	 * virtual functions required in the OS specific 
	 * device class 
	 */
	virtual void init(const char * devref) = 0;
	virtual uint8_t sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
		void * buffer, uint16_t bufferlen) = 0;
	virtual void osmsSleep(uint32_t milliseconds) = 0;
	virtual void identify() = 0;
	/*
	 * virtual functions required to be implemented 
	 * because they are called by msed.cpp
	 */
	virtual uint8_t initialsetup(char * password) = 0;
	virtual uint8_t setSIDPassword(char * oldpassword, char * newpassword,
		uint8_t hasholdpwd = 1, uint8_t hashnewpwd = 1) = 0;
	virtual uint8_t setNewPassword(char * password, char * userid, char * newpassword) = 0;
	virtual uint8_t loadPBA(char * password, char * filename) = 0;
	virtual uint8_t setLockingRange(uint8_t lockingrange, uint8_t lockingstate,
		char * Admin1Password) = 0;
	virtual uint8_t configureLockingRange(uint8_t lockingrange, OPAL_TOKEN enabled, 
		char * password) = 0;
	virtual uint8_t setMBRDone(uint8_t state, char * Admin1Password) = 0;
	virtual uint8_t setMBREnable(uint8_t state, char * Admin1Password) = 0;
	virtual uint8_t enableUser(char * password, char * userid) = 0;
	virtual uint8_t activateLockingSP(char * password) = 0;
	virtual uint8_t diskQuery() = 0;
	virtual uint8_t takeOwnership(char * newpassword) = 0;
	virtual uint8_t revertLockingSP(char * password, uint8_t keep = 0) = 0;	
	virtual uint8_t revertTPer(char * password, uint8_t PSID = 0) = 0;
	virtual uint8_t objDump(char *sp, char * auth, char *pass,
		char * objID) = 0;
	virtual uint8_t rawCmd(char *sp, char * auth, char *pass,
		char *invoker, char *method, char *plist) = 0;
	/*
	* virtual functions required to be implemented
	* because they are called by MsedSession.cpp
	*/
	virtual uint8_t exec(MsedCommand * cmd, MsedResponse &response, 
		uint8_t protocol = 0x01) = 0;
	virtual uint16_t comID() = 0;
protected:
    const char * dev;
    uint8_t isOpen = FALSE;
    OPAL_DiskInfo disk_info;
	MsedResponse response;
	MsedResponse propertiesResponse;
	MsedSession *session;

};