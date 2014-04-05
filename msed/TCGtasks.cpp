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
#include "endianfixup.h"
#include "TCGstructures.h"
#include "noparser.h"

int diskQuery(char * devref)
{
    LOG(D4) << "Entering diskQuery(char * devref)" << devref;
    TCGdev * dev = new TCGdev(devref);
    if (!dev->isPresent()) {
        LOG(E) << "Device not present" << devref;
        return 1;
    }
    if (!dev->isOpal2()) {
        LOG(E) << "Device does not support Opal 2.0" << devref;
        return 1;
    }
    dev->puke();
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

    //#if defined __gnu_linux__
    //	TCGdev *device = new TCGdev("/dev/sdh");
    //#elif defined _WIN32
    //	TCGdev *device = new TCGdev("\\\\.\\PhysicalDrive3");
    //#endif

    int rc = 0;
    TCGHeader * h;
    void *resp = ALIGNED_ALLOC(4096, IO_BUFFER_LENGTH);
    memset(resp, 0, IO_BUFFER_LENGTH);
    TCGdev *device = new TCGdev(devref);
    if (!device->isPresent()) LOG(E) << "Device was not present";
    //	Start Session
    TCGcommand *cmd = new TCGcommand(); // Start with an empty class
    rc = cmd->startSession(device, 1, TCG_UID::TCG_ADMINSP_UID, TRUE);
    if (0 != rc) {
        LOG(E) << "Unauthenticated StartSession failed " << rc;
        return rc;
    }

    // session[TSN:HSN] -> C_PIN_MSID_UID.Get[Cellblock : [startColumn = PIN,
    //                       endColumn = PIN]]
    cmd->reset(device->comID(), TCG_UID::TCG_C_PIN_MSID_TABLE, TCG_METHOD::GET);
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
    LOG(D3) << "Dumping Get C_PIN";
    IFLOG(D3) cmd->dump();
    memset(resp, 0, IO_BUFFER_LENGTH);
    rc = cmd->execute(device, resp);
    if (0 != rc) {
        LOG(E) << "Get C PIN failed " << rc;
        IFLOG(D3) hexDump(resp, 16);
        goto exit;
    }
    LOG(D3) << "Dumping GET C PIN Reply";
    IFLOG(D3) hexDump(resp, 128);
    /* The pin is the ever so original "micron" so
     * I'll just use that instead of pretending
     * I'm parsing the reply
     */
    // session[TSN:HSN] <- EOS
    rc = cmd->endSession(device);
    if (0 != rc) {
        LOG(E) << "EndSession failed " << rc;
        IFLOG(D3) hexDump(resp, 128);
        goto exit;
    }
    /*
     * We now have the PIN to sign on and take ownership
     * so lets give it a shot
     */
    rc = cmd->startSession(device, 1, TCG_UID::TCG_ADMINSP_UID, TRUE,
                           (char *) "micron", TCG_UID::TCG_SID_UID);
    if (0 != rc) {
        LOG(E) << " Authenticated StartSession failed " << rc;
        return rc;
    }
    // session[TSN:HSN] -> C_PIN_SID_UID.Set[Values = [PIN = <new_SID_password>]]
    /*
     * Change the password --- Yikes!!!
     */
    cmd->reset(device->comID(), TCG_UID::TCG_C_PIN_SID_TABLE, TCG_METHOD::SET);
    cmd->addToken(TCG_TOKEN::STARTLIST);
    cmd->addToken(TCG_TOKEN::STARTNAME);
    cmd->addToken(TCG_TINY_ATOM::UINT_01); // Values
    cmd->addToken(TCG_TOKEN::STARTLIST);
    cmd->addToken(TCG_TOKEN::STARTNAME);
    cmd->addToken(TCG_TINY_ATOM::UINT_03); // column 4 is the PIN
    cmd->addToken(newpassword);
    cmd->addToken(TCG_TOKEN::ENDNAME);
    cmd->addToken(TCG_TOKEN::ENDLIST);
    cmd->addToken(TCG_TOKEN::ENDNAME);
    cmd->addToken(TCG_TOKEN::ENDLIST);
    cmd->complete();
    LOG(D3) << "Dumping SetPassword";
    IFLOG(D3) cmd->dump();
    rc = cmd->execute(device, resp);
    if (0 != rc) {
        LOG(E) << "Password Change Failed " << rc;
        IFLOG(D3) hexDump(resp, 16);
        goto exit;
    }
    LOG(D3) << "Dumping Set new Password Reply";
    IFLOG(D3) hexDump(resp, 128);
    h = (TCGHeader *) resp;
    if (0x2c != SWAP32(h->cp.length)) {
        LOG(E) << "Set Failed";
        goto exit;
    }
    LOG(I) << "takeownership complete new SID password = " << newpassword;
    // session[TSN:HSN] <- EOS
    rc = cmd->endSession(device);
    if (0 != rc) {
        LOG(E) << "EndSession failed " << rc;
        IFLOG(D3) hexDump(resp, 128);
        goto exit;
    }
exit:
    /*  ******************  */
    /*  CLEANUP LEAVE HERE  */
    /*  ******************  */
    delete device;
    ALIGNED_FREE(resp);
    LOG(D4) << "Exiting changeInitialPassword()";
    return 0;
}

int revertTPer(char * devref, char * password)
{
    LOG(D4) << "Entering revertTPer(char * devref, char * password)";

    //#if defined __gnu_linux__
    //	TCGdev *device = new TCGdev("/dev/sdh");
    //#elif defined _WIN32
    //	TCGdev *device = new TCGdev("\\\\.\\PhysicalDrive3");
    //#endif

    int rc = 0;
    TCGHeader * h;
    void *resp = ALIGNED_ALLOC(4096, IO_BUFFER_LENGTH);
    memset(resp, 0, IO_BUFFER_LENGTH);
    /*
     * Revert the TPer
     */
    TCGdev *device = new TCGdev(devref);
    if (!device->isPresent()) LOG(E) << "Device was not present";
    TCGcommand *cmd = new TCGcommand(); // Start with an empty class
    rc = cmd->startSession(device, 1, TCG_UID::TCG_ADMINSP_UID, TRUE,
                           password, TCG_UID::TCG_SID_UID);
    if (0 != rc) {
        LOG(E) << "Authenticated StartSession failed " << rc;
        return rc;
    }
    //	session[TSN:HSN]->AdminSP_UID.Revert[]
    cmd->reset(device->comID(), TCG_UID::TCG_ADMINSP_UID, TCG_METHOD::REVERT);
    cmd->addToken(TCG_TOKEN::STARTLIST);
    cmd->addToken(TCG_TOKEN::ENDLIST);
    cmd->complete();
    LOG(D3) << "Dumping RevertTPer";
    IFLOG(D3) cmd->dump();

    rc = cmd->execute(device, resp);
    if (0 != rc) {
        LOG(E) << "RevertTper Failed " << rc;
        hexDump(resp, 128);
        goto exit;
    }
    LOG(D3) << "Revert[]  Reply";
    IFLOG(D3) hexDump(resp, 128);
    h = (TCGHeader *) resp;
    if (0x2c != SWAP32(h->cp.length)) {
        LOG(E) << "Revert Failed";
        goto exit;
    }
    LOG(I) << "revertTper completed successfully";
    // session is aborted by TPER
    // so session[TSN:HSN] <- EOS
    // is not needed
exit:
    /*  ******************  */
    /*  CLEANUP LEAVE HERE  */
    /*  ******************  */
    delete device;
    ALIGNED_FREE(resp);
    LOG(D4) << "Exiting RevertTperevertTPer()";
    return rc;
}