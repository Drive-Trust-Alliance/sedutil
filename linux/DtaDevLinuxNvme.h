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
#pragma once
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0)
#include <linux/nvme_ioctl.h>
#else
#include <linux/nvme.h>
#endif
#include "DtaStructures.h"
#include "DtaDevLinuxDrive.h"

/** Linux specific implementation of DtaDevOS.
 * Uses the NVMe to send commands to the
 * device
 */
#define is_aligned(POINTER, BYTE_COUNT) \
    (((uintptr_t)(const void *)(POINTER)) % (BYTE_COUNT) == 0)
class DtaDevLinuxNvme: public DtaDevLinuxDrive{
public:

  static bool isDtaDevLinuxNvmeDevRef(const char * devref);

  static DtaDevLinuxNvme * getDtaDevLinuxNvme(const char * devref,
                                              DTA_DEVICE_INFO & disk_info);

    /** NVMe specific method to send a command to the device
     * @param cmd command to be sent to the device
     * @param protocol security protocol to be used in the command
     * @param comID communications ID to be used
     * @param buffer input/output buffer
     * @param bufferlen length of the input/output buffer
     */
    uint8_t sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
            void * buffer, uint32_t bufferlen);

    /** NVMe specific routine to send an identify to the device */
    virtual bool identify(DTA_DEVICE_INFO& disk_info)
  {
    InterfaceDeviceID interfaceDeviceIdentification;
    return  identifyUsingNvmeIdentify(fd, interfaceDeviceIdentification, disk_info);
  }


  DtaDevLinuxNvme(int _fd)
    : DtaDevLinuxDrive(_fd)
  {}

  ~DtaDevLinuxNvme(){}


  static
  bool identifyUsingNvmeIdentify(int fd,
                                 InterfaceDeviceID & interfaceDeviceIdentification,
                                 DTA_DEVICE_INFO & disk_info);

};
