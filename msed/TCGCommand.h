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
#pragma once
/** A class to build & send TCG Command streams to a TPer.
 * This class attempts to closely mimic the command
 * pseudo code used in the TCG documents, the syntactic
 * sugar is not represented.  See TCG document Storage Architecture
 * Core Specification R2.00 V2.00 Section 3.2.1.2 for all
 * the gory details.
 *
 * See also TCGLexicon for structs, typedefs and enums used to encode
 * the bytestream.
 */
#include "TCGLexicon.h"
class Device;

class TCGCommand {
public:
    TCGCommand();
    TCGCommand(uint16_t comIDex, TCG_UID InvokingUid, TCG_METHOD method);
    ~TCGCommand();
    void addToken(TCG_TOKEN token);
    void addToken(TCG_TINY_ATOM token);
    void addToken(TCG_UID token);
    void addToken(const char * bytestring);
    void addToken(uint16_t);
    void setProtocol(uint8_t value);
    uint8_t SEND(Device * device);
    uint8_t RECV(Device * device, void * resp);
    void complete(uint8_t EOD = 1);
    uint8_t execute(Device * device, void * responseBuffer);
    void reset(uint16_t comID);
    void reset(uint16_t comID, TCG_UID InvokingUid, TCG_METHOD method);
    uint8_t startSession(Device * device, uint32_t HSN, TCG_UID SP,
            uint8_t Write,
            char * HostChallenge = NULL,
            TCG_UID SignAuthority = TCG_UID::TCG_UID_HEXFF);
    uint8_t endSession(Device * device);
    void dump();
private:
    uint8_t *buffer;
    uint32_t bufferpos = 0;
    /* The session numbers should be taken from the
     * syncsession response so there will be no
     * issues with endianess
     */
    uint32_t TSN = 0;
    uint32_t HSN = 0;
    uint8_t TCGProtocol = 0x01;
};

