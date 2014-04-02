/* C:B**************************************************************************
This software is Copyright © 2014 Michael Romeo <r0m30@r0m30.com>

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
#include "DiskList.h"
#include "Device.h"
#include "HexDump.h"
#include "TCGCommand.h"
#include "Endianfixup.h"
#include "TCGStructures.h"

int main(int argc, char * argv[])
{
    void *resp = ALIGNED_ALLOC(4096, IO_BUFFER_LENGTH);
    memset(resp, 0, IO_BUFFER_LENGTH);
#if defined __gnu_linux__
    Device *device = new Device("/dev/sdh");
#elif defined _WIN32
    Device *device = new Device("\\\\.\\PhysicalDrive3");
#endif
    //   int d0rc = device->SendCmd(IF_RECV, 0x01, 0x0001, resp, IO_BUFFER_LENGTH);
    //   HexDump(resp, 256);
    DiskList * dl = new DiskList();
    delete dl;
    // d0Response
    device->Puke();
    //	Start Session
    TCGCommand *cmd = new TCGCommand(0x1000, TCG_UID::SMUID,
                                     TCG_METHOD::STARTSESSION);
    cmd->addToken(TCG_TOKEN::STARTLIST); // [  (Open Bracket)
    //  cmd->addToken(TCG_TINY_ATOM::uINT01); // HostSessionID : 0x01
    cmd->addToken(99); // HostSessionID : 0x99
    cmd->addToken(TCG_UID::ADMINSP); // SPID : ADMINSP
    cmd->addToken(TCG_TINY_ATOM::uINT01); // write : 1
    cmd->addToken(TCG_TOKEN::ENDLIST); // ]  (Close Bracket)
    cmd->complete();
    cmd->setProtocol(0x01);
    cmd->dump(); // have a look see
    memset(resp, 0, IO_BUFFER_LENGTH);
    int rc = cmd->execute(device, resp);
    if (0 != rc) {
        printf("StartSession failed %d", rc);
        HexDump(resp, 16);
        goto exit;
    }
    printf("\nDumping SyncSession Reply\n");
    HexDump(resp, 128);
    {
        StartSessionResponse * ssreply = (StartSessionResponse *) resp;
        cmd->setHSN(ssreply->HostSessionNumber);
        cmd->setTSN(ssreply->TPerSessionNumber);
    }
    // session[TSN:HSN] -> C_PIN_MSID_UID.Get[Cellblock : [startColumn = PIN,
    //                       endColumn = PIN]]
    cmd->reset(0x1000, TCG_UID::C_PIN_MSID, TCG_METHOD::GET);
    cmd->addToken(TCG_TOKEN::STARTLIST);
    cmd->addToken(TCG_TOKEN::STARTLIST);

exit:
    /*  ******************  */
    /*  CLEANUP LEAVE HERE  */
    /*  ******************  */
    delete device;
    ALIGNED_FREE(resp);
    return 0;
}

