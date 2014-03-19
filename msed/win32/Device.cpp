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
#include "..\os.h"
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include "Device.h"
#include "ioctlATA.h"

using namespace std;
/** Device Class (win32) represents a single disk device.
 *  This class is stored in a vector so to avoid writing a 
 *  copy operator and an assignment operator no custom destructor
 *  is used leading to this unfortunate class method structure
 */
Device::Device() {}
/** initializes the class device reference. */
void Device::init(TCHAR * devref) {
	dev = devref;
}
/** Send an ioctl to this device. */
UINT8 Device::SendCmd(ATACOMMAND cmd, UINT8 protocol, UINT16 comID, PVOID buffer, ULONG bufferlen) {
	if (!isOpen) {
		hDev = CreateFile((TCHAR *)dev,
			GENERIC_WRITE | GENERIC_READ,
			FILE_SHARE_WRITE | FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);
		if (INVALID_HANDLE_VALUE == hDev) {
			DWORD err = GetLastError();
			cout << "\nerror opening file " << dev << " " << err << "\n" << endl;
			return 0xff;
		}
	}
	isOpen = TRUE;
	return(ioctlATA(hDev, cmd, protocol, comID, buffer, bufferlen));
}
/** Close the filehandle so this object can be delete. */
void Device::close()
{
	if (isOpen) CloseHandle(hDev);
}
Device::~Device(){}