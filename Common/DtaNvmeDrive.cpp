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

#include "DtaNvmeDrive.h"
#include "DtaHexDump.h"
#include "NVMeStructures.h"

DtaDrive *
DtaNvmeDrive::getDtaNvmeDrive(const char * devref, DTA_DEVICE_INFO & device_info) {

  LOG(D4) << "DtaNvmeDrive::getDtaNvmeDrive(\"" << devref << "\", _)";

  bool accessDenied=false;
  OSDEVICEHANDLE osDeviceHandle = OS.openAndCheckDeviceHandle(devref, accessDenied);
  LOG(D4) << "DtaNvmeDrive::getDtaNvmeDrive(\"" << devref << "\", _)"
          << " => " << osDeviceHandle
          << ", accessDenied=" << std::boolalpha << accessDenied ;
  if (osDeviceHandle==INVALID_HANDLE_VALUE || accessDenied) {
    if (osDeviceHandle!=INVALID_HANDLE_VALUE)  OS.closeDeviceHandle(osDeviceHandle);
   return NULL;
  }

  LOG(D4) << "DtaNvmeDrive::getDtaNvmeDrive: Success opening device " << devref
          << " as file handle " << HEXON(4) << (size_t) osDeviceHandle;


  InterfaceDeviceID interfaceDeviceIdentification;

  if (! identifyUsingNvmeIdentify(osDeviceHandle, interfaceDeviceIdentification, device_info)) {
    device_info.devType = DEVICE_TYPE_OTHER;
    // LOG(E) << "DtaNvmeDrive::getDtaNvmeDrive: Device " << devref << " is NOT Nvme?! -- file handle " << HEXON(8) << (size_t) osDeviceHandle;
    OS.closeDeviceHandle(osDeviceHandle);
    return NULL;
  }

  return getDtaOSSpecificNvmeDrive(osDeviceHandle);
}




uint8_t DtaNvmeDrive::sendCmd(ATACOMMAND cmd,
                              uint8_t protocol,
                              uint16_t comID,
                              void * buffer, unsigned int bufferlen)
{
  LOG(D4) << "Entering DtaNvmeDrive::sendCmd";

  struct nvme_admin_cmd nvme_cmd;
  memset(&nvme_cmd, 0, sizeof(nvme_cmd));

  if (TRUSTED_RECEIVE == cmd) {
    LOG(D4) << "Nvme Security Receive Command";
    nvme_cmd.opcode = NVME_SECURITY_RECV;
  }
  else {
    LOG(D4) << "Nvme Security Send Command";
    nvme_cmd.opcode = NVME_SECURITY_SEND;
  }

  nvme_cmd.cdw10 = static_cast<__u32>(protocol << 24 | comID << 8);
  nvme_cmd.cdw11 = bufferlen;
  nvme_cmd.data_len = bufferlen;
  nvme_cmd.addr = reinterpret_cast<__u64>(buffer);

  uint32_t result=0;
  int err = OS.PerformNVMeCommand(osDeviceHandle, reinterpret_cast<uint8_t *>(&nvme_cmd), &result);

  if (err < 0)
    return static_cast<uint8_t>(errno);
  else if (err != 0) {
    //fprintf(stderr, "NVME Security Command Error:%d\n", err);
    LOG(D4) << "NVME Security Command Error: " << err ;
    IFLOG(D4) DtaHexDump(&nvme_cmd, sizeof(nvme_cmd));
  } else {
    LOG(D4) << "NVME Security Command Success:" << result;
  }
  //LOG(D4) << "NVMe command:";
  //IFLOG(D4) DtaHexDump(&nvme_cmd, sizeof(nvme_cmd));
  //LOG(D4) << "NVMe buffer @ " << buffer;
  //IFLOG(D4) DtaHexDump(buffer, bufferlen);
  return static_cast<uint8_t>(err);
}



typedef dictionary * responseParser(const unsigned char * response,
                                    InterfaceDeviceID & interfaceDeviceIdentification,
                                    DTA_DEVICE_INFO & device_info);

static
dictionary *
parseIdentifyControllerResponse(const unsigned char * response,
                                InterfaceDeviceID & interfaceDeviceIdentification,
                                DTA_DEVICE_INFO & device_info)
{
  const uint8_t *results = reinterpret_cast<const uint8_t *>(response);
  results += 4;
  safecopy(device_info.serialNum, sizeof(device_info.serialNum), results, sizeof(device_info.serialNum));
  results += 20;
  safecopy(device_info.modelNum, sizeof(device_info.modelNum), results, sizeof(device_info.modelNum));
  results += 40;
  safecopy(device_info.firmwareRev, sizeof(device_info.firmwareRev), results, sizeof(device_info.firmwareRev));

  memcpy(interfaceDeviceIdentification, response, sizeof(InterfaceDeviceID));

  // device is apparently a NVMe disk
  return new dictionary
    {
      {"Vendor ID"         , (const char *)device_info.vendorID    },
      {"Model Number"      , (const char *)device_info.modelNum    },
      {"Firmware Revision" , (const char *)device_info.firmwareRev },
      {"Serial Number"     , (const char *)device_info.serialNum   },
    };
}

static
bool _NvmeIdentify(const char * variant_name, uint32_t namespace_id, uint8_t subcommand, responseParser *parser,
                   OSDEVICEHANDLE osDeviceHandle,
                   InterfaceDeviceID & interfaceDeviceIdentification,
                   DTA_DEVICE_INFO & device_info)
{

  uint8_t data[4096]; // per 5.1.13.1

  struct nvme_admin_cmd cmd;

  memset(&cmd, 0, sizeof(cmd));
  memset(data, 0, sizeof(data));

  LOG(D4) << "Nvme Identify Command -- " << variant_name
          << " namespace " << HEXON(8) << namespace_id
          << " subcommand "  << HEXON(2) << (uint32_t)subcommand;
  cmd.opcode   = NVME_IDENTIFY;
  cmd.nsid     = (uint32_t)namespace_id;
  cmd.addr     = (uint64_t)&data;
  cmd.data_len = (uint32_t)sizeof(data);
  cmd.cdw10    = (uint32_t)subcommand;

  uint32_t status;
  int err;
    
  err = OS.PerformNVMeCommand(osDeviceHandle, reinterpret_cast<uint8_t *>(&cmd), &status);
  if (err) {
    device_info.devType = DEVICE_TYPE_OTHER;
    LOG(D4)
      << "Nvme Identify " << variant_name << "  error --  "
      << " kernel status " << HEXON(4) << err
      << " NVMe status " << HEXON(4) << status
      << " errno " << HEXOFF << errno ;
    if (errno==EINVAL) {
      LOG(D4) << "errno==EINVAL -- NVMe status significant";
    }
    if (err==0x4002) {
      LOG(D4) << "INVALID_FIELD: A reserved coded value or an unsupported value in a defined field(0x4002)";
    }
    LOG(D4)  << std::endl <<  std::endl;
  }
  if (err)
    return false;

  IFLOG(D4) {
    LOG(D4) << "Nvme Identify " << variant_name << " success";
    DtaHexDump(&data, (unsigned int)bounded((unsigned int)0x010, cmd.data_len, (unsigned int)0x200));
  }

  dictionary * responses = (*parser)(data,interfaceDeviceIdentification,device_info);
  IFLOG(D4) {
    LOG(D4) << variant_name << " responses:" ;
    for (dictionary::iterator it = responses->begin(); it != responses->end(); it++)
      LOG(D4) << it->first    // string (key)
              << ':'
              << it->second;  // string's value
  }
  delete responses;

  return true;
}


/***
static
dictionary *
parseActiveNamespaceIDsResponse(const unsigned char * data,
                                InterfaceDeviceID & interfaceDeviceIdentification,
                                DTA_DEVICE_INFO & device_info)
{
  const uint32_t activeID= ( * ( reinterpret_cast<const uint32_t *>(data) ) );
  if ( 0 != activeID ) {
    char aID[] = "00000000000";
    sprintf(aID, "%d", activeID);
    return new dictionary { { "Active Namespace ID" , aID } };
  } else {
    return new dictionary { };
  }
}
***/

static
bool
getNamespaceID(OSDEVICEHANDLE osDeviceHandle,uint32_t & namespace_id) {
uint8_t data[4096]; // per 5.1.13.1

  struct nvme_admin_cmd cmd;
  int err;

  memset(&cmd, 0, sizeof(cmd));
  memset(data, 0, sizeof(data));

  LOG(D4) << "getNamespaceID";
  cmd.opcode   = NVME_IDENTIFY;
  cmd.nsid     = NVME_NSID_NONE;
  cmd.addr     = (uint64_t)&data;
  cmd.data_len = (uint32_t)sizeof(data);
  cmd.cdw10    = NVME_IDENTIFY_CNS_NS_ACTIVE_LIST;

  uint32_t status;
  err = OS.PerformNVMeCommand(osDeviceHandle, reinterpret_cast<uint8_t *>(&cmd), &status);

  if (err) {
    LOG(D4)
      << "getNamespaceID  error --  "
      << " kernel status " << HEXON(4) << err
      << " NVMe status " << HEXON(4) << status
      << " errno " << HEXOFF << errno ;
    if (errno==EINVAL) {
      LOG(D4) << "errno==EINVAL -- NVMe status significant";
    }
    if (err==0x4002) {
      LOG(D4) << "INVALID_FIELD: A reserved coded value or an unsupported value in a defined field(0x4002)";
    }
    LOG(D4)  << std::endl <<  std::endl;
    return false;
  }

  IFLOG(D4) {
    LOG(D4) << "getNamespaceID NVMe Identify success";
    DtaHexDump(&data, bounded((unsigned int)0x010, cmd.data_len, (unsigned int)0x200));
  }

  const uint32_t
    * responses = reinterpret_cast<const uint32_t *>(data),
    * begin = responses,
    * end = begin + sizeof(data)/sizeof(uint32_t);

  IFLOG(D4) {
    LOG(D4) << " responses:" ;
    for (const uint32_t * it = begin; it != end && 0!=*it; it++)
      LOG(D4) << *it;
  }

  if (begin<end) {
    namespace_id = *begin;
    return true;
  }

  return false;
}




static
bool
getNamespaceInfo(OSDEVICEHANDLE osDeviceHandle,
                 uint32_t namespace_id,
                 DTA_DEVICE_INFO & device_info) {
uint8_t data[4096]; // per 5.1.13.1

  struct nvme_admin_cmd cmd;
  int err;

  memset(&cmd, 0, sizeof(cmd));
  memset(data, 0, sizeof(data));

  LOG(D4) << "getNamespaceInfo";
  cmd.opcode   = NVME_IDENTIFY;
  cmd.nsid     = namespace_id;
  cmd.addr     = (uint64_t)&data;
  cmd.data_len = (uint32_t)sizeof(data);
  cmd.cdw10    = NVME_IDENTIFY_CNS_NS;

  uint32_t status;
  err = OS.PerformNVMeCommand(osDeviceHandle, reinterpret_cast<uint8_t *>(&cmd), &status);

  if (err) {
    LOG(D4)
      << "getNamespaceInfo  error --  "
      << " kernel status " << HEXON(4) << err
      << " NVMe status " << HEXON(4) << status
      << " errno " << HEXOFF << errno ;
    if (errno==EINVAL) {
      LOG(D4) << "errno==EINVAL -- NVMe status significant";
    }
    if (err==0x4002) {
      LOG(D4) << "INVALID_FIELD: A reserved coded value or an unsupported value in a defined field(0x4002)";
    }
    LOG(D4)  << std::endl <<  std::endl;
    return false;
  }

  IFLOG(D4) {
    LOG(D4) << "getNamespaceInfo NVMe Identify success";
    DtaHexDump(&data, (unsigned int)bounded((unsigned int)0x010, cmd.data_len, (unsigned int)0x200));
  }

  device_info.devSize = * (reinterpret_cast<uint64_t *>(data+48)) ;  // See Figure 114
  safecopy(device_info.worldWideName, sizeof(device_info.worldWideName),
           data+120, 8);  // See Figure 114
  return true;
}



bool DtaNvmeDrive::identifyUsingNvmeIdentify(OSDEVICEHANDLE osDeviceHandle,
                                             InterfaceDeviceID & interfaceDeviceIdentification,
                                             DTA_DEVICE_INFO & device_info)
{
  LOG(D4) << "Entering DtaNvmeDrive::identifyUsingNvmeIdentify";


  uint32_t namespace_id = 0;
  if (!getNamespaceID(osDeviceHandle, namespace_id))
    return false;

  LOG(D4) << "Active Namespace ID is " << namespace_id;

  if (! getNamespaceInfo(osDeviceHandle, namespace_id, device_info))
    return false;

  LOG(D4) << "Device size is " << device_info.devSize;
  LOG(D4) << "WWN is " << __HEXON(2) << std::uppercase
          << (uint16_t)device_info.worldWideName[0]
          << (uint16_t)device_info.worldWideName[1]
          << (uint16_t)device_info.worldWideName[2]
          << (uint16_t)device_info.worldWideName[3]
          << (uint16_t)device_info.worldWideName[4]
          << (uint16_t)device_info.worldWideName[5]
          << (uint16_t)device_info.worldWideName[6]
          << (uint16_t)device_info.worldWideName[7]
    ;

  if (! (_NvmeIdentify("Identify Controller", NVME_NSID_NONE, NVME_IDENTIFY_CONTROLLER_DATA_STRUCTURE,
                         parseIdentifyControllerResponse,
                         osDeviceHandle, interfaceDeviceIdentification, device_info)))
    return false;


  device_info.devType = DEVICE_TYPE_NVME;
  return true;
}


bool DtaNvmeDrive::identify(DTA_DEVICE_INFO& device_info)
{
  InterfaceDeviceID interfaceDeviceIdentification;
  return identifyUsingNvmeIdentify(osDeviceHandle, interfaceDeviceIdentification, device_info);
}
