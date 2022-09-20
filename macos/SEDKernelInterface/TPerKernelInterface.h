//
//  TPerKernelInterface.h
//  TPerKernelInterface
//
//  Created by Jackie Marks on 9/21/15.
//  Copyright © 2015 Bright Plaza Inc. All rights reserved.
//

#ifndef TPerKernelInterface_h
#define TPerKernelInterface_h

#include <IOKit/IOTypes.h>
#include "UserKernelShared.h"
#include "DtaStructures.h"

#if defined ( __cplusplus )
extern "C" {
#endif

    kern_return_t OpenUserClient(io_service_t service, io_connect_t *pConnect);
    kern_return_t CloseUserClient(io_connect_t connect);


//  Access the drive and update the DTA_DEVICE_INFO produced by Identify/Inquiry
//  and the cached Discovery0 information from the driver
    kern_return_t TPerUpdate(io_registry_entry_t driverService, DTA_DEVICE_INFO * pdi);

// Low-level functions

// Driver info cache update
    kern_return_t updatePropertiesInIORegistry(io_connect_t connect);

//  Access the connected SCSI device
//  to send SCSI commands to an attached SCSI TPer, or
//  to send SCSI-wrapped AT commands to a SAT TPer
//
    kern_return_t PerformSCSICommand(io_connect_t connect,
                                     SCSICommandDescriptorBlock cdb,
                                     const void * buffer,
                                     const uint64_t bufferSize,
                                     const uint64_t requestedTransferLength,
                                     uint64_t *pLengthActuallyTransferred);

#if defined (__cplusplus)
}
#endif

#endif  // #ifndef TPerKernelInterface_h
