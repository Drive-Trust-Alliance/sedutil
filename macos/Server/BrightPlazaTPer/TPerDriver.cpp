//
//  TPerDriver.cpp
//  SedUserClient
//
//  Created by Jackie Marks on 9/21/15.
//  Copyright © 2015 Bright Plaza Inc. All rights reserved.
//


//#pragma clang diagnostic push
//#pragma clang diagnostic ignored "-Wsign-conversion"
//#include <libkern/libkern.h>
//#pragma clang diagnostic pop


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wdocumentation-html"
#include <IOKit/scsi/SCSITask.h>
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#include <IOKit/IOUserClient.h>
#include <IOKit/IOBufferMemoryDescriptor.h>
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#include <IOKit/storage/IOBlockStorageDriver.h>
#pragma clang diagnostic pop

#include <SEDKernelInterface/UserKernelShared.h>
#include "TPerDriver.h"
#include "CDBAccess.hpp"
#include "PrintBuffer.h"

#include "kernel_debug.h"

#define super IOSCSIPeripheralDeviceType00

OSDefineMetaClassAndStructors(com_brightplaza_BrightPlazaTPer, IOSCSIPeripheralDeviceType00 ) //IOService)


//*****************
// apple IOService override
//*****************
bool DriverClass::start(IOService* provider)
{
    bool ret;
    IOLOG_DEBUG("%s[%p]::%s(provider = %p), provider->getName() = %s\n",
                getName(), this, __FUNCTION__, provider, provider->getName());
    if ( (ret = super::start(provider) ) ) {
        registerService();
    }
    else if (getProperty(IOTCGPropertiesKey)) { // pointer is not null => super::InitializeDeviceSupport returned true
                                             // and deviceIsTPer returned false
        IOLOG_DEBUG("%s[%p]::%s - leaving start, Device Support was initialized but is NOT TPer, calling stop\n",
              getName(), this, __FUNCTION__);

        super::stop(provider);   // releases provider - having
    }
    // if neither case, then super::InitializeDeviceSupport returned false and we don't need to call stop
    
    IOLOG_DEBUG("%s[%p]::%s - leaving start, returning %d\n",
          getName(), this, __FUNCTION__, ret);
    return ret;
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
    return super::InitializeDeviceSupport() && deviceIsTPer();
}

//*****************
// private
//*****************

// called from InitializeDeviceSupport
// performs ATA IDENTIFY DEVICE on drive
// sets "TCG Supported" to true if this is a TCG-encrypted drive
// and our driver should remain attached to the drive
bool DriverClass::deviceIsTPer( void )
{
    bool isTPer = false;
    IOBufferMemoryDescriptor * md = IOBufferMemoryDescriptor::withCapacity ( IDENTIFY_RESPONSE_SIZE, kIODirectionIn, false );
    if ( md ) {
        void * identifyResponse = md->getBytesNoCopy ( );
        bzero ( identifyResponse, md->getLength ( ) );
        
        identifyDevice( md );
        
        if (0xA5==((UInt8 *)identifyResponse)[510]) {  // checksum is present
            UInt8 checksum=0;
            for (UInt8 * p = ((UInt8 *)identifyResponse), * end = ((UInt8 *)identifyResponse) + 512; p<end ; p++)
                checksum=(UInt8)(checksum+(*p));
            if (checksum != 0) {
                IOLOG_DEBUG("%s[%p]::%s *** IDENTIFY DEVICE response checksum failed *** !!!\n",
                            getName(), this, __FUNCTION__);
            }
        }
        
        OSDictionary * characteristics = parseIdentifyResponse(((UInt8 *)identifyResponse));
        OSNumber * tcgOptions = OSRequiredCast(OSNumber,characteristics->getObject(IOTCGOptionsKey));
        // TCG Options word is valid and TCG Features present bit is one
        isTPer = (tcgOptions->unsigned16BitValue() & 0xE001) == 0x4001;
        if (isTPer) {
            setProperty(IOIdentifyDeviceResponseKey, identifyResponse, IDENTIFY_RESPONSE_SIZE);
            setProperty(IOIdentifyCharacteristicsKey, characteristics);
            IOLOG_DEBUG("%s[%p]::%s characteristics->getRetainCount() => %d",
                        getName(), this, __FUNCTION__, characteristics->getRetainCount());
            updateLockingPropertiesInIORegistry();
        }
        characteristics -> release () ;
        characteristics = NULL ;
        
        md->release ( );
        md = NULL;
    }
    

    IOLOG_DEBUG("%s[%p]::%s *** end of function, isTPer is %d\n", getName(), this, __FUNCTION__, isTPer);
    return isTPer;
}


IOReturn DriverClass::identifyDevice( IOBufferMemoryDescriptor * md )
{
    IOLOG_DEBUG("%s[%p]::%s\n", getName(), this, __FUNCTION__);

    static SCSICommandDescriptorBlock identifyCDB =
      { 0xA1,    // Byte  0  ATA PASS-THROUGH (12)
        //    /*
        //     * Byte 1 is the protocol 4 = PIO IN and 5 = PIO OUT, in bits 4-1 (i.e., not bit zero)
        //     */
        0x04<<1, // Byte  1  ATA Protocol = PIO Data-In
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
        0xEC,    // Byte  9  COMMAND=ATA IDENTIFY DEVICE  TODO: symbolicate
        0x00,    // Byte 10  Reserved -- zero
        0x00,    // Byte 11  CONTROL -- so far always zero
      };
    return SCSIPassthrough(identifyCDB, md);
}

IOReturn DriverClass::discovery0( IOBufferMemoryDescriptor * md )
{
    IOLOG_DEBUG("%s[%p]::%s\n", getName(), this, __FUNCTION__);
    
    static SCSICommandDescriptorBlock discovery0CDB =
      { 0xA1,    // Byte  0  ATA PASS-THROUGH (12)
        //    /*
        //     * Byte 1 is the protocol 4 = PIO IN and 5 = PIO OUT, in bits 4-1 (i.e., not bit zero)
        //     */
        0x04<<1, // Byte  1  ATA Protocol = PIO Data-In
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
        0x5C,    // Byte  9  COMMAND=ATA TRUSTED RECEIVE (PIO)  TODO: symbolicate
        0x00,    // Byte 10  Reserved -- zero
        0x00,    // Byte 11  CONTROL -- so far always zero
      };
    return SCSIPassthrough(discovery0CDB, md);
}



IOReturn DriverClass::updateLockingPropertiesInIORegistry( void )
{
    IOReturn result = kIOReturnError;
    uint8_t lockingByte = 0;
    bool MBREnabled = false;
    bool MBRDone = false;
    bool lockingEnabled = false;
    bool locked = false;
    UInt8 * d0Response = NULL;
    const OSObject * objects[4];
    const OSSymbol * keys[4];
    
    IOBufferMemoryDescriptor * md;
    
    if ( (md = IOBufferMemoryDescriptor::withCapacity ( DISCOVERY0_RESPONSE_SIZE,
                                                       kIODirectionIn, false ) ) ) {
        d0Response = ( UInt8 * ) md->getBytesNoCopy ( );
        bzero ( d0Response, md->getLength ( ) );
        
        result = discovery0( md );
        
        lockingByte = d0Response[68];
        MBREnabled  = lockingByte & 0x10;
        MBRDone     = lockingByte & 0x20;
        lockingEnabled = lockingByte & 0x02;
        locked      = lockingByte & 0x04;
        
        OSData * d0Buffer = OSData::withBytes(d0Response, DISCOVERY0_RESPONSE_SIZE);
        if ( getProperty(IODiscovery0ResponseKey) )
            removeProperty(IODiscovery0ResponseKey);
        setProperty(IODiscovery0ResponseKey, d0Buffer);
        d0Buffer -> release( );
        md->release ( );
    }
    IOLOG_DEBUG("%s[%p]::%s locking enabled, locked, MBREnabled, MBRDone = %d, %d, %d, %d\n",
                getName(), this, __FUNCTION__, lockingEnabled, locked, MBREnabled, MBRDone);
    
    objects[0] = OSBoolean::withBoolean(lockingEnabled);
    objects[1] = OSBoolean::withBoolean(locked);
    objects[2] = OSBoolean::withBoolean(MBREnabled);
    objects[3] = OSBoolean::withBoolean(MBRDone);
    
    keys[0] = OSSymbol::withCString(IOLockingEnabledKey);
    keys[1] = OSSymbol::withCString(IOLockedKey);
    keys[2] = OSSymbol::withCString(IOMBREnabledKey);
    keys[3] = OSSymbol::withCString(IOMBRDoneKey);
    
    OSDictionary * lockingFeature = OSDictionary::withObjects(objects, keys, 4);
    
    if ( getProperty(IOTPerLockingFeatureKey))
        removeProperty(IOTPerLockingFeatureKey);
    
    setProperty(IOTPerLockingFeatureKey, lockingFeature);
    
    IOLOG_DEBUG("%s[%p]::%s after setting OSBoolean Objects %d, %d, %d, %d\n",
                getName(), this, __FUNCTION__, lockingEnabled, locked, MBREnabled, MBRDone);
    IOLOG_DEBUG("%s[%p]::%s lockingFeature->getRetainCount() => %d",
                getName(), this, __FUNCTION__, lockingFeature->getRetainCount());
    
    if (lockingFeature) {
        
        lockingFeature->release();
        
    }
    return result;
}

typedef struct _IDENTIFY_RESPONSE {
    uint8_t ignore1;             //word 0
    uint8_t ignore2 : 7;         //word 0
    uint8_t devType : 1;         //word 0
    
    uint8_t ignore3[18];         //words 1-9
    uint8_t serialNumber[20];    //words 10-19
    uint8_t ignore4[6];          //words 20-22
    uint8_t firmwareRevision[8]; //words 23-26
    uint8_t modelNum[40];        //words 27-46
    uint8_t readMultFlags[2];    //word 47
    uint8_t TCGOptions[2];       //word 48
    uint8_t ignore5[102];        //words 49-99
    uint8_t maxLBA[8];           //words 100-103
    uint8_t ignore6[8];          //words 104-107
    uint8_t worldWideName[8];    //words 108-111
    uint8_t ignore7[32];         //words 112-127
    uint8_t securityStatus[2];   //word 128
    uint8_t vendorSpecific[62];  //words 129-159
    uint8_t ignored8[32];        //words 160-175
    uint8_t mediaSerialNum[60];  //words 176-205
    uint8_t ignored9[96];        //words 206-254
    uint8_t integrityWord[2];    //word 255
} IDENTIFY_RESPONSE;

OSDictionary * DriverClass::parseIdentifyResponse( const unsigned char * response)
{
    IDENTIFY_RESPONSE *resp = (IDENTIFY_RESPONSE *)response;
    
    uint8_t serialNumber[sizeof(resp->serialNumber)+1];
    for (size_t i = 0; i < sizeof(resp->serialNumber); i += 2) {         // uint8_t[20]
        serialNumber[i] = resp->serialNumber[i + 1];
        serialNumber[i + 1] = resp->serialNumber[i];
    }
    serialNumber[sizeof(resp->serialNumber)] = 0;
    
    uint8_t firmwareRevision[sizeof(resp->firmwareRevision)+1];
    for (size_t i = 0; i < sizeof(resp->firmwareRevision); i += 2) {       // uint8_t[8]
        firmwareRevision[i] = resp->firmwareRevision[i + 1];
        firmwareRevision[i + 1] = resp->firmwareRevision[i];
    }
    firmwareRevision[sizeof(resp->firmwareRevision)] = 0;

    uint8_t modelNumber[sizeof(resp->modelNum)+1];
    for (size_t i = 0; i < sizeof(resp->modelNum); i += 2) {          // uint8_t[40]
        modelNumber[i] = resp->modelNum[i + 1];
        modelNumber[i + 1] = resp->modelNum[i];
    }
    modelNumber[sizeof(resp->modelNum)] = 0;
    
    uint8_t worldWideName[sizeof(resp->worldWideName)];
    for (size_t i = 0; i < sizeof(resp->worldWideName); i += 2) {          // uint8_t[8]
        worldWideName[i] = resp->worldWideName[i + 1];
        worldWideName[i + 1] = resp->worldWideName[i];
    }
    
    const OSObject * objects[6];
    const OSSymbol * keys[6];
    
    objects[0] = OSNumber::withNumber( (unsigned long long)(resp->TCGOptions[1]<<8 | resp->TCGOptions[0]), 16);
    keys[0]    = OSSymbol::withCString( IOTCGOptionsKey );
    
    objects[1] = OSString::withCString( resp->devType ? "OTHER" : "ATA");
    keys[1]    = OSSymbol::withCString( IODeviceTypeKey );
    
    objects[2] = OSString::withCString( (const char *)serialNumber);
    keys[2]    = OSSymbol::withCString( IOSerialNumberKey );
    
    objects[3] = OSString::withCString( (const char *)modelNumber);
    keys[3]    = OSSymbol::withCString( IOModelNumberKey );
    
    objects[4] = OSString::withCString( (const char *)firmwareRevision);
    keys[4]    = OSSymbol::withCString( IOFirmwareRevisionKey );
    
    objects[5] = OSData::withBytes((const void *)worldWideName, sizeof(worldWideName));
    keys[5]    = OSSymbol::withCString( IOWorldWideNameKey );
    
    OSDictionary * result = OSDictionary::withObjects(objects, keys, 6, 8);
    
    return result;
}



//*****************
// public
//*****************

// SCSI Passthrough function called from user client
IOReturn DriverClass::SCSIPassthrough(SCSICommandDescriptorBlock cdb,
                                      IOBufferMemoryDescriptor * md,
                                      uint64_t * pTransferSize)
{
    IOLOG_DEBUG("%s[%p]::%s\n", getName(), this, __FUNCTION__);
    IOReturn ret = kIOReturnInternalError;
    SCSIServiceResponse serviceResponse  = kSCSIServiceResponse_SERVICE_DELIVERY_OR_TARGET_FAILURE;
    
    SCSITaskIdentifier request = GetSCSITask();
    if ( ! request ) {
        return ret;
    }
    
    
    ret = prepareSCSIPassthrough(cdb, md, request);
    
    if ( ret != kIOReturnSuccess )
        return ret;
    
    serviceResponse = SendCommand(request, SED_TIMEOUT);
    
    ret = completeSCSIPassthrough(md, request, serviceResponse, pTransferSize);
    
    if (request != NULL ) {
        ReleaseSCSITask(request);
        request = NULL;
    }
    
    return ret;
    
}



//*****************
// private
//*****************

IOReturn DriverClass::prepareSCSIPassthrough(SCSICommandDescriptorBlock cdb,
                                             IOBufferMemoryDescriptor * md,
                                             SCSITaskIdentifier request)
{
    if ( ! md ) {
        IOLOG_DEBUG( "%s::%s data BufferDescriptor error", getName ( ), __FUNCTION__);
        return kIOReturnVMError;                                            // !!immediate exit, do not pass through!!
    }
    
    
    IOLOG_DEBUG("%s::%s, CDB is ", getName ( ), __FUNCTION__ );
    for (int i =0; i < 16; i++)
        IOLOG_DEBUG(" %02X", cdb[i]);
    IOLOG_DEBUG("\n");
    
    
    SetCommandDescriptorBlock( request,
                               cdb[ 0], cdb[ 1], cdb[ 2], cdb[ 3], cdb[ 4], cdb[ 5], cdb[ 6], cdb[ 7],
                               cdb[ 8], cdb[ 9], cdb[10], cdb[11], cdb[12], cdb[13], cdb[14], cdb[15]);
    SetTimeoutDuration(request, SED_TIMEOUT);
    SetDataTransferDirection(request, getATATaskCommandDir(cdb));
    SetRequestedDataTransferCount(request, getATATransferSize(cdb));
    
    md->prepare();
    SetDataBuffer(request, md);
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
    IOLOG_DEBUG( "%s::%s sense data is:   %s\n", getName ( ), __FUNCTION__, str);
}

IOReturn DriverClass::completeSCSIPassthrough(IOBufferMemoryDescriptor * md,
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
        IOLOG_DEBUG( "%s::%s task status is ", getName ( ), __FUNCTION__);

        switch (taskStatus) {
            case kSCSITaskStatus_GOOD:
                IOLOG_DEBUG("GOOD!\n");
                ret = kIOReturnSuccess;
                if (pTransferSize) {
                    *pTransferSize = GetRealizedDataTransferCount(request);
                }
                break;
            case kSCSITaskStatus_CHECK_CONDITION:
                IOLOG_DEBUG("CHECK_CONDITION\n");
                senseDataSize = GetAutoSenseDataSize( request );
                
                GetAutoSenseData(request, &senseData, senseDataSize);
                
                if (senseData.VALID_RESPONSE_CODE & kSENSE_DATA_VALID)
                    printSenseData( &senseData );
                else {
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
                IOLOG_DEBUG("Other Task Status\n");
                break;
        }
    }
    else
    {
        IOLOG_DEBUG( "Task Not Complete -  %s::%s buffer\n", getName ( ), __FUNCTION__);
    }
    md->complete();

    return ret;
}


//**************
// apple overrides - debugging only
//**************

#if DRIVER_DEBUG

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
    
    return this;
}

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

#if DRIVER_DEBUG
void DriverClass::stop(IOService* provider)
{
    IOLOG_DEBUG("%s[%p]::%s *** before super\n", getName(), this, __FUNCTION__ );
    super::stop(provider);
    IOLOG_DEBUG("%s[%p]::%s *** after super \n", getName(), this, __FUNCTION__ );
}
#endif  // DRIVER_DEBUG



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
