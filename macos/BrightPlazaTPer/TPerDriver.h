//
//  TPerDriver.h
//  SedUserClient
//
//  Created by Jackie Marks on 9/21/15.
//  Copyright © 2015 Bright Plaza Inc. All rights reserved.
//


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#include <IOKit/scsi/IOSCSIPeripheralDeviceType00.h>
#pragma clang diagnostic pop
#include "InterfaceDeviceID.h"


class DriverClass : public IOSCSIPeripheralDeviceType00
{
	OSDeclareDefaultStructors(com_brightplaza_BrightPlazaTPer)
    friend class com_brightplaza_BrightPlazaTPerUserClient;
	
public:
    IOReturn PerformSCSICommand(SCSICommandDescriptorBlock cdb,
                               IOBufferMemoryDescriptor * md,
                               uint64_t * pTransferSize = NULL);
    IOReturn updatePropertiesInIORegistry( void ) ;

    // IOService methods
    virtual bool start(IOService* provider) APPLE_KEXT_OVERRIDE;
    virtual void stop(IOService* provider) APPLE_KEXT_OVERRIDE;
    virtual IOService* probe(IOService* provider, SInt32* score) APPLE_KEXT_OVERRIDE;
    virtual void systemWillShutdown(IOOptionBits specifier);
    
    
    
    /*!
    @function initialPowerStateForDomainState
    @abstract Determines which power state a device is in, given the current power domain state.
    @discussion Power management calls this method once, when the driver is initializing power management.
    Subclasses should not need to override this method.
    @param flags Flags that describe the character of "domain power"; they represent the <code>outputPowerCharacter</code> field of a state in the power domain's power state array.
    @result A state number.
    */
    virtual unsigned long    initialPowerStateForDomainState ( IOPMPowerFlags flags ) APPLE_KEXT_OVERRIDE;
    
    
    /*!
    @function setPowerState
    @abstract Requests a power managed driver to change the power state of its device.
    @discussion Requests a power managed driver to change the power state of its device. Most subclasses
    of IOSCSIProtocolInterface have class-specific mechanisms and should not override this routine.
    See IOSCSIProtocolServices.h, IOSCSIBlockCommandsDevice.h, IOSCSIReducedBlockCommandsDevice.h, and
    IOSCSIMultimediaCommandsDevice.h for more information about power management changes.
    Subclasses should not need to override this method.
    @param powerStateOrdinal The number in the power state array to which the drive is being instructed to change.
    @param whichDevice A pointer to the power management object which registered to manage power for this device.
    The whichDevice field is not pertinent to us since the driver is both the "policy maker" for the device,
    and the "policy implementor" for the device.
    @result See IOService.h for details.
    */
    virtual IOReturn     setPowerState ( unsigned long powerStateOrdinal, IOService * whichDevice ) APPLE_KEXT_OVERRIDE;
    
protected:
    
    // IOService methods

    
    
    // override of Apple function that is called from start
    // we use this override to determine whether device is a TCG TPer
    // if not, we abort the startup of the driver
    bool InitializeDeviceSupport ( void ) APPLE_KEXT_OVERRIDE;


private:

    //
    // functions called from SCSIPassThrough
    //
    IOReturn prepareSCSICommand(SCSICommandDescriptorBlock cdb,
                                UInt8 cdbSize,
                                IOBufferMemoryDescriptor * md,
                                SCSITaskIdentifier request,
                                UInt8 direction,
                                UInt64 count);

    IOReturn completeSCSICommand(IOBufferMemoryDescriptor * md,
                                 SCSITaskIdentifier request,
                                 SCSIServiceResponse serviceResponse,
                                 uint64_t * pTransferSize);
    
    void printSenseData( SCSI_Sense_Data * sense);

    
    //
    // Initialization functions
    //
    bool IdentifyTPer(void);
    
    void GetDeviceInfo(DTA_DEVICE_INFO &di);
    
    void updateIORegistryFromD0Response(const uint8_t * d0Response, DTA_DEVICE_INFO & di);

    IOReturn updatePropertiesInIORegistryWithDiscovery0CDB(SCSICommandDescriptorBlock cdb, DTA_DEVICE_INFO & di );

    //
    // General SCSI Interface
    //
    bool deviceIsStandardSCSI(InterfaceDeviceID & deviceIdentification, DTA_DEVICE_INFO &di);
    IOReturn __inquiry(uint8_t evpd, uint8_t page_code, IOBufferMemoryDescriptor * md, UInt16 & dataSize );
    IOReturn __inquiry__EVPD(uint8_t page_code, IOBufferMemoryDescriptor * md, UInt16 & dataSize );
    IOReturn inquiryStandardDataAll_SCSI( IOBufferMemoryDescriptor * md);
    OSDictionary * parseInquiryStandardDataAllResponse(const unsigned char * response,
                                                       InterfaceDeviceID & interfaceDeviceIdentification,
                                                       DTA_DEVICE_INFO & di);


#define USE_INQUIRY_PAGE_00h
#if defined(USE_INQUIRY_PAGE_00h)
    bool deviceIsPage00SCSI(bool & deviceSupportsPage80,
                            bool & deviceSupportsPage89);
    IOReturn inquiryPage00_SCSI( IOBufferMemoryDescriptor * md, UInt16 & dataSize );
    OSDictionary * parseInquiryPage00Response(const unsigned char * response,
                                              bool & deviceSupportsPage00,
                                              bool & deviceSupportsPage80,
                                              bool & deviceSupportsPage83,
                                              bool & deviceSupportsPage89);
#endif // defined(USE_INQUIRY_PAGE_00h)

#define USE_INQUIRY_PAGE_80h
#if defined(USE_INQUIRY_PAGE_80h)
    bool deviceIsPage80SCSI(const InterfaceDeviceID & interfaceDeviceIdentification, DTA_DEVICE_INFO &di);
    IOReturn inquiryPage80_SCSI( IOBufferMemoryDescriptor * md );
    OSDictionary * parseInquiryPage80Response(const InterfaceDeviceID & interfaceDeviceIdentification,
                                              const unsigned char * response,
                                              DTA_DEVICE_INFO & di);
#endif // defined(USE_INQUIRY_PAGE_80h)


#define USE_INQUIRY_PAGE_83h
#if defined(USE_INQUIRY_PAGE_83h)
    bool deviceIsPage83SCSI(DTA_DEVICE_INFO &di);
    IOReturn inquiryPage83_SCSI( IOBufferMemoryDescriptor * md, UInt16 & dataSize );
    OSDictionary * parseInquiryPage83Response( const unsigned char * response, UInt16 dataSize, DTA_DEVICE_INFO & di);
#endif // defined(USE_INQUIRY_PAGE_83h)


#define USE_INQUIRY_PAGE_89h
#if defined(USE_INQUIRY_PAGE_89h)
    bool deviceIsPage89SCSI(DTA_DEVICE_INFO &di);
    IOReturn inquiryPage89_SCSI( IOBufferMemoryDescriptor * md );
    OSDictionary * parseInquiryPage89Response( const unsigned char * response, DTA_DEVICE_INFO & di);
#endif // defined(USE_INQUIRY_PAGE_89h)

#if DEBUG
    bool deviceIsPageXXSCSI(uint8_t evpd, const char * key);
    IOReturn inquiryPageXX_SCSI(uint8_t evpd, IOBufferMemoryDescriptor * md );
#endif // DEBUG

    bool identifyUsingSCSIInquiry(InterfaceDeviceID & interfaceDeviceIdentification,
                                  DTA_DEVICE_INFO & di);
    
    //
    // SCSI (SAS) devices
    IOReturn updatePropertiesInIORegistry_SCSI( DTA_DEVICE_INFO & di );
    bool deviceIsTPer_SCSI(DTA_DEVICE_INFO &di);

    //
    // SAT (ATA-passthrough) devices
    IOReturn identifyDevice_SAT( IOBufferMemoryDescriptor * md);
    OSDictionary * parseIdentifyDeviceResponse(const InterfaceDeviceID & interfaceDeviceIdentification,
                                               const unsigned char * response,
                                               DTA_DEVICE_INFO & di);
    bool deviceIsSAT(const InterfaceDeviceID & interfaceDeviceIdentification,
                     DTA_DEVICE_INFO &di,
                     OSDictionary **pIdentifyCharacteristics);
    IOReturn updatePropertiesInIORegistry_SAT( DTA_DEVICE_INFO & di );
    bool deviceIsTPer_SAT(const InterfaceDeviceID & interfaceDeviceIdentification,
                          OSDictionary * identifyCharacteristics,
                          DTA_DEVICE_INFO &di);


#if DRIVER_DEBUG
    
    // IOService methods
public:
    virtual bool init(OSDictionary* dictionary = 0) APPLE_KEXT_OVERRIDE;
    virtual void free(void) APPLE_KEXT_OVERRIDE;
    virtual bool willTerminate(IOService* provider, IOOptionBits options) APPLE_KEXT_OVERRIDE;
    virtual bool didTerminate(IOService* provider, IOOptionBits options, bool* defer) APPLE_KEXT_OVERRIDE;
    virtual bool terminate(IOOptionBits options = 0) APPLE_KEXT_OVERRIDE;
    virtual bool finalize(IOOptionBits options) APPLE_KEXT_OVERRIDE;
    virtual bool attach(IOService * provider) APPLE_KEXT_OVERRIDE;
    virtual void detach(IOService *provider) APPLE_KEXT_OVERRIDE;
    virtual bool open(IOService * forClient, IOOptionBits options=0, void * arg=0) APPLE_KEXT_OVERRIDE;
    virtual void close(IOService * forClient, IOOptionBits options=0) APPLE_KEXT_OVERRIDE;
    
    // IOService methods
protected:
    void TerminateDeviceSupport( void ) APPLE_KEXT_OVERRIDE;
    void StartDeviceSupport( void ) APPLE_KEXT_OVERRIDE;
    void StopDeviceSupport( void ) APPLE_KEXT_OVERRIDE;
#endif

};

// Smuggling low pointer bits out of the driver to trace objects and providers in the kernel
//
#define REVEAL(p)((uint16_t)(0x0FFFFFF & (intptr_t)(p)))
#define REVEALFMT "0x%06X"
#define IOLOG_DEBUG_METHOD(fmt, ...) IOLOG_DEBUG("%s[" REVEALFMT "]::%s" fmt, getName(), REVEAL(this), __FUNCTION__ ,##__VA_ARGS__)
