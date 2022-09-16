//
//  OSInterface.m
//  sedagent
//
//  Created by Scott Marks on 04/10/2016.
//  Copyright © 2016 Bright Plaza Inc. All rights reserved.
//

#import "OSInterface.h"

// STUBS -- must subclass

@implementation OSInterface
@synthesize manager;


#pragma mark -
#pragma mark TCG

// STUB: The layers above don't yet know how to compose TPer commands
//-(bool) isTPerLocked:(TPer *)tPer {return false;}


-(bool) sendIFCommand:(TCG_IO_COMMAND  __unused)tcg_io_command
                 tPer:(TPer * __unused)tPer
               buffer:(void * __unused)buffer
              pLength:(uint64_t * __unused )pLength {
    return false;
}
-(bool) sendIFSendTper:(TPer * __unused)tPer buffer:(void * __unused)buffer pLength:(uint64_t * __unused)pLength {
    return false;
}
-(bool) sendIFRecvTper:(TPer * __unused)tPer buffer:(void * __unused)buffer pLength:(uint64_t * __unused)pLength {
    return false;
}

-(TCGSTATUSCODE) revertDrive:(TPer * __unused)tPer PSID:(NSString * __unused)PSID
                continuation:(ResultContinuation __unused) resultContinuation { return FAIL; }
-(TCGSTATUSCODE) getMSID:(NSMutableString * __unused)MSID forTPer:(TPer * __unused)tPer { return FAIL; }

// password NSString *, DTA code will hash
-(TCGSTATUSCODE)lockOrUnlockDrive:(TPer * __unused)tPer
                locking:(BOOL __unused) locking
               password:(NSString * __unused)pwd
           continuation:(ResultContinuation __unused) resultContinuation { return FAIL; };

-(TCGSTATUSCODE) setupDrive:(TPer * __unused)tPer password:(NSString * __unused)password { return FAIL; }
-(TCGSTATUSCODE) eraseDrive:(TPer * __unused)tPer password:(NSString * __unused)password { return FAIL; }
-(TCGSTATUSCODE) changePasswordForDrive:(TPer * __unused)tPer
                   oldPassword:(NSString * __unused)oldPassword
                   newPassword:(NSString * __unused)newPassword
{ return FAIL; }
-(TCGSTATUSCODE) changePasswordForDrive:(TPer * __unused)tPer
                      userName:(NSString * __unused)userName
                   oldPassword:(NSString * __unused)oldPassword
                   newPassword:(NSString * __unused)newPassword
{ return FAIL; }

// hostChallenge NSData *, DTA code will not hash
-(TCGSTATUSCODE)lockOrUnlockDrive:(TPer * __unused)tPer
                locking:(BOOL __unused) locking
          hostChallenge:(NSData * __unused) hostChallenge
           continuation:(ResultContinuation __unused) resultContinuation { return FAIL; };

-(TCGSTATUSCODE) setupDrive:(TPer * __unused)tPer hostChallenge:(NSData * __unused) hostChallenge { return FAIL; }
-(TCGSTATUSCODE) eraseDrive:(TPer * __unused)tPer hostChallenge:(NSData * __unused) hostChallenge { return FAIL; }
-(TCGSTATUSCODE) changeHostChallengeForDrive:(TPer * __unused)tPer
                   oldHostChallenge:(NSData * __unused) oldHostChallenge
                   newHostChallenge:(NSData * __unused) newHostChallenge
{ return FAIL; }
-(TCGSTATUSCODE) changeHostChallengeForDrive:(TPer * __unused)tPer
                           userName:(NSString * __unused) userName
                   oldHostChallenge:(NSData * __unused) oldHostChallenge
                   newHostChallenge:(NSData * __unused) newHostChallenge
{ return FAIL; }

@end
