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
#pragma once
#include "DtaOS.h"

#define className DtaWindows
#define Windows (*(static_cast<className *>(&OS)))

class className : public DtaOS {
public:
    using DtaOS::DtaOS;;


    virtual OSDEVICEHANDLE openDeviceHandle(const char * devref, bool & accessDenied);

    virtual void closeDeviceHandle(OSDEVICEHANDLE osDeviceHandle);

    virtual std::vector<std::string> generateDtaDriveDevRefs();

    virtual void errorNoAccess(const char * devref);

    virtual
    dictionary* getOSSpecificInformation(OSDEVICEHANDLE osDeviceHandle,
                                         const char* devref,
                                         InterfaceDeviceID& interfaceDeviceIdentification,
                                         DTA_DEVICE_INFO& device_info);

    virtual void * alloc_aligned_MIN_BUFFER_LENGTH_buffer ();

    virtual void free_aligned_MIN_BUFFER_LENGTH_buffer (void * aligned_buffer);


    /** Perform an ATA command using the current operating system HD interface
     *
     * @param osDeviceHandle    OSDEVICEHANDLE of already-opened raw device file
     * @param cmd               ATACOMMAND opcode IDENTIFY_DEVICE, TRUSTED_SEND, or TRUSTED_RECEIVE
     * @param securityProtocol  security protocol ID per ATA command spec
     * @param comID             communication channel ID per TCG spec
     * @param buffer            address of data buffer
     * @param bufferlen         data buffer len, also output transfer length
     *
     * Returns the result of the os system call
     */
    virtual int PerformATACommand_via_HD(OSDEVICEHANDLE osDeviceHandle,
        ATACOMMAND cmd, uint8_t securityProtocol, uint16_t comID,
        void* buffer, unsigned int& bufferlen) {
        (void)((void)osDeviceHandle,
            (void)cmd, (void)securityProtocol, (void)comID,
            (void)buffer, (void)bufferlen);
        return DTAERROR_FAILURE;
    };


    /** Perform a SCSI command using the current operating system SCSI interface
     *
     * @param osDeviceHandle            OSDEVICEHANDLE osDeviceHandle of already-opened raw device file
     * @param dxfer_direction direction of transfer PSC_FROM/TO_DEV
     * @param cdb             SCSI command data buffer
     * @param cdb_len         length of SCSI command data buffer (often 12)
     * @param buffer          SCSI data buffer
     * @param bufferlen       SCSI data buffer len, also output transfer length
     * @param sense           SCSI sense data buffer
     * @param senselen        SCSI sense data buffer len (usually 32?)
     * @param pmasked_status  pointer to storage for masked_status, or NULL if not desired
     * @param timeout         optional timeout (in msecs)
     *
     * Returns the result of the os system call, as well as possibly setting *pmasked_status
     */
    virtual int PerformSCSICommand(OSDEVICEHANDLE osDeviceHandle,
                                   int dxfer_direction,
                                   uint8_t * cdb,   unsigned char cdb_len,
                                   void * buffer,   unsigned int& bufferlen,
        unsigned char * sense, unsigned char & senselen,
                                   SCSI_STATUS_CODE * pmasked_status,
                                   unsigned int timeout);


    /** Perform a NVMe command using the Linux `nvme_admin_cmd' (NVMe standard) interface with ioctl `NVME_IOCTL_ADMIN_CMD'
     *
     * @param osDeviceHandle            OSDEVICEHANDLE osDeviceHandle of already-opened raw device file
     * @param cmd             NVMe command struct
     *
     * Returns the result of the os system call, as well as possibly setting *pstatus
     */
      virtual int PerformNVMeCommand(OSDEVICEHANDLE osDeviceHandle,
                                     uint8_t * cmd,
                                     uint32_t *pstatus);



};
