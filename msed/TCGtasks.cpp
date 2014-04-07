/* C:B**************************************************************************
This software is Copyright (c) 2014 Michael Romeo <r0m30@r0m30.com>

THIS SOFTWARE IS PROVIDED BY THE AUTHORS ''AS IS'' AND ANY EXPRESS
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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

int diskQuery(char * devref, uint8_t silent)
{
    LOG(D4) << "Entering diskQuery()" << devref;
    TCGdev * dev = new TCGdev(devref);
    if (!dev->isPresent()) {
        LOG(E) << "Device not present" << devref;
        return 1;
    }
    if (!dev->isOpal2()) {
        LOG(E) << "Device does not support Opal 2.0" << devref;
        return 1;
    }
    if (!silent) {
        dev->puke();
    }
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

    TCGcommand *cmd = new TCGcommand();
    TCGdev *device = new TCGdev(devref);
	if (!(device->isOpal2())) return 0xff;
    //	Start Session
    TCGsession * session = new TCGsession(device);
    if (session->start(TCG_UID::TCG_ADMINSP_UID)) return 0xff;
    // session[TSN:HSN] -> C_PIN_MSID_UID.Get[Cellblock : [startColumn = PIN,
    //                       endColumn = PIN]]
    cmd->reset(TCG_UID::TCG_C_PIN_MSID_TABLE, TCG_METHOD::GET);
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
    if (session->sendCommand(cmd)) return 0xff;
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
    cmd->reset(TCG_UID::TCG_C_PIN_SID_TABLE, TCG_METHOD::SET);
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
    if (session->sendCommand(cmd)) return 0xff;
    LOG(I) << "takeownership complete new SID password = " << newpassword;
    // session[TSN:HSN] <- EOS
    delete session;
    delete device;
    LOG(D4) << "Exiting changeInitialPassword()";
    return 0;
}

int revertTPer(char * devref, char * password)
{
    LOG(D4) << "Entering revertTPer(char * devref, char * password)";

    /*
     * Revert the TPer
     */

    TCGdev *device = new TCGdev(devref);
	if (!(device->isOpal2())) return 0xff;
    TCGcommand *cmd = new TCGcommand();
    TCGsession * session = new TCGsession(device);
    if (session->start(TCG_UID::TCG_ADMINSP_UID, password, TCG_UID::TCG_SID_UID)) return 0xff;
    //	session[TSN:HSN]->AdminSP_UID.Revert[]
    cmd->reset(TCG_UID::TCG_ADMINSP_UID, TCG_METHOD::REVERT);
    cmd->addToken(TCG_TOKEN::STARTLIST);
    cmd->addToken(TCG_TOKEN::ENDLIST);
    cmd->complete();
    session->expectAbort();
    if (session->sendCommand(cmd)) return 0xff;
    LOG(I) << "revertTper completed successfully";
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
	if (!(device->isOpal2())) return 0xff;
    TCGcommand *cmd = new TCGcommand();
    TCGsession * session = new TCGsession(device);
    if (session->start(TCG_UID::TCG_ADMINSP_UID, password, TCG_UID::TCG_SID_UID)) return 0xff;
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
        LOG(E) << "Locking Activate Failed";
        goto exit;
    }
    LOG(I) << "Locking SP Activate Complete";
exit:
    /*  ******************  */
    /*  CLEANUP LEAVE HERE  */
    /*  ******************  */
    delete device;
    delete session;
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
	if (!(device->isOpal2())) return 0xff;
    TCGcommand *cmd = new TCGcommand();
    TCGsession * session = new TCGsession(device);
    // session[0:0]->SMUID.StartSession[HostSessionID:HSN, SPID : LockingSP_UID, Write : TRUE,
    //                   HostChallenge = <Admin1_password>, HostSigningAuthority = Admin1_UID]
    if (session->start(TCG_UID::TCG_LOCKINGSP_UID, password, TCG_UID::TCG_ADMIN1_UID)) return 0xff;
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
    if (session->sendCommand(cmd)) return 0xff;
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
exit:
    /*  ******************  */
    /*  CLEANUP LEAVE HERE  */
    /*  ******************  */
    delete session;
    delete device;
    LOG(D4) << "Exiting activatLockingSP()";
    return rc;
}