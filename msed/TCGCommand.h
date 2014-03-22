#pragma once
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
 * sugar is not represented.  See TCG document Storage Archatecture
 Core Specification R2.00 V2.00 Section 3.2.1.2 for all 
 * the gory details.
 * 
 * See also TCGLexicon for structs, typedefs and enums used to encode 
 * the bytestream.
 */
#include "TCGLexicon.h"
class Device;
class TCGCommand
{
public:
	TCGCommand(UINT32 comIDex, TCG_USER InvokingUid,TCG_METHOD method);
	~TCGCommand();
	void addToken(TCG_TOKEN token);
	void addToken(TCG_TINY_ATOM token);
	void addToken(TCG_USER token);
	void addToken(TCG_NAME token);
	void addToken(char * bytestring);
	void addToken(UINT16);

	void setHSN(UINT32 value);
	void setTSN(UINT32 value);
	void setProtocol(UINT8 value);
	void complete();
	UINT8 execute(Device * device, LPVOID responseBuffer);
	void reset(UINT32 comIDex, TCG_USER InvokingUid, TCG_METHOD method);
	void dump();
private:
#define TCGUSER_SIZE 3
	unsigned char TCGUSER[TCGUSER_SIZE][8];
#define TCGMETHOD_SIZE 2
	unsigned char TCGMETHOD[TCGMETHOD_SIZE][8];
	unsigned char *buffer;
	UINT32 bufferpos = 0;
	/* The session numbers should be taken from the
	 * syncsession response so there will be no 
	 * issues with endianess
	 */
	UINT32 TSN = 0;
	UINT32 HSN = 0;
	UINT16 comID;
	UINT32 excomID;   // this is taken from the Tper so it's big endian
	UINT8 TCGProtocol = 0x00;
};

