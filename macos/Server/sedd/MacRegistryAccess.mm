//
//  MacRegistryAccess.m
//  seddaemonandclient
//
//  Created by Jackie Marks on 11/22/16.
//  Copyright © 2016 MagnoliaHeights. All rights reserved.
//

#import <SEDKernelInterface/SEDKernelInterface.h>
#import <IOKit/IOBSD.h>
#import "MacRegistryAccess.h"
#include "debug.h"

#define DEBUG_MACREGISTRYACCESS 0
#if DEBUG_MACREGISTRYACCESS
#define MACREGISTRYACCESS_DEBUG NSLOG_DEBUG
#else
#define MACREGISTRYACCESS_DEBUG(...) do ; while (0)
#endif

static void getString( io_registry_entry_t driverService, CFStringRef key, char * buffer, size_t length ) {
    
    NSString * name = (NSString *)CFBridgingRelease(IORegistryEntrySearchCFProperty(driverService,
                                                                                    kIOServicePlane,
                                                                                    key,
                                                                                    kCFAllocatorDefault,
                                                                                    kIORegistryIterateRecursively));
    
//    assert([name getCString: buffer
//           maxLength: length
//            encoding: NSUTF8StringEncoding]);
    if (name != nil)
        assert([name getBytes:(void *)buffer maxLength:(NSUInteger)length usedLength:NULL
                     encoding:NSUTF8StringEncoding options:0
                        range:NSMakeRange(0,name.length) remainingRange:NULL]);
}

static void getStringFromDict( io_registry_entry_t driverService __unused,
                               NSDictionary *dict,
                               NSString * key, char * buffer, size_t length ) {
    
    NSString * name = [dict valueForKey: key];
    
//    assert([name getCString: buffer
//           maxLength: length
//            encoding: NSUTF8StringEncoding]);
    if (name != nil)
        assert([name getBytes:(void *)buffer maxLength:(NSUInteger)length usedLength:NULL
                     encoding:NSUTF8StringEncoding options:0
                        range:NSMakeRange(0,name.length) remainingRange:NULL]);
}



@implementation MacRegistryAccess

+ (instancetype)sharedAccess {
    static MacRegistryAccess *sharedAccess = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedAccess = [[self alloc] init];
    });
    return sharedAccess;
}

-(NSDictionary<NSString *, StorageDevice *> *) storageDevicesWithInterface:(MacOSXInterface *)interface{
    return [self storageDevicesFromIORegistryWithInterface:interface];
}


-(NSDictionary<NSString *, StorageDevice *> *) storageDevicesFromIORegistryWithInterface:(MacOSXInterface *)interface{
    NSMutableDictionary<NSString *,StorageDevice *> * stgDevs=nil;
    @autoreleasepool {
        IOIterator * iterator=[IOIterator iteratorMatchingServiceClass: @kIOBlockStorageDriver];
        if (nil==iterator) return nil;
        
        stgDevs = [NSMutableDictionary new];
        IORegistryEntry * blockStorageDriver;
        while ((blockStorageDriver = [IORegistryEntry entryFromObject:[iterator next]])) {
            NSString * bsdName = (NSString *)[blockStorageDriver searchForProperty: @kIOBSDNameKey
                                                                  searchingParents: NO];
            if (!bsdName) continue;
            NSNumber * bsdUnit = (NSNumber *)[blockStorageDriver searchForProperty: @kIOBSDUnitKey
                                                                  searchingParents: NO];
            NSMutableDictionary * characteristics =
               [NSMutableDictionary dictionaryWithDictionary:@{@kIOBSDNameKey: bsdName, @kIOBSDUnitKey: bsdUnit }];
            
            NSDictionary * deviceCharacteristics =
                (NSDictionary *)[blockStorageDriver searchForProperty: @IODeviceCharacteristicsKey
                                                     searchingParents: YES];
            [characteristics addEntriesFromDictionary: deviceCharacteristics];
            
            NSDictionary * protocolCharacteristics =
                (NSDictionary *)[blockStorageDriver searchForProperty: @IOProtocolCharacteristicsKey
                                                     searchingParents: YES];
            [characteristics addEntriesFromDictionary: protocolCharacteristics];
            
            TPerDriverRegistryEntry * tPerDriver =
                [TPerDriverRegistryEntry entryFromEntry: [blockStorageDriver searchForRegistryEntryMatching: @kBrightPlazaDriverClass
                                                                             searchingParents: YES]];
            StorageDevice * sd = nil;
            if ( tPerDriver ) {
                NSDictionary * tPerCharacteristics = [tPerDriver dictPropertyForKey: @IOIdentifyCharacteristicsKey];
                [characteristics addEntriesFromDictionary: tPerCharacteristics];
                sd=[[TPer alloc] initWithCharacteristics:characteristics
                                      blockStorageDriver:blockStorageDriver
                                        tPerDriverHandle:tPerDriver
                                               interface:interface];
            } else {
                sd=[[StorageDevice alloc] initWithCharacteristics:characteristics
                                               blockStorageDriver:blockStorageDriver
                                                        interface:interface];
            }
            stgDevs[sd.uniqueID]=sd;  // TODO: just use NSArray?
        }
    }
    return stgDevs;
}



-(bool) fillStorageDeviceData: (STORAGE_DEVICE_DATA *) pStorageDeviceData
             forStorageDevice: (StorageDevice *) storageDevice
{
    bzero(pStorageDeviceData, sizeof(STORAGE_DEVICE_DATA));
    pStorageDeviceData->isTPer = NO;
#define getStringField(driver,name,field) getString(driver, CFSTR(name), (char *)(pStorageDeviceData->field), sizeof(pStorageDeviceData->field))
#define getStringFieldFromDict(driver,name,field) getStringFromDict(driver, dict, name, (char *)(pStorageDeviceData->field), sizeof(pStorageDeviceData->field))
    
    IORegistryEntry * blockStorageDriver = storageDevice.blockStorageDriver;
    io_registry_entry_t driver = blockStorageDriver.object;
    
    getStringField(driver,kIOBSDNameKey, bsdName);
    if (! *(pStorageDeviceData->bsdName) )
        return false;
    
    NSNumber * bsdUnit = (NSNumber *)CFBridgingRelease(IORegistryEntrySearchCFProperty(driver,
                                                                                       kIOServicePlane,
                                                                                       CFSTR(kIOBSDUnitKey),
                                                                                       kCFAllocatorDefault,
                                                                                       kIORegistryIterateRecursively));
    pStorageDeviceData->bsdUnit = (unsigned char)[bsdUnit charValue];
    
    NSNumber * num = (NSNumber *)CFBridgingRelease(IORegistryEntrySearchCFProperty(driver,
                                                                                   kIOServicePlane,
                                                                                   CFSTR("Size"),
                                                                                   kCFAllocatorDefault,
                                                                                   kIORegistryIterateRecursively));
    pStorageDeviceData->size = (size_t)[num longLongValue];  // Should the "Size" entry be unsigned?  Is it?
    
    MACREGISTRYACCESS_DEBUG(@"************driver service is %d", driver);
    NSDictionary * dict;
    
    dict = (__bridge_transfer NSDictionary *)IORegistryEntrySearchCFProperty(driver,
                                                                             kIOServicePlane,
                                                                             CFSTR(IOProtocolCharacteristicsKey),
                                                                             kCFAllocatorDefault,
                                                                             kIORegistryIterateRecursively|kIORegistryIterateParents);
    getStringFieldFromDict(driver,@"Physical Interconnect", physIntercnct);
    getStringFieldFromDict(driver,@"Physical Interconnect Location", physIntercnctLocation);
    
    dict = (__bridge_transfer NSDictionary *)IORegistryEntrySearchCFProperty(driver,
                                                                             kIOServicePlane,
                                                                             CFSTR(IODeviceCharacteristicsKey),
                                                                             kCFAllocatorDefault,
                                                                             kIORegistryIterateRecursively|kIORegistryIterateParents);
    getStringFieldFromDict(driver,@"Serial Number", serialNumber);
    getStringFieldFromDict(driver,@"Product Name", modelNumber);
    getStringFieldFromDict(driver,@"Product Revision Level", firmwareRevision);
    
    
    
    return true;
}
#undef getStringField
#undef getStringFieldFromDict


-(bool) isDriverEntrySetUp:(IORegistryEntry *)driverEntry
{
    NSDictionary * dict=[driverEntry dictPropertyForKey:@IOTPerLockingFeatureKey];
    if (!dict) return false;
    
    return [dict[@IOMBREnabledKey] boolValue] || [dict[@IOLockingEnabledKey] boolValue];
}

-(bool) isTPerSetUp:(TPer *)tPer {
    @autoreleasepool {
        //        IORegistryEntry * driverEntry=[self findDriverEntryFor:tPer];
        if (!tPer.driverHandle) return false;
        return [self isDriverEntrySetUp: tPer.driverHandle];
    }
}


-(bool) isDriverEntrySetUpAndLocked:(IORegistryEntry *)driverEntry
{
    if (![self isDriverEntrySetUp:driverEntry]) return false;
    
    NSDictionary * dict=[driverEntry dictPropertyForKey:@IOTPerLockingFeatureKey];
    return [dict[@IOLockedKey] boolValue]  || ![dict[@IOMBRDoneKey] boolValue];
}

-(bool) isTPerSetUpAndLocked:(TPer *)tPer {
    @autoreleasepool {
        //        IORegistryEntry * driverEntry=[self findDriverEntryFor:tPer];
        if (!tPer.driverHandle) return false;
        return [self isDriverEntrySetUpAndLocked: tPer.driverHandle];
    }
}

-(bool) fillTPerData: (STORAGE_DEVICE_DATA *) pTPerData
             forTPer: (TPer *) tPer
{
    if (![self fillStorageDeviceData:pTPerData forStorageDevice:tPer])
        return false;
    
    pTPerData->isTPer = YES;
    
#define getStringField(driver,name,field) getString(driver, CFSTR(name), (char *)(pTPerData->field), sizeof(pTPerData->field))
#define getStringFieldFromDict(driver,name,field) getStringFromDict(driver, dict, name, (char *)(pTPerData->field), sizeof(pTPerData->field))
    
    io_registry_entry_t driver = tPer.driverHandle.object;
    
    NSDictionary * dict;
    dict = (__bridge_transfer NSDictionary *)IORegistryEntryCreateCFProperty(driver,
                                                                             CFSTR(IOIdentifyCharacteristicsKey),
                                                                             CFAllocatorGetDefault(), 0);
    if ( !dict )
        return false;

    getStringFieldFromDict(driver,@IOSerialNumberKey, serialNumber);
    getStringFieldFromDict(driver,@IOModelNumberKey, modelNumber);
    getStringFieldFromDict(driver,@IOFirmwareRevisionKey, firmwareRevision);
    NSData * worldWideName = dict[@IOWorldWideNameKey];
    [worldWideName getBytes:pTPerData->worldWideName length:sizeof(pTPerData->worldWideName)];
    
    
    dict
    = (__bridge_transfer NSDictionary *)IORegistryEntryCreateCFProperty(driver,
                                                                        CFSTR(IOTPerLockingFeatureKey),
                                                                        CFAllocatorGetDefault(), 0);
    
    if ( !dict )
        return false;
    
    pTPerData->lockingEnabled = [dict[@IOLockingEnabledKey] boolValue];
    pTPerData->locked         = [dict[@IOLockedKey        ] boolValue];
    pTPerData->mbrEnabled     = [dict[@IOMBREnabledKey    ] boolValue];
    pTPerData->mbrDone        = [dict[@IOMBRDoneKey       ] boolValue];
    
    return true;
}




@end
