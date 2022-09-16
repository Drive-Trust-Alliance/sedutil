//
//  StorageDevice.h
//  sedagent
//
//  Created by Scott Marks on 04/10/2016.
//  Copyright © 2016 Bright Plaza Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "IOObject.h"
@class MacOSXInterface;

@interface StorageDevice : NSObject
-(id) initWithCharacteristics:(NSDictionary *)characteristics
           blockStorageDriver:(IORegistryEntry *) blockStorageDriver
                    interface:(MacOSXInterface *) interface;

@property (nonatomic,readonly) NSString * deviceName;
@property (nonatomic,readonly) NSNumber * unitNumber;
@property (nonatomic,readonly) NSString * modelNumber;
@property (nonatomic,readonly) NSString * serialNumber;
@property (nonatomic,readonly) NSString * uniqueID;
@property (nonatomic,readonly) IORegistryEntry * blockStorageDriver ;
@property (nonatomic,readonly) MacOSXInterface *interface;

-(NSString *)base64Data;

-(void) setUniqueIDFromWorldWideName:(NSData *)worldWideName;

@end
