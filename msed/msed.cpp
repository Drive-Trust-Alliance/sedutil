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
#include "TCGCommand.h"

int main(int argc, char * argv[])
{
	// DiskList *dList = new DiskList();
	TCGCommand *cmd = new TCGCommand(TCG_USER::SMUID, TCG_METHOD::STARTSESSION);
	cmd->addToken(TCG_TOKEN::STARTLIST);	// [  (Open Bracket)
	cmd->addToken(TCG_TINY_ATOM::uINT01);   // HostSessionID : 0x01
	cmd->addToken(TCG_USER::ADMINSP);		// SPID : ADMINSP 
	cmd->addToken(TCG_TINY_ATOM::uINT01);  // write : 1
	cmd->addToken(TCG_TOKEN::ENDLIST);	// ]  (Close Bracket)
	cmd->complete();
	cmd->dump();  // have a look see
	return 0;
}

