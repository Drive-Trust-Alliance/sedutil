/* C:B**************************************************************************
This software is Copyright � 2014 Michael Romeo <r0m30@r0m30.com>

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

int changeInitialPassword()
{
    int rc = 0;
    TCGHeader * h;
    void *resp = ALIGNED_ALLOC(4096, IO_BUFFER_LENGTH);
    memset(resp, 0, IO_BUFFER_LENGTH);
#if defined __gnu_linux__
    TCGdev *device = new TCGdev("/dev/sdh");
#elif defined _WIN32
    TCGdev *device = new TCGdev("\\\\.\\PhysicalDrive3");
#endif
    uint16_t comID = device->comID();
    //   int d0rc = device->SendCmd(IF_RECV, 0x01, 0x0001, resp, IO_BUFFER_LENGTH);
    //   HexDump(resp, 256);
    diskList * dl = new diskList();
    delete dl;
    // d0Response
    device->puke();
    //	Start Session
    TCGcommand *cmd = new TCGcommand(); // Start with an empty class
    rc = cmd->startSession(device, 1, TCG_UID::TCG_ADMINSP_UID, TRUE);
    if (0 != rc) {
        printf(" Unauthenticated StartSession failed %d\n", rc);
        return rc;
    }

    // session[TSN:HSN] -> C_PIN_MSID_UID.Get[Cellblock : [startColumn = PIN,
    //                       endColumn = PIN]]
    cmd->reset(comID, TCG_UID::TCG_C_PIN_MSID_TABLE, TCG_METHOD::GET);
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
    printf("\nDumping Get C_PIN\n");
    cmd->dump();
    memset(resp, 0, IO_BUFFER_LENGTH);
    rc = cmd->execute(device, resp);
    if (0 != rc) {
        printf("Get C PIN failed %d\n", rc);
        hexDump(resp, 16);
        goto exit;
    }
    printf("\nDumping GET C PIN Reply\n");
    hexDump(resp, 128);
    /* The pin is the ever so original "micron" so
     * I'll just use that instead of pretending
     * I'm parsing the reply
     */
    // session[TSN:HSN] <- EOS
    rc = cmd->endSession(device);
    if (0 != rc) {
        printf("EndSession failed %d\n", rc);
        hexDump(resp, 128);
        goto exit;
    }
    /*
     * We now have the PIN to sign on and take ownership
     * so lets give it a shot
     */
    rc = cmd->startSession(device, 1, TCG_UID::TCG_ADMINSP_UID, TRUE,
                           (char *) "micron", TCG_UID::TCG_SID_UID);
    if (0 != rc) {
        printf(" Authenticated StartSession failed %d\n", rc);
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
    cmd->addToken("newPassword");
    cmd->addToken(TCG_TOKEN::ENDNAME);
    cmd->addToken(TCG_TOKEN::ENDLIST);
    cmd->addToken(TCG_TOKEN::ENDNAME);
    cmd->addToken(TCG_TOKEN::ENDLIST);
    cmd->complete();
    printf("Dumping SetPassword\n");
    cmd->dump();
    rc = cmd->execute(device, resp);
    if (0 != rc) {
        printf("Password Change Failed %d\n", rc);
        hexDump(resp, 16);
        goto exit;
    }
    printf("\nDumping Set new Password Reply\n");
    hexDump(resp, 128);
    h = (TCGHeader *) resp;
    if (0x2c != SWAP32(h->cp.length)) {
        printf("Set Failed\n");
        goto exit;
    }
    // session[TSN:HSN] <- EOS
    rc = cmd->endSession(device);
    if (0 != rc) {
        printf("EndSession failed %d\n", rc);
        hexDump(resp, 128);
        goto exit;
    }
exit:
    /*  ******************  */
    /*  CLEANUP LEAVE HERE  */
    /*  ******************  */
    delete device;
    ALIGNED_FREE(resp);
    return 0;
}

int revertSP()
{
    int rc = 0;
    TCGHeader * h;
    void *resp = ALIGNED_ALLOC(4096, IO_BUFFER_LENGTH);
    memset(resp, 0, IO_BUFFER_LENGTH);
#if defined __gnu_linux__
    TCGdev *device = new TCGdev("/dev/sdh");
#elif defined _WIN32
    TCGdev *device = new TCGdev("\\\\.\\PhysicalDrive3");
#endif
    uint16_t comID = device->comID();
    diskList * dl = new diskList();
    delete dl;
    // d0Response
    device->puke();
    /*
     * Revert the SP
     */
    TCGcommand *cmd = new TCGcommand(); // Start with an empty class
    rc = cmd->startSession(device, 1, TCG_UID::TCG_ADMINSP_UID, TRUE,
                           (char*) "newPassword", TCG_UID::TCG_SID_UID);
    if (0 != rc) {
        printf(" Authenticated StartSession failed %d\n", rc);
        return rc;
    }
    //	session[TSN:HSN]->AdminSP_UID.Revert[]
    cmd->reset(device->comID(), TCG_UID::TCG_ADMINSP_UID, TCG_METHOD::REVERT);
    cmd->addToken(TCG_TOKEN::STARTLIST);
    cmd->addToken(TCG_TOKEN::ENDLIST);
    cmd->complete();
    printf("Dumping Revert\n");
    cmd->dump();

    rc = cmd->execute(device, resp);
    if (0 != rc) {
        printf("Revert Failed %d\n", rc);
        hexDump(resp, 128);
        goto exit;
    }
    printf("Revert  Reply\n");
    hexDump(resp, 128);
    h = (TCGHeader *) resp;
    if (0x2c != SWAP32(h->cp.length)) {
        printf("Revert Failed\n");
        goto exit;
    }
    // session is aborted by TPER
    // so session[TSN:HSN] <- EOS
    // is not needed
exit:
    /*  ******************  */
    /*  CLEANUP LEAVE HERE  */
    /*  ******************  */
    delete device;
    ALIGNED_FREE(resp);
    return rc;
}