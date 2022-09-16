//
//  sedd
//  SED Daemon
//
//  The daemon uses a model-view-controller architecture, except the view is mediated by network connections,
//     so Connection objects created Bonjour servers or Bluetooth servers fill that role
//  The data model is the StorageDeviceManager with its storage devices and the TPer subset thereof
//  The controller is the DaemonClientController attached to each Connection object
//     and accessing [StorageDeviceManager sharedManager]
//
#import "log.h"
#import "StorageDeviceManager.h"
#import "DaemonClientController.h"
#import "SEDNotifications.h"
#include "debug.h"

#define firewallPath "/usr/libexec/ApplicationFirewall/socketfilterfw"
static void
firewall(const char * path, NSString * command) {
    command = [NSString stringWithFormat:@"%s --%@ \"%s\"", firewallPath, command, path];
    int result = system(command.UTF8String);
    NSLOG_DEBUG(@"system(\"%s\") = %d", command.UTF8String, result);
#pragma unused(result)
}


#define IDENTITY_NAME @"Bright Plaza SED Server"
#define KEYCHAIN_PATH "/Library/Keychains/Bright Plaza SED.keychain"
#define KEYCHAIN_PASSWORD_ITEM_NAME @"Bright Plaza SED Keychain"

// Get a SecKeychainRef to the keychain in keychainPath
static SecKeychainRef copyKeychainRef(const char * keychainPath) {
    SecKeychainRef keychainRef=NULL;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    OSStatus err = SecKeychainOpen(keychainPath, &keychainRef);
#pragma clang diagnostic pop
    if (errSecSuccess != err) {
        NSLog(@"SecKeychainOpen %s returned %d", keychainPath, err);
        return NULL;
    }
    NSLog(@"%@ is the SecKeychainRef for %s", keychainRef, keychainPath);
    return keychainRef;
}


// Get a SecIdentityRef to IDENTITY_NAME in SEDKeychain (suggestively named parameter)
static SecIdentityRef copyIdentityRef(SecKeychainRef SEDKeychain, NSString * matchLabel) {
    OSStatus err = errSecNotAvailable;
    CFArrayRef originalSearchList = NULL;
    err = SecKeychainCopySearchList(& originalSearchList);
    if (errSecSuccess != err) {
        NSLog(@"sedd::copyIdentityRef -- SecKeychainCopySearchList failed?!  err=%d", err);
        return NULL;
    }
    CFArrayRef matchResultsRef = NULL;
    @try {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
        err = SecKeychainSetSearchList((__bridge CFArrayRef)@[(__bridge id)SEDKeychain]);
#pragma clang diagnostic pop
        if (errSecSuccess != err) {
            NSLog(@"sedd::copyIdentityRef -- SecKeychainSetSearchList failed?!  err=%d", err);
            return NULL;
        }
        NSDictionary * matching = @{
                                    (id) kSecAttrLabel:         matchLabel,
                                    (id) kSecClass:             (id) kSecClassIdentity,
                                    (id) kSecReturnRef:         @YES,
                                    (id) kSecReturnAttributes:  @YES,
                                    (id) kSecMatchLimit:        (id) kSecMatchLimitAll,
                                    };
        err = SecItemCopyMatching((__bridge CFDictionaryRef)matching, (CFTypeRef *)&matchResultsRef);
        if (errSecSuccess != err) {
            NSLog(@"sedd::copyIdentityRef -- SecItemCopyMatching failed?!  err=%d", err);
            return NULL;
        }
    }
    @finally {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
        err = SecKeychainSetSearchList(originalSearchList);
#pragma clang diagnostic pop
        if (errSecSuccess != err) {
            NSLog(@"sedd::copyIdentityRef -- SecKeychainSetSearchList failed?!  err=%d", err);
            return NULL;
        }
        CFRelease(originalSearchList);
    }
    NSArray<NSDictionary *> * matchResults = (__bridge NSArray<NSDictionary *> *)matchResultsRef;
    for (NSDictionary * matchDict in matchResults) {
        NSString * label = matchDict[ (__bridge id) kSecAttrLabel ];
        NSLOG_DEBUG(@"sedd::copyIdentityRef -- label=%@",label);
        if ([label isEqual: matchLabel]) {
            SecIdentityRef identityRef = (__bridge_retained SecIdentityRef) (matchDict[ (__bridge id) kSecValueRef]);
            NSLOG_DEBUG(@"sedd::copyIdentityRef -- label matches, returning %@", identityRef);
            return identityRef;
        }
    }
    return NULL ;
}

static bool getSEDKeychainIdentity(SecIdentityRef& serverIdentity, SecKeychainRef& SEDKeychain, NSString ** nameLabel) {
    // Get the server identity
    SEDKeychain = copyKeychainRef(KEYCHAIN_PATH);
    if (NULL == SEDKeychain) {
        NSLog(@"Could not find keychain %s", KEYCHAIN_PATH);
        return false;
    }
    serverIdentity = copyIdentityRef(SEDKeychain, IDENTITY_NAME);
    if (NULL == serverIdentity) {
        NSLog(@"Could not find server identity \"%@\" in keychain %@", IDENTITY_NAME, SEDKeychain);
        return false;
    }
    *nameLabel=IDENTITY_NAME;
    return true;
}

int main(const int argc , const char **argv ) {
    int result=EXIT_SUCCESS;
    turnOffLogging();

    NSLOG_DEBUG(@"sedd::main argc=%d", argc);
    for (int i=0 ; i <argc ; i++) {
        NSLOG_DEBUG(@"sedd::main argv[%d]=%s", i, argv[i]);
    }
    
    // Open up the firewall for sedd
    for (NSString * command in @[@"blockapp", @"remove", @"add", @"unblockapp"])
        firewall(argv[0], command);
    
    @autoreleasepool {
        
//        SEDUserNotificationCenterDelegate * userNoficationCenterDelegate = [SEDUserNotificationCenterDelegate new];
//        [userNoficationCenterDelegate becomeNotificationDelegate];
        
        // Important to create the manager singleton immediately, because it creates its
        // MacFileSystemInterface singleton, which in turn immediately claims any locked drives.
        // We want claim those drives ASAP after sedd starts.
        StorageDeviceManager * manager = [StorageDeviceManager sharedManager];
        SecIdentityRef serverIdentity;
        SecKeychainRef serverKeychain;
        NSString * keychainItemName;
        if (!getSEDKeychainIdentity(serverIdentity, serverKeychain, &keychainItemName))
        {
            result = EXIT_FAILURE;
            goto EXIT;
        }
        DaemonClientController * controller = [DaemonClientController sharedController];
        controller.serverIdentity = serverIdentity;
        controller.keychain=serverKeychain;
        controller.keychainPasswordItemName=KEYCHAIN_PASSWORD_ITEM_NAME;
        controller.manager = manager;   // controller becomes manager's delegate
        
        if (! [manager start]) {
            NSLOG_DEBUG(@"Error starting deviceManager");
            result=EXIT_FAILURE;
            goto EXIT;
        }
        NSLOG_DEBUG(@"Started device manager");
        
        // Block to attach server-created connections to the controller
        ConnectionFactoryBlock daemonClientConnection = ^(NSInputStream * i, NSOutputStream * o, NSNetService * n,
                                                          NSUInteger loopbackPort, bool isLoopback) {
            DaemonClient * client = [DaemonClient clientWithInputStream: i outputStream: o netService: n
                                                           loopbackPort: loopbackPort isLoopback: isLoopback
                                                             controller: controller];
            [controller addClient:client];
        };

#if ENABLE_BONJOUR || 1
        BonjourServer * boujourServer = [BonjourServer serverWithConnectionFactoryBlock: daemonClientConnection
                                                                       localMachineOnly:false];
        if ([boujourServer startNetwork] ) {
            NSLOG_DEBUG(@"main -- Started ethernet server on port %zu.", (size_t) [boujourServer netServicePort]);
        } else {
            NSLog(@"main -- Error starting Bonjour server");
        }
#endif

#if ENABLE_BLUETOOTH & 0
        BluetoothServer * bluetoothServer = [BluetoothServer serverWithConnectionFactoryBlock: daemonClientConnection];
        if ([bluetoothServer startNetwork]) {
            NSLOG_DEBUG(@"Bluetooth server created");
        } else {
            NSLOG_DEBUG(@"failed to create Bluetooth server");
        }
#endif

        [[NSRunLoop currentRunLoop] run];  // go!
    
    }

EXIT:
    return result;
}
