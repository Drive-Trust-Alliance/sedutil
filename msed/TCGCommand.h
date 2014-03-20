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
/** A class to build TCG Command streams.
 * This class attempts to closely mimic the command 
 * pseudo code used in the TCG documents, the syntactic
 * sugar is not represented.  See TCG document Storage Archatecture
 Core Specification R2.00 V2.00 Section 3.2.1.2 for all 
 * the gory details.
 * 
 * This class works eith the session class to construct and exchange 
 * commands with the TPer.  
 *
 * see also TCGLexicon for structs, typedefs and enums used to encode 
 * the bytestream.
 */
#include "TCGLexicon.h"
class TCGCommand
{
public:
	TCGCommand(TCG_USER InvokingUid,TCG_METHOD method);
	~TCGCommand();
	void addToken(TCG_TOKEN token);
	void addToken(TCG_TINY_ATOM token);
	void addToken(TCG_USER token);
	void complete();
	void dump();
private:
#define TCGUSER_SIZE 3
	unsigned char TCGUSER[TCGUSER_SIZE][8];
#define TCGMETHOD_SIZE 1
	unsigned char TCGMETHOD[TCGMETHOD_SIZE][8];
	unsigned char *buffer;
	UINT32 bufferpos = 0;
	//TCG_USER InvokingUid;
	//TCG_METHOD method;
};

