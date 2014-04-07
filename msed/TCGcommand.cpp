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

 * C:E********************************************************************* */
#include "os.h"
#include <stdio.h>
#include "TCGcommand.h"
#include "TCGdev.h"
#include "endianfixup.h"
#include "hexDump.h"
#include "TCGstructures.h"
#include "noparser.h"

/*
 * Initialize: allocate the buffer ONLY reset needs to be called to
 * initialize the headers etc
 */
TCGcommand::TCGcommand()
{
	LOG(D4) << "Creating TCGcommand()";
    buffer = (uint8_t *) ALIGNED_ALLOC(4096, IO_BUFFER_LENGTH);
}

/* Fill in the header information and format the call */
TCGcommand::TCGcommand(TCG_UID InvokingUid, TCG_METHOD method)
{
	LOG(D4) << "Creating TCGvommand(uint16_t ID, TCG_UID InvokingUid, TCG_METHOD method)";
    /* allocate the buffer */
    buffer = (uint8_t *) ALIGNED_ALLOC(4096, IO_BUFFER_LENGTH);
    reset(InvokingUid, method);
}

/* Fill in the header information ONLY (no call) */
void
TCGcommand::reset()
{
	LOG(D4) << "Entering TCGcommand::reset(uint16_t comID)";
    memset(buffer, 0, IO_BUFFER_LENGTH);
    TCGHeader * hdr;
    hdr = (TCGHeader *) buffer;

    bufferpos = sizeof (TCGHeader);
}

void
TCGcommand::reset(TCG_UID InvokingUid, TCG_METHOD method)
{
	LOG(D4) << "Entering TCGcommand::reset(uint16_t comID, TCG_UID InvokingUid, TCG_METHOD method)";
    reset(); // build the headers
    buffer[bufferpos++] = TCG_TOKEN::CALL;
    buffer[bufferpos++] = TCG_SHORT_ATOM::BYTESTRING8;
    memcpy(&buffer[bufferpos], &TCGUID[InvokingUid][0], 8); /* bytes 2-9 */
    bufferpos += 8;
    buffer[bufferpos++] = TCG_SHORT_ATOM::BYTESTRING8;
    memcpy(&buffer[bufferpos], &TCGMETHOD[method][0], 8); /* bytes 11-18 */
    bufferpos += 8;
}

void
TCGcommand::addToken(uint16_t number)
{
	LOG(D4) << "Entering TCGcommand::addToken(uint16_t number)";
    buffer[bufferpos++] = 0x82;
    buffer[bufferpos++] = ((number & 0xff00) >> 8);
    buffer[bufferpos++] = (number & 0x00ff);
}

void
TCGcommand::addToken(const char * bytestring)
{
	LOG(D4) << "Entering TCGcommand::addToken(const char * bytestring)";
	uint16_t length = (uint16_t) strlen(bytestring);
    if (strlen(bytestring) < 16) {
        /* use tiny atom */
        buffer[bufferpos++] = (uint8_t) length | 0xa0;
    }
    else if(length < 2048) {
        /* Use Medium Atom */
        buffer[bufferpos++] = 0xd0 | (uint8_t) ((length >> 8) & 0x07);
        buffer[bufferpos++] = (uint8_t) (length & 0x00ff);
    }
    else {
        /* Use Large Atom */
        LOG(E) << "FAIL -- can't send LARGE ATOM size bytestring in 2048 Packet";
    }
    memcpy(&buffer[bufferpos], bytestring, (strlen(bytestring)));
    bufferpos += (strlen(bytestring));

}

void
TCGcommand::addToken(TCG_TOKEN token)
{
	LOG(D4) << "Entering TCGcommand::addToken(TCG_TOKEN token)";
	buffer[bufferpos++] = (uint8_t) token;
}

void
TCGcommand::addToken(TCG_TINY_ATOM token)
{
	LOG(D4) << "Entering TCGcommand::addToken(TCG_TINY_ATOM token)";
	buffer[bufferpos++] = (uint8_t) token;
}

void
TCGcommand::addToken(TCG_UID token)
{
	LOG(D4) << "Entering TCGcommand::addToken(TCG_UID token)";
	buffer[bufferpos++] = TCG_SHORT_ATOM::BYTESTRING8;
    memcpy(&buffer[bufferpos], &TCGUID[token][0], 8);
    bufferpos += 8;
}

void
TCGcommand::complete(uint8_t EOD)
{
	LOG(D4) << "Entering TCGcommand::complete(uint8_t EOD)";
    if (EOD) {
        buffer[bufferpos++] = TCG_TOKEN::ENDOFDATA;
        buffer[bufferpos++] = TCG_TOKEN::STARTLIST;
        buffer[bufferpos++] = 0x00;
        buffer[bufferpos++] = 0x00;
        buffer[bufferpos++] = 0x00;
        buffer[bufferpos++] = TCG_TOKEN::ENDLIST;
    }
    /* fill in the lengths and add the modulo 4 padding */
    TCGHeader * hdr;
    hdr = (TCGHeader *) buffer;
    hdr->subpkt.length = SWAP32(bufferpos - sizeof (TCGHeader));
    while (bufferpos % 4 != 0) {
        buffer[bufferpos++] = 0x00;
    }
    hdr->pkt.length = SWAP32((bufferpos - sizeof (TCGComPacket))
                             - sizeof (TCGPacket));
    hdr->cp.length = SWAP32(bufferpos - sizeof (TCGComPacket));
}

void *
TCGcommand::getBuffer()
{
	return buffer;
}
void
TCGcommand::setcomID(uint16_t comID)
{
	TCGHeader * hdr;
	hdr = (TCGHeader *)buffer;
	LOG(D4) << "Entering TCGcommand::setcomID()";
	hdr->cp.extendedComID[0] = ((comID & 0xff00) >> 8);
	hdr->cp.extendedComID[1] = (comID & 0x00ff);
	hdr->cp.extendedComID[2] = 0x00;
	hdr->cp.extendedComID[3] = 0x00;
}

void 
TCGcommand::setTSN(uint32_t TSN) {
	TCGHeader * hdr;
	hdr = (TCGHeader *)buffer;
	LOG(D4) << "Entering TCGcommand::setTSN()";
	hdr->pkt.TSN = TSN;
}

void 
TCGcommand::setHSN(uint32_t HSN) {
	TCGHeader * hdr;
	hdr = (TCGHeader *)buffer;
	LOG(D4) << "Entering TCGcommand::setHSN()";
	hdr->pkt.HSN = HSN;
}

void
TCGcommand::dump()
{
	LOG(D4) << "Entering TCGcommand::dump()";
	LOG(D3) << "Dumping TCGCommand buffer";
    hexDump(buffer, bufferpos);
}

TCGcommand::~TCGcommand()
{
	LOG(D4) << "Destroying TCGcommand";
    ALIGNED_FREE(buffer);
}
