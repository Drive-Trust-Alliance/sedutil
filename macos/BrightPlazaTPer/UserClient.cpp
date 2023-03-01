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


#if DRIVER_DEBUG
#define UC_IOLOG_DEBUG IOLOG_DEBUG
#define UC_IOLOG_DEBUG_METHOD IOLOG_DEBUG_METHOD
#else // !defined(DEBUG_USERCLIENT)
#define UC_IOLOG_DEBUG(...) do ; while (0)
#define UC_IOLOG_DEBUG_METHOD(...) do ; while (0)
#endif // DRIVER_DEBUG

IOReturn UserClientClass::externalMethod(uint32_t selector, IOExternalMethodArguments* arguments,
                                                IOExternalMethodDispatch* dispatch, OSObject* target, void* reference)

{
    UC_IOLOG_DEBUG_METHOD("(%d, %p, %p, %p, %p)",
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
    UC_IOLOG_DEBUG(kUserClientClass "::sUserClientPerformSCSICommand(" REVEALFMT ", %p, %p)", REVEAL(target), reference, arguments);
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
    uint64_t bS = userBufferSize;
    uint64_t * pBufferSize = &bS;
    IOSleep(1);
    UC_IOLOG_DEBUG_METHOD("(buffer = %16p, pBufferSize = %16p -> %llu) -- fProvider=0x%016lx",
          userBuffer, pBufferSize, (pBufferSize) ? *pBufferSize : 0 ,
          (unsigned long)(void *)fProvider );
    IOSleep(1);

    uint64_t allocatedBufferLength = *pBufferSize    ;
    IOSleep(1);
    UC_IOLOG_DEBUG_METHOD("(direction=0x%08X requiredTransferLength = %llu, allocatedBufferLength = %llu)",
                        direction, requiredTransferLength, allocatedBufferLength);
    IOSleep(1);

    // check that driver is ready and arguments are valid
    if (fProvider == NULL || isInactive()) {
        IOSleep(1);
        IOLOG_DEBUG_METHOD(" fProvider NULL or inactive");
        IOSleep(1);
        return kIOReturnNotAttached;
    }

    if (!(fProvider->isOpen(this))) {
        IOSleep(1);
        IOLOG_DEBUG_METHOD(" fProvider not open");
        IOSleep(1);
        return kIOReturnNotOpen;
    }

    if ( NULL == userBuffer) {  // check buffer
        IOSleep(1);
        IOLOG_DEBUG_METHOD(" No buffer provided");
        IOSleep(1);
        return kIOReturnNoSpace ;
    }

    if (allocatedBufferLength < requiredTransferLength ) {  // check buffer length
        IOSleep(1);
        IOLOG_DEBUG_METHOD(" Allocated buffer length %llu less than required buffer length %llu",
                           allocatedBufferLength, requiredTransferLength);
        IOSleep(1);
        return kIOReturnNoSpace ;
    }

    IOSleep(1);
    UC_IOLOG_DEBUG_METHOD(" requiredBufferLength is %llu, direction is 0x%08X",
                           requiredTransferLength, direction );
    IOSleep(1);

    IOBufferMemoryDescriptor * md = IOBufferMemoryDescriptor::withCapacity( requiredTransferLength, direction, false);
    if ( !md ) {
        IOSleep(1);
        IOLOG_DEBUG_METHOD(" no md - data BufferDescriptor error");
        IOSleep(1);
        return kIOReturnVMError;
    }

    UInt8 * kernelBuffer = ( UInt8 * ) md->getBytesNoCopy();
    IOByteCount kernelBufferLength = md->getLength();
    bzero( kernelBuffer, kernelBufferLength );


    IOSleep(1);
    UC_IOLOG_DEBUG_METHOD(" md=%16p kernelBuffer=%16p kernelBufferLength=%llu",
                           md, kernelBuffer, kernelBufferLength);
    IOSleep(1);

    if (direction == kIODirectionOut) {       // userland write
        IOSleep(1);
        UC_IOLOG_DEBUG_METHOD(" copyin uaddr=%16p kaddr=%16p len=%llu",
                                userBuffer, kernelBuffer, kernelBufferLength);
        IOSleep(1);
        (void)copyin((const user_addr_t) userBuffer, (void *)kernelBuffer, requiredTransferLength);     // into kernel
    }

    IOSleep(1);
    UC_IOLOG_DEBUG_METHOD(" fProvider open; about to call driver SCSIPassThrough code");
    IOSleep(1);


    uint64_t lengthActuallyTransferred = requiredTransferLength;
    IOReturn result = fProvider->PerformSCSICommand(cdb, md, &lengthActuallyTransferred);
    *pLengthActuallyTransferred = lengthActuallyTransferred;



    IOSleep(1);
    UC_IOLOG_DEBUG_METHOD(" result is 0x%08X requiredBufferLength=%llu", result, requiredTransferLength);
    IOSleep(1);

    if (direction == kIODirectionIn) {      // userland read
        (void)copyout( (const void *) kernelBuffer, (user_addr_t)userBuffer, requiredTransferLength );  // out of kernel
        UC_IOLOG_DEBUG_METHOD(" SCSI response is");
// TODO: BOOM        IOLOGBUFFER_DEBUG(tagstring, userBuffer, requiredBufferLength);
    }

    md->release();


    IOSleep(1);
    UC_IOLOG_DEBUG_METHOD("(userBuffer = %16p, pBufferSize = %16p -> %llu) -- fProvider=" REVEALFMT,
                 userBuffer, pBufferSize, (pBufferSize) ? *pBufferSize : 0 ,
                REVEAL(fProvider));
    IOSleep(1);

    return result;


}

#if DEBUG
#define IOLOG_DEBUG_IOExternalMethodArguments(arguments) IOLOG_IOExternalMethodArguments(__FUNCTION__,arguments)
void IOLOG_IOExternalMethodArguments(const char * fnname, IOExternalMethodArguments * arguments);
void IOLOG_IOExternalMethodArguments(const char * fnname, IOExternalMethodArguments * arguments)
{
    IOLOG_DEBUG(" In SCOTT %s", fnname);

    IOLOG_DEBUG(" version=%d",arguments->version);

    IOLOG_DEBUG(" selector%d", arguments->selector);

    IOLOG_DEBUG(" asyncWakePort=0x%016lX", (size_t)(void *)(arguments->asyncWakePort));

    IOLOG_DEBUG(" asyncReference=0x%016lX", (size_t)(void *)(arguments->asyncReference));
    IOLOG_DEBUG(" asyncReferenceCount=%d", arguments->asyncReferenceCount);

    IOLOG_DEBUG(" scalarInput=0x%016lX", (size_t)(void *)(arguments->scalarInput));
    IOLOG_DEBUG(" scalarInputCount=%d", arguments->scalarInputCount);

    for (uint32_t i=0; i<arguments->scalarInputCount; i++) {
        IOLOG_DEBUG(" scalarInput[%d]=0x%016llX", i, (arguments->scalarInput)[i]);
    }

    IOLOG_DEBUG(" structureInput=0x%016lX", (size_t)(void *)(arguments->structureInput));
    IOLOG_DEBUG(" structureInputCount=%d", arguments->structureInputSize);
    IOLOG_DEBUG(" structureInputDescriptor=0x%016lX", (size_t)(void *)(arguments->structureInputDescriptor));

    IOLOG_DEBUG(" scalarOutput=0x%016lX", (size_t)(void *)(arguments->scalarOutput));
    IOLOG_DEBUG(" scalarOutputCount=%d", arguments->scalarOutputCount);
    IOLOG_DEBUG(" structureOutput=0x%016lX", (size_t)(void *)(arguments->structureOutput));
    IOLOG_DEBUG(" structureOutputCount=%d", arguments->structureOutputSize);
    IOLOG_DEBUG(" structureOutputDescriptor=0x%016lX", (size_t)(void *)(arguments->structureOutputDescriptor));

    IOLOG_DEBUG(" structureVariableOutputData=0x%016lX", (size_t)(void *)(arguments->structureVariableOutputData));
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
    IOReturn success;
    UC_IOLOG_DEBUG_METHOD(" fProvider =" REVEALFMT, REVEAL(fProvider) );


    // check that driver is ready and arguments are valid
    if (fProvider == NULL || isInactive()) {
        IOLOG_DEBUG_METHOD(" fProvider NULL or inactive");
        return kIOReturnNotAttached;
    }

    if (!(fProvider->isOpen(this))) {
        IOLOG_DEBUG_METHOD(" fProvider not open");
        return kIOReturnNotOpen;
    }

    success = fProvider->updatePropertiesInIORegistry();

    UC_IOLOG_DEBUG_METHOD(" after driver function, success is 0x%04x",success);
    return success;
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
    IOReturn	success = kIOReturnSuccess;

    UC_IOLOG_DEBUG_METHOD(" fProvider=" REVEALFMT, REVEAL(fProvider) );

    if (fProvider == NULL || isInactive()) {
		// Return an error if we don't have a provider. This could happen if the user process
		// called openUserClient without calling IOServiceOpen first. Or, the user client could be
		// in the process of being terminated and is thus inactive.
        success = kIOReturnNotAttached;
	}

    else if (!fProvider->open(this)) {
	      // The most common reason this open call will fail is because the provider is already open
	      // and it doesn't support being opened by more than one client at a time.
		success = kIOReturnExclusiveAccess;
	}

    UC_IOLOG_DEBUG_METHOD(" success is 0x%08X",success);
    return success;
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
    UC_IOLOG_DEBUG_METHOD();

    IOReturn	success = kIOReturnSuccess;

    if (fProvider == NULL) {
		// Return an error if we don't have a provider. This could happen if the user process
		// called closeUserClient without calling IOServiceOpen first.
		success = kIOReturnNotAttached;
		IOLOG_DEBUG_METHOD(" fProvider is NULL, returning kIOReturnNotAttached");
	}
	else if (!fProvider->isOpen(this)) {
        // Make sure we're the one who opened our provider before we tell it to close.
		success = kIOReturnNotOpen;
		IOLOG_DEBUG_METHOD(" fProvider = " REVEALFMT " is not Open, returning kIOReturnNotOpen", REVEAL(fProvider));
	}
    else {
        UC_IOLOG_DEBUG_METHOD(" *** before fProvider->close(" REVEALFMT ")", REVEAL(this));
        fProvider->close(this);
        UC_IOLOG_DEBUG_METHOD(" *** after fProvider->close(" REVEALFMT ")", REVEAL(this));
    }

    UC_IOLOG_DEBUG_METHOD(" *** success is 0x%08X", success);
    return success;
}

//*****************
// apple UserClient methods
//*****************

// initWithTask is called as a result of the user process calling IOServiceOpen.
bool UserClientClass::initWithTask(task_t owningTask, void* securityToken, UInt32 type)
{
    // This is IOLOG_DEBUG because getName relies on the superclass initialization.
    IOLOG_DEBUG(kUserClientClass "[" REVEALFMT "]::%s(" REVEALFMT ", " REVEALFMT ", %u)", REVEAL(this), __FUNCTION__, REVEAL(owningTask), REVEAL(securityToken), (unsigned int)type);
    UC_IOLOG_DEBUG_METHOD(" *** before super");
    bool success = super::initWithTask(owningTask, securityToken, type);

    // This UC_IOLOG_DEBUG must follow super::initWithTask because getName relies on the superclass initialization.
    UC_IOLOG_DEBUG_METHOD(" *** after super, success is %s", success ? "true" : "false");

    fTask = owningTask;
    fProvider = NULL;

    return success;
}


// start is called after initWithTask as a result of the user process calling IOServiceOpen.
bool UserClientClass::start(IOService* provider)
{
    UC_IOLOG_DEBUG_METHOD("(" REVEALFMT ")", REVEAL(provider));

    // Verify that this user client is being started with a provider that it knows
    // how to communicate with.
    fProvider = OSDynamicCast(DriverClass, provider);
    if (fProvider == NULL ) {
        // It's important not to call super::start if some previous condition
        // (like an invalid provider) would cause this function to return false.
        // I/O Kit won't call stop on an object if its start function returned false.
        IOLOG_DEBUG_METHOD(" fProvider is NULL, not calling super");
        return false;
    }

    UC_IOLOG_DEBUG_METHOD(" *** before super(" REVEALFMT ")", REVEAL(fProvider));
    bool success = super::start(provider);
    UC_IOLOG_DEBUG_METHOD(" *** after super, success is %s", success ? "true" : "false");

    return success;
}


// clientClose is called as a result of the user process calling IOServiceClose.
IOReturn UserClientClass::clientClose(void)
{
    UC_IOLOG_DEBUG_METHOD();

    // Defensive coding in case the user process called IOServiceClose
    // without calling closeUserClient first.
    UC_IOLOG_DEBUG_METHOD(" *** Defensive call to closeUserClient");
    (void) closeUserClient();

    // Inform the user process that this user client is no longer available. This will also cause the
    // user client instance to be destroyed.
    //
    // terminate would return false if the user process still had this user client open.
    // This should never happen in our case because this code path is only reached if the user process
    // explicitly requests closing the connection to the user client.
    UC_IOLOG_DEBUG_METHOD(" calling terminate");
    bool success = terminate();
    if (!success) {
        IOLOG_DEBUG_METHOD(" *** terminate() call failed?!");
    }

    UC_IOLOG_DEBUG_METHOD(" *** DON'T call super, which just returns kIOReturnUnsupported.");

    return kIOReturnSuccess;  // TODO:  OK to ignore the terminate() result?!
}

//*****************
// apple UserClient methods
//*****************

// didTerminate is called at the end of the termination process. It is a notification
// that a provider has been terminated, sent after recursing up the stack, in leaf-to-root order.
bool UserClientClass::didTerminate(IOService* provider, IOOptionBits options, bool* defer)
{
    UC_IOLOG_DEBUG_METHOD("(" REVEALFMT ", %d, " REVEALFMT ")", REVEAL(provider), options, REVEAL(defer));
    // If all pending I/O has been terminated, close our provider. If I/O is still outstanding, set defer to true
    // and the user client will not have stop called on it.
    UC_IOLOG_DEBUG_METHOD(" defer is " REVEALFMT ", *defer is %s", REVEAL(defer), defer==NULL ? "*NULL" : *defer ? "true" : "false");
    if (defer != NULL && !*defer) {
        UC_IOLOG_DEBUG_METHOD(" calling closeUserClient");
        closeUserClient();
        UC_IOLOG_DEBUG_METHOD(" back from closeUserClient, setting *defer=false");
        *defer = false;
    }

    UC_IOLOG_DEBUG_METHOD(" *** before super");
    bool success = super::didTerminate(provider, options, defer);
    UC_IOLOG_DEBUG_METHOD(" *** after super, success is %s", success ? "true" : "false");

    return success;
}

//*****************
// apple UserClient methods
//          not overriden except debug messages for program flow
//*****************

void UserClientClass::stop(IOService* provider)
{
    UC_IOLOG_DEBUG_METHOD("(" REVEALFMT ")", REVEAL(provider));
    UC_IOLOG_DEBUG_METHOD(" *** before super");
    super::stop(provider);
    UC_IOLOG_DEBUG_METHOD(" *** after super");
}


bool UserClientClass::terminate(IOOptionBits options)
{
    UC_IOLOG_DEBUG_METHOD(" *** before super(%d)", options);
    bool success = super::terminate(options);
    UC_IOLOG_DEBUG_METHOD(" *** after super, success is %s", success ? "true" : "false");

    return success;
}


bool UserClientClass::finalize(IOOptionBits options)
{
    UC_IOLOG_DEBUG_METHOD(" *** before super(%d)", options);
    bool success = super::finalize(options);
    UC_IOLOG_DEBUG_METHOD(" *** after super, success is %s", success ? "true" : "false");

    return success;
}

// override if alternate action if the user process crashes instead of exiting normally.
IOReturn UserClientClass::clientDied(void)
{
    UC_IOLOG_DEBUG_METHOD(" *** before super");
    IOReturn result = super::clientDied();
    UC_IOLOG_DEBUG_METHOD(" *** after super, result is 0x%08X", result);

    return result;
}


// willTerminate is called at the beginning of the termination process. It is a notification
// that a provider has been terminated, sent before recursing up the stack, in root-to-leaf order.
//
// This is where any pending I/O should be terminated. At this point the user client has been marked
// inactive and any further requests from the user process should be returned with an error.
bool UserClientClass::willTerminate(IOService* provider, IOOptionBits options)
{
    UC_IOLOG_DEBUG_METHOD(" *** before super(" REVEALFMT ", %d)", REVEAL(provider), options);
    bool success = super::willTerminate(provider, options);
    UC_IOLOG_DEBUG_METHOD(" *** after super, success is %s", success ? "true" : "false");

    return success;
}
