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


#include "log.h"
#include "fnmatch.h"

#include "DtaLinuxBlockStorageDevice.h"


DtaLinuxBlockStorageDevice *
DtaLinuxBlockStorageDevice::getDtaLinuxBlockStorageDevice(const char * devref, DTA_DEVICE_INFO & device_info) {

  bool accessDenied=false;
  OSDEVICEHANDLE osDeviceHandle = OS.openAndCheckDeviceHandle(devref, accessDenied);
  if (osDeviceHandle==INVALID_HANDLE_VALUE || accessDenied)
    return NULL;

  LOG(D4) << "Success opening device " << devref << " as file handle " << HEXON(4) << (size_t) osDeviceHandle;

  InterfaceDeviceID interfaceDeviceIdentification;

  //
  if (! identify(osDeviceHandle, interfaceDeviceIdentification, device_info)) {
    device_info.devType = DEVICE_TYPE_OTHER;
    LOG(E) << " Device " << devref << " is NOT Block Storage Device?! -- file handle " << HEXON(2) << (size_t) osDeviceHandle;
    OS.closeDeviceHandle(osDeviceHandle);
    return NULL;
  }

  return new DtaLinuxBlockStorageDevice(osDeviceHandle);
}



bool DtaLinuxBlockStorageDevice::identify(OSDEVICEHANDLE osDeviceHandle,
                                          InterfaceDeviceID & /* interfaceDeviceIdentification */,
                                          DTA_DEVICE_INFO & device_info)
{
  return true;
}

uint8_t DtaLinuxBlockStorageDevice::discovery0(DTA_DEVICE_INFO& /* device_info */ ) {
  return DTAERROR_FAILURE;
}
