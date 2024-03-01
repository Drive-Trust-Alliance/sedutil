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
#include "DtaDevLinuxSata.h"
#include "DtaHexDump.h"
#include "ParseATIdentify.h"

//
// taken from <scsi/scsi.h> to avoid SCSI/ATA name collision
//


bool DtaDevLinuxSata::identifyUsingATAIdentifyDevice(int fd,
                                                     InterfaceDeviceID & interfaceDeviceIdentification,
                                                     DTA_DEVICE_INFO & disk_info,
                                                     dictionary ** ppIdentifyCharacteristics) {

  // Test whether device is a SAT drive by attempting
  // SCSI passthrough of ATA Identify Device command
  // If it works, as a side effect, parse the Identify response

  bool isSAT = false;
  void * identifyDeviceResponse = alloc_aligned_buffer();
  if ( identifyDeviceResponse == NULL ) {
    LOG(E) << " *** memory buffer allocation failed *** !!!";
    return false;
  }
#define IDENTIFY_RESPONSE_SIZE 512
  bzero ( identifyDeviceResponse, IDENTIFY_RESPONSE_SIZE );

  unsigned int dataLen = IDENTIFY_RESPONSE_SIZE;
  LOG(D4) << "Invoking identifyDevice_SAT --  dataLen=" << std::hex << "0x" << dataLen ;
  isSAT = (0 == identifyDevice_SAT( fd, identifyDeviceResponse, dataLen ));

  if (isSAT) {
    LOG(D4) << " identifyDevice_SAT returned zero -- is SAT" ;

    if (0xA5==((uint8_t *)identifyDeviceResponse)[510]) {  // checksum is present
      uint8_t checksum=0;
      for (uint8_t * p = ((uint8_t *)identifyDeviceResponse),
             * end = ((uint8_t *)identifyDeviceResponse) + 512;
           p<end ;
           p++)
        checksum=(uint8_t)(checksum+(*p));
      if (checksum != 0) {
        LOG(D1) << " *** IDENTIFY DEVICE response checksum failed *** !!!" ;
      }
    } else {
      LOG(D4) << " *** IDENTIFY DEVICE response checksum not present" ;
    }
    IFLOG(D4) {
      LOG(D4) << "ATA IDENTIFY DEVICE response: dataLen=" << std::hex << "0x" << dataLen ;
      DtaHexDump(identifyDeviceResponse, dataLen);
    }

    dictionary *pIdentifyCharacteristics =
      parseATAIdentifyDeviceResponse(interfaceDeviceIdentification,
                                     ((uint8_t *)identifyDeviceResponse),
                                     disk_info);
    if (ppIdentifyCharacteristics != NULL)
      (*ppIdentifyCharacteristics) = pIdentifyCharacteristics;
    else if (pIdentifyCharacteristics !=NULL)
      delete pIdentifyCharacteristics;
  } else {
    LOG(D4) << " identifyDevice_SAT returned non-zero -- is not SAT" ;
  }
  free(identifyDeviceResponse);

  return isSAT;
}



int DtaDevLinuxSata::identifyDevice_SAT( int fd, void * buffer , unsigned int & dataLength)
{

  // LOG(D4) << " identifyDevice_SAT about to PerformATAPassThroughCommand" ;
  int result=PerformATAPassThroughCommand(fd,
                                          IDENTIFY, 0, 0,
                                          buffer, dataLength);
  IFLOG(D4) {
    LOG(D4) << "identifyDevice_SAT: result=" << result << " dataLength=" << std::hex << "0x" << dataLength ;
    if (0==result)
      DtaHexDump(buffer, dataLength);
  }
  return result;
}


int DtaDevLinuxSata::PerformATAPassThroughCommand(int fd,
                                                  int cmd, int securityProtocol, int comID,
                                                  void * buffer,  unsigned int & bufferlen)
{
  uint8_t protocol;
  int dxfer_direction;
  unsigned int timeout;

  switch (cmd)
    {
    case IDENTIFY:
      timeout=600;  //  IDENTIFY sg.timeout = 600; // Sabrent USB-SATA adapter 1ms,6ms,20ms,60 NG, 600ms OK
      protocol = PIO_DATA_IN;
      dxfer_direction = PSC_FROM_DEV;
      break;

    case IF_RECV:
      timeout=60000;
      protocol = PIO_DATA_IN;
      dxfer_direction = PSC_FROM_DEV;
      break;

    case IF_SEND:
      timeout=60000;
      protocol = PIO_DATA_OUT;
      dxfer_direction = PSC_TO_DEV;
      break;

    default:
      LOG(E) << "Exiting DtaDevLinuxSata::PerformATAPassThroughCommand because of unrecognized cmd=" << cmd << "?!" ;
      return 0xff;
    }


  CScsiCmdATAPassThrough_12 cdb;
  uint8_t * cdbBytes=(uint8_t *)&cdb;  // We use direct byte pointer because bitfields are unreliable
  cdbBytes[1] = protocol << 1;
  cdbBytes[2] = (protocol==PIO_DATA_IN ? 1 : 0) << 3 |  // TDir
                1                               << 2 |  // ByteBlock
                2                                       // TLength  10b => transfer length in Count
                ;
  cdb.m_Features = securityProtocol;
  cdb.m_Count = bufferlen/512;
  cdb.m_LBA_Mid = comID & 0xFF;          // ATA lbaMid   / TRUSTED COMID low
  cdb.m_LBA_High = (comID >> 8) & 0xFF; // ATA lbaHihg  / TRUSTED COMID high
  cdb.m_Command = cmd;

  unsigned char sense[32];
  unsigned char senselen=sizeof(sense);
  bzero(&sense, senselen);

  unsigned int dataLength = bufferlen;
  unsigned char masked_status=GOOD;

  int result=DtaDevLinuxScsi::PerformSCSICommand(fd,
                                                 dxfer_direction,
                                                 cdbBytes, (unsigned char)sizeof(cdb),
                                                 buffer, dataLength,
                                                 sense, senselen,
                                                 &masked_status,
                                                 timeout);
  if (result < 0) {
    LOG(D4) << "PerformSCSICommand returned " << result;
    LOG(D4) << "sense after ";
    IFLOG(D4) DtaHexDump(&sense, senselen);
    return 0xff;
  }

  LOG(D4) << "PerformSCSICommand returned " << result;
  LOG(D4) << "sense after ";
  IFLOG(D4) DtaHexDump(&sense, senselen);

  // check for successful target completion
  if (masked_status != GOOD)
    {
      LOG(D4) << "masked_status=" << masked_status << "=" << statusName(masked_status) << " != GOOD  cmd=" <<
        (cmd == IF_SEND ? std::string("IF_SEND") :
         cmd == IF_RECV ? std::string("IF_RECV") :
         cmd == IDENTIFY ? std::string("IDENTIFY") :
         std::to_string(cmd));
      LOG(D4) << "sense after ";
      IFLOG(D4) DtaHexDump(&sense, senselen);
      return 0xff;
    }

  if (! ((0x00 == sense[0]) && (0x00 == sense[1])) ||
      ((0x72 == sense[0]) && (0x0b == sense[1])) ) {
    LOG(D4) << "PerformATAPassThroughCommand disqualifying ATA response --"
            << " sense[0]=0x" << std::hex << sense[0]
            << " sense[1]=0x" << std::hex << sense[1];
    return 0xff; // not ATA response
  }

  LOG(D4) << "buffer after ";
  IFLOG(D4) DtaHexDump(buffer, dataLength);
  LOG(D4) << "PerformATAPassThroughCommand returning sense[11]=0x" << std::hex << (unsigned int)sense[11];
  return (sense[11]);

}




dictionary *
DtaDevLinuxSata::parseATAIdentifyDeviceResponse(const InterfaceDeviceID & interfaceDeviceIdentification,
                                                const unsigned char * response,
                                                DTA_DEVICE_INFO & di)
{
  if (NULL == response)
    return NULL;

  const IDENTIFY_RESPONSE & resp = *(IDENTIFY_RESPONSE *)response;

  parseATIdentifyResponse(&resp, &di);

  if (deviceNeedsSpecialAction(interfaceDeviceIdentification,
                               splitVendorNameFromModelNumber)) {
    LOG(D4) << " *** splitting VendorName from ModelNumber";
    LOG(D4) << " *** was vendorID=\"" << di.vendorID << "\" modelNum=\"" <<  di.modelNum << "\"";
    memcpy(di.vendorID, di.modelNum, sizeof(di.vendorID));
    memmove(di.modelNum,
            di.modelNum+sizeof(di.vendorID),
            sizeof(di.modelNum)-sizeof(di.vendorID));
    memset(di.modelNum+sizeof(di.modelNum)-sizeof(di.vendorID),
           0,
           sizeof(di.vendorID));
    LOG(D4) << " *** now vendorID=\"" << di.vendorID << "\" modelNum=\"" <<  di.modelNum << "\"";
  }


  std::ostringstream ss1;
  ss1 << "0x" << std::hex << std::setw(4) << std::setfill('0');
  ss1 << (int)(resp.TCGOptions[1]<<8 | resp.TCGOptions[0]);;
  std::string options = ss1.str();

  std::ostringstream ss;
  ss << std::hex << std::setw(2) << std::setfill('0');
  for (uint8_t &b: di.worldWideName) ss << (int)b;
  std::string wwn = ss.str();
  return new dictionary
    {
      {"TCG Options"       , options                        },
      {"Device Type"       , resp.devType ? "OTHER" : "ATA" },
      {"Serial Number"     , (const char *)di.serialNum     },
      {"Model Number"      , (const char *)di.modelNum      },
      {"Firmware Revision" , (const char *)di.firmwareRev   },
      {"World Wide Name"   , wwn                            },
    };
}



/** Send an ioctl to the device using pass through. */
uint8_t DtaDevLinuxSata::sendCmd(ATACOMMAND cmd, uint8_t securityProtocol, uint16_t comID,
                                 void * buffer, uint32_t bufferlen)
{
  LOG(D1) << "Entering DtaDevLinuxSata::sendCmd";

  IFLOG(D4) {
    LOG(D4) << "sendCmd: before";
    if (cmd == IF_SEND) {
      LOG(D4) << "bufferlen = 0x" << std::hex << bufferlen ;
      DtaHexDump(buffer, bufferlen);
    }
  }

  /*
   * Do the IO
   */
  int result= PerformATAPassThroughCommand(fd, cmd, securityProtocol, comID,
                                           buffer, bufferlen);

  LOG(D4) << "sendCmd: after -- result = " << result ;
  IFLOG(D4) {
    if (0==result && cmd != IF_SEND) {
      LOG(D4) << "bufferlen = 0x" << std::hex << bufferlen ;
      DtaHexDump(buffer, bufferlen);
    }
  }

  return result;
}


bool  DtaDevLinuxSata::identify(DTA_DEVICE_INFO& disk_info)
{
  InterfaceDeviceID interfaceDeviceIdentification;
  return identifyUsingATAIdentifyDevice(fd, interfaceDeviceIdentification, disk_info, NULL);
}
