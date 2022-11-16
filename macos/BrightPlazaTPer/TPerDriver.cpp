//
//  TPerDriver.cpp
//  SedUserClient
//
//  Created by Jackie Marks on 9/21/15.
//  Copyright © 2022 Bright Plaza Inc. All rights reserved.
//

#include <IOKit/IOUserClient.h>
#include "TPerDriver.h"
#include "CDBAccess.hpp"

#include <SEDKernelInterface/SEDKernelInterface.h>

#include "kernel_debug.h"
#include "kernel_PrintBuffer.h"
#include "DtaStructures.h"
#include "DtaEndianFixup.h"

#define super IOSCSIPeripheralDeviceType00

OSDefineMetaClassAndStructors(com_brightplaza_BrightPlazaTPer, IOSCSIPeripheralDeviceType00 ) //IOService)


//*****************
// apple IOService override
//*****************
bool DriverClass::start(IOService* provider)
{
    bool ret;
    IOLOG_DEBUG("%s[%p]::%s(provider = %p), provider->getName() = %s",
                getName(), this, __FUNCTION__, provider, provider->getName());
    if ( (ret = super::start(provider) ) ) {
        IOLOG_DEBUG("%s[%p]::%s - super::start(provider) returned true, calling registerService()",
              getName(), this, __FUNCTION__);
        registerService();
    }
    else if (getProperty(IOInterfaceTypeKey)) { // pointer is not null => super::InitializeDeviceSupport returned true
                                             // and deviceIsTPer returned false
        IOLOG_DEBUG("%s[%p]::%s - leaving start, Device Support was initialized but is NOT TPer, calling stop\n",
              getName(), this, __FUNCTION__);

        stop(provider);   // releases provider - having
    }
    // if neither case, then super::InitializeDeviceSupport returned false and we don't need to call stop

    IOLOG_DEBUG("%s[%p]::%s - leaving start, returning %d\n",
          getName(), this, __FUNCTION__, ret);
    return ret;
}

void DriverClass::GetDeviceInfoFromIORegistry(DTA_DEVICE_INFO &di) {
    char * v = GetVendorString ( );
    if (v != NULL) {
        strlcpy((char *)di.vendorName, v, sizeof(di.vendorName));
        IOLOG_DEBUG("%s[%p]::%s - di.vendorName set to \"%s\"\n",
                    getName(), this, __FUNCTION__, di.vendorName);
    }
    char * p = GetProductString ( );
    if (p != NULL) {
        strlcpy((char *)di.modelNum, p, sizeof(di.modelNum));
        IOLOG_DEBUG("%s[%p]::%s - di.modelNum set to \"%s\"\n",
                    getName(), this, __FUNCTION__, di.modelNum);
    }
    char * r = GetRevisionString ( );
    if (r != NULL) {
        strlcpy((char *)di.firmwareRev, r, sizeof(di.firmwareRev));
        IOLOG_DEBUG("%s[%p]::%s - di.firmwareRev set to \"%s\"\n",
                    getName(), this, __FUNCTION__, di.firmwareRev);
    }

    unsigned long long blockSize = 0;
    unsigned long long blockCount = 0;
    __unused bool determined = DetermineMediumCapacity (&blockSize, &blockCount);
    IOLOG_DEBUG("%s[%p]::%s - DetermineMediumCapacity returned %s",
                getName(), this, __FUNCTION__, determined ? "true" : "false");
    IOLOG_DEBUG("%s[%p]::%s - blockSize is %llu\n",  getName(), this, __FUNCTION__, blockSize);
    IOLOG_DEBUG("%s[%p]::%s - blockCount is %llu\n",  getName(), this, __FUNCTION__, blockCount);
    di.devSize = blockSize * blockCount;
    IOLOG_DEBUG("%s[%p]::%s - di.devSize set to %llu\n",  getName(), this, __FUNCTION__, di.devSize);
}

// IOSCSIPrimaryBlockCommandsDevice method
// override of Apple function that is called from start  --
// we use this override to determine whether device is a TCG TPer
// if not, we abort the startup of the driver.
// deviceIsTPer will set property IOTCGPropertiesKey
// it will be an empty dictionary if device is not a TPer
// and we will need to call stop (in the start method) to terminate device support
bool DriverClass::InitializeDeviceSupport ( void )
{
    IOLOG_DEBUG("%s[%p]::%s about to call super::InitializeDeviceSupport() ...", getName(), this, __FUNCTION__);
    if (! super::InitializeDeviceSupport()) {
        IOLOG_DEBUG("%s[%p]::%s super::InitializeDeviceSupport() returned false", getName(), this, __FUNCTION__);
        return false;
    }
    IOLOG_DEBUG("%s[%p]::%s super::InitializeDeviceSupport() returned true", getName(), this, __FUNCTION__);

    InterfaceDeviceID interfaceDeviceIdentification;
    DTA_DEVICE_INFO di;

    if (!identifyUsingSCSIInquiry(interfaceDeviceIdentification, di)) {
        IOLOG_DEBUG("%s[%p]::%s Device is NOT SCSI", getName(), this, __FUNCTION__);
        return false;
    }
    IOLOG_DEBUG("%s[%p]::%s Device is SCSI", getName(), this, __FUNCTION__);

    di.devType = DEVICE_TYPE_SAS;

    if (deviceIsTPer_SCSI(di)) {
        IOLOG_DEBUG("%s[%p]::%s Device is TPer_SCSI", getName(), this, __FUNCTION__);
        setProperty(IOInterfaceTypeKey, IOInterfaceTypeSCSI);
        setProperty(IODtaDeviceInfoKey, &di, sizeof(di));
        return true;
    }

    OSDictionary * identifyCharacteristics = NULL;
    if (deviceIsSAT(di, &identifyCharacteristics)) {
        di.devType = DEVICE_TYPE_USB;
        IOLOG_DEBUG("%s[%p]::%s Device is SAT", getName(), this, __FUNCTION__);
        bool result = deviceIsTPer_SAT(interfaceDeviceIdentification, identifyCharacteristics, di) ;
        if ( NULL != identifyCharacteristics ) {
            identifyCharacteristics -> release();
            identifyCharacteristics = NULL ;
        }
        if (result) {
            IOLOG_DEBUG("%s[%p]::%s Device is TPer_SAT", getName(), this, __FUNCTION__);
            setProperty(IOInterfaceTypeKey, IOInterfaceTypeSAT);
            setProperty(IODtaDeviceInfoKey, &di, sizeof(di));
            return true;
        }
    }

    IOLOG_DEBUG("%s[%p]::%s Device is not for this driver", getName(), this, __FUNCTION__);
    return false;
}


//*****************
// public
//*****************


// SCSI Passthrough function called from user client
IOReturn DriverClass::PerformSCSICommand(SCSICommandDescriptorBlock cdb,
                                         IOBufferMemoryDescriptor * md,
                                         uint64_t * pTransferSize)
{
    IOLOG_DEBUG("%s[%p]::%s", getName(), this, __FUNCTION__);
    IOReturn ret = kIOReturnUnsupported;

    if ( md == NULL ) {
        return kIOReturnInternalError;
    }

    uint64_t transferSize = md -> getLength();
    if (pTransferSize != NULL && 0 < *pTransferSize)
        transferSize = *pTransferSize;


    SCSITaskIdentifier request = GetSCSITask();
    if ( request == NULL ) {
        return kIOReturnInternalError;
    }


    UInt8 direction = kSCSIDataTransfer_NoDataTransfer;
    UInt8 cdbSize=0;
    switch (cdb[0]) {
        case kSCSICmd_REQUEST_SENSE:
            cdbSize = 6;
            direction = kSCSIDataTransfer_FromTargetToInitiator;
            IOLOG_DEBUG("%s[%p]::%s REQUEST SENSE, direction=In", getName(), this, __FUNCTION__);
            break;
        case kSCSICmd_INQUIRY: // INQUIRY
            cdbSize = 6;
            direction = kSCSIDataTransfer_FromTargetToInitiator;
            IOLOG_DEBUG("%s[%p]::%s INQUIRY, direction=In", getName(), this, __FUNCTION__);
            break;
        case kSCSICmd_ATA_PASS_THROUGH: // ATA PASS-THROUGH
            cdbSize = 12; // 16; ?? Variable?
            direction = getATATaskCommandDir(cdb);
            transferSize = getATATransferSize(cdb);
            IOLOG_DEBUG("%s[%p]::%s ATA PASS-THROUGH, direction=%s transfersize=%llu", getName(), this, __FUNCTION__,
                        (direction==kSCSIDataTransfer_FromTargetToInitiator ? "In"
                         : direction==kSCSIDataTransfer_FromInitiatorToTarget ? "Out" : "?!?"),
                        transferSize);
            break;
        case kSCSICmd_SECURITY_PROTOCOL_IN: // SECURITY PROTOCOL IN
            cdbSize = 12;
            direction = kSCSIDataTransfer_FromTargetToInitiator;
            IOLOG_DEBUG("%s[%p]::%s SECURITY PROTOCOL IN, direction=In", getName(), this, __FUNCTION__);
            break;
        case kSCSICmd_SECURITY_PROTOCOL_OUT: // SECURITY PROTOCOL OUT
            cdbSize = 12;
            direction = kSCSIDataTransfer_FromInitiatorToTarget;
            IOLOG_DEBUG("%s[%p]::%s SECURITY PROTOCOL OUT, direction=Out", getName(), this, __FUNCTION__);
            break;
        default:
            IOLOG_DEBUG("%s[%p]::%s UNRECOGNIZED COMMAND=0x%02X", getName(), this, __FUNCTION__, cdb[0]);
            return ret;
    }


    ret = prepareSCSICommand(cdb, cdbSize, md, request, direction, transferSize);

    if ( ret != kIOReturnSuccess ) {
        IOLOG_DEBUG("%s[%p]::%s prepareSCSICommand failed, ret=0x%08X", getName(), this, __FUNCTION__, ret);
        return ret;
    }

#if DEBUG
    if (direction == kSCSIDataTransfer_FromInitiatorToTarget) {
        IOLOG_DEBUG("%s::%s, output buffer is ", getName ( ), __FUNCTION__ );
        IOLOGBUFFER_DEBUG(NULL, md->getBytesNoCopy(), transferSize);
    }
#endif //DEBUG

    // Call IOSCSIPrimaryCommandsDevice::SendCommand
    SCSIServiceResponse serviceResponse = SendCommand(request, SED_TIMEOUT);

    if ( serviceResponse != kSCSIServiceResponse_TASK_COMPLETE) {
        IOLOG_DEBUG("%s[%p]::%s Hmm, SendCommand returned %d", getName(), this, __FUNCTION__, serviceResponse);
    }

    ret = completeSCSICommand(md, request, serviceResponse, &transferSize);

    if ( ret != kIOReturnSuccess ) {
        IOLOG_DEBUG("%s[%p]::%s completeSCSICommand failed, ret=0x%08X", getName(), this, __FUNCTION__, ret);
        return ret;
    }

#if DEBUG
    if (direction == kSCSIDataTransfer_FromTargetToInitiator) {
        IOLOG_DEBUG("%s::%s, input buffer is ", getName ( ), __FUNCTION__ );
        IOLOGBUFFER_DEBUG(NULL, md->getBytesNoCopy(), transferSize);
    }
#endif //DEBUG

    if (pTransferSize != NULL)
        *pTransferSize = transferSize;

    ReleaseSCSITask(request);
    request = NULL;

    return ret;

}


IOReturn DriverClass::updatePropertiesInIORegistry( void )
{
    OSObject * it = getProperty(IOInterfaceTypeKey);
    if (NULL == it)
        return kIOReturnUnsupported;
    else {
        const char * interfaceType = OSRequiredCast(OSString, it)->getCStringNoCopy();
        if (NULL == interfaceType)
            return kIOReturnUnsupported;
        DTA_DEVICE_INFO di;
        if (0 == strncmp(interfaceType, IOInterfaceTypeSAT, 3))
            return updatePropertiesInIORegistry_SAT(di);
        else if (0 == strncmp(interfaceType, IOInterfaceTypeSCSI, 4))
            return updatePropertiesInIORegistry_SCSI(di);
        else
            return kIOReturnUnsupported;
    }
}

//*****************
// private
//*****************

bool DriverClass::identifyUsingSCSIInquiry(InterfaceDeviceID interfaceDeviceIdentification, DTA_DEVICE_INFO &di) {
    if (!deviceIsStandardSCSI(interfaceDeviceIdentification, di)) {
        IOLOG_DEBUG("%s[%p]::%s Device is not Standard SCSI -- not for this driver", getName(), this, __FUNCTION__);
        return false;
    }
    IOLOG_DEBUG("%s[%p]::%s Device identification fields:", getName(), this, __FUNCTION__);
    IOLOGBUFFER_DEBUG(NULL, interfaceDeviceIdentification, sizeof(InterfaceDeviceID));


    // Extract information from Inquiry VPD pages
    //

    bool deviceSupportsPage80=false;
    bool deviceSupportsPage89=false;
#if defined(USE_INQUIRY_PAGE_00h)
    if (deviceIsPage00SCSI(deviceSupportsPage80,
                           deviceSupportsPage89)) {
        IOLOG_DEBUG("%s[%p]::%s Device is Page 00 SCSI", getName(), this, __FUNCTION__);
        IOLOG_DEBUG("%s[%p]::%s Device %s support Page 80h", getName(), this, __FUNCTION__,
                    deviceSupportsPage80 ? "DOES" : "DOES NOT");
        IOLOG_DEBUG("%s[%p]::%s Device %s support Page 89h", getName(), this, __FUNCTION__,
                    deviceSupportsPage89 ? "DOES" : "DOES NOT");
    } else  {
        IOLOG_DEBUG("%s[%p]::%s Device is not Page 00 SCSI", getName(), this, __FUNCTION__);
        return false;  // Mandatory, according to standard
    }
#endif // defined(USE_INQUIRY_PAGE_00h)

#if defined(USE_INQUIRY_PAGE_80h)
    if (deviceSupportsPage80) {
        if (deviceIsPage80SCSI(interfaceDeviceIdentification, di)) {
            IOLOG_DEBUG("%s[%p]::%s Device is Page 80 SCSI", getName(), this, __FUNCTION__);
        } else  {
            IOLOG_DEBUG("%s[%p]::%s Device is not Page 80 SCSI", getName(), this, __FUNCTION__);
            return false;  // Claims to support it on Page 00h, but does not
        }
    }
#endif // defined(USE_INQUIRY_PAGE_80h)

#if defined(USE_INQUIRY_PAGE_83h)
    if (deviceIsPage83SCSI(di)) {
        IOLOG_DEBUG("%s[%p]::%s Device is Page 83 SCSI", getName(), this, __FUNCTION__);
    } else  {
        IOLOG_DEBUG("%s[%p]::%s Device is not Page 83 SCSI", getName(), this, __FUNCTION__);
        return false;  // Mandatory, according to standard
    }
#endif // defined(USE_INQUIRY_PAGE_83h)


#if defined(USE_INQUIRY_PAGE_89h)
    if (deviceSupportsPage89) {
        if (deviceIsPage89SCSI(di)) {
            IOLOG_DEBUG("%s[%p]::%s Device is Page 89 SCSI", getName(), this, __FUNCTION__);
        } else  {
            IOLOG_DEBUG("%s[%p]::%s Device is not Page 89 SCSI", getName(), this, __FUNCTION__);
            return false;   // Claims to support it on page 00h, but does not
        }
    }
#if DEBUG
    else {
        IOLOG_DEBUG("%s[%p]::%s Device does not claim to support Page 89 -- trying it anyway", getName(), this, __FUNCTION__);
        if (deviceIsPage89SCSI(di)) {
            IOLOG_DEBUG("%s[%p]::%s Device is Page 89 SCSI!!", getName(), this, __FUNCTION__);
        }
    }
#endif
#endif // defined(USE_INQUIRY_PAGE_89h)

#if DEBUG
    deviceIsPageXXSCSI(kINQUIRY_PageB0_PageCode, IOInquiryPageB0ResponseKey);
    deviceIsPageXXSCSI(kINQUIRY_PageB1_PageCode, IOInquiryPageB1ResponseKey);
    deviceIsPageXXSCSI(kINQUIRY_PageB2_PageCode, IOInquiryPageB2ResponseKey);
    deviceIsPageXXSCSI(kINQUIRY_PageC0_PageCode, IOInquiryPageC0ResponseKey);
    deviceIsPageXXSCSI(kINQUIRY_PageC1_PageCode, IOInquiryPageC1ResponseKey);
#endif
    

    return true;
}

IOReturn DriverClass::identifyDevice_SAT( IOBufferMemoryDescriptor * md )
{
    IOLOG_DEBUG("%s[%p]::%s", getName(), this, __FUNCTION__);

    static SCSICommandDescriptorBlock identifyCDB_SAT =
      { kSCSICmd_ATA_PASS_THROUGH,    // Byte  0  ATA PASS-THROUGH (12)
        //    /*
        //     * Byte 1 is the protocol 4 = PIO IN and 5 = PIO OUT, in bits 4-1 (i.e., not bit zero)
        //     */
        PIO_DataIn<<1, // Byte  1  ATA Protocol = PIO Data-In
        //    /*
        //     * Byte 2 is:
        //     * bits 7-6 OFFLINE - Amount of time the command can take the bus offline
        //     * bit 5    CK_COND - If set the command will always return a condition check
        //     * bit 4    RESERVED
        //     * bit 3    T_DIR   - transfer direction 1 in, 0 out
        //     * bit 2    BYTE_BLock  1 = transfer in blocks, 0 transfer in bytes
        //     * bits 1-0 T_LENGTH -  2 = the length is in the sector count field
        //     */
        0x0E,    // Byte  2  OFF_LINE=0, CK_CON=0, T_DIR=1, BYT_BLOK=1=blocks, T_LENGTH=2 (in sector_count)
        0x00,    // Byte  3  FEATURES SECURITY PROTOCOL = 0 (TCG)  -- not a security command
        0x01,    // Byte  4  SECTOR_COUNT = 1
        0x00,    // Byte  5  LBA_LOW  -- unused
        0x00,    // Byte  6  LBA_MID  -- unused
        0x00,    // Byte  7  LBA_HIGH -- unused
        0x00,    // Byte  8  DEVICE -- so far always zero
        kATACmd_IDENTIFY_DEVICE,    // Byte  9  COMMAND=ATA IDENTIFY DEVICE
        0x00,    // Byte 10  Reserved -- zero
        0x00,    // Byte 11  CONTROL -- so far always zero
      };
    return PerformSCSICommand(identifyCDB_SAT, md);
}

bool DriverClass::deviceIsSAT(DTA_DEVICE_INFO &di, OSDictionary ** pIdentifyCharacteristics)
{
    // Test whether device is a SAT drive by attempting
    // SCSI passthrough of ATA Identify Device command
    // If it works, as a side effect, parse the Identify response
    // and save it in the IO Registry
    bool isSAT = false;
    IOBufferMemoryDescriptor * md = IOBufferMemoryDescriptor::withCapacity ( IDENTIFY_RESPONSE_SIZE, kIODirectionIn, false );
    if ( md != NULL ) {
        void * identifyResponse = md->getBytesNoCopy ( );
        bzero ( identifyResponse, md->getLength ( ) );

        isSAT = (kIOReturnSuccess == identifyDevice_SAT( md ));

        if (isSAT) {

            if (0xA5==((UInt8 *)identifyResponse)[510]) {  // checksum is present
                UInt8 checksum=0;
                for (UInt8 * p = ((UInt8 *)identifyResponse), * end = ((UInt8 *)identifyResponse) + 512; p<end ; p++)
                    checksum=(UInt8)(checksum+(*p));
                if (checksum != 0) {
                    IOLOG_DEBUG("%s[%p]::%s *** IDENTIFY DEVICE response checksum failed *** !!!\n",
                                getName(), this, __FUNCTION__);
                }
            }
#if DEBUG
            setProperty(IOIdentifyDeviceResponseKey, identifyResponse, IDENTIFY_RESPONSE_SIZE);
#endif // DEBUG

            *pIdentifyCharacteristics = parseIdentifyResponse(((UInt8 *)identifyResponse), di);
#if DEBUG
            setProperty(IOIdentifyCharacteristicsKey, *pIdentifyCharacteristics);
#endif // DEBUG
        }
        md->release ( );
    }
    IOLOG_DEBUG("%s[%p]::%s *** end of function, isSAT is %d\n", getName(), this, __FUNCTION__, isSAT);
    return isSAT;
}

bool DriverClass::deviceIsTPer_SAT(const InterfaceDeviceID interfaceDeviceIdentification,
                                   OSDictionary * identifyCharacteristics,
                                   DTA_DEVICE_INFO &di)
{
    // Test whether the device is a TPer by
    // retrieving level 0 discovery information via SAT
    // using ATA TRUSTED RECEIVE
    // If it works, as a side effect, parse the discovery0 response
    // and save it in the IO Registry


    // We are short-circuiting all the careful checking below when we have a known interface
    // device, particularly one that incorrectly fails to claim to be a TPer.
    if (0 != (tryUnjustifiedLevel0Discovery & actionForID(interfaceDeviceIdentification))) {
        IOLOG_DEBUG("%s[%p]::%s *** interface device ID matches tperOverride entry", getName(), this, __FUNCTION__);
        if (kIOReturnSuccess == updatePropertiesInIORegistry_SAT(di)) {
            IOLOG_DEBUG("%s[%p]::%s *** tperOverride level 0 discovery worked", getName(), this, __FUNCTION__);
            return true;
        }
        IOLOG_DEBUG("%s[%p]::%s *** despite matching tperOverride entry, level 0 discovery did not work", getName(), this, __FUNCTION__);
    }

#if defined(UNJUSTIFIED_LEVEL_0_DISCOVERY)
    if (kIOReturnSuccess == updatePropertiesInIORegistry_SAT(di)) {
        IOLOG_DEBUG("%s[%p]::%s *** unjustified level 0 discovery worked", getName(), this, __FUNCTION__);
        return true;
    }
#endif // defined(UNJUSTIFIED_LEVEL_0_DISCOVERY)


    if (identifyCharacteristics == NULL){
        IOLOG_DEBUG("%s[%p]::%s *** identifyCharacteristics is NULL", getName(), this, __FUNCTION__);
        return false;
    }

    OSNumber * tcgOptions = OSRequiredCast(OSNumber,identifyCharacteristics->getObject(IOTCGOptionsKey));
    if (tcgOptions == NULL) {
        IOLOG_DEBUG("%s[%p]::%s *** tcgOptions is NULL", getName(), this, __FUNCTION__);
        return false;
    }


    // TCG Options word is valid and TCG Features present bit is one
    IOLOG_DEBUG("%s[%p]::%s *** from tcgOptions->unsigned16BitValue() is 0x%04X", getName(), this, __FUNCTION__,
                tcgOptions->unsigned16BitValue());
    bool isTPer = (tcgOptions->unsigned16BitValue() & 0xE001) == 0x4001;
    IOLOG_DEBUG("%s[%p]::%s *** from tcgOptions, isTPer is %s", getName(), this, __FUNCTION__,
                isTPer ? "true" : "false");

    if (isTPer) {
        isTPer = (kIOReturnSuccess == updatePropertiesInIORegistry_SAT(di));
        IOLOG_DEBUG("%s[%p]::%s *** level 0 discovery worked", getName(), this, __FUNCTION__);
    }
    IOLOG_DEBUG("%s[%p]::%s *** end of function, isTPer is %s", getName(), this, __FUNCTION__,
                isTPer ? "true" : "false");

    return isTPer;
}

bool DriverClass::deviceIsStandardSCSI(InterfaceDeviceID interfaceDeviceIdentification, DTA_DEVICE_INFO &di)
{
    // Test whether device is a SCSI drive by attempting
    // SCSI Inquiry command
    // If it works, as a side effect, parse the Inquiry response
    // and save it in the IO Registry
    bool isStandardSCSI = false;
    size_t transferSize = sizeof(SCSICmd_INQUIRY_StandardDataAll);
    IOBufferMemoryDescriptor * md = IOBufferMemoryDescriptor::withCapacity ( transferSize, kIODirectionIn, false );
    if ( md != NULL ) {
        void * inquiryResponse = md->getBytesNoCopy( );
        bzero ( inquiryResponse, md->getLength ( ) );
        isStandardSCSI = ( kIOReturnSuccess == inquiryStandardDataAll_SCSI( md ) );
        if (isStandardSCSI) {
#if DEBUG
            setProperty(IOInquiryDeviceResponseKey, inquiryResponse, static_cast<unsigned int>(transferSize));
#endif // DEBUG
            OSDictionary * characteristics =
                parseInquiryStandardDataAllResponse(static_cast <const unsigned char * >(inquiryResponse),
                                                    interfaceDeviceIdentification, di);
#if DEBUG
            setProperty(IOInquiryCharacteristicsKey, characteristics);
#endif // DEBUG
            characteristics -> release () ;
            characteristics = NULL ;
        }
        md->release ( );
        md = NULL;
    }
    IOLOG_DEBUG("%s[%p]::%s *** end of function, isStandardSCSI is %d\n", getName(), this, __FUNCTION__, isStandardSCSI);
    return isStandardSCSI;
}


IOReturn DriverClass::__inquiry(uint8_t evpd, uint8_t page_code, IOBufferMemoryDescriptor * md, UInt16 & dataSize)
{
    IOLOG_DEBUG("%s[%p]::%s", getName(), this, __FUNCTION__);

#define USE_INHERITED_API
#undef USE_INHERITED_API

#if defined(USE_INHERITED_API)

    md->prepare();
    dataSize = (UInt16)md->getLength();
    void * data = md->getBytesNoCopy();
    bzero(data,dataSize);
    // Use inherited IOSCSIPrimaryCommandsDevice::RetrieveINQUIRYData
    bool success = RetrieveINQUIRYData(evpd, page_code, (UInt8 *)data, &dataSize );
    md->complete();
    return success ? kIOReturnSuccess : kIOReturnIOError;

#else  // !defined(USE_INHERITED_API)

    static SCSICommandDescriptorBlock inquiryCDB_SCSI =
      { kSCSICmd_INQUIRY,           // Byte  0  INQUIRY 12h
        0x00,                       // Byte  1  Logical Unit Number| Reserved | EVPD
        0x00,                       // Byte  2  Page Code
        0x00,                       // Byte  3  Allocation length (MSB)
        0x00,                       // Byte  4  Allocation length (LSB)
        0x00,                       // Byte  5  Control
      };
    unsigned long long len = md->getLength();
    inquiryCDB_SCSI[1] = evpd;
    inquiryCDB_SCSI[2] = page_code;
    inquiryCDB_SCSI[3] = (uint8_t)(len >> 8);
    inquiryCDB_SCSI[4] = (uint8_t)(len     );
    IOLOG_DEBUG("%s[%p]::%s len=%llu=0x%02X:0x%02X\n", getName(), this, __FUNCTION__,
                len, (uint8_t)(len >> 8), (uint8_t)(len     ));
    uint64_t transferSize = dataSize ;
    IOReturn ret = PerformSCSICommand(inquiryCDB_SCSI, md, &transferSize);
    if (ret == kIOReturnSuccess) {
        dataSize = static_cast<UInt16>(transferSize) ;
    }
    return ret;

#endif //  defined(USE_INHERITED_API)


}

IOReturn DriverClass::__inquiry__EVPD(uint8_t page_code, IOBufferMemoryDescriptor * md, UInt16 & dataSize )
{
    IOLOG_DEBUG("%s[%p]::%s", getName(), this, __FUNCTION__);
    return __inquiry(0x01, page_code, md, dataSize);
}


IOReturn DriverClass::inquiryStandardDataAll_SCSI( IOBufferMemoryDescriptor * md )
{
    IOLOG_DEBUG("%s[%p]::%s", getName(), this, __FUNCTION__);
    UInt16 dataSize = static_cast<UInt16>(md->getLength());
    return __inquiry(0x00, 0x00, md, dataSize);
}


OSDictionary * DriverClass::parseInquiryStandardDataAllResponse( const unsigned char * response, unsigned char interfaceDeviceIdentification[], DTA_DEVICE_INFO & di)
{
    SCSICmd_INQUIRY_StandardDataAll *resp = (SCSICmd_INQUIRY_StandardDataAll *)response;

    memcpy(interfaceDeviceIdentification, resp->VENDOR_IDENTIFICATION, kINQUIRY_VENDOR_IDENTIFICATION_Length
                                                            + kINQUIRY_PRODUCT_IDENTIFICATION_Length
                                                            + kINQUIRY_PRODUCT_REVISION_LEVEL_Length);

    GetDeviceInfoFromIORegistry(di);

    const OSObject * objects[4];
    const OSSymbol * keys[4];

    objects[0] = OSString::withCString( "SCSI");
    keys[0]    = OSSymbol::withCString( IODeviceTypeKey );

    objects[1] = OSString::withCString( (const char *)di.modelNum);
    keys[1]    = OSSymbol::withCString( IOModelNumberKey );

    objects[2] = OSString::withCString( (const char *)di.firmwareRev);
    keys[2]    = OSSymbol::withCString( IOFirmwareRevisionKey );

    objects[3] = OSString::withCString( (const char *)di.vendorName);
    keys[3]    = OSSymbol::withCString( IOVendorNameKey );

    OSDictionary * result = OSDictionary::withObjects(objects, keys, 4, 4);

    return result;
}




#if defined(USE_INQUIRY_PAGE_00h)
#pragma mark -
#pragma mark Inquiry Page 00h

bool DriverClass::deviceIsPage00SCSI(bool & deviceSupportsPage80,
                                     bool & deviceSupportsPage89)
{
    // Test whether device is a SCSI drive by attempting
    // SCSI Inquiry command
    // If it works, as a side effect, parse the Inquiry response
    // and save it in the IO Registry
    bool isPage00SCSI = false;

    // Mandatory support checked locally
    bool deviceSupportsPage00=false;
    bool deviceSupportsPage83=false;
    size_t transferSize = 256;
    IOBufferMemoryDescriptor * md = IOBufferMemoryDescriptor::withCapacity ( transferSize, kIODirectionIn, false );
    if ( md ) {
        void * inquiryResponse = md->getBytesNoCopy( );
        bzero ( inquiryResponse, md->getLength ( ) );
        UInt16 dataSize = static_cast<UInt16>(transferSize);
        isPage00SCSI = ( kIOReturnSuccess == inquiryPage00_SCSI( md, dataSize ) );
        if (isPage00SCSI) {
#if DEBUG
            setProperty(IOInquiryPage00ResponseKey, inquiryResponse, dataSize);
#endif // DEBUG
            OSDictionary * characteristics =
                parseInquiryPage00Response(static_cast <const unsigned char * >(inquiryResponse),
                                           deviceSupportsPage00,
                                           deviceSupportsPage80,
                                           deviceSupportsPage83,
                                           deviceSupportsPage89);
            if (!(deviceSupportsPage00 && deviceSupportsPage83)) {
                if (!deviceSupportsPage00) {
                    IOLOG_DEBUG("%s[%p]::%s Mandatory Inquiry VPD page code 00h support not indicated", getName(), this, __FUNCTION__);
                }
                if (!deviceSupportsPage83) {
                    IOLOG_DEBUG("%s[%p]::%s Mandatory Inquiry VPD page code 83h support not indicated", getName(), this, __FUNCTION__);
                }
                isPage00SCSI = false;
            }
#if DEBUG
            setProperty(IOInquiryPage00CharacteristicsKey, characteristics);
#endif // DEBUG
            characteristics -> release () ;
            characteristics = NULL ;
        }
        md->release ( );
        md = NULL;
    }
    IOLOG_DEBUG("%s[%p]::%s *** end of function, isPage00SCSI is %d\n", getName(), this, __FUNCTION__, isPage00SCSI);
    return isPage00SCSI;
}


IOReturn DriverClass::inquiryPage00_SCSI( IOBufferMemoryDescriptor * md, UInt16 & dataSize )
{
    IOLOG_DEBUG("%s[%p]::%s", getName(), this, __FUNCTION__);
    return __inquiry__EVPD(kINQUIRY_Page00_PageCode, md, dataSize);
}


OSDictionary * DriverClass::parseInquiryPage00Response(const unsigned char * response,
                                                       bool & deviceSupportsPage00,
                                                       bool & deviceSupportsPage80,
                                                       bool & deviceSupportsPage83,
                                                       bool & deviceSupportsPage89)
{
    SCSICmd_INQUIRY_Page00_Header *resp = (SCSICmd_INQUIRY_Page00_Header *)response;

    IOLOG_DEBUG("%s[%p]::%s supported VPD page codes:", getName(), this, __FUNCTION__);
    IOLOGBUFFER_DEBUG(NULL, 1+(&resp->PAGE_LENGTH), resp->PAGE_LENGTH);
    for (UInt8 *p = &resp->PAGE_LENGTH, * const pLast = p+*p; ++p<=pLast ;) {
        UInt8 pageCode = *p;
        switch (pageCode) {
            case kINQUIRY_Page00_PageCode:
                deviceSupportsPage00=true;
                IOLOG_DEBUG("%s[%p]::%s deviceSupportsPage00=true", getName(), this, __FUNCTION__);
                break;
            case kINQUIRY_Page80_PageCode:
                deviceSupportsPage80=true;
                IOLOG_DEBUG("%s[%p]::%s deviceSupportsPage80=true", getName(), this, __FUNCTION__);
                break;
            case kINQUIRY_Page83_PageCode:
                deviceSupportsPage83=true;
                IOLOG_DEBUG("%s[%p]::%s deviceSupportsPage83=true", getName(), this, __FUNCTION__);
                break;
            case kINQUIRY_Page89_PageCode:
                deviceSupportsPage89=true;
                IOLOG_DEBUG("%s[%p]::%s deviceSupportsPage89=true", getName(), this, __FUNCTION__);
                break;
            default:  // Others ignored
                IOLOG_DEBUG("%s[%p]::%s VPD page code %02Xh ignored", getName(), this, __FUNCTION__,
                            pageCode);
                ;
        }
    }


    const OSObject * objects[1];
    const OSSymbol * keys[1];

    objects[0] = OSData::withBytes((const void *)resp, 4+resp->PAGE_LENGTH);
    keys[0]    = OSSymbol::withCString( IOInquiryPage00ResponseKey );

    OSDictionary * result = OSDictionary::withObjects(objects, keys, 1, 1);

    return result;
}
#endif // defined(USE_INQUIRY_PAGE_00h)





#if defined(USE_INQUIRY_PAGE_80h)
#pragma mark -
#pragma mark Inquiry Page 80h

bool DriverClass::deviceIsPage80SCSI(InterfaceDeviceID interfaceDeviceIdentification, DTA_DEVICE_INFO &di)
{
    // Test whether device is a SCSI drive by attempting
    // SCSI Inquiry command
    // If it works, as a side effect, parse the Inquiry response
    // and save it in the IO Registry
    bool isPage80SCSI = false;
    size_t transferSize = 256;
    IOBufferMemoryDescriptor * md = IOBufferMemoryDescriptor::withCapacity ( transferSize, kIODirectionIn, false );
    if ( md ) {
        void * inquiryResponse = md->getBytesNoCopy( );
        bzero ( inquiryResponse, md->getLength ( ) );
        isPage80SCSI = ( kIOReturnSuccess == inquiryPage80_SCSI( md ) );
        if (isPage80SCSI) {
#if DEBUG
            setProperty(IOInquiryPage80ResponseKey, inquiryResponse, (unsigned int)transferSize);
#endif // DEBUG
            OSDictionary * characteristics =
                parseInquiryPage80Response(static_cast <const unsigned char * >(inquiryResponse), interfaceDeviceIdentification, di);
#if DEBUG
            setProperty(IOInquiryPage80CharacteristicsKey, characteristics);
#endif // DEBUG
            characteristics -> release () ;
            characteristics = NULL ;
        }
        md->release ( );
        md = NULL;
    }
    IOLOG_DEBUG("%s[%p]::%s *** end of function, isPage80SCSI is %d\n", getName(), this, __FUNCTION__, isPage80SCSI);
    return isPage80SCSI;
}


IOReturn DriverClass::inquiryPage80_SCSI( IOBufferMemoryDescriptor * md)
{
    IOLOG_DEBUG("%s[%p]::%s", getName(), this, __FUNCTION__);
    UInt16 dataSize = static_cast<UInt16>(md->getLength());
    return __inquiry__EVPD(kINQUIRY_Page80_PageCode, md, dataSize);
}


OSDictionary * DriverClass::parseInquiryPage80Response( const unsigned char * response, InterfaceDeviceID interfaceDeviceIdentification, DTA_DEVICE_INFO & di)
{
    SCSICmd_INQUIRY_Page80_Header *resp = (SCSICmd_INQUIRY_Page80_Header *)response;

    uint8_t serialNumber[257];
    bzero(serialNumber, sizeof(serialNumber));
    memcpy(serialNumber, &resp->PRODUCT_SERIAL_NUMBER, resp->PAGE_LENGTH);
    if (0 != (reverseInquiryPage80SerialNumber & actionForID(interfaceDeviceIdentification))) {
        IOLOG_DEBUG("%s[%p]::%s *** reversing Inquiry Page80 serial number", getName(), this, __FUNCTION__);
        IOLOG_DEBUG("%s[%p]::%s Inquiry Page80 serial number was %s", getName(), this, __FUNCTION__, serialNumber);

        size_t n = strlen((const char *)(serialNumber));
        if ( 1 < n ) {
            uint8_t temp;
            for (uint8_t * p = serialNumber, * q = p + n - 1 ; p<q; p++,--q) {
                temp = *p;
                *p = *q;
                *q = temp;
            }
        }
    }
    IOLOG_DEBUG("%s[%p]::%s Inquiry Page80 serial number is %s", getName(), this, __FUNCTION__, serialNumber);
    memcpy(di.serialNum, serialNumber, sizeof(di.serialNum));


    const OSObject * objects[2];
    const OSSymbol * keys[2];

    objects[0] = OSString::withCString( (const char *)serialNumber);
    keys[0]    = OSSymbol::withCString( IOSerialNumberKey );

    objects[1] = OSData::withBytes((const void *)resp, 4 + resp->PAGE_LENGTH);
    keys[1]    = OSSymbol::withCString( IOInquiryPage80ResponseKey );

    OSDictionary * result = OSDictionary::withObjects(objects, keys, 2, 2);

    return result;
}
#endif // defined(USE_INQUIRY_PAGE_80h)



#if defined(USE_INQUIRY_PAGE_83h)
#pragma mark -
#pragma mark Inquiry Page 83h

bool DriverClass::deviceIsPage83SCSI(DTA_DEVICE_INFO &di)
{
    // Test whether device is a SCSI drive by attempting
    // SCSI Inquiry command
    // If it works, as a side effect, parse the Inquiry response
    // and save it in the IO Registry
    bool isPage83SCSI = false;
    size_t transferSize = 256;
    IOBufferMemoryDescriptor * md = IOBufferMemoryDescriptor::withCapacity ( transferSize, kIODirectionIn, false );
    if ( md ) {
        void * inquiryResponse = md->getBytesNoCopy( );
        bzero ( inquiryResponse, md->getLength ( ) );
        UInt16 dataSize = static_cast<UInt16>(transferSize);
        isPage83SCSI = ( kIOReturnSuccess == inquiryPage83_SCSI( md, dataSize ) );
        if (isPage83SCSI) {
#if DEBUG
//            setProperty(IOInquiryPage83ResponseKey, inquiryResponse, dataSize);
            setProperty(IOInquiryPage83ResponseKey, inquiryResponse, static_cast<UInt16>(transferSize));
#endif // DEBUG
            OSDictionary * characteristics =
                parseInquiryPage83Response(static_cast <const unsigned char * >(inquiryResponse), dataSize, di);
#if DEBUG
            setProperty(IOInquiryPage83CharacteristicsKey, characteristics);
#endif // DEBUG
            characteristics -> release () ;
            characteristics = NULL ;
        }
        md->release ( );
        md = NULL;
    }
    IOLOG_DEBUG("%s[%p]::%s *** end of function, isPage83SCSI is %d\n", getName(), this, __FUNCTION__, isPage83SCSI);
    return isPage83SCSI;
}


IOReturn DriverClass::inquiryPage83_SCSI( IOBufferMemoryDescriptor * md, UInt16 & dataSize )
{
    IOLOG_DEBUG("%s[%p]::%s", getName(), this, __FUNCTION__);
    return __inquiry__EVPD(kINQUIRY_Page83_PageCode, md, dataSize);
}


OSDictionary * DriverClass::parseInquiryPage83Response( const unsigned char * response, UInt16 dataSize, DTA_DEVICE_INFO & di)
{
    const OSObject * objects[] = {OSData::withBytes(response, dataSize)};
    const OSSymbol * keys   [] = {OSSymbol::withCString( IOInquiryPage83ResponseKey )};
    OSDictionary * result = OSDictionary::withObjects(objects, keys, 1, 4);

    const unsigned char * pdescs = response + sizeof(SCSICmd_INQUIRY_Page83_Header);

    // We use dataSize instead of page_length because e.g. SABRENT returns the wrong value
    const unsigned char * pdescs_end = response + dataSize;

    // Parse descriptors
    for (const unsigned char * p = pdescs; p<pdescs_end ; ) {
        const SCSICmd_INQUIRY_Page83_Identification_Descriptor & desc =
            * reinterpret_cast <const SCSICmd_INQUIRY_Page83_Identification_Descriptor *> (p);
        const unsigned char identifier_length = desc.IDENTIFIER_LENGTH ;

        // Because we must use dataSize instead of PAGE_LENGTH, we just stop if when it looks like we
        // are parsing garbage
        if (0 == identifier_length)
            break;
        bool descriptorUnrecognized = false;

#define desc_type(code_set,identifier_type) \
    (((code_set & kINQUIRY_Page83_CodeSetMask) << 8) | (identifier_type & kINQUIRY_Page83_IdentifierTypeMask))

        switch (desc_type(desc.CODE_SET, desc.IDENTIFIER_TYPE)) {
            case desc_type(kINQUIRY_Page83_CodeSetBinaryData, kINQUIRY_Page83_IdentifierTypeIEEE_EUI64):
            {
                bzero(di.worldWideName, sizeof(di.worldWideName));
                memcpy(di.worldWideName, &desc.IDENTIFIER, min(identifier_length, sizeof(di.worldWideName)));
                OSData* wwn = OSData::withBytes(di.worldWideName, sizeof(di.worldWideName));
                result->setObject(IOWorldWideNameKey, wwn);
                wwn->release();
            }
                break;

            case desc_type(kINQUIRY_Page83_CodeSetBinaryData, kINQUIRY_Page83_IdentifierTypeNAAIdentifier):
            {
                OSData * NAAID = OSData::withBytes(&desc.IDENTIFIER, identifier_length);
                result->setObject(IONAAIDKey, NAAID);
                NAAID->release();
            }
                break;
            case desc_type(kINQUIRY_Page83_CodeSetASCIIData, kINQUIRY_Page83_IdentifierTypeVendorID):
            {
                OSString * VendorID = OSString::withCString(reinterpret_cast<const char *>(&desc.IDENTIFIER),
                                                            identifier_length);
                result->setObject(IOVendorIDKey, VendorID);
                VendorID -> release();
                // We already get Vendor Name and Serial Number using superclass methods
            }
                break;

            default:
                descriptorUnrecognized = true;
                break;
        }
        if (descriptorUnrecognized)
            break;

        p = &desc.IDENTIFIER + identifier_length;
    }

    return result;
}
#endif // defined(USE_INQUIRY_PAGE_83h)



#if defined(USE_INQUIRY_PAGE_89h)
#pragma mark -
#pragma mark Inquiry Page 89h

bool DriverClass::deviceIsPage89SCSI(DTA_DEVICE_INFO &di)
{
    // Test whether device is a SCSI drive by attempting
    // SCSI Inquiry command
    // If it works, as a side effect, parse the Inquiry response
    // and save it in the IO Registry
    bool isPage89SCSI = false;
    size_t transferSize = sizeof(SCSICmd_INQUIRY_Page89_Data);
    IOBufferMemoryDescriptor * md = IOBufferMemoryDescriptor::withCapacity ( transferSize, kIODirectionIn, false );
    if ( md ) {
        void * inquiryResponse = md->getBytesNoCopy( );
        bzero ( inquiryResponse, md->getLength ( ) );
        isPage89SCSI = ( kIOReturnSuccess == inquiryPage89_SCSI( md ) );
        if (isPage89SCSI) {
#if DEBUG
//#error DEBUG is defined
            setProperty(IOInquiryPage89ResponseKey, inquiryResponse, (unsigned int)transferSize);
#endif // DEBUG
            OSDictionary * characteristics =
                parseInquiryPage89Response(static_cast <const unsigned char * >(inquiryResponse), di);
#if DEBUG
            setProperty(IOInquiryPage89CharacteristicsKey, characteristics);
#endif // DEBUG
            characteristics -> release () ;
            characteristics = NULL ;
        }
        md->release ( );
        md = NULL;
    }
    IOLOG_DEBUG("%s[%p]::%s *** end of function, isPage89SCSI is %d\n", getName(), this, __FUNCTION__, isPage89SCSI);
    return isPage89SCSI;
}


IOReturn DriverClass::inquiryPage89_SCSI( IOBufferMemoryDescriptor * md )
{
    IOLOG_DEBUG("%s[%p]::%s", getName(), this, __FUNCTION__);
    UInt16 dataSize = static_cast<UInt16>(md->getLength());
    return __inquiry__EVPD(kINQUIRY_Page89_PageCode, md, dataSize);
}


OSDictionary * DriverClass::parseInquiryPage89Response( const unsigned char * response, DTA_DEVICE_INFO & di)
{
    SCSICmd_INQUIRY_Page89_Data *resp = (SCSICmd_INQUIRY_Page89_Data *)response;

    uint8_t vendorName[sizeof(resp->SAT_VENDOR_IDENTIFICATION)+1];
    memcpy(vendorName, resp->SAT_VENDOR_IDENTIFICATION, sizeof(resp->SAT_VENDOR_IDENTIFICATION));
    vendorName[sizeof(resp->SAT_VENDOR_IDENTIFICATION)] = 0;
    memcpy(di.vendorName, resp->SAT_VENDOR_IDENTIFICATION, sizeof(resp->SAT_VENDOR_IDENTIFICATION));

    uint8_t firmwareRevision[sizeof(resp->SAT_PRODUCT_REVISION_LEVEL)+1];
    memcpy(firmwareRevision, resp->SAT_PRODUCT_REVISION_LEVEL, sizeof(resp->SAT_PRODUCT_REVISION_LEVEL));
    firmwareRevision[sizeof(resp->SAT_PRODUCT_REVISION_LEVEL)] = 0;
    memcpy(di.firmwareRev, resp->SAT_PRODUCT_REVISION_LEVEL, sizeof(di.firmwareRev));


    uint8_t modelNumber[sizeof(resp->SAT_PRODUCT_IDENTIFICATION)+1];
    memcpy(modelNumber, resp->SAT_PRODUCT_IDENTIFICATION, sizeof(resp->SAT_PRODUCT_IDENTIFICATION));
    modelNumber[sizeof(resp->SAT_PRODUCT_IDENTIFICATION)] = 0;
    memcpy(di.modelNum, resp->SAT_PRODUCT_IDENTIFICATION, sizeof(di.modelNum));


    const OSObject * objects[6];
    const OSSymbol * keys[6];

    objects[0] = OSString::withCString( "SCSI");
    keys[0]    = OSSymbol::withCString( IODeviceTypeKey );

    objects[1] = OSString::withCString( (const char *)modelNumber);
    keys[1]    = OSSymbol::withCString( IOModelNumberKey );

    objects[2] = OSString::withCString( (const char *)firmwareRevision);
    keys[2]    = OSSymbol::withCString( IOFirmwareRevisionKey );

    objects[3] = OSString::withCString( (const char *)vendorName);
    keys[3]    = OSSymbol::withCString( IOVendorNameKey );

    objects[4] = OSData::withBytes((const void *)resp->IDENTIFY_DATA, sizeof(resp->IDENTIFY_DATA));
    keys[4]    = OSSymbol::withCString( IOIdentifyCharacteristicsKey );

    objects[5] = OSData::withBytes((const void *)resp, sizeof(SCSICmd_INQUIRY_Page89_Data));
    keys[5]    = OSSymbol::withCString( IOInquiryPage89ResponseKey );

    OSDictionary * result = OSDictionary::withObjects(objects, keys, 6, 6);

    return result;
}
#endif // defined(USE_INQUIRY_PAGE_89h)

#if DEBUG
bool DriverClass::deviceIsPageXXSCSI(uint8_t evpd, const char * key)
{
    // Test whether device is a SCSI drive by attempting
    // SCSI Inquiry command
    // If it works, as a side effect, parse the Inquiry response
    // and save it in the IO Registry
    bool isPageXXSCSI = false;
    size_t transferSize = 256;
    IOBufferMemoryDescriptor * md = IOBufferMemoryDescriptor::withCapacity ( transferSize, kIODirectionIn, false );
    if ( md ) {
        void * inquiryResponse = md->getBytesNoCopy( );
        bzero ( inquiryResponse, md->getLength ( ) );
        isPageXXSCSI = ( kIOReturnSuccess == inquiryPageXX_SCSI(evpd, md ) );
        if (isPageXXSCSI) {
            setProperty(key, inquiryResponse, (unsigned int)transferSize);
        }
        md->release ( );
        md = NULL;
    }
    IOLOG_DEBUG("%s[%p]::%s *** end of function, isPage%02XSCSI is %d\n", getName(), this, __FUNCTION__, evpd, isPageXXSCSI);
    return isPageXXSCSI;
}

IOReturn DriverClass::inquiryPageXX_SCSI(uint8_t evpd, IOBufferMemoryDescriptor * md )
{
    IOLOG_DEBUG("%s[%p]::%s", getName(), this, __FUNCTION__);
    UInt16 dataSize = static_cast<UInt16>(md->getLength());
    return __inquiry__EVPD(evpd, md, dataSize);
}
#endif // DEBUG


#pragma mark -

bool DriverClass::deviceIsTPer_SCSI(DTA_DEVICE_INFO &di)
{
    // Test whether the device is a TPer by
    // retrieving level 0 discovery information via SCSI
    // using SCSI SECURITY PROTOCOL IN
    // If it works, as a side effect, parse the discovery0 response
    // and save it in the IO Registry
    bool isTPer = false;

    isTPer = (kIOReturnSuccess == updatePropertiesInIORegistry_SCSI(di));

    IOLOG_DEBUG("%s[%p]::%s *** end of function, isTPer is %d\n", getName(), this, __FUNCTION__, isTPer);
    return isTPer;
}

/*
 Adapted from void DtaDev::parseDiscovery0Features(const uint8_t * d0Response, DTA_DEVICE_INFO & di)
 */

static
bool parseDiscovery0Features(const uint8_t * d0Response, DTA_DEVICE_INFO & di)
{
    Discovery0Header * hdr = (Discovery0Header *) d0Response;
    uint32_t length = SWAP32(hdr->length);
    if (0 == length) {
        IOLOG_DEBUG("No D0Response");
        return false;
    }
    
    if (sizeof(hdr->length)+length > 8192 ) {
        IOLOG_DEBUG("Level 0 Discovery header length abnormal 0x%08X", length);
        return false;
    }

    IOLOG_DEBUG("Dumping D0Response");
    IOLOGBUFFER_DEBUG(NULL, d0Response, sizeof(hdr->length)+length);
    
    di.TPer = 0;
    di.Locking = 0;
    di.Geometry = 0;
    di.Enterprise = 0;
    di.OPAL10 = 0;
    di.SingleUser = 0;
    di.DataStore = 0;
    di.DataStore_maxTables = 0;
    di.DataStore_maxTableSize = 0;
    di.PYRITE = 0;
    di.PYRITE2 = 0;
    di.RUBY = 0;
    di.ANY_OPAL_SSC = 0;
    di.BlockSID = 0;
    di.NSLocking = 0;
    di.DataRemoval = 0;
    di.VendorSpecific = 0;
    di.Unknown = 0;

    uint8_t *cpos = (uint8_t *) d0Response + sizeof(Discovery0Header);
    uint8_t *epos = (uint8_t *) d0Response + sizeof(hdr->length) + length;
    do {
        Discovery0Features * body = (Discovery0Features *) cpos;
        uint16_t featureCode = SWAP16(body->TPer.featureCode);
        IOLOG_DEBUG("Discovery0 FeatureCode: 0x%04X", featureCode);
        switch (featureCode) { /* could use of the structures here is a common field */
        case FC_TPER: /* TPer */
            IOLOG_DEBUG("TPer Feature");
            di.TPer = 1;
            di.TPer_ACKNACK = body->TPer.acknack;
            di.TPer_async = body->TPer.async;
            di.TPer_bufferMgt = body->TPer.bufferManagement;
            di.TPer_comIDMgt = body->TPer.comIDManagement;
            di.TPer_streaming = body->TPer.streaming;
            di.TPer_sync = body->TPer.sync;
            break;
        case FC_LOCKING: /* Locking*/
            IOLOG_DEBUG("Locking Feature");
            di.Locking = 1;
            di.Locking_locked = body->locking.locked;
            di.Locking_lockingEnabled = body->locking.lockingEnabled;
            di.Locking_lockingSupported = body->locking.lockingSupported;
            di.Locking_MBRDone = body->locking.MBRDone;
            di.Locking_MBREnabled = body->locking.MBREnabled;
            di.Locking_mediaEncrypt = body->locking.mediaEncryption;
            break;
        case FC_GEOMETRY: /* Geometry Features */
            IOLOG_DEBUG("Geometry Feature");
            di.Geometry = 1;
            di.Geometry_align = body->geometry.align;
            di.Geometry_alignmentGranularity = SWAP64(body->geometry.alignmentGranularity);
            di.Geometry_logicalBlockSize = SWAP32(body->geometry.logicalBlockSize);
            di.Geometry_lowestAlignedLBA = SWAP64(body->geometry.lowestAlighedLBA);
            break;
        case FC_ENTERPRISE: /* Enterprise SSC */
            IOLOG_DEBUG("Enterprise SSC Feature");
            di.Enterprise = 1;
            di.ANY_OPAL_SSC = 1;  // TODO: Surely ANY_OPAL_SSC is misnamed.  Enterprise SSC is not an Opal SSC.
            di.Enterprise_rangeCrossing = body->enterpriseSSC.rangeCrossing;
            di.Enterprise_basecomID = SWAP16(body->enterpriseSSC.baseComID);
            di.Enterprise_numcomID = SWAP16(body->enterpriseSSC.numberComIDs);
            break;
        case FC_OPALV100: /* Opal V1 */
            IOLOG_DEBUG("Opal v1.0 SSC Feature");
            di.OPAL10 = 1;
            di.ANY_OPAL_SSC = 1;
            di.OPAL10_basecomID = SWAP16(body->opalv100.baseComID);
            di.OPAL10_numcomIDs = SWAP16(body->opalv100.numberComIDs);
            break;
        case FC_SINGLEUSER: /* Single User Mode */
            IOLOG_DEBUG("Single User Mode Feature");
            di.SingleUser = 1;
            di.SingleUser_all = body->singleUserMode.all;
            di.SingleUser_any = body->singleUserMode.any;
            di.SingleUser_policy = body->singleUserMode.policy;
            di.SingleUser_lockingObjects = SWAP32(body->singleUserMode.numberLockingObjects);
            break;
        case FC_DATASTORE: /* Datastore Tables */
            IOLOG_DEBUG("Datastore Feature");
            di.DataStore = 1;
            di.DataStore_maxTables = SWAP16(body->datastore.maxTables);
            di.DataStore_maxTableSize = SWAP32(body->datastore.maxSizeTables);
            di.DataStore_alignment = SWAP32(body->datastore.tableSizeAlignment);
            break;
        case FC_OPALV200: /* OPAL V200 */
            IOLOG_DEBUG("Opal v2.0 SSC Feature");
            di.ANY_OPAL_SSC = 1;
            di.OPAL20 = 1;
            di.OPAL20_basecomID = SWAP16(body->opalv200.baseCommID);
            di.OPAL20_initialPIN = body->opalv200.initialPIN;
            di.OPAL20_revertedPIN = body->opalv200.revertedPIN;
            di.OPAL20_numcomIDs = SWAP16(body->opalv200.numCommIDs);
            di.OPAL20_numAdmins = SWAP16(body->opalv200.numlockingAdminAuth);
            di.OPAL20_numUsers = SWAP16(body->opalv200.numlockingUserAuth);
            di.OPAL20_rangeCrossing = body->opalv200.rangeCrossing;
            break;
        case FC_PYRITE: /* PYRITE 0x302 */
            IOLOG_DEBUG("Pyrite SSC Feature");
            di.ANY_OPAL_SSC = 1;
            di.PYRITE= 1;
            di.PYRITE_version = body->opalv200.version;
            di.PYRITE_basecomID = SWAP16(body->opalv200.baseCommID);
            di.PYRITE_initialPIN = body->opalv200.initialPIN;
            di.PYRITE_revertedPIN = body->opalv200.revertedPIN;
            di.PYRITE_numcomIDs = SWAP16(body->opalv200.numCommIDs);
            // temp patch ; use OPAL2 diskinfo if needed; need create pyrite class in the future
            di.OPAL20_basecomID = SWAP16(body->opalv200.baseCommID);
            di.OPAL20_initialPIN = body->opalv200.initialPIN;
            di.OPAL20_revertedPIN = body->opalv200.revertedPIN;
            di.OPAL20_numcomIDs = SWAP16(body->opalv200.numCommIDs);
            di.OPAL20_numAdmins = 1; // SWAP16(body->opalv200.numlockingAdminAuth);
            di.OPAL20_numUsers = 2; // SWAP16(body->opalv200.numlockingUserAuth);
            di.OPAL20_rangeCrossing = body->opalv200.rangeCrossing;
            di.OPAL20_version = body->opalv200.version;
            // does pyrite has data store. no feature set for data store default vaule 128K
            di.DataStore = 1;
            di.DataStore_maxTables = 1; //  SWAP16(body->datastore.maxTables);
            di.DataStore_maxTableSize = 131072; //  10485760 (OPAL2); // SWAP32(body->datastore.maxSizeTables);
            di.DataStore_alignment = 1; //  SWAP32(body->datastore.tableSizeAlignment);
            break;
        case FC_PYRITE2: /* PYRITE 2 0x303 */
            IOLOG_DEBUG("Pyrite 2 SSC Feature");
            di.ANY_OPAL_SSC = 1;
            di.PYRITE2 = 1;
            di.PYRITE2_version = body->opalv200.version;
            di.PYRITE2_basecomID = SWAP16(body->opalv200.baseCommID);
            di.PYRITE2_initialPIN = body->opalv200.initialPIN;
            di.PYRITE2_revertedPIN = body->opalv200.revertedPIN;
            di.PYRITE2_numcomIDs = SWAP16(body->opalv200.numCommIDs);
            // temp patch ; use OPAL2 diskinfo if needed; need create pyrite class in the future
            di.OPAL20_basecomID = SWAP16(body->opalv200.baseCommID);
            di.OPAL20_initialPIN = body->opalv200.initialPIN;
            di.OPAL20_revertedPIN = body->opalv200.revertedPIN;
            di.OPAL20_numcomIDs = SWAP16(body->opalv200.numCommIDs);
            di.OPAL20_numAdmins = 1; // SWAP16(body->opalv200.numlockingAdminAuth);
            di.OPAL20_numUsers = 2; // SWAP16(body->opalv200.numlockingUserAuth);
            di.OPAL20_rangeCrossing = body->opalv200.rangeCrossing;
            di.OPAL20_version = body->opalv200.version;
            // does pyrite has data store. no feature set for data store default vaule 128K
            di.DataStore = 1;
            di.DataStore_maxTables = 1; //  SWAP16(body->datastore.maxTables);
            di.DataStore_maxTableSize = 131072; //  10485760 (OPAL2); // SWAP32(body->datastore.maxSizeTables);
            di.DataStore_alignment = 1; //  SWAP32(body->datastore.tableSizeAlignment);
            break;
        case FC_RUBY: /* RUBY 0x304 */
            IOLOG_DEBUG("Ruby SSC Feature");
            di.ANY_OPAL_SSC = 1;
            di.RUBY = 1;
            di.RUBY_version = body->opalv200.version;
            di.RUBY_basecomID = SWAP16(body->opalv200.baseCommID);
            di.RUBY_initialPIN = body->opalv200.initialPIN;
            di.RUBY_revertedPIN = body->opalv200.revertedPIN;
            di.RUBY_numcomIDs = SWAP16(body->opalv200.numCommIDs);
            di.RUBY_numAdmins = SWAP16(body->opalv200.numlockingAdminAuth);
            di.RUBY_numUsers = SWAP16(body->opalv200.numlockingUserAuth);
            // temp patch ; use OPAL2 diskinfo if needed; need create pyrite class in the future
            di.OPAL20_basecomID = SWAP16(body->opalv200.baseCommID);
            di.OPAL20_initialPIN = body->opalv200.initialPIN;
            di.OPAL20_revertedPIN = body->opalv200.revertedPIN;
            di.OPAL20_numcomIDs = SWAP16(body->opalv200.numCommIDs);
            di.OPAL20_numAdmins = 1; // SWAP16(body->opalv200.numlockingAdminAuth);
            di.OPAL20_numUsers = 2; // SWAP16(body->opalv200.numlockingUserAuth);
            di.OPAL20_rangeCrossing = body->opalv200.rangeCrossing;
            di.OPAL20_version = body->opalv200.version;
            // does pyrite has data store. no feature set for data store default vaule 128K
            di.DataStore = 1;
            di.DataStore_maxTables = 1; //  SWAP16(body->datastore.maxTables);
            di.DataStore_maxTableSize = 131072; //  10485760 (OPAL2); // SWAP32(body->datastore.maxSizeTables);
            di.DataStore_alignment = 1; //  SWAP32(body->datastore.tableSizeAlignment);

            break;
        case FC_BlockSID: /* Block SID 0x402 */
            IOLOG_DEBUG("Block SID Feature");
            di.BlockSID = 1;
            di.BlockSID_BlockSIDState = body->blocksidauth.BlockSIDState;
            di.BlockSID_SIDvalueState = body->blocksidauth.SIDvalueState;
            di.BlockSID_HardReset = body->blocksidauth.HardReset;
            break;
        case FC_NSLocking:
            IOLOG_DEBUG("Namespace Locking Feature");
            di.NSLocking = 1;
            di.NSLocking_version = body->Configurable_Namespace_LockingFeature.version;
            di.Max_Key_Count = body->Configurable_Namespace_LockingFeature.Max_Key_Count;
            di.Unused_Key_Count = body->Configurable_Namespace_LockingFeature.Unused_Key_Count;
            di.Max_Range_Per_NS = body->Configurable_Namespace_LockingFeature.Max_Range_Per_NS;
            break;
        case FC_DataRemoval: /* Data Removal mechanism 0x404 */
            IOLOG_DEBUG("Data Removal Feature");
            di.DataRemoval = 1;
            di.DataRemoval_version = body->dataremoval.version;
            di.DataRemoval_Mechanism = body->dataremoval.DataRemoval_Mechanism;
            di.DataRemoval_TimeFormat_Bit5 = body->dataremoval.DataRemoval_TimeFormat_Bit5;
            di.DataRemoval_Time_Bit5 = body->dataremoval.DataRemoval_Time_Bit5;
            di.DataRemoval_TimeFormat_Bit5 = body->dataremoval.DataRemoval_TimeFormat_Bit4;
            di.DataRemoval_Time_Bit5 = body->dataremoval.DataRemoval_Time_Bit4;
            di.DataRemoval_TimeFormat_Bit5 = body->dataremoval.DataRemoval_TimeFormat_Bit3;
            di.DataRemoval_Time_Bit5 = body->dataremoval.DataRemoval_Time_Bit3;
            di.DataRemoval_TimeFormat_Bit5 = body->dataremoval.DataRemoval_TimeFormat_Bit2;
            di.DataRemoval_Time_Bit5 = body->dataremoval.DataRemoval_Time_Bit2;
            di.DataRemoval_TimeFormat_Bit5 = body->dataremoval.DataRemoval_TimeFormat_Bit1;
            di.DataRemoval_Time_Bit5 = body->dataremoval.DataRemoval_Time_Bit1;
            di.DataRemoval_TimeFormat_Bit5 = body->dataremoval.DataRemoval_TimeFormat_Bit0;
            di.DataRemoval_Time_Bit5 = body->dataremoval.DataRemoval_Time_Bit0;
            break;
        default:
            if (FC_Min_Vendor_Specific <= featureCode) {
                // silently ignore vendor specific segments as there is no public doc on them
                di.VendorSpecific += 1;
                IOLOG_DEBUG("Vendor Specific Feature Code 0x%04X", featureCode);
            }
            else {
                di.Unknown += 1;
                IOLOG_DEBUG("Unknown Feature Code 0x%04X", featureCode);
                /* should do something here */
            }
            break;
        }
        cpos = cpos + (body->TPer.length + 4);
    }
    while (cpos < epos);
    // do adjustment for No Additional data store case
    if (!di.DataStore  || !di.DataStore_maxTables || !di.DataStore_maxTableSize) {
        di.DataStore_maxTableSize = 10 * 1024 * 1024;
    }

    return true;
}


//IOReturn DriverClass::securityProctocolOut_SCSI( IOBufferMemoryDescriptor * md )
// {
//     IOLOG_DEBUG("%s[%p]::%s", getName(), this, __FUNCTION__);
//
//     static SCSICommandDescriptorBlock securityProtocolOutCDB_SCSI =
//       { kSCSICmd_SECURITY_PROTOCOL_OUT,    // Byte  0  SECURITY PROTOCOL OUT B5h
//         0x01,    // Byte  1  Security Protocol
//         0x00,    // Byte  2  Security Protocol Specfic
//         0x00,    // Byte  3  Security Protocol Specfic
//         0x80,    // Byte  4  INC_512 | reserved
//         0x00,    // Byte  5  Reserved
//         0x00,    // Byte  6  Allocation Length (MSB)
//         0x00,    // Byte  7  Allocation Length
//         0x00,    // Byte  8  Allocation Length
//         0x04,    // Byte  9  Allocation Length (LSB)
//         0x00,    // Byte 10  Reserved
//         0x00,    // Byte 11  Control
//       };
//     return PerformSCSICommand(securityProtocolOutCDB_SCSI, md);
// }


void DriverClass::updateIORegistryFromD0Response(const uint8_t * d0Response, DTA_DEVICE_INFO & di)
{
    parseDiscovery0Features(d0Response, di);
}

IOReturn DriverClass::updatePropertiesInIORegistryWithDiscovery0CDB(SCSICommandDescriptorBlock cdb, DTA_DEVICE_INFO & di)
{
    IOBufferMemoryDescriptor * md = IOBufferMemoryDescriptor::withCapacity ( DISCOVERY0_RESPONSE_SIZE,
                                                                             kIODirectionIn, false );
    if (NULL == md) {
        return kIOReturnInternalError;
    }

    UInt8 * d0Response = ( UInt8 * ) md->getBytesNoCopy ( );
    bzero ( d0Response, md->getLength ( ) );

    IOReturn result = PerformSCSICommand(cdb, md);

    updateIORegistryFromD0Response(d0Response, di);

#if DEBUG
    OSData * d0Buffer = OSData::withBytes(d0Response, DISCOVERY0_RESPONSE_SIZE);
    if ( getProperty(IODiscovery0ResponseKey) )
        removeProperty(IODiscovery0ResponseKey);
    setProperty(IODiscovery0ResponseKey, d0Buffer);
    d0Buffer -> release( );
#endif // DEBUG

    md->release ( );

    return result;
}


IOReturn DriverClass::updatePropertiesInIORegistry_SCSI( DTA_DEVICE_INFO & di )
{
    IOLOG_DEBUG("%s[%p]::%s", getName(), this, __FUNCTION__);

    static SCSICommandDescriptorBlock acquireDiscovery0ResponseCDB_SCSI =
      { kSCSICmd_SECURITY_PROTOCOL_IN,    // Byte  0  SECURITY PROTOCOL IN  A2h
        0x01,    // Byte  1  Security Protocol
        0x00,    // Byte  2  Security Protocol Specfic (COMID MSB)
        0x01,    // Byte  3  Security Protocol Specfic (COMID LSB)
        0x80,    // Byte  4  INC_512 | reserved
        0x00,    // Byte  5  Reserved
        0x00,    // Byte  6  Allocation Length (MSB)
        0x00,    // Byte  7  Allocation Length
        0x00,    // Byte  8  Allocation Length
        0x04,    // Byte  9  Allocation Length (LSB)
        0x00,    // Byte 10  Reserved
        0x00,    // Byte 11  Control
      };
    return updatePropertiesInIORegistryWithDiscovery0CDB(acquireDiscovery0ResponseCDB_SCSI, di);
}

IOReturn DriverClass::updatePropertiesInIORegistry_SAT( DTA_DEVICE_INFO & di )
{
    IOLOG_DEBUG("%s[%p]::%s", getName(), this, __FUNCTION__);

    static SCSICommandDescriptorBlock acquireDiscovery0ResponseCDB_SAT =
      { kSCSICmd_ATA_PASS_THROUGH,    // Byte  0  ATA PASS-THROUGH (12)
        //    /*
        //     * Byte 1 is the protocol 4 = PIO IN and 5 = PIO OUT, in bits 4-1 (i.e., not bit zero)
        //     */
        PIO_DataIn<<1, // Byte  1  ATA Protocol = PIO Data-In
        //    /*
        //     * Byte 2 is:
        //     * bits 7-6 OFFLINE - Amount of time the command can take the bus offline
        //     * bit 5    CK_COND - If set the command will always return a condition check
        //     * bit 4    RESERVED
        //     * bit 3    T_DIR   - transfer direction 1 in, 0 out
        //     * bit 2    BYTE_BLock  1 = transfer in blocks, 0 transfer in bytes
        //     * bits 1-0 T_LENGTH -  2 = the length is in the sector count field
        //     */
        0x0E,    // Byte  2  OFF_LINE=0, CK_CON=0, T_DIR=1, BYT_BLOK=1=blocks, T_LENGTH=2 (in sector_count)
        0x01,    // Byte  3  FEATURES SECURITY PROTOCOL = 1 (TCG)  -- so far, always use ATA SEC PROTOCOL 1
        0x04,    // Byte  4  SECTOR_COUNT = 4
        0x00,    // Byte  5  LBA_LOW  -- TCG reserved, always zero
        0x01,    // Byte  6  LBA_MID  -- COMID=1 (low byte)
        0x00,    // Byte  7  LBA_HIGH -- COMID=0 (high byte)
        0x00,    // Byte  8  DEVICE -- so far always zero
        kATACmd_TRUSTED_RECEIVE_PIO,    // Byte  9  COMMAND=ATA TRUSTED RECEIVE (PIO)  TODO: symbolicate
        0x00,    // Byte 10  Reserved -- zero
        0x00,    // Byte 11  CONTROL -- so far always zero
      };
    return updatePropertiesInIORegistryWithDiscovery0CDB(acquireDiscovery0ResponseCDB_SAT, di);
}

OSDictionary * DriverClass::parseIdentifyResponse( const unsigned char * response, DTA_DEVICE_INFO & di)
{
    const IDENTIFY_RESPONSE & resp = *(IDENTIFY_RESPONSE *)response;

    parseATIdentifyResponse(&resp, &di);

    const OSObject * objects[6];
    const OSSymbol * keys[6];

    objects[0] = OSNumber::withNumber( (unsigned long long)(resp.TCGOptions[1]<<8 | resp.TCGOptions[0]), 16);
    keys[0]    = OSSymbol::withCString( IOTCGOptionsKey );

    objects[1] = OSString::withCString( resp.devType ? "OTHER" : "ATA");
    keys[1]    = OSSymbol::withCString( IODeviceTypeKey );

    objects[2] = OSString::withCString( (const char *)di.serialNum);
    keys[2]    = OSSymbol::withCString( IOSerialNumberKey );

    objects[3] = OSString::withCString( (const char *)di.modelNum);
    keys[3]    = OSSymbol::withCString( IOModelNumberKey );

    objects[4] = OSString::withCString( (const char *)di.firmwareRev);
    keys[4]    = OSSymbol::withCString( IOFirmwareRevisionKey );

    objects[5] = OSData::withBytes((const void *)di.worldWideName, sizeof(di.worldWideName));
    keys[5]    = OSSymbol::withCString( IOWorldWideNameKey );

    OSDictionary * result = OSDictionary::withObjects(objects, keys, 6, 6);

    return result;
}




//*****************
// private
//*****************

IOReturn DriverClass::prepareSCSICommand(SCSICommandDescriptorBlock cdb,
                                         UInt8 cdbSize,
                                         IOBufferMemoryDescriptor * md,
                                         SCSITaskIdentifier request,
                                         UInt8 direction,
                                         UInt64 count)
{
    if ( ! md ) {
        IOLOG_DEBUG( "%s::%s data BufferDescriptor error", getName ( ), __FUNCTION__);
        return kIOReturnVMError;                                            // !!immediate exit, do not pass through!!
    }


    IOLOG_DEBUG("%s::%s, CDB is ", getName ( ), __FUNCTION__ );
    IOLOGBUFFER_DEBUG(NULL, cdb, cdbSize);

    switch (cdbSize) {
        case 6:
            SetCommandDescriptorBlock( request,
                                       cdb[ 0], cdb[ 1], cdb[ 2], cdb[ 3], cdb[ 4], cdb[ 5]);
            break;
        case 12:
            SetCommandDescriptorBlock( request,
                                       cdb[ 0], cdb[ 1], cdb[ 2], cdb[ 3], cdb[ 4], cdb[ 5], cdb[ 6], cdb[ 7],
                                       cdb[ 8], cdb[ 9], cdb[10], cdb[11]);
            break;
        case 16:
            SetCommandDescriptorBlock( request,
                                       cdb[ 0], cdb[ 1], cdb[ 2], cdb[ 3], cdb[ 4], cdb[ 5], cdb[ 6], cdb[ 7],
                                       cdb[ 8], cdb[ 9], cdb[10], cdb[11], cdb[12], cdb[13], cdb[14], cdb[15]);
            break;
        default:
            return kIOReturnBadArgument;
    }

    md->prepare();
    if (direction == kSCSIDataTransfer_FromTargetToInitiator) {
        bzero(md->getBytesNoCopy(), md->getLength());
    }
    SetDataBuffer(request, md);
    SetTimeoutDuration(request, SED_TIMEOUT);
    SetDataTransferDirection(request, direction);
    SetRequestedDataTransferCount(request, count);

    return kIOReturnSuccess;
}


bool GetAutoSenseData (SCSITaskIdentifier request, SCSI_Sense_Data *  senseData, UInt8 senseDataSize );
UInt8 GetAutoSenseDataSize(SCSITaskIdentifier request );



void DriverClass::printSenseData ( SCSI_Sense_Data * sense )
{
    char    str[256];
    UInt8   key, ASC, ASCQ;

    key     = sense->SENSE_KEY & 0x0F;
    ASC     = sense->ADDITIONAL_SENSE_CODE;
    ASCQ    = sense->ADDITIONAL_SENSE_CODE_QUALIFIER;
    snprintf ( str, 64, "Key: $%02hhx, ASC: $%02hhx, ASCQ: $%02hhx  ", key, ASC, ASCQ );
    IOLOG_DEBUG( "%s::%s sense data is:   %s", getName ( ), __FUNCTION__, str);
}

IOReturn DriverClass::completeSCSICommand(IOBufferMemoryDescriptor * md,
                                          SCSITaskIdentifier request,
                                          SCSIServiceResponse serviceResponse,
                                          uint64_t * pTransferSize)
{
    SCSITaskStatus taskStatus = kSCSITaskStatus_No_Status;
    SCSI_Sense_Data senseData;
    UInt8 senseDataSize;

    IOReturn ret = kIOReturnError;

    taskStatus = GetTaskStatus(request);

    IOLOG_DEBUG("%s[%p]::%s(): service response: %u, task status: %u\n",
          getName(), this, __FUNCTION__, serviceResponse, taskStatus);

    if ( serviceResponse == kSCSIServiceResponse_TASK_COMPLETE ) {
        IOLOG_DEBUG( "%s::%s service response is Task_Complete!!!\n", getName ( ), __FUNCTION__);

        switch (taskStatus) {
            case kSCSITaskStatus_GOOD:
                IOLOG_DEBUG( "%s::%s task status is GOOD!", getName ( ), __FUNCTION__);
                ret = kIOReturnSuccess;
                if (pTransferSize) {
                    *pTransferSize = GetRealizedDataTransferCount(request);
                }
                break;
            case kSCSITaskStatus_CHECK_CONDITION:
                IOLOG_DEBUG( "%s::%s task status is CHECK_CONDITION", getName ( ), __FUNCTION__);
                senseDataSize = GetAutoSenseDataSize( request );

                GetAutoSenseData(request, &senseData, senseDataSize);

                if (senseData.VALID_RESPONSE_CODE & kSENSE_DATA_VALID) {
                    printSenseData( &senseData );
                } else {
                    IOLOG_DEBUG( "%s::%s senseData is Invalid!!!, size is %d",
                                getName ( ), __FUNCTION__, senseDataSize );
                    IOLOG_DEBUG( "%s::%s senseData is %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
                                getName ( ), __FUNCTION__,
                                senseData.VALID_RESPONSE_CODE,
                                senseData.SEGMENT_NUMBER,
                                senseData.SENSE_KEY,
                                senseData.INFORMATION_1,
                                senseData.INFORMATION_2,
                                senseData.INFORMATION_3,
                                senseData.INFORMATION_4,
                                senseData.ADDITIONAL_SENSE_LENGTH,
                                senseData.COMMAND_SPECIFIC_INFORMATION_1,
                                senseData.COMMAND_SPECIFIC_INFORMATION_2,
                                senseData.COMMAND_SPECIFIC_INFORMATION_3,
                                senseData.COMMAND_SPECIFIC_INFORMATION_4,
                                senseData.ADDITIONAL_SENSE_CODE,
                                senseData.ADDITIONAL_SENSE_CODE_QUALIFIER,
                                senseData.FIELD_REPLACEABLE_UNIT_CODE,
                                senseData.SKSV_SENSE_KEY_SPECIFIC_MSB,
                                senseData.SENSE_KEY_SPECIFIC_MID,
                                senseData.SENSE_KEY_SPECIFIC_LSB );
                    printSenseData( &senseData );
                }
                break;
            default:
                IOLOG_DEBUG( "%s::%s task status is some other task status 0x%02X", getName ( ), __FUNCTION__,
                            (uint8_t)taskStatus);
                break;
        }
    }
    else
    {
        IOLOG_DEBUG( "%s::%s service response is Task Not Complete", getName ( ), __FUNCTION__);
    }
    md->complete();

    return ret;
}

#if !defined(MIN_PROBE)
#define MIN_PROBE 15001
#endif // !defined(MIN_PROBE)

#undef MIN_PROBE
#define MIN_PROBE 0x3eee

IOService* DriverClass::probe(IOService* provider, SInt32* score)
{
    // tried using getName(), this, __FUNCTION__ in snprint
    // and kernel panics.
    // it does work directly, however, in IOLOG
    // does IOLOG do some testing and still perform (a different sort?) getName() on the
    // driver, even though it is not yet published???
    // (just hand-waving)
    //
    // cannot determine applicability of our driver to this device
    // in the probe function because device support is not initialized

    IOLOG_DEBUG("%s[%p]::%s(provider is %s, score is %d)\n",
          getName(), this, __FUNCTION__, provider->getName(), (int)*score);
#if defined(MIN_PROBE)
    if ((*score) < MIN_PROBE) {
        *score = MIN_PROBE;
        IOLOG_DEBUG("%s[%p]::%s(provider is %s, score raised to %d)\n",
              getName(), this, __FUNCTION__, provider->getName(), (int)*score);
    }
#endif // defined(MIN_PROBE)
#if DRIVER_DEBUG
#else // !defined(MIN_PROBE)
#pragma unused(provider)
#pragma unused(score)
#endif
    return this;
}


//**************
// apple overrides - debugging only
//**************

#if DRIVER_DEBUG


void DriverClass::TerminateDeviceSupport( void )
{
    IOLOG_DEBUG("%s[%p]::%s *** before super\n", getName(), this, __FUNCTION__ );
    super::TerminateDeviceSupport();
    IOLOG_DEBUG("%s[%p]::%s *** after super \n", getName(), this, __FUNCTION__ );
}

void DriverClass::StartDeviceSupport( void )
{
    IOLOG_DEBUG("%s[%p]::%s *** before super\n", getName(), this, __FUNCTION__ );
    super::StartDeviceSupport();
    IOLOG_DEBUG("%s[%p]::%s *** after super \n", getName(), this, __FUNCTION__ );
}

void DriverClass::StopDeviceSupport( void )
{
    IOLOG_DEBUG("%s[%p]::%s *** before super\n", getName(), this, __FUNCTION__ );
    super::StopDeviceSupport();
    IOLOG_DEBUG("%s[%p]::%s *** after super \n", getName(), this, __FUNCTION__ );
}


bool DriverClass::attach(IOService* provider)
{
    IOLOG_DEBUG("%s[%p]::%s *** before super\n", getName(), this, __FUNCTION__ );
    return super::attach(provider);
}

void DriverClass::detach(IOService* provider)
{
    IOLOG_DEBUG("%s[%p]::%s *** before super\n", getName(), this, __FUNCTION__);
    super::detach(provider);
    IOLOG_DEBUG("%s[%p]::%s *** after super \n", getName(), this, __FUNCTION__ );
}

bool DriverClass::open(IOService *  forClient,
                       IOOptionBits options,
                       void *       arg)
{
    IOLOG_DEBUG("%s[%p]::%s *** before super\n", getName(), this, __FUNCTION__ );
    return super::open(forClient, options, arg);
}

void DriverClass::close(IOService *  forClient,
                        IOOptionBits options)
{
    IOLOG_DEBUG("%s[%p]::%s *** before super\n", getName(), this, __FUNCTION__ );
    super::close(forClient, options);
    IOLOG_DEBUG("%s[%p]::%s *** after super\n", getName(), this, __FUNCTION__ );
}

void DriverClass::stop(IOService* provider)
{
    IOLOG_DEBUG("%s[%p]::%s *** before super\n", getName(), this, __FUNCTION__ );
    super::stop(provider);
    IOLOG_DEBUG("%s[%p]::%s *** after super \n", getName(), this, __FUNCTION__ );
}


bool DriverClass::init(OSDictionary* dictionary)
{

    if (!super::init(dictionary)) {
        IOLOG_DEBUG("%s[%p]::%s *** after super, no dict \n", getName(), this, __FUNCTION__ );
        return false;
    }
    IOLOG_DEBUG("%s[%p]::%s *** after super \n", getName(), this, __FUNCTION__ );

    return true;
}

void DriverClass::free(void)
{
    IOLOG_DEBUG("%s[%p]::%s *** before super\n", getName(), this, __FUNCTION__ );
    super::free();
}

// willTerminate is called at the beginning of the termination process. It is a notification
// that a provider has been terminated, sent before recursing up the stack, in root-to-leaf order.
//
// This is where any pending I/O should be terminated. At this point the user client has been marked
// inactive and any further requests from the user process should be returned with an error.
bool DriverClass::willTerminate(IOService* provider, IOOptionBits options)
{
    bool	success;
    IOLOG_DEBUG("%s[%p]::%s(%p, %u)\n", getName(), this, __FUNCTION__, provider, (unsigned int)options);
    success = super::willTerminate(provider, options);
    IOLOG_DEBUG("%s[%p]::%s *** after super \n", getName(), this, __FUNCTION__ );
    return success;
}

// didTerminate is called at the end of the termination process. It is a notification
// that a provider has been terminated, sent after recursing up the stack, in leaf-to-root order.
bool DriverClass::didTerminate(IOService* provider, IOOptionBits options, bool* defer)
{
    bool	success;
    IOLOG_DEBUG("%s[%p]::%s(%p, %u, %p)\n", getName(), this, __FUNCTION__, provider, (unsigned int)options, defer);
    IOLOG_DEBUG("%s[%p]::%s *** before super\n", getName(), this, __FUNCTION__ );
    success = super::didTerminate(provider, options, defer);
    IOLOG_DEBUG("%s[%p]::%s *** after super \n", getName(), this, __FUNCTION__ );  // never get here???
    return success;
}

// Production drivers will rarely need to override terminate. Termination processing should be done in
// willTerminate or didTerminate instead.
bool DriverClass::terminate(IOOptionBits options)
{
    bool	success;
    IOLOG_DEBUG("%s[%p]::%s *** before super\n", getName(), this, __FUNCTION__ );
    success = super::terminate(options);
    IOLOG_DEBUG("%s[%p]::%s *** after super \n", getName(), this, __FUNCTION__ );
    return success;
}


// We override finalize only to log that it has been called to make it easier to follow the driver's lifecycle.
// Production drivers will rarely need to override finalize.
bool DriverClass::finalize(IOOptionBits options)
{
    bool	success;
    IOLOG_DEBUG("%s[%p]::%s *** before super\n", getName(), this, __FUNCTION__ );
    success = super::finalize(options);
    IOLOG_DEBUG("%s[%p]::%s *** after super \n", getName(), this, __FUNCTION__ );
    return success;
}

#endif // if DRIVER_DEBUG
