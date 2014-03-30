/* C:B**************************************************************************
This software is Copyright � 2014 Michael Romeo <r0m30@r0m30.com>

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
#include "../os.h" 
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "../Device.h"
#include "../DiskList.h"
#include "../Discovery0.h"
/** List of Devices and their OPAL status. 
 * loops through the physical devices until
 * there is an open error querying each one 
 * for an OPAL response, if we get an OPAL response
 * then it is parsed and optionally displayed. 
 */
DiskList::DiskList()
{
	int i = 0;
	uint8_t iorc;
	Device * d;
	buffer = aligned_alloc(512, IO_BUFFER_LENGTH);  
	while (true) {
		snprintf(devname, 23, "\\\\.\\PhysicalDrive%i", i);
		snprintf(devname, 23, "\\\\.\\PhysicalDrive3");
		printf("Testing %s for TCG OPAL ... ", devname);
		d = new Device(devname);
//		disk.push_back(*d);
		memset(buffer, 0, IO_BUFFER_LENGTH);
		iorc = d->SendCmd(IF_RECV, 0x01, 0x0100, buffer, bufferlen);
		if (0xff == iorc) break;
		if (0x00 == iorc) {
			printf(" Yes\n");
			new Discovery0(buffer);
		} else {
			printf(" No\n");
		}
		break;
		if (i++ == MAX_DISKS) {
			std::cout << "20+ disks, really?" << std::endl;
			break;
		}
		delete d;
	}
	printf(" No disk present Ending Scan\n");
}


DiskList::~DiskList()
{
	if (NULL != buffer)
		free(buffer);
}
