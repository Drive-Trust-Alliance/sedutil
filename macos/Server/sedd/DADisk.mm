//
//  DADisk.m
//  seddaemonandclient
//
//  Created by Scott Marks on 05/13/2016.
//  Copyright © 2016 MagnoliaHeights. All rights reserved.
//

#import "DADisk.h"

#include "debug.h"

@interface DADisk()
@property (nonatomic,assign) DADiskRef disk;
@end

@implementation DADisk
@synthesize disk;

+(nullable DADisk *) diskFromBSDName:(nonnull NSString  *)bsdName session:(nonnull DASessionRef)session
{
    return [[self alloc] initFromBSDName:bsdName session:session];
}

+(nullable DADisk *) diskFromDiskRef:(nonnull DADiskRef)diskRef
{
    return [[self alloc] initFromDiskRef:diskRef];
}

-(id) initFromDiskRef:(DADiskRef)diskRef
{
    if ((self = [super init])) {
        CFRetain(diskRef);  // we don't know that we own this one, and we're going to release it ...
        self.disk=diskRef;
        NSLOG_DEBUG(@"DADisk::initFromDiskRef -- self=0x%016lX disk=0x%016lX", (size_t)(__bridge void *)self, (size_t)(void *)diskRef);
    }
    return self;
}

-(void) setDisk:(DADiskRef CF_RELEASES_ARGUMENT) diskRef
{
    if (NULL!=diskRef) {
        CFRetain(diskRef);
    }
    if (NULL != disk) {
        CFRelease(disk);
    }
    disk=diskRef;
    if (NULL!=diskRef) {
        CFRelease(diskRef);
    }
}

-(id) initFromBSDName:(NSString *) bsdName session:(DASessionRef)session
{
    if ((self = [super init])) {
        const char * diskName = [bsdName cStringUsingEncoding: NSUTF8StringEncoding] ;
        if (!diskName) {
            self=nil;
        } else {
            DADiskRef diskRef=DADiskCreateFromBSDName(kCFAllocatorDefault,
                                                      session,
                                                      diskName);
            if (!diskRef) {
                self=nil;
            } else {
                // we created this one, and we're going to release it ...
                self.disk=diskRef;
                NSLOG_DEBUG(@"DADisk::initFromBSDName -- self=0x%016lX disk=0x%016lX",
                            (size_t)(__bridge void *)self, (size_t)(void *)diskRef);
            }
        }
    }
    return self;
}

-( void ) unmountWithCallback:(nullable callbackfn)callback userInfo:(nullable void *)userInfo
{
    NSLOG_DEBUG(@"DADisk::unmountWithCallback -- self=0x%016lX disk=0x%016lX", (size_t)(__bridge void *)self, (size_t)(void *)self.disk);
    DADiskRef diskRef=self.disk;
    if (diskRef) {
        DADiskUnmount(diskRef, kDADiskUnmountOptionForce | kDADiskUnmountOptionWhole, callback, userInfo);
    }
}

-( void ) mountWithCallback:(nullable callbackfn)callback whole:(bool)whole userInfo:(nullable void *)userInfo
{
    NSLOG_DEBUG(@"DADisk::mountWithCallback -- self=0x%016lX disk=0x%016lX", (size_t)(__bridge void *)self, (size_t)(void *)self.disk);
    DADiskRef diskRef=self.disk;
    if (diskRef) {
        DADiskMount(diskRef, NULL, (whole ? kDADiskMountOptionWhole : kDADiskMountOptionDefault), callback, (void *)userInfo);
    }
}

-(void) claimWithCallback:(nullable callbackfn)callback userInfo:(nullable void *)userInfo
{
    NSLOG_DEBUG(@"DADisk::claimWithCallback -- self=0x%016lX disk=0x%016lX", (size_t)(__bridge void *)self, (size_t)(void *)self.disk);
    DADiskRef diskRef=self.disk;
    if (diskRef) {
        DADiskClaim(diskRef, kDADiskClaimOptionDefault, NULL, NULL, callback, userInfo);
    }
}

-(void) unclaim
{
    NSLOG_DEBUG(@"DADisk::unclaim -- self=0x%016lX disk=0x%016lX", (size_t)(__bridge void *)self, (size_t)(void *)self.disk);
    DADiskRef diskRef=self.disk;
    if (diskRef) {
        DADiskUnclaim(diskRef);
    }
}

-(NSString *)bsdName {
    DADiskRef diskRef=self.disk;
    if (diskRef) {
        const char * diskName=DADiskGetBSDName(diskRef);
        if (diskName) {
            return [NSString stringWithCString:diskName encoding:NSUTF8StringEncoding];
        }
    }
    return @"???";
}

-(nullable IORegistryEntry *)media {
    DADiskRef diskRef=self.disk;
    if (diskRef) {
        return [[IORegistryEntry alloc] initWithObject:DADiskCopyIOMedia(diskRef)];
    }
    return nil;
}

-(void) dealloc
{
    self.disk=NULL;
}

@end
