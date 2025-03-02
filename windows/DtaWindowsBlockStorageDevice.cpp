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

#include "DtaWindowsBlockStorageDevice.h"


DtaWindowsBlockStorageDevice *
DtaWindowsBlockStorageDevice::getDtaWindowsBlockStorageDevice(const char * devref, DTA_DEVICE_INFO & device_info) {

  bool accessDenied=false;
  OSDEVICEHANDLE osDeviceHandle = OS.openAndCheckDeviceHandle(devref, accessDenied);
  if (osDeviceHandle==INVALID_HANDLE_VALUE || accessDenied)
    return NULL;

  LOG(D4) << "Success opening device " << devref << " as file handle " << HEXON(4) << (size_t) osDeviceHandle;

  InterfaceDeviceID interfaceDeviceIdentification;

  //
  if (! identify(osDeviceHandle, devref, interfaceDeviceIdentification, device_info)) {
    device_info.devType = DEVICE_TYPE_OTHER;
    LOG(E) << " Device " << devref << " is NOT Block Storage Device?! -- file handle " << HEXON(2) << (size_t) osDeviceHandle;
    OS.closeDeviceHandle(osDeviceHandle);
    return NULL;
  }

  return new DtaWindowsBlockStorageDevice(osDeviceHandle);
}



bool DtaWindowsBlockStorageDevice::identify(OSDEVICEHANDLE osDeviceHandle,
    const char* devref,
    InterfaceDeviceID & interfaceDeviceIdentification, DTA_DEVICE_INFO & device_info)
{
    dictionary* properties = OS.getOSSpecificInformation(osDeviceHandle, devref, interfaceDeviceIdentification, device_info);
    if (properties != NULL) {
        delete properties;
        return true;
    }
  return false;
}

uint8_t DtaWindowsBlockStorageDevice::discovery0(DTA_DEVICE_INFO& /* device_info */ ) {
  return false;
}
