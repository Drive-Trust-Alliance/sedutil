//
//  TPerKernelInterface.c
//  TPerKernelInterface
//
//  Created by Jackie Marks on 9/21/15.
//  Copyright © 2015 Bright Plaza Inc. All rights reserved.
//

#include "TPerKernelInterface.h"

#include "PrintBuffer.h"
#include <unistd.h>
#define _POSIX_SOURCE
#include <sys/stat.h>
#include "ATAStructures.h"


kern_return_t SCSIPassThroughInterface(io_connect_t connect,
                                       SCSICommandDescriptorBlock cdb,
                                       const void * buffer,
                                       const uint64_t * pBufferLen)
{
    uint64_t	input[4];

    // *** check arguments
 
    // check for NULL pointers
    if ( buffer == NULL )
        return kIOReturnBadArgument;
    
    if ( pBufferLen == NULL )
        return kIOReturnBadArgument;
    
    // check buffer alignment
    if ( 0 != ((uint64_t)buffer & (uint64_t)(getpagesize()-1) ) )
        return kIOReturnNotAligned;

    // check for inconsistent cdb
    // add more conditions as we need them

    uint8_t protocol = (cdb[1] >> 1) & 0x0f ;
    uint8_t command = cdb[9];

    switch (command) {
        case IDENTIFY:
            if ( protocol != (uint8_t)PIO_DATA_IN )     // IDENTIFY -- PIO-DataIn  (userland read)
                return kIOReturnBadArgument;
            if ( cdb[2] != 0x0E )                       // T_DIR = 1, BYTE_BLOCK = 1, Length in Sector Count
                return kIOReturnBadArgument;
            break;
                
        case IF_RECV:
            if ( protocol != (uint8_t)PIO_DATA_IN )     // IF_RECV  -- PIO-DataIn  (userland read)
                return kIOReturnBadArgument;
            if ( cdb[2] != 0x0E )                       // T_DIR = 1, BYTE_BLOCK = 1, Length in Sector Count
                return kIOReturnBadArgument;
            break;

        case IF_SEND:
            if ( protocol != (uint8_t)PIO_DATA_OUT )    // IF_SEND  -- PIO-DataOut (userland write)
                return kIOReturnBadArgument;
            if ( cdb[2] != 0x06 )                       // T_DIR = 0, BYTE_BLOCK = 1, Length in Sector Count
                return kIOReturnBadArgument;
            break;

        default:
            return kIOReturnBadArgument;
    }
    
    assert( sizeof( SCSICommandDescriptorBlockAsTwoQuads ) == sizeof( SCSICommandDescriptorBlock ) ) ;
    
    *( SCSICommandDescriptorBlockAsTwoQuads *)(& input[0] )=
        *( SCSICommandDescriptorBlockAsTwoQuads *)cdb ; // HACKATRONIC!
    
    assert( sizeof( SCSICommandDescriptorBlockAsTwoQuads ) == 2 * sizeof( input[0])) ;

    
    input[2] = (const uint64_t)buffer;                     // void * == uint64_t
    input[3] = (const uint64_t)*pBufferLen;                 // size_t == uint64_t
    
#if DEBUG
    
//    printf("\nTPerKernelInterface::%s(cdb = %16p)\n", __FUNCTION__, (void *)cdb);
//    printBuffer(cdb, sizeof( SCSICommandDescriptorBlock));
//
//    printf("%s(buffer = %16p , &bufferLen = %16p -> %llu)\n\n\n",
//           __FUNCTION__, buffer,  (void *)pBufferLen, *pBufferLen);
//
//    printBuffer(buffer, *pBufferLen);
//    printf("\n\n\n");
//
//    fflush(stdout);

#endif
    
    kern_return_t kernResult =
        IOConnectCallScalarMethod(connect,					      // an io_connect_t returned from IOServiceOpen().
                                  kSedUserClientSCSIPassThrough,  // selector of the function to be called via the user client.
                                  input,                          // input scalar parameters.
                                  4,                              // number of scalar input parms.
                                  NULL,                           // output scalar parameters.
                                  0);                             // pointer to the number of output scalar parms.

#if DEBUG
//    printf("%s(buffer = %16p , pBufferLen = %16p -> %llu)\n",
//           __FUNCTION__, buffer,  (void *)pBufferLen, (long long)*pBufferLen);
//    if ( buffer ) printBuffer(buffer, *pBufferLen);
//    printf("%s(kernResult = %d = 0x%X)\n\n",
//           __FUNCTION__, kernResult, (unsigned)kernResult);
#endif
    return kernResult;
}

kern_return_t OpenUserClient(io_service_t service, io_connect_t *pConnect)
{
    io_connect_t connect;
    
    // This call will cause the user client to be instantiated. It returns an io_connect_t handle
    // that is used for all subsequent calls to the user client.
//#if DEBUG
//    fprintf(stderr, "OpenUserClient -- service is %d.\n", service);
//#endif
    kern_return_t kernResult = IOServiceOpen(service, mach_task_self(), 0, &connect);
    *pConnect = connect;
    if (kernResult != kIOReturnSuccess) {
        fprintf(stderr, "OpenUserClient: error -- IOServiceOpen returned 0x%08x\n", kernResult);
    }
    else {
    // This calls the openUserClient method in SedUserClient inside the kernel.
//#if DEBUG
//        fprintf(stderr, "OpenUserClient IOServiceOpen successful -- connect is %d (%d).\n", connect, *pConnect);
//#endif
        kernResult = IOConnectCallScalarMethod(*pConnect, kSedUserClientOpen, NULL, 0, NULL, NULL);
//#if DEBUG
//        if (kernResult == kIOReturnSuccess) {
//            fprintf(stderr, "OpenUserClient IOConnectCallScalarMethod successful.\n");
//        }
//        else {
//            fprintf(stderr, "OpenUserClient error -- IOConnectCallScalarMethod returned 0x%08x.\n\n", kernResult);
//        }
//#endif
    }
    return kernResult;
}


kern_return_t CloseUserClient(io_connect_t connect)
{
    // This calls the closeUserClient method in SedUserClient inside the kernel, which in turn closes
    // the driver.
    kern_return_t	SedUserClientCloseResult;
    kern_return_t	IOServiceCloseResult;
//#if DEBUG
//    fprintf(stderr, "CloseUserClient -- connect is %d.\n", connect);
//#endif
    SedUserClientCloseResult = IOConnectCallScalarMethod(connect, kSedUserClientClose, NULL, 0, NULL, NULL);
//#if DEBUG
//    fprintf(stderr, "CloseUserClient -- SedUserClientCloseResult is %d.\n", SedUserClientCloseResult);
//#endif
    IOServiceCloseResult = IOServiceClose(connect);  // releases connect
//#if DEBUG
//    fprintf(stderr, "CloseUserClient -- IOServiceCloseResult is %d.\n", IOServiceCloseResult);
//#endif
    return SedUserClientCloseResult ? SedUserClientCloseResult : IOServiceCloseResult;
}

// ************
// *** TCG functions
// ************

void IdentifyDevice(io_registry_entry_t driverService, UInt8 * buffer) {
    
    CFDataRef data = IORegistryEntryCreateCFProperty(driverService,
                                                     CFSTR( IOIdentifyDeviceResponseKey ),
                                                     kCFAllocatorDefault, 0);
    
    CFDataGetBytes(data, CFRangeMake(0,CFDataGetLength(data)), buffer);
    
    CFRelease(data);
    
}



bool Discovery0(io_registry_entry_t driverService, UInt8 * buffer) {

    CFDataRef data = (CFDataRef)IORegistryEntryCreateCFProperty(driverService,
                                                                CFSTR(IODiscovery0ResponseKey),
                                                                CFAllocatorGetDefault(), 0);
    
    if ( data == NULL )
        return false;
    CFDataGetBytes(data, CFRangeMake(0,CFDataGetLength(data)), buffer);
    CFRelease(data);
    return true;
    
}


kern_return_t updateLockingPropertiesInIORegistry(io_connect_t connect) {
    return IOConnectCallScalarMethod(connect, kSedUserClientUpdateLockingPropertiesInIORegistry, NULL, 0, NULL, NULL);
}
