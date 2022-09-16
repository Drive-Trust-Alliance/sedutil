//
//  StorageDeviceManager.h
//  sedagent
//
//  Created by Scott Marks on 04/10/2016.
//  Copyright © 2016 Bright Plaza Inc. All rights reserved.
//
//  Central point for knowledge about the macOS storage devices, including TPers
//  Essentially the "data model" for the daemon.
//
#import "DeviceDataGatingProtocol.h"
#import "DeviceChangeDelegate.h"
#import "singleton.h"
#import "StorageDevice.h"
#import "TPer.h"
#import "MacOSXInterface.h"


@interface StorageDeviceManager : NSObject<DeviceChangeDelegate>

@property (nonatomic, weak) id<DeviceChangeDelegate> delegate;

@property (nonatomic, strong, readonly) MacOSXInterface * osInterface;
@property (nonatomic, strong, readonly) NSDictionary<NSString *, StorageDevice *> * storageDevices;
@property (nonatomic, strong, readonly) NSDictionary<NSString *, TPer *> * tPers;
@property (nonatomic, strong, readonly) NSDictionary<NSString *, TPer *> * lockedTPers;

declareSingleton(sharedManager);

- (bool) start;
- (void) stop;
- (bool) isManagingDeviceNamed:(NSString *)deviceName;

@end
