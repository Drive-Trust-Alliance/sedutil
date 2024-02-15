/* C:B**************************************************************************
This software is Copyright 2014-2017 Bright Plaza Inc. <drivetrust@drivetrust.com>

This file is part of sedutil.

sedutil is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

sedutil is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sedutil.  If not, see <http://www.gnu.org/licenses/>.

 * C:E********************************************************************** */
#include "os.h"
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <scsi/sg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/hdreg.h>
#include <errno.h>
#include <vector>
#include <fstream>
#include "DtaDevLinuxScsi.h"
#include "DtaHexDump.h"
//
// taken from <scsi/scsi.h> to avoid SCSI/ATA name collision
//

/*
 *  Status codes
 */

#define GOOD                 0x00
#define CHECK_CONDITION      0x01
#define CONDITION_GOOD       0x02
#define BUSY                 0x04
#define INTERMEDIATE_GOOD    0x08
#define INTERMEDIATE_C_GOOD  0x0a
#define RESERVATION_CONFLICT 0x0c
#define COMMAND_TERMINATED   0x11
#define QUEUE_FULL




DtaDevLinuxScsi *
DtaDevLinuxScsi::getDtaDevLinuxScsi(const char * devref, DTA_DEVICE_INFO & di) {

  if (! (0 == fnmatch("sd[a-z]", devref)))
    return NULL;

  int fd = fdopen(devref);
  if (fd < 0)
    return NULL;

  LOG(D1) << "Success opening device " << devref << " as file handle " << (int32_t) fd;


  InterfaceDeviceID interfaceDeviceIdentification;

  if (!identifyUsingSCSIInquiry(fd, interfaceDeviceIdentification, di)) {
    LOG(E) << " Device "<< devref <<" is NOT SCSI?! -- file handle " << (int32_t) fd;
    close(fd);
    return NULL;
  }

  // Early check for SAS TPer as SAT testing messes some of those up :(
  if (deviceIsTPer_SCSI(fd, di) {
    LOG(D4) << " Device "<< devref <<" is SAS TPer" ;
    return new DtaDevLinuxScsi(fd);
  }

  std::unordered_map * identifyCharacteristics = NULL;
  if (deviceIsSata(fd, interfaceDeviceIdentification, di, &identifyCharacteristics)) {
    auto result = deviceIsTPer_Sata(fd, interfaceDeviceIdentification, identifyCharacteristics, di);
    if ( NULL != identifyCharacteristics ) {
      delete identifyCharacteristics;
      identifyCharacteristics = NULL ;
    }
    if ( result ) {
      LOG(D4) << " Device "<< devref <<" is Sata TPer" ;
    } else {
      LOG(D4) << " Device "<< devref <<" is Sata but not a TPer -- assuming plain Sata" ;
    }
    return new DtaDevLinuxSata(fd);
  }

  LOG(D4) << " Device " << devref << " is neither SAS TPer nor Sata -- assuming plain SAS" ;
  return new DtaDevLinuxScsi(fd);

}



bool DtaDevLinuxScsi::identifyUsingSCSIInquiry(InterfaceDeviceID & interfaceDeviceIdentification,
                                               DTA_DEVICE_INFO &di) {
    if (!deviceIsStandardSCSI(interfaceDeviceIdentification, di)) {
        IOLOG_DEBUG_METHOD(" Device is not Standard SCSI -- not for this driver");
        return false;
    }
    IOLOG_DEBUG_METHOD(" Device identification fields:");
    IOLOGBUFFER_DEBUG(NULL, interfaceDeviceIdentification, sizeof(InterfaceDeviceID));


    // Extract information from Inquiry VPD pages
    //
    // ... or not.  Maybe someday.
    return true;
}



bool DtaDevLinuxScsi::deviceIsStandardSCSI(InterfaceDeviceID & interfaceDeviceIdentification, DTA_DEVICE_INFO &disk_info)
{

    uint8_t sense[18];
    uint8_t cdb[sizeof(CScsiCmdInquiry)];

    LOG(D4) << "Entering DtaDevLinuxScsi::identify_ScsiS()";
    uint8_t * buffer = (uint8_t *) aligned_alloc(IO_BUFFER_ALIGNMENT, MIN_BUFFER_LENGTH);

    memset(&cdb, 0, sizeof (cdb));
    memset(&sense, 0, sizeof (sense));


    // fill out SCSI command
    auto p = (CScsiCmdInquiry *) cdb;
    p->m_Opcode = p->OPCODE;
    p->m_AllocationLength = htons(sizeof(CScsiCmdInquiry_StandardData));

    // fill out SCSI Generic structure
    // execute I/O
  unsigned char masked_status;
  int result=PerformSCSICommand(fd,
                                SG_DXFER_FROM_DEV,
                                cdb, sizeof(cdb),
                                buffer, MIN_BUFFER_LENGTH,
                                sense, sizeof(sense),
                                &masked_status);

  if (result < 0 || masked_status != GOOD)  {
    disk_info->devType = DEVICE_TYPE_OTHER;
    free(buffer);
    return false;
  }

    // response is a standard INQUIRY (at least 36 bytes)
    auto resp = (CScsiCmdInquiry_StandardData *) buffer;

    // make sure SCSI target is disk

    // I decided to drop the resid test after reviewing this text from
    // https://tldp.org/HOWTO/SCSI-Generic-HOWTO/x356.html
    // -- it was the note that got me.  scm Thu Feb 15 03:00:18 PM EST 2024

    //

    // 6.20. resid

    // This is the residual count from the data transfer. It is 'dxfer_len'
    // less the number of bytes actually transferred. In practice it only
    // reports underruns (i.e. positive number) as data overruns should never
    // happen. This value will be zero if there was no underrun or the SCSI
    // adapter doesn't support this feature. [1] The type of resid is int .

    // Notes

    // [1] Unfortunately some adapters drivers report an incorrect number for
    // 'resid'. This is due to some "fuzziness" in the internal interface
    // definitions within the Linux scsi subsystem concerning the _exact_
    // number of bytes to be transferred. Therefore only applications tied to
    // a specific adapter that is known to give the correct figure should use
    // this feature. Hopefully this will be cleared up in the near future.




    // if (((sg.dxfer_len - sg.resid) < sizeof(CScsiCmdInquiry_StandardData)) // some drive return more than sizeof(CScsiCmdInquiry_StandardData)
    //     || (resp->m_PeripheralDeviceType != 0x0))
    if (resp->m_PeripheralDeviceType != 0x0)
    {
        // LOG(D4) << "cdb after sg.dxfer_len - sg.resid != sizeof(CScsiCmdInquiry_StandardData || resp->m_PeripheralDeviceType != 0x0";
        LOG(D4) << "cdb after resp->m_PeripheralDeviceType != 0x0";
        IFLOG(D4) DtaHexDump(cdb, sizeof (cdb));
        LOG(D4) << "sense after ";
        IFLOG(D4) DtaHexDump(sense, sizeof (sense));
        disk_info->devType = DEVICE_TYPE_OTHER;
	// LOG(D4) << "sg.dxfer_len=" << sg.dxfer_len << " sg.resid=" << sg.resid <<
	// 		 " sizeof(CScsiCmdInquiry_StandardData)=" << sizeof(CScsiCmdInquiry_StandardData) <<
	// 		" resp->m_PeripheralDeviceType=" << resp->m_PeripheralDeviceType;
	LOG(D4) << "resp->m_PeripheralDeviceType=" << std::hex << resp->m_PeripheralDeviceType;
        free(buffer);
        return false;
    }

    // fill out disk info fields
    safecopy(disk_info->serialNum, sizeof(disk_info->serialNum), resp->m_T10VendorId, sizeof(resp->m_T10VendorId));
    safecopy(disk_info->firmwareRev, sizeof(disk_info->firmwareRev), resp->m_ProductRevisionLevel, sizeof(resp->m_ProductRevisionLevel));
    safecopy(disk_info->modelNum, sizeof(disk_info->modelNum), resp->m_ProductId, sizeof(resp->m_ProductId));

    // device is apparently a SCSI disk
    disk_info->devType = DEVICE_TYPE_SAS;
    isScsiS = 1;

    // free buffer and return
    free(buffer);
    return true;

}





bool DtaDevLinuxScsi::identify_ScsiS(DTA_DEVICE_INFO *disk_info)
{
    sg_io_hdr_t sg;
    uint8_t sense[18];
    uint8_t cdb[sizeof(CScsiCmdInquiry)];

    LOG(D4) << "Entering DtaDevLinuxScsi::identify_ScsiS()";
    uint8_t * buffer = (uint8_t *) aligned_alloc(IO_BUFFER_ALIGNMENT, MIN_BUFFER_LENGTH);

    memset(&cdb, 0, sizeof (cdb));
    memset(&sense, 0, sizeof (sense));
    memset(&sg, 0, sizeof (sg));

    // fill out SCSI command
    auto p = (CScsiCmdInquiry *) cdb;
    p->m_Opcode = p->OPCODE;
    p->m_AllocationLength = htons(sizeof(CScsiCmdInquiry_StandardData));

    // fill out SCSI Generic structure
    sg.interface_id = 'S';
    sg.dxfer_direction = SG_DXFER_FROM_DEV;
    sg.cmd_len = sizeof (cdb);
    sg.mx_sb_len = sizeof (sense);
    sg.iovec_count = 0;
    sg.dxfer_len = MIN_BUFFER_LENGTH;
    sg.dxferp = buffer;
    sg.cmdp = cdb;
    sg.sbp = sense;
    sg.timeout = 60000;
    sg.flags = 0;
    sg.pack_id = 0;
    sg.usr_ptr = NULL;

    // execute I/O
    if (ioctl(fd, SG_IO, &sg) < 0) {
        LOG(D4) << "cdb after ioctl(fd, SG_IO, &sg)";
        IFLOG(D4) DtaHexDump(cdb, sizeof (cdb));
        LOG(D4) << "sense after ";
        IFLOG(D4) DtaHexDump(sense, sizeof (sense));
        disk_info->devType = DEVICE_TYPE_OTHER;
        free(buffer);
        return false;
    }

    // check for successful target completion
    if (sg.masked_status != GOOD)
    {
        LOG(D4) << "cdb after sg.masked_status != GOOD";
        IFLOG(D4) DtaHexDump(cdb, sizeof (cdb));
        LOG(D4) << "sense after ";
        IFLOG(D4) DtaHexDump(sense, sizeof (sense));
        disk_info->devType = DEVICE_TYPE_OTHER;
        free(buffer);
        return false;
    }

    // response is a standard INQUIRY (at least 36 bytes)
    auto resp = (CScsiCmdInquiry_StandardData *) buffer;

    // make sure SCSI target is disk
    if (((sg.dxfer_len - sg.resid) < sizeof(CScsiCmdInquiry_StandardData)) // some drive return more than sizeof(CScsiCmdInquiry_StandardData)
        || (resp->m_PeripheralDeviceType != 0x0))
    {
        LOG(D4) << "cdb after sg.dxfer_len - sg.resid != sizeof(CScsiCmdInquiry_StandardData || resp->m_PeripheralDeviceType != 0x0";
        IFLOG(D4) DtaHexDump(cdb, sizeof (cdb));
        LOG(D4) << "sense after ";
        IFLOG(D4) DtaHexDump(sense, sizeof (sense));
        disk_info->devType = DEVICE_TYPE_OTHER;
	LOG(D4) << "sg.dxfer_len=" << sg.dxfer_len << " sg.resid=" << sg.resid <<
			 " sizeof(CScsiCmdInquiry_StandardData)=" << sizeof(CScsiCmdInquiry_StandardData) <<
			" resp->m_PeripheralDeviceType=" << resp->m_PeripheralDeviceType;
        free(buffer);
        return false;
    }

    // fill out disk info fields
    safecopy(disk_info->serialNum, sizeof(disk_info->serialNum), resp->m_T10VendorId, sizeof(resp->m_T10VendorId));
    safecopy(disk_info->firmwareRev, sizeof(disk_info->firmwareRev), resp->m_ProductRevisionLevel, sizeof(resp->m_ProductRevisionLevel));
    safecopy(disk_info->modelNum, sizeof(disk_info->modelNum), resp->m_ProductId, sizeof(resp->m_ProductId));

    // device is apparently a SCSI disk
    disk_info->devType = DEVICE_TYPE_ScsiS;
    isScsiS = 1;

    // free buffer and return
    free(buffer);
    return true;

}






























static
bool identifyUsingSCSIInquiry(int fd, InterfaceDeviceID interfaceDeviceIdentification, DTA_DEVICE_INFO * disk_info)
{
    uint8_t sense[18];
    uint8_t cdb[sizeof(CScsiCmdInquiry)];

    LOG(D4) << "Entering DtaDevLinuxScsi::identify_ScsiS()";
    uint8_t * buffer = (uint8_t *) aligned_alloc(IO_BUFFER_ALIGNMENT, MIN_BUFFER_LENGTH);

    memset(&cdb, 0, sizeof (cdb));
    memset(&sense, 0, sizeof (sense));
    memset(&sg, 0, sizeof (sg));

    // fill out SCSI command
    auto p = (CScsiCmdInquiry *) cdb;
    p->m_Opcode = p->OPCODE;
    p->m_AllocationLength = htons(sizeof(CScsiCmdInquiry_StandardData));

    // fill out SCSI Generic structure
    sg.interface_id = 'S';
    sg.dxfer_direction = SG_DXFER_FROM_DEV// ;
    // sg.cmd_len = sizeof (cdb);
    // sg.mx_sb_len = sizeof (sense);
    // sg.iovec_count = 0;
    // sg.dxfer_len = MIN_BUFFER_LENGTH;
    // sg.dxferp = buffer;
    // sg.cmdp = cdb;
    // sg.sbp = sense;
    // sg.timeout = 60000;
    // sg.flags = 0;
    // sg.pack_id = 0;
    // sg.usr_ptr = NULL;

    // // execute I/O
    // if (ioctl(fd, SG_IO, &sg) < 0) {
    //     LOG(D4) << "cdb after ioctl(fd, SG_IO, &sg)";
    //     IFLOG(D4) DtaHexDump(cdb, sizeof (cdb));
    //     LOG(D4) << "sense after ";
    //     IFLOG(D4) DtaHexDump(sense, sizeof (sense));
    //     disk_info->devType = DEVICE_TYPE_OTHER;
    //     free(buffer);
    //     return false;
    // }

    // // check for successful target completion
    // if (sg.masked_status != GOOD)
    // {
    //     LOG(D4) << "cdb after sg.masked_status != GOOD";
    //     IFLOG(D4) DtaHexDump(cdb, sizeof (cdb));
    //     LOG(D4) << "sense after ";
    //     IFLOG(D4) DtaHexDump(sense, sizeof (sense));
    //     disk_info->devType = DEVICE_TYPE_OTHER;
    //     free(buffer);
    //     return false;
    // }

    // // response is a standard INQUIRY (at least 36 bytes)
    // auto resp = (CScsiCmdInquiry_StandardData *) buffer;

    // // make sure SCSI target is disk
    // if (((sg.dxfer_len - sg.resid) < sizeof(CScsiCmdInquiry_StandardData)) // some drive return more than sizeof(CScsiCmdInquiry_StandardData)
    //     || (resp->m_PeripheralDeviceType != 0x0))
    // {
    //     LOG(D4) << "cdb after sg.dxfer_len - sg.resid != sizeof(CScsiCmdInquiry_StandardData || resp->m_PeripheralDeviceType != 0x0";
    //     IFLOG(D4) DtaHexDump(cdb, sizeof (cdb));
    //     LOG(D4) << "sense after ";
    //     IFLOG(D4) DtaHexDump(sense, sizeof (sense));
    //     disk_info->devType = DEVICE_TYPE_OTHER;
    //     LOG(D4) << "sg.dxfer_len=" << sg.dxfer_len << " sg.resid=" << sg.resid <<
    //     		 " sizeof(CScsiCmdInquiry_StandardData)=" << sizeof(CScsiCmdInquiry_StandardData) <<
    //     		" resp->m_PeripheralDeviceType=" << resp->m_PeripheralDeviceType;
    //     free(buffer);
    //     return false;
q    }

    // fill out disk info fields
    safecopy(disk_info->serialNum, sizeof(disk_info->serialNum), resp->m_T10VendorId, sizeof(resp->m_T10VendorId));
    safecopy(disk_info->firmwareRev, sizeof(disk_info->firmwareRev), resp->m_ProductRevisionLevel, sizeof(resp->m_ProductRevisionLevel));
    safecopy(disk_info->modelNum, sizeof(disk_info->modelNum), resp->m_ProductId, sizeof(resp->m_ProductId));

    // device is apparently a SCSI disk
    disk_info->devType = DEVICE_TYPE_ScsiS;

    // free buffer and return
    free(buffer);
    return true;
}


static
bool deviceIsTPer_SCSI(int fd, DTA_DEVICE_INFO & di);

static
bool deviceIsScsiT(int fd, InterfaceDeviceID interfaceDeviceIdentification, DTA_DEVICE_INFO & di, std::unordered_map * * pidentifyCharacteristics;);

static
bool deviceIsTPer_ScsiT(int fd, InterfaceDeviceID interfaceDeviceIdentification, std::unordered_map * identifyCharacteristics, DTA_DEVICE_INFO & di);





































using namespace std;

/** The Device class represents a single disk device.
 *  Linux specific implementation using the SCSI generic interface and
 *  SCSI ATA Pass Through (12) command
 */
DtaDevLinuxScsi::DtaDevLinuxScsi() {
isScsiS = 0;
}

bool DtaDevLinuxScsi::init(const char * devref)
{
    LOG(D1) << "Creating DtaDevLinuxScsi::DtaDev() " << devref;
	bool isOpen = FALSE;

    if (access(devref, R_OK | W_OK)) {
        LOG(E) << "You do not have permission to access the raw disk in write mode";
        LOG(E) << "Perhaps you might try sudo to run as root";
    }

    if ((fd = open(devref, O_RDWR)) < 0) {
        isOpen = FALSE;
        // This is a D1 because diskscan looks for open fail to end scan
        LOG(D1) << "Error opening device " << devref << " " << (int32_t) fd;
        //        if (-EPERM == fd) {
        //            LOG(E) << "You do not have permission to access the raw disk in write mode";
        //            LOG(E) << "Perhaps you might try sudo to run as root";
        //        }
    }
    else {
        isOpen = TRUE;
    }
	return isOpen;
}

/** Send an ioctl to the device using pass through. */
uint8_t DtaDevLinuxScsi::sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                         void * buffer, uint32_t bufferlen)
{
    if(isScsiS) {
        return(sendCmd_ScsiS(cmd, protocol, comID, buffer, bufferlen));
    }
    sg_io_hdr_t sg;
    uint8_t sense[32]; // how big should this be??
    uint8_t cdb[12];

    LOG(D1) << "Entering DtaDevLinuxScsi::sendCmd";
    memset(&cdb, 0, sizeof (cdb));
    memset(&sense, 0, sizeof (sense));
    memset(&sg, 0, sizeof (sg));
    /*
     * Initialize the CDB as described in ScsiT-2 and the
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
    sg.timeout = 60000;
    if (IF_RECV == cmd) {
        // how do I know it is discovery 0 ?
        cdb[1] = 4 << 1; // PIO DATA IN
        cdb[2] = 0x0E; // T_DIR = 1, BYTE_BLOCK = 1, Length in Sector Count
        cdb[4] = bufferlen / 512; // Sector count / transfer length (512b blocks)
        sg.dxfer_direction = SG_DXFER_FROM_DEV;
        sg.dxfer_len = bufferlen;
    }
    else if (IDENTIFY == cmd) {
        sg.timeout = 600; // Sabrent USB-ScsiTA adapter 1ms,6ms,20ms,60 NG, 600ms OK
        cdb[1] = 4 << 1; // PIO DATA IN
        cdb[2] = 0x0E; // T_DIR = 1, BYTE_BLOCK = 1, Length in Sector Count
        cdb[4] = 1; // Sector count / transfer length (512b blocks)
        sg.dxfer_direction = SG_DXFER_FROM_DEV;
        sg.dxfer_len = 512; // if not exactly 512-byte, exteremly long timeout, 2 - 3 minutes
    }
    else {
        cdb[1] = 5 << 1; // PIO DATA OUT
        cdb[2] = 0x06; // T_DIR = 0, BYTE_BLOCK = 1, Length in Sector Count
        cdb[4] = bufferlen / 512; // Sector count / transfer length (512b blocks)
        sg.dxfer_direction = SG_DXFER_TO_DEV;
        sg.dxfer_len = bufferlen;
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
    sg.dxfer_len = bufferlen;
    sg.dxferp = buffer;
    sg.cmdp = cdb;
    sg.sbp = sense;
    //sg.timeout = 60000;
    sg.flags = 0;
    sg.pack_id = 0;
    sg.usr_ptr = NULL;
    //    LOG(D4) << "cdb before ";
    //    IFLOG(D4) DtaHexDump(cdb, sizeof (cdb));
    //    LOG(D4) << "sg before ";
    //    IFLOG(D4) DtaHexDump(&sg, sizeof (sg));
    /*
     * Do the IO
     */
    if (ioctl(fd, SG_IO, &sg) < 0) {
    //    LOG(D4) << "cdb after ";
    //    IFLOG(D4) DtaHexDump(cdb, sizeof (cdb));
    //    LOG(D4) << "sense after ";
    //    IFLOG(D4) DtaHexDump(sense, sizeof (sense));
        return 0xff;
    }
    //    LOG(D4) << "cdb after ";
    //    IFLOG(D4) DtaHexDump(cdb, sizeof (cdb));
    //    LOG(D4) << "sg after ";
    //    IFLOG(D4) DtaHexDump(&sg, sizeof (sg));
    //    LOG(D4) << "sense after ";
    //    IFLOG(D4) DtaHexDump(sense, sizeof (sense));
    if (!((0x00 == sense[0]) && (0x00 == sense[1])))
        if (!((0x72 == sense[0]) && (0x0b == sense[1]))) return 0xff; // not ATA response
    return (sense[11]);
}

bool DtaDevLinuxScsi::identify(DTA_DEVICE_INFO& disk_info)
{
  LOG(D1) << "Entering DtaDevLinuxScsi::identify";
  sg_io_hdr_t sg;
  uint8_t sense[32]; // how big should this be??
  uint8_t cdb[12];
  memset(&cdb, 0, sizeof (cdb));
  memset(&sense, 0, sizeof (sense));
  memset(&sg, 0, sizeof (sg));
  LOG(D4) << "Entering DtaDevLinuxScsi::identify()";
  // uint8_t bus_sas = 0;
  vector<uint8_t> nullz(512, 0x00);
  uint8_t * buffer = (uint8_t *) memalign(IO_BUFFER_ALIGNMENT, MIN_BUFFER_LENGTH);
  memset(buffer, 0, MIN_BUFFER_LENGTH);
  /*
   * Initialize the CDB as described in ScsiT-2 and the
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
  cdb[1] = 4 << 1; // PIO DATA IN
  cdb[2] = 0x0E; // T_DIR = 1, BYTE_BLOCK = 1, Length in Sector Count
  sg.dxfer_direction = SG_DXFER_FROM_DEV;
  cdb[4] = 1;
  cdb[9] = 0xec; // IF_SEND/IF_RECV
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
  sg.dxfer_len = 512;
  sg.dxferp = buffer;
  sg.cmdp = cdb;
  sg.sbp = sense;
  sg.timeout = 60000;
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
    IFLOG(D4) DtaHexDump(cdb, sizeof (cdb));
    LOG(D4) << "sense after ";
    IFLOG(D4) DtaHexDump(sense, sizeof (sense));
    disk_info.devType = DEVICE_TYPE_OTHER;
    sendCmd(IDENTIFY, 0, 0, buffer, IO_BUFFER_LENGTH);
    // bus_sas =1;
  }

  uint8_t result;
  result = sendCmd(IDENTIFY, 0, 0, buffer, 512 );
  if (result) {
    LOG(D1) << "Exiting DtaDevLinuxScsi::identify (1)";
    return false;
  }





  //    LOG(D4) << "cdb after ";
  //    IFLOG(D4) hexDump(cdb, sizeof (cdb));
  //    LOG(D4) << "sg after ";
  //    IFLOG(D4) hexDump(&sg, sizeof (sg));
  //    LOG(D4) << "sense after ";
  //    IFLOG(D4) hexDump(sense, sizeof (sense));

  ifstream kopts;
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

  if (!(memcmp(nullz.data(), buffer, 512))) {
    disk_info.devType = DEVICE_TYPE_OTHER;
    // XXX: ioctl call was aborted or returned no data, most probably
    //      due to driver not being libata based, let's try ScsiS instead.
    LOG(D1) << "Exiting DtaDevLinuxScsi::identify (2)";
    return identify_ScsiS(&disk_info);
  }


  IDENTIFY_RESPONSE * id = (IDENTIFY_RESPONSE *) buffer;
  disk_info.devType = DEVICE_TYPE_ATA;
  // if (!bus_sas) {
  //   LOG(D1) << "DtaDevLinuxScsi::identify (3) -- bus_sas==0, not flipping";
  //   memcpy(disk_info.serialNum, id->serialNumber, sizeof (disk_info.serialNum));
  //   memcpy(disk_info.firmwareRev, id->firmwareRevision, sizeof (disk_info.firmwareRev));
  //   memcpy(disk_info.modelNum, id->modelNum, sizeof (disk_info.modelNum));

  // } else {
    // LOG(D1) << "DtaDevLinuxScsi::identify (3) -- bus_sas!=0, flipping";
    // looks like linux does the byte flipping for you, but not for device on ScsiS
    for (unsigned int i = 0; i < sizeof (disk_info.serialNum); i += 2) {
      disk_info.serialNum[i] = id->serialNumber[i + 1];
      disk_info.serialNum[i + 1] = id->serialNumber[i];
    }
    for (unsigned int i = 0; i < sizeof (disk_info.firmwareRev); i += 2) {
      disk_info.firmwareRev[i] = id->firmwareRevision[i + 1];
      disk_info.firmwareRev[i + 1] = id->firmwareRevision[i];
    }
    for (unsigned int i = 0; i < sizeof (disk_info.modelNum); i += 2) {
      disk_info.modelNum[i] = id->modelNum[i + 1];
      disk_info.modelNum[i + 1] = id->modelNum[i];
    }
  // }

  int nonp ;
  nonp=0;
  for (unsigned int i=0; i < sizeof (disk_info.serialNum) ; i++) {
    if (!isprint((unsigned)disk_info.serialNum[i])) {
      nonp=1;
      break;
    }
  }
  if (nonp) memset(disk_info.serialNum,0,sizeof(disk_info.serialNum));
  nonp=0;
  for (unsigned int i=0; i < sizeof (disk_info.firmwareRev) ; i++) {
    if (!isprint((unsigned)disk_info.firmwareRev[i])) {
      nonp=1;
      break;
    }
  }
  if (nonp) memset(disk_info.firmwareRev,0,sizeof(disk_info.firmwareRev));
  nonp=0;
  for (unsigned int i=0; i < sizeof (disk_info.modelNum) ; i++) {
    if (!isprint((unsigned)disk_info.modelNum[i])) {
      nonp=1;
      break;
    }
  }
  if (nonp) memset(disk_info.modelNum,0,sizeof(disk_info.modelNum));
  free(buffer);


  // TODO: Also do discovery0 here.

  LOG(D1) << "Exiting DtaDevLinuxScsi::identify (3)";
  return discovery0(disk_info) == DTAERROR_SUCCESS;

}


/** Send an ioctl to the device using pass through. */
uint8_t DtaDevLinuxScsi::sendCmd_ScsiS(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                         void * buffer, uint32_t bufferlen)
{
    sg_io_hdr_t sg;
    uint8_t sense[32]; // how big should this be??
    uint8_t cdb[12];

    LOG(D1) << "Entering DtaDevLinuxSara::sendCmd_ScsiS";
    memset(&cdb, 0, sizeof (cdb));
    memset(&sense, 0, sizeof (sense));
    memset(&sg, 0, sizeof (sg));


	LOG(D4) << "sizeof(unsigned)=" << sizeof(unsigned) << " sizeof(uint8_t)=" << sizeof(uint8_t) << " sizeof(uint16_t)=" << sizeof(uint16_t);

        // initialize SCSI CDB
        switch(cmd)
        {/* JERRY
        default:
        {
            return 0xff;
        }
	*/
        case IF_RECV:
        {
            auto * p = (CScsiCmdSecurityProtocolIn *) cdb;
            p->m_Opcode = p->OPCODE;
            p->m_SecurityProtocol = protocol;
            p->m_SecurityProtocolSpecific = htons(comID);
            //p->m_INC_512 = 1;
            //p->m_AllocationLength = htonl(bufferlen/512);
            p->m_INC_512 = 0;
            p->m_AllocationLength = htonl(bufferlen);
            break;
        }
        case IF_SEND:
        {
            auto * p = (CScsiCmdSecurityProtocolOut *) cdb;
            p->m_Opcode = p->OPCODE;
            p->m_SecurityProtocol = protocol;
            p->m_SecurityProtocolSpecific = htons(comID);
            //p->m_INC_512 = 1;
            //p->m_TransferLength = htonl(bufferlen/512);
            p->m_INC_512 = 0;
            p->m_TransferLength = htonl(bufferlen);
            break;
        }
        case IDENTIFY:
        {
	    return 0xff;
	}
        default:
        {
            return 0xff;
        }
        }

        // fill out SCSI Generic structure
        sg.interface_id = 'S';
        sg.dxfer_direction = (cmd == IF_RECV) ? SG_DXFER_FROM_DEV : SG_DXFER_TO_DEV;
        sg.cmd_len = sizeof (cdb);
        sg.mx_sb_len = sizeof (sense);
        sg.iovec_count = 0;
        sg.dxfer_len = bufferlen;
        sg.dxferp = buffer;
        sg.cmdp = cdb;
        sg.sbp = sense;
        sg.timeout = 60000;
        sg.flags = 0;
        sg.pack_id = 0;
        sg.usr_ptr = NULL;

        // execute I/O
        if (ioctl(fd, SG_IO, &sg) < 0) {
            LOG(D4) << "cdb after ioctl(fd, SG_IO, &sg) cmd( " << cmd ;
            IFLOG(D4) DtaHexDump(cdb, sizeof (cdb));
            LOG(D4) << "sense after ";
            IFLOG(D4) DtaHexDump(sense, sizeof (sense));
            return 0xff;
        }

        // check for successful target completion
        if (sg.masked_status != GOOD)
        {
            LOG(D4) << "cdb after sg.masked_status != GOOD cmd " << cmd;
            IFLOG(D4) DtaHexDump(cdb, sizeof (cdb));
            LOG(D4) << "sense after ";
            IFLOG(D4) DtaHexDump(sense, sizeof (sense));
            return 0xff;
        }

        // success
        return 0;
    }

static void safecopy(uint8_t * dst, size_t dstsize, uint8_t * src, size_t srcsize)
{
    const size_t size = min(dstsize, srcsize);
    if (size > 0) memcpy(dst, src, size);
    if (size < dstsize) memset(dst+size, '\0', dstsize-size);
}

bool DtaDevLinuxScsi::identify_ScsiS(DTA_DEVICE_INFO *disk_info)
{
    sg_io_hdr_t sg;
    uint8_t sense[18];
    uint8_t cdb[sizeof(CScsiCmdInquiry)];

    LOG(D4) << "Entering DtaDevLinuxScsi::identify_ScsiS()";
    uint8_t * buffer = (uint8_t *) aligned_alloc(IO_BUFFER_ALIGNMENT, MIN_BUFFER_LENGTH);

    memset(&cdb, 0, sizeof (cdb));
    memset(&sense, 0, sizeof (sense));
    memset(&sg, 0, sizeof (sg));

    // fill out SCSI command
    auto p = (CScsiCmdInquiry *) cdb;
    p->m_Opcode = p->OPCODE;
    p->m_AllocationLength = htons(sizeof(CScsiCmdInquiry_StandardData));

    // fill out SCSI Generic structure
    sg.interface_id = 'S';
    sg.dxfer_direction = SG_DXFER_FROM_DEV;
    sg.cmd_len = sizeof (cdb);
    sg.mx_sb_len = sizeof (sense);
    sg.iovec_count = 0;
    sg.dxfer_len = MIN_BUFFER_LENGTH;
    sg.dxferp = buffer;
    sg.cmdp = cdb;
    sg.sbp = sense;
    sg.timeout = 60000;
    sg.flags = 0;
    sg.pack_id = 0;
    sg.usr_ptr = NULL;

    // execute I/O
    if (ioctl(fd, SG_IO, &sg) < 0) {
        LOG(D4) << "cdb after ioctl(fd, SG_IO, &sg)";
        IFLOG(D4) DtaHexDump(cdb, sizeof (cdb));
        LOG(D4) << "sense after ";
        IFLOG(D4) DtaHexDump(sense, sizeof (sense));
        disk_info->devType = DEVICE_TYPE_OTHER;
        free(buffer);
/** Close the device reference so this object can be delete. */
DtaDevLinuxScsi::~DtaDevLinuxScsi()
{
    LOG(D1) << "Destroying DtaDevLinuxScsi";
    close(fd);
}







/** Send a SCSI command using an ioctl to the device. */

int DtaDevLinuxScsi::PerformSCSICommand(int fd,
                                        int dxfer_direction,
                                        uint8_t * cdb,
                                        unsigned char cdb_len,
                                        void * buffer,
                                        unsigned_int bufferlen,
                                        uint8_t * sense,
                                        unsigned char senselen
                                        unsigned char * pmasked_status)
{
  if (fd<0) {
    LOG(E) << "Scsi device not open";
    return EBADF;
  }

  sg_io_hdr_t sg;
  bzero(sg, sizeof(sg));

  sg.interface_id = 'S';
  sg.dxfer_direction = dxfer_direction;
  sg.cmd_len = cdb_len;
  sg.mx_sb_len = senselen;
  sg.dxfer_len = bufferlen;
  sg.dxferp = buffer;
  sg.cmdp = cdb;
  sg.sbp = sense;
  sg.timeout = 60000;


  //    LOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/) << "cdb before ";
  //    IFLOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/) hexDump(cdb, sizeof (cdb));
  //    LOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/) << "sg before ";
  //    IFLOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/) hexDump(&sg, sizeof (sg));

  /*
   * Do the IO
   */
  int result = ioctl(fd, SG_IO, &sg);
  IFLOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/) {
    if (result < 0) {
      LOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/)
        << "cdb after ioctl returned " << result << " (" << strerror(result) << ")" ;
      DtaHexDump(cdb, sizeof (cdb));
      LOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/) << "sense after ";
      DtaHexDump(sense, sizeof (sense));
    }
  }
  if (pmasked_status != NULL) {
    *pmasked_status = sg.masked_status;
#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)
#define maybe(status) (((*pmasked_status) & STATUS_MASK ) == status) ? STRINGIFY(status) :
    IFLOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/) {
      if (*pmasked_status != GOOD) {
        LOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/)
          << "cdb after sg.masked_status  == "
          << ( maybe(GOOD) // NO, we just checked this
               maybe(CHECK_CONDITION)
               maybe(CONDITION_GOOD)
               maybe(BUSY)
               maybe(INTERMEDIATE_GOOD)
               maybe(INTERMEDIATE_C_GOOD)
               maybe(RESERVATION_CONFLICT)
               maybe(COMMAND_TERMINATED)
               maybe(QUEUE_FULL)
               "????" )
          << " == " << std::hex << (int)sg.masked_status;
#undef maybe
#undef STRINGIFY
#undef STRINGIFY_
        DtaHexDump(cdb, sizeof (cdb));
        LOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/) << "sense after ";
        DtaHexDump(sense, sizeof (sense));
      }
    }
  }
  return result;
}
