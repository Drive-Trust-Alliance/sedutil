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
#include <stdio.h>
void HexDump(void * address, int length) {
	UINT8 * cpos = (UINT8 *)address;
	UINT8 * epos = cpos + length;
	int rpos = 0;
	printf("\n");
	while (cpos < epos){
		printf("%04x %02x%02x%02x%02x %02x%02x%02x%02x ", rpos, cpos[0], cpos[1], cpos[2], cpos[3], cpos[4], cpos[5], cpos[6], cpos[7]);
		printf("%02x%02x%02x%02x %02x%02x%02x%02x\n", cpos[8], cpos[9], cpos[10], cpos[11], cpos[12], cpos[13], cpos[14], cpos[15]);
		cpos += 16;
		rpos += 16;
	}
}