/* C:B**************************************************************************
This software is Copyright 2014-2016 Bright Plaza Inc. <drivetrust@drivetrust.com>

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
#include "os.h"
#include <mach/mach_port.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/scsi/SCSITaskLib.h>
//#include <SEDKernelInterface/SEDKernelInterface.h>
#include "TPerKernelInterface.h"
#include "PrintBuffer.h"
#include "RegistryUtilities.h"
#include "DtaMacOSConstants.h"
#include "DtaDevMacOS_UAS_SAT.h"

using namespace std;

/** The Device class represents a single disk device.
 *  Mac specific implementation using the USB interface
 */

uint8_t DtaDevMacOS_UAS_SAT::sendCmd(ATACOMMAND cmd,                  // IF_RECV (0x5c) / IF_SEND (0x5e) / IDENTIFY (0xec)
                                     uint8_t secProtocol,             // this is ATA sec protocol, not 4,5 for data in/out
                                     uint16_t comID,
                                     void * buffer,                   // either inBuffer or outBuffer, aligned
                                     uint16_t bufferlen)              // allocated length
{
    kern_return_t kernResult;
    uint64_t bufferLen64 = bufferlen;
    // not testing the sense buffer
    //    uint8_t sense[32]; // how big should this be??
    SCSICommandDescriptorBlock cdb;
    
    LOG(D1) << "Entering DtaDevMacOSUSB::sendCmd";
    memset(&cdb, 0, sizeof (cdb));
    //    /*
    //     * Initialize the CDB as described in SAT-2 and the
    //     * ATA Command set reference (protocol and commID placement)
    //     */
    
    cdb[0] = 0xa1; // SCSI ata pass-through(12) command (see e.g. 04-262r8.pdf)
    
    //    /*
    //     * Byte 1 is the protocol 4 = PIO IN and 5 = PIO OUT
    //     * Byte 2 is:
    //     * bits 7-6 OFFLINE - Amount of time the command can take the bus offline
    //     * bit 5    CK_COND - If set the command will always return a condition check
    //     * bit 4    RESERVED
    //     * bit 3    T_DIR   - transfer direction 1 in, 0 out
    //     * bit 2    BYTE_BLock  1 = transfer in blocks, 0 transfer in bytes
    //     * bits 1-0 T_LENGTH -  10 = the length id in sector count
    //     */
    
    if (IDENTIFY == cmd) {
        LOG(D3) << "Identify Command";
        cdb[1] = 4 << 1; // PIO DATA IN
        cdb[2] = 0x0E; // T_DIR = 1, BYTE_BLOCK = 1, Length in Sector Count
        memset(buffer, 0, bufferlen);
    } else if (IF_RECV == cmd) {
        LOG(D3) << "Security Receive Command";
        cdb[1] = 4 << 1; // PIO DATA IN
        cdb[2] = 0x0E; // T_DIR = 1, BYTE_BLOCK = 1, Length in Sector Count
        memset(buffer, 0, bufferlen);
    } else if (IF_SEND == cmd) {
        LOG(D3) << "Security Send Command";
        cdb[1] = 5 << 1; // PIO DATA OUT
        cdb[2] = 0x06; // T_DIR = 0, BYTE_BLOCK = 1, Length in Sector Count
    } else {
        LOG(E) << "Unexpected Command";
        return 0xff;
    }
    
    cdb[3] = secProtocol; // ATA features / TRUSTED S/R security protocol
    cdb[4] = bufferlen / 512; // Sector count / transfer length (512b blocks)
    //      cdb[5] = reserved;
    cdb[7] = ((comID & 0xff00) >> 8);
    cdb[6] = (comID & 0x00ff);
    //      cdb[8] = 0x00;              // device
    cdb[9] = cmd; // IF_SEND/IF_RECV
    //      cdb[10] = 0x00;              // reserved
    //      cdb[11] = 0x00;              // control
    
#if DEBUG
    //      printf("in SendCmd CDB is : ");
    //      printBuffer(cdb, sizeof(cdb));
#endif
    kernResult = SCSIPassThroughInterface(connect, cdb,
                                          buffer, &bufferLen64);
    
    return (kernResult == kIOReturnSuccess) ? 0 : 0xff; // not ATA response -- just for stubbing, not for reals
}

static void ReformatIdentifyResponseAsDiskInfo(uint8_t * response, OPAL_DiskInfo& disk_info)
{
    IDENTIFY_RESPONSE & id = * (IDENTIFY_RESPONSE *) response;
    disk_info.devType = DEVICE_TYPE_USB; // TODO -- generalize for other devices when they are supported by BPTperDriver
    for (size_t i = 0; i < sizeof (disk_info.serialNum); i += 2) {         // uint8_t[20]
        disk_info.serialNum[i] = id.serialNum[i + 1];
        disk_info.serialNum[i + 1] = id.serialNum[i];
    }
    for (size_t i = 0; i < sizeof (disk_info.firmwareRev); i += 2) {       // uint8_t[8]
        disk_info.firmwareRev[i] = id.firmwareRev[i + 1];
        disk_info.firmwareRev[i + 1] = id.firmwareRev[i];
    }
    for (size_t i = 0; i < sizeof (disk_info.modelNum); i += 2) {          // uint8_t[40]
        disk_info.modelNum[i] = id.modelNum[i + 1];
        disk_info.modelNum[i + 1] = id.modelNum[i];
    }
}

uint8_t DtaDevMacOS_UAS_SAT::identify(OPAL_DiskInfo& disk_info )
{
    uint8_t response[IDENTIFY_RESPONSE_SIZE]={0};
    
    LOG(D4) << "Entering DtaDevMacOSUSB::identify()";
    if (KERN_SUCCESS == GetIdentifyResponseFromIORegistry(response) ){
        ReformatIdentifyResponseAsDiskInfo(response, disk_info);
        return KERN_SUCCESS;
    }
    return KERN_FAILURE;
}


uint8_t DtaDevMacOS_UAS_SAT::discovery0(uint8_t * response)
{
    return GetD0ResponseFromIORegistry(response);
}

