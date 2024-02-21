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


bool DtaDevLinuxScsi::identifyUsingSCSIInquiry(int fd,
                                               InterfaceDeviceID & interfaceDeviceIdentification,
                                               DTA_DEVICE_INFO & disk_info) {

  if (!deviceIsStandardSCSI(fd, interfaceDeviceIdentification, disk_info)) {
    LOG(E) << " Device is not Standard SCSI -- not for this driver";
    return false;
  }



#if defined(EXTRACT_INFORMATION_FROM_INQUIRY_VPD_PAGES)
  // Extract information from Inquiry VPD pages
  //

  bool deviceSupportsPage80=false;
  bool deviceSupportsPage89=false;
#if defined(USE_INQUIRY_PAGE_00h)
  if (deviceIsPage00SCSI(deviceSupportsPage80,
                         deviceSupportsPage89)) {
    LOG(D4) <<" Device is Page 00 SCSI";
    LOG(D4) <<" Device %s support Page 80h",
      deviceSupportsPage80 ? "DOES" : "DOES NOT";
    LOG(D4) <<" Device %s support Page 89h",
      deviceSupportsPage89 ? "DOES" : "DOES NOT";
  } else  {
    LOG(D4) <<" Device is not Page 00 SCSI";
#undef ALLOW_INQUIRY_PAGE_00_FAILURES
#if defined( ALLOW_INQUIRY_PAGE_00_FAILURES )
    // Some external USB-SATA adapters do not support the VPD pages but it's OK
    // For instance, the Innostor Technology IS888 USB3.0 to SATA bridge identifies its
    // medium, not itself, in the Inquiry response, so we have no way of matching on it
    // short of delving into the USB world
    return true;  // ¯\_(ツ)_/¯
#else // !defined( ALLOW_INQUIRY_PAGE_00_FAILURES )
    return false;  // Mandatory, according to standard
#endif // defined( ALLOW_INQUIRY_PAGE_00_FAILURES )
  }
#endif // defined(USE_INQUIRY_PAGE_00h)

#if defined(USE_INQUIRY_PAGE_80h)
  if (deviceSupportsPage80) {
    if (deviceIsPage80SCSI(interfaceDeviceIdentification, di)) {
      LOG(D4) <<" Device is Page 80 SCSI";
    } else  {
      LOG(D4) <<" Device is not Page 80 SCSI";
      return false;  // Claims to support it on Page 00h, but does not
    }
  }
#endif // defined(USE_INQUIRY_PAGE_80h)

#if defined(USE_INQUIRY_PAGE_83h)
  if (deviceIsPage83SCSI(di)) {
    LOG(D4) <<" Device is Page 83 SCSI";
  } else  {
    LOG(D4) <<" Device is not Page 83 SCSI";
    return false;  // Mandatory, according to standard
  }
#endif // defined(USE_INQUIRY_PAGE_83h)


#if defined(USE_INQUIRY_PAGE_89h)
  if (deviceSupportsPage89) {
    if (deviceIsPage89SCSI(di)) {
      LOG(D4) <<" Device is Page 89 SCSI";
    } else  {
      LOG(D4) <<" Device is not Page 89 SCSI";
      return false;   // Claims to support it on page 00h, but does not
    }
  }
#if DRIVER_DEBUG
  else {
    LOG(D4) <<" Device does not claim to support Page 89 -- trying it anyway";
    if (deviceIsPage89SCSI(di)) {
      LOG(D4) <<" Device is Page 89 SCSI!!";
    }
  }
#endif
#endif // defined(USE_INQUIRY_PAGE_89h)

#if DRIVER_DEBUG
  deviceIsPageXXSCSI(kINQUIRY_PageB0_PageCode, IOInquiryPageB0ResponseKey);
  deviceIsPageXXSCSI(kINQUIRY_PageB1_PageCode, IOInquiryPageB1ResponseKey);
  deviceIsPageXXSCSI(kINQUIRY_PageB2_PageCode, IOInquiryPageB2ResponseKey);
  deviceIsPageXXSCSI(kINQUIRY_PageC0_PageCode, IOInquiryPageC0ResponseKey);
  deviceIsPageXXSCSI(kINQUIRY_PageC1_PageCode, IOInquiryPageC1ResponseKey);
#endif


#endif // defined(EXTRACT_INFORMATION_FROM_INQUIRY_VPD_PAGES)



  return true;
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





/** Send an ioctl to the device using pass through. */
uint8_t DtaDevLinuxScsi::sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                                  void * buffer, uint32_t bufferlen)
{
  LOG(D1) << "Entering DtaDevLinuxSara::sendCmd_SAS";


  int dxfer_direction;

  // initialize SCSI CDB and dxfer_direction
  uint8_t cdb[12];
  memset(&cdb, 0, sizeof (cdb));
  switch(cmd)
    {
    case IF_RECV:
      {
        dxfer_direction = SG_DXFER_FROM_DEV;
        auto * p = (CScsiCmdSecurityProtocolIn *) cdb;
        p->m_Opcode = p->OPCODE;
        p->m_SecurityProtocol = protocol;
        p->m_SecurityProtocolSpecific = htons(comID);
        p->m_INC_512 = 1;
        p->m_AllocationLength = htonl(bufferlen/512);
        break;
      }
    case IF_SEND:
      {
        dxfer_direction = SG_DXFER_TO_DEV;
        auto * p = (CScsiCmdSecurityProtocolOut *) cdb;
        p->m_Opcode = p->OPCODE;
        p->m_SecurityProtocol = protocol;
        p->m_SecurityProtocolSpecific = htons(comID);
        p->m_INC_512 = 1;
        p->m_TransferLength = htonl(bufferlen/512);
        break;
      }
    default:
      {
        return 0xff;
      }
    }


  // execute I/O
  unsigned int transferlen = bufferlen;
  uint8_t sense[32]; // how big should this be??
  memset(&sense, 0, sizeof (sense));
  uint8_t masked_status=GOOD;
  int result=PerformSCSICommand(dxfer_direction,
                                cdb, sizeof(cdb),
                                buffer, transferlen,
                                sense, sizeof(sense),
                                &masked_status);
  if (result < 0) {
    LOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/) << "cdb after ";
    IFLOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/) DtaHexDump(cdb, sizeof (cdb));
    LOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/) << "sense after ";
    IFLOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/) DtaHexDump(sense, sizeof (sense));
    return 0xff;
  }

  // check for successful target completion
  if (masked_status != GOOD)
    {
      LOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/) << "cdb after ";
      IFLOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/) DtaHexDump(cdb, sizeof (cdb));
      LOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/) << "sense after ";
      IFLOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/) DtaHexDump(sense, sizeof (sense));
      return 0xff;
    }

  // success
  return 0x00;
}

bool  DtaDevLinuxScsi::identify(DTA_DEVICE_INFO& disk_info)
{
  InterfaceDeviceID interfaceDeviceIdentification;
  return identifyUsingSCSIInquiry(fd, interfaceDeviceIdentification, disk_info);
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

// Pulled in from master just now: Mon Feb 19 13:43:40 EST 2024


void DtaDevLinuxSata::identify_SAS(OPAL_DiskInfo *disk_info)
{
    sg_io_hdr_t sg;
    uint8_t sense[18];
    uint8_t cdb[sizeof(CScsiCmdInquiry)];

    LOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/) << "Entering DtaDevLinuxSata::identify_SAS()";
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
        LOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/) << "cdb after ";
        IFLOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/) DtaHexDump(cdb, sizeof (cdb));
        LOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/) << "sense after ";
        IFLOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/) DtaHexDump(sense, sizeof (sense));
        disk_info->devType = DEVICE_TYPE_OTHER;
        free(buffer);
        return;
    }

    // check for successful target completion
    if (sg.masked_status != GOOD)
    {
        LOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/) << "cdb after ";
        IFLOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/) DtaHexDump(cdb, sizeof (cdb));
        LOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/) << "sense after ";
        IFLOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/) DtaHexDump(sense, sizeof (sense));
        disk_info->devType = DEVICE_TYPE_OTHER;
        free(buffer);
        return;
    }

    // response is a standard INQUIRY (at least 36 bytes)
    auto resp = (CScsiCmdInquiry_StandardData *) buffer;

    // make sure SCSI target is disk
    if (sg.dxfer_len - sg.resid != sizeof(CScsiCmdInquiry_StandardData)
        || resp->m_PeripheralDeviceType != 0x0)
    {
        LOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/) << "cdb after ";
        IFLOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/) DtaHexDump(cdb, sizeof (cdb));
        LOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/) << "sense after ";
        IFLOG(D1 /*** DEBUG ***TODO FIX BACK TO D4 *** DEBUG ***/) DtaHexDump(sense, sizeof (sense));
        disk_info->devType = DEVICE_TYPE_OTHER;
        free(buffer);
        return;
    }

    //***DEBUG***
    printf("DtaDevLinuxSata::identify_SAS -- Inquiry successful, resp:\n");
    DtaHexDump(resp, sizeof(CScsiCmdInquiry_StandardData));
    //***DEBUG***


    // fill out disk info fields
    safecopy(disk_info->serialNum, sizeof(disk_info->serialNum), resp->m_T10VendorId, sizeof(resp->m_T10VendorId));
    safecopy(disk_info->firmwareRev, sizeof(disk_info->firmwareRev), resp->m_ProductRevisionLevel, sizeof(resp->m_ProductRevisionLevel));
    safecopy(disk_info->modelNum, sizeof(disk_info->modelNum), resp->m_ProductId, sizeof(resp->m_ProductId));

    // device is apparently a SCSI disk
    disk_info->devType = DEVICE_TYPE_SAS;
    isSAS = 1;

    // free buffer and return
    free(buffer);
    return;
}


#endif // NO_LONGER_USED
