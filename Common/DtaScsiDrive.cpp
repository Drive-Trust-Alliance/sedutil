/* C:B**************************************************************************
   This software is © 2014 Bright Plaza Inc. <drivetrust@drivetrust.com>

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
#include "DtaScsiDrive.h"
#include "DtaHexDump.h"

DtaDrive *
DtaScsiDrive::getDtaScsiDrive(const char * devref, DTA_DEVICE_INFO & device_info) {

  bool accessDenied=false;
  OSDEVICEHANDLE osDeviceHandle = OS.openAndCheckDeviceHandle(devref, accessDenied);
  if (osDeviceHandle==INVALID_HANDLE_VALUE || accessDenied) {
    if (osDeviceHandle!=INVALID_HANDLE_VALUE)  OS.closeDeviceHandle(osDeviceHandle);
   return NULL;
  }

  LOG(D4) << "DtaScsiDrive::getDtaScsiDrive: "
          << "Success opening device " << devref << " as file handle " << HEXON(2) << (size_t) osDeviceHandle;


  InterfaceDeviceID interfaceDeviceIdentification;

  // In theory every USB device should respond to Scsi commands, in particularly Inquiry.  Is this true?
  if (! identifyUsingSCSIInquiry(osDeviceHandle, interfaceDeviceIdentification, device_info)) {
    device_info.devType = DEVICE_TYPE_OTHER;
    // TODO:  Restore this comment once we sort out UAS vs internal SCSI
    // LOG(E) << " Device " << devref << " is NOT Scsi?! -- file handle " << HEXON(8) << (size_t) osDeviceHandle;
    OS.closeDeviceHandle(osDeviceHandle);
    return NULL;
  }

  return getDtaOSSpecificScsiDrive(osDeviceHandle);
}




bool DtaScsiDrive::identifyUsingSCSIInquiry(OSDEVICEHANDLE osDeviceHandle,
                                            InterfaceDeviceID & interfaceDeviceIdentification,
                                            DTA_DEVICE_INFO & disk_info) {
  LOG(D4) <<"DtaScsiDrive::identifyUsingSCSIInquiry(osDeviceHandle=" << HEXON(8) << ", ...)" ;
  if (!deviceIsStandardSCSI(osDeviceHandle, interfaceDeviceIdentification, disk_info)) {
    LOG(D4) << "DtaScsiDrive::identifyUsingSCSIInquiry: "
            << "Device is not standard SCSI, returning false";
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



bool DtaScsiDrive::deviceIsStandardSCSI(OSDEVICEHANDLE osDeviceHandle, InterfaceDeviceID & interfaceDeviceIdentification, DTA_DEVICE_INFO &di)
{
  // Test whether device is a SCSI drive by attempting
  // SCSI Inquiry command
  // If it works, as a side effect, parse the Inquiry response
  // and save it in the IO Registry
  bool isStandardSCSI = false;
  unsigned int transferSize = sizeof(CScsiCmdInquiry_StandardData);
    void * inquiryResponse =  OS.alloc_aligned_MIN_BUFFER_LENGTH_buffer();
  if ( inquiryResponse != NULL ) {
    memset(inquiryResponse, 0, MIN_BUFFER_LENGTH );
    isStandardSCSI = ( 0 == inquiryStandardDataAll_SCSI(osDeviceHandle, inquiryResponse, transferSize ) );
    if (isStandardSCSI) {
      dictionary * inquiryCharacteristics =
        parseInquiryStandardDataAllResponse(static_cast <const unsigned char * >(inquiryResponse),
                                            interfaceDeviceIdentification,
                                            di);
      // Customization could use Inquiry characteristics
      if ( NULL != inquiryCharacteristics ) {
        IFLOG(D3) {
          LOG(D3) << "DtaScsiDrive::deviceIsStandardSCSI(osDeviceHandle=" << HEXON(2) << osDeviceHandle << ", ...)";
          LOG(D3) << "inquiryCharacteristics for Scsi Device: ";
          for (dictionary::iterator it=inquiryCharacteristics->begin(); it!=inquiryCharacteristics->end(); it++)
            LOG(D3) << "  " << it->first << ":" << it->second << std::endl;
        }
        delete inquiryCharacteristics;
        inquiryCharacteristics = NULL ;
      }
    }
    OS.free_aligned_MIN_BUFFER_LENGTH_buffer(inquiryResponse);
    inquiryResponse = NULL;
  }
  return isStandardSCSI;
}

int DtaScsiDrive::inquiryStandardDataAll_SCSI(OSDEVICEHANDLE osDeviceHandle, void * inquiryResponse, unsigned int & dataSize )
{
  return __inquiry(osDeviceHandle, 0x00, 0x00, inquiryResponse, dataSize);
}



int DtaScsiDrive::__inquiry(OSDEVICEHANDLE osDeviceHandle, uint8_t evpd, uint8_t page_code, void * inquiryResponse, unsigned int & dataSize)
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
  unsigned char sense[32];
  unsigned char senselen=sizeof(sense);
  SCSI_STATUS_CODE masked_status;
  return OS.PerformSCSICommand(osDeviceHandle,
                               PSC_FROM_DEV,
                               (uint8_t *)&cdb, sizeof(cdb),
                               inquiryResponse, dataSize,
                               sense, senselen,
                               &masked_status);
}

dictionary *
DtaScsiDrive::parseInquiryStandardDataAllResponse(const unsigned char * response,
                                                     InterfaceDeviceID & interfaceDeviceIdentification,
                                                     DTA_DEVICE_INFO & device_info)
{
  const CScsiCmdInquiry_StandardData *resp = reinterpret_cast <const CScsiCmdInquiry_StandardData *>(response);

//  MSVC doesn't like this because it is sure we don't know that m_T10VendorID is too small.
//
//  memcpy(interfaceDeviceIdentification, resp->m_T10VendorId, sizeof(InterfaceDeviceID));
//
  const void* iDIPointer = resp->m_T10VendorId;
  memcpy(interfaceDeviceIdentification, iDIPointer, sizeof(InterfaceDeviceID));

  // How sad is this?  // *** TODO *** Is this really correct?
  safecopy(device_info.serialNum, sizeof(device_info.serialNum), resp->m_T10VendorId, sizeof(resp->m_T10VendorId));

  safecopy(device_info.firmwareRev, sizeof(device_info.firmwareRev), resp->m_ProductRevisionLevel, sizeof(resp->m_ProductRevisionLevel));
  safecopy(device_info.modelNum, sizeof(device_info.modelNum), resp->m_ProductId, sizeof(resp->m_ProductId));

  // device is apparently a SCSI disk
  return new dictionary
    {
      {"Device Type"       , "SCSI"                       },
      {"Vendor ID"         , (const char *)device_info.vendorID    },
      {"Model Number"      , (const char *)device_info.modelNum    },
      {"Firmware Revision" , (const char *)device_info.firmwareRev },
      {"Serial Number"     , (const char *)device_info.serialNum   },
    };
}





/** Send an ioctl to the device using pass through. */
uint8_t DtaScsiDrive::sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                                 void * buffer, unsigned int bufferlen)
{
  LOG(D4) << "Entering DtaDevScsi::sendCmd";


  int dxfer_direction;

  // initialize SCSI CDB and dxfer_direction
  uint8_t cdb[12];
  memset(&cdb, 0, sizeof (cdb));
  switch(cmd)
    {
    case TRUSTED_RECEIVE:
      {
        dxfer_direction = PSC_FROM_DEV;
        CScsiCmdSecurityProtocolIn & p = * (CScsiCmdSecurityProtocolIn *) cdb;
        p.m_Opcode = CScsiCmdSecurityProtocolIn::OPCODE;
        p.m_SecurityProtocol = protocol;
        p.m_SecurityProtocolSpecific = htons(comID);
        p.m_INC_512 = 1;
        p.m_AllocationLength = htonl(bufferlen/512);
        break;
      }
    case TRUSTED_SEND:
      {
        dxfer_direction = PSC_TO_DEV;
        CScsiCmdSecurityProtocolOut & p = * (CScsiCmdSecurityProtocolOut *) cdb;
        p.m_Opcode = CScsiCmdSecurityProtocolIn::OPCODE;
        p.m_SecurityProtocol = protocol;
        p.m_SecurityProtocolSpecific = htons(comID);
        p.m_INC_512 = 1;
        p.m_TransferLength = htonl(bufferlen/512);
        break;
      }
    default:
      {
        LOG(D4) << "Unknown cmd=0x" << std::hex << cmd
                << " -- returning 0xff from DtaDevScsi::sendCmd";
        return 0xff;
      }
    }


  // execute I/O
  unsigned int transferlen = bufferlen;

  unsigned char sense[32]; // how big should this be??
  unsigned char senselen=sizeof(sense);
  memset(&sense, 0, senselen);

  SCSI_STATUS_CODE masked_status=GOOD;

  int result=OS.PerformSCSICommand(osDeviceHandle,
                                   dxfer_direction,
                                   cdb, sizeof(cdb),
                                   buffer, transferlen,
                                   sense, senselen,
                                   &masked_status);
  if (result < 0) {
    LOG(D4) << "cdb after ";
    IFLOG(D4) DtaHexDump(cdb, sizeof (cdb));
    LOG(D4) << "sense after ";
    IFLOG(D4) DtaHexDump(sense, senselen);
    LOG(D4) << "Error result=" << result << " from PerformSCSICommand "
            << " -- returning 0xff from DtaDevScsi::sendCmd";
    return 0xff;
  }

  // check for successful target completion
  if (masked_status != GOOD)
    {
      LOG(D4) << "cdb after ";
      IFLOG(D4) DtaHexDump(cdb, sizeof (cdb));
      LOG(D4) << "sense after ";
      IFLOG(D4) DtaHexDump(sense, senselen);
      LOG(D4) << "Masked_status=" << HEXON(2) << masked_status << HEXOFF << "=" << statusName(masked_status) << "!=GOOD "
              << "-- returning 0xff from DtaDevScsi::sendCmd";
      return 0xff;
    }

  // success
  LOG(D4) << "Returning 0x00 from DtaDevScsi::sendCmd";
  return 0x00;
}

bool DtaScsiDrive::identify(DTA_DEVICE_INFO& disk_info)
{
  InterfaceDeviceID interfaceDeviceIdentification;
  return identifyUsingSCSIInquiry(osDeviceHandle, interfaceDeviceIdentification, disk_info);
}
