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
#ifdef __gnu_linux__
#include <unistd.h>
#endif
#include "TCGCommand.h"
#include "Device.h"
#include "Endianfixup.h"
#include "HexDump.h"
#include "TCGStructures.h"

TCGCommand::TCGCommand(uint16_t ID, TCG_UID InvokingUid, TCG_METHOD method)
{
    /* ******************* BS ALERT **************************
     * this is ugly and stupid but VS2013 gives an error when I
     * try and initialize these in the header declaration
     */
    uint8_t tu[TCGUID_SIZE][8]{
        // users
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff}, // session management
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}, // special "thisSP" syntax
        {0x00, 0x00, 0x02, 0x05, 0x00, 0x00, 0x00, 0x01}, // Administrative SP
        {0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x01}, //anybody
        {0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x06}, // SID
        // tables
        {0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x84, 0x02}, // C_PIN_MSID

    };
    memcpy(TCGUID, tu, TCGUID_SIZE * 8);

    uint8_t tm[TCGMETHOD_SIZE][8]{
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x01}, // Properties
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x02}, //STARTSESSION
        {0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x16}, // Get
    };
    memcpy(TCGMETHOD, tm, TCGMETHOD_SIZE * 8);
    /* ******************* BS ALERT ************************* */
    /*
     * allocate the buffer and build the call *
     */
    buffer = (uint8_t *) ALIGNED_ALLOC(512, IO_BUFFER_LENGTH);
    reset(ID, InvokingUid, method);
}

void
TCGCommand::reset(uint16_t ID, TCG_UID InvokingUid, TCG_METHOD method)
{
    comID = ID;
    memset(buffer, 0, IO_BUFFER_LENGTH);
    TCGHeader * hdr;
    /* build the headers */
    hdr = (TCGHeader *) buffer;
    /* ComPacket */
    hdr->cp.ExtendedComID[0] = ((comID & 0xff00) >> 8);
    hdr->cp.ExtendedComID[1] = (comID & 0x00ff);

    hdr->cp.ExtendedComID[2] = 0x00;
    hdr->cp.ExtendedComID[3] = 0x00;
    hdr->pkt.TSN = TSN;
    hdr->pkt.HSN = HSN;
    /*
     * for session startup the packet and the data sub packet
     * just need a length an we wont know that until after
     * we have added the payload and padding
     */
    bufferpos = sizeof (TCGHeader);
    buffer[bufferpos++] = TCG_TOKEN::CALL;
    buffer[bufferpos++] = TCG_SHORT_ATOM::BYTESTRING8;
    memcpy(&buffer[bufferpos], &TCGUID[InvokingUid][0], 8); /* bytes 2-9 */
    bufferpos += 8;
    buffer[bufferpos++] = TCG_SHORT_ATOM::BYTESTRING8;
    memcpy(&buffer[bufferpos], &TCGMETHOD[method][0], 8); /* bytes 11-18 */
    bufferpos += 8;
}

void
TCGCommand::addToken(uint16_t number)
{
    buffer[bufferpos++] = 0x82;
    buffer[bufferpos++] = ((number & 0xff00) >> 8);
    buffer[bufferpos++] = (number & 0x00ff);
}

void
TCGCommand::addToken(const char * bytestring)
{
    if (strlen(bytestring) < 16) {
        /* use tiny atom */
        buffer[bufferpos++] = strlen(bytestring) | 0xa0;

    }
    else if (strlen(bytestring) < 2048) {
        /* Use Medium Atom */
        buffer[bufferpos++] = 0xd0;
        buffer[bufferpos++] = 0x0000 | ((strlen(bytestring)) & 0x00ff);
    }
    else {
        /* Use Large Atom */
        printf("\n\nFAIL missing code -- large atom for bytestring \n");
    }
    memcpy(&buffer[bufferpos], bytestring, (strlen(bytestring)));
    bufferpos += (strlen(bytestring));

}

void
TCGCommand::addToken(TCG_TOKEN token)
{
    buffer[bufferpos++] = token;
}

void
TCGCommand::addToken(TCG_TINY_ATOM token)
{
    buffer[bufferpos++] = token;
}

void
TCGCommand::addToken(TCG_NAME token)
{
    buffer[bufferpos++] = token;
}

void
TCGCommand::addToken(TCG_UID token)
{
    buffer[bufferpos++] = TCG_SHORT_ATOM::BYTESTRING8;
    memcpy(&buffer[bufferpos], &TCGUID[token][0], 8);
    bufferpos += 8;
}

void
TCGCommand::complete()
{
    buffer[bufferpos++] = TCG_TOKEN::ENDOFDATA;
    buffer[bufferpos++] = TCG_TOKEN::STARTLIST;
    buffer[bufferpos++] = 0x00;
    buffer[bufferpos++] = 0x00;
    buffer[bufferpos++] = 0x00;
    buffer[bufferpos++] = TCG_TOKEN::ENDLIST;
    /* fill in the lengths and add the modulo 4 padding */
    TCGHeader * hdr;
    hdr = (TCGHeader *) buffer;
    hdr->subpkt.Length = SWAP32(bufferpos - sizeof (TCGHeader));
    while (bufferpos % 4 != 0) {
        buffer[bufferpos++] = 0x00;
    }
    hdr->pkt.Length = SWAP32((bufferpos - sizeof (TCGComPacket))
                             - sizeof (TCGPacket));
    hdr->cp.Length = SWAP32(bufferpos - sizeof (TCGComPacket));
}

uint8_t
TCGCommand::execute(Device * d, void * resp)
{
    uint8_t iorc;
    iorc = SEND(d);
    if (0x00 == iorc)
        iorc = RECV(d, resp);
    return iorc;
}

uint8_t
TCGCommand::SEND(Device * d)
{
    return d->SendCmd(IF_SEND, TCGProtocol, comID, buffer, IO_BUFFER_LENGTH);
}

uint8_t
TCGCommand::RECV(Device * d, void * resp)
{
    return d->SendCmd(IF_RECV, TCGProtocol, comID, resp, IO_BUFFER_LENGTH);
}

void
TCGCommand::setHSN(uint32_t value)
{
    HSN = value;
}

void
TCGCommand::setTSN(uint32_t value)
{
    TSN = value;
}

void
TCGCommand::setProtocol(uint8_t value)
{
    TCGProtocol = value;
}

void
TCGCommand::dump()
{
    printf("\n TCGCommand buffer\n");
    HexDump(buffer, bufferpos + + sizeof (TCGHeader) + 2);
}

TCGCommand::~TCGCommand()
{
    ALIGNED_FREE(buffer);
}
