//
//  OSInterface.h
//  sedagent
//
//  Created by Scott Marks on 04/10/2016.
//  Copyright © 2016 Bright Plaza Inc. All rights reserved.
//
#import "SEDClientServerCommunication.h"
#import "DeviceChangeDelegate.h"
#import "DeviceDataGatingProtocol.h"
#import <sedutil/sedutil.h>

@class StorageDevice;
@class StorageDeviceManager;
@interface OSInterface: NSObject
@property(nonatomic,weak)StorageDeviceManager * manager;
@end

#pragma mark -
#pragma mark TCG

//#import "ContinuationDefs.h"
#import "TCG_defs.h"

@class TPer;


@interface OSInterface(TCG)
#if NONSTUB
-(bool) sendIFCommand:(TCG_IO_COMMAND)tcg_io_command tPer:(TPer *)tPer
               buffer:(void *)buffer pLength:(uint64_t *)pLength;
-(bool) sendIFSendTper:(TPer *)tPer buffer:(void *)buffer pLength:(uint64_t *)pLength;
-(bool) sendIFRecvTper:(TPer *)tPer buffer:(void *)buffer pLength:(uint64_t *)pLength;
#endif

// STUB: The layers above don't yet know how to compose TPer commands

-(TCGSTATUSCODE) revertDrive:(TPer * )tPer PSID:(NSString * )PSID continuation:(ResultContinuation)resultContinuation;

-(TCGSTATUSCODE) getMSID:(NSMutableString *)MSID forTPer:(TPer *)tPer;


-(TCGSTATUSCODE)lockOrUnlockDrive:(TPer *)tPer
                locking:(BOOL) locking
               password:(NSString *)password
           continuation:(ResultContinuation) resultContinuation;

- (TCGSTATUSCODE) setupDrive:(TPer *)tPer password:(NSString *)password;
#if MULTISTART
- (TCGSTATUSCODE) multiUserSetupDrive:(TPer *)tPer password:(NSString *)password;
#endif // MULTISTART
- (TCGSTATUSCODE) unprotectDrive:(TPer *)tPer password:(NSString *)password;
- (TCGSTATUSCODE) eraseDrive:(TPer *)tPer password:(NSString *)password;
- (TCGSTATUSCODE) changePasswordForDrive:(TPer *)tPer
                   oldPassword:(NSString *)oldPassword
                   newPassword:(NSString *)newPassword;
- (TCGSTATUSCODE) changePasswordForDrive:(TPer *)tPer
                      userName:(NSString *)userName
                 adminPassword:(NSString *)adminPassword
                   newPassword:(NSString *)newPassword;

// hostChallenge NSData *, DTA code will not hash
-(TCGSTATUSCODE)lockOrUnlockDrive:(TPer *)tPer
                locking:(BOOL) locking
          hostChallenge:(NSData *) hostChallenge
           continuation:(ResultContinuation) resultContinuation;
- (TCGSTATUSCODE) setupDrive:(TPer *)tPer hostChallenge:(NSData *) hostChallenge;
#if MULTISTART
- (TCGSTATUSCODE) multiUserSetupDrive:(TPer *)tPer hostChallenge:(NSData *)hostChallenge;
#endif // MULTISTART
- (TCGSTATUSCODE) unprotectDrive:(TPer *)tPer  hostChallenge:(NSData *) hostChallenge;
- (TCGSTATUSCODE) eraseDrive:(TPer *)tPer hostChallenge:(NSData *) hostChallenge;
- (TCGSTATUSCODE) changeHostChallengeForDrive:(TPer *)tPer
                   oldHostChallenge:(NSData *) oldHostChallenge
                   newHostChallenge:(NSData *) newHostChallenge;
- (TCGSTATUSCODE) changeHostChallengeForDrive:(TPer *)tPer
                           userName:(NSString *)userName
                 adminHostChallenge:(NSData *) adminHostChallenge
                   newHostChallenge:(NSData *) newHostChallenge;
@end
