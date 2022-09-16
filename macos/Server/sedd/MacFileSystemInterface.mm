//
//  MacFileSystemInterface.m
//  seddaemonandclient
//
//  Created by Jackie Marks on 8/29/16.
//  Copyright © 2016 MagnoliaHeights. All rights reserved.
//

#import <IOKit/usb/IOUSBLib.h>
#import <IOKit/IOCFPlugIn.h>

#import "MacFileSystemInterface.h"
#import "StorageDeviceManager.h"
#import "DADisk.h"
#import "TCGDrive.h"
#import "NSData+hex.h"
#import "SEDNotifications.h"
#include "debug.h"

@interface MacFileSystemInterface()
@property (nonatomic,assign) DASessionRef session;
@property (nonatomic, strong) NSMutableArray<TPer *>  * reenumeratingDevices;

void unmountCallbackWithContinuation( DADiskRef disk, DADissenterRef __nullable dissenter, void * continuation);
@end


@implementation MacFileSystemInterface
@synthesize manager;
@synthesize session;
@synthesize reenumeratingDevices;

-(id) init
{
    if ((self = [super init])) {
        // Mac specific
        self.reenumeratingDevices = [NSMutableArray<TPer *> array];
        self.session = DASessionCreate(kCFAllocatorDefault);
        if (!self.session) return nil;
        [self claimDrivesOnStartup];
    }
    return self;
}

-(void) claim:(DADisk *)disk {
    [disk claimWithCallback:NULL userInfo: NULL];
}

-(bool) claimDrivesOnStartup {
    @autoreleasepool {
        NSMutableArray<NSNumber *> * drivesToBeClaimed = [NSMutableArray<NSNumber *> array];
        DriverIterator * iterator = [DriverIterator iterator];
        if (!iterator) return false;
        
        TPerDriverRegistryEntry * driver;
        while ((driver=[iterator next])) {
            [driver updateLockingPropertiesInIORegistry];
            if ( [driver isLocked] ) {
                NSString * bsdName = [driver findBSDNameOfDescendant];
                if ( bsdName == nil ) {
                    NSLOG_DEBUG(@"in claimDrivesOnStartup - drive to claim later is %u", driver.object);
                    [drivesToBeClaimed addObject: @(driver.object)];
                } else {
                    DADisk * disk = [DADisk diskFromBSDName:bsdName session:self.session];
                    [self claim: disk];
                }
            }
        }
        if (0<drivesToBeClaimed.count)
            [self ensureDrivesClaimed: drivesToBeClaimed];
    }
    return true;
}

- (void) ensureDrivesClaimed: (NSMutableArray<NSNumber *> *) drivesToBeClaimed {
    // process of building up registry is slow
    // this method is called if driver is in registry, but media hasn't shown up yet
    @autoreleasepool {
        // Make a copy to avoid modifying-while-enumerating
        NSArray * drives = [NSArray arrayWithArray:drivesToBeClaimed];
        for ( NSNumber * service in drives ) {
            // race condition above, try again
            
            uint64_t  entryID = [service unsignedLongLongValue];
            TPerDriverRegistryEntry * driver = [TPerDriverRegistryEntry entryForEntryID: entryID];
            
            for ( int i = 0; i < 100; i++ ) {  //maximum 100 microseconds
                NSString * bsdName = [driver findBSDNameOfDescendant];
                if ( bsdName == nil ) {
                    NSLOG_DEBUG(@"/nin claimDrivesOnStartup - drive to claim is %llu/n", entryID);
                    struct timespec t = { 0, 1000 }; // 1 microsecond
                    nanosleep( &t, NULL );
                } else {
                    DADisk * disk = [DADisk diskFromBSDName:bsdName session:self.session];
                    [self claim: disk];
                    [drivesToBeClaimed removeObject: service];
                    break;
                }
            }
        }
    }
}

-(bool) startWatchingDeviceChanges {
    [self registerCallbacks];
    return true;
}

-(void) stopWatchingDeviceChanges {
    if (NULL!=self.session) {
        [self unregisterCallbacks];
        self.session=NULL;
    }
}



#pragma mark -
#pragma mark Disk Arbitration

-(void) setSession:(DASessionRef CF_RELEASES_ARGUMENT) sessionRef
{
    if (NULL!=sessionRef) {
        CFRetain(sessionRef);
    }
    if (NULL != session) {
        CFRelease(session);
    }
    session=sessionRef;
    if (NULL!=sessionRef) {
        CFRelease(sessionRef);
    }
}

-(void) registerCallbacks
{
    DARegisterDiskPeekCallback(self.session,
                               kDADiskDescriptionMatchMediaWhole,
                               1,
                               peekCallback,
                               (void *)self);
    DARegisterDiskDisappearedCallback(self.session,
                                      kDADiskDescriptionMatchMediaWhole,
                                      disappearedCallback,
                                      (void *)self);
    DASessionScheduleWithRunLoop(self.session, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
}

-(void) unregisterCallbacks
{
    DASessionUnscheduleFromRunLoop(self.session, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
    DAUnregisterCallback(self.session, (void *)disappearedCallback, (void *)self);
    DAUnregisterCallback(self.session, (void *)peekCallback,        (void *)self);
}


static void peekCallback(DADiskRef diskRef, void * intf)
{
    @autoreleasepool {
        MacFileSystemInterface * interface = (__bridge MacFileSystemInterface *)intf;
        NSLOG_DEBUG(@"MacFileSystemInterface::peekCallback -- interface=0x%016lX disk=0x%016lX",
                    (size_t)(__bridge void *)interface, (size_t)(void *)diskRef);
        
        DADisk * disk=[DADisk diskFromDiskRef:diskRef];
        NSString * deviceName=disk.bsdName;
        NSLOG_DEBUG(@"MacFileSystemInterface peekCallback - new attachment of drive - %@\n", deviceName);
        
        TPerDriverRegistryEntry * driver =
            [TPerDriverRegistryEntry forDriveNamed: disk.bsdName];

        if (! driver ) {
            NSLOG_DEBUG(@"MacFileSystemInterface peekCallback: no driver service -- %@ is not a TPer\n", deviceName);
            return;
        }
        
        // Have a TPer

        NSString * worldWideName = driver.worldWideName.hex;
        
        if ( interface.reenumeratingDevices.count ) {
            NSUInteger index =
                [interface.reenumeratingDevices indexOfObjectPassingTest: ^BOOL (TPer * t, NSUInteger, BOOL *) {
                    return [t.uniqueID isEqualToString: worldWideName];
                } ];
            if (index != NSNotFound) {
                [interface.reenumeratingDevices removeObjectAtIndex: index];
            }
        }
        
        [driver updateLockingPropertiesInIORegistry];
        if ( [driver isLocked] ) {
            // Have a locked TPer
            NSLOG_DEBUG(@"MacFileSystemInterface::peekCallback: Have a locked TPer -  %@: %@\n",
                        disk.bsdName, worldWideName);

            TCGSTATUSCODE ret = FAIL;
            NSData * hostChallenge = tPerRememberedAdminHostChallenge(worldWideName);
            bool autoUnlock = tPerUnlockAutomatically(worldWideName);
            bool isCurrentlyBeingLocked = tPerIsCurrentlyBeingLocked(worldWideName);
            if (hostChallenge && autoUnlock && !isCurrentlyBeingLocked) {
                @autoreleasepool {  // autoreleasepool to collect the connection so one is available to updateLockingPropertiesInIORegistry
                    // Cobble together a direct drive unlocking
                    TPerUserClient * connection = [TPerUserClient connectionToDriver:driver];
                    if (connection) {
                        TCGDrive * tcgDrive = [TCGDrive driveForBsdName:deviceName
                                                          driverService:driver.object
                                                                connect:connection.object];
                        if (tcgDrive) {
                            ret = [tcgDrive unlockDriveWithHostChallenge: hostChallenge];
                            if (SUCCESS == ret) {
                                [driver updateLockingPropertiesInIORegistry];
                                setTPerIsCurrentlyBeingLocked(worldWideName, false);
                                showNotificationAutomaticallyUnlockingTPer(worldWideName, deviceName);
                                [interface reenumerateUSBBusAboveDiskNamed:deviceName];
                            }
                        }
                    }
                }
            } else {
                if (!hostChallenge) {
                    NSLOG_DEBUG(@"MacFileSystemInterface::peekCallback: -- no remembered host Challenge");
                }
                if (!autoUnlock) {
                    NSLOG_DEBUG(@"MacFileSystemInterface::peekCallback: -- autoUnlock off");
                }
                if (isCurrentlyBeingLocked)
                    NSLOG_DEBUG(@"MacFileSystemInterface::peekCallback: -- is currently being locked");
                }
                [interface claim:disk];
            }
        
        [interface.manager endDevicesChangingWithTag:@"Peek callback"];
    }
}

-(bool) isReenumeratingDeviceNamed:(NSString *)deviceName {
    __block bool result = false;
    [self.reenumeratingDevices enumerateObjectsUsingBlock:^(TPer * _Nonnull obj,
                                                            NSUInteger __unused idx,
                                                            BOOL * _Nonnull stop) {
        result = *stop = [deviceName isEqualToString:obj.deviceName];
    }];
    return result;
}

static void disappearedCallback(DADiskRef diskRef, void * intf)
{
    @autoreleasepool {
        DADisk * disk=[DADisk diskFromDiskRef:diskRef];
        NSString *bsdName = disk.bsdName;
        NSLOG_DEBUG(@"disappeared callback - disk is - %@\n", bsdName);
#if DEBUG
        CFDictionaryRef diskDescriptionRef = DADiskCopyDescription( diskRef );
        if (diskDescriptionRef) {
            NSLOG_DEBUG(@"disappeared callback -- description is %@", (__bridge NSDictionary *)diskDescriptionRef);
            CFRelease(diskDescriptionRef);
        }
#endif
        MacFileSystemInterface * interface = (__bridge MacFileSystemInterface *)intf;
        if ([interface isReenumeratingDeviceNamed:bsdName]) {
            NSLOG_DEBUG(@"disappeared callback - currently reenumerating %@\n", bsdName);
            return;
        }
        if (![interface.manager isManagingDeviceNamed:bsdName]){
            NSLOG_DEBUG(@"disappeared callback - not managing %@\n", bsdName);
            return;
        }
        usleep(1);      // still occasionally failing so added this
        NSLOG_DEBUG(@"disappeared callback - detachment of drive - %@\n", bsdName);
        [interface.manager devicesChangedWithTag:@"Disappeared callback"];
    }
}

void unmountCallbackWithContinuation(DADiskRef disk, DADissenterRef __nullable dissenter, void * continuation)
{
#pragma unused(disk)
    @autoreleasepool {
        
        NSLOG_DEBUG(@"MacFileSystemInterface::unmountCallbackWithContinuation bsdName = %s", DADiskGetBSDName( disk ) );
        if (dissenter)
            NSLOG_DEBUG(@"MacFileSystemInterface::unmountCallbackWithContinuation - unmount failed");
        
        if (continuation) {
            Continuation cont = (__bridge_transfer Continuation)(CFTypeRef)continuation;
            cont();
        }
    }
}

-(void) unmountDriveUsingDiskArbitration:(NSString *) bsdName {
    @autoreleasepool {
        // called when there's been no change in file system
        DADisk * disk = [DADisk diskFromBSDName:bsdName session:self.session];
        [disk unmountWithCallback: NULL userInfo: NULL];
    }
}

-(void) unmountDriveUsingDiskArbitration:(NSString *) bsdName withContinuation: (Continuation) cont{
    @autoreleasepool {
        // called when there's been no change in file system
        DADisk * disk = [DADisk diskFromBSDName:bsdName session:self.session];
        [disk unmountWithCallback: unmountCallbackWithContinuation userInfo: (void *) (__bridge_retained CFTypeRef)cont];
    }
}

- (void) remountDriveUsingDiskArbitration:(NSString *) bsdName partitions:(NSArray *)partitions {
#pragma unused(bsdName)
    // called when there's been no change in file system
    @autoreleasepool {
        NSLOG_DEBUG(@"MacFileSystemInterface::remountDriveUsingDiskArbitration -- bsdName=%@", bsdName);
        if (partitions && partitions.count) {
            NSLOG_DEBUG(@"MacFileSystemInterface::remountDriveUsingDiskArbitration should be remounting %@ %ld partitions:%@",
                        bsdName, partitions.count, partitions);
            NSString * partition;
            for (partition in partitions ){
                DADisk * disk = [DADisk diskFromBSDName:partition session:self.session];
                if (!disk) return;
                NSLOG_DEBUG(@"MacFileSystemInterface::remountDriveUsingDiskArbitration -- disk=0x%016lX", (size_t)(void *)disk.disk);
                [disk mountWithCallback: NULL whole:false userInfo: NULL];
            }
        }
    }
}
#pragma mark -
#pragma mark File System

-(void) reenumerateUSBDevice: (io_registry_entry_t) deviceEntry
{
    IOCFPlugInInterface     **ioDeviceInKernel=NULL;        // fetching this now
    SInt32                  score1;          // not used
    if (kIOReturnSuccess != IOCreatePlugInInterfaceForService(deviceEntry,
                                                              kIOUSBDeviceUserClientTypeID,
                                                              kIOCFPlugInInterfaceID,
                                                              &ioDeviceInKernel,
                                                              &score1)
        || !ioDeviceInKernel)
        return;
    
    IOUSBDeviceInterface187	**ioDeviceInterface=NULL;		// fetching this now
    if (kIOReturnSuccess != ((*ioDeviceInKernel)->QueryInterface)(ioDeviceInKernel,
                                                                  CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID187),
                                                                  (void **)&ioDeviceInterface)
        || !ioDeviceInterface)
        return;
    
    if (kIOReturnSuccess != ((*ioDeviceInterface)->USBDeviceOpen)((void *)ioDeviceInterface))
        return;
    if (kIOReturnSuccess != ((*ioDeviceInterface)->USBDeviceReEnumerate)((void *)ioDeviceInterface,
                                                                         kUSBReEnumerateReleaseDeviceBit))
        return;
    
    (void)(((*ioDeviceInterface)->USBDeviceClose)((void *)ioDeviceInterface)); // It's probably already closed
}

- (void) reenumerateUSBBusAboveDiskNamed:(NSString *)bsdName {
    @autoreleasepool {
        DADisk * disk = [DADisk diskFromBSDName:bsdName session:self.session];
        IORegistryEntry * media=disk.media;
        
        IOIterator * iterator=media.ancestorIterator;
        
        IORegistryEntry * usbDevice = NULL;
        IORegistryEntry * entry;
        while ( (entry = iterator.next) )  {
            NSString * nameOverride=[entry stringPropertyForKey:@"IOClassNameOverride"];
            if ( nameOverride && [nameOverride isEqualToString:@"IOUSBDevice"] ) {
                usbDevice = entry;
                break;
            }
        }
        NSLOG_DEBUG(@"Found USBDevice = %d", usbDevice.object);
        [self reenumerateUSBDevice: usbDevice.object];
    }
}


- (void) reenumerateUSBAndMountDrive:(TPer *)tPer {
    [self.reenumeratingDevices addObject: tPer];
    [self reenumerateUSBBusAboveDiskNamed:tPer.deviceName];
}


-(NSArray *)partitionsOfDeviceNamed:(NSString *)bsdName
{
    if (!bsdName) return nil;
    
    IORegistryEntry * deviceEntry=[IORegistryEntry entryForBSDName:bsdName];
    if (!deviceEntry) return nil;
    
    IOIterator * iterator=deviceEntry.descendantIterator;
    if (!iterator) return nil;
    
    NSMutableArray * partitions=[NSMutableArray new];
    IORegistryEntry * descendant;
    while ((descendant = iterator.next)) {
        NSDictionary * properties=descendant.properties;
        NSString * name  = properties[@"BSD Name"];  // TODO: are we sure these are there?  Could blow out if not.
        NSNumber * whole = properties[@"Whole"];
        NSNumber * open  = properties[@"Open"];
        if ( name && whole && !whole.boolValue && open && open.boolValue) {
            [partitions addObject: name];
        }
    }
    
    NSLOG_DEBUG(@"%@ has %ld open partitions:", bsdName, partitions.count);
    NSString * partition;
    for (partition in partitions) {
        NSLOG_DEBUG(@"  %@", partition);
    }
    
    return 0<partitions.count ? partitions : nil ;
}

@end
