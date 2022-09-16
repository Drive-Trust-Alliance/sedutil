//
//  DaemonClient.mm
//  SED
//
//  A DaemonClient is attached to each Connection created by a network server.
//  It interprets commands and produces responses.
//
//  Created by Scott Marks on 07/05/2017.
//
//

#import <SEDClientServerCommunication/SEDClientStatusCodes.h>
#import "DaemonClient.h"
#import "DaemonClientController.h"
#import "debug.h"

#pragma mark private StorageDevice extension for UI display sorting
@interface StorageDevice(Comparison)
- (NSComparisonResult) compare:(StorageDevice *)other;
@end

@implementation StorageDevice(Comparison)
- (NSComparisonResult) compare:(StorageDevice *)other {
    // sort by unitNumber.
    // TODO:   Jackie suggests sorting by user nickname defaulting to unique name
    if (self.unitNumber < other.unitNumber) return NSOrderedAscending;
    if (self.unitNumber > other.unitNumber) return NSOrderedDescending;
    return NSOrderedSame; // ?? TODO: assert?
}
@end



#pragma mark DaemonClient private interface
@interface DaemonClient()<ConnectionDelegate>
@property (nonatomic,weak) DaemonClientController * controller;
@property (nonatomic, strong) Connection * connection;
@end

#pragma mark -
#pragma mark DaemonClient implementation
@implementation DaemonClient

#pragma mark -
#pragma mark DaemonClient protocol methods

@synthesize controller;
@synthesize connection;

+(instancetype) clientWithInputStream: (NSInputStream *) i outputStream: (NSOutputStream *) o
                           netService: (NSNetService *)n
                         loopbackPort: (NSUInteger) loopbackPort
                           isLoopback: (bool) isLoopback
                           controller: (DaemonClientController *)controller
{
    return [[self alloc] initWithInputStream: i outputStream: o netService:n
                                  loopbackPort: loopbackPort isLoopback: isLoopback 
                                  controller: controller];
}

-(DaemonClient *) initWithInputStream: (NSInputStream *) i outputStream: (NSOutputStream *) o
                           netService: (NSNetService *) n
                         loopbackPort: (NSUInteger) loopbackPort
                           isLoopback: (bool) isLoopback
                           controller: (DaemonClientController *)c
{
    if ((self = [super init])) {
        self.connection = [Connection connectionWithInputStream:i outputStream:o netService:n delegate:self
                                                     isLoopback:isLoopback];
        self.controller = c;
        [self.connection openWithServerIdentity: c.serverIdentity
                                       keychain: c.keychain
                                   passwordName: c.keychainPasswordItemName
                                   loopbackPort: loopbackPort ];
    }
    return self;
}

-( NSDictionary <NSString *, StorageDevice *> *) storageDevices { return self.controller.storageDevices; }
-( NSDictionary <NSString *, TPer *> *) tPers { return self.controller.tPers; }


#pragma mark -
#pragma mark connection methods

#pragma mark output to connection
- (void) appendOutputString:(NSString *)string {
    NSData * data = [string dataUsingEncoding:NSUTF8StringEncoding];
    [self.connection outputData:data];
}

- (void) outputCode:(SEDClientStatusCode)code {
    [self appendOutputString:[NSString stringWithFormat: @"%u ", code ]];
}

- (void) CRLFAfterString:(NSString *)string afterCode:(SEDClientStatusCode)code {
    [self outputCode:code];
    [self appendOutputString:string];
    [self.connection outputCRLF];
}

- (bool) sendCRLFAfterString:(NSString *)string afterCode:(SEDClientStatusCode)code {
    [self CRLFAfterString:string afterCode:code];
    return [self.connection endOutput];
}

- (void) CRLFAfterString:(NSString *)string {
    [self appendOutputString:string];
    [self.connection outputCRLF];
}

- (bool) sendCRLFAfterData:(NSData *)data afterCode:(SEDClientStatusCode)code {
    [self outputCode:code];
    [self.connection outputData:data];
    [self.connection outputCRLF];
    return [self.connection endOutput];
}

- (bool) sendCRLFAfterBase64EncodedData:(NSData *)data afterCode:(SEDClientStatusCode)code {
    [self sendCRLFAfterString:[data base64EncodedStringWithOptions:0] afterCode:code];
    return [self.connection endOutput];
}

-(void) beginOutput {[self.connection beginOutput];}

-(void) close { [self.connection close]; }

#pragma mark input from connection

-(NSString *)scanWord:(NSUInteger *) pOffset { return [self.connection scanWord:pOffset]; }

-(NSString *)scanUpToCRLF:(NSUInteger *) pOffset { return [self.connection scanUpToCRLF:pOffset]; }

-(void) ignoreThroughCRLF:(NSUInteger *)pOffset { [self.connection ignoreThroughCRLF:pOffset]; }  // TODO: do these really need to be on separate lines?

-(NSData *)extractDataAtOffset:(NSUInteger *)pOffset { return [self.connection extractDataAtOffset:pOffset]; }

-(NSData *)extractPasswordData:(NSUInteger *) pOffset {
    [self ignoreThroughCRLF:pOffset];
    NSString * base64EncodedPassword=[self scanUpToCRLF:pOffset];
    NSData * data=[[NSData alloc] initWithBase64EncodedString:base64EncodedPassword options:0];
    return data;
}

-(NSData *)scanHashedPassword:(NSUInteger *) pOffset {
    NSData * data=[self extractPasswordData: pOffset];
    return data;
}

-(NSString *)scanEncodedPassword:(NSUInteger *) pOffset {
    NSData * data=[self scanHashedPassword: pOffset];
    NSString * password=[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
    return password;
}

#pragma mark -
#pragma mark Client output utilities

- (bool) sendOKWithCode: (SEDClientStatusCode)code
{
    return [self sendCRLFAfterString:@"ok" afterCode:code];
}

- (bool) sendOK: (StorageDevice *)sd code: (SEDClientStatusCode)code
{
    return [self sendCRLFAfterString:[NSString stringWithFormat:@"%@ -- ok", sd.uniqueID] afterCode:code];
}

- (bool) failMsg:(NSString *)msg code:(SEDClientStatusCode)code
{
    [self.controller performSelector:@selector(endDevicesChanging) withObject:nil afterDelay:0.25];
    return [self sendCRLFAfterString:msg afterCode:code];
}

- (bool) failMsg:(NSString *)msg sd:(StorageDevice *)sd code:(SEDClientStatusCode)code
{
    return [self failMsg:[NSString stringWithFormat: @"%@ -- %@", sd.uniqueID, msg] code:code];
}

static inline NSString * tagged(NSString * header, NSString * tag) {
    return tag ? [NSString stringWithFormat: @"%@ -- %@", header, tag] : header;
}

-(NSArray<NSString *> *) sortedStorageDeviceNames {
    return [self.storageDevices keysSortedByValueUsingSelector:@selector(compare:)];
}
-(NSArray<NSString *> *) sortedTPerNames {
    return [self.tPers keysSortedByValueUsingSelector:@selector(compare:)];
}

// Note: stringWithFormat:, initWithFormat:, etc., ignore width specifications on %@ args.  Groan.

- (NSString *) descriptionOfTPer:(TPer *)t {
    return
    [NSString stringWithFormat:@"%-40s %-25s %-15s %-13s %-8s",  // TODO: uniqueID smaller when WWN avail for non-TPer
     t.uniqueID.trimmed.UTF8String,
     t.modelNumber.trimmed.UTF8String,
     t.serialNumber.trimmed.UTF8String,
     t.SSCName.trimmed.UTF8String,
     !t.isSetUp ? "Not set up" : t.isLocked ? "Locked" : "Unlocked"];
}

- (NSString *) descriptionOfStorageDevice:(StorageDevice *)sd {
    return
    [NSString stringWithFormat:@"%-40s %-25s %-15s -- not a TPer",  // TODO: uniqueID smaller when WWN avail for non-TPer
     sd.uniqueID.trimmed.UTF8String,
     sd.modelNumber.trimmed.UTF8String,
     sd.serialNumber.trimmed.UTF8String];
}

- (NSString *) JSONForTPer:(TPer *)t {
    return
    [NSString stringWithFormat:@"  {\"uniqueID\":\"%@\", \"modelNumber\":\"%@\", \"serialNumber\":\"%@\", \"SSC\":\"%@\", \"state\":\"%@\"}",
     t.uniqueID,
     t.modelNumber,
     t.serialNumber,
     t.SSCName,
     !t.isSetUp ? @"Not set up" : t.isLocked ? @"Locked" : @"Unlocked"];
}

- (NSString *) JSONForStorageDevice:(StorageDevice *)sd {
    return
    [NSString stringWithFormat:@"  {\"uniqueID\":\"%@\", \"modelNumber\":\"%@\", \"serialNumber\":\"%@\", \"SSC\":\"none\"}",
     sd.uniqueID,
     sd.modelNumber,
     sd.serialNumber];
}

- (void)storageDevice:(StorageDevice *)sd descriptionWithCode:(SEDClientStatusCode)code tag:(NSString * )tag
{
    NSString * header = tagged(sd.uniqueID, tag);
    [self CRLFAfterString:header afterCode:code];
    NSString * description;
    if ( sd.isTPer ) {
        TPer * t = (TPer *)sd;
        description = [self descriptionOfTPer: t];
    } else {
        description = [self descriptionOfStorageDevice:sd];
    }
    [self CRLFAfterString:description];
}

- (void)storageDeviceDescriptionsWithCode:(SEDClientStatusCode)code tag:(NSString *)tag
{
    NSString * header = tagged([NSString stringWithFormat:@"%3lu storage devices", self.storageDevices.count], tag);
    [self CRLFAfterString:header afterCode:code];
    for (NSString * uniqueID in self.sortedStorageDeviceNames) {
        StorageDevice * sd =self.storageDevices[uniqueID];
        NSString * description;
        if ( sd.isTPer ) {
            TPer * t = (TPer *)sd;
            description = [self descriptionOfTPer: t];
        } else {
            description = [self descriptionOfStorageDevice:sd];
        }
        [self CRLFAfterString:description];
    }
}


- (void)tPerDescriptionsWithCode:(SEDClientStatusCode)code tag:(NSString *)tag
{
    NSString * header = tagged([NSString stringWithFormat:@"%3lu TPers", self.tPers.count], tag);
    [self CRLFAfterString:header afterCode:code];
    for (NSString * uniqueID in self.sortedTPerNames) {
        TPer * t =self.tPers[uniqueID];
        NSString * description = [self descriptionOfTPer: t];
        [self CRLFAfterString:description];
    }
}



- (void)storageDeviceDescriptionsInJSONWithCode:(SEDClientStatusCode)code tag:(NSString *)tag
{
    NSString * header = tagged([NSString stringWithFormat:@"%3lu storage devices", self.storageDevices.count], tag);
    [self CRLFAfterString:header afterCode:code];
    NSUInteger devicesRemaining=self.storageDevices.count;
    [self CRLFAfterString:@"["];
    for (NSString * uniqueID in self.sortedStorageDeviceNames) {
        StorageDevice * sd =self.storageDevices[uniqueID];
        NSString * json;
        if ( sd.isTPer ) {
            TPer * t = (TPer *)sd;
            json = [self JSONForTPer:t];
        } else {
            json = [self JSONForStorageDevice:sd];
        }
        if (0<--devicesRemaining) {
            json = [json stringByAppendingString:@","];
        }
        [self CRLFAfterString:json];
    }
    [self CRLFAfterString:@"]"];
}



- (void)tPerDescriptionsInJSONWithCode:(SEDClientStatusCode)code tag:(NSString *)tag
{
    NSString * header = tagged([NSString stringWithFormat:@"%3lu TPers", self.tPers.count], tag);
    [self CRLFAfterString:header afterCode:code];
    NSUInteger tPersRemaining=self.tPers.count;
    [self CRLFAfterString:@"["];
    for (NSString * uniqueID in self.sortedTPerNames) {
        TPer * t =self.tPers[uniqueID];
        NSString * json = [self JSONForTPer:t];
        if (0<--tPersRemaining) {
            json = [json stringByAppendingString:@","];
        }
        [self CRLFAfterString:json];
    }
    [self CRLFAfterString:@"]"];
}

- (void)tPersDataWithCode:(SEDClientStatusCode)code tag:(NSString *)tag
{
    // A race condition may result in t.base64Data returning nil,
    // so we accumulate all the non-nil results and return only those.
    // TODO -- really?!  If so, then doesn't the same consideration hold for storage device descriptions above?
    NSMutableArray<NSString *> * descriptions=[NSMutableArray new];
    for (TPer * t in [self.tPers.allValues sortedArrayUsingSelector:@selector(compare:)]) {
        NSString * base64Data = t.base64Data;
        if (nil != base64Data ) {
            NSString * description =
            [NSString stringWithFormat:@"%@\n%@", t.uniqueID, base64Data];
            [descriptions addObject:description];
        }
    }
    NSString * header = tagged([NSString stringWithFormat:@"%3lu TPers", descriptions.count], tag);
    [self CRLFAfterString:header afterCode:code];
    for (NSString * description in descriptions) {
        [self CRLFAfterString:description];
    }
}

- (void)storageDevicesDataWithCode:(SEDClientStatusCode)code tag:(NSString *)tag
{
    // A race condition may result in t.base64Data returning nil,
    // so we accumulate all the non-nil results and return only those.
    // TODO -- really?!  If so, then doesn't the same consideration hold for storage device descriptions above?
    NSMutableArray<NSString *> * descriptions=[NSMutableArray new];
    for (StorageDevice * sd in [self.storageDevices.allValues sortedArrayUsingSelector:@selector(compare:)]) {
        NSString * base64Data = sd.base64Data;
        if (nil != base64Data ) {
            NSString * description =
            [NSString stringWithFormat:@"%@\n%@", sd.uniqueID, base64Data];
            [descriptions addObject:description];
        }
    }
    NSString * header = tagged([NSString stringWithFormat:@"%3lu StorageDevices", descriptions.count], tag);
    [self CRLFAfterString:header afterCode:code];
    for (NSString * description in descriptions) {
        [self CRLFAfterString:description];
    }
}

- (void)tPer:(TPer *)t dataWithCode:(SEDClientStatusCode)code tag:(NSString *)tag
{
    NSString * base64Data = t.base64Data;
    if (nil != base64Data ) {
        NSString * header = tagged(t.uniqueID,tag);
        [self CRLFAfterString:header afterCode:code];
        [self CRLFAfterString:base64Data];
    }
}




- (bool) sendTPersDataWithCode:(SEDClientStatusCode) code tag:(NSString *)tag {
    [self beginOutput];
    [self tPersDataWithCode:code tag:tag];
    return [self sendOKWithCode:code];
}

- (bool) sendStorageDevicesDataWithCode:(SEDClientStatusCode) code tag:(NSString *)tag {
    [self beginOutput];
    [self storageDevicesDataWithCode:code tag:tag];
    return [self sendOKWithCode:code];
}

- (bool) sendTPer:(TPer *)t dataWithCode:(SEDClientStatusCode) code tag:(NSString *)tag {
    [self beginOutput];
    [self tPer:t dataWithCode:code tag:tag];
    return [self sendOKWithCode:code];
}


- (void)tPer:(TPer *)t MSIDWithCode:(SEDClientStatusCode)code tag:(NSString *)tag
{
    NSString * MSID = t.MSID;
    if (nil != MSID ) {
        NSString * header = tagged([NSString stringWithFormat:@"%@ MSID", t.uniqueID],tag);
        [self CRLFAfterString:header afterCode:code];
        [self CRLFAfterString:MSID];
    }
}

- (bool) sendTPer:(TPer *)t MSIDWithCode:(SEDClientStatusCode) code tag:(NSString *)tag {
    [self beginOutput];
    [self tPer:t MSIDWithCode:code tag:tag];
    return [self sendOKWithCode:code];
}

#pragma mark -
#pragma mark Client input utilities

- (StorageDevice *) extractStorageDeviceNamedAtOffset:(NSUInteger *)pOffset {
    NSString * word = [self scanWord:pOffset];
    if (word) {
        StorageDevice * sd = [self.storageDevices objectForKey:word]; // nil if not found
        if (sd) return sd;
    } else {
        word=@"Missing device name";
    }
    [self failMsg:[NSString stringWithFormat: @"%@?", word] code:SEDClientUnknownDevice];
    return nil;
}


- (TPer *) extractTPerNamedAtOffset:(NSUInteger *)pOffset {
    StorageDevice * sd = [self extractStorageDeviceNamedAtOffset:pOffset];
    if (sd) {
        if (sd.isTPer) return (TPer *)sd;
        [self failMsg:@"not a TPer" sd:sd code:SEDClientNotATPer];;
    }
    return nil;
}


- (NSString *) extractUserNameAtOffset:(NSUInteger *)pOffset {
    static NSDictionary<NSString *,NSString *> *canonicalName = ^{
        static NSArray<NSArray<NSString *>*>* equivalentNames=  // First one in each row is the canonical name
        @[@[@"Admin1", @"User0", @"0"],
          @[@"User1", @"1"],
          @[@"User2", @"2"],
          @[@"User3", @"3"],
          @[@"User4", @"4"],
          @[@"User5", @"5"],
          @[@"User6", @"6"],
          @[@"User7", @"7"],
          @[@"User8", @"8"]];
        NSMutableDictionary * e = [NSMutableDictionary new];
        for (NSArray<NSString *>* row in equivalentNames) {
            for (NSString * name in row) {
                e[name]=row[0];
            }
        }
        return [NSDictionary dictionaryWithDictionary:e];
    }();
    NSString * word = [self scanWord:pOffset];
    if (!word) {
        [self failMsg:@"Missing username" code:SEDClientBadUserName];
        return nil;
    }
    NSString * userName = [canonicalName objectForKey:word.capitalizedString];
    if (!userName) {
        [self failMsg:[NSString stringWithFormat:@"bad username %@?",word] code:SEDClientBadUserName];
    }
    return userName;
}

#pragma mark -
#pragma mark Command implementations


#pragma mark ECHO

- (bool)echo: (NSUInteger)offset
{
    NSData * data=[self extractDataAtOffset:&offset];
    [self beginOutput];
    return [self sendCRLFAfterData:data afterCode:SEDClientEcho];
}

// Create info table from SEDCommands.inc
#define __defcmd_header__ static NSDictionary * info= @{
#define __defcmd__(c,_,__,i,...)@#c:@i,
#define __defcmd_trailer__ };

#include <SEDClientServerCommunication/SEDCommands.inc>

#undef __defcmd_header__
#undef __defcmd__
#undef __defcmd_trailer__


#pragma mark HELP

- (bool)help: (NSUInteger __unused)ignored
{
    [self beginOutput];
    [self CRLFAfterString:@"commands:" afterCode:SEDClientHelp];
    for (NSString * command in [info.allKeys sortedArrayUsingSelector:@selector(compare:)]) {
        [self CRLFAfterString:command];
    };
    return [self sendOKWithCode:SEDClientHelp];
}


#pragma mark INFO

- (bool)info: (NSUInteger __unused)ignored
{
    [self beginOutput];
    [self CRLFAfterString:@"sedd interface commands:" afterCode:SEDClientInfo];
    [self appendOutputString:@CRLF@CRLF];
    for (NSString * command in [info.allKeys sortedArrayUsingSelector:@selector(compare:)]) {
        [self appendOutputString:command];
        [self appendOutputString:@" "];
        [self CRLFAfterString:info[command]];
        [self appendOutputString:@CRLF];
    };
    return [self sendOKWithCode:SEDClientInfo];
}


#pragma mark LIST

- (bool)listStorageDevices: (NSUInteger __unused)offset
{
    [self beginOutput];
    [self CRLFAfterString:[NSString stringWithFormat:@"%lu storage devices", self.storageDevices.count]
                afterCode:SEDClientListDevices];
    for (NSString * uniqueID in self.sortedStorageDeviceNames) {
        [self CRLFAfterString:uniqueID];
    }
    return [self sendOKWithCode:SEDClientListDevices];
}


#pragma mark TLST

- (bool)listTPers: (NSUInteger __unused)offset
{
    [self beginOutput];
    [self CRLFAfterString:[NSString stringWithFormat:@"%lu TPers", self.tPers.count]
                afterCode:SEDClientListTPers];
    for (NSString * uniqueID in self.sortedTPerNames) {
        [self CRLFAfterString:uniqueID];
    }
    return [self sendOKWithCode:SEDClientListTPers];
}


#pragma mark SHOW

- (bool)showStorageDevices:(NSUInteger __unused)offset
{
    [self beginOutput];
    [self storageDeviceDescriptionsWithCode:SEDClientShowDevices tag:nil];
    return [self sendOKWithCode:SEDClientShowDevices];
}


#pragma mark SHSD

- (bool)showStorageDevice:(NSUInteger __unused)offset
{
    [self beginOutput];
    StorageDevice * sd=[self extractStorageDeviceNamedAtOffset:&offset];
    if (sd) {
        [self storageDevice:sd descriptionWithCode:SEDClientShowDevice tag:nil];
        return [self sendOKWithCode:SEDClientShowDevice];
    }
    return true;
}


#pragma mark TSHW

- (bool)showTPers:(NSUInteger __unused)offset
{
    [self beginOutput];
    [self tPerDescriptionsWithCode:SEDClientShowTPers tag:nil];
    return [self sendOKWithCode:SEDClientShowTPers];
}


#pragma mark JSON

- (bool)showStorageDevicesAsJSON:(NSUInteger __unused)offset
{
    [self beginOutput];
    [self storageDeviceDescriptionsInJSONWithCode:SEDClientShowDevicesAsJSON tag:nil];
    return [self sendOKWithCode:SEDClientShowDevicesAsJSON];
}


#pragma mark TJSN

- (bool)showTPersAsJSON:(NSUInteger __unused)offset
{
    [self beginOutput];
    [self tPerDescriptionsInJSONWithCode:SEDClientShowTPersAsJSON tag:nil];
    return [self sendOKWithCode:SEDClientShowTPersAsJSON];
}


#pragma mark TDAT

- (bool) getTPersData:(NSUInteger __unused)offset
{
    return [self sendTPersDataWithCode:SEDClientGetTPers tag:nil];
}


#pragma mark STDV

- (bool) getStorageDevicesData:(NSUInteger __unused)offset
{
    return [self sendStorageDevicesDataWithCode:SEDClientGetStorageDevices tag:nil];
}


#pragma mark TDAT

- (bool) getTPerData: (NSUInteger)offset
{
    TPer * t=[self extractTPerNamedAtOffset:&offset];
    return [self sendTPer:t dataWithCode:SEDClientGetTPer tag:nil];
}


#pragma mark MSID

- (bool) getMSID: (NSUInteger)offset
{
    TPer * t=[self extractTPerNamedAtOffset:&offset];
    return [self sendTPer:t MSIDWithCode:SEDClientGetMSID tag:nil];
}


#pragma mark DXCN

- (bool) disconnect: (NSUInteger __unused) ignored
{
    [self beginOutput];
    [self close];
    return true;
}

- (NSNumber *) extractFlagAtOffset:(NSUInteger *)pOffset {
    static NSDictionary<NSString *,NSString *> *canonicalName = ^{
        static NSArray<NSArray<NSString *>*>* equivalentNames=  // First one in each row is the canonical name
        @[@[@"1", @"YES", @"TRUE" ],
          @[@"0", @"NO" , @"FALSE"]];
        NSMutableDictionary * e = [NSMutableDictionary new];
        for (NSArray<NSString *>* row in equivalentNames) {
            for (NSString * name in row) {
                e[name]=row[0];
            }
        }
        return [NSDictionary dictionaryWithDictionary:e];
    }();
    NSString * word = [self scanWord:pOffset];
    if (!word) {
        [self failMsg:@"Missing setting" code:SEDClientBadSetting];
        return nil;
    }
    NSString * flag = [canonicalName objectForKey:word.uppercaseString];
    if (!flag) {
        [self failMsg:[NSString stringWithFormat:@"bad setting %@?",word] code:SEDClientBadSetting];
        return nil;
    }
    return @([flag isEqualToString:@"1"]);
}


#pragma mark RHCH

- (bool) removeHostChallenge:(NSUInteger)offset
{
    TPer * t=[self extractTPerNamedAtOffset:&offset];
    t.rememberedAdminHostChallenge = nil;
    return [self sendOKWithCode:SEDClientRemoveHostChallenge];
}


#pragma mark SHCH

- (bool) setHostChallenge:(NSUInteger)offset
{
    TPer * t=[self extractTPerNamedAtOffset:&offset];
    NSData * hostChallenge = [self extractPasswordData:&offset];
    t.rememberedAdminHostChallenge = 0 < hostChallenge.length ? hostChallenge : nil;
    return [self sendOKWithCode:SEDClientSetHostChallenge];
}


#pragma mark GHCH

- (bool) hostChallenge:(NSUInteger)offset
{
    TPer * t=[self extractTPerNamedAtOffset:&offset];
    NSData * hostChallenge = t.rememberedAdminHostChallenge;
    return [self sendCRLFAfterBase64EncodedData:hostChallenge
                                      afterCode:SEDClientGetHostChallenge];
}


#pragma mark SAUN

- (bool) setAutomaticUnlock:(NSUInteger)offset
{
    TPer * t=[self extractTPerNamedAtOffset:&offset];
    NSNumber * automaticUnlock = [self extractFlagAtOffset:&offset];
    if (automaticUnlock==nil) {
        return true;
    }
    t.autoUnlock = automaticUnlock;
    return [self sendOKWithCode:SEDClientSetAutomaticUnlock];
}


#pragma mark GAUN

- (bool) automaticUnlock:(NSUInteger)offset
{
    TPer * t=[self extractTPerNamedAtOffset:&offset];
    return [self sendCRLFAfterString:(t.autoUnlock ? @"YES" : @"NO")
                           afterCode:SEDClientGetAutomaticUnlock];
}


#pragma mark locking result continuation

- (ResultContinuation) createSendLockingResultContinuationTPer: (TPer *) t
                                                   successCode:(SEDClientStatusCode) successCode
                                             wrongPasswordCode:(SEDClientStatusCode) wrongPasswordCode
                                          lockedOutCode:(SEDClientStatusCode) lockedOutCode
{
    ResultContinuation cont = ^void(TCGSTATUSCODE result) {
        bool resultSentOK = false;
        
        switch (result) {            // TODO: non-success (or all) should send out result, too.
            case SUCCESS:
                resultSentOK = [self sendOK:t code:successCode];
                break;
            case NOT_AUTHORIZED:
                resultSentOK = [self failMsg:@"incorrect password" sd:t code:wrongPasswordCode];
                break;
            case AUTHORITY_LOCKED_OUT:
                resultSentOK = [self failMsg:@"locked out!" sd:t code:lockedOutCode];
                break;
            case INVALID_PARAMETER:
                resultSentOK = [self failMsg:@"invalid parameter?" sd:t code:wrongPasswordCode];
                break;
                
            case SP_BUSY:
            case SP_FAILED:
            case SP_DISABLED:
            case SP_FROZEN:
            case NO_SESSIONS_AVAILABLE:
            case UNIQUENESS_CONFLICT:
            case INSUFFICIENT_SPACE:
            case INSUFFICIENT_ROWS:
            case INVALID_FUNCTION:
            case INVALID_REFERENCE:
            case TPER_MALFUNCTION:
            case TRANSACTION_FAILURE:
            case RESPONSE_OVERFLOW:
            case FAIL:
                resultSentOK = [self failMsg:[NSString stringWithFormat:@"unexpected error 0x%04X",result ]
                                            sd:t code:SEDClientUnexpectedError];
                break;
                
            default:
                break;
        }
        
        if (! resultSentOK) {
            [self disconnect:0];            /////// TODO !!!!!!!!!!!!!!!!! check for bluetooth  !!!!!!!!!
        }
    } ;
    
    return cont;
}


#pragma mark drive state changing utilities

- (bool) requireSetUpTPer:(TPer *)t operation:(bool(^)(void))operation
{
    [self beginOutput];
    if (!t) return true;
    if (!t.isSetUp) return [self failMsg:@"not set up" code:SEDClientNotSetUp];
    return operation();
}

- (bool) requireNotSetUpTPer:(TPer *)t operation:(bool(^)(void))operation
{
    [self beginOutput];
    if (!t) return true;
    if (t.isSetUp) return [self failMsg:@"already set up" code:SEDClientSetupAlreadySetUp];
    return operation();
}


typedef NSString * (^Scanner)(NSUInteger *);

- (Scanner) scanWord { return ^(NSUInteger * pOffset){return [self scanWord:pOffset];};}

- (Scanner) scanEncodedPassword { return ^(NSUInteger * pOffset){return [self scanEncodedPassword:pOffset];};
}

#pragma mark LOCK utilities

- (bool) requireUnlockedTPer:(TPer *)t operation:(bool(^)(ResultContinuation lockContinuation))operation
{
    ResultContinuation lockContinuation =
        [self createSendLockingResultContinuationTPer:t
                                          successCode:SEDClientLockSuccess
                             wrongPasswordCode:SEDClientLockWrongPassword
                                 lockedOutCode:SEDClientLockLockedOut];
    
    return [self requireSetUpTPer:t operation:^{
        if (t.isLocked) return [self failMsg:@"already locked" sd:t code:SEDClientLockAlreadyLocked];
        return operation(lockContinuation);
    }];
}


- (bool) lockTPer:(TPer *)t password:(NSString *)password
{
    return [self requireUnlockedTPer:t operation:^(ResultContinuation lockContinuation){
        if (!password)
            return [self failMsg:@"password required" sd:t code:SEDClientLockPasswordRequired];
        // only returns nonzero if continuation is not executed
        return [t lockWithPassword:password continuation:lockContinuation];
    }];
}


- (bool) lockTPer:(TPer *)t hostChallenge:(NSData *)hostChallenge
{
    return [self requireUnlockedTPer:t operation:^(ResultContinuation lockContinuation){
        if (!hostChallenge)
            return [self failMsg:@"host challenge required" sd:t code:SEDClientLockPasswordRequired];
        
        // only returns nonzero if continuation is not executed
        return [t lockWithHostChallenge:hostChallenge continuation:lockContinuation];
    }];
}



- (bool) lockWithPassword:(NSUInteger)offset
                  scanner:(NSString * (^)(NSUInteger *))scanner
{
    TPer * t=[self extractTPerNamedAtOffset:&offset];
    NSString * password=scanner(&offset);
    return [self lockTPer:t password:password];
}

#pragma mark LOCK

- (bool) lockTPer: (NSUInteger)offset
{
    return [self lockWithPassword:offset scanner:self.scanWord];
}


#pragma mark XLCK

- (bool) lockTPerEncoded: (NSUInteger)offset
{
    return [self lockWithPassword:offset scanner:self.scanEncodedPassword];
}


#pragma mark ZLCK

- (bool) lockTPerHashed: (NSUInteger)offset
{
    TPer * t=[self extractTPerNamedAtOffset:&offset];
    NSData * hostChallenge=[self extractPasswordData:&offset];
    return [self lockTPer:t hostChallenge:hostChallenge];
}


#pragma mark UNLK utilities

- (bool) requireLockedTPer:(TPer *)t operation:(bool(^)(ResultContinuation unlockContinuation))operation
{
    ResultContinuation unlockContinuation =
        [self createSendLockingResultContinuationTPer:t
                                          successCode:SEDClientUnlockSuccess
                             wrongPasswordCode:SEDClientUnlockWrongPassword
                                 lockedOutCode:SEDClientUnlockLockedOut];
    
    return [self requireSetUpTPer:t operation:^{
        if (!t.isLocked) return [self failMsg:@"already unlocked" sd:t code:SEDClientUnlockAlreadyUnlocked];
        return operation(unlockContinuation);
    }];
}

- (bool) unlockTPer:(TPer *)t password:(NSString *)password autoUnlock:(BOOL)autoUnlock
{
    return [self requireLockedTPer:t operation:^(ResultContinuation unlockContinuation){
        if (!password)
            return [self failMsg:@"password required" sd:t code:SEDClientUnlockPasswordRequired];

        return [t unlockWithPassword: password
                 autoUnlock:autoUnlock
                        continuation: unlockContinuation];
    }];
}


- (bool) unlockTPer:(TPer *)t hostChallenge:(NSData *)hostChallenge autoUnlock:(BOOL)autoUnlock
{
    return [self requireLockedTPer:t operation:^(ResultContinuation unlockContinuation){
        if (!hostChallenge)
            return [self failMsg:@"hostChallenge required" sd:t code:SEDClientUnlockPasswordRequired];
        return [t unlockWithHostChallenge:hostChallenge
                      autoUnlock:autoUnlock
                             continuation:unlockContinuation];
    }];
}


- (bool) unlockWithPassword:(NSUInteger)offset
                    scanner:(NSString * (^)(NSUInteger *))scanner
{
    TPer * t=[self extractTPerNamedAtOffset:&offset];
    NSString * password=scanner(&offset);
    NSNumber * autoUnlock = [self extractFlagAtOffset:&offset];
    if (autoUnlock==nil) {
        return [self failMsg:@"autoUnlock flag required" sd:t code:SEDClientUnlockAutomaticallyRequired];
    }
    return [self unlockTPer:t password:password autoUnlock:autoUnlock.boolValue];
}

#pragma mark UNLK

- (bool) unlockTPer: (NSUInteger)offset
{
    return [self unlockWithPassword:offset scanner:self.scanWord];
}


#pragma mark XUNL

- (bool) unlockTPerEncoded:(NSUInteger)offset
{
    return [self unlockWithPassword:offset scanner:self.scanEncodedPassword];
}


#pragma mark ZUNL

- (bool) unlockTPerHashed:(NSUInteger)offset
{
    TPer * t=[self extractTPerNamedAtOffset:&offset];
    NSNumber * autoUnlock = [self extractFlagAtOffset:&offset];
    if (nil == autoUnlock) {
        return [self failMsg:@"autoUnlock flag required" sd:t code:SEDClientUnlockAutomaticallyRequired];
    }
    NSData * hostChallenge = [self extractPasswordData:&offset];
    return [self unlockTPer:t hostChallenge:hostChallenge autoUnlock:autoUnlock.boolValue];
}


#pragma mark Setup utilities

- (bool) setupTPer:(TPer *)t password:(NSString *)password
{
    return [self requireNotSetUpTPer:t operation:^{
        return ([t setupWithPassword:password])
                    ? [self sendOK:t code:SEDClientSetupSuccess]
                : password
                    ? [self failMsg:@"drive not set up -- erase it first" sd:t code:SEDClientSetupNotErased]
                    : [self failMsg:@"new password required" sd:t code:SEDClientSetupPasswordRequired];}];
}

- (bool) setupTPer:(TPer *)t hostChallenge:(NSData *)hostChallenge
{
    return [self requireNotSetUpTPer:t operation:^{
        return ([t setupWithHostChallenge:hostChallenge])
                    ? [self sendOK:t code:SEDClientSetupSuccess]
                : hostChallenge
                    ? [self failMsg:@"drive not set up -- erase it first?" sd:t code:SEDClientSetupNotErased]
                    : [self failMsg:@"new hostChallenge required" sd:t code:SEDClientSetupPasswordRequired];}];
}

- (bool) setupWithPassword:(NSUInteger)offset
                   scanner:(NSString * (^)(NSUInteger *))scanner
{
    TPer * t=[self extractTPerNamedAtOffset:&offset];
    NSString * password=scanner(&offset);
    return [self setupTPer:t password:password];
}

#if MULTISTART
- (bool) multiUserSetupTPer:(TPer *)t password:(NSString *)password
{
    return [self requireNotSetUpTPer:t operation:^{
        return ([t multiUserSetupWithPassword:password])
        ? [self sendOK:t code:SEDClientSetupSuccess]
        : password
        ? [self failMsg:@"drive not set up -- erase it first" sd:t code:SEDClientSetupNotErased]
        : [self failMsg:@"new password required" sd:t code:SEDClientSetupPasswordRequired];}];
}

- (bool) multiUserSetupTPer:(TPer *)t hostChallenge:(NSData *)hostChallenge
{
    return [self requireNotSetUpTPer:t operation:^{
        return ([t multiUserSetupWithHostChallenge:hostChallenge])
        ? [self sendOK:t code:SEDClientSetupSuccess]
        : hostChallenge
        ? [self failMsg:@"drive not set up -- erase it first?" sd:t code:SEDClientSetupNotErased]
        : [self failMsg:@"new hostChallenge required" sd:t code:SEDClientSetupPasswordRequired];}];
}

- (bool) multiUserSetupWithPassword:(NSUInteger)offset
                   scanner:(NSString * (^)(NSUInteger *))scanner
{
    TPer * t=[self extractTPerNamedAtOffset:&offset];
    NSString * password=scanner(&offset);
    return [self multiUserSetupTPer:t password:password];
}
#endif //MULTISTART


#pragma mark STUP

- (bool) setup:(NSUInteger)offset
{
    return [self setupWithPassword:offset scanner:self.scanWord];
}


#pragma mark XSTP

- (bool) setupEncoded:(NSUInteger)offset
{
    return [self setupWithPassword:offset scanner:self.scanEncodedPassword];
}


#pragma mark ZSTP

- (bool) setupHashed:(NSUInteger)offset
{
    TPer * t=[self extractTPerNamedAtOffset:&offset];
    NSData * hostChallenge = [self extractPasswordData:&offset];
    return [self setupTPer:t hostChallenge:hostChallenge];
}

#if MULTISTART
#pragma mark MUST

- (bool) multiUserSetup:(NSUInteger)offset
{
    return [self multiUserSetupWithPassword:offset scanner:self.scanWord];
}


#pragma mark XMUS

- (bool) multiUserSetupEncoded:(NSUInteger)offset
{
    return [self multiUserSetupWithPassword:offset scanner:self.scanEncodedPassword];
}


#pragma mark ZMUS

- (bool) multiUserSetupHashed:(NSUInteger)offset
{
    TPer * t=[self extractTPerNamedAtOffset:&offset];
    NSData * hostChallenge = [self extractPasswordData:&offset];
    return [self multiUserSetupTPer:t hostChallenge:hostChallenge];
}
#endif // MULTISTART


#pragma mark Erase utilities

- (bool) eraseTPer:(TPer *)t password:(NSString *)password
{
    return [self requireSetUpTPer:t operation:^{
        return ([t eraseWithPassword:password])
                    ? [self sendOK:t code:SEDClientEraseSuccess]
                : password
                    ? [self failMsg:@"incorrect password" sd:t code:SEDClientEraseWrongPassword]
                    : [self failMsg:@"password required" sd:t code:SEDClientErasePasswordRequired];}];
}

- (bool) eraseTPer:(TPer *)t hostChallenge:(NSData *)hostChallenge
{
    return [self requireSetUpTPer:t operation:^{
        return ([t eraseWithHostChallenge:hostChallenge])
                    ? [self sendOK:t code:SEDClientEraseSuccess]
                : hostChallenge
                    ? [self failMsg:@"incorrect hostChallenge" sd:t code:SEDClientEraseWrongPassword]
                    : [self failMsg:@"hostChallenge required" sd:t code:SEDClientErasePasswordRequired];}];
}

- (bool) eraseWithPassword:(NSUInteger)offset
                   scanner:(NSString * (^)(NSUInteger *))scanner
{
    TPer * t=[self extractTPerNamedAtOffset:&offset];
    NSString * password=scanner(&offset);
    return [self eraseTPer:t password:password];
}


#pragma mark ERAS

- (bool) erase:(NSUInteger)offset
{
    return [self eraseWithPassword:offset scanner:self.scanWord];
}


#pragma mark XERA

- (bool) eraseEncoded:(NSUInteger)offset
{
    return [self eraseWithPassword:offset scanner:self.scanEncodedPassword];
}


#pragma mark ZERA

- (bool) eraseHashed:(NSUInteger)offset
{
    TPer * t=[self extractTPerNamedAtOffset:&offset];
    NSData * hostChallenge = [self extractPasswordData:&offset];
    return [self eraseTPer:t hostChallenge:hostChallenge];
}


#pragma mark Revert utilities



#pragma mark locking result continuation

- (ResultContinuation) createSendPSIDRevertResultContinuationTPer:(TPer *) t
                                                      successCode:(SEDClientStatusCode) successCode
                                                wrongPasswordCode:(SEDClientStatusCode) wrongPasswordCode
{
    ResultContinuation cont = ^void(TCGSTATUSCODE result) {
        [self beginOutput];
        bool resultSentOK = false;
        
        switch (result) {            // TODO: non-success (or all) should send out result, too.
            case SUCCESS:
                resultSentOK = [self sendOK:t code:successCode];
                break;
            case NOT_AUTHORIZED:
                resultSentOK = [self failMsg:@"incorrect password" sd:t code:wrongPasswordCode];
                break;

            case INVALID_PARAMETER:
                resultSentOK = [self failMsg:@"invalid parameter?" sd:t code:wrongPasswordCode];
                break;
                
            case AUTHORITY_LOCKED_OUT:
            case SP_BUSY:
            case SP_FAILED:
            case SP_DISABLED:
            case SP_FROZEN:
            case NO_SESSIONS_AVAILABLE:
            case UNIQUENESS_CONFLICT:
            case INSUFFICIENT_SPACE:
            case INSUFFICIENT_ROWS:
            case INVALID_FUNCTION:
            case INVALID_REFERENCE:
            case TPER_MALFUNCTION:
            case TRANSACTION_FAILURE:
            case RESPONSE_OVERFLOW:
            case FAIL:
                resultSentOK = [self failMsg:[NSString stringWithFormat:@"unexpected error 0x%04X",result ]
                                          sd:t code:SEDClientUnexpectedError];
                break;
                
            default:
                break;
        }
        
        if (! resultSentOK) {
            [self disconnect:0];            /////// TODO !!!!!!!!!!!!!!!!! check for bluetooth  !!!!!!!!!
        }
    } ;
    
    return cont;
}

- (bool) revertTPer: (TPer *) t withPSID:(NSString *)PSID continuation:(ResultContinuation)continuation{
    if (!t) return true;
    return [t revertWithPSID:PSID continuation:continuation];
}


#pragma mark PSID

- (bool) PSIDRevert: (NSUInteger)offset
{
    TPer * t=[self extractTPerNamedAtOffset:&offset];
    NSString * password = [self scanWord:&offset];
    if (!password)
        return [self failMsg:@"password required" sd:t code:SEDClientPSIDRevertPasswordRequired];
    ResultContinuation revertContinuation =
        [self createSendPSIDRevertResultContinuationTPer:t
                                             successCode:SEDClientPSIDRevertSuccess
                                       wrongPasswordCode:SEDClientPSIDRevertWrongPassword];
    return [self revertTPer: t withPSID:password continuation:revertContinuation];
}



#pragma mark Unprotect utilities

- (bool) unprotectTPer:(TPer *)t password:(NSString *)password
{
    return [self requireSetUpTPer:t operation:^{
        return ([t unprotectWithPassword:password])
                    ? [self sendOK:t code:SEDClientUnprotectSuccess]
                : password
                    ? [self failMsg:@"drive not protected" sd:t code:SEDClientUnprotectFailure]
                    : [self failMsg:@"password required" sd:t code:SEDClientUnprotectFailure];}];
}

- (bool) unprotectTPer:(TPer *)t hostChallenge:(NSData *)hostChallenge
{
    return [self requireSetUpTPer:t operation:^{
        return ([t unprotectWithHostChallenge:hostChallenge])
                    ? [self sendOK:t code:SEDClientUnprotectSuccess]
                : hostChallenge
                    ? [self failMsg:@"drive not protected" sd:t code:SEDClientUnprotectFailure]
                    : [self failMsg:@"hostChallenge required" sd:t code:SEDClientUnprotectFailure];}];
}


#pragma mark UNPR

- (bool) unprotect:(NSUInteger)offset
{
    TPer * t=[self extractTPerNamedAtOffset:&offset];
    NSString * password=[self scanWord:&offset];
    return [self unprotectTPer:t password:password];
}


#pragma mark XUNP

- (bool) unprotectEncoded:(NSUInteger)offset
{
    TPer * t=[self extractTPerNamedAtOffset:&offset];
    NSString * password=[self scanEncodedPassword:&offset];
    return [self unprotectTPer:t password:password];
}


#pragma mark ZUNP

- (bool) unprotectHashed:(NSUInteger)offset
{
    TPer * t=[self extractTPerNamedAtOffset:&offset];
    NSData * hostChallenge = [self extractPasswordData:&offset];
    return [self unprotectTPer:t hostChallenge:hostChallenge];
}


#pragma mark Change password/host challenge utilities

- (bool) changePasswordForTPer:(TPer *)t oldPassword:(NSString *)oldPassword newPassword:(NSString *)newPassword
{
    return [self requireSetUpTPer:t operation:^{
        return ([t changePassword: oldPassword to: newPassword])
                    ? [self sendOK:t code: SEDClientChangeSuccess]
                : (oldPassword && newPassword)
                    ? [self failMsg:@"wrong old password" sd:t code:SEDClientChangeWrongOldPassword]
                    : [self failMsg:@"old and new passwords required" sd:t code:SEDClientChangePasswordsRequired];}];
}

- (bool) changeHostChallengeForTPer:(TPer *)t
                   oldHostChallenge:(NSData *)oldHostChallenge
                   newHostChallenge:(NSData *)newHostChallenge
{
    return [self requireSetUpTPer:t operation:^{
        return ([t changeHostChallenge:oldHostChallenge to:newHostChallenge])
                    ? [self sendOK:t code: SEDClientChangeSuccess]
               : (oldHostChallenge && newHostChallenge)
                    ? [self failMsg:@"wrong old host challenge" sd:t code:SEDClientChangeWrongOldPassword]
                    : [self failMsg:@"old and new host challenges required" sd:t code:SEDClientChangePasswordsRequired];}];
}

- (bool) changePassword:(NSUInteger)offset
                scanner:(NSString * (^)(NSUInteger *))scanner
{
    TPer * t=[self extractTPerNamedAtOffset:&offset];
    NSString * oldPassword=scanner(&offset);
    NSString * newPassword=scanner(&offset);
    return [self changePasswordForTPer:t oldPassword:oldPassword newPassword:newPassword];
}

#pragma mark Change user password utilities

- (bool) requireSetUpTPer:(TPer *)t userName:(NSString *)userName operation:(bool(^)(void))operation
{
    [self beginOutput];
    if (!t) return true;
    if (!t.isSetUp) return [self failMsg:@"not set up" code:SEDClientNotSetUp];
    if (!userName) return true;
    return operation();
}

- (bool) changePasswordForTPer:(TPer *)t userName:(NSString *)userName
                 adminPassword:(NSString *)adminPassword newPassword:(NSString *)newPassword
{
    return ([t changeUser:userName adminPassword: adminPassword to: newPassword])
                ? [self sendOK:t code: SEDClientChangeSuccess]
            : (adminPassword && newPassword)
                ? [self failMsg:@"wrong admin password" sd:t code:SEDClientChangeWrongOldPassword]
                : [self failMsg:@"old and new passwords required" sd:t code:SEDClientChangePasswordsRequired];
}

- (bool) changeHostChallengeForTPer:(TPer *)t userName:(NSString *)userName
                 adminHostChallenge:(NSData *)adminHostChallenge
                   newHostChallenge:(NSData *)newHostChallenge
{
    return ([t changeUser:userName adminHostChallenge:adminHostChallenge to:newHostChallenge])
                ? [self sendOK:t code: SEDClientChangeSuccess]
            : (adminHostChallenge && newHostChallenge)
                ? [self failMsg:@"wrong adin host challenge" sd:t code:SEDClientChangeWrongOldPassword]
                : [self failMsg:@"old and new host challenges required" sd:t code:SEDClientChangePasswordsRequired];
}


#pragma mark CNHG

- (bool) changePassword:(NSUInteger)offset
{
    return [self changePassword:offset scanner:self.scanWord];
}


#pragma mark XCHG

- (bool) changePasswordEncoded:(NSUInteger)offset
{
    return [self changePassword:offset scanner:self.scanEncodedPassword];
}


#pragma mark ZCHG

- (bool) changePasswordHashed:(NSUInteger)offset
{
    TPer * t=[self extractTPerNamedAtOffset:&offset];
    NSData * oldHostChallenge = [self extractPasswordData:&offset];
    NSData * newHostChallenge = [self extractPasswordData:&offset];
    return [self changeHostChallengeForTPer:t oldHostChallenge:oldHostChallenge newHostChallenge:newHostChallenge];
}


#pragma mark CHUP

- (bool) changeUserPassword:(NSUInteger)offset
{
    TPer * t=[self extractTPerNamedAtOffset:&offset];
    NSString * userName=[self extractUserNameAtOffset:&offset];
    NSString * adminPassword = [self scanWord:&offset];
    NSString * newPassword = [self scanWord:&offset];
    return [self requireSetUpTPer:t userName:userName operation:^{
        return [self changePasswordForTPer:t userName:userName adminPassword:adminPassword newPassword:newPassword];
    }];
}


#pragma mark XCCP

- (bool) changeUserPasswordEncoded:(NSUInteger)offset
{
    TPer * t=[self extractTPerNamedAtOffset:&offset];
    NSString * userName=[self extractUserNameAtOffset:&offset];
    NSString * adminPassword = [self scanEncodedPassword:&offset];
    NSString * newPassword = [self scanEncodedPassword:&offset];
    return [self requireSetUpTPer:t userName:userName operation:^{
        return [self changePasswordForTPer:t userName:userName adminPassword:adminPassword newPassword:newPassword];
    }];
}


#pragma mark ZCCP

- (bool) changeUserPasswordHashed:(NSUInteger)offset
{
    TPer * t=[self extractTPerNamedAtOffset:&offset];
    NSString * userName=[self extractUserNameAtOffset:&offset];
    NSData * adminHostChallenge = [self extractPasswordData:&offset];
    NSData * newHostChallenge = [self extractPasswordData:&offset];
    return [self requireSetUpTPer:t userName:userName operation:^{
        return [self changeHostChallengeForTPer:t
                                       userName:userName
                             adminHostChallenge:adminHostChallenge
                               newHostChallenge:newHostChallenge];
    }];
}


#pragma mark DXUP

- (bool) disableUser:(NSUInteger)offset
{
    TPer * t=[self extractTPerNamedAtOffset:&offset];
    NSString * userName=[self extractUserNameAtOffset:&offset];
    NSString * adminPassword = [self scanWord:&offset];
    return [self requireSetUpTPer:t userName:userName operation:^{
        return [self changePasswordForTPer:t userName:userName adminPassword:adminPassword newPassword:@""];
    }];
}


#pragma mark XDXP

- (bool) disableUserEncoded:(NSUInteger)offset
{
    TPer * t=[self extractTPerNamedAtOffset:&offset];
    NSString * userName=[self extractUserNameAtOffset:&offset];
    NSString * adminPassword = [self scanEncodedPassword:&offset];
    return [self requireSetUpTPer:t userName:userName operation:^{
        return [self changePasswordForTPer:t userName:userName adminPassword:adminPassword newPassword:@""];
    }];
}


#pragma mark ZDXP

- (bool) disableUserHashed:(NSUInteger)offset
{
    TPer * t=[self extractTPerNamedAtOffset:&offset];
    NSString * userName=[self extractUserNameAtOffset:&offset];
    NSData * adminHostChallenge = [self extractPasswordData:&offset];
    return [self requireSetUpTPer:t userName:userName operation:^{
        return [self changeHostChallengeForTPer:t
                                       userName:userName
                             adminHostChallenge:adminHostChallenge
                               newHostChallenge:[NSData new]];
    }];
}



#pragma mark command dispatch table
// Note inconsistency:
// lock and unlock will use the whole remainder of the input line as the password
// and hence it can include blanks or whatever.
// change expects two "words" as pulled out by extractWord, and hence "isspace"-delimited.
// The "Encoded" versions just expect the password to be base64-encoded on following lines.
// The "Hashed" versions expect the password to be hashed and then base64-encoded, so the
// daemon need not hash them again, allowing it to be more responsive.

// Create dispatch table from SEDCommands.inc
// TODO: switch to two NSDictionary objects for code and selector?  Avoid the search loop.
#define __defcmd_header__ static struct { const char * command; uint commandCode; SEL selector;} dispatch[ ] = {
#define __defcmd__(c,code,s,i,...){#c, code, sel_registerName(#s ":")},
#define __defcmd_trailer__ };
#include <SEDClientServerCommunication/SEDCommands.inc>
#undef __defcmd_header__
#undef __defcmd__
#undef __defcmd_trailer__

static NSInteger nDispatchEntries= sizeof(dispatch)/sizeof(dispatch[0]);

- (bool) processInput
{
    bool ret = false;
    bool endDevicesChangingCalledHere = false;
    NSUInteger offset=0;
    NSString * command = [self scanWord:&offset];
    
    for (NSInteger i=0; i<nDispatchEntries; i++) {
        NSString * dispatchCommand = [NSString stringWithCString:dispatch[i].command encoding:NSUTF8StringEncoding];
        if (NSOrderedSame == [command caseInsensitiveCompare:dispatchCommand]) {
            SEL selector = dispatch[i].selector;
            if ([self respondsToSelector:selector]) {
                switch (dispatch[i].commandCode) {
                        
                    case SEDCommandErase:
                    case SEDCommandSetup:
                    case SEDCommandChangePwd:
                    case SEDCommandUnprotect:
                        endDevicesChangingCalledHere = true;
                    case SEDCommandLock:                    // matching endDevicesChanging in resultContinuation
                    case SEDCommandUnlock:                  // matching endDevicesChanging in resultContinuation
                        [self.controller beginDevicesChanging];
                    default:
                        break;
                }
                
                IMP imp = [ self methodForSelector: selector ];
                typedef bool (*cmd)(id, SEL, NSUInteger);
                cmd f = (cmd)imp;
                ret = f(self,selector,offset);
                
            } else {
                NSString * unimplemented=[NSString stringWithFormat: @"command %s unimplemented!", dispatch[i].command];
                ret = [self sendCRLFAfterString:unimplemented afterCode:SEDClientUnimplemented];
            }
            if (endDevicesChangingCalledHere)
                [self.controller endDevicesChanging];
            
            return ret;
        }
    }
    NSString * unknown=[NSString stringWithFormat: @"%@? ", command];
    [self beginOutput];
    ret = [self sendCRLFAfterString:unknown afterCode:SEDClientUnknownCommand];
    
    return ret;
}

#pragma mark -
#pragma mark ConnectionDelegate methods

-(bool) announce:(NSUInteger)loopbackPort
{
    [self beginOutput];
    NSString * announcement =
        self.connection.isLoopback
            ?  [NSString stringWithFormat:@"sedd reconnected to loopback port %ld", (long)loopbackPort]
            :  [NSString stringWithFormat:@"sedd loopback port %ld", (long)loopbackPort];    
    return [self sendCRLFAfterString:announcement afterCode:SEDClientAnnounce];
}

- (bool) denounce
{
    [self beginOutput];
    return [self sendCRLFAfterString:@"bye!" afterCode:SEDClientDenounce];
}

- ( void ) connectionDidClose
{
    [self.controller removeClient:self];
    self.controller = nil;
    self.connection = nil ;
}


#pragma mark -
#pragma mark DeviceChangeDelegate protocol methods

-(void) devicesChanged { [self devicesChangedWithTag:nil]; }

-(void) devicesChangedWithTag:(NSString * )tag {
    NSLOG_DEBUG(@"DaemonClient devicesChangedWithTag:%@", tag);
    [self sendTPersDataWithCode:SEDClientShowForUpdate tag:tag];
}

@end
