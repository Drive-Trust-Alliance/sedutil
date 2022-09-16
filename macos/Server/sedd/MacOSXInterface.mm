//
//  MacOSXInterface.mm
//  sedagent
//
//  Created by Jackie Marks on 04/2/2016.
//  Copyright © 2016 Bright Plaza Inc. All rights reserved.
//

#import <SEDKernelInterface/SEDKernelInterface.h>  // C interface to TPer kernel driver via "user client"
#import "TCGDrive.h"
#import "StorageDeviceManager.h"
#import "MacOSXInterface.h"
#include "debug.h"


// xxxxxx  TODO need to put this in OSInterface
// TODO factor using blocks to hide passwords-to-be-hashed from hostChallenges-not-to-be-hashed?
//      could wrap up MacOSXInterface stuff in block, do most of the structure in OSInterface
//
@interface MacOSXInterface()      //   <DeviceChangemanager>
@end

@implementation MacOSXInterface
@synthesize fileSystemInterface;

#pragma mark -
#pragma mark Drive commands from client

// single password for locking and mbr
- (TCGSTATUSCODE) lockDrive:(TCGDrive *)tcgDrive
         password:(NSString *)password
       connection:(TPerUserClient *)connection
{
    
    TCGSTATUSCODE ret = [tcgDrive lockDriveWithPassword: password];
    
    if ( ret == SUCCESS ) {
        ret = KERN_SUCCESS == updateLockingPropertiesInIORegistry(connection.object) ? SUCCESS : FAIL ;
    }
    return ret;
}

- (TCGSTATUSCODE) lockDrive:(TCGDrive *)tcgDrive
    hostChallenge:(NSData *)hostChallenge
       connection:(TPerUserClient *)connection
{
    
    TCGSTATUSCODE ret = [tcgDrive lockDriveWithHostChallenge: hostChallenge];
    
    if (SUCCESS == ret) {
        ret = KERN_SUCCESS == updateLockingPropertiesInIORegistry(connection.object) ? SUCCESS : FAIL ;
    }
    return ret;
}

- (TCGSTATUSCODE) unlockDrive:(TCGDrive *)tcgDrive
           password:(NSString *)password
         connection:(TPerUserClient *)connection
{
    TCGSTATUSCODE ret = [tcgDrive unlockDriveWithPassword: password];
    
    if (SUCCESS == ret) {
        ret = KERN_SUCCESS == updateLockingPropertiesInIORegistry(connection.object) ? SUCCESS : FAIL ;
    }
    return ret;
}

- (TCGSTATUSCODE) unlockDrive:(TCGDrive *)tcgDrive
                hostChallenge:(NSData *)hostChallenge
                   connection:(TPerUserClient *)connection
{
    TCGSTATUSCODE ret = [tcgDrive unlockDriveWithHostChallenge:hostChallenge];
    
    if (SUCCESS == ret) {
        ret = KERN_SUCCESS == updateLockingPropertiesInIORegistry(connection.object) ? SUCCESS : FAIL ;
    }
    return ret;
}


typedef TCGSTATUSCODE(^InterfaceOperation)(TCGDrive * tcgDrive, TPerUserClient * connection);
-(TCGSTATUSCODE) unmountAndRemountTPer:(TPer *)tPer
                       aroundOperation:(InterfaceOperation)operation
                          continuation:(ResultContinuation)resultContinuation
{
    TCGSTATUSCODE ret = FAIL;
    
    @autoreleasepool {
        NSString * bsdName = tPer.deviceName;
        IORegistryEntry * driver=tPer.driverHandle;
        if (!driver) return ret;  // TODO: FAIL_NO_DRIVER
        
        TPerUserClient * connection = [TPerUserClient connectionToDriver:driver];
        if (!connection) return ret;  // TODO: FAIL_COULD_NOT_CONNECT_TO_DRIVER
        
        NSArray * partitions=[self.fileSystemInterface partitionsOfDeviceNamed:bsdName];
        
        Continuation continuation = ^{
            NSLOG_DEBUG(@"In continuation:");
            NSLOG_DEBUG(@"bsdName=%@", bsdName);
            NSLOG_DEBUG(@"driver=%@ driver.object=%d", driver, driver.object);
            NSLOG_DEBUG(@"connection=%@ connection.object=%d", connection, connection.object);
            NSLOG_DEBUG(@"partitions=%@", partitions);
            
            TCGSTATUSCODE cret = FAIL;
            TCGDrive * tcgDrive = [TCGDrive driveForBsdName:bsdName
                                              driverService:driver.object
                                                    connect:connection.object];
            if (tcgDrive)
                cret = operation(tcgDrive, connection);
            if ( cret == SUCCESS ) {
                cret = KERN_SUCCESS == updateLockingPropertiesInIORegistry(connection.object) ? SUCCESS : FAIL ;
                [self.fileSystemInterface reenumerateUSBAndMountDrive:tPer];
            } else if (partitions) {      // assumes mbr failed and drive is in same state
                [self.fileSystemInterface remountDriveUsingDiskArbitration:bsdName partitions:partitions];
            }
            resultContinuation(cret);
        };
        
        NSLOG_DEBUG(@"unmountAndRemountTPer - before unmountDrive, drive = %@, continuation = 0x%p", bsdName, (void *) &continuation );
        
        if ( partitions ) {
            [self.fileSystemInterface unmountDriveUsingDiskArbitration:bsdName withContinuation: continuation];
        } else {
            continuation();
        }
    }
    return SUCCESS;  // At least, so far ...
}

-(TCGSTATUSCODE)lockOrUnlockDrive:(TPer *)tPer
                locking:(BOOL) locking
               password:(NSString *)password
           continuation:(ResultContinuation) resultContinuation
{
    InterfaceOperation lockOrUnlock =
                           locking
                                ? ^(TCGDrive * tcgDrive, TPerUserClient * connection){
                                        return [self lockDrive:tcgDrive password:password connection:connection];
                                   }
                                : ^(TCGDrive * tcgDrive, TPerUserClient * connection){
                                        return [self unlockDrive:tcgDrive password:password connection:connection];
                                   };
    return [self unmountAndRemountTPer:tPer
                       aroundOperation:lockOrUnlock
                          continuation:resultContinuation];
}

-(TCGSTATUSCODE)lockOrUnlockDrive:(TPer *)tPer
                locking:(BOOL) locking
          hostChallenge:(NSData *)hostChallenge
           continuation:(ResultContinuation) resultContinuation
{
    InterfaceOperation lockOrUnlock =
                           locking
                               ? ^(TCGDrive * tcgDrive, TPerUserClient * connection){
                                       return [self lockDrive:tcgDrive hostChallenge:hostChallenge connection:connection];
                                  }
                               : ^(TCGDrive * tcgDrive, TPerUserClient * connection){
                                       return [self unlockDrive:tcgDrive hostChallenge:hostChallenge connection:connection];
                                  };
    return [self unmountAndRemountTPer:tPer
                     aroundOperation:lockOrUnlock
                        continuation:resultContinuation];
}

- (TCGSTATUSCODE) withTPer:(TPer *)tPer onTCGDrive:(TCGSTATUSCODE (^)(TCGDrive * tcgDrive))operation {
    [self.manager beginDevicesChanging];
    TCGSTATUSCODE ret = FAIL;
    
    @autoreleasepool {
        NSString * bsdName = tPer.deviceName;
        
        IORegistryEntry * driver =tPer.driverHandle;
        if (!driver) return ret;  // TODO: FAIL_NO_DRIVER
        
        TPerUserClient * connection = [TPerUserClient connectionToDriver:driver];
        if (!connection) return ret;  // TODO: FAIL_COULD_NOT_CONNECT_TO_DRIVER
        
        TCGDrive * tcgDrive = [TCGDrive driveForBsdName:bsdName
                                          driverService:driver.object
                                                connect:connection.object];
        if (!tcgDrive) return ret; // TODO: FAIL_NO_SUCH_DRIVE
        
        ret = operation(tcgDrive);
        if (SUCCESS == ret) {
            ret = KERN_SUCCESS == updateLockingPropertiesInIORegistry(connection.object) ? SUCCESS : FAIL ;
        }
    }
    
    return ret;
}

- (TCGSTATUSCODE) unlockDrive:(TPer *)tPer hostChallenge:(NSData *)hostChallenge
{
    return [self withTPer:tPer onTCGDrive:^(TCGDrive * tcgDrive){
        return [tcgDrive unlockDriveWithHostChallenge: hostChallenge];
    }];
}

- (TCGSTATUSCODE) setupDrive:(TPer *)tPer password:(NSString *)password
{
    return [self withTPer:tPer onTCGDrive:^(TCGDrive * tcgDrive){
        return [tcgDrive setupDriveWithPassword: password];
    }];
}

- (TCGSTATUSCODE) setupDrive:(TPer *)tPer hostChallenge:(NSData *)hostChallenge
{
    return [self withTPer:tPer onTCGDrive:^(TCGDrive * tcgDrive){
        return [tcgDrive setupDriveWithHostChallenge:hostChallenge];
    }];
}

#if MULTISTART
- (TCGSTATUSCODE) multiUserSetupDrive:(TPer *)tPer password:(NSString *)password
{
    return [self withTPer:tPer onTCGDrive:^(TCGDrive * tcgDrive){
        return [tcgDrive multiUserSetupDriveWithPassword: password];
    }];
}

- (TCGSTATUSCODE) multiUserSetupDrive:(TPer *)tPer hostChallenge:(NSData *)hostChallenge
{
    return [self withTPer:tPer onTCGDrive:^(TCGDrive * tcgDrive){
        return [tcgDrive setupDriveWithHostChallenge:hostChallenge];
    }];
}
#endif // MULTISTART


- (TCGSTATUSCODE) eraseDrive:(TPer *)tPer password:(NSString *)password
{
    [self.manager beginDevicesChanging];
    TCGSTATUSCODE ret = FAIL;
    
    @autoreleasepool {
        NSString * bsdName = tPer.deviceName;
        
        IORegistryEntry * driver =tPer.driverHandle;
        if (!driver) return ret;  // TODO: FAIL_NO_DRIVER
        
        TPerUserClient * connection = [TPerUserClient connectionToDriver:driver];
        if (!connection) return ret;  // TODO: FAIL_COULD_NOT_CONNECT_TO_DRIVER
        
        TCGDrive * tcgDrive = [TCGDrive driveForBsdName:bsdName
                                          driverService:driver.object
                                                connect:connection.object];
        if (!tcgDrive) return ret; // TODO: FAIL_NO_SUCH_DRIVE
        
        NSArray * partitions=[self.fileSystemInterface partitionsOfDeviceNamed:bsdName];
        if ( partitions )
            [self.fileSystemInterface unmountDriveUsingDiskArbitration: bsdName];
        
        ret = [tcgDrive eraseDriveWithPassword: password];
        
        if (SUCCESS == ret) {
            updateLockingPropertiesInIORegistry(connection.object);
            [self.fileSystemInterface reenumerateUSBAndMountDrive: tPer ];
        } else if (partitions) {
            [self.fileSystemInterface remountDriveUsingDiskArbitration:bsdName partitions:partitions];
        }
    }
    return ret;
}

- (TCGSTATUSCODE) eraseDrive:(TPer *)tPer hostChallenge:(NSData *)hostChallenge
{
    [self.manager beginDevicesChanging];
    TCGSTATUSCODE ret = FAIL;
    
    @autoreleasepool {
        NSString * bsdName = tPer.deviceName;
        
        IORegistryEntry * driver =tPer.driverHandle;
        if (!driver) return ret;  // TODO: FAIL_NO_DRIVER
        
        TPerUserClient * connection = [TPerUserClient connectionToDriver:driver];
        if (!connection) return ret;  // TODO: FAIL_COULD_NOT_CONNECT_TO_DRIVER
        
        TCGDrive * tcgDrive = [TCGDrive driveForBsdName:bsdName
                                          driverService:driver.object
                                                connect:connection.object];
        if (!tcgDrive) return ret; // TODO: FAIL_NO_SUCH_DRIVE
        
        NSArray * partitions=[self.fileSystemInterface partitionsOfDeviceNamed:bsdName];
        if ( partitions )
            [self.fileSystemInterface unmountDriveUsingDiskArbitration: bsdName];
        
        ret = [tcgDrive eraseDriveWithHostChallenge:(NSData *)hostChallenge];
        
        if (SUCCESS == ret) {
            updateLockingPropertiesInIORegistry(connection.object);
            [self.fileSystemInterface reenumerateUSBAndMountDrive: tPer ];
        } else if (partitions) {
            [self.fileSystemInterface remountDriveUsingDiskArbitration:bsdName partitions:partitions];
        }
    }
    return ret;
}


-(TCGSTATUSCODE) revertDrive:(TPer * )tPer PSID:(NSString * )PSID continuation:(ResultContinuation)resultContinuation
{
    return [self unmountAndRemountTPer:tPer
                       aroundOperation:^(TCGDrive * tcgDrive, TPerUserClient *){return [tcgDrive revertWithPSID:PSID];}
                          continuation:resultContinuation];
    
}




- (TCGSTATUSCODE) changePasswordForDrive:(TPer *)tPer
                   oldPassword:(NSString *)oldPassword
                   newPassword:(NSString *)newPassword
{
    return [self withTPer:tPer onTCGDrive:^(TCGDrive * tcgDrive){
        NSLOG_DEBUG(@"In changePasswordForDrive: about to change password from \"%@\" to \"%@\"", oldPassword, newPassword);
        TCGSTATUSCODE ret = [tcgDrive changeOldPassword: oldPassword toNewPassword: newPassword];
        NSLOG_DEBUG(@"In changePasswordForDrive: change password result=%d", ret);
        return ret;
    }];
}

- (TCGSTATUSCODE) changeHostChallengeForDrive:(TPer *)tPer
                   oldHostChallenge:(NSData *)oldHostChallenge
                   newHostChallenge:(NSData *)newHostChallenge
{
    return [self withTPer:tPer onTCGDrive:^(TCGDrive * tcgDrive){
        return [tcgDrive changeOldHostChallenge:oldHostChallenge toNewHostChallenge:newHostChallenge];
    }];
}

- (TCGSTATUSCODE) changePasswordForDrive:(TPer *)tPer
                      userName:(NSString *)userName
                   adminPassword:(NSString *)adminPassword
                   newPassword:(NSString *)newPassword
{
    NSLOG_DEBUG( @"passwords are %@ and %@ ", adminPassword, newPassword );
    return [self withTPer:tPer onTCGDrive:^(TCGDrive * tcgDrive){
        return [tcgDrive changeUser:userName adminPassword: adminPassword toNewPassword: newPassword];
    }];
}

- (TCGSTATUSCODE) changeHostChallengeForDrive:(TPer *)tPer
                           userName:(NSString *)userName
                   adminHostChallenge:(NSData *)adminHostChallenge
                   newHostChallenge:(NSData *)newHostChallenge
{
    return [self withTPer:tPer onTCGDrive:^(TCGDrive * tcgDrive){
        return [tcgDrive changeUser:userName adminHostChallenge:adminHostChallenge toNewHostChallenge:newHostChallenge];
    }];
}


- (TCGSTATUSCODE) unprotectDrive:(TPer *)tPer password:(NSString *)password
{
    return [self withTPer:tPer onTCGDrive:^(TCGDrive * tcgDrive){
        return [tcgDrive unprotectDriveWithPassword: password];
    }];
}


- (TCGSTATUSCODE) unprotectDrive:(TPer *)tPer hostChallenge:(NSData *)hostChallenge
{
    return [self withTPer:tPer onTCGDrive:^(TCGDrive * tcgDrive){
        return [tcgDrive unprotectDriveWithHostChallenge:hostChallenge];
    }];
}


- (TCGSTATUSCODE) getMSID:(NSMutableString *)MSID forTPer:(TPer *)tPer
{
    TCGSTATUSCODE ret = FAIL;
    
    @autoreleasepool {
        NSString * bsdName = tPer.deviceName;
        
        IORegistryEntry * driver =tPer.driverHandle;
        if (!driver) return ret;  // TODO: FAIL_NO_DRIVER
        
        TPerUserClient * connection = [TPerUserClient connectionToDriver:driver];
        if (!connection) return ret;  // TODO: FAIL_COULD_NOT_CONNECT_TO_DRIVER
        
        TCGDrive * tcgDrive = [TCGDrive driveForBsdName:bsdName
                                          driverService:driver.object
                                                connect:connection.object];
        if (!tcgDrive) return ret; // TODO: FAIL_NO_SUCH_DRIVE
        
        ret = [tcgDrive getMSID:MSID];
        NSLOG_DEBUG(@"In MSID result=%d", ret);
    } // autoreleasepool
    
    return ret;
}

//#pragma mark -
//#pragma mark OSInterfaceProtocol
#if NONSTUB
#pragma mark - IFCommands for future
-(bool) sendIFCommand:(TCG_IO_COMMAND)tcg_io_command
                 tPer:(TPer *)tPer
               buffer:(void *)buffer
              pLength:(uint64_t *)pLength
{
    SCSICommandDescriptorBlock cdb={0};
    cdb[0] = SCSI_ATA_PASS_THROUGH;
    
    ATA_COMMAND ataCommand;
    switch (tcg_io_command) {
        case TCG_IF_RECV:
            ataCommand=ATA_TRUSTED_RECEIVE_PIO;
            cdb[1] = 4 << 1; // PIO DATA IN
            cdb[2] = 0x0E; // T_DIR = 1, BYTE_BLOCK = 1, Length in Sector Count
            break;
            
        case TCG_IF_SEND:
            ataCommand=ATA_TRUSTED_SEND_PIO;
            cdb[1] = 5 << 1; // PIO DATA OUT
            cdb[2] = 0x06; // T_DIR = 0, BYTE_BLOCK = 1, Length in Sector Count
            break;
            
        default:
            return false;
    }
    
    cdb[3] = 1; // ATA features / TRUSTED S/R security protocol
    cdb[4] = (UInt8)((*pLength)/512); // Sector count / transfer length (512b blocks)
    //      cdb[5] = reserved;
    [tPer getComID:&cdb[6]];
    //      cdb[8] = 0x00;              // device
    cdb[9] = ataCommand;
    //      cdb[10] = 0x00;              // reserved
    //      cdb[11] = 0x00;              // control
    
    @autoreleasepool {
        // looks wrong IOConnection should be driver, but SCSIPassTHough takes connect (io_connect_t to userclient)
        //        TPerUserClient * connection=[self findTPerDriverFor:tPer];
        IORegistryEntry * driver = tPer.driverHandle;
        TPerUserClient * connect = [TPerUserClient connectionToDriver: driver];
        
        
        if (!connect) return false;
        
        kern_return_t kernResult = SCSIPassThroughInterface(connect.object, cdb, buffer, pLength);
        
        return (kernResult == kIOReturnSuccess) ;
    }
}


-(bool) sendIFSendTper:(TPer *)tPer buffer:(void *)buffer pLength:(uint64_t *)pLength {
    return [self sendIFCommand:(TCG_IO_COMMAND)TCG_IF_RECV tPer:tPer buffer:buffer pLength:pLength];
}
-(bool) sendIFRecvTper:(TPer *)tPer buffer:(void *)buffer pLength:(uint64_t *)pLength {
    return [self sendIFCommand:(TCG_IO_COMMAND)TCG_IF_SEND tPer:tPer buffer:buffer pLength:pLength];
}
#endif //NONSTUB


@end



