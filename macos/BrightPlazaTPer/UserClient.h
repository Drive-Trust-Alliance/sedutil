//
//  UserClient.h
//  UserClient
//
//  Created by Jackie Marks on 9/21/15.
//  Copyright © 2015 Bright Plaza Inc. All rights reserved.
//

#include "TPerDriver.h"
#include "TPerDriverMethodIndex.h"


class UserClientClass : public IOUserClient
{
    OSDeclareDefaultStructors(com_brightplaza_BrightPlazaTPerUserClient)
    
protected:
    DriverClass *                           fProvider;
    task_t									fTask;
    static const IOExternalMethodDispatch	sMethods[kSedUserClientNumberOfMethods];
    
public:
    // IOUserClient methods
    virtual void stop(IOService* provider) APPLE_KEXT_OVERRIDE;
    virtual bool start(IOService* provider) APPLE_KEXT_OVERRIDE;
    
    virtual bool initWithTask(task_t owningTask, void* securityToken, UInt32 type) APPLE_KEXT_OVERRIDE;
    
    virtual IOReturn clientClose(void) APPLE_KEXT_OVERRIDE;
    virtual IOReturn clientDied(void) APPLE_KEXT_OVERRIDE;
    
    virtual bool willTerminate(IOService* provider, IOOptionBits options) APPLE_KEXT_OVERRIDE;
    virtual bool didTerminate(IOService* provider, IOOptionBits options, bool* defer) APPLE_KEXT_OVERRIDE;
    
    virtual bool terminate(IOOptionBits options = 0) APPLE_KEXT_OVERRIDE;
    virtual bool finalize(IOOptionBits options) APPLE_KEXT_OVERRIDE;
    
protected:
    
    // KPI for supporting access from both 32-bit and 64-bit user processes beginning with Mac OS X 10.5.
    virtual IOReturn externalMethod(uint32_t selector,
                                    IOExternalMethodArguments* arguments,
                                    IOExternalMethodDispatch* dispatch,
                                    OSObject* target,
                                    void* reference) APPLE_KEXT_OVERRIDE;
    
    // UserClient methods
    static IOReturn sOpenUserClient(UserClientClass* target,
                                    void* reference,
                                    IOExternalMethodArguments* arguments);
    virtual IOReturn openUserClient(void);
    
    
    static IOReturn sCloseUserClient(UserClientClass* target,
                                     void* reference,
                                     IOExternalMethodArguments* arguments);
    virtual IOReturn closeUserClient(void);
    
    
    // TPer methods
    static IOReturn sUserClientPerformSCSICommand(UserClientClass* target,
                                               void* reference,
                                               IOExternalMethodArguments* arguments);
    IOReturn userClientPerformSCSICommand(SCSICommandDescriptorBlock cdb,
                                          void * inBuffer,
                                          uint64_t inBufferSize,
                                          IODirection direction,
                                          uint64_t requiredTransferLength,
                                          uint64_t *pLengthActuallyTransferred);

    
    static IOReturn sUserClientUpdatePropertiesInIORegistry(UserClientClass* target,
                                                                   void* reference,
                                                                   IOExternalMethodArguments* arguments);
    IOReturn UserClientUpdatePropertiesInIORegistry(void);
    
};
