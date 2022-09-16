//
//  MacFileSystemInterface.h
//  seddaemonandclient
//
//  Created by Jackie Marks on 8/29/16.
//  Copyright © 2016 MagnoliaHeights. All rights reserved.
//

#import "TPer.h"
#import "DeviceChangeDelegate.h"
#import "DeviceDataGatingProtocol.h"

@class StorageDeviceManager;

@interface MacFileSystemInterface : NSObject<DeviceDataGatingProtocol>

-(bool) startWatchingDeviceChanges;
-(void) stopWatchingDeviceChanges;
-(bool) claimDrivesOnStartup;

-(void) reenumerateUSBAndMountDrive:(TPer *) tPer ;
-(void) remountDriveUsingDiskArbitration:(NSString *) bsdName partitions:(NSArray *)partitions;

-(void) unmountDriveUsingDiskArbitration:(NSString *) bsdName;
-(void) unmountDriveUsingDiskArbitration:(NSString *) bsdName withContinuation: (Continuation) cont;

-(NSArray *)partitionsOfDeviceNamed:(NSString *)bsdName;

@property (nonatomic,weak) StorageDeviceManager * manager;
@end
