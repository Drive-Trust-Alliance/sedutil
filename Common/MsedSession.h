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
/*
 * Manage the session to a TPer.
 */
#include "MsedLexicon.h"
#include <vector>
class MsedCommand;
class MsedDev;
class MsedResponse;

using namespace std;
/** Encapsulate the session management functionality */
class MsedSession {
public:
    /** Constructor
     * @param device the device the session is to be conducted with
     */
    MsedSession(MsedDev * device);
    /** Destructor ends the session if required */
    ~MsedSession();
    /** start an anonymous session 
     * @param SP the Security Provider to start the session with */
    uint8_t start(OPAL_UID SP);    
    /** Start an authenticated session (OPAL only) 
     * @param SP the securitly provider to start the session with
     * @param HostChallenge the password to start the session
     * @param SignAuthority the Signing authority (in a simple session this is the user)
     *  */    
    uint8_t start(OPAL_UID SP, char * HostChallenge, OPAL_UID SignAuthority);
    /** Start an authenticated session (OPAL only) 
     * @param SP the securitly provider to start the session with
     * @param HostChallenge the password to start the session
     * @param SignAuthority the Signing authority (in a simple session this is the user)
     *  */
    uint8_t start(OPAL_UID SP, char * HostChallenge, vector<uint8_t> SignAuthority);
    /** Authenticate an already started session 
     * @param Authority the authority to authenticate
     * @param Challenge the password
     */
    uint8_t authenticate(vector<uint8_t> Authority, char * Challenge);
    /** assign the security protocol to be used in the sessiion
     * @param value the security protocol number 
     */
    void setProtocol(uint8_t value);
    /** The password is not to be hashed.
     *  This is used when the factory default password or the PSID password is
     * used to authenticate a session 
     */
    void dontHashPwd();
    /** expect the session to abort.
     * this is used when the method called will abort the session (revert)
     * to suppress the normal error checking 
     */
    void expectAbort();
    /** send a command to the device in this session 
     * @param cmd  The MsedCommand object 
     * @param response The MesdResponse object 
     */
    uint8_t sendCommand(MsedCommand * cmd, MsedResponse & response);
private:
    /** Default constructor, private should never be called */
    MsedSession();
    /** return a string explaining the method status 
     * @param status the method status code returned 
     */
    char * methodStatus(uint8_t status);
    MsedDev * d;   /**< Pointer to device this session is with */
    uint32_t bufferpos = 0;   /**< psooition in the response buffer the parser is at */
    uint32_t TSN = 0;   /**< TPer session number */
    uint32_t HSN = 0;  /**< Host session number */
    uint8_t willAbort = 0;   /**< Command is expected to abort */
	uint8_t hashPwd = 1;  /**< hash the password when authenticating */
    uint8_t SecurityProtocol = 0x01;  /**< The seurity protocol to be used */
	uint8_t lastRC;  /**< last return code */
};

