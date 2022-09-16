//
//  MacRegistryAccess.h
//  seddaemonandclient
//
//  Created by Jackie Marks on 11/22/16.
//  Copyright © 2016 MagnoliaHeights. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "StorageDeviceData.h"
#import "StorageDevice.h"
#import "TPer.h"
#import "MacOSXInterface.h"

@interface MacRegistryAccess : NSObject
+ (instancetype)sharedAccess; 


-(bool) fillStorageDeviceData: (STORAGE_DEVICE_DATA *) pStorageDeviceData
             forStorageDevice: (StorageDevice *) storageDevice;

-(bool) fillTPerData: (STORAGE_DEVICE_DATA *) pTPerData
             forTPer: (TPer *) tPer;

-(NSDictionary<NSString *,StorageDevice *> *) storageDevicesWithInterface:(MacOSXInterface *)interface ;

-(bool) isTPerSetUp:(TPer *)tPer;
-(bool) isTPerSetUpAndLocked:(TPer *)tPer;

@end


