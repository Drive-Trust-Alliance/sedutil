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
#include "noparser.h"

int main(int argc, char * argv[])
{
	int rc = 0;
    void *resp = ALIGNED_ALLOC(4096, IO_BUFFER_LENGTH);
    memset(resp, 0, IO_BUFFER_LENGTH);
#if defined __gnu_linux__
    Device *device = new Device("/dev/sdh");
#elif defined _WIN32
    Device *device = new Device("\\\\.\\PhysicalDrive3");
#endif
	uint16_t comID = device->comID();
    //   int d0rc = device->SendCmd(IF_RECV, 0x01, 0x0001, resp, IO_BUFFER_LENGTH);
    //   HexDump(resp, 256);
    DiskList * dl = new DiskList();
    delete dl;
    // d0Response
   /* device->Puke();*/
    //	Start Session
    TCGCommand *cmd = new TCGCommand(); // Start with an empty class
	cmd->StartSession(device, 1, TCG_UID::TCG_UID_ADMINSP, TRUE);
   
    // session[TSN:HSN] -> C_PIN_MSID_UID.Get[Cellblock : [startColumn = PIN,
    //                       endColumn = PIN]]
	cmd->reset(comID, TCG_UID::TCG_TABLE_C_PIN_MSID, TCG_METHOD::GET);
    cmd->addToken(TCG_TOKEN::STARTLIST);
    cmd->addToken(TCG_TOKEN::STARTLIST);
	cmd->addToken(TCG_TOKEN::STARTNAME);
	cmd->addToken(TCG_TOKEN::STARTCOLUMN);
	cmd->addToken(TCG_TINY_ATOM::UINT_03);		// column 3 is the PIN
	cmd->addToken(TCG_TOKEN::ENDNAME);
	cmd->addToken(TCG_TOKEN::STARTNAME);
	cmd->addToken(TCG_TOKEN::ENDCOLUMN);
	cmd->addToken(TCG_TINY_ATOM::UINT_03);		// column 3 is the PIN
	cmd->addToken(TCG_TOKEN::ENDNAME);
	cmd->addToken(TCG_TOKEN::ENDLIST);
	cmd->addToken(TCG_TOKEN::ENDLIST);
	cmd->complete();
	printf("\nDumping Get C_PIN\n");
	cmd->dump();
	memset(resp, 0, IO_BUFFER_LENGTH);
	rc = cmd->execute(device, resp);
	if (0 != rc) {
		printf("Get C PIN failed %d", rc);
		HexDump(resp, 16);
		goto exit;
	}
	printf("\nDumping GET C PIN Reply\n");
	HexDump(resp, 128);
	/* The pin is the ever so original "micron" so
	 * I'll just use that instead of pretending 
	 * I'm parsing the reply
	 */
	// session[TSN:HSN] <- EOS
	cmd->EndSession(device);
	/* 
	 * We now have the PIN to sign on and take ownership
	 * so lets give it a shot 
	 */
	cmd->StartSession(device, 1, TCG_UID::TCG_UID_ADMINSP, TRUE, "micron", TCG_UID::TCG_UID_SID);


exit:
    /*  ******************  */
    /*  CLEANUP LEAVE HERE  */
    /*  ******************  */
    delete device;
    ALIGNED_FREE(resp);
    return 0;
}

