//
//  TPerKernelInterface.c
//  TPerKernelInterface
//
//  Created by Jackie Marks on 9/21/15.
//  Copyright © 2015 Bright Plaza Inc. All rights reserved.
//

#include "TPerKernelInterface.h"

#include <unistd.h>
#define _POSIX_SOURCE
#include <sys/stat.h>
#include "InterfaceCommandCodes.h"


// ************
// *** TCG functions
// ************

kern_return_t PerformSCSICommand(io_connect_t connect,
                                 SCSICommandDescriptorBlock cdb,
                                 const void * buffer,
                                 const uint64_t bufferLen,
                                 const uint64_t requestedTransferLength,
                                 uint64_t *pActualTransferLength)
{

    // *** check arguments
    if (connect == IO_OBJECT_NULL)
        return kIOReturnBadArgument;
    
    // check for NULL pointers
    if ( NULL==buffer || 0==bufferLen )
        return kIOReturnBadArgument;
    
    enum IODirection  // TODO: import this, a la ATAProtocolDirections.cpp
    {
        kIODirectionNone  = 0x0,//                    same as VM_PROT_NONE
        kIODirectionIn    = 0x1,// User land 'read',  same as VM_PROT_READ
        kIODirectionOut   = 0x2,// User land 'write', same as VM_PROT_WRITE
        kIODirectionOutIn = kIODirectionOut | kIODirectionIn,
        kIODirectionInOut = kIODirectionIn  | kIODirectionOut,

        // these flags are valid for the prepare() method only
        kIODirectionPrepareToPhys32   = 0x00000004,
        kIODirectionPrepareNoFault    = 0x00000008,
        kIODirectionPrepareReserved1  = 0x00000010,
    #define IODIRECTIONPREPARENONCOHERENTDEFINED    1
        kIODirectionPrepareNonCoherent = 0x00000020,

        // these flags are valid for the complete() method only
    #define IODIRECTIONCOMPLETEWITHERRORDEFINED             1
        kIODirectionCompleteWithError = 0x00000040,
    #define IODIRECTIONCOMPLETEWITHDATAVALIDDEFINED 1
        kIODirectionCompleteWithDataValid = 0x00000080,
    } direction = kIODirectionNone;

    if ( 0 != ((uint64_t)buffer & (uint64_t)(sysconf(_SC_PAGESIZE)-1) ) )
        return kIOReturnNotAligned;
    // check for inconsistent cdb
    // add more conditions as we need them
    switch (cdb[0]) {
        case kSCSICmd_ATA_PASS_THROUGH: // ATA PASS-THROUGH
            {
                const uint8_t ATACommand = cdb[9];
                const uint8_t protocol = (cdb[1] >> 1) & 0x0f ;
                switch (ATACommand) {
                    case kATACmd_IDENTIFY_DEVICE: // IDENTIFY
                    case kATACmd_TRUSTED_RECEIVE_PIO: // TRUSTED RECEIVE
                        if (protocol != PIO_DataIn)
                            return kIOReturnBadArgument;
                        direction = kIODirectionIn;
                        break;
                    case kATACmd_TRUSTED_SEND_PIO: // TRUSTED SEND
                        if (protocol != PIO_DataOut)
                            return kIOReturnBadArgument;
                        direction = kIODirectionOut;
                        break;
                    default:
                        return kIOReturnBadArgument;
                }
            }
            break;

        case kSCSICmd_INQUIRY: // INQUIRY
        case kSCSICmd_SECURITY_PROTOCOL_IN:     // SECURITY PROTOCOL IN
            direction = kIODirectionIn;
            break;

        case kSCSICmd_SECURITY_PROTOCOL_OUT:    // SECURITY PROTOCOL OUT
            direction = kIODirectionOut;
            break;
        default:
            return kIOReturnBadArgument;
    }

    {
        uint64_t    input[6];
        uint64_t   output[1];
        assert( sizeof( SCSICommandDescriptorBlockAsTwoQuads ) == sizeof( SCSICommandDescriptorBlock ) ) ;
        assert( sizeof( SCSICommandDescriptorBlockAsTwoQuads ) == 2 * sizeof( input[0])) ;

        *( SCSICommandDescriptorBlockAsTwoQuads *)(& input[0] )=
            *( SCSICommandDescriptorBlockAsTwoQuads *)cdb ; // HACKATRONIC!
        input[2] = (const uint64_t)buffer;                  // void * == uint64_t
        input[3] = (const uint64_t)bufferLen;               // size_t == uint64_t
        input[4] = (const uint64_t)direction;               // size_t == uint64_t
        input[5] = (const uint64_t)requestedTransferLength; // size_t == uint64_t

        uint32_t outputCount = 1;
        kern_return_t kernResult =
            IOConnectCallScalarMethod(connect,					      // an io_connect_t returned from IOServiceOpen().
                                      kSedUserClientPerformSCSICommand,  // selector of the function to be called via the user client.
                                      input,                          // input scalar parameters.
                                      6,                              // number of scalar input parms.
                                      output,                         // output scalar parameters.
                                      &outputCount);                  // pointer to number of output scalar parms.
        
        if (1 <= outputCount) {
            *pActualTransferLength = output[0];
        }
        return kernResult;
    }
}



kern_return_t updatePropertiesInIORegistry(io_connect_t connect) {
    return IOConnectCallScalarMethod(connect, kSedUserClientUpdatePropertiesInIORegistry, NULL, 0, NULL, NULL);
}

kern_return_t TPerUpdate(io_connect_t connect, void /* DTA_DEVICE_INFO */ * pdi) {
    kern_return_t ret = updatePropertiesInIORegistry(connect);
    if (ret == KERN_SUCCESS && pdi != NULL ) {
        CFDataRef data = (CFDataRef)IORegistryEntryCreateCFProperty(connect,
                                                                    CFSTR(IODtaDeviceInfoKey),
                                                                    CFAllocatorGetDefault(), 0);
        if ( data == NULL )
            return KERN_FAILURE;
        
        CFDataGetBytes(data, CFRangeMake(0,CFDataGetLength(data)), pdi);
        
        CFRelease(data);
    }
    return ret;
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
