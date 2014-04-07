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
#include "TCGlexicon.h"
class TCGdev;

class TCGcommand {
public:
    TCGcommand();
    TCGcommand(TCG_UID InvokingUid, TCG_METHOD method);
    ~TCGcommand();
	void * getCmdBuffer();
	void * getRespBuffer();
    void addToken(TCG_TOKEN token);
    void addToken(TCG_TINY_ATOM token);
    void addToken(TCG_UID token);
    void addToken(const char * bytestring);
	void addToken(uint8_t bytes[], uint16_t size);
    void addToken(uint16_t);
	void setcomID(uint16_t comID);
	void setHSN(uint32_t HSN);
	void setTSN(uint32_t TSN);
    void complete(uint8_t EOD = 1);
    void reset();
    void reset(TCG_UID InvokingUid, TCG_METHOD method);
    void dump();
private:
    uint8_t *cmdbuf;
	uint8_t *respbuf;
    uint32_t bufferpos = 0;
};

