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

#include "DtaScsiDrive.h"

/**  specific implementation of DtaDev.
 * Uses the SCSI generic ioctls to send commands to the
 * device
 */
class DtaAtaDrive: public DtaDrive {
public:
    using DtaDrive::DtaDrive;

    static DtaAtaDrive * getDtaOSSpecificAtaDrive(OSDEVICEHANDLE _osDeviceHandle, bool scsiTranslated);
    static
        DtaDrive*
        getDtaAtaDrive(const char* devref, DTA_DEVICE_INFO& device_info);

  /** Ata specific method to send an ATA command to the device
   * @param cmd ATA command to be sent to the device
   * @param protocol security protocol to be used in the command
   * @param comID communications ID to be used
   * @param buffer input/output buffer
   * @param bufferlen length of the input/output buffer
   */
  virtual uint8_t sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                          void * buffer, uint32_t bufferlen);


  /** Identify this device using ATA Identify Device command
   *  via Scsi ATA pass-through
   *  to obtain data to fill out disk_info.
   */
  virtual bool identify(DTA_DEVICE_INFO& device_info);



  DtaAtaDrive(OSDEVICEHANDLE _osDeviceHandle, bool _scsiTranslated)
    : DtaDrive(_osDeviceHandle),
      scsiTranslated(_scsiTranslated)
  {}


  static
  bool identifyUsingATAIdentifyDevice(OSDEVICEHANDLE osDeviceHandle,
                                      InterfaceDeviceID & interfaceDeviceIdentification,
                                      DTA_DEVICE_INFO & device_info,
                                      dictionary ** pIdentifyCharacteristics,
                                      bool & scsiTranslated);

private:
  static
    int __identifyDevice( OSDEVICEHANDLE osDeviceHandle, bool scsiTranslated, void * buffer , unsigned int & dataLength);

  static
  dictionary *
  parseATAIdentifyDeviceResponse(const InterfaceDeviceID & interfaceDeviceIdentification,
                                 const unsigned char * response,
                                 DTA_DEVICE_INFO & device_info);

  bool scsiTranslated;

    /** Perform an ATA command using the current operating system HD interface
     *
     * @param cmd               ATACOMMAND opcode IDENTIFY_DEVICE, TRUSTED_SEND, or TRUSTED_RECEIVE
     * @param securityProtocol  security protocol ID per ATA command spec
     * @param comID             communication channel ID per TCG spec
     * @param buffer            address of data buffer
     * @param bufferlen         data buffer len, also output transfer length
     *
     * Returns the result of the os system call
     */
    int PerformATACommand(ATACOMMAND cmd, uint8_t securityProtocol, uint16_t comID,
                          void * buffer,  unsigned int & bufferlen)
    {
        return OS.PerformATACommand(osDeviceHandle, scsiTranslated,
                                    cmd, securityProtocol, comID,
                                    buffer,  bufferlen);
    }

    static
    bool tryToIdentifyUsingATAIdentifyDevice_WithScsiTranslated(bool &scsiTranslated,
                                                                OSDEVICEHANDLE osDeviceHandle,
                                                                InterfaceDeviceID &interfaceDeviceIdentification,
                                                                DTA_DEVICE_INFO &disk_info,
                                                                dictionary **ppIdentifyCharacteristics,
                                                                void *identifyDeviceResponse,
                                                                unsigned int &dataLen);

};
