/* C:B**************************************************************************
This software is Copyright 2014,2015 Michael Romeo <r0m30@r0m30.com>

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
#include <linux/hdreg.h>
#include <errno.h>
#include <vector>
#include <fstream>
#include "MsedDev.h"
#include "MsedHexDump.h"

using namespace std;

/** The Device class represents a single disk device.
 *  Linux specific implementation using the SCSI generic interface and
 *  SCSI ATA Pass Through (12) command
 */
MsedDev::MsedDev(const char * devref)
{
    LOG(D1) << "Creating MsedDev::MsedDev() " << devref;
    dev = devref;
    ifstream kopts;
    
    if(access("/dev/sda", R_OK | W_OK)) {
        LOG(E) << "You do not have permission to access the raw disk in write mode";
        LOG(E) << "Perhaps you might try sudo to run as root";
    }
    kopts.open("/sys/module/libata/parameters/allow_tpm", ios::in);
    if (!kopts) {
	LOG(W) << "Unable to verify Kernel flag libata.allow_tpm ";
    } 
    else {
        if('1' !=  kopts.get()) {
            LOG(E) << "The Kernel flag libata.allow_tpm is not set correctly";
               LOG(E) << "Please see the readme note about setting the libata.allow_tpm ";
        }
        kopts.close();   
    }
    
    memset(&disk_info, 0, sizeof (OPAL_DiskInfo));
    if ((fd = open(dev, O_RDWR)) < 0) {
        isOpen = FALSE;
        // This is a D1 because diskscan looks for open fail to end scan
        LOG(D1) << "Error opening device " << dev << " " << (int32_t) fd;
        //        if (-EPERM == fd) {
        //            LOG(E) << "You do not have permission to access the raw disk in write mode";
        //            LOG(E) << "Perhaps you might try sudo to run as root";
        //        }
    }
    else {
        isOpen = TRUE;

        identify();
        if (!disk_info.devType) discovery0();
    }
}

/** Send an ioctl to the device using pass through. */
uint8_t MsedDev::sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                         void * buffer, uint16_t bufferlen)
{
    sg_io_hdr_t sg;
    uint8_t sense[32]; // how big should this be??
    uint8_t cdb[12];

    LOG(D1) << "Entering MsedDev::sendCmd";
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
     * bits 1-0 T_LENGTH -  10 = the length id in sector count
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
    //    LOG(D4) << "cdb before ";
    //    IFLOG(D4) hexDump(cdb, sizeof (cdb));
    //    LOG(D4) << "sg before ";
    //    IFLOG(D4) hexDump(&sg, sizeof (sg));
    /*
     * Do the IO
     */
    if (ioctl(fd, SG_IO, &sg) < 0) {
        LOG(D4) << "cdb after ";
        IFLOG(D4) MsedHexDump(cdb, sizeof (cdb));
        LOG(D4) << "sense after ";
        IFLOG(D4) MsedHexDump(sense, sizeof (sense));
        return 0xff;
    }
    //    LOG(D4) << "cdb after ";
    //    IFLOG(D4) hexDump(cdb, sizeof (cdb));
    //    LOG(D4) << "sg after ";
    //    IFLOG(D4) hexDump(&sg, sizeof (sg));
    //    LOG(D4) << "sense after ";
    //    IFLOG(D4) hexDump(sense, sizeof (sense));
    if (!((0x00 == sense[0]) && (0x00 == sense[1])))
        if (!((0x72 == sense[0]) && (0x0b == sense[1]))) return 0xff; // not ATA response
    return (sense[11]);
}

void MsedDev::identify()
{
    LOG(D4) << "Entering MsedDev::identify()";
    vector<uint8_t> nullz(512, 0x00);
    if (!isOpen) return; //disk open failed so this will too
    uint8_t * buffer = (uint8_t *) ALIGNED_ALLOC(4096, IO_BUFFER_LENGTH);
    memset(buffer, 0, IO_BUFFER_LENGTH);
    if (ioctl(fd, HDIO_GET_IDENTITY, buffer) < 0) {
        LOG(E) << "Identify failed " << strerror(errno);
        disk_info.devType = 1;
        return;
    }

    if (!(memcmp(nullz.data(), buffer, 512))) {
        disk_info.devType = 1;
        return;
    }
    IDENTIFY_RESPONSE * id = (IDENTIFY_RESPONSE *) buffer;
    disk_info.devType = id->devType;
    memcpy(disk_info.serialNum, id->serialNum, sizeof (disk_info.serialNum));
    memcpy(disk_info.firmwareRev, id->firmwareRev, sizeof (disk_info.firmwareRev));
    memcpy(disk_info.modelNum, id->modelNum, sizeof (disk_info.modelNum));
    // looks like linux does the byte flipping for you
    //    for (int i = 0; i < sizeof (disk_info.serialNum); i += 2) {
    //        disk_info.serialNum[i] = id->serialNum[i + 1];
    //        disk_info.serialNum[i + 1] = id->serialNum[i];
    //    }
    //    for (int i = 0; i < sizeof (disk_info.firmwareRev); i += 2) {
    //        disk_info.firmwareRev[i] = id->firmwareRev[i + 1];
    //        disk_info.firmwareRev[i + 1] = id->firmwareRev[i];
    //    }
    //    for (int i = 0; i < sizeof (disk_info.modelNum); i += 2) {
    //        disk_info.modelNum[i] = id->modelNum[i + 1];
    //        disk_info.modelNum[i + 1] = id->modelNum[i];
    //    }

    ALIGNED_FREE(buffer);
    return;
}

void MsedDev::osmsSleep(uint32_t ms)
{

    usleep(ms * 1000); //convert to microseconds
    return;
}

/** Close the device reference so this object can be delete. */
MsedDev::~MsedDev()
{
    LOG(D1) << "Destroying MsedDev";
    close(fd);
}