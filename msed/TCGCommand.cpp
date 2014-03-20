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
#include "TCGCommand.h"
#include "Endianfixup.h"
#include "HexDump.h"
#include <stdio.h>

TCGCommand::TCGCommand(TCG_USER InvokingUid, TCG_METHOD method)
{
	/* ******************* BS ALERT **************************
	 * this is ugly and stupid but VS2013 gives an error when I
	 * try and initialize these in the header declareation
	 */
	unsigned char tu[TCGUSER_SIZE][8]{
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff},  // session management
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 },  // special "Thissp" syntax
		{ 0x00, 0x00, 0x02, 0x05, 0x00, 0x00, 0x00, 0x01 }  // Administrative SP
	};
	memcpy(TCGUSER, tu, TCGUSER_SIZE*8);
	unsigned char tm[TCGMETHOD_SIZE][8]{
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x02 } //STARTSESSION
	};
	memcpy(TCGMETHOD, tm, TCGMETHOD_SIZE * 8);
	/* ******************* BS ALERT ************************* */
	/* 
	 * allocate the buffer and build the call *
	 */
	buffer = (unsigned char *)malloc(4096);
	memset(buffer, 0, 4096);
	TCGHeader * hdr;
	/* build the headers */
	hdr = (TCGHeader *)buffer;
	/* ComPacket */
	hdr->cp.ExtendedComID[0] = 0x07;
	hdr->cp.ExtendedComID[1] = 0xfe;
	hdr->cp.ExtendedComID[2] = 0x00;
	hdr->cp.ExtendedComID[3] = 0x00;
	hdr->cp.Length = 1;
	hdr->pkt.Length = 2;
	hdr->subpkt.Length = 3;
	/*
	* for session startup the packet and the data sub packet
	* just need a length an we wont know that until after
	* we have added the payload and padding
	*/
	bufferpos = sizeof(TCGHeader);
	buffer[bufferpos++] = TCG_TOKEN::CALL;
	buffer[bufferpos++] = TCG_SHORT_ATOM::BYTESTRING8;
	memcpy(&buffer[bufferpos], &TCGUSER[InvokingUid][0], 8); /* bytes 2-9 */
	bufferpos += 8;
	buffer[bufferpos++] = TCG_SHORT_ATOM::BYTESTRING8;
	memcpy(&buffer[bufferpos], &TCGMETHOD[method][0], 8); /* bytes 11-18 */
	bufferpos += 8;
}
void TCGCommand::addToken(TCG_TOKEN token) {
	buffer[bufferpos++] = token;
}
void TCGCommand::addToken(TCG_TINY_ATOM token) {
	buffer[bufferpos++] = token;
}
void TCGCommand::addToken(TCG_USER token) {
	buffer[bufferpos++] = TCG_SHORT_ATOM::BYTESTRING8;
	memcpy(&buffer[bufferpos], &TCGUSER[token][0], 8);
	bufferpos += 8;
}
void TCGCommand::complete() {
	buffer[bufferpos++] = TCG_TOKEN::ENDOFDATA;
	buffer[bufferpos++] = TCG_TOKEN::STARTLIST;
	buffer[bufferpos++] = 0x00;
	buffer[bufferpos++] = 0x00;
	buffer[bufferpos++] = 0x00;
	buffer[bufferpos++] = TCG_TOKEN::ENDLIST;
	/* fill in the lengths and add the modulo 4 padding */
	TCGHeader * hdr;
	hdr = (TCGHeader *)buffer;
	hdr->subpkt.Length = SWAP32(bufferpos - sizeof(TCGHeader));
	while (bufferpos % 4 != 0){
		buffer[bufferpos++] = 0x00;
	}
	hdr->pkt.Length = SWAP32((bufferpos - sizeof(TCGComPacket))
		- sizeof(TCGPacket));
	hdr->cp.Length = SWAP32(bufferpos - sizeof(TCGComPacket));
}
void TCGCommand::dump() {
	HexDump(buffer, bufferpos + +sizeof(TCGHeader) + 2);
}
TCGCommand::~TCGCommand()
{
	free (buffer);
}
