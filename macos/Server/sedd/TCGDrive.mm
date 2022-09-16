//
//  TCGDrive.m
//  sedagent
//
//  Created by Jackie Marks on 4/14/16.
//  Copyright © 2016 Bright Plaza Inc. All rights reserved.
//

#import <string>
#import <SEDKernelInterface/UserKernelShared.h>
#import "TCGDrive.h"
#include "debug.h"
#include <sedutil/sedutil.h>

@interface TCGDrive ()
@property (nonatomic,assign) OPAL_DiskInfo DtaDiskInfo;
@property (nonatomic,assign) io_registry_entry_t driverService;
@property (nonatomic,assign) io_connect_t connect;
@property (nonatomic,strong) NSString *bsdName;

@property (nonatomic,strong) NSString *modelNumber;
@property (nonatomic,strong) NSString *firmwareRevision;


-(bool) identifyDevice;
-(void) discovery0;

@end

@interface DTADevice: NSObject
+(instancetype) deviceForDrive:(TCGDrive *)drive ;
@property (nonatomic,assign) DtaDevOS * dtaDev;
@end
@implementation DTADevice
@synthesize dtaDev;
+(instancetype) deviceForDrive:(TCGDrive *)drive {
    return [[self alloc] initForDrive:drive];
}

-(id)initForDrive:(TCGDrive *)drive {
    if ((self=[super init])) {
        char * cBsdName = const_cast<char *>([drive.bsdName cStringUsingEncoding:NSUTF8StringEncoding]);
        
        if ((bool)drive.DtaDiskInfo.OPAL20) {
            self.dtaDev = new DtaDevOpal2();
        }
#if OPAL1_SUPPORTED
        else if ((bool)drive.DtaDiskInfo.OPAL10) {
            self.dtaDev = new DtaDevOpal1();
        }
#endif //OPAL1_SUPPORTED
#if ENTERPRISE_SUPPORTED
        else if ((bool)drive.DtaDiskInfo.Enterprise) {
            self.dtaDev = new DtaDevEnterprise();
        }
#endif // ENTERPRISE_SUPPORTED
        else {
            self.dtaDev = nullptr;
        }// note we've already put in discovery0 info above
        if (self.dtaDev) {
            self.dtaDev->init(cBsdName, drive.driverService, drive.connect, drive.DtaDiskInfo);
        }
    }
    return self;
}
-(void)dealloc {
    if (dtaDev) delete dtaDev;
}
@end



@implementation TCGDrive

@synthesize driverService;
@synthesize connect;
@synthesize bsdName;
@synthesize DtaDiskInfo;


@synthesize serialNumber;
@synthesize modelNumber;
@synthesize firmwareRevision;


+(TCGDrive *) driveForBsdName: (NSString *) bsdName
                driverService: (io_registry_entry_t) service
                      connect: (io_connect_t) cnct
{
    return [[TCGDrive alloc] initForBsdName: bsdName
                              driverService:service
                                    connect:cnct];
}

-(TCGDrive *) initForBsdName: (NSString *) inBsdName
               driverService: (io_registry_entry_t) service
                     connect: (io_connect_t) cnct
{
    // do not know if we need the mseddata info before locking/unlocking, but just reading from registry


    if ( (self=[super init]) ) {
        timespec t = { 0, 1000000 }; // 1 millisecond
        self.bsdName = inBsdName;
        self.connect = cnct;
        self.driverService = service;
        int i = 0;
        for ( ; ![self identifyDevice] && i < 10; i++ ) {       // give it time to end up in registry
            nanosleep( &t, NULL );
        }
        if ( i == 10 )
            return nil;
//        [self identifyDevice];
        [self discovery0];
    }
    return self;
}

-(bool) identifyDevice {
    // does not actually do an identify device request to the drive, just reads from the registry
    CFDictionaryRef dict = (CFDictionaryRef)IORegistryEntryCreateCFProperty(self.driverService,
                                                                            CFSTR(IOIdentifyCharacteristicsKey),
                                                                            CFAllocatorGetDefault(), 0);

    if (!dict) return false;
    
    NSString * paddedSerialNumber = (__bridge NSString *)CFDictionaryGetValue(dict, CFSTR(IOSerialNumberKey));
    [self setValue: [paddedSerialNumber stringByTrimmingCharactersInSet: [NSCharacterSet whitespaceCharacterSet]]
            forKey: @"SerialNumber"];
    
    NSString * paddedModelNumber = (__bridge NSString *)CFDictionaryGetValue(dict, CFSTR(IOModelNumberKey));
    self.modelNumber = [paddedModelNumber stringByTrimmingCharactersInSet: [NSCharacterSet whitespaceCharacterSet] ];
    
    NSString * paddedFirmwareRevision = (__bridge NSString *)CFDictionaryGetValue(dict, CFSTR(IOFirmwareRevisionKey));
    self.firmwareRevision = [paddedFirmwareRevision stringByTrimmingCharactersInSet: [NSCharacterSet whitespaceCharacterSet] ];
    CFRelease(dict);
    return true;
}

-(void) discovery0 {
    CFDataRef data = (CFDataRef)IORegistryEntryCreateCFProperty(self.driverService,
                                                                CFSTR(IODiscovery0ResponseKey),
                                                                CFAllocatorGetDefault(), 0);
    const uint8_t * d0Response = CFDataGetBytePtr(data);
    OPAL_DiskInfo & disk_info = DtaDiskInfo;
    parseDiscovery0Features(d0Response, disk_info);
    CFRelease(data);
}

static inline vector<uint8_t> token(NSData * data) {
    const uint8_t * bytes=static_cast<const uint8_t *>(data.bytes);
    const size_t length=static_cast<const size_t>(data.length);
    assert(length<253);
    vector<uint8_t> t({0xD0,static_cast<uint8_t>(length)});
    t.reserve(length+2);  //for efficiency, make room all at once;
    t.insert(t.end(),bytes,bytes+length); // merge in the bytes from data
    return t;
}

///////////////// adding the making of an msed dev to unlock & lock
-(TCGSTATUSCODE) revertWithPSID: (NSString *) PSID
{
    TCGSTATUSCODE res = SUCCESS;
    @autoreleasepool {
        DTADevice * d = [DTADevice deviceForDrive:self];
        size_t len = [PSID lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
        
        char password[len+1];
        
        strlcpy(password, [PSID cStringUsingEncoding:NSUTF8StringEncoding], len+1);
        
        res = (TCGSTATUSCODE) d.dtaDev->revertTPer(password, 1, 0);
        NSLOG_DEBUG( @"in revertWithPSID after revertTPer, res = %d\n", res);
    }
    return res;
}

-(TCGSTATUSCODE) unlockDriveWithPassword: (NSString *) pwd
{
    TCGSTATUSCODE res = SUCCESS;
    @autoreleasepool {
        DTADevice * d = [DTADevice deviceForDrive:self];
        size_t len = [pwd lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
        
        char password[len+1];
        
        strlcpy(password, [pwd cStringUsingEncoding:NSUTF8StringEncoding], len+1);
        
        // Some drives, at least the Crucial MX200, do not properly set "locked" when
        // power-cycle reset.  So we just don't try.
        if ( DtaDiskInfo.Locking_lockingEnabled && DtaDiskInfo.Locking_locked ) {
            res = (TCGSTATUSCODE) d.dtaDev->setLockingRange(0, READWRITE, password);
            NSLOG_DEBUG( @"in unlockDrive after setLockingRange, res = %d\n", res);
        }
        
        if ( SUCCESS != res) goto EXIT;
        
        // note - msed mixes up TCGSTATUSCODE and uint8 - need to cast here
        // but should probably use error codes all the way through
        // in general DtaDiskInfo is not going to be updated
        // but with SEDAccess this won't be a problem, since drive disappears
        // from SEDAccess when unlocked (and uses both methods.)
        // perhaps a driver method - isLockedViaMBR?
        
        if (self.DtaDiskInfo.Locking_MBREnabled && !self.DtaDiskInfo.Locking_MBRDone ) {
            res = (TCGSTATUSCODE) d.dtaDev->setMBRDone( 1 , password);
            NSLOG_DEBUG( @"in unlockDriveWithPassword after setMBRDone, res = %d\n", res);
        }
    EXIT:;
    }
    return res;
}

-(TCGSTATUSCODE) unlockDriveWithHostChallenge:(NSData *)hostChallenge
{
    TCGSTATUSCODE res = SUCCESS;
    @autoreleasepool {
        DTADevice * d = [DTADevice deviceForDrive:self];
        
        vector<uint8_t> hc=token(hostChallenge);
        
        // Some drives, at least the Crucial MX200, do not properly set "locked" when
        // power-cycle reset.  So we just don't try.
        if ( DtaDiskInfo.Locking_lockingEnabled && DtaDiskInfo.Locking_locked ) {
            res = (TCGSTATUSCODE) d.dtaDev->setLockingRange(0, READWRITE, hc);
            NSLOG_DEBUG( @"in unlockDrive after setLockingRange, res = %d\n", res);
        }
        
        if ( SUCCESS != res) goto EXIT;
        
        // note - msed mixes up TCGSTATUSCODE and uint8 - need to cast here
        // but should probably use error codes all the way through
        // in general DtaDiskInfo is not going to be updated
        // but with SEDAccess this won't be a problem, since drive disappears
        // from SEDAccess when unlocked (and uses both methods.)
        // perhaps a driver method - isLockedViaMBR?
        
        if (self.DtaDiskInfo.Locking_MBREnabled && !self.DtaDiskInfo.Locking_MBRDone ) {
            res = (TCGSTATUSCODE) d.dtaDev->setMBRDone( 1 , hc);
            NSLOG_DEBUG( @"in unlockDriveWithHostChallenge after setMBRDone, res = %d\n", res);
        }
    EXIT:;
    }
    return res;
}

-(TCGSTATUSCODE) lockDriveWithPassword: (NSString *) pwd
{
    TCGSTATUSCODE res = SUCCESS;
    @autoreleasepool {
        DTADevice * d = [DTADevice deviceForDrive:self];
        
        size_t len = [pwd lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
        
        char password[len+1];
        
        strlcpy(password, [pwd cStringUsingEncoding:NSUTF8StringEncoding], len+1);
        
        if ( DtaDiskInfo.Locking_lockingEnabled && !self.DtaDiskInfo.Locking_locked ) {
            res = (TCGSTATUSCODE) d.dtaDev->setLockingRange(0, LOCKED, password);
            NSLOG_DEBUG( @"in lockDriveWithPassword after setMBRDone, res = %d\n", res);
        }
        
        if ( SUCCESS != res) goto EXIT;
        
        if ( self.DtaDiskInfo.Locking_MBREnabled && self.DtaDiskInfo.Locking_MBRDone ) {
            res = (TCGSTATUSCODE) d.dtaDev->setMBRDone( 0 , password);
            NSLOG_DEBUG( @"in lockDriveWithPassword after setMBRDone, res = %d\n", res);
        }
    EXIT:;
    }
    return res;
}

-(TCGSTATUSCODE) lockDriveWithHostChallenge: (NSData *) hostChallenge
{
    TCGSTATUSCODE res = SUCCESS;
    @autoreleasepool {
        DTADevice * d = [DTADevice deviceForDrive:self];
        
        vector<uint8_t> hc=token(hostChallenge);
        if ( DtaDiskInfo.Locking_lockingEnabled && !self.DtaDiskInfo.Locking_locked ) {
            res = (TCGSTATUSCODE) d.dtaDev->setLockingRange(0, LOCKED, hc);
            NSLOG_DEBUG( @"in lockDriveWithHostChallenge after setMBRDone, res = %d\n", res);
        }
        
        if ( SUCCESS != res) goto EXIT;
        
        if ( self.DtaDiskInfo.Locking_MBREnabled && self.DtaDiskInfo.Locking_MBRDone ) {
            res = (TCGSTATUSCODE) d.dtaDev->setMBRDone( 0 , hc);
            NSLOG_DEBUG( @"in lockDriveWithHostChallenge after setMBRDone, res = %d\n", res);
        }
    EXIT:;
    }
    return res;
}

-(TCGSTATUSCODE) setupDriveWithPassword: (NSString *) pwd
{
    TCGSTATUSCODE res = SUCCESS;
    @autoreleasepool {
        DTADevice * d = [DTADevice deviceForDrive:self];
        
        size_t len = [pwd lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
        
        char password[len+1];
        
        strlcpy(password, [pwd cStringUsingEncoding:NSUTF8StringEncoding], len+1);
        
        res = (TCGSTATUSCODE) d.dtaDev->initialSetup(password);
        NSLOG_DEBUG( @"in setupDriveWithPassword after initialSetup, res = %d\n", res);
        if ( SUCCESS != res) goto EXIT;
        
        res = (TCGSTATUSCODE) d.dtaDev->configureLockingRange(0, 3, password);
        NSLOG_DEBUG( @"in setupDriveWithPassword after configureLockingRange, res = %d\n", res);
        
    EXIT:;
    }
    return res;
}

-(TCGSTATUSCODE) setupDriveWithHostChallenge: (NSData *) hostChallenge
{
    TCGSTATUSCODE res = SUCCESS;
    @autoreleasepool {
        DTADevice * d = [DTADevice deviceForDrive:self];
        
        vector<uint8_t> hc=token(hostChallenge);
        
        res = (TCGSTATUSCODE) d.dtaDev->initialSetup(hc);
        NSLOG_DEBUG( @"in setupDriveWithHostChallenge after initialSetup, res = %d\n", res);
        if ( SUCCESS != res) goto EXIT;
        
        res = (TCGSTATUSCODE) d.dtaDev->configureLockingRange(0, 3, hc);
        NSLOG_DEBUG( @"in setupDriveWithHostChallenge after configureLockingRange, res = %d\n", res);
        
    EXIT:;
    }
    return res;
}

#if MULTISTART
-(TCGSTATUSCODE) multiUserSetupDriveWithPassword: (NSString *) pwd
{
    TCGSTATUSCODE res = SUCCESS;
    @autoreleasepool {
        DTADevice * d = [DTADevice deviceForDrive:self];
        
        size_t len = [pwd lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
        
        char password[len+1];
        
        strlcpy(password, [pwd cStringUsingEncoding:NSUTF8StringEncoding], len+1);
        
        res = (TCGSTATUSCODE) d.dtaDev->initialSetup(password);
        NSLOG_DEBUG( @"in multiUserSetupDriveWithPassword after initialSetup, res = %d\n", res);
        if ( SUCCESS != res) goto EXIT;
        
        res = (TCGSTATUSCODE) d.dtaDev->configureLockingRange(0, 3, password);
        NSLOG_DEBUG( @"in multiUserSetupDriveWithPassword after configureLockingRange, res = %d\n", res);
        if ( SUCCESS != res) goto EXIT;
        
        res = (TCGSTATUSCODE) d.dtaDev->multiUserSetup(password);
        NSLOG_DEBUG( @"in multiUserSetupDriveWithPassword after multiUserSetup, res = %d\n", res);
        
    EXIT:;
    }
    return res;
}

-(TCGSTATUSCODE) multiUserSetupDriveWithHostChallenge: (NSData *) hostChallenge
{
    TCGSTATUSCODE res = SUCCESS;
    @autoreleasepool {
        DTADevice * d = [DTADevice deviceForDrive:self];
        
        vector<uint8_t> hc=token(hostChallenge);
        
        res = (TCGSTATUSCODE) d.dtaDev->initialSetup(hc);
        NSLOG_DEBUG( @"in multiUserSetupDriveWithHostChallenge after initialSetup, res = %d\n", res);
        if ( SUCCESS != res) goto EXIT;
        
        res = (TCGSTATUSCODE) d.dtaDev->configureLockingRange(0, 3, hc);
        NSLOG_DEBUG( @"in multiUserSetupDriveWithHostChallenge after configureLockingRange, res = %d\n", res);
        if ( SUCCESS != res) goto EXIT;
        
        res = (TCGSTATUSCODE) d.dtaDev->multiUserSetup(hc);
        NSLOG_DEBUG( @"in multiUserSetupDriveWithPassword after multiUserSetup, res = %d\n", res);
        
    EXIT:;
    }
    return res;
}
#endif // MULTISTART

-(TCGSTATUSCODE) unprotectDriveWithPassword: (NSString *) pwd  // same as changeOldPassword:pwd toNewPassword:@""
{
    TCGSTATUSCODE res = SUCCESS;
    @autoreleasepool {
        DTADevice * d = [DTADevice deviceForDrive:self];
        
        size_t len = [pwd lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
        
        char password[len+1];
        
        strlcpy(password, [pwd cStringUsingEncoding:NSUTF8StringEncoding], len+1);
        
        res = (TCGSTATUSCODE)d.dtaDev->setLockingRange(0, OPAL_LOCKINGSTATE::READWRITE, password);
        NSLOG_DEBUG( @"in unprotectDriveWithPassword after setLockingRange, res = %d\n", res);
        if ( SUCCESS != res) goto EXIT;
        
        res = (TCGSTATUSCODE)d.dtaDev->revertLockingSP(password, 1);
        NSLOG_DEBUG( @"in unprotectDriveWithPassword after revertLockingSP, res = %d\n", res);
        if ( SUCCESS != res) goto EXIT;
        
        res = (TCGSTATUSCODE)d.dtaDev->revertTPer(password, 0, 0);
        NSLOG_DEBUG( @"in unprotectDriveWithPassword after revertTPer, res = %d\n", res);
        
    EXIT:;
    }
    return res;
}


-(TCGSTATUSCODE) unprotectDriveWithHostChallenge: (NSData *) hostChallenge  // same as changeOldPassword:pwd toNewPassword:@""
{
    TCGSTATUSCODE res = SUCCESS;
    @autoreleasepool {
        DTADevice * d = [DTADevice deviceForDrive:self];
        
        vector<uint8_t> hc=token(hostChallenge);
        
        res = (TCGSTATUSCODE)d.dtaDev->setLockingRange(0, OPAL_LOCKINGSTATE::READWRITE, hc);
        NSLOG_DEBUG( @"in unprotectDriveWithPassword after setLockingRange, res = %d\n", res);
        if ( SUCCESS != res) goto EXIT;
        
        res = (TCGSTATUSCODE)d.dtaDev->revertLockingSP(hc, 1);
        NSLOG_DEBUG( @"in unprotectDriveWithPassword after revertLockingSP, res = %d\n", res);
        if ( SUCCESS != res) goto EXIT;
        
        res = (TCGSTATUSCODE)d.dtaDev->revertTPer(hc, 0, 0);
        NSLOG_DEBUG( @"in unprotectDriveWithPassword after revertTPer, res = %d\n", res);
        
    EXIT:;
    }
    return res;
}



-(TCGSTATUSCODE) changeOldPassword: (NSString *) pwdOld
                     toNewPassword: (NSString *) pwdNew
{
    TCGSTATUSCODE res = SUCCESS;
    @autoreleasepool {
        DTADevice * d = [DTADevice deviceForDrive:self];
        
        size_t oLen = [pwdOld lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
        size_t nLen = [pwdNew lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
        
        char oPassword[oLen+1];
        char nPassword[nLen+1];
        
        strlcpy(oPassword, [pwdOld cStringUsingEncoding:NSUTF8StringEncoding], oLen+1);
        strlcpy(nPassword, [pwdNew cStringUsingEncoding:NSUTF8StringEncoding], nLen+1);
        
        if (strnlen(nPassword, 250) == 0) { // removing password
            
            res = (TCGSTATUSCODE)d.dtaDev->setLockingRange(0, OPAL_LOCKINGSTATE::READWRITE, oPassword);
            NSLOG_DEBUG( @"in changePassword after setLockingRange, res = %d\n", res);
            if ( SUCCESS != res) goto EXIT;
            
            res = (TCGSTATUSCODE)d.dtaDev->revertLockingSP(oPassword, 1);
            NSLOG_DEBUG( @"in changePassword after revertLockingSP, res = %d\n", res);
            if ( SUCCESS != res) goto EXIT;
            
            res = (TCGSTATUSCODE)d.dtaDev->revertTPer(oPassword, 0, 0);
            NSLOG_DEBUG( @"in changePassword after revertTPer, res = %d\n", res);
            
        } else {
            
            res = (TCGSTATUSCODE)d.dtaDev->setPassword(oPassword, (char *) "Admin1", nPassword);
            NSLOG_DEBUG( @"in changePassword after setPassword, res = %d\n", res);
            if ( SUCCESS != res) goto EXIT;
            
            res = (TCGSTATUSCODE)d.dtaDev->setSIDPassword(oPassword, nPassword);
            NSLOG_DEBUG( @"in changePassword after setSIDPassword, res = %d\n", res);
        }
    EXIT:;
    }
    return res;
}



-(TCGSTATUSCODE) changeOldHostChallenge:(NSData *)oldHostChallenge
                     toNewHostChallenge:(NSData *)newHostChallenge
{
    TCGSTATUSCODE res = SUCCESS;
    @autoreleasepool {
        DTADevice * d = [DTADevice deviceForDrive:self];
        
        vector<uint8_t> oldhc=token(oldHostChallenge);
        vector<uint8_t> newhc=token(newHostChallenge);
        
        if (newhc.size() == 0) { // removing password
            
            res = (TCGSTATUSCODE)d.dtaDev->setLockingRange(0, OPAL_LOCKINGSTATE::READWRITE, oldhc);
            NSLOG_DEBUG( @"in changeHostChallenge after setLockingRange, res = %d\n", res);
            if ( SUCCESS != res) goto EXIT;
            
            res = (TCGSTATUSCODE)d.dtaDev->revertLockingSP(oldhc, 1);
            NSLOG_DEBUG( @"in changeHostChallenge after revertLockingSP, res = %d\n", res);
            if ( SUCCESS != res) goto EXIT;
            
            res = (TCGSTATUSCODE)d.dtaDev->revertTPer(oldhc, 0, 0);
            NSLOG_DEBUG( @"in changeHostChallenge after revertTPer, res = %d\n", res);
            
        } else {
            
            res = (TCGSTATUSCODE)d.dtaDev->setPassword(oldhc, (char *) "Admin1", newhc);
            NSLOG_DEBUG( @"in changeHostChallenge after setPassword, res = %d\n", res);
            if ( SUCCESS != res) goto EXIT;
            
            res = (TCGSTATUSCODE)d.dtaDev->setSIDPassword(oldhc, newhc);
            NSLOG_DEBUG( @"in changeHostChallenge after setSIDPassword, res = %d\n", res);
        }
    EXIT:;
    }
    return res;
}

-(TCGSTATUSCODE) changeUser:(NSString *)userName
                adminPassword: (NSString *) pwdAdmin
              toNewPassword: (NSString *) pwdNew
{
    if ([@"Admin1" isEqualToString:userName])
        return [self changeOldPassword:pwdAdmin toNewPassword:pwdNew];
    TCGSTATUSCODE res = SUCCESS;
    @autoreleasepool {
        DTADevice * d = [DTADevice deviceForDrive:self];
        
        size_t uLen = [userName lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
        size_t aLen = [pwdAdmin lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
        size_t nLen = [pwdNew lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
        
        char user[uLen+1];
        char aPassword[aLen+1];
        char nPassword[nLen+1];
        
        strlcpy(user, [userName cStringUsingEncoding:NSUTF8StringEncoding], uLen+1);
        strlcpy(aPassword, [pwdAdmin cStringUsingEncoding:NSUTF8StringEncoding], aLen+1);
        strlcpy(nPassword, [pwdNew cStringUsingEncoding:NSUTF8StringEncoding], nLen+1);
        
        uint8_t result;
        if (nLen == 0) {
            result = d.dtaDev->enableUser(aPassword, user, OPAL_FALSE);
            NSLOG_DEBUG( @"in changePassword after enableUser, res = %d\n", result);
        } else {
            result = (TCGSTATUSCODE)d.dtaDev->setPassword(aPassword, user, nPassword);
            NSLOG_DEBUG( @"in changePassword after setPassword, res = %d\n", result);
            if (0==result) {
                result = d.dtaDev->enableUser(aPassword, user, OPAL_TRUE);
                NSLOG_DEBUG( @"in changePassword after enableUser, res = %d\n", res);
            }
        }
        if (result) res=FAIL;
    }
    return res;
}



-(TCGSTATUSCODE) changeUser:(NSString *)userName
           adminHostChallenge:(NSData *)adminHostChallenge
         toNewHostChallenge:(NSData *)newHostChallenge
{
    if ([@"Admin1" isEqualToString:userName])
        return [self changeOldHostChallenge:adminHostChallenge toNewHostChallenge:newHostChallenge];
    TCGSTATUSCODE res = SUCCESS;
    @autoreleasepool {
        DTADevice * d = [DTADevice deviceForDrive:self];
        
        size_t uLen = [userName lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
        char user[uLen+1];
        strlcpy(user, [userName cStringUsingEncoding:NSUTF8StringEncoding], uLen+1);
        vector<uint8_t> adminhc=token(adminHostChallenge);
        
        uint8_t result;
        if (newHostChallenge.length == 0) {
            result = d.dtaDev->enableUser(adminhc, user, OPAL_FALSE);
            NSLOG_DEBUG( @"in changePassword after enableUser, res = %d\n", result);
        } else {
            vector<uint8_t> newhc=token(newHostChallenge);
            result = (TCGSTATUSCODE)d.dtaDev->setPassword(adminhc, user, newhc);
            NSLOG_DEBUG( @"in changePassword after setPassword, res = %d\n", result);
            if (0==result) {
                result = d.dtaDev->enableUser(adminhc, user, OPAL_TRUE);
                NSLOG_DEBUG( @"in changePassword after enableUser, res = %d\n", res);
            }
        }
        if (result) res=FAIL;
    }
    return res;
}

-(TCGSTATUSCODE) eraseDriveWithPassword: (NSString *) pwd
{
    TCGSTATUSCODE res = SUCCESS;
    @autoreleasepool {
        DTADevice * d = [DTADevice deviceForDrive:self];
        
        size_t len = [pwd lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
        
        char password[len+1];
        
        strlcpy(password, [pwd cStringUsingEncoding:NSUTF8StringEncoding], len+1);
        
        res = (TCGSTATUSCODE) d.dtaDev->revertTPer(password);
        
        NSLOG_DEBUG( @"in eraseDrive, after revertTPer, res = %d\n", res);
    }
    return res;
}


-(TCGSTATUSCODE) eraseDriveWithHostChallenge:(NSData *)hostChallenge
{
    TCGSTATUSCODE res = SUCCESS;
    @autoreleasepool {
        DTADevice * d = [DTADevice deviceForDrive:self];
        
        vector<uint8_t> hc=token(hostChallenge);
        
        res = (TCGSTATUSCODE) d.dtaDev->revertTPer(hc);
        
        NSLOG_DEBUG( @"in eraseDrive, after revertTPer, res = %d\n", res);
    }
    return res;
}


-(TCGSTATUSCODE) getMSID:(NSMutableString *)MSID
{
    TCGSTATUSCODE res = SUCCESS;
    @autoreleasepool {
        DTADevice * d = [DTADevice deviceForDrive:self];
        DtaDevOS  * devOS = d.dtaDev;
        DtaDevOpal * dev = dynamic_cast<DtaDevOpal *>(devOS);
        string m;
        if (dev->getMSID(m))
        {
            LOG(E) << "setPassword failed to retrieve MSID";
            return FAIL;
        }
        NSLOG_DEBUG( @"in eraseDrive, after revertTPer, res = %d\n", res);
        NSString * mm = [NSString stringWithCString:m.c_str() encoding:NSUTF8StringEncoding];
        [MSID setString:mm];
    }
    return res;
}




@end
