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
TCGcommand::TCGcommand(uint16_t ID, TCG_UID InvokingUid, TCG_METHOD method)
{
	LOG(D4) << "Creating TCGvommand(uint16_t ID, TCG_UID InvokingUid, TCG_METHOD method)";
    /* allocate the buffer */
    buffer = (uint8_t *) ALIGNED_ALLOC(4096, IO_BUFFER_LENGTH);
    reset(ID, InvokingUid, method);
}

/* Fill in the header information ONLY (no call) */
void
TCGcommand::reset(uint16_t comID)
{
	LOG(D4) << "Entering TCGcommand::reset(uint16_t comID)";
    memset(buffer, 0, IO_BUFFER_LENGTH);
    TCGHeader * hdr;
    hdr = (TCGHeader *) buffer;
    hdr->cp.extendedComID[0] = ((comID & 0xff00) >> 8);
    hdr->cp.extendedComID[1] = (comID & 0x00ff);
    hdr->cp.extendedComID[2] = 0x00;
    hdr->cp.extendedComID[3] = 0x00;
    hdr->pkt.TSN = TSN;
    hdr->pkt.HSN = HSN;
    bufferpos = sizeof (TCGHeader);
}

void
TCGcommand::reset(uint16_t comID, TCG_UID InvokingUid, TCG_METHOD method)
{
	LOG(D4) << "Entering TCGcommand::reset(uint16_t comID, TCG_UID InvokingUid, TCG_METHOD method)";
    reset(comID); // build the headers
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
        LOG(E) << "FAIL -- can't send LARGE ATOM size bytestring in 2048 Packet";
    }
    memcpy(&buffer[bufferpos], bytestring, (strlen(bytestring)));
    bufferpos += (strlen(bytestring));

}

void
TCGcommand::addToken(TCG_TOKEN token)
{
	LOG(D4) << "Entering TCGcommand::addToken(TCG_TOKEN token)";
	buffer[bufferpos++] = token;
}

void
TCGcommand::addToken(TCG_TINY_ATOM token)
{
	LOG(D4) << "Entering TCGcommand::addToken(TCG_TINY_ATOM token)";
	buffer[bufferpos++] = token;
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

uint8_t
TCGcommand::execute(TCGdev * d, void * resp)
{
	LOG(D4) << "Entering TCGcommand::execute(TCGdev * d, void * resp)";
    uint8_t iorc;
    iorc = SEND(d);
    if (0x00 == iorc)
        iorc = RECV(d, resp);
    return iorc;
}

uint8_t
TCGcommand::SEND(TCGdev * d)
{
	LOG(D4) << "Entering TCGcommand::SEND(TCGdev * d)";
    return d->sendCmd(IF_SEND, TCGProtocol, d->comID(), buffer, IO_BUFFER_LENGTH);
}

uint8_t
TCGcommand::RECV(TCGdev * d, void * resp)
{
	LOG(D4) << "Entering TCGcommand::RECV(TCGdev * d, void * resp)";
    return d->sendCmd(IF_RECV, TCGProtocol, d->comID(), resp, IO_BUFFER_LENGTH);
}

uint8_t
TCGcommand::startSession(TCGdev * device,
                         uint32_t hostSession,
                         TCG_UID SP,
                         uint8_t Write,
                         char * HostChallenge,
                         TCG_UID SignAuthority)
{
	LOG(D4) << "Entering TCGcommand::startSession ";
	int rc = 0;
    reset(device->comID(), TCG_UID::TCG_SMUID_UID, TCG_METHOD::STARTSESSION);
    addToken(TCG_TOKEN::STARTLIST); // [  (Open Bracket)
    addToken(hostSession); // HostSessionID : sessionnumber
    addToken(SP); // SPID : SP
    if (Write)
        addToken(TCG_TINY_ATOM::UINT_01);
    else
        addToken(TCG_TINY_ATOM::UINT_00);
    if (NULL != HostChallenge) {
        addToken(TCG_TOKEN::STARTNAME);
        addToken(TCG_TINY_ATOM::UINT_00); // first optional paramater
        addToken(HostChallenge);
        addToken(TCG_TOKEN::ENDNAME);
        addToken(TCG_TOKEN::STARTNAME);
        addToken(TCG_TINY_ATOM::UINT_03); // fourth optional paramater
        addToken(SignAuthority);
        addToken(TCG_TOKEN::ENDNAME);
    }
    addToken(TCG_TOKEN::ENDLIST); // ]  (Close Bracket)
    complete();
    setProtocol(0x01);
    LOG(D3) << "Dumping StartSession";
    IFLOG(D3) dump(); 
    rc = SEND(device);
    if (0 != rc) {
        LOG(E) << "StartSession failed on send " << rc;
        return rc;
    }
    //    Sleep(250);
    memset(buffer, 0, IO_BUFFER_LENGTH);
    rc = RECV(device, buffer);
    if (0 != rc) {
        LOG(E) << "StartSession failed on recv" <<  rc;
        return rc;
    }
    LOG(D3) << "Dumping StartSession Reply (SyncSession)";
    IFLOG(D3) dump();
    SSResponse * ssresp = (SSResponse *) buffer;
    if (0x49 != SWAP32(ssresp->h.cp.length) || (0 == ssresp->TPerSessionNumber)) {
        LOG(E) << "Invalid SyncSession response";
        return 0xff;
    }
    HSN = ssresp->HostSessionNumber;
    TSN = ssresp->TPerSessionNumber;
    return 0;
}

uint8_t
TCGcommand::endSession(TCGdev * device)
{
	LOG(D4) << "Entering TCGcommand::endSession";
    int rc = 0;
    reset(device->comID());
    addToken(TCG_TOKEN::ENDOFSESSION); // [  (Open Bracket)
    complete(0);
    HSN = 0;
    TSN = 0;
    rc = SEND(device);
    if (0 != rc) {
        LOG(E) << "EndSession failed on send rc =" << rc;
        return rc;
    }
    memset(buffer, 0, IO_BUFFER_LENGTH);
    rc = RECV(device, buffer);
    if (0 != rc) {
        LOG(E) << "EndSession failed on recv  rc =" << rc;
        return rc;
    }
    TCGHeader * resp = (TCGHeader *) buffer;
    if (0x25 != SWAP32(resp->cp.length)) {
		LOG(E) << "Invalid EndSession response";
        return 0xff;
    }
	LOG(D3) << "Dumping EndSession Reply";
    IFLOG(D3) dump();
    return 0;
}

void
TCGcommand::setProtocol(uint8_t value)
{
	LOG(D4) << "Entering TCGcommand::setProtocol";
    TCGProtocol = value;
}

void
TCGcommand::dump()
{
	LOG(D4) << "Entering TCGcommand::dump";
	LOG(D3) << "Dumping TCGCommand buffer";
    hexDump(buffer, bufferpos);
}

TCGcommand::~TCGcommand()
{
	LOG(D4) << "Destroying TCGcommand";
    ALIGNED_FREE(buffer);
}
