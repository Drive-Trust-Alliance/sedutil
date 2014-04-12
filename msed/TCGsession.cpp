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
#include "TCGsession.h"
#include "TCGdev.h"
#include "TCGcommand.h"
#include "TCGresponse.h"
#include "endianfixup.h"
#include "hexDump.h"
#include "TCGstructures.h"

/*
 * Start a session
 */
TCGsession::TCGsession(TCGdev * device)
{
    LOG(D4) << "Creating TCGsession()";
    d = device;
}

uint8_t
TCGsession::start(TCG_UID SP, char * HostChallenge, TCG_UID SignAuthority)
{
    LOG(D4) << "Entering TCGsession::startSession ";
    TCGcommand *cmd = new TCGcommand();
    TCGresponse * response;
    cmd->reset(TCG_UID::TCG_SMUID_UID, TCG_METHOD::STARTSESSION);
    cmd->addToken(TCG_TOKEN::STARTLIST); // [  (Open Bracket)
    cmd->addToken(105); // HostSessionID : sessionnumber
    cmd->addToken(SP); // SPID : SP
    cmd->addToken(TCG_TINY_ATOM::UINT_01); // write
    if (NULL != HostChallenge) {
        cmd->addToken(TCG_TOKEN::STARTNAME);
        cmd->addToken(TCG_TINY_ATOM::UINT_00);
        cmd->addToken(HostChallenge);
        cmd->addToken(TCG_TOKEN::ENDNAME);
        cmd->addToken(TCG_TOKEN::STARTNAME);
        cmd->addToken(TCG_TINY_ATOM::UINT_03);
        cmd->addToken(SignAuthority);
        cmd->addToken(TCG_TOKEN::ENDNAME);
    }
    cmd->addToken(TCG_TOKEN::ENDLIST); // ]  (Close Bracket)
    cmd->complete();
    if (sendCommand(cmd)) return 0xff;
    response = new TCGresponse(cmd->getRespBuffer());
    // call user method SL HSN TSN EL EOD SL 00 00 00 EL
    //   0   1     2     3  4   5   6  7   8

    HSN = SWAP32(response->getUint32(4));
    TSN = SWAP32(response->getUint32(5));
    delete response;
    return 0;
}

uint8_t
TCGsession::sendCommand(TCGcommand * cmd)
{
    LOG(D4) << "Entering TCGsession::sendCommand()";
    uint8_t rc;
    TCGresponse * response;
    cmd->setHSN(HSN);
    cmd->setTSN(TSN);
    cmd->setcomID(d->comID());

    d->exec(cmd, TCGProtocol);
    /*
     * Check out the basics that so that we know we
     * have a sane reply to work with
     */
    response = new TCGresponse(cmd->getRespBuffer());
    // zero lengths -- these are big endian but it doesn't matter for uint = 0
    if ((0 == response->h.cp.length) |
        (0 == response->h.pkt.length) |
        (0 == response->h.subpkt.length)) {
        LOG(E) << "One or more header fields have 0 length";
        delete response;
        return 0xff;
    }
    // if we get an endsession response return 0
    if ((1 == SWAP32(response->h.subpkt.length)) && (0xfa == response->tokenIs(0))) {
        delete response;
        return 0;
    }
    // IF we received a method status return it
    if (!(((uint8_t) TCG_TOKEN::ENDLIST == (uint8_t) response->tokenIs(response->getTokenCount() - 1)) &&
        ((uint8_t) TCG_TOKEN::STARTLIST == (uint8_t) response->tokenIs(response->getTokenCount() - 5)))) {
        // no method status so we hope we reported the error someplace else
        LOG(E) << "Method Status missing";
        delete response;
        return 0xff;
    }
    if (TCGSTATUSCODE::SUCCESS != response->getUint8(response->getTokenCount() - 4)) {
        LOG(E) << "method status code " <<
                methodStatus(response->getUint8(response->getTokenCount() - 4));
    }
    rc = response->getUint8(response->getTokenCount() - 4);
    delete response;
    return rc;
}

void
TCGsession::setProtocol(uint8_t value)
{
    LOG(D4) << "Entering TCGsession::setProtocol";
    TCGProtocol = value;
}

void
TCGsession::expectAbort()
{
    LOG(D4) << "Entering TCGsession::methodStatus()";
    willAbort = 1;
}

char *
TCGsession::methodStatus(uint8_t status)
{
    LOG(D4) << "Entering TCGsession::methodStatus()";
    switch (status) {
    case TCGSTATUSCODE::AUTHORITY_LOCKED_OUT:
        return (char *) "AUTHORITY_LOCKED_OUT";
    case TCGSTATUSCODE::FAIL:
        return (char *) "FAIL";
    case TCGSTATUSCODE::INSUFFICIENT_ROWS:
        return (char *) "INSUFFICIENT_ROWS";
    case TCGSTATUSCODE::INSUFFICIENT_SPACE:
        return (char *) "INSUFFICIENT_SPACE";
    case TCGSTATUSCODE::INVALID_PARAMETER:
        return (char *) "INVALID_PARAMETER";
    case TCGSTATUSCODE::NOT_AUTHORIZED:
        return (char *) "NOT_AUTHORIZED";
    case TCGSTATUSCODE::NO_SESSIONS_AVAILABLE:
        return (char *) "NO_SESSIONS_AVAILABLE";
    case TCGSTATUSCODE::RESPONSE_OVERFLOW:
        return (char *) "RESPONSE_OVERFLOW";
    case TCGSTATUSCODE::SP_BUSY:
        return (char *) "SP_BUSY";
    case TCGSTATUSCODE::SP_DISABLED:
        return (char *) "SP_DISABLED";
    case TCGSTATUSCODE::SP_FAILED:
        return (char *) "SP_FAILED";
    case TCGSTATUSCODE::SP_FROZEN:
        return (char *) "SP_FROZEN";
    case TCGSTATUSCODE::SUCCESS:
        return (char *) "SUCCESS";
    case TCGSTATUSCODE::TPER_MALFUNCTION:
        return (char *) "TPER_MALFUNCTION";
    case TCGSTATUSCODE::TRANSACTION_FAILURE:
        return (char *) "TRANSACTION_FAILURE";
    case TCGSTATUSCODE::UNIQUENESS_CONFLICT:
        return (char *) "UNIQUENESS_CONFLICT";
    default:
        return (char *) "Unknown status code";
    }
}

TCGsession::~TCGsession()
{
    LOG(D4) << "Destroying TCGsession";
    TCGcommand *cmd = new TCGcommand();
    if (!willAbort) {
        cmd->reset();
        cmd->addToken(TCG_TOKEN::ENDOFSESSION);
        cmd->complete(0);
        if (sendCommand(cmd)) {
            LOG(E) << "EndSession Failed";
        }
    }
}
