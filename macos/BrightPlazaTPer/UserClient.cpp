//
//  TPerUserClient.cpp
//  SedUserCLient
//
//  Created by Jackie Marks on 9/21/15.
//  Copyright © 2022 Bright Plaza Inc. All rights reserved.
//

#include <IOKit/IOBufferMemoryDescriptor.h>

#include "UserClient.h"
#include "CDBAccess.hpp"

#include "kernel_debug.h"
#include "kernel_PrintBuffer.h"
#include "TPerDriverMethodIndex.h"

#define super IOUserClient


// Even though we are defining the convenience macro super for the superclass, you must use the actual class name
// in the OS*MetaClass macros.

OSDefineMetaClassAndStructors(com_brightplaza_BrightPlazaTPerUserClient, IOUserClient)

// User client method dispatch table.
//
// The user client mechanism is designed to allow calls from a user process to be dispatched to
// any IOService-based object in the kernel. Almost always this mechanism is used to dispatch calls to
// either member functions of the user client itself or of the user client's provider. The provider is
// the driver which the user client is connecting to the user process.
//
// It is recommended that calls be dispatched to the user client and not directly to the provider driver.
// This allows the user client to perform error checking on the parameters before passing them to the driver.
// It also allows the user client to do any endian-swapping of parameters in the cross-endian case.



#define __TPerDriverMethod__(index, method, nScalarInputs, nStructInputs, nScalarOutputs, nStructOutputs) \
{(IOExternalMethodAction) &UserClientClass::method, nScalarInputs, nStructInputs, nScalarOutputs, nStructOutputs},
const IOExternalMethodDispatch UserClientClass::sMethods[kSedUserClientNumberOfMethods] = {
#include "SEDKernelInterface/TPerDriverDispatchTable.inc"
};
#undef __TPerDriverMethod__

IOReturn UserClientClass::externalMethod(uint32_t selector, IOExternalMethodArguments* arguments,
                                                IOExternalMethodDispatch* dispatch, OSObject* target, void* reference)

{
    char tagstring[128];
    snprintf( tagstring, sizeof(tagstring), "%s[%p]::%s", getName(), this, __FUNCTION__);
    IOLOG_DEBUG("%s(%d, %p, %p, %p, %p)\n", tagstring,
		  selector, arguments, dispatch, target, reference);
        
    if (selector < (uint32_t) kSedUserClientNumberOfMethods) {
        dispatch = (IOExternalMethodDispatch *) &sMethods[selector];
        
        if (!target) {
            target = this;
		}
    }
        
	return super::externalMethod(selector, arguments, dispatch, target, reference);
}

//*****************
// SCSI Passthrough Driver specific methods
//*****************

IOReturn UserClientClass::sUserClientPerformSCSICommand(UserClientClass* target,
                                                     void* reference __unused,
                                                     IOExternalMethodArguments* arguments)
{
    IOSleep(1);
    IOLOG_DEBUG("BrightPlazaTPer User Client:: in static SCSI Passthrough\n");
    return target->userClientPerformSCSICommand(*((SCSICommandDescriptorBlock * )(void *)&arguments->scalarInput[0]),  // cdb
                                                (void *)arguments->scalarInput[2],
                                                arguments->scalarInput[3],
                                                (IODirection)arguments->scalarInput[4],
                                                arguments->scalarInput[5],
                                                &arguments->scalarOutput[0]);
}

IOReturn UserClientClass::userClientPerformSCSICommand(SCSICommandDescriptorBlock cdb,
                                                       void * userBuffer,
                                                       const uint64_t userBufferSize,
                                                       IODirection direction,
                                                       uint64_t requiredTransferLength,
                                                       uint64_t *pLengthActuallyTransferred)
{
    char tagstring[128];
    
    uint64_t bS = userBufferSize;
    uint64_t * pBufferSize = &bS;
    snprintf( tagstring, sizeof(tagstring), "%s[%p]::%s", getName(), this, __FUNCTION__);
    
    IOSleep(1);
    IOLOG_DEBUG("%s(buffer = %16p, pBufferSize = %16p -> %llu) -- fProvider=0x%016lx",
          tagstring,
          userBuffer, pBufferSize, (pBufferSize) ? *pBufferSize : 0 ,
          (unsigned long)(void *)fProvider );
    IOSleep(1);

    uint64_t allocatedBufferLength = *pBufferSize    ;
    IOSleep(1);
    IOLOG_DEBUG("%s(direction=0x%08X requiredTransferLength = %llu, allocatedBufferLength = %llu)",
                tagstring, direction, requiredTransferLength, allocatedBufferLength);
    IOSleep(1);

    // check that driver is ready and arguments are valid
    if (fProvider == NULL || isInactive()) {
        IOSleep(1);
        IOLOG_DEBUG("%s - fProvider NULL or inactive", tagstring);
        IOSleep(1);
        return kIOReturnNotAttached;
    }
    
    if (!(fProvider->isOpen(this))) {
        IOSleep(1);
        IOLOG_DEBUG("%s - fProvider not open", tagstring);
        IOSleep(1);
        return kIOReturnNotOpen;
    }
    
    if ( NULL == userBuffer) {  // check buffer
        IOSleep(1);
        IOLOG_DEBUG("%s - No buffer provided", tagstring);
        IOSleep(1);
        return kIOReturnNoSpace ;
    }
    
    if (allocatedBufferLength < requiredTransferLength ) {  // check buffer length
        IOSleep(1);
        IOLOG_DEBUG("%s - Allocated buffer length %llu less than required buffer length %llu",
                    tagstring, allocatedBufferLength, requiredTransferLength);
        IOSleep(1);
        return kIOReturnNoSpace ;
    }

    IOSleep(1);
    IOLOG_DEBUG( "%s - requiredBufferLength is %llu, direction is 0x%08X",
                tagstring, requiredTransferLength, direction );
    IOSleep(1);

    IOBufferMemoryDescriptor * md = IOBufferMemoryDescriptor::withCapacity( requiredTransferLength, direction, false);
    if ( !md ) {
        IOSleep(1);
        IOLOG_DEBUG( "%s - no md - data BufferDescriptor error", tagstring);
        IOSleep(1);
        return kIOReturnVMError;
    }
    
    UInt8 * kernelBuffer = ( UInt8 * ) md->getBytesNoCopy();
    IOByteCount kernelBufferLength = md->getLength();
    bzero( kernelBuffer, kernelBufferLength );


    IOSleep(1);
    IOLOG_DEBUG("%s md=%16p kernelBuffer=%16p kernelBufferLength=%llu",
                tagstring, md, kernelBuffer, kernelBufferLength);
    IOSleep(1);

    if (direction == kIODirectionOut) {       // userland write
        IOSleep(1);
        IOLOG_DEBUG("%s copyin uaddr=%16p kaddr=%16p len=%llu",
                    tagstring, userBuffer, kernelBuffer, kernelBufferLength);
        IOSleep(1);
        (void)copyin((const user_addr_t) userBuffer, (void *)kernelBuffer, requiredTransferLength);     // into kernel
    }

    IOSleep(1);
    IOLOG_DEBUG("%s - fProvider open; about to call driver SCSIPassThrough code\n", tagstring);
    IOSleep(1);
    
    
    uint64_t lengthActuallyTransferred = requiredTransferLength;
    IOReturn result = fProvider->PerformSCSICommand(cdb, md, &lengthActuallyTransferred);
    *pLengthActuallyTransferred = lengthActuallyTransferred;
    
    
    
    IOSleep(1);
    IOLOG_DEBUG("%s - result is 0x%08X requiredBufferLength=%llu", tagstring, result, requiredTransferLength);
    IOSleep(1);

    if (direction == kIODirectionIn) {      // userland read
        (void)copyout( (const void *) kernelBuffer, (user_addr_t)userBuffer, requiredTransferLength );  // out of kernel
        IOLOG_DEBUG("%s SCSI response is", tagstring);
// TODO: BOOM        IOLOGBUFFER_DEBUG(tagstring, userBuffer, requiredBufferLength);
    }
    
    md->release();

    
    IOSleep(1);
    IOLOG_DEBUG("%s(userBuffer = %16p, pBufferSize = %16p -> %llu) -- fProvider=0x%016lx\n",
                tagstring, userBuffer, pBufferSize, (pBufferSize) ? *pBufferSize : 0 ,
                (unsigned long)(void *)fProvider );
    IOSleep(1);

    return result;


}

#if DEBUG
#define IOLOG_DEBUG_IOExternalMethodArguments(arguments) IOLOG_IOExternalMethodArguments(__FUNCTION__,arguments)
void IOLOG_IOExternalMethodArguments(const char * fnname, IOExternalMethodArguments * arguments);
void IOLOG_IOExternalMethodArguments(const char * fnname, IOExternalMethodArguments * arguments)
{
    IOLOG_DEBUG("In SCOTT %s", fnname);
    
    IOLOG_DEBUG("version=%d\n",arguments->version);
    
    IOLOG_DEBUG("selector%d\n", arguments->selector);
    
    IOLOG_DEBUG("asyncWakePort=0x%016lX\n", (size_t)(void *)(arguments->asyncWakePort));
    
    IOLOG_DEBUG("asyncReference=0x%016lX\n", (size_t)(void *)(arguments->asyncReference));
    IOLOG_DEBUG("asyncReferenceCount=%d\n", arguments->asyncReferenceCount);

    IOLOG_DEBUG("scalarInput=0x%016lX\n", (size_t)(void *)(arguments->scalarInput));
    IOLOG_DEBUG("scalarInputCount=%d\n", arguments->scalarInputCount);
    
    for (uint32_t i=0; i<arguments->scalarInputCount; i++) {
        IOLOG_DEBUG("scalarInput[%d]=0x%016llX\n", i, (arguments->scalarInput)[i]);
    }
    
    IOLOG_DEBUG("structureInput=0x%016lX\n", (size_t)(void *)(arguments->structureInput));
    IOLOG_DEBUG("structureInputCount=%d\n", arguments->structureInputSize);
    IOLOG_DEBUG("structureInputDescriptor=0x%016lX\n", (size_t)(void *)(arguments->structureInputDescriptor));

    IOLOG_DEBUG("scalarOutput=0x%016lX\n", (size_t)(void *)(arguments->scalarOutput));
    IOLOG_DEBUG("scalarOutputCount=%d\n", arguments->scalarOutputCount);
    IOLOG_DEBUG("structureOutput=0x%016lX\n", (size_t)(void *)(arguments->structureOutput));
    IOLOG_DEBUG("structureOutputCount=%d\n", arguments->structureOutputSize);
    IOLOG_DEBUG("structureOutputDescriptor=0x%016lX\n", (size_t)(void *)(arguments->structureOutputDescriptor));

    IOLOG_DEBUG("structureVariableOutputData=0x%016lX\n", (size_t)(void *)(arguments->structureVariableOutputData));
}
#else // !DEBUG
#define IOLOG_DEBUG_IOExternalMethodArguments(arguments) do {} while(0)
#endif


IOReturn UserClientClass::sUserClientUpdatePropertiesInIORegistry(UserClientClass* target,
                                                                 void* reference __unused,
                                                                 IOExternalMethodArguments* arguments __unused)
{
    return target->UserClientUpdatePropertiesInIORegistry();
}

IOReturn UserClientClass::UserClientUpdatePropertiesInIORegistry( void )
{
    IOReturn ret;
    char tagstring[128];
    snprintf( tagstring, sizeof(tagstring), "%s[%p]::%s", getName(), this, __FUNCTION__);
    IOLOG_DEBUG("%s -- fProvider=0x%016lx\n",
                tagstring, (unsigned long)(void *)fProvider );
    
    
    // check that driver is ready and arguments are valid
    if (fProvider == NULL || isInactive()) {
        IOLOG_DEBUG("%s - fProvider NULL or inactive",
                    tagstring);
        return kIOReturnNotAttached;
    }
    
    if (!(fProvider->isOpen(this))) {
        IOLOG_DEBUG("%s - fProvider not open",
                    tagstring);
        return kIOReturnNotOpen;
    }
    
    ret = fProvider->updatePropertiesInIORegistry();
    
    IOLOG_DEBUG("%s -- after driver function", tagstring );
    return ret;
}




// **********
// *** UserClient methods
// **********

IOReturn UserClientClass::sOpenUserClient(UserClientClass* target,
                                          void* reference __unused,
                                          IOExternalMethodArguments* arguments __unused)
{
    return target->openUserClient();
}

IOReturn UserClientClass::openUserClient(void)
{
    char tagstring[128];
    snprintf( tagstring, sizeof(tagstring), "%s[%p]::%s", getName(), this, __FUNCTION__);

    IOReturn	result = kIOReturnSuccess;
	
	IOLOG_DEBUG("%s() -- fProvider=0x%016lx\n", tagstring,
          (unsigned long)(void *)fProvider);
    
    if (fProvider == NULL || isInactive()) {
		// Return an error if we don't have a provider. This could happen if the user process
		// called openUserClient without calling IOServiceOpen first. Or, the user client could be
		// in the process of being terminated and is thus inactive.
        result = kIOReturnNotAttached;
	}
    else if (!fProvider->open(this)) {
		// The most common reason this open call will fail is because the provider is already open
		// and it doesn't support being opened by more than one client at a time.
		result = kIOReturnExclusiveAccess;
	}
        
    IOLOG_DEBUG("%s() => 0x%08x\n", tagstring, result);
    return result;
}


IOReturn UserClientClass::sCloseUserClient(UserClientClass* target,
                                           void* reference __unused,
                                           IOExternalMethodArguments* arguments __unused)
{
    IOReturn ret = target->closeUserClient();
    if ( kIOReturnSuccess != ret) {
        IOLOG_DEBUG("%s[%p]::%s]closeUserClient failed, returned 0x%08x\n", target->getName(), target, __FUNCTION__, ret);
    }
    return ret;
}


IOReturn UserClientClass::closeUserClient(void)
{
    char tagstring[128];
    snprintf( tagstring, sizeof(tagstring), "%s[%p]::%s", getName(), this, __FUNCTION__);
	IOLOG_DEBUG("%s()\n", tagstring);
    
    IOReturn	result = kIOReturnSuccess;
    
    if (fProvider == NULL) {
		// Return an error if we don't have a provider. This could happen if the user process
		// called closeUserClient without calling IOServiceOpen first. 
		result = kIOReturnNotAttached;
		IOLOG_DEBUG("%s(): returning kIOReturnNotAttached.\n", tagstring);
	}
	else if (fProvider->isOpen(this)) {
		// Make sure we're the one who opened our provider before we tell it to close.
        IOLOG_DEBUG("%s(): calling fProvider->close(%p).\n", tagstring, this);
		fProvider->close(this);
	}
	else {
		result = kIOReturnNotOpen;
		IOLOG_DEBUG("%s(): returning kIOReturnNotOpen.\n", tagstring);
	}
	
    return result;
}

//*****************
// apple UserClient methods
//*****************

// initWithTask is called as a result of the user process calling IOServiceOpen.
bool UserClientClass::initWithTask(task_t owningTask, void* securityToken, UInt32 type)
{
    bool success = super::initWithTask(owningTask, securityToken, type);
    
    // This IOLOG_DEBUG must follow super::initWithTask because getName relies on the superclass initialization.
    IOLOG_DEBUG("%s[%p]::%s](%p, %p, %u)\n", getName(), this, __FUNCTION__, owningTask, securityToken, (unsigned int)type);
    
    fTask = owningTask;
    fProvider = NULL;
    
    return success;
}


// start is called after initWithTask as a result of the user process calling IOServiceOpen.
bool UserClientClass::start(IOService* provider)
{
    char tagstring[128];
    snprintf( tagstring, sizeof(tagstring), "%s[%p]::%s", getName(), this, __FUNCTION__);

    bool	success;
    
    IOLOG_DEBUG("%s(%p)\n", tagstring, provider);
    
    // Verify that this user client is being started with a provider that it knows
    // how to communicate with.
    fProvider = OSDynamicCast(DriverClass, provider);
    success = (fProvider != NULL);
    
    if (success) {
        // It's important not to call super::start if some previous condition
        // (like an invalid provider) would cause this function to return false.
        // I/O Kit won't call stop on an object if its start function returned false.
        success = super::start(provider);
    }
    
    return success;
}


// clientClose is called as a result of the user process calling IOServiceClose.
IOReturn UserClientClass::clientClose(void)
{
    char tagstring[128];
    snprintf( tagstring, sizeof(tagstring), "%s[%p]::%s", getName(), this, __FUNCTION__);

    IOLOG_DEBUG("%s()\n", tagstring);
    
    // Defensive coding in case the user process called IOServiceClose
    // without calling closeUserClient first.
    (void) closeUserClient();
    
    // Inform the user process that this user client is no longer available. This will also cause the
    // user client instance to be destroyed.
    //
    // terminate would return false if the user process still had this user client open.
    // This should never happen in our case because this code path is only reached if the user process
    // explicitly requests closing the connection to the user client.
    bool success = terminate();
    if (!success) {
        IOLOG_DEBUG("%s(): terminate() failed.\n", tagstring);
    }
    
    // DON'T call super::clientClose, which just returns kIOReturnUnsupported.
    
    return kIOReturnSuccess;
}

//*****************
// apple UserClient methods
//*****************

// didTerminate is called at the end of the termination process. It is a notification
// that a provider has been terminated, sent after recursing up the stack, in leaf-to-root order.
bool UserClientClass::didTerminate(IOService* provider, IOOptionBits options, bool* defer)
{
    char tagstring[128];
    snprintf( tagstring, sizeof(tagstring), "%s[%p]::%s", getName(), this, __FUNCTION__);

    
    IOLOG_DEBUG("%s(%p, %u, %p)\n", tagstring, provider, (unsigned int)options, defer);
    
    // If all pending I/O has been terminated, close our provider. If I/O is still outstanding, set defer to true
    // and the user client will not have stop called on it.
    kern_return_t ret = closeUserClient();
    if (kIOReturnSuccess != ret) {
        IOLOG_DEBUG("%s -- closeUserClient returned 0x%08x\n", tagstring, ret);
    }

    *defer = false;
    
    return super::didTerminate(provider, options, defer);
}

//*****************
// apple UserClient methods
//          not overriden except debug messages for program flow
//*****************

void UserClientClass::stop(IOService* provider)
{
    char tagstring[128];
    snprintf( tagstring, sizeof(tagstring), "%s[%p]::%s", getName(), this, __FUNCTION__);
    IOLOG_DEBUG("%s(%p)\n", tagstring, provider);
    
    super::stop(provider);
}


bool UserClientClass::terminate(IOOptionBits options)
{
    char tagstring[128];
    snprintf( tagstring, sizeof(tagstring), "%s[%p]::%s", getName(), this, __FUNCTION__);
    IOLOG_DEBUG("%s(%u)\n", tagstring, (unsigned int)options);
    
    return super::terminate(options);
}


bool UserClientClass::finalize(IOOptionBits options)
{
    char tagstring[128];
    snprintf( tagstring, sizeof(tagstring), "%s[%p]::%s", getName(), this, __FUNCTION__);
    IOLOG_DEBUG("%s(%u)\n", tagstring, (unsigned int)options);
    
    return super::finalize(options);
}

// override if alternate action if the user process crashes instead of exiting normally.
IOReturn UserClientClass::clientDied(void)
{
    char tagstring[128];
    snprintf( tagstring, sizeof(tagstring), "%s[%p]::%s", getName(), this, __FUNCTION__);
    
    IOLOG_DEBUG("%s()\n", tagstring);
    
    // default implementation just calls clientClose.
    return super::clientDied();
}


// willTerminate is called at the beginning of the termination process. It is a notification
// that a provider has been terminated, sent before recursing up the stack, in root-to-leaf order.
//
// This is where any pending I/O should be terminated. At this point the user client has been marked
// inactive and any further requests from the user process should be returned with an error.
bool UserClientClass::willTerminate(IOService* provider, IOOptionBits options)
{
    char tagstring[128];
    snprintf( tagstring, sizeof(tagstring), "%s[%p]::%s", getName(), this, __FUNCTION__);

    IOLOG_DEBUG("%s(%p, %u)\n", tagstring, provider, (unsigned int)options);
    
    return super::willTerminate(provider, options);
}



