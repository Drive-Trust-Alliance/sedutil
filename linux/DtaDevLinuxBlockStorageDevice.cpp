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

#include <string>
#include <regex>

#include "oui_vendor.hpp"
#include "stdio.h"

#include <SEDKernelInterface/SEDKernelInterface.h>
#include "DtaHashPassword.h"
#include "DtaDevLinuxTPer.h"
#include "DtaDevOS.h"


#define ERRORS_TO_STDERR
//#undef ERRORS_TO_STDERR



const DTA_DEVICE_INFO & DtaDevLinuxBlockStorageDevice::device_info() {
  assert(NULL != pdevice_info);
  return *pdevice_info;
}




// Sorting order
bool DtaDevLinuxBlockStorageDevice::deviceNameLessThan(DtaDevLinuxBlockStorageDevice * a,
                                                       DtaDevLinuxBlockStorageDevice * b)
  const string & aName = a->deviceName;
  const string & bName = b->deviceName;
  const auto aNameLength = aName.length();
  const auto bNameLength = bName.length();
  if (aNameLength < bNameLength ) {
    return true;
  }
  if (bNameLength < aNameLength ) {
    return false;
  }
  return aName < bName ;
}


std::vector<DtaDevLinuxBlockStorageDevice *> DtaDevLinuxBlockStorageDevice::enumerateBlockStorageDevices() {
  std::vector<DtaDevLinuxBlockStorageDevice *>devices;

  const CFIndex kCStringSize = 128;
  char nameBuffer[kCStringSize];
  bzero(nameBuffer,kCStringSize);

  DTA_DEVICE_INFO di;

  string deviceNameStr;

  // Iterate over nodes of class IOBlockStorageDevice or subclass thereof
  DIR *dir = opendir("/dev");
  if (dir!=NULL)
    {
      struct dirent *dirent;
      while((dirent=readdir(dir))!=NULL) {
        const char * devref = const_cast<const char *>(dirent->d_name);
        if((0==fnmatch("sd[a-z]",devref,0)) ||
           (0==fnmatch("nvme[0-9]",devref,0)) ||
           (0==fnmatch("nvme[0-9][0-9]",devref,0))
           ) {
          bzero(di, sizeof(DTA_DEVICE_INFO));
          DtaDevLinuxBlockStorageDevice * blockStorageDevice =
            DtaDevLinuxBlockStorageDevice::getBlockStorageDevice(devref, &di);
          assert(blockStorageDevice!=NULL);
          devices.push_back(blockStorageDevice);
        }
      }
      closedir(dir);
    }

  sort(devices.begin(), devices.end(), DtaDevLinuxBlockStorageDevice::deviceNameLessThan);
  return devices;
}



static void trimBuffer(char * buffer, size_t len) {
  char * b = buffer;
  char * e = b + len;
  // Trim off trailing whitespace (and nulls)
  char * ne = e;
  while (b < ne && (ne[-1] == 0 || isspace(ne[-1])))
    *--ne = 0;
  // Slide left over leading whitespace
  char * nb = b;
  while (nb < ne && isspace(*nb))
    nb++;
  if (nb == ne) {
    bzero(buffer, len) ; // No contents
  } else if (b != nb) {
    memmove(b, nb, (size_t)(ne - nb));
    memset(b + (size_t)(ne - nb), 0,  len - (size_t)(ne - nb));
  }

}


void DtaDevLinuxBlockStorageDevice::polishDeviceInfo(DTA_DEVICE_INFO & device_info) {

#define is_not_all_NULs(field) (__is_not_all_NULs(device_info.field, sizeof(device_info.field)))
#define is_not_all_zeroes(field) (__is_not_all_zeroes(device_info.field, sizeof(device_info.field)))

#define _trim(field) trimBuffer((char *)device_info.field, sizeof(device_info.field))

  _trim(modelNum);
  _trim(vendorID);
  _trim(serialNum);
  _trim(firmwareRev);
  _trim(manufacturerName);



#define VENDOR_ID_MIGHT_BE_FIRST_WORD_OF_MODEL_NUMBER
#if defined( VENDOR_ID_MIGHT_BE_FIRST_WORD_OF_MODEL_NUMBER )
  if (!is_not_all_NULs(vendorID)) {
    std::regex leftmostWord("([^ ]*) (.*)");
    std::cmatch match;
    if (std::regex_match((const char *)device_info.modelNum,
                         match,
                         leftmostWord)) {
      string vendorName = match.str(1);
      const char * candidateVendorID = vendorName.c_str();
      if (candidateVendorID != NULL && 0 < strlen(candidateVendorID)) {
        // Is the first word of modelNum also a vendorID (canonically, if necessary)?
        const char * vendor = vendor_for_vendorID_canonically_if_necessary(candidateVendorID);
        if (vendor != NULL) {  // OK then
          size_t vendorID_length_plus_one=strnlen(candidateVendorID, sizeof(device_info.vendorID))+1;
          strncpy((char *)device_info.vendorID, candidateVendorID, vendorID_length_plus_one);

          memset(device_info.modelNum, ' ', vendorID_length_plus_one);
          _trim(modelNum);
        }
      }
    }
  }
#endif // defined( VENDOR_ID_MIGHT_BE_FIRST_WORD_OF_MODEL_NUMBER )

#undef _trim

#define EXTEND_DTA_DEVICE_INFO_WITH_OUI_VENDOR_DATA
#if defined( EXTEND_DTA_DEVICE_INFO_WITH_OUI_VENDOR_DATA )
  if (is_not_all_NULs(worldWideName)) {
    device_info.worldWideNameIsSynthetic = 0;
    if (!is_not_all_NULs(manufacturerName)) {
      char oui[8]={0};
      snprintf(oui, 8, "%02X%02X%02X%02X",
               device_info.worldWideName[0],
               device_info.worldWideName[1],
               device_info.worldWideName[2],
               device_info.worldWideName[3]);
      assert(oui[0]=='5');
      const char * manufacturer = manufacturer_for_oui((const char *)&oui[1]);
      if (manufacturer != NULL) {
        strncpy((char *)device_info.manufacturerName,
                manufacturer,
                sizeof(device_info.manufacturerName));
      }
    }
    if (!is_not_all_NULs(vendorID)) {
      const char * vendorID =
        vendorID_for_vendor_canonically_if_necessary((const char *)device_info.manufacturerName);
      if (vendorID != NULL) {
        strncpy((char *)device_info.vendorID, vendorID, sizeof(device_info.vendorID));
      }
    }
  } else {
    if (is_not_all_NULs(vendorID)) {
      if (!is_not_all_NULs(manufacturerName)) {
        const char * vendor =
          vendor_for_vendorID_canonically_if_necessary((const char *)device_info.vendorID);
        if (vendor != NULL) {
          strncpy((char *)device_info.manufacturerName, vendor, sizeof(device_info.manufacturerName));
        }
        const char * oui =
          oui_for_vendorID_canonically_if_necessary((const char *)device_info.vendorID);
        if (oui == NULL) {
          oui = private_oui ; // "Private"
        }

        char worldWideNameHex[17];
        snprintf(worldWideNameHex, 17, "5%6s0%08X", oui, 0);

        char * serialNumberHex = worldWideNameHex+8;
        if (is_not_all_zeroes(serialNum)) {
          // Is there a stretch of at least 8 hex digits in serialNum?
          std::regex rightmostEightHexDigits(".*([A-Fa-f0-9]{8})");
          std::cmatch match;
          if (std::regex_match((const char *)device_info.serialNum,
                               match,
                               rightmostEightHexDigits)) {
            strncpy(serialNumberHex, (const char *)(match.str(1).c_str()), 8);
          } else {
            // Synthesize WWN "serial number" suffix as DtaHash of vendorID, modelNum, and passwordSalt
            uint8_t uID[sizeof(device_info.vendorID)+ sizeof(device_info.modelNum)+1];
            memcpy(&uID[0],
                   device_info.vendorID, sizeof(device_info.vendorID));
            memcpy(&uID[sizeof(device_info.vendorID)],
                   device_info.modelNum, sizeof(device_info.modelNum));
            vector<uint8_t> hash={4,0xd0, 0,0,0,0};
            vector<uint8_t> salt(device_info.passwordSalt,
                                 device_info.passwordSalt+sizeof(device_info.passwordSalt));
            DtaHashPassword(hash, (char *)uID, salt);
            snprintf(serialNumberHex, 9, "%02X%02X%02X%02X", hash[2] , hash[3] , hash[4] , hash[5]);
          }
        }


        for (size_t i=0; i<sizeof(device_info.worldWideName); i++) {
          char nybbles[3];
          nybbles[0]=worldWideNameHex[2*i  ];
          nybbles[1]=worldWideNameHex[2*i+1];
          nybbles[2]=0;
          device_info.worldWideName[i] = (uint8_t)strtol(nybbles, NULL, 16);
          device_info.worldWideNameIsSynthetic = 1;
        }

      }
    }
  }
#endif // defined( EXTEND_DTA_DEVICE_INFO_WITH_OUI_VENDOR_DATA )

  return;
}


// Factory for this class or subclass instances

DtaDevLinuxBlockStorageDevice *
DtaDevLinuxBlockStorageDevice::getBlockStorageDevice(const char * devref, DTA_DEVICE_INFO * pdi)
{
  DtaDevLinuxBlockStorageDevice * device = NULL;
  if ( 0==fnmatch("sd[a-z]",devref,0)
     ) {
    device = getDtaDevLinuxSerialAttachedDevice(devref, pdi);
  }

  if ( (0==fnmatch("nvme[0-9]",devref,0)) ||
       (0==fnmatch("nvme[0-9][0-9]",devref,0))
     ) {
    device = getDtaDevLinuxNvme(devref, pdi);
  }

  if (device == NULL) {
    device = new DtaDevLinuxBlockStorageDevice(devref, pdi);
  }

  return device;

}



DtaDevLinuxBlockStorageDevice::~DtaDevLinuxBlockStorageDevice () {
  if (properties != NULL )
    CFRelease(properties);
}

uint8_t DtaDevLinuxBlockStorageDevice::isAnySSC()
{
  return pdevice_info->ANY_OPAL_SSC;
}

const char * DtaDevLinuxBlockStorageDevice::getVendorID()
{
  return (const char *)&pdevice_info->vendorID;
}

const char * DtaDevLinuxBlockStorageDevice::getManufacturerName()
{
  return (const char *)&pdevice_info->manufacturerName;
}

const char * DtaDevLinuxBlockStorageDevice::getFirmwareRev()
{
  return (const char *)&pdevice_info->firmwareRev;
}

const char * DtaDevLinuxBlockStorageDevice::getModelNum()
{
  return (const char *)&pdevice_info->modelNum;
}

const char * DtaDevLinuxBlockStorageDevice::getSerialNum()
{
  return (const char *)&pdevice_info->serialNum;
}


const vector<uint8_t> DtaDevLinuxBlockStorageDevice::getPasswordSalt()
{
  const uint8_t * b=pdevice_info->passwordSalt;
  return vector<uint8_t>(b,b+sizeof(pdevice_info->passwordSalt));
}


const vector<uint8_t> DtaDevLinuxBlockStorageDevice::getWorldWideName()
{
  const uint8_t * b=pdevice_info->worldWideName;
  return vector<uint8_t>(b,b+sizeof(pdevice_info->worldWideName));
}

uint8_t DtaDevLinuxBlockStorageDevice::getWorldWideNameIsSynthetic()
{
  return pdevice_info->worldWideNameIsSynthetic;
}

const char * DtaDevLinuxBlockStorageDevice::getPhysicalInterconnect()
{
  return (const char *)&pdevice_info->physicalInterconnect;
}
const char * DtaDevLinuxBlockStorageDevice::getPhysicalInterconnectLocation()
{
  return (const char *)&pdevice_info->physicalInterconnectLocation;
}
const char * DtaDevLinuxBlockStorageDevice::getBSDName()
{
  return (const char *)deviceName.c_str();
}

DTA_DEVICE_TYPE DtaDevLinuxBlockStorageDevice::getDevType()
{
  return pdevice_info->devType;
}

const std::string DtaDevLinuxBlockStorageDevice::getDevPath () {
  return ("/dev/"+deviceName).substr(0,25);
}

const unsigned long long DtaDevLinuxBlockStorageDevice::getSize() {
  return pdevice_info->devSize;
}
