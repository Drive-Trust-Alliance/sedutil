//
//  DaemonClientController.mm
//  SED
//
//  Created by Scott Marks on 07/05/2017.
//
//

#import "DaemonClientController.h"
#import "debug.h"

@interface DaemonClientController()
@property(nonatomic, strong) NSMutableArray<DaemonClient *>* clients;
@end


@implementation DaemonClientController

#pragma mark -
#pragma mark DaemonClientController methods
implementSingleton(sharedController)

@synthesize serverIdentity;
@synthesize keychain;
@synthesize keychainPasswordItemName;
@synthesize manager;
@synthesize clients;

- (DaemonClientController *) init {
    if ((self=[super init])) {
        self.clients=[NSMutableArray new];
    }
    return self;
}

- (void) setManager:(StorageDeviceManager *)_manager {
    manager=_manager;
    manager.delegate=self;
}

- (void) addClient: (DaemonClient *)client { [self.clients addObject: client]; }

- (void) removeClient: (DaemonClient *)client { [self.clients removeObject: client]; }

-( NSDictionary <NSString *, StorageDevice *> *) storageDevices { return self.manager.storageDevices; }
-( NSDictionary <NSString *, TPer *> *) tPers { return self.manager.tPers; }

#pragma mark -
#pragma mark DeviceChangeDelegate protocol methods

-(void) devicesChangedWithTag:(NSString * )tag {
    NSLOG_DEBUG(@"DaemonClientController devicesChangedWithTag:%@", tag);
    for (DaemonClient * client in self.clients) [client devicesChangedWithTag: tag];
}

-(void) devicesChanged { [self devicesChangedWithTag: nil]; }

-(void) beginDevicesChangingWithTag:(NSString *)tag {
    NSLOG_DEBUG(@"DaemonClientController beginDevicesChangingWithTag:%@", tag);
    [self.manager beginDevicesChangingWithTag: tag];
}

-(void) beginDevicesChanging { [ self beginDevicesChangingWithTag: nil ]; }

-(void) endDevicesChangingWithTag:(NSString *)tag {
    NSLOG_DEBUG(@"DaemonClientController endDevicesChangingWithTag:%@", tag);
    [self.manager endDevicesChangingWithTag: tag];
}

-(void) endDevicesChanging { [ self endDevicesChangingWithTag: nil ]; }

@end
