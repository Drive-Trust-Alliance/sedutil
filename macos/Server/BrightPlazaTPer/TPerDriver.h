//
//  TPerDriver.h
//  SedUserClient
//
//  Created by Jackie Marks on 9/21/15.
//  Copyright © 2015 Bright Plaza Inc. All rights reserved.
//


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wdocumentation-html"
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#include <IOKit/scsi/IOSCSIPeripheralDeviceNub.h>  // TODO: tell Apple about the documentation bugs in these files
#include <IOKit/scsi/IOSCSIPeripheralDeviceType00.h>
#include <IOKit/scsi/IOSCSIBlockCommandsDevice.h>
#include <IOKit/scsi/IOBlockStorageServices.h>
#pragma clang diagnostic pop
#include "UserKernelShared.h"

class DriverClass : public IOSCSIPeripheralDeviceType00
{
	OSDeclareDefaultStructors(com_brightplaza_BrightPlazaTPer)
    friend class com_brightplaza_BrightPlazaTPerUserClient;
	
public:
    IOReturn SCSIPassthrough(SCSICommandDescriptorBlock cdb,
                             IOBufferMemoryDescriptor * md,
                             uint64_t * pTransferSize = NULL);
#if ONE_FUNCTION_SCSIPASSTHROUGH
    IOReturn onefunctionSCSIPassthrough(SCSICommandDescriptorBlock cdb,
                             IOBufferMemoryDescriptor * md,
                             uint64_t * pTransferSize = NULL);
#endif //ONE_FUNCTION_SCSIPASSTHROUGH


    virtual bool start(IOService* provider) APPLE_KEXT_OVERRIDE;

    
    
    // IOService methods
#if DRIVER_DEBUG
	virtual bool init(OSDictionary* dictionary = 0) APPLE_KEXT_OVERRIDE;
	virtual void free(void) APPLE_KEXT_OVERRIDE;
	virtual IOService* probe(IOService* provider, SInt32* score) APPLE_KEXT_OVERRIDE;
	virtual void stop(IOService* provider) APPLE_KEXT_OVERRIDE;
	virtual bool willTerminate(IOService* provider, IOOptionBits options) APPLE_KEXT_OVERRIDE;
	virtual bool didTerminate(IOService* provider, IOOptionBits options, bool* defer) APPLE_KEXT_OVERRIDE;
    virtual bool terminate(IOOptionBits options = 0) APPLE_KEXT_OVERRIDE;
    virtual bool finalize(IOOptionBits options) APPLE_KEXT_OVERRIDE;
    virtual bool attach(IOService * provider) APPLE_KEXT_OVERRIDE;
    virtual void detach(IOService *provider) APPLE_KEXT_OVERRIDE;
    virtual bool open(IOService * forClient, IOOptionBits options=0, void * arg=0) APPLE_KEXT_OVERRIDE;
    virtual void close(IOService * forClient, IOOptionBits options=0) APPLE_KEXT_OVERRIDE;
#endif
    
    IOReturn identifyDevice( IOBufferMemoryDescriptor * md);
    IOReturn discovery0( IOBufferMemoryDescriptor * md );
    IOReturn updateLockingPropertiesInIORegistry( void );
    
protected:
    // override of Apple function that is called from start
    // we use this override to determine whether device is a TCG TPer
    // if not, we abort the startup of the driver
    bool InitializeDeviceSupport ( void ) APPLE_KEXT_OVERRIDE;

#if DRIVER_DEBUG
    void TerminateDeviceSupport( void ) APPLE_KEXT_OVERRIDE;
    void StartDeviceSupport( void ) APPLE_KEXT_OVERRIDE;
    void StopDeviceSupport( void ) APPLE_KEXT_OVERRIDE;
#endif


private:
    
    // two functions called from SCSIPassThrough
    IOReturn prepareSCSIPassthrough(SCSICommandDescriptorBlock cdb,
                                    IOBufferMemoryDescriptor * md,
                                    SCSITaskIdentifier request);
    
    IOReturn completeSCSIPassthrough(IOBufferMemoryDescriptor * md,
                                     SCSITaskIdentifier request,
                                     SCSIServiceResponse serviceResponse,
                                     uint64_t * pTransferSize);
    
    // called from InitializeDeviceSupport
    // performs IDENTIFY DEVICE on drive
    // checks Trusted Computing flag to determine
    // whether our driver should remain attached to the drive
    bool deviceIsTPer(void);
    
    OSDictionary * parseIdentifyResponse( const unsigned char * response);
    
    void printSenseData( SCSI_Sense_Data * sense);
};
