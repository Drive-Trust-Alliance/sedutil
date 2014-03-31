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
#include <ctype.h>
void HexDump(void * address, int length) {
	uint8_t display[17];
	uint8_t * cpos = (uint8_t *)address;
	uint8_t * epos = cpos + length;
	int rpos = 0;
	int dpos = 0;
	printf("\n%04x ",rpos);
	while (cpos < epos){
		printf("%02x", cpos[0]);
		if (!((++rpos) % 4)) printf(" ");
		display[dpos++] = (isprint(cpos[0]) ? cpos[0] : 0x2e );
		cpos += 1;
		if (16 == dpos) {
			dpos = 0;
			display[16] = 0x00;
			printf(" %s \n", display);
			if(cpos < epos) printf("%04x ", rpos);
			memset(&display,0,sizeof(display));
		}
	}
	if (dpos != 0) {
		if (dpos % 4) printf(" ");
			printf("  ");
		for (int i = dpos ; i < 15; i++) {
			if (!(i % 4)) printf(" ");
			printf("  ");
		}
		display[dpos] = 0x00;
		printf(" %s\n", display);
	}
}