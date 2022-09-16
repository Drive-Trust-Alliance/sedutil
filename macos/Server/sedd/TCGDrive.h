//
//  TCGDrive.h
//  sedagent
//
//  Created by Jackie Marks on 4/14/16.
//  Copyright © 2016 Bright Plaza Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

#include <sedutil/sedutil.h>


@interface TCGDrive : NSObject

@property (nonatomic, strong) NSString *serialNumber;

+(TCGDrive *) driveForBsdName: (NSString *) bsdName
                driverService: (io_registry_entry_t) service
                      connect: (io_connect_t) cnct;

-(TCGSTATUSCODE) getMSID:(NSMutableString *)MSID;

-(TCGSTATUSCODE) revertWithPSID: (NSString *) PSID;

-(TCGSTATUSCODE) unlockDriveWithPassword: (NSString *) pwd;
-(TCGSTATUSCODE) lockDriveWithPassword: (NSString *) pwd;

-(TCGSTATUSCODE) setupDriveWithPassword: (NSString *) pwd;
#if MULTISTART
-(TCGSTATUSCODE) multiUserSetupDriveWithPassword: (NSString *) pwd;
#endif // MULTISTART
-(TCGSTATUSCODE) eraseDriveWithPassword: (NSString *) pwd;
-(TCGSTATUSCODE) unprotectDriveWithPassword: (NSString *) pwd;
-(TCGSTATUSCODE) changeOldPassword: (NSString *) pwdOld
                     toNewPassword: (NSString *) pwdNew;
-(TCGSTATUSCODE) changeUser: (NSString *)userName
              adminPassword: (NSString *) pwdAdmin
              toNewPassword: (NSString *) pwdNew;

// new non-hashing versions
-(TCGSTATUSCODE) unlockDriveWithHostChallenge: (NSData *) hostChallenge;
-(TCGSTATUSCODE) lockDriveWithHostChallenge: (NSData *) hostChallenge;

-(TCGSTATUSCODE) setupDriveWithHostChallenge: (NSData *) hostChallenge;
#if MULTISTART
-(TCGSTATUSCODE) multiUserSetupDriveWithHostChallenge: (NSData *) hostChallenge;
#endif // MULTISTART
-(TCGSTATUSCODE) eraseDriveWithHostChallenge: (NSData *) hostChallenge;
-(TCGSTATUSCODE) unprotectDriveWithHostChallenge: (NSData *) hostChallenge;
-(TCGSTATUSCODE) changeOldHostChallenge: (NSData *) hostChallengeOld
                     toNewHostChallenge: (NSData *) hostChallengeNew;
-(TCGSTATUSCODE) changeUser: (NSString *)userName
         adminHostChallenge: (NSData *) hostChallengeAdmin
         toNewHostChallenge: (NSData *) hostChallengeNew;


@end
