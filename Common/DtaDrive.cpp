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

#include "DtaDrive.h"
#include "ParseDiscovery0Features.h"

DtaDrive * DtaDrive::getDtaDrive(const char * devref,
                                 DTA_DEVICE_INFO &device_info,
                                 bool & accessDenied)
{
    LOG(D4) << "DtaDrive::getDtaDrive(\"" << devref << "\", device_info, accessDenied=" << std::boolalpha << accessDenied << ")";
    OSDEVICEHANDLE osDeviceHandle = OS.openDeviceHandle(devref, accessDenied);
    if (INVALID_HANDLE_VALUE==osDeviceHandle || accessDenied) {
      LOG(D4) << "DtaDrive::getDtaDrive"
              << " osDeviceHandle=" << HEXON(8) << osDeviceHandle
              << " accessDenied=" << std::boolalpha << accessDenied
              << " could not open device handle, returning NULL";

        return NULL;
    }

    DtaDrive* drive = NULL;
    InterfaceDeviceID interfaceDeviceIdentification;
    memset(interfaceDeviceIdentification, 0, sizeof(interfaceDeviceIdentification));
    LOG(D4) << devref << " driveParameters:";
    dictionary * maybeDriveParameters =
        OS.getOSSpecificInformation(osDeviceHandle, devref, interfaceDeviceIdentification, device_info);
    OS.closeDeviceHandle(osDeviceHandle);

    if (maybeDriveParameters == NULL) {
        LOG(D4) << "Failed to determine drive parameters for " << devref;
        return NULL;
    }

    dictionary & driveParameters = *maybeDriveParameters;
    IFLOG(D4)
    for (const auto & pair : driveParameters) {
        LOG(D4) << pair.first << ":\"" << pair.second << "\"";
    }

    #define trySubclass(variant)                                               \
    if ((drive = DtaDrive::getDtaDrive(variant, devref, device_info)) != NULL) \
    {                                                                          \
        break;                                                                 \
    } else

    #define logSubclassFailed(variant)                                                   \
    LOG(D4) << "DtaDrive::getDtaDrive(" #variant ", \"" << devref << "\", device_info) " \
            << "returned NULL";

    #define skipSubclass(variant)                                                        \
    LOG(D4) << "DtaDrive::getDtaDrive(" #variant ", \"" << devref << "\", device_info) " \
            << "unimplmented";


    // Create a subclass instance based on device_info.devType as determined by
    // getOSSpecificInformation.  Customizing code has device_info and
    // drive parameters available.
    //
    switch (device_info.devType) {
        case DEVICE_TYPE_SCSI:  // SCSI
        case DEVICE_TYPE_SAS:   // SCSI
            trySubclass(ScsiDrive)
            break;

        case DEVICE_TYPE_USB:   // UAS SAT -- USB -> SCSI -> AT pass-through
            if (!deviceNeedsSpecialAction(interfaceDeviceIdentification,avoidSlowSATATimeout)) {
                trySubclass(SataDrive);
            }
            if (!deviceNeedsSpecialAction(interfaceDeviceIdentification,avoidSlowSASTimeout)) {
                trySubclass(ScsiDrive);
            }
            trySubclass(BlockStorageDevice);   // TODO: hack
            break;

        case DEVICE_TYPE_NVME:  // NVMe
            if (deviceNeedsSpecialAction(interfaceDeviceIdentification, acceptPseudoDeviceImmediately)) {
                trySubclass(BlockStorageDevice);   // TODO: hack
                break;
            }

            trySubclass(NvmeDrive)   // TODO test
            trySubclass(BlockStorageDevice);   // TODO: hacks
            break;

        case DEVICE_TYPE_ATA:   // SATA / PATA
            trySubclass(AtaDrive)
            break;

        case DEVICE_TYPE_OTHER:
            LOG(D4) << "Unimplemented device type " << devref;
            break;

        default:
            break;
    }

    delete &driveParameters;
    return drive ;
}


uint8_t DtaDrive::discovery0(DTA_DEVICE_INFO & disk_info) {
  LOG(D4) << "DtaDrive::discovery0";
  void * d0Response = OS.alloc_aligned_MIN_BUFFER_LENGTH_buffer();
  if (d0Response == NULL)
      return DTAERROR_COMMAND_ERROR;
  memset(d0Response, 0, MIN_BUFFER_LENGTH);

  int lastRC = sendCmd(TRUSTED_RECEIVE, 0x01, 0x0001, d0Response, MIN_BUFFER_LENGTH);
  if ((lastRC ) != 0) {
    LOG(D4) << "DtaDrive::discovery0:Acquiring Discovery 0 response failed " << lastRC;
    return DTAERROR_COMMAND_ERROR;
  }
  parseDiscovery0Features((uint8_t *)d0Response, disk_info);
  OS.free_aligned_MIN_BUFFER_LENGTH_buffer(d0Response);
  LOG(D4) << "DtaDrive::discovery0:Acquiring Discovery 0 response succeeded.";
  return DTAERROR_SUCCESS;
}
