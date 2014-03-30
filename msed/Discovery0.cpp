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
#include "os.h" 
#include <iostream>
#include <stdio.h>
#include "Discovery0.h"
#include "D0Structures.h"
#include "Endianfixup.h"
#include "HexDump.h"


using namespace std;
/** Decode the Discovery 0 response. Scans the D0 response and creates get-able 
* variables that can be queried later as required.  This code also takes care of
* the endianess conversions either via a bitswap in the structure or executing
* a macro when the input buffer is read.
*/
/* TODO: this should throw an exception if a request is made for a field that 
 * was not returned in the D0 response 
 */
Discovery0::Discovery0(void * d0Response)
{
	char fc[10];
	char ver[10];
	char scratch[25];
	uint8_t * epos, *cpos;
	Discovery0Header * hdr;
	Discovery0Features * body;
	epos = cpos = (uint8_t *)d0Response;
	hdr = (Discovery0Header *)d0Response;
	cout << "\nDumping D0Response"<< std::endl;
	HexDump(hdr, SWAP32(hdr->length));
	epos = epos + SWAP32(hdr->length);
	cpos = cpos + 48;  // TODO: check header version 

	do  {
		body = (Discovery0Features *)cpos;
                snprintf(fc, 8, "0x%04x", SWAP16(body->TPer.featureCode));
		snprintf(ver, 8, "0x%02x", body->TPer.version);
		switch (SWAP16(body->TPer.featureCode)) {         /* could use of the structures here is a common field */
		case FC_TPER:					/* TPer */

			cout << "\nParsing TPer functions  (" << fc << " ver " << ver << ")" << std::endl;
			cout << "ACKNAK           = " << (body->TPer.acknack ? "Y" : "N") << std::endl;
			cout << "ASYNC            = " << (body->TPer.Async ? "Y" : "N") << std::endl;
			cout << "BufferManagement = " << (body->TPer.bufferManagement ? "Y" : "N") << std::endl;
			cout << "comIDManagement  = " << (body->TPer.comIDManagement ? "Y" : "N") << std::endl;
			cout << "Streaming        = " << (body->TPer.streaming ? "Y" : "N") << std::endl;
			cout << "Sync             = " << (body->TPer.Sync ? "Y" : "N") << std::endl;
			break;
		case FC_LOCKING:					/* Locking*/
			cout << "\nParsing Locking functions   (" << fc << " ver " << ver << ")" << std::endl;
			cout << "Locked             = " << (body->Locking.locked ? "Y" : "N") << std::endl;
			cout << "LockingEnabled     = " << (body->Locking.lockingEnabled ? "Y" : "N") << std::endl;
			cout << "LockingSupported   = " << (body->Locking.LockingSupported ? "Y" : "N") << std::endl;
			cout << "MBRDone            = " << (body->Locking.MBRDone ? "Y" : "N") << std::endl;
			cout << "MBREnabled         = " << (body->Locking.MBREnabled ? "Y" : "N") << std::endl;
			cout << "MediaEncrypt       = " << (body->Locking.mediaEncryption ? "Y" : "N") << std::endl;
			cout << std::endl;
			break;
		case FC_GEOMETRY:					/* Geometry Features */
			cout << "\nParsing Geometry Information   (" << fc << " ver " << ver << ")" << std::endl;
			cout << "Align                 = " << (body->Geometry.align ? "Y" : "N") << std::endl;
			cout << "Alignment Granularity = " << SWAP32(body->Geometry.alignmentGranularity) << std::endl;
			cout << "Logical Block size    = " << SWAP32(body->Geometry.logicalBlockSize) << std::endl;
			cout << "Lowest Aligned LBA    = " << SWAP32(body->Geometry.lowestAlighedLBA) << std::endl;
			break;
		case FC_ENTERPRISE:					/* Enterprise SSC */
			cout << "\nParsing Enterprise SSC   (" << fc << " ver " << ver << ")" << std::endl;
			cout << "Range crossing      = " << (body->EnterpriseSSC.rangeCrossing ? "Y" : "N") << std::endl;
			cout << "Base commID         = " << SWAP16(body->EnterpriseSSC.baseComID) << std::endl;
			cout << "Number of commIDs   = " << SWAP16(body->EnterpriseSSC.numberComIDs) << std::endl;
			break;
		case FC_SINGLEUSER:					/* Single User Mode */
			cout << "\nParsing Single User Mode   (" << fc << " ver " << ver << ")" << std::endl;
			cout << "ALL                         = " << (body->SingleUserMode.all ? "Y" : "N") << std::endl;
			cout << "ANY                         = " << (body->SingleUserMode.any ? "Y" : "N") << std::endl;
			cout << "Policy                      = " << (body->SingleUserMode.policy ? "Y" : "N") << std::endl;
			cout << "Number of Locking Objects   = " << SWAP32(body->SingleUserMode.numberLockingObjects) << std::endl;
			break;
		case FC_DATASTORE:					/* Datastore Tables */
			cout << "\nParsing Extended Datastore   (" << fc << " ver " << ver << ")" << std::endl;
			cout << "Max Tables             = " << SWAP16(body->Datastore.maxTables) << std::endl;
			cout << "Max Size Tables        = " << SWAP32(body->Datastore.maxSizeTables) << std::endl;
			cout << "Table size alignment   = " << SWAP32(body->Datastore.tableSizeAlignment) << std::endl;
			break;
		case FC_OPALV200:					/* OPAL V200 */
			cout << "\nParsing OPAL V200   (" << fc << " ver " << ver << ")" << std::endl;
			snprintf(scratch, 8, "0x%04x", SWAP16(body->OPALv200.baseCommID));
			cout << "Base commID                = " << scratch << std::endl;
			snprintf(scratch, 8, "0x%02x", body->OPALv200.initialPin);
			cout << "Initial PIN                = " << scratch << std::endl;
			snprintf(scratch, 8, "0x%02x", body->OPALv200.revertedPin);
			cout << "Reverted PIN               = " << scratch << std::endl;
			cout << "Number of commIDs          = " << SWAP16(body->OPALv200.numCommIDs) << std::endl;
			cout << "Number of Locking Admins   = " << SWAP16(body->OPALv200.numlockingAdminAuth) << std::endl;
			cout << "Number of Locking Users    = " << SWAP16(body->OPALv200.numlockingUserAuth) << std::endl;
			cout << "Range Crossing             = " << (body->OPALv200.rangeCrossing ? "Y" : "N") << std::endl;
			break;
		default:
			cout << "IGNORING unknown function code " << fc << " ver " << ver << std::endl;
			/* should do something here */
			break;
		}
		cpos = cpos + (body->TPer.length + 4);
	} while (cpos < epos);
}

Discovery0::~Discovery0()
{
}
