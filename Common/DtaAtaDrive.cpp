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
#include "DtaAtaDrive.h"
#include "DtaHexDump.h"
#include "ParseATIdentify.h"
#include <map>

DtaDrive*
DtaAtaDrive::getDtaAtaDrive(const char* devref, DTA_DEVICE_INFO& di) {

  bool accessDenied=false;
  OSDEVICEHANDLE osDeviceHandle = OS.openAndCheckDeviceHandle(devref, accessDenied);
  if (osDeviceHandle==INVALID_HANDLE_VALUE || accessDenied) {
    if (osDeviceHandle!=INVALID_HANDLE_VALUE)  OS.closeDeviceHandle(osDeviceHandle);
    return NULL;
  }

  LOG(D4) << "Success opening device " << devref << " as file handle " << HEXON(4) << (size_t)osDeviceHandle;


  InterfaceDeviceID interfaceDeviceIdentification;

  // In theory every USB device should respond to Scsi commands, in particularly Inquiry.  Is this true?
  dictionary* identifyCharacteristics = NULL;
    bool scsiTranslated=true;
  if (DtaAtaDrive::identifyUsingATAIdentifyDevice(osDeviceHandle,
      interfaceDeviceIdentification,
      di,
      &identifyCharacteristics,
      scsiTranslated)) {

    // The completed identifyCharacteristics map could be useful to customizing code here
    if (NULL != identifyCharacteristics) {
      LOG(D3) << "identifyCharacteristics for ATA Device: ";
      for (const auto& it : *identifyCharacteristics)
        LOG(D3) << "  " << it.first << ":" << it.second;
      delete identifyCharacteristics;
      identifyCharacteristics = NULL;
    }
    LOG(D4) << " Device " << devref << " is Ata, scsiTranslated=" << std::boolalpha << scsiTranslated;
    return getDtaOSSpecificAtaDrive(osDeviceHandle, scsiTranslated);
  } else {
    OS.closeDeviceHandle(osDeviceHandle);
    return NULL;
  }
}



bool DtaAtaDrive::tryToIdentifyUsingATAIdentifyDevice_WithScsiTranslated(bool &scsiTranslated,
                                                                                OSDEVICEHANDLE osDeviceHandle,
                                                                                InterfaceDeviceID &interfaceDeviceIdentification,
                                                                                DTA_DEVICE_INFO &disk_info,
                                                                                dictionary **ppIdentifyCharacteristics,
                                                                                void *identifyDeviceResponse,
                                                                                unsigned int &dataLen)
{
    LOG(D4) << "DtaAtaDrive::identifyUsingATAIdentifyDevice: invoking __identifyDevice --"
            << " "
            << "scsiTranslated=" << std::boolalpha << scsiTranslated
            << " "
            << "dataLen=" << HEXON(4) << dataLen ;
    bool isATA = (0 == __identifyDevice(osDeviceHandle, scsiTranslated, identifyDeviceResponse, dataLen ));

    if (isATA) {
        LOG(D4) << "DtaAtaDrive::identifyUsingATAIdentifyDevice: __identifyDevice returned zero -- is ATA" ;

        if (0xA5==((uint8_t *)identifyDeviceResponse)[510]) {  // checksum is present
            LOG(D4) << "DtaAtaDrive::identifyUsingATAIdentifyDevice:checksum flag seen ... computing checksum ..." ;
            uint8_t checksum=0;
            for (uint8_t * p = ((uint8_t *)identifyDeviceResponse),
                 * end = ((uint8_t *)identifyDeviceResponse) + 512;
                 p<end ;
                 p++)
                checksum=(uint8_t)(checksum+(*p));
            if (checksum != 0) {
                LOG(D1) << " *** IDENTIFY DEVICE response checksum failed *** !!!" ;
            } else {
                LOG(D4) << "DtaAtaDrive::identifyUsingATAIdentifyDevice:... checksum passed." ;
            }
        } else {
            LOG(D4) << "DtaAtaDrive::identifyUsingATAIdentifyDevice:checksum flag not presend" ;
        }
        IFLOG(D4) {
            LOG(D4) << "ATA IDENTIFY DEVICE response: dataLen=" << HEXON(4) << dataLen ;
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
        LOG(D4) << " __identifyDevice returned non-zero -- is not ATA with scsiTranslated=" << std::boolalpha << scsiTranslated ;
    }
    return isATA;
}

bool DtaAtaDrive::identifyUsingATAIdentifyDevice(OSDEVICEHANDLE osDeviceHandle,
                                                  InterfaceDeviceID & interfaceDeviceIdentification,
                                                  DTA_DEVICE_INFO & disk_info,
                                                  dictionary ** ppIdentifyCharacteristics,
                                                  bool & scsiTranslated) {

  // Test whether device is a SAT drive by attempting
  // SCSI passthrough of ATA Identify Device command
  // If it works, as a side effect, parse the Identify response

  bool isATA = false;
  void * identifyDeviceResponse = OS.alloc_aligned_MIN_BUFFER_LENGTH_buffer();
  if ( identifyDeviceResponse == NULL ) {
    LOG(E) << " *** memory buffer allocation failed *** !!!";
    return false;
  }

  unsigned int dataLen = ATA_IDENTIFY_DEVICE_RESPONSE_SIZE;
  memset(identifyDeviceResponse, 0, dataLen);

    scsiTranslated=false;
    isATA=tryToIdentifyUsingATAIdentifyDevice_WithScsiTranslated(scsiTranslated, osDeviceHandle, interfaceDeviceIdentification, disk_info, ppIdentifyCharacteristics, identifyDeviceResponse, dataLen);
    if (!isATA) {
        scsiTranslated=true;
        isATA=tryToIdentifyUsingATAIdentifyDevice_WithScsiTranslated(scsiTranslated, osDeviceHandle, interfaceDeviceIdentification, disk_info, ppIdentifyCharacteristics, identifyDeviceResponse, dataLen);
    }


  OS.free_aligned_MIN_BUFFER_LENGTH_buffer(identifyDeviceResponse);

  return isATA;
}



int DtaAtaDrive::__identifyDevice( OSDEVICEHANDLE osDeviceHandle, bool scsiTranslated, void * buffer , unsigned int & dataLength)
{

  // LOG(D4) << " __identifyDevice about to OS.PerformATACommand" ;
  int result=OS.PerformATACommand(osDeviceHandle, scsiTranslated, IDENTIFY_DEVICE, 0, 0, buffer, dataLength);
  IFLOG(D4) {
    LOG(D4) << "__identifyDevice: result=" << result << " dataLength=" << HEXON(8) << dataLength ;
    if (0==result)
      DtaHexDump(buffer, dataLength);
  }
  return result;
}


dictionary *
DtaAtaDrive::parseATAIdentifyDeviceResponse(const InterfaceDeviceID & interfaceDeviceIdentification,
                                            const unsigned char * response,
                                            DTA_DEVICE_INFO & device_info)
{
  if (NULL == response)
    return NULL;

  const ATA_IDENTIFY_DEVICE_RESPONSE & resp = *(ATA_IDENTIFY_DEVICE_RESPONSE *)response;

  parseATIdentifyResponse(&resp, &device_info);

  if (deviceNeedsSpecialAction(interfaceDeviceIdentification,
                               splitVendorNameFromModelNumber)) {
    LOG(D4) << " *** splitting VendorName from ModelNumber";
    LOG(D4) << " *** was vendorID=\"" << device_info.vendorID << "\" modelNum=\"" <<  device_info.modelNum << "\"";
    memcpy(device_info.vendorID, device_info.modelNum, sizeof(device_info.vendorID));
    memmove(device_info.modelNum,
            device_info.modelNum+sizeof(device_info.vendorID),
            sizeof(device_info.modelNum)-sizeof(device_info.vendorID));
    memset(device_info.modelNum+sizeof(device_info.modelNum)-sizeof(device_info.vendorID),
           0,
           sizeof(device_info.vendorID));
    LOG(D4) << " *** now vendorID=\"" << device_info.vendorID << "\" modelNum=\"" <<  device_info.modelNum << "\"";
  }


  std::ostringstream ss1;
  ss1 << HEXON(4) << (int)(resp.TCGOptions[1]<<8 | resp.TCGOptions[0]);
  std::string options = ss1.str();

  std::ostringstream ss;
  ss << HEXON(2); ss.seekp(0);
  for (uint8_t &b: device_info.worldWideName) ss << (int)b;
  std::string wwn = ss.str();
  return new dictionary
    {
      {"TCG Options"       , options                        },
      {"Device Type"       , resp.devType ? "OTHER" : "ATA" },
      {"Serial Number"     , (const char *)device_info.serialNum     },
      {"Model Number"      , (const char *)device_info.modelNum      },
      {"Firmware Revision" , (const char *)device_info.firmwareRev   },
      {"World Wide Name"   , wwn                            },
    };
}



/** Send an ioctl to the device using pass through. */
uint8_t DtaAtaDrive::sendCmd(ATACOMMAND cmd, uint8_t securityProtocol, uint16_t comID,
                              void * buffer, uint32_t bufferlen)
{
  LOG(D4) << "Entering DtaDevAta::sendCmd";

  IFLOG(D4) {
    LOG(D4) << "sendCmd: before";
    if (cmd == TRUSTED_SEND) {
      LOG(D4) << "bufferlen = 0x" << std::hex << bufferlen ;
      DtaHexDump(buffer, bufferlen);
    }
  }

  /*
   * Do the IO
   */
  int result= PerformATACommand(cmd, securityProtocol, comID, buffer, bufferlen);

  LOG(D4) << "sendCmd: after -- result = " << result ;
  IFLOG(D4) {
    if (0==result && cmd != TRUSTED_SEND) {
      LOG(D4) << "bufferlen = 0x" << std::hex << bufferlen ;
      DtaHexDump(buffer, bufferlen);
    }
  }

  return static_cast<uint8_t>(result);
}


bool  DtaAtaDrive::identify(DTA_DEVICE_INFO& disk_info)
{
  InterfaceDeviceID interfaceDeviceIdentification;
  return identifyUsingATAIdentifyDevice(osDeviceHandle, interfaceDeviceIdentification, disk_info, NULL, scsiTranslated);
}
