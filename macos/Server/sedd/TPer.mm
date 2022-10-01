//
//  TPer.mm
//  sedagent
//
//  Created by Scott Marks on 04/10/2016.
//  Copyright © 2016 Bright Plaza Inc. All rights reserved.
//

#import <IOKit/IOBSD.h>
#import <SEDKernelInterface/UserKernelShared.h>
#import <SEDClientServerCommunication/NSData+Hashing.h>
#import "MacOSXInterface.h"
//#import "MacRegistryAccess.h"
#include "debug.h"

#import "TPer.h"






@interface TPer()
#if NONSTUB
@property(nonatomic,assign) SessionNumber TSN;
@property(nonatomic,assign) SessionNumber HSN;
@property(nonatomic,assign) ComID comID;
#endif //NONSTUB
@property(nonatomic,assign) OPAL_DiskInfo diskInfo;
@property(nonatomic,strong) TPerDriverRegistryEntry * driverHandle;
@property(nonatomic,strong) NSString * SSCName;
@property(nonatomic,assign) bool isLocked;
@property (nonatomic, readonly) MacRegistryAccess *registryAccess;
@end

@implementation TPer
#if NONSTUB
@synthesize TSN;
@synthesize HSN;
@synthesize comID;
#endif // NONSTUB
@synthesize diskInfo;
@synthesize driverHandle;
@synthesize SSCName;
@synthesize isLocked;


// Mac-specific - change later?
-(MacRegistryAccess *) registryAccess {
    return [MacRegistryAccess sharedAccess];
}


-(id) initWithCharacteristics:(NSDictionary *)characteristics
           blockStorageDriver:(IORegistryEntry *)b
             tPerDriverHandle:(TPerDriverRegistryEntry *)tPerDriver
                    interface:(MacOSXInterface *) i {
    if ((self=[super initWithCharacteristics:characteristics blockStorageDriver:b interface:i])) {
        self.driverHandle = tPerDriver;
        NSData * d0ResponseData = [tPerDriver dataPropertyForKey: @IODiscovery0ResponseKey];
        parseDiscovery0Features(static_cast<const uint8_t *>(d0ResponseData.bytes), diskInfo);
        self.SSCName = self.diskInfo.OPAL20     ? @"Opal 2"
                     : self.diskInfo.OPAL10     ? @"Opal 1"
                     : self.diskInfo.Enterprise ? @"Enterprise"
                     : @"<SSC n/a>";
        NSData * worldWideName =
            [[tPerDriver dictPropertyForKey:@IOIdentifyCharacteristicsKey] objectForKey:@IOWorldWideNameKey];
        [self setUniqueIDFromWorldWideName: worldWideName];
        self.isCurrentlyBeingLocked = false;
    }
    return self;
}


-(NSData *) hash:(NSString *)password {
    const char * pwd=[password UTF8String];
    const char * salt=[self.serialNumber UTF8String];
    NSData * data=[NSData keyDerivedFromDataBytes:pwd dataLength:strlen(pwd)
                                        saltBytes:salt saltLength:strlen(salt)];
    return data;
}


-(NSString *) hashAndEncode:(NSString *)password {
    NSData * data=[self hash:password];
    NSString * s=[data base64EncodedStringWithOptions:0];
    return s;
}

#define TPersKey @"TPers"
#define adminHostChallengeKey @"adminHostChallenge" // TODO: Shift this to the KeyStore?
#define rememberAdminHostChallengeKey @"rememberAdminHostChallenge"
#define unlockAutomaticallyKey @"autoUnlock"
#define isCurrentlyBeingLockedKey @"isCurrentlyBeingLocked"

static NSDictionary * tPersPreferences() {
    NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
    assert([defaults synchronize]);
    NSDictionary * preferences = [defaults valueForKey:TPersKey];
    return preferences;
}

static
void setTPersPreferences(NSDictionary * preferences) {
    NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
    if (preferences && 0<preferences.count)
        [defaults setValue:preferences forKey:TPersKey];
    else
        [defaults removeObjectForKey:TPersKey];
    [defaults synchronize];
}


static
id __nullable tPerPreferenceForKey(NSString * name, NSString * key) {
    NSDictionary * preferences = tPersPreferences();
    NSDictionary * driveInfo = [preferences objectForKey:name];  // [nil objectForKey:...]=>nil
    return [driveInfo objectForKey:key]; // [nil objectForKey:...]=>nil
}

static
void setTPerPreferenceForKey(id __nullable object, NSString * name, NSString * key) {  // TODO: across hosts?
    NSDictionary * preferences = tPersPreferences();
    NSDictionary * driveInfo = [preferences objectForKey:name];
    
    NSMutableDictionary * updatedDriveInfo = [NSMutableDictionary dictionaryWithDictionary: driveInfo];
    if (object)
        [updatedDriveInfo setObject:(id _Nonnull)object forKey:key];
    else
        [updatedDriveInfo removeObjectForKey:key];
    
    NSMutableDictionary * updatedPreferences = [NSMutableDictionary dictionaryWithDictionary:preferences];
    if (0==updatedDriveInfo.count)
        [updatedPreferences removeObjectForKey:name];
    else
        [updatedPreferences setObject:updatedDriveInfo forKey:name];
    
    setTPersPreferences(updatedPreferences);
}

NSData * tPerRememberedAdminHostChallenge(NSString * name) {
    return tPerPreferenceForKey(name, adminHostChallengeKey); // [nil objectForKey:...]=>nil
}

void setTPerRememberedAdminHostChallenge(NSString * name, NSData * hostChallenge) {  // TODO: across hosts?
    setTPerPreferenceForKey(hostChallenge, name, adminHostChallengeKey);
}

-(NSData *) rememberedAdminHostChallenge {
    return tPerRememberedAdminHostChallenge(self.uniqueID); // [nil objectForKey:...]=>nil
}

-(void) setRememberedAdminHostChallenge:(NSData *)hostChallenge {  // TODO: across hosts?
    setTPerRememberedAdminHostChallenge(self.uniqueID, hostChallenge);
}


bool tPerUnlockAutomatically(NSString * name) {
    return ((NSNumber *)tPerPreferenceForKey(name, unlockAutomaticallyKey)).boolValue; // [nil objectForKey:...]=>nil
}

void setTPerUnlockAutomatically(NSString * name, bool autoUnlock) {  // TODO: across hosts?
    setTPerPreferenceForKey([NSNumber numberWithBool:autoUnlock], name, unlockAutomaticallyKey);
}

-(bool) autoUnlock {
    return tPerUnlockAutomatically(self.uniqueID); // [nil objectForKey:...]=>nil
}

-(void) setAutoUnlock:(bool)autoUnlock {  // TODO: across hosts?
    setTPerUnlockAutomatically(self.uniqueID, autoUnlock);
}


bool tPerIsCurrentlyBeingLocked(NSString * name) {
    return ((NSNumber *)tPerPreferenceForKey(name, isCurrentlyBeingLockedKey)).boolValue; // [nil objectForKey:...]=>nil
}

void setTPerIsCurrentlyBeingLocked(NSString * name, bool isCurrentlyBeingLocked) {  // TODO: across hosts?
    setTPerPreferenceForKey([NSNumber numberWithBool:isCurrentlyBeingLocked], name, isCurrentlyBeingLockedKey);
}

-(bool) isCurrentlyBeingLocked {
    return tPerIsCurrentlyBeingLocked(self.uniqueID); // [nil objectForKey:...]=>nil
}

-(void) setIsCurrentlyBeingLocked:(bool)isCurrentlyBeingLocked {  // TODO: across hosts?
    setTPerIsCurrentlyBeingLocked(self.uniqueID, isCurrentlyBeingLocked);
}




#if NONSTUB
-(bool) hashpwd {return true;}
#define SECTOR_SIZE 512
#define COMMAND_BUFFER_SIZE PAGE_SIZE
#define RESPONSE_BUFFER_SIZE PAGE_SIZE
-(bool) sendCommand:(NSData *)command pResponse:(NSData **)pResponse {
    NSData * dataSubpacket = [NSData dataSubpacketPayload:command];
    NSLOG_DEBUG(@"sendCommand:dataSubpacket=%@", dataSubpacket);
    NSData * packet = [NSData packetSubpacket:dataSubpacket TSN:TSN HSN:HSN];
    NSLOG_DEBUG(@"sendCommand:packet=%@", packet);
    NSData * compacket = [NSData compacketPacket:packet comID:comID];
    NSLOG_DEBUG(@"sendCommand:compacket=%@", compacket);

    int result;
    static void * commandBuffer = NULL;
    if (NULL == commandBuffer) {
        result = posix_memalign(&commandBuffer, COMMAND_BUFFER_SIZE, PAGE_SIZE);
        if ( 0!=result ) {
            return false;
        }
    }
    static void * responseBuffer = NULL;
    if (NULL == responseBuffer) {
        result = posix_memalign(&responseBuffer, RESPONSE_BUFFER_SIZE, PAGE_SIZE);
        if ( 0!=result ) {
            return false;
        }
    }

    bzero(commandBuffer, COMMAND_BUFFER_SIZE);
    bzero(responseBuffer, RESPONSE_BUFFER_SIZE);
    uint64_t transferLength;
    bool tranfserOK;

    transferLength=compacket.length;
    transferLength=((transferLength+SECTOR_SIZE-1)/SECTOR_SIZE)*SECTOR_SIZE;
    if (4096<transferLength) transferLength=COMMAND_BUFFER_SIZE;
    [compacket getBytes:commandBuffer length:transferLength];
    tranfserOK = [self.interface sendIFSend:self buffer:commandBuffer pLength:&transferLength];
    if (!tranfserOK) {
        return false;
    }
    // Check transferLength?

    transferLength=RESPONSE_BUFFER_SIZE;
    tranfserOK = [self.interface sendIFRecv:self buffer:responseBuffer pLength:&transferLength];
    if (!tranfserOK) {
        return false;
    }

    (*pResponse)= [NSData dataWithBytes:responseBuffer length:transferLength];

    return true;
}

-(bool) responseSpecifiesSuccess:(NSData *)response {
    const List * statusValues=[response parseList];
    if (!statusValues) return false;
    const Value * * values;
    size_t valuesLength;
    statusValues->getValues(values,valuesLength);
    return (3==valuesLength)
        && (zero==values[0])
        && (zero==values[1])
        && (zero==values[2]);
}

-(bool) startSessionWithPassword: (NSData *)password {
    bzero(TSN, sizeof(SessionNumber));
    bzero(HSN, sizeof(SessionNumber));
    ComID BogusComID={0x07, 0xFE};
    memcpy(comID, BogusComID, sizeof(ComID));
    int hostSessionID=105; //???
    NSData * pwd = self.hashpwd ? [self hash:password] : password;

    const MethodCall * pt=call(sessionManagement,
                               startSession,
                               list(atom(hostSessionID),
                                    lockingSP,
                                    one,
                                    name_value(atom(hostChallenge), pwd.atom),
                                    name_value(atom(hostSigningAuthority), admin1)),
                               list(zero,zero,zero));

    NSData * startSessionWithPasswordCommand = [NSData streamElement:pt];
    NSLOG_DEBUG(@"startSessionWithPasswordCommand=%@", startSessionWithPasswordCommand);

    NSData * response=nil;
    bool success= [self sendCommand: startSessionWithPasswordCommand pResponse: &response];
    // Aren't yet getting TSN from the response
    SessionNumber BogusTSN={0xFF, 0xFF, 0xFC, 0x00};
    SessionNumber BogusHSN={0x00, 0x00, 0x00, 0x69};
    memcpy(BogusTSN, TSN, sizeof(SessionNumber));
    memcpy(BogusHSN, HSN, sizeof(SessionNumber));
    return success && nil!=response && [self responseSpecifiesSuccess:response];
}

-(bool) unlockGlobalLockingRange {

    const MethodCall * pt=call(lockingRangeGlobal,
                               set,
                               list(name_value(atom(readLocked), zero),
                                    name_value(atom(writeLocked), zero)),
                               list(zero,zero,zero));
    NSData * unlockGlobalLockingRangecommand = [NSData streamElement:pt];
    NSLOG_DEBUG(@"unlockGlobalLockingRangecommand=%@", unlockGlobalLockingRangecommand);

    NSData * response = nil;
    bool transferOK = [self sendCommand: unlockGlobalLockingRangecommand pResponse:&response];

    return transferOK && nil!=response && [self responseSpecifiesSuccess:response];
}

-(bool) endSession {

    NSData * endSessionCommand =[NSData streamElement:&EndOfSession];
    NSLOG_DEBUG(@"endSessionCommand=%@", endSessionCommand);

    NSData * response = nil;
    bool transferOK = [self sendCommand: endSessionCommand pResponse:&response];

    return transferOK && nil!=response && [self responseSpecifiesSuccess:response];
}
#endif // NONSTUB
// STUB: the methods below should compute compackets and use them
// Instead, the MacOSInterface dangles flying spaghetti monster entrails
// for them to slurp.



static inline NSString * emptyIfNil(NSString * password) { return password ?: @"" ; }

-(bool) isSetUp {
    return [self.registryAccess isTPerSetUp:self];
}

-(bool) isLocked {
    return [self.registryAccess isTPerSetUpAndLocked:self];
}

-(bool) lockWithPassword: (NSString *)password
            continuation: (ResultContinuation) resultContinuation
{
    self.isCurrentlyBeingLocked = true;
    int ret = [self.interface lockOrUnlockDrive:self locking:YES password:emptyIfNil(password)
                                   continuation: resultContinuation ];
    // self.isCurrentlyBeingLocked = false; -- TOO SOON!  peek callback has (probably) not yet happened
    return ret == 0;
}

-(bool) lockWithHostChallenge:(NSData *)hostChallenge continuation:(ResultContinuation)resultContinuation
{
    self.isCurrentlyBeingLocked = true;
    int ret = [self.interface lockOrUnlockDrive:self locking:YES hostChallenge:hostChallenge
                                   continuation: resultContinuation ];
    // self.isCurrentlyBeingLocked = false; -- TOO SOON!  peek callback has (probably) not yet happened
    return ret == 0;
}

- (ResultContinuation) wrappedUnlockContinuation:(__nullable ResultContinuation) resultContinuation
                    autoUnlock:(BOOL)autoUnlock
                                   hostChallenge:(NSData *)hostChallenge {
    ResultContinuation wrappedContinuation=^(TCGSTATUSCODE result) {
        self.rememberedAdminHostChallenge = (autoUnlock && SUCCESS==result) ? hostChallenge : nil;
        self.autoUnlock = autoUnlock;
        if (resultContinuation) resultContinuation(result);
    };
    return wrappedContinuation;
}

-(bool) unlockWithPassword:(NSString *)password
       autoUnlock:(BOOL)autoUnlock
              continuation:(__nullable ResultContinuation) resultContinuation
{
    int ret = [self.interface lockOrUnlockDrive:self locking:NO password:emptyIfNil(password)
                                   continuation: [self wrappedUnlockContinuation:resultContinuation
                                                    autoUnlock:autoUnlock
                                                                   hostChallenge:[self hash:password]]];
    return ret == 0;
}


-(bool) unlockWithHostChallenge:(NSData *)hostChallenge
            autoUnlock:(BOOL)autoUnlock
                   continuation:(__nullable ResultContinuation)resultContinuation
{
    int ret = [self.interface lockOrUnlockDrive:self locking:NO hostChallenge:hostChallenge
                                   continuation:[self wrappedUnlockContinuation:resultContinuation
                                                            autoUnlock:autoUnlock
                                                                  hostChallenge:hostChallenge]];
    return ret == 0;
}



-(bool) eraseWithPassword: (NSString *)password {
    int ret = [self.interface eraseDrive:self password: emptyIfNil(password)];
    return ret == 0;
}

-(bool) eraseWithHostChallenge:(NSData *)hostChallenge {
    int ret = [self.interface eraseDrive:self hostChallenge: hostChallenge];
    return ret == 0;
}

-(bool) setupWithPassword: (NSString *)password {
    int ret = [self.interface setupDrive:self password:emptyIfNil(password)];
    return ret == 0;
}

-(bool) setupWithHostChallenge:(NSData *)hostChallenge {
    int ret = [self.interface setupDrive:self hostChallenge:hostChallenge];
    return ret == 0;
}

#if MULTISTART
-(bool) multiUserSetupWithPassword: (NSString *)password {
    int ret = [self.interface multiUserSetupDrive:self password:emptyIfNil(password)];
    return ret == 0;
}

-(bool) multiUserSetupWithHostChallenge:(NSData *)hostChallenge {
    int ret = [self.interface multiUserSetupDrive:self hostChallenge:hostChallenge];
    return ret == 0;
}
#endif  //  MULTISTART

-(bool) unprotectWithPassword: (NSString *)password {
    int ret = [self.interface unprotectDrive:self password:emptyIfNil(password)];
    return ret == 0;
}

-(bool) unprotectWithHostChallenge:(NSData *)hostChallenge {
    int ret = [self.interface unprotectDrive:self hostChallenge:hostChallenge];
    return ret == 0;
}

-(bool) changePassword: (NSString *)oldPassword to: (NSString *)newPassword
{
    int ret = [self.interface changePasswordForDrive:self
                                         oldPassword:emptyIfNil(oldPassword)
                                         newPassword:emptyIfNil(newPassword)];
    return ret == 0;
}

-(bool) changeHostChallenge:(NSData *)oldhostChallenge to:(NSData *)newHostChallenge
{
    int ret = [self.interface changeHostChallengeForDrive:self
                                         oldHostChallenge:oldhostChallenge
                                         newHostChallenge:newHostChallenge];
    return ret == 0;
}

-(bool) changeUser:(NSString *)userName adminPassword: (NSString *)adminPassword to: (NSString *)newPassword
{
    int ret = [self.interface changePasswordForDrive:self
                                            userName:userName
                                       adminPassword:emptyIfNil(adminPassword)
                                         newPassword:emptyIfNil(newPassword)];
    return ret == 0;
}

-(bool) changeUser:(NSString *)userName adminHostChallenge:(NSData *)adminHostChallenge to:(NSData *)newHostChallenge
{
    int ret = [self.interface changeHostChallengeForDrive:self
                                                 userName:userName
                                       adminHostChallenge:adminHostChallenge
                                         newHostChallenge:newHostChallenge];
    return ret == 0;
}

-(bool) revertWithPSID:(NSString *)PSID continuation:(ResultContinuation)continuation{
    int ret = [self.interface revertDrive:self PSID:PSID continuation:continuation];
    return ret == 0;
}

#if NONSTUB
-(void) getComID:(uint8_t *)buffer {
    buffer[0]=comID[1];
    buffer[1]=comID[0];
}
#endif //NONSTUB



-(NSString *)base64Data {
    @autoreleasepool {
#if DEBUG
        NSLOG_DEBUG(@"base64DataForTPer: trying to fill base64 data for %@", self.uniqueID);
        NSString * uid = self.uniqueID;
#endif
        STORAGE_DEVICE_DATA TPerData;
        if ( [self.registryAccess fillTPerData: &TPerData forTPer:self] )
        {
            TPerData.autoUnlock = self.autoUnlock;
            NSLOG_DEBUG(@"When creating base64Data for TPer %@, autoUnlock set to %@",
                        self.uniqueID, self.autoUnlock ? @"true" : @"false");
            NSData * unencoded = [ NSData dataWithBytesNoCopy:&TPerData length:sizeof(TPerData) freeWhenDone:NO];
            return [unencoded base64EncodedStringWithOptions:(NSDataBase64Encoding64CharacterLineLength)];
        }
#if DEBUG
        NSLOG_DEBUG(@"base64DataForTPer: could not fillTPerData for %@", uid);
#endif
        return nil;
        
    }
}

-(NSString *)MSID {
    @autoreleasepool {
#if DEBUG
        NSLOG_DEBUG(@"base64DataForTPer: trying to fill base64 data for %@", self.uniqueID);
        NSString * uid = self.uniqueID;
#endif
        NSMutableString * m = [NSMutableString new];
        int ret = [self.interface getMSID:m forTPer:self] ;
        if (0==ret) return m;
#if DEBUG
        NSLOG_DEBUG(@"MSID: could not get MSID for %@", uid);
#endif
        return nil;
    }
}

@end



@implementation StorageDevice (MaybeATPer)
- (bool) isTPer {
    return [ self isKindOfClass: [ TPer class] ] ;
}
@end
