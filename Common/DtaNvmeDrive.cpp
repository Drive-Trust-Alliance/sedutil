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
    // LOG(E) << "DtaNvmeDrive::getDtaNvmeDrive: Device " << devref << " is NOT Nvme?! -- file handle " << HEXON(2) << (size_t) osDeviceHandle;
    OS.closeDeviceHandle(osDeviceHandle);
    return NULL;
  }

  return getDtaOSSpecificNvmeDrive(osDeviceHandle);
}


dictionary *
DtaNvmeDrive::parseIdentifyResponse(const unsigned char * response,
                                    InterfaceDeviceID & interfaceDeviceIdentification,
                                    DTA_DEVICE_INFO & device_info)
{
  device_info.devType = DEVICE_TYPE_NVME;

  const uint8_t *results = reinterpret_cast<const uint8_t *>(response);
  results += 4;
  safecopy(device_info.serialNum, sizeof(device_info.serialNum), results, sizeof(device_info.serialNum));
  results += sizeof(device_info.serialNum);
  safecopy(device_info.modelNum, sizeof(device_info.modelNum), results, sizeof(device_info.modelNum));
  results += sizeof(device_info.modelNum);
  safecopy(device_info.firmwareRev, sizeof(device_info.firmwareRev), results, sizeof(device_info.firmwareRev));

  memcpy(interfaceDeviceIdentification, response, sizeof(InterfaceDeviceID));

  // device is apparently a NVMe disk
  return new dictionary
    {
      {"Device Type"       , "NVMe"                       },
      {"Vendor ID"         , (const char *)device_info.vendorID    },
      {"Model Number"      , (const char *)device_info.modelNum    },
      {"Firmware Revision" , (const char *)device_info.firmwareRev },
      {"Serial Number"     , (const char *)device_info.serialNum   },
    };
}





/** Send an ioctl to the device using pass through. */
uint8_t DtaNvmeDrive::sendCmd(ATACOMMAND cmd,
                              uint8_t protocol,
                              uint16_t comID,
                              void * buffer, unsigned int bufferlen)
{
  LOG(D1) << "Entering DtaNvmeDrive::sendCmd";

  struct nvme_admin_cmd nvme_cmd;
  memset(&nvme_cmd, 0, sizeof(nvme_cmd));

  if (TRUSTED_RECEIVE == cmd) {
    LOG(D3) << "Nvme Security Receive Command";
    nvme_cmd.opcode = NVME_SECURITY_RECV;
  }
  else {
    LOG(D3) << "Nvme Security Send Command";
    nvme_cmd.opcode = NVME_SECURITY_SEND;
  }

  nvme_cmd.cdw10 = static_cast<__u32>(protocol << 24 | comID << 8);
  nvme_cmd.cdw11 = bufferlen;
  nvme_cmd.data_len = bufferlen;
  nvme_cmd.addr = reinterpret_cast<__u64>(buffer);


  int err = OS.PerformNVMeCommand(osDeviceHandle, reinterpret_cast<uint8_t *>(&nvme_cmd));

  if (err < 0)
    return static_cast<uint8_t>(errno);
  else if (err != 0) {
    //fprintf(stderr, "NVME Security Command Error:%d\n", err);
    LOG(D3) << "NVME Security Command Error: " << err ;
    IFLOG(D4) DtaHexDump(&nvme_cmd, sizeof(nvme_cmd));
  }
  else
    LOG(D3) << "NVME Security Command Success:" << nvme_cmd.result;
  //LOG(D4) << "NVMe command:";
  //IFLOG(D4) DtaHexDump(&nvme_cmd, sizeof(nvme_cmd));
  //LOG(D4) << "NVMe buffer @ " << buffer;
  //IFLOG(D4) DtaHexDump(buffer, bufferlen);
  return static_cast<uint8_t>(err);
}

bool DtaNvmeDrive::identifyUsingNvmeIdentify(OSDEVICEHANDLE osDeviceHandle,
                                             InterfaceDeviceID & interfaceDeviceIdentification,
                                             DTA_DEVICE_INFO & device_info)
{
  LOG(D4) << "Entering DtaNvmeDrive::identifyUsingNvmeIdentify";

  uint8_t ctrl[4096];

  struct nvme_admin_cmd cmd;
  memset(&cmd, 0, sizeof(cmd));

  LOG(D3) << "Nvme Identify Command";
  cmd.opcode = NVME_IDENTIFY;
  cmd.nsid = 0;
  cmd.addr = (unsigned long long)&ctrl;
  cmd.data_len = 4096;
  cmd.cdw10 = 1;

  int err = OS.PerformNVMeCommand(osDeviceHandle, reinterpret_cast<uint8_t *>(&cmd));

  if (err) {
    device_info.devType = DEVICE_TYPE_OTHER;
    LOG(D4) << "Nvme Identify error. NVMe status " << err;
    IFLOG(D4) DtaHexDump(&cmd, sizeof(cmd));
    IFLOG(D4) DtaHexDump(&ctrl, sizeof(ctrl));
    return false;
  }

  dictionary * responses = parseIdentifyResponse(ctrl,interfaceDeviceIdentification,device_info);

  (void)responses; // TODO: Or make use of this here!

  delete responses;
  return true;
}

bool DtaNvmeDrive::identify(DTA_DEVICE_INFO& device_info)
{
  InterfaceDeviceID interfaceDeviceIdentification;
  return identifyUsingNvmeIdentify(osDeviceHandle, interfaceDeviceIdentification, device_info);
}
