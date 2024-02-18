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
#if defined(__APPLE__) && defined(__MACH__)
    /* Apple OSX and iOS (Darwin). ------------------------------ */
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR == 1
    /* iOS in Xcode simulator */

#elif TARGET_OS_IPHONE == 1
    /* iOS on iPhone, iPad, etc. */

#elif TARGET_OS_MAC == 1
    /* OSX */

#include "os.h"
#include <log/log.h>
#include <mach/mach_port.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/scsi/SCSITaskLib.h>
#include <IOKit/scsi/SCSICommandOperationCodes.h>
#include <SEDKernelInterface/InterfaceCommandCodes.h>
#include <SEDKernelInterface/TPerKernelInterface.h>

// #include <SEDKernelInterface/SEDKernelInterface.h>

#include "DtaMacOSConstants.h"
#include "DtaDevMacOSTPer_SCSI.h"

using namespace std;

/** The Device class represents a single disk device.
 *  Mac specific implementation using the USB interface
 */

uint8_t DtaDevMacOSTPer_SCSI::sendCmd(ATACOMMAND cmd,                  // IF_RECV (0x5c) / IF_SEND (0x5e) / IDENTIFY (0xec)
                                     uint8_t secProtocol,             // this is ATA sec protocol, not 4,5 for data in/out
                                     uint16_t comID,
                                     void * buffer,                   // either inBuffer or outBuffer, aligned
                                     size_t bufferlen)              // allocated length
{
    kern_return_t kernResult;
    uint64_t bufferLen64 = bufferlen;
    // not testing the sense buffer
    //    uint8_t sense[32]; // how big should this be??
    SCSICommandDescriptorBlock cdb;
    uint8_t cdbLength=0;
    
    LOG(D1) << "Entering DtaDevMacOSTPer_SCSI::sendCmd";
    memset(&cdb, 0, sizeof (cdb));
    //    /*
    //     * Initialize the CDB as described in SAT-2 and the
    //     * ATA Command set reference (protocol and commID placement)
    //     */
    
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

    switch (cmd) {
        case IDENTIFY:  // SCSI Inquiry
            cdb[0] = kSCSICmd_INQUIRY;
            cdb[1] = 1;                  // EVPD
            cdb[2] = 0x89;               // Page Code
            cdb[3] = (uint8_t)(bufferlen >> 8);     // Allocation Length (MSB)
            cdb[4] = (uint8_t)(bufferlen & 0xFF);   // Allocation Length (LSB)
            cdbLength = 6;
            break;

        case IF_RECV:
            cdb[0] = kSCSICmd_SECURITY_PROTOCOL_IN;
            cdb[1] = secProtocol;
            cdb[2] = ((comID & 0xff00) >> 8);
            cdb[3] = (comID & 0x00ff);
            cdb[6] = (uint8_t)((bufferlen & 0xff000000) >> 24);
            cdb[7] = (uint8_t)((bufferlen & 0x00ff0000) >> 16);
            cdb[8] = (uint8_t)((bufferlen & 0x0000ff00) >> 8);
            cdb[9] = (uint8_t)((bufferlen & 0x000000ff));
            cdbLength = 12;
            break;
            
        case IF_SEND:
            cdb[0] = kSCSICmd_SECURITY_PROTOCOL_OUT;
            cdb[1] = secProtocol;
            cdb[2] = ((comID & 0xff00) >> 8);
            cdb[3] = (comID & 0x00ff);
            cdb[6] = (uint8_t)((bufferlen & 0xff000000) >> 24);
            cdb[7] = (uint8_t)((bufferlen & 0x00ff0000) >> 16);
            cdb[8] = (uint8_t)((bufferlen & 0x0000ff00) >> 8);
            cdb[9] = (uint8_t)((bufferlen & 0x000000ff));
            cdbLength = 12;
            break;

        default:
            LOG(E) << "Unexpected Command";
            return 0xff;
    }

    (void)cdbLength;  // TODO: use this 
    
    
#if DEBUG
    //      printf("in SendCmd CDB is : ");
    //      printBuffer(cdb, sizeof(cdb));
#endif
    kernResult = PerformSCSICommand(connect, cdb, buffer, bufferLen64, bufferLen64, &bufferLen64);
    
    return (kernResult == kIOReturnSuccess) ? 0 : 0xff; // not ATA response -- just for stubbing, not for reals
}

#endif
#endif // defined(__APPLE__) && defined(__MACH__)
