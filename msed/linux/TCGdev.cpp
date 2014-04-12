/* C:B**************************************************************************
This software is Copyright 2014 Michael Romeo <r0m30@r0m30.com>

This file is part of msed.

msed is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

msed is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with msed.  If not, see <http://www.gnu.org/licenses/>.

 * C:E********************************************************************** */
#include "os.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <scsi/sg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "TCGdev.h"
#include "hexDump.h"

using namespace std;

/** The Device class represents a single disk device.
 *  Linux specific implementation using the SCSI generic interface and
 *  SCSI ATA Pass Through (12) command
 */
TCGdev::TCGdev(const char * devref)
{
    LOG(D4) << "Creating TCGdev::TCGdev() " << devref;
    dev = devref;
    memset(&disk_info, 0, sizeof (TCG_DiskInfo));
    if ((fd = open(dev, O_RDWR)) < 0) {
        isOpen = FALSE;
        // This is a D1 because diskscan looks for open fail to end scan
        LOG(D1) << "Error opening device " << dev;
    }
    else {
        isOpen = TRUE;
        discovery0();
    }
}

/** Send an ioctl to the device using pass through. */
uint8_t TCGdev::sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                        void * buffer, uint16_t bufferlen)
{
    sg_io_hdr_t sg;
    uint8_t sense[32]; // how big should this be??
    uint8_t cdb[12];

    LOG(D4) << "Entering TCGdev::sendCmd";
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
    cdb[7] = ((comID & 0xff00) >> 8);
    cdb[6] = (comID & 0x00ff);
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
    LOG(D4) << "cdb before ";
    IFLOG(D4) hexDump(cdb, sizeof (cdb));
    //    LOG(D4) << "sg before ";
    //    IFLOG(D4) hexDump(&sg, sizeof (sg));
    /*
     * Do the IO
     */
    if (ioctl(fd, SG_IO, &sg) < 0) {
        LOG(D4) << "cdb after ";
        IFLOG(D4) hexDump(cdb, sizeof (cdb));
        //        LOG(D4) << "sg after ";
        //        IFLOG(D4) hexDump(&sg, sizeof (sg));
        LOG(D4) << "sense after ";
        IFLOG(D4) hexDump(sense, sizeof (sense));
        return 0xff;
    }
    LOG(D4) << "cdb after ";
    IFLOG(D4) hexDump(cdb, sizeof (cdb));
    //    LOG(D4) << "sg after ";
    //    IFLOG(D4) hexDump(&sg, sizeof (sg));
    LOG(D4) << "sense after ";
    IFLOG(D4) hexDump(sense, sizeof (sense));
    if (!((0x00 == sense[0]) && (0x00 == sense[1])))
        if (!((0x72 == sense[0]) && (0x0b == sense[1]))) return 0xff; // not ATA response
    return (sense[11]);
}

void TCGdev::osmsSleep(uint32_t ms)
{
    usleep(ms * 1000); //convert to microseconds
    return;
}

/** Close the device reference so this object can be delete. */
TCGdev::~TCGdev()
{
    LOG(D4) << "Destroying TCGdev";
    close(fd);
}