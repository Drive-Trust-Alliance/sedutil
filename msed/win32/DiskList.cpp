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
#include <stdlib.h>
#include <stdio.h>
#include "..\Device.h"
#include "..\DiskList.h"
/** Brute force disk scan. 
 * loops through the physical devices until
 * there is an open error. Creates a Device 
 * and reports OPAL support. 
 */
DiskList::DiskList()
{
	int i = 0;
	Device * d;
	printf("\nScanning for Opal 2.0 compliant disks\n");
	while (TRUE) {
		SNPRINTF(devname, 23, "\\\\.\\PhysicalDrive%i", i);
		//		sprintf_s(devname, 23, "\\\\.\\PhysicalDrive3", i);
		d = new Device(devname);
		if (d->isPresent()) {
			printf("%s %s", devname, (d->isOpal2() ? " Yes\n" : " No \n"));
			if(MAX_DISKS == i) {
				printf("%d + disks, really?\n",MAX_DISKS);
				delete d;
				break;
			}
		}
		else break;

		delete d;
		i += 1;
	}
	printf("\n No more disks present ending scan\n");
}


DiskList::~DiskList()
{
	if (NULL != buffer)
		_aligned_free(buffer);
}
