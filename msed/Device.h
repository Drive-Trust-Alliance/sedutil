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

#if (!defined _WIN32) && (!defined __gnu_linux__)
#error "Unsupported Operating System"
#endif
#include "TCGStructures.h"

class Device
{
public:
	Device(char * devref);
	~Device();
	uint8_t SendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
		void * buffer, uint16_t bufferlen);
	uint8_t isOpal2();
	uint8_t isPresent();
	uint16_t comID();
	void Puke();
private:
	void Discovery0();
	char * dev;
	TCG_FILE_DESCRIPTOR hDev;
	uint8_t isOpen = FALSE;
	void *ataPointer;
	TCG_DiskInfo disk_info;
};