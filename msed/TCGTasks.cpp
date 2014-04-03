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

int changeInitialPassword()
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
	device->Puke();
	//	Start Session
	TCGCommand *cmd = new TCGCommand(); // Start with an empty class
	rc = cmd->startSession(device, 1, TCG_UID::TCG_UID_ADMINSP, TRUE);
	if (0 != rc) {
		printf(" Unauthenticated StartSession failed %d\n", rc);
		return rc;
	}

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
		printf("Get C PIN failed %d\n", rc);
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
	rc = cmd->endSession(device);
	if (0 != rc) {
		printf("EndSession failed %d\n", rc);
		HexDump(resp, 128);
		goto exit;
	}
	/*
	* We now have the PIN to sign on and take ownership
	* so lets give it a shot
	*/
	rc = cmd->startSession(device, 1, TCG_UID::TCG_UID_ADMINSP, TRUE,
		"micron", TCG_UID::TCG_UID_SID);
	if (0 != rc) {
		printf(" Authenticated StartSession failed %d\n", rc);
		return rc;
	}
	// session[TSN:HSN] -> C_PIN_SID_UID.Set[Values = [PIN = <new_SID_password>]]
	/*
	* Change the password --- Yikes!!!
	*/
	cmd->reset(device->comID(), TCG_UID::TCG_TABLE_C_PIN_SID, TCG_METHOD::SET);
	cmd->addToken(TCG_TOKEN::STARTLIST);
	cmd->addToken(TCG_TOKEN::STARTNAME);
	cmd->addToken(TCG_TINY_ATOM::UINT_01);  // Values
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
		HexDump(resp, 16);
		goto exit;
	}
	printf("\nDumping Set new Password Reply\n");
	HexDump(resp, 128);
	TCGHeader * h = (TCGHeader *)resp;
	if (0x2c != SWAP32(h->cp.Length)) {
		printf("Set Failed\n");
		goto exit;
	}
	// session[TSN:HSN] <- EOS
	rc = cmd->endSession(device);
	if (0 != rc) {
		printf("EndSession failed %d\n", rc);
		HexDump(resp, 128);
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
	void *resp = ALIGNED_ALLOC(4096, IO_BUFFER_LENGTH);
	memset(resp, 0, IO_BUFFER_LENGTH);
#if defined __gnu_linux__
	Device *device = new Device("/dev/sdh");
#elif defined _WIN32
	Device *device = new Device("\\\\.\\PhysicalDrive3");
#endif
	uint16_t comID = device->comID();
	DiskList * dl = new DiskList();
	delete dl;
	// d0Response
	device->Puke();
	/*
	* Revert the SP
	*/
	TCGCommand *cmd = new TCGCommand(); // Start with an empty class
	rc = cmd->startSession(device, 1, TCG_UID::TCG_UID_ADMINSP, TRUE,
		"newPassword", TCG_UID::TCG_UID_SID);
	if (0 != rc) {
		printf(" Authenticated StartSession failed %d\n", rc);
		return rc;
	}
	//	session[TSN:HSN]->AdminSP_UID.Revert[]
	cmd->reset(device->comID(), TCG_UID::TCG_UID_ADMINSP, TCG_METHOD::REVERT);
	cmd->addToken(TCG_TOKEN::STARTLIST);
	cmd->addToken(TCG_TOKEN::ENDLIST);
	cmd->complete();
	printf("Dumping Revert\n");
	cmd->dump();

	rc = cmd->execute(device, resp);
	if (0 != rc) {
		printf("Revert Failed %d\n", rc);
		HexDump(resp, 128);
		goto exit;
	}
	printf("Revert  Reply\n");
	HexDump(resp, 128);
	TCGHeader * h = (TCGHeader *)resp;
	if (0x2c != SWAP32(h->cp.Length)) {
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