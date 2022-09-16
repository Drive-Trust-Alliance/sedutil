//
//  StorageDevice.m
//  sedagent
//
//  Created by Scott Marks on 04/10/2016.
//  Copyright © 2016 Bright Plaza Inc. All rights reserved.
//

#import <IOKit/IOBSD.h>
#import <SEDKernelInterface/UserKernelShared.h>
#import "SEDClientServerCommunication.h"
#import "MacRegistryAccess.h"
#import "MacOSXInterface.h"
#import "StorageDevice.h"
#include "debug.h"

@interface StorageDevice()
@property (nonatomic, strong, readwrite) NSString * deviceName;
@property (nonatomic, strong, readwrite) NSNumber * unitNumber;
@property (nonatomic, strong, readwrite) NSString * modelNumber;
@property (nonatomic, strong, readwrite) NSString * serialNumber;
@property (nonatomic, strong, readwrite) NSString * uniqueID;
@property (nonatomic, strong, readwrite) IORegistryEntry * blockStorageDriver ;
@property (nonatomic, strong, readwrite) MacOSXInterface *interface;

@end

@implementation StorageDevice
@synthesize deviceName;
@synthesize unitNumber;
@synthesize modelNumber;
@synthesize serialNumber;
@synthesize uniqueID;
@synthesize blockStorageDriver ;
@synthesize interface;

static inline NSString * OR(NSString * a, NSString * b){return a ? a : b;}

-(id) initWithCharacteristics:(NSDictionary *)characteristics
           blockStorageDriver:(IORegistryEntry *)b
                    interface:(MacOSXInterface *)i {
    if ((self = [super init])) {
        self.deviceName = [characteristics objectForKey:@kIOBSDNameKey];
        self.unitNumber = [characteristics objectForKey:@kIOBSDUnitKey];
        self.modelNumber = OR([characteristics objectForKey: @IOModelNumberKey],
                              [characteristics objectForKey: @IOProductNameKey]);
        self.serialNumber = OR([characteristics objectForKey:@IOSerialNumberKey],@"<n/a>");
        // TODO: World Wide Name for storage devices?
        self.uniqueID = [NSString stringWithFormat: @"%@#%@", self.modelNumber.trimmed, self.serialNumber.trimmed] ;
        self.blockStorageDriver=b;
        self.interface=i;
    }
    return self;
}

// Mac-specific - change later?
-(MacRegistryAccess *) registryAccess {
    return [MacRegistryAccess sharedAccess];
}


-(NSString *)base64Data {
    @autoreleasepool {
#if DEBUG
        NSLOG_DEBUG(@"base64Data: for %@", self.uniqueID);
        NSString * uid = self.uniqueID;
#endif
        STORAGE_DEVICE_DATA StorageDeviceData;
        if ( [self.registryAccess fillStorageDeviceData: &StorageDeviceData forStorageDevice:self] )
        {
            NSData * unencoded = [ NSData dataWithBytesNoCopy:&StorageDeviceData length:sizeof(StorageDeviceData) freeWhenDone:NO];
            return [unencoded base64EncodedStringWithOptions:(NSDataBase64Encoding64CharacterLineLength)];
        }
#if DEBUG
        NSLOG_DEBUG(@"base64Data: could not fill storage device data for %@", uid);
#endif
        return nil;
        
    }
}

-(void) setUniqueIDFromWorldWideName:(NSData *)worldWideName { // allow subclass to override
    if (worldWideName) {
        self.uniqueID=worldWideName.hex;
    }
}
@end
