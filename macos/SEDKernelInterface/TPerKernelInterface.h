//
//  TPerKernelInterface.h
//  TPerKernelInterface
//
//  Created by Jackie Marks on 9/21/15.
//  Copyright © 2015 Bright Plaza Inc. All rights reserved.
//

#ifndef TPerKernelInterface_h
#define TPerKernelInterface_h

#include <IOKit/IOKitLib.h>
#include "UserKernelShared.h"



#if defined ( __cplusplus )
extern "C" {
#endif

    kern_return_t OpenUserClient(io_service_t service, io_connect_t *pConnect);
    kern_return_t CloseUserClient(io_connect_t connect);


// SWG specificed functions
    bool Discovery0(io_registry_entry_t driverService, UInt8 * buffer);

// Driver info cache update
    kern_return_t updateLockingPropertiesInIORegistry(io_connect_t connect);

// Low-level functions

//  Access the USB SCSI emulation to send wrapped ATA commands to a SATA TPer
//
    kern_return_t SCSIPassThroughInterface(io_connect_t connect,
                                       SCSICommandDescriptorBlock cdb,
                                       const void * buffer,
                                       const uint64_t * pbufferSize);

//  Perform an AT Identify command and return the result in the buffer
//
    void IdentifyDevice(io_registry_entry_t driverService, UInt8 * buffer);

#if defined (__cplusplus)
}
#endif

#endif  // #ifndef TPerKernelInterface_h
