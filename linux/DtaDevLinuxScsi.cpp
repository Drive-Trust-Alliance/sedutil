/* C:B**************************************************************************
   This software is Copyright (c) 2014-2024 Bright Plaza Inc. <drivetrust@drivetrust.com>

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
#include <map>
#include <fnmatch.h>

#include "DtaStructures.h"
#include "DtaDevLinuxSata.h"
#include "DtaDevLinuxScsi.h"
#include "DtaHexDump.h"
#include "ParseATIdentify.h"
#include "InterfaceDeviceID.h"
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
#define QUEUE_FULL           0x14

#define STATUS_MASK          0x3e



DtaDevLinuxScsi *
DtaDevLinuxScsi::getDtaDevLinuxScsi(const char * devref, DTA_DEVICE_INFO & di) {

  if (! (0 == fnmatch("sd[a-z]", devref, 0)))
    return NULL;

  int fd = fdopen(devref);
  if (fd < 0)
    return NULL;

  LOG(D1) << "Success opening device " << devref << " as file handle " << (int32_t) fd;


  InterfaceDeviceID interfaceDeviceIdentification;

  if (!identifyUsingSCSIInquiry(fd, interfaceDeviceIdentification, di)) {
    di.devType = DEVICE_TYPE_OTHER;
    LOG(E) << " Device " << devref << " is NOT SCSI?! -- file handle " << (int32_t) fd;
    close(fd);
    return NULL;
  }


  dictionary * identifyCharacteristics = NULL;
  if (DtaDevLinuxSata::identifyUsingATAIdentifyDevice(fd, interfaceDeviceIdentification, di, &identifyCharacteristics)) {
    // The completed identifyCharacteristics map could be useful to customizing code here
    if ( NULL != identifyCharacteristics ) {
      delete identifyCharacteristics;
      identifyCharacteristics = NULL ;
    }
    LOG(D4) << " Device " << devref << " is Sata";
    di.devType = DEVICE_TYPE_SATA;
    return new DtaDevLinuxSata(fd);
  }

  // Even though the device is SAS,
  // the (possibly partially completed) identifyCharacteristics map could be useful to customizing code here
  if ( NULL != identifyCharacteristics ) {
    delete identifyCharacteristics;
    identifyCharacteristics = NULL ;
  }
  LOG(D4) << " Device " << devref << " is not Sata, assuming plain Scsi (SAS)" ;
  di.devType = DEVICE_TYPE_SAS;
  return new DtaDevLinuxScsi(fd);
}



bool DtaDevLinuxScsi::deviceIsStandardSCSI(int fd, InterfaceDeviceID & interfaceDeviceIdentification, DTA_DEVICE_INFO &di)
{
  // Test whether device is a SCSI drive by attempting
  // SCSI Inquiry command
  // If it works, as a side effect, parse the Inquiry response
  // and save it in the IO Registry
  bool isStandardSCSI = false;
  unsigned int transferSize = sizeof(CScsiCmdInquiry_StandardData);
  void * inquiryResponse =  aligned_alloc(IO_BUFFER_ALIGNMENT, MIN_BUFFER_LENGTH);
  if ( inquiryResponse != NULL ) {
    bzero ( inquiryResponse, transferSize );
    isStandardSCSI = ( 0 == inquiryStandardDataAll_SCSI( fd,  inquiryResponse, transferSize ) );
    if (isStandardSCSI) {
      dictionary * characteristics =
        parseInquiryStandardDataAllResponse(static_cast <const unsigned char * >(inquiryResponse),
                                            interfaceDeviceIdentification,
                                            di);
      // Customization could use Inquiry characteristics
      delete characteristics ;
      characteristics = NULL ;
    }
    free(inquiryResponse);
    inquiryResponse = NULL;
  }
  return isStandardSCSI;
}

int DtaDevLinuxScsi::inquiryStandardDataAll_SCSI(int fd, void * inquiryResponse, unsigned int & dataSize )
{
  return __inquiry( fd, 0x00, 0x00, inquiryResponse, dataSize);
}



int DtaDevLinuxScsi::__inquiry(int fd, uint8_t evpd, uint8_t page_code, void * inquiryResponse, unsigned int & dataSize)
{
  static CScsiCmdInquiry cdb =
    { CScsiCmdInquiry::OPCODE , // m_Opcode
      0                       , // m_EVPD
      0x00                    , // m_Reserved_1
      0x00                    , // m_PageCode
      0x0000                  , // m_AllocationLength
      0x00                    , // m_Control
    };

  cdb.m_EVPD             = evpd;
  cdb.m_PageCode         = page_code;
  cdb.m_AllocationLength = htons(dataSize);

  return PerformSCSICommand(fd,
                            SG_DXFER_FROM_DEV,
                            (uint8_t *)&cdb, sizeof(cdb),
                            inquiryResponse, dataSize,
                            NULL, 0,  NULL);
}


static void safecopy(uint8_t * dst, size_t dstsize, const uint8_t * src, size_t srcsize)
{
  if (dstsize<=srcsize)
    memcpy(dst,src,dstsize);
  else {
    memcpy(dst,src,srcsize);
    memset(dst+srcsize, '\0', dstsize-srcsize);
  }
}

dictionary *
DtaDevLinuxScsi::parseInquiryStandardDataAllResponse(const unsigned char * response,
                                                     InterfaceDeviceID & interfaceDeviceIdentification,
                                                     DTA_DEVICE_INFO & di)
{
  const CScsiCmdInquiry_StandardData *resp = reinterpret_cast <const CScsiCmdInquiry_StandardData *>(response);

  memcpy(interfaceDeviceIdentification, resp->m_T10VendorId, sizeof(InterfaceDeviceID));

  // How sad is this?  // *** TODO *** Is this really correct?
  safecopy(di.serialNum, sizeof(di.serialNum), resp->m_T10VendorId, sizeof(resp->m_T10VendorId));

  safecopy(di.firmwareRev, sizeof(di.firmwareRev), resp->m_ProductRevisionLevel, sizeof(resp->m_ProductRevisionLevel));
  safecopy(di.modelNum, sizeof(di.modelNum), resp->m_ProductId, sizeof(resp->m_ProductId));

  // device is apparently a SCSI disk
  return new dictionary
    {
      {"Device Type"       , "SCSI"                       },
      {"Model Number"      , (const char *)di.modelNum    },
      {"Firmware Revision" , (const char *)di.firmwareRev },
      {"Serial Number"     , (const char *)di.serialNum   },
      {"Vendor ID"         , (const char *)di.vendorID    },
    };
}




/** Send a SCSI command using an ioctl to the device. */

int DtaDevLinuxScsi::PerformSCSICommand(int fd,
                                        int dxfer_direction,
                                        uint8_t * cdb,   unsigned char cdb_len,
                                        void * buffer,   unsigned int & bufferlen,
                                        uint8_t * sense, unsigned char senselen,
                                        unsigned char * pmasked_status)
{
  if (fd<=0) {
    LOG(E) << "Scsi device not open";
    return EBADF;
  }

  sg_io_hdr_t sg;
  bzero(&sg, sizeof(sg));

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
      if (sense != NULL) {
        LOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/) << "sense after ";
        DtaHexDump(sense, sizeof (sense));
      }
    }
  }

  // Without any real justification we set bufferlen to the value of dxfer_len - resid
  bufferlen = sg.dxfer_len - sg.resid;

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
        if (sense != NULL) {
          LOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/) << "sense after ";
          DtaHexDump(sense, sizeof (sense));
        }
      }
    }
  }
  return result;
}















#if NO_LONGER_USED






/** Send an ioctl to the device using pass through. */
uint8_t DtaDevLinuxSA::sendCmd_SAS(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                         void * buffer, uint32_t bufferlen)
{
    sg_io_hdr_t sg;
    uint8_t sense[32]; // how big should this be??
    uint8_t cdb[12];

    LOG(D1) << "Entering DtaDevLinuxSara::sendCmd_SAS";
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









bool DtaDevLinuxScsi::identifyUsingSCSIInquiry_OG(int fd,
                                                  InterfaceDeviceID & interfaceDeviceIdentification,
                                                  DTA_DEVICE_INFO & disk_info)
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
  int bufferlen = MIN_BUFFER_LENGTH;
  int result=PerformSCSICommand(fd,
                                SG_DXFER_FROM_DEV,
                                cdb, sizeof(cdb),
                                buffer, bufferlen,
                                sense, sizeof(sense),
                                &masked_status);

  if (result < 0 || masked_status != GOOD)  {
    disk_info.devType = DEVICE_TYPE_OTHER;
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
//   {
//     LOG(D4) << "cdb after sg.dxfer_len - sg.resid != sizeof(CScsiCmdInquiry_StandardData || resp->m_PeripheralDeviceType != 0x0";

  if ((bufferlen < sizeof(CScsiCmdInquiry_StandardData) || resp->m_PeripheralDeviceType != 0x0)
    {
      LOG(D4) << "cdb after bufferlen < sizeof(CScsiCmdInquiry_StandardData) || resp->m_PeripheralDeviceType != 0x0";
      IFLOG(D4) DtaHexDump(cdb, sizeof (cdb));
      LOG(D4) << "sense after ";
      IFLOG(D4) DtaHexDump(sense, sizeof (sense));
      disk_info.devType = DEVICE_TYPE_OTHER;
      // LOG(D4) << "sg.dxfer_len=" << sg.dxfer_len << " sg.resid=" << sg.resid <<
      // 		 " sizeof(CScsiCmdInquiry_StandardData)=" << sizeof(CScsiCmdInquiry_StandardData) <<
      // 		" resp->m_PeripheralDeviceType=" << resp->m_PeripheralDeviceType;
      LOG(D4) << "bufferlen=" << bufferlen << " resp->m_PeripheralDeviceType=" << std::hex << resp->m_PeripheralDeviceType;
      free(buffer);
      return false;
    }

  // fill out disk info fields
  safecopy(disk_info.serialNum, sizeof(disk_info.serialNum), resp->m_T10VendorId, sizeof(resp->m_T10VendorId));
  safecopy(disk_info.firmwareRev, sizeof(disk_info.firmwareRev), resp->m_ProductRevisionLevel, sizeof(resp->m_ProductRevisionLevel));
  safecopy(disk_info.modelNum, sizeof(disk_info.modelNum), resp->m_ProductId, sizeof(resp->m_ProductId));

  // device is apparently a SCSI disk
  disk_info.devType = DEVICE_TYPE_SAS;

  // free buffer and return
  free(buffer);
  return true;

}

#endif // NO_LONGER_USED
