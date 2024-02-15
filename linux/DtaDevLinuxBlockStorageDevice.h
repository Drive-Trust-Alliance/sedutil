/* C:B**************************************************************************
   This software is Copyright 2022 Bright Plaza Inc. <drivetrust@drivetrust.com>

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
#pragma once

#include <vector>
#include <string>
#include "DtaStructures.h"
#include "DtaDev.h"

class DtaDevLinuxBlockStorageDevice {
public:

  virtual ~DtaDevLinuxBlockStorageDevice();
  /** Does the device conform to ANY TCG storage SSC */
  uint8_t isAnySSC();
  /** Returns the Vendor ID reported by the Identify command */
  const char *getVendorID();
  /** Returns the Manufacturer Name reported by the Identify command */
  const char *getManufacturerName();
  /** Returns the Firmware revision reported by the identify command */
  const char *getFirmwareRev();
  /** Returns the Model Number reported by the Identify command */
  const char *getModelNum();
  /** Returns the Serial Number reported by the Identify command */
  const char *getSerialNum();
  /** Returns the password salt, usually the Serial Number reported by the Identify command unmodified by subsequent polishing*/
  const vector<uint8_t> getPasswordSalt();
  /** Returns the World Wide Name reported by the Identify command */
  const vector<uint8_t> getWorldWideName();
  /** Returns whether that World Wide Name was actually derived using OUI data based heuristics */
  uint8_t getWorldWideNameIsSynthetic();
  /** Returns the Physical Interconnect technology */
  const char *getPhysicalInterconnect();
  /** Returns the Physical Interconnect Location */
  const char * getPhysicalInterconnectLocation();
  /** Returns the BSD Name */
  const char *getBSDName();
  /* What type of disk attachment is used */
  DTA_DEVICE_TYPE getDevType();

  const std::string getDevPath ();

  const unsigned long long getSize();

  typedef <std::pair<DtaDevLinuxBlockStorageDevice *, DTA_DEVICE_INFO *> bsdPair;
  static std::vector<bsdPair> enumerateBlockStorageDevices();

  static DtaDevLinuxBlockStorageDevice * getBlockStorageDevice(const char * devref, DTA_DEVICE_INFO &di);  // Factory for this class or subclass instances


  const DTA_DEVICE_INFO & device_info(void);  /**< Weak reference to Structure containing info from properties, including identify and discovery 0 if available
                                                 Asserts if no such reference.*/


private:

  // derived
  static bool deviceNameLessThan(bsdPair * a, bsdPair * b);  // for sorting
protected:
  DTA_DEVICE_INFO * pdevice_info;  /**< Weak reference to Structure containing info from properties, including identify and discovery 0 if available*/
  static void polishDeviceInfo(DTA_DEVICE_INFO & device_info);
  DtaDevLinuxBlockStorageDevice()
    : deviceName(NULL),
      pdevice_info(NULL)
  {};

  DtaDevLinuxBlockStorageDevice(std::string deviceName)
    : deviceName(deviceName),
      pdevice_info(NULL)
  {};

  DtaDevLinuxBlockStorageDevice(std::string deviceName, DTA_DEVICE_INFO & di)
    : deviceName(deviceName),
      pdevice_info(pdi)
  {init(deviceName.c_str());};

  bool init(const char * devref);

  int fd;
};

#endif // defined(__APPLE__) && defined(__MACH__)
