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
#include "endianfixup.h"
#include "hexDump.h"
#include "TCGstructures.h"
#include "noparser.h"

/*
 * Start a session
 */
TCGsession::TCGsession(TCGdev * device)
{
    LOG(D4) << "Creating TCGsession()";
    d = device;
}

uint8_t
TCGsession::SEND(TCGcommand * cmd)
{
    LOG(D4) << "Entering TCGsession::SEND(TCGcommand * cmd)";
    return d->sendCmd(IF_SEND, TCGProtocol, d->comID(), cmd->getCmdBuffer(), IO_BUFFER_LENGTH);
}

uint8_t
TCGsession::RECV(void * resp)
{
    LOG(D4) << "Entering TCGsession::RECV(void * resp)";
    return d->sendCmd(IF_RECV, TCGProtocol, d->comID(), resp, IO_BUFFER_LENGTH);
}

uint8_t
TCGsession::start(TCG_UID SP, char * HostChallenge, TCG_UID SignAuthority)
{
    LOG(D4) << "Entering TCGsession::startSession ";
    TCGcommand *cmd = new TCGcommand();
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
    SSResponse * ssresp = (SSResponse *) cmd->getRespBuffer();
    HSN = ssresp->HostSessionNumber;
    TSN = ssresp->TPerSessionNumber;
    return 0;
}

uint8_t
TCGsession::sendCommand(TCGcommand * cmd)
{
    LOG(D4) << "Entering TCGsession::sendCommand()";
    uint8_t rc;
    GenericResponse * r;
    cmd->setHSN(HSN);
    cmd->setTSN(TSN);
    cmd->setcomID(d->comID());
	LOG(D3) << "Dumping command buffer";
	IFLOG(D3) hexDump(cmd->getCmdBuffer(), 128);
    rc = SEND(cmd);
    if (0 != rc) {
        LOG(E) << "Command failed on send " << rc;
        return rc;
    }
    //    Sleep(250);
    memset(cmd->getRespBuffer(), 0, IO_BUFFER_LENGTH);
    rc = RECV(cmd->getRespBuffer());
    LOG(D3) << "Dumping reply buffer";
    IFLOG(D3) hexDump(cmd->getRespBuffer(), 128);
    if (0 != rc) {
        LOG(E) << "Command failed on recv" << rc;
        return rc;
    }
    /*
     * Check out the basics that so that we know we
     * have a sane reply to work with
     */
    r = (GenericResponse *) cmd->getRespBuffer();
    // zero lengths -- these are big endian but it doesn't matter for uint = 0
    if ((0 == r->h.cp.length) |
        (0 == r->h.pkt.length) |
        (0 == r->h.subpkt.length)) {
        LOG(E) << "One or more header fields have 0 length";
        return 0xff;
    }
    // if we get an endsession response return 0
    if ((1 == SWAP32(r->h.subpkt.length)) && (0xfa == r->payload[0])) return 0;
    // IF we received a method status return it
    if (!((0xf1 == r->payload[SWAP32(r->h.subpkt.length) - 1]) &&
        (0xf0 == r->payload[SWAP32(r->h.subpkt.length) - 5]))) {
        // no method status so we hope we reported the error someplace else
        LOG(E) << "Method Status missing";
        return 0xff;
    }
    if (0x00 != r->payload[SWAP32(r->h.subpkt.length) - 4]) {
        LOG(E) << "Non-zero method status code " <<
                methodStatus(r->payload[SWAP32(r->h.subpkt.length) - 4]);
    }
    return r->payload[SWAP32(r->h.subpkt.length) - 4];
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
