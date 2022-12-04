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
#include <IOKit/storage/ata/IOATAStorageDefines.h>
//#include <SEDKernelInterface/SEDKernelInterface.h>
#include "TPerKernelInterface.h"
#include "PrintBuffer.h"
#include "RegistryUtilities.h"
#include "DtaMacOSConstants.h"
#include "DtaDevMacOS_ATA.h"

using namespace std;

/** The Device class represents a single disk device.
 */

uint8_t DtaDevMacOS_ATA::sendCmd(ATACOMMAND cmd,                  // IF_RECV (0x5c) / IF_SEND (0x5e) / IDENTIFY (0xec)
                                 uint8_t secProtocol,             // this is ATA sec protocol, not 4,5 for data in/out
                                 uint16_t comID,
                                 void * buffer,                   // either inBuffer or outBuffer, aligned
                                 size_t bufferlen)              // allocated length
{
     return 0xff; // not ATA response -- just for stubbing, not for reals
}


uint8_t DtaDevMacOS_ATA::identify(OPAL_DiskInfo& disk_info )
{
    return KERN_FAILURE;
}


uint8_t DtaDevMacOS_ATA::discovery0(uint8_t * response)
{
    return KERN_FAILURE;
}

