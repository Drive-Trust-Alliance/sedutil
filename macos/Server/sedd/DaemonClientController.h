//
//  DaemonClientController.h
//  SED
//
//  Created by Scott Marks on 07/05/2017.
//
//

#import <Foundation/Foundation.h>
#import "DeviceChangeDelegate.h"
#import "StorageDeviceManager.h"
#import "TPer.h"
#import "DaemonClient.h"
#import "singleton.h"  //TODO: needed?

@interface DaemonClientController: NSObject<DeviceChangeDelegate>
declareSingleton(sharedController);

@property (nonatomic, assign) SecIdentityRef serverIdentity;
@property (nonatomic, assign) SecKeychainRef keychain;
@property (nonatomic, strong) NSString * keychainPasswordItemName;
@property (nonatomic, strong) StorageDeviceManager * manager;
@property (nonatomic,readonly) NSDictionary <NSString*, StorageDevice *> * storageDevices;
@property (nonatomic,readonly) NSDictionary <NSString *, TPer *> * tPers;

- (void) addClient: (DaemonClient *)client;
- (void) removeClient: (DaemonClient *)client;
@end
