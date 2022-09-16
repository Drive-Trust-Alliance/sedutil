//
//  TPer.h
//  sedagent
//
//  Created by Scott Marks on 04/10/2016.
//  Copyright © 2016 Bright Plaza Inc. All rights reserved.
//

#import "StorageDevice.h"
#import "ContinuationDefs.h"
#import "TPerDriverRegistryEntry.h"

// Direct manipulation of preferences
extern NSData * tPerRememberedAdminHostChallenge(NSString * tPerName);
extern void setTPerRememberedAdminHostChallenge(NSString * tPerName, NSData * hostChallenge);

extern bool tPerUnlockAutomatically(NSString * tPerName);
extern void setTPerUnlockAutomatically(NSString * tPerName, bool autoUnlock);

extern bool tPerIsCurrentlyBeingLocked(NSString * tPerName);
extern void setTPerIsCurrentlyBeingLocked(NSString * tPerName, bool isCurrentlyBeingLocked);


@interface TPer : StorageDevice

@property(nonatomic,readonly) TPerDriverRegistryEntry * driverHandle;
@property(nonatomic,readonly) NSString * SSCName;
@property(nonatomic,readonly) bool isSetUp;
@property(nonatomic,readonly) bool isLocked;
@property(nonatomic,strong) NSData * rememberedAdminHostChallenge;
@property(nonatomic,assign) bool autoUnlock;
@property(nonatomic,assign) bool isCurrentlyBeingLocked;

-(instancetype) initWithCharacteristics:(NSDictionary *)characteristics
                     blockStorageDriver:(IORegistryEntry *)blockStorageDriver
                       tPerDriverHandle:(TPerDriverRegistryEntry *)tPerDriver
                              interface:(MacOSXInterface *)interface ;


#if NONSTUB
-(void) getComID:(uint8_t *)buffer;
#endif //NONSTUB
-(NSString *)MSID;
// PSID Revert
-(bool) revertWithPSID:(NSString *)PSID continuation:(ResultContinuation)continuation;

// password is NSString *, DTA code hashes
-(bool) lockWithPassword:(NSString *)password
            continuation:(ResultContinuation) resultContinuation;
-(bool) unlockWithPassword:(NSString *)password
       autoUnlock:(BOOL)autoUnlock
              continuation:(ResultContinuation) resultContinuation;
-(bool) eraseWithPassword:(NSString *)password;
-(bool) setupWithPassword:(NSString *)password;
#if MULTISTART
-(bool) multiUserSetupWithPassword: (NSString *)password;
#endif // MULTISTART
-(bool) unprotectWithPassword:(NSString *)password;
-(bool) changePassword:(NSString *)oldPassword to:(NSString *)newPassword;
-(bool) changeUser:(NSString *)userName adminPassword:(NSString *)adminPassword to:(NSString *)newPassword;


// hostChallenge is NSData *, DTA code does not hash
-(bool) lockWithHostChallenge:(NSData *)hostChallenge
                 continuation:(ResultContinuation) resultContinuation;
-(bool) unlockWithHostChallenge:(NSData *)hostChallenge
            autoUnlock:(BOOL)autoUnlock
                   continuation:(ResultContinuation) resultContinuation;
-(bool) eraseWithHostChallenge:(NSData *)hostChallenge;
-(bool) setupWithHostChallenge:(NSData *)hostChallenge;
#if MULTISTART
-(bool) multiUserSetupWithHostChallenge:(NSData *)hostChallenge;
#endif // MULTISTART
-(bool) unprotectWithHostChallenge:(NSData *)hostChallenge;
-(bool) changeHostChallenge:(NSData *)oldhostChallenge to:(NSData *)newHostChallenge;
-(bool) changeUser:(NSString *)userName adminHostChallenge:(NSData *)adminHostChallenge to:(NSData *)newHostChallenge;

-(NSData *) hash:(NSString *)password;
-(NSString *) hashAndEncode:(NSString *)password;


@end

@interface StorageDevice (MaybeATPer)
@property (nonatomic, readonly) bool isTPer;
@end
