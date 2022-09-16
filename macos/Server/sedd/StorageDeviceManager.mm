//
//  StorageDeviceManager.m
//  sedagent
//
//  Created by Scott Marks on 04/10/2016.
//  Copyright © 2016 Bright Plaza Inc. All rights reserved.
//
//  Central point for knowledge about the macOS storage registryDevicesCache, including TPers
//  Essentially the "data model" for the daemon.
//

#import "StorageDeviceManager.h"
#import "MacRegistryAccess.h"
#import "MacFileSystemInterface.h"      // xxx need to make a general superclass
#import "SEDNotifications.h"
#import "debug.h"


@interface StorageDeviceManager()<DeviceChangeDelegate>
@property (nonatomic, strong, readwrite) MacOSXInterface * osInterface;
@property (nonatomic, strong, readwrite) NSDictionary * registryDevicesCache;
@property (nonatomic, assign) bool devicesChanging;
@property (readonly) MacRegistryAccess * registryAccess;
@property (nonatomic, strong, readwrite) MacFileSystemInterface * fileSystemInterface;
@end

@implementation StorageDeviceManager
@synthesize osInterface;
@synthesize registryDevicesCache;
@synthesize devicesChanging;
@synthesize delegate;
@synthesize fileSystemInterface;



implementSingleton(sharedManager)

-(id) init
{
    if ((self = [super init])) {
        self.osInterface = [MacOSXInterface new];
        self.osInterface.manager = self;
        
        
        self.fileSystemInterface = [MacFileSystemInterface new];
        self.fileSystemInterface.manager = self;
        
        self.osInterface.fileSystemInterface = self.fileSystemInterface;
        
        [self invalidateRegistryDevicesCache];
        self.devicesChanging=false;
    }
    return self;
}

- (void) invalidateRegistryDevicesCache {registryDevicesCache=nil;}


-(bool) start {
   [[NSDistributedNotificationCenter defaultCenter] addObserver: self
                                                        selector: @selector(devicesChanged)
                                                            name: @"TPer Update"
                                                         object: NULL ];
    __block bool anyDrivesUnlocked = false;
    for (TPer * t in self.lockedTPers.allValues) {
        NSLOG_DEBUG(@"StorageDeviceManager:start -- looking at locked TPer %@", t.uniqueID);
        if (t.autoUnlock) {
            NSData * hostChallenge = t.rememberedAdminHostChallenge;
            if (hostChallenge) {
                NSLOG_DEBUG(@"StorageDeviceManager:start -- attempting to unlock locked TPer %@", t.uniqueID);
                [t unlockWithHostChallenge:hostChallenge
                                autoUnlock:true
                              continuation:^(TCGSTATUSCODE result) {
                                  if (result==SUCCESS) {
                                      NSLOG_DEBUG(@"StorageDeviceManager:start -- did unlock locked TPer %@ as %@",
                                                  t.uniqueID, t.deviceName);
                                      showNotificationAutomaticallyUnlockingTPer(t.uniqueID, t.deviceName);
                                      anyDrivesUnlocked = true;
                                  }
                }];
            }
        }
    }
    if (anyDrivesUnlocked) {
        NSLOG_DEBUG(@"StorageDeviceManager:start -- some devices automatically unlocked at startup");
        [self endDevicesChangingWithTag:@"Automatic unlock at startup"];
    }
    return [self.fileSystemInterface startWatchingDeviceChanges];
}


// NOT CALLED
-(void) stop {
    [[NSDistributedNotificationCenter defaultCenter] removeObserver: self name:@"TPer Update" object:NULL];
    [self.fileSystemInterface stopWatchingDeviceChanges];
}

-(MacRegistryAccess *)registryAccess {
    return [MacRegistryAccess sharedAccess];
}

-(NSDictionary<NSString *, StorageDevice *> *) storageDevices {
    if (!self.registryDevicesCache) {
        self.registryDevicesCache=[self.registryAccess storageDevicesWithInterface:self.osInterface];
    }
    return self.registryDevicesCache;
}

-(NSDictionary<NSString *, TPer *> *) tPers {
    NSMutableDictionary * tdict=[NSMutableDictionary new];
    [self.storageDevices enumerateKeysAndObjectsUsingBlock:^(id _Nonnull key,
                                                             id _Nonnull obj,
                                                             BOOL * _Nonnull stop __unused) {
        if ([obj isTPer]) tdict[key]=obj;
    }];
    return tdict;
}

-(NSDictionary<NSString *, TPer *> *) lockedTPers {
    NSMutableDictionary * tdict=[NSMutableDictionary new];
    [self.storageDevices enumerateKeysAndObjectsUsingBlock:^(id _Nonnull key,
                                                             id _Nonnull obj,
                                                             BOOL * _Nonnull stop __unused) {
        if ([obj isTPer]) {
            TPer * t = (TPer *)obj;
            if (t.isSetUp && t.isLocked) {
                tdict[key]=t;
            }
        }
    }];
    return tdict;
}

-(bool) isManagingDeviceNamed:(NSString *)deviceName {
    __block bool result = false;
    [self.storageDevices enumerateKeysAndObjectsUsingBlock:^(id _Nonnull __unused key,
                                                             id _Nonnull obj,
                                                             BOOL * _Nonnull stop) {
        result = *stop = [deviceName isEqualToString:((StorageDevice *) obj).deviceName];
    }];
    return result;
}


-(void) beginDevicesChanging {
    [self beginDevicesChangingWithTag: nil];
}

-(void) beginDevicesChangingWithTag:(NSString * ) tag {
#pragma unused(tag)
    NSLOG_DEBUG(@"StorageDeviceManager beginDevicesChangingWithTag:%@", tag);
    self.devicesChanging=true;
}

-(void) endDevicesChanging {
    [self endDevicesChangingWithTag: nil];
}
-(void) endDevicesChangingWithTag:(NSString *) tag {
    NSLOG_DEBUG(@"StorageDeviceManager endDevicesChangingWithTag:%@", tag);
    self.devicesChanging=false;
    [self devicesChangedWithTag:tag];
}

-(void) devicesChanged {
    [self devicesChangedWithTag:nil];
}

-(void) devicesChangedWithTag:(NSString *)tag {
    NSLOG_DEBUG(@"StorageDeviceManager devicesChangedWithTag:%@", tag);
    [self invalidateRegistryDevicesCache];
    if (!self.devicesChanging) {
        NSLOG_DEBUG(@"devicesChanging flag false, sending update");
        [self.delegate devicesChangedWithTag: tag];
    } else {
        NSLOG_DEBUG(@"devicesChanging flag true, NOT sending update");
    }
}


#pragma mark -
#pragma mark Temporary - selector for distributed notification
// temporary for SEDUtil-cli which is not connecting to daemon
// but sends out s notification when it sends commands to a drive
// xxxx add to queue! will get lost if devicechanging is true
-(void) devicesChangedNotification: (NSNotification * __unused) aNotification {
    [self invalidateRegistryDevicesCache];
    if (! self.devicesChanging) [self devicesChanged];
}


-(void)dealloc
{
    [self stop];
    [self invalidateRegistryDevicesCache];
    self.delegate = nil;
}

@end
