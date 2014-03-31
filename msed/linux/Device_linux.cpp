/* C:B**************************************************************************
This software is Copyright " 2014 Michael Romeo <r0m30@r0m30.com>

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
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <scsi/sg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../Device.h"
#include "../HexDump.h"

using namespace std;

/** The Device class represents a single disk device.
 *  Linux specific implementation using the SCSI generic interface and
 *  SCSI ATA Pass Through (12) command 0xa1
 */
Device::Device(const char * devref)
{
    dev = devref;
    if ((fd = open(dev, O_RDWR)) < 0)
        isOpen = FALSE;
    else
        isOpen = TRUE;
}

/** Send an ioctl to the device using pass through. */
uint8_t Device::SendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID, void * buffer, int16_t bufferlen)
{
    sg_io_hdr_t sg;
    uint8_t sense[32]; // how big should this be??
    uint8_t cdb[12];
    if (!isOpen) return 0xfe; //disk open failed so this will too
    memset(&cdb, 0, sizeof (cdb));
    memset(&sense, 0, sizeof (sense));
    memset(&sg, 0, sizeof (sg));
    /*
     * Initialize the CDB as described in SAT-2 and the
     * ATA Command set reference (protocol and commID placement)
     * We need a few more standards bodies --NOT--
     */

    cdb[0] = 0xa1; // ata pass through(12)
    /*
     * Byte 1 is the protocol 4 = PIO IN and 5 = PIO OUT
     * Byte 2 is:
     * bits 7-6 OFFLINE - Amount of time the command can take the bus offline
     * bit 5    CK_COND - If set the command will always return a condition check
     * bit 4    RESERVED
     * bit 3    T_DIR   - transfer direction 1 in, 0 out
     * bit 2    BYTE_BLock  1 = transfer in blocks, 0 transfer in bytes
     * bits 1-0 T_LENGTH - 10 = length in sector count
     */
    if (IF_RECV == cmd) {
        cdb[1] = 4 << 1; // PIO DATA IN
        cdb[2] = 0x0E; // T_DIR = 1, BYTE_BLOCK = 1, Length in Sector Count
        sg.dxfer_direction = SG_DXFER_FROM_DEV;
    }
    else {
        cdb[1] = 5 << 1; // PIO DATA OUT
        cdb[2] = 0x06; // T_DIR = 0, BYTE_BLOCK = 1, Length in Sector Count
        sg.dxfer_direction = SG_DXFER_TO_DEV;
    }
    cdb[3] = protocol; // ATA features / TRUSTED S/R security protocol
    cdb[4] = bufferlen / 512; // Sector count / transfer length (512b blocks)
    //      cdb[5] = reserved;
    cdb[6] = ((comID & 0xff00) >> 8);
    cdb[7] = (comID & 0x00ff);
    //      cdb[8] = 0x00;              // device
    cdb[9] = cmd; // IF_SEND/IF_RECV
    //      cdb[10] = 0x00;              // reserved
    //      cdb[11] = 0x00;              // control
    /*
     * Set up the SCSI Generic structure
     * see the SG HOWTO for the best info I could find
     */
    sg.interface_id = 'S';
    //      sg.dxfer_direction = Set in if above
    sg.cmd_len = sizeof (cdb);
    sg.mx_sb_len = sizeof (sense);
    sg.iovec_count = 0;
    sg.dxfer_len = IO_BUFFER_LENGTH;
    sg.dxferp = buffer;
    sg.cmdp = cdb;
    sg.sbp = sense;
    sg.timeout = 5000;
    sg.flags = 0;
    sg.pack_id = 0;
    sg.usr_ptr = NULL;
    //        printf("\ncdb before \n");
    //	HexDump(cdb, sizeof(cdb));
    //        printf("\nsg before \n");
    //	HexDump(&sg, sizeof(sg));
    /*
     * Do the IO
     */
    if (ioctl(fd, SG_IO, &sg) < 0) {
        printf("\ncdb after \n");
        HexDump(cdb, sizeof (cdb));
        printf("\nsg after \n");
        HexDump(&sg, sizeof (sg));
        printf("\nsense after \n");
        HexDump(sense, sizeof (sense));
        return 0xff;
    }
    printf("\ncdb after \n");
    HexDump(cdb, sizeof (cdb));
    printf("\nsg after \n");
    HexDump(&sg, sizeof (sg));
    printf("\nsense after \n");
    HexDump(sense, sizeof (sense));
    return (sense[11]);
}

/** Close the device reference so this object can be delete. */
Device::~Device()
{
    close(fd);
}