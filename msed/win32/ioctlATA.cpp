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
#include "winos.h"
#include <Ntddscsi.h>
#include <stdio.h>
#include "ioctoATA.h"

UINT8 ioctlATA(HANDLE hDevice, ATACOMMAND cmd, UINT8 protocol, UINT16 comID, PVOID buffer, ULONG bufferlen) {
	UINT8 result = 0xff;               
	DWORD lasterr;
	DWORD ignored = 0;                     // discard results
	ATA_PASS_THROUGH_DIRECT * ata = (ATA_PASS_THROUGH_DIRECT *)malloc(sizeof(ATA_PASS_THROUGH_DIRECT));
	if (NULL == ata) return false;
	/* 
     * Initialize the ATA_PASS_THROUGH_DIRECT structures
     * per windows DOC with the secial sauce from the
     * ATA Command set reference (protocol and commID)
     */
	
	memset(ata, 0, sizeof(ATA_PASS_THROUGH_DIRECT));
	ata->Length = sizeof(ATA_PASS_THROUGH_DIRECT);
	if (IF_RECV == cmd)
		ata->AtaFlags = 0x00 | ATA_FLAGS_DRDY_REQUIRED | ATA_FLAGS_NO_MULTIPLE | ATA_FLAGS_DATA_IN;
	else
		ata->AtaFlags = 0x00 | ATA_FLAGS_DRDY_REQUIRED | ATA_FLAGS_NO_MULTIPLE | ATA_FLAGS_DATA_OUT; 
	ata->DataBuffer = buffer;
	ata->DataTransferLength = bufferlen;
	ata->TimeOutValue = 30;
	/* these were a b**** to find  defined in TCG specs but location is defined in ATA spec */
	ata->CurrentTaskFile[0] = protocol;             // Protocol
	ata->CurrentTaskFile[1] = int(bufferlen/512);             // Payload in number of 512 blocks
												// Damn self inflicted little endian bugs
	ata->CurrentTaskFile[3] = ((comID &0xff00) >> 8);        // Commid MSB
	ata->CurrentTaskFile[4] = (comID & 0x00ff);             // Commid LSB
	ata->CurrentTaskFile[6] = cmd;             // ata Command (0x5e or ox5c)

	DeviceIoControl(hDevice,							// device to be queried
		IOCTL_ATA_PASS_THROUGH_DIRECT,							// operation to perform
		ata, sizeof(ATA_PASS_THROUGH_DIRECT),
		ata, sizeof(ATA_PASS_THROUGH_DIRECT),
		&ignored,													// # bytes returned
		(LPOVERLAPPED)NULL);									// synchronous I/O
	result = ata->CurrentTaskFile[0];
	free(ata);
	//if (lasterr = GetLastError()){
	//	return (lasterr);
	//else
		return (result);
}



