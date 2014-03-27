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
#include "DiskList.h"
#include "Device.h"
#include "HexDump.h"
#include "TCGCommand.h"
#include "Endianfixup.h"

int main(int argc, char * argv[])
{
	void *resp = _aligned_malloc(IO_BUFFER_LENGTH,512);
	memset(resp, 0, IO_BUFFER_LENGTH);
	Device *device = new Device("\\\\.\\PhysicalDrive3");
// reset the protocol stack
	//unsigned char protocolReset[] = {
	//	0x10, 0x00, 0x00, 0x00,
	//	0x00, 0x00, 0x00, 0x00,
	//};
	//memcpy(resp, protocolReset,8);
	//device->SendCmd(IF_SEND, 0x02, 0x1000, resp, 512);
	//memset(resp, 0, IO_BUFFER_LENGTH);
	//Sleep(250);
	//device->SendCmd(IF_RECV, 0x02, 0x1000, resp, 512);
	//HexDump(resp, 16);
	
//	 get a commid  == should fail because the TPer doesnt support comID management
	device->SendCmd(IF_RECV, 0x00, 0x0000, resp, 512);
	HexDump(resp, 512);
////	return 0;
//TPer reset 
	//device->SendCmd(IF_SEND, 0x02, 0x0004, resp, 512);
	//HexDump(resp, 16);
//	return 0;
	 DiskList *dList = new DiskList();
	 //return 0;
// properties
//
// session[0:0] -> SMUID.Properties[ HostProperties = [“MaxComPacketSize” = 4096, 
// “MaxResponseComPacketSize” = 4096, “MaxPacketSize” = 4076,
// “MaxIndTokenSize” = 4040, “MaxPackets” = 1, “MaxSubpackets” = 1,
// “MaxMethods” = 1] ]
	//  0000 00000000 07FE0000 00000000 00000000
	//	0010 000000D0 00000000 00000000 00000000
	//	0020 00000000 00000000 000000B8 00000000
	//	0030 00000000 000000AB F8A80000 00000000
	//	0040 00FFA800 00000000 00FF01F0 F200F0F2
	//	0050 D0104D61 78436F6D 5061636B 65745369
	//	0060 7A658210 00F3F2D0 184D6178 52657370
	//	0070 6F6E7365 436F6D50 61636B65 7453697A
	//	0080 65821000 F3F2AD4D 61785061 636B6574
	//	0090 53697A65 820FECF3 F2AF4D61 78496E64
	//	00A0 546F6B65 6E53697A 65820FC8 F3F2AA4D
	//	00B0 61785061 636B6574 7301F3F2 AD4D6178
	//	00C0 53756270 61636B65 747301F3 F2AA4D61
	//	00D0 784D6574 686F6473 01F3F1F3 F1F9F000
	//	00E0 0000F100 00000000 00000000 00000000
	//TCGCommand *cmd = new TCGCommand(extendedcomID, TCG_USER::SMUID, TCG_METHOD::PROPERTIES);
	//cmd->addToken(TCG_TOKEN::STARTLIST);	// [  (Open Bracket)
	//cmd->addToken(TCG_TOKEN::STARTNAME);	// 
	//cmd->addToken(TCG_NAME::HOSTPROPERTIES);
	////cmd->addToken("HostProperties");
	//cmd->addToken(TCG_TOKEN::STARTLIST);	// [  (Open Bracket)
	//cmd->addToken(TCG_TOKEN::STARTNAME);
	//cmd->addToken("MaxComPacketSize");
	//cmd->addToken((UINT16) 4096);
	//cmd->addToken(TCG_TOKEN::ENDNAME);
	//cmd->addToken(TCG_TOKEN::STARTNAME);
	//cmd->addToken("MaxResponseComPacketSize");
	//cmd->addToken((UINT16)4096);
	//cmd->addToken(TCG_TOKEN::ENDNAME);
	//cmd->addToken(TCG_TOKEN::STARTNAME);
	//cmd->addToken("MaxPacketSize");
	//cmd->addToken((UINT16)4076);
	//cmd->addToken(TCG_TOKEN::ENDNAME);
	//cmd->addToken(TCG_TOKEN::STARTNAME);
	//cmd->addToken("MaxIndTokenSize");
	//cmd->addToken((UINT16)4040);
	//cmd->addToken(TCG_TOKEN::ENDNAME);
	//cmd->addToken(TCG_TOKEN::STARTNAME);
	//cmd->addToken("MaxPackets");
	//cmd->addToken(TCG_TINY_ATOM::uINT01);
	//cmd->addToken(TCG_TOKEN::ENDNAME);
	//cmd->addToken(TCG_TOKEN::STARTNAME);
	//cmd->addToken("MaxSubpackets");
	//cmd->addToken(TCG_TINY_ATOM::uINT01);
	//cmd->addToken(TCG_TOKEN::ENDNAME);
	//cmd->addToken(TCG_TOKEN::STARTNAME);
	//cmd->addToken("MaxMethods");
	//cmd->addToken(TCG_TINY_ATOM::uINT01);
	//cmd->addToken(TCG_TOKEN::ENDNAME);
	//cmd->addToken(TCG_TOKEN::ENDLIST);	// ]  (Close Bracket)
	//cmd->addToken(TCG_TOKEN::ENDNAME);	// 
	//cmd->addToken(TCG_TOKEN::ENDLIST);	// ]  (Close Bracket)
	//cmd->complete();
	//cmd->dump();  // have a look see
	//cmd->setProtocol(0x00);
	//int rc = cmd->execute(device, resp);
	//HexDump(resp, 128);
	//delete device;
	//_aligned_free(resp);
	//return rc;
//Start Session

	TCGCommand *cmd = new TCGCommand (0x10000000, TCG_USER::SMUID, TCG_METHOD::STARTSESSION);
	cmd->addToken(TCG_TOKEN::STARTLIST);	// [  (Open Bracket)
//	cmd->addToken(TCG_TINY_ATOM::uINT01);   // HostSessionID : 0x01
	cmd->addToken(99);   // HostSessionID : 0x99
	cmd->addToken(TCG_USER::ADMINSP);		// SPID : ADMINSP 
	cmd->addToken(TCG_TINY_ATOM::uINT01);  // write : 1
// try adding a user/host signing authority
	cmd->addToken(TCG_TOKEN::STARTNAME);
	cmd->addToken("HostChallenge");
	cmd->addToken("anybody");
	cmd->addToken(TCG_TOKEN::ENDNAME);
	cmd->addToken(TCG_TOKEN::STARTNAME);
	cmd->addToken("HostSigningAuthority");
	cmd->addToken(TCG_USER::ANYBODY);
	cmd->addToken(TCG_TOKEN::ENDNAME);
// END user/signing auth code
	cmd->addToken(TCG_TOKEN::ENDLIST);	// ]  (Close Bracket)
// try adding a user/host signing authority
	cmd->complete();
	cmd->setProtocol(0x01);
	cmd->dump();  // have a look see
	int rc = cmd->execute(device, resp);
//	HexDump(resp, 128);
/*  ******************  */
/*  CLEANUP LEAVE HERE  */
/*  ******************  */
	delete device;
	_aligned_free(resp);
	return 0;
}

