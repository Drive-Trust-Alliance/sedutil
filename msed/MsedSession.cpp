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
#include "MsedSession.h"
#include "MsedDev.h"
#include "MsedCommand.h"
#include "MsedResponse.h"
#include "MsedEndianFixup.h"
#include "MsedHexDump.h"
#include "MsedHashPwd.h"
#include "MsedStructures.h"

using namespace std;

/*
 * Start a session
 */
MsedSession::MsedSession(MsedDev * device)
{
    LOG(D4) << "Creating MsedSsession()";
    d = device;
}

uint8_t
MsedSession::start(OPAL_UID SP)
{
    return (start(SP, NULL, OPAL_UID::OPAL_UID_HEXFF));
}

uint8_t
MsedSession::start(OPAL_UID SP, char * HostChallenge, OPAL_UID SignAuthority)
{
    LOG(D4) << "Entering MsedSession::startSession ";
	vector<uint8_t> hash, salt(DEFAULTSALT);
    MsedCommand *cmd = new MsedCommand();
    MsedResponse response;
    cmd->reset(OPAL_UID::OPAL_SMUID_UID, OPAL_METHOD::STARTSESSION);
    cmd->addToken(OPAL_TOKEN::STARTLIST); // [  (Open Bracket)
    cmd->addToken(105); // HostSessionID : sessionnumber
    cmd->addToken(SP); // SPID : SP
    cmd->addToken(OPAL_TINY_ATOM::UINT_01); // write
    if (NULL != HostChallenge) {
        cmd->addToken(OPAL_TOKEN::STARTNAME);
        cmd->addToken(OPAL_TINY_ATOM::UINT_00);
		if (hashPwd) {
			hash.clear();
			MsedHashPwd(hash, HostChallenge, salt);
			cmd->addToken(hash);
		}
		else
		    cmd->addToken(HostChallenge);
        cmd->addToken(OPAL_TOKEN::ENDNAME);
        cmd->addToken(OPAL_TOKEN::STARTNAME);
        cmd->addToken(OPAL_TINY_ATOM::UINT_03);
        cmd->addToken(SignAuthority);
        cmd->addToken(OPAL_TOKEN::ENDNAME);
    }
    cmd->addToken(OPAL_TOKEN::ENDLIST); // ]  (Close Bracket)
    cmd->complete();
	if (sendCommand(cmd, response)) {
		LOG(E) << "Session start failed";
		delete cmd;
		return 0xff;
	}
   
    // call user method SL HSN TSN EL EOD SL 00 00 00 EL
    //   0   1     2     3  4   5   6  7   8

    HSN = SWAP32(response.getUint32(4));
    TSN = SWAP32(response.getUint32(5));
	delete cmd;
    return 0;
}

uint8_t
MsedSession::sendCommand(MsedCommand * cmd, MsedResponse & response)
{
    LOG(D4) << "Entering MsedSession::sendCommand()";
    cmd->setHSN(HSN);
    cmd->setTSN(TSN);
    cmd->setcomID(d->comID());

    d->exec(cmd, response, SecurityProtocol);
    /*
     * Check out the basics that so that we know we
     * have a sane reply to work with
     */
    // if outstanding data is <> 0 then the drive has more data but we dont support it
	if (0 != response.h.cp.outstandingData) {
		LOG(E) << "Outstanding data <> 0 -- no program support";
		return 0xff;
	}
    // zero lengths -- these are big endian but it doesn't matter for uint = 0
    if ((0 == response.h.cp.length) ||
        (0 == response.h.pkt.length) ||
        (0 == response.h.subpkt.length)) {
        LOG(E) << "One or more header fields have 0 length";
        return 0xff;
    }
    // if we get an endsession response return 0
    if ((1 == SWAP32(response.h.subpkt.length)) && (0xfa == response.tokenIs(0))) {
        return 0;
    }
    // IF we received a method status return it
    if (!(((uint8_t) OPAL_TOKEN::ENDLIST == (uint8_t) response.tokenIs(response.getTokenCount() - 1)) &&
        ((uint8_t) OPAL_TOKEN::STARTLIST == (uint8_t) response.tokenIs(response.getTokenCount() - 5)))) {
        // no method status so we hope we reported the error someplace else
        LOG(E) << "Method Status missing";
        return 0xff;
    }
    if (OPALSTATUSCODE::SUCCESS != response.getUint8(response.getTokenCount() - 4)) {
        LOG(E) << "method status code " <<
                methodStatus(response.getUint8(response.getTokenCount() - 4));
    }
    return response.getUint8(response.getTokenCount() - 4);
}

void
MsedSession::setProtocol(uint8_t value)
{
    LOG(D4) << "Entering MsedSession::setProtocol";
    SecurityProtocol = value;
}

void
MsedSession::dontHashPwd()
{
	LOG(D4) << "Entering MsedSession::setProtocol";
	hashPwd = 0;
}

void
MsedSession::expectAbort()
{
    LOG(D4) << "Entering MsedSession::methodStatus()";
    willAbort = 1;
}

char *
MsedSession::methodStatus(uint8_t status)
{
    LOG(D4) << "Entering MsedSession::methodStatus()";
    switch (status) {
    case OPALSTATUSCODE::AUTHORITY_LOCKED_OUT:
        return (char *) "AUTHORITY_LOCKED_OUT";
    case OPALSTATUSCODE::FAIL:
        return (char *) "FAIL";
    case OPALSTATUSCODE::INSUFFICIENT_ROWS:
        return (char *) "INSUFFICIENT_ROWS";
    case OPALSTATUSCODE::INSUFFICIENT_SPACE:
        return (char *) "INSUFFICIENT_SPACE";
    case OPALSTATUSCODE::INVALID_PARAMETER:
        return (char *) "INVALID_PARAMETER";
    case OPALSTATUSCODE::NOT_AUTHORIZED:
        return (char *) "NOT_AUTHORIZED";
    case OPALSTATUSCODE::NO_SESSIONS_AVAILABLE:
        return (char *) "NO_SESSIONS_AVAILABLE";
    case OPALSTATUSCODE::RESPONSE_OVERFLOW:
        return (char *) "RESPONSE_OVERFLOW";
    case OPALSTATUSCODE::SP_BUSY:
        return (char *) "SP_BUSY";
    case OPALSTATUSCODE::SP_DISABLED:
        return (char *) "SP_DISABLED";
    case OPALSTATUSCODE::SP_FAILED:
        return (char *) "SP_FAILED";
    case OPALSTATUSCODE::SP_FROZEN:
        return (char *) "SP_FROZEN";
    case OPALSTATUSCODE::SUCCESS:
        return (char *) "SUCCESS";
    case OPALSTATUSCODE::TPER_MALFUNCTION:
        return (char *) "TPER_MALFUNCTION";
    case OPALSTATUSCODE::TRANSACTION_FAILURE:
        return (char *) "TRANSACTION_FAILURE";
    case OPALSTATUSCODE::UNIQUENESS_CONFLICT:
        return (char *) "UNIQUENESS_CONFLICT";
    default:
        return (char *) "Unknown status code";
    }
}

MsedSession::~MsedSession()
{
    LOG(D4) << "Destroying MsedSession";
	MsedResponse response;
    if (!willAbort) {
        MsedCommand *cmd = new MsedCommand();
        cmd->reset();
        cmd->addToken(OPAL_TOKEN::ENDOFSESSION);
        cmd->complete(0);
        if (sendCommand(cmd, response)) {
            LOG(E) << "EndSession Failed";
        }
        delete cmd;
    }
}
