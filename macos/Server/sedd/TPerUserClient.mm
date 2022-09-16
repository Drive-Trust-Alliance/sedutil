//
//  TPerUserClient.m
//  seddaemonandclient
//
//  Created by Scott Marks on 04/23/2016.
//  Copyright © 2016 MagnoliaHeights. All rights reserved.
//

#import <SEDKernelInterface/SEDKernelInterface.h>
#import "TPerUserClient.h"
#include "debug.h"

@implementation TPerUserClient
-(id) initForDriver:(IORegistryEntry *)driver {
    io_connect_t connect=0;
    kern_return_t kernResult = OpenUserClient(driver.object,&connect);
    if ( kIOReturnSuccess != kernResult) {
        return self=nil;
    }
    self=[super initWithObject:connect];
    NSLOG_DEBUG(@"TPerUserClient::initForDriver opened client 0x%016x", self.object);
    return self;
}

+(instancetype) connectionToDriver:(IORegistryEntry *)driver{
    return [[self alloc] initForDriver:driver];
}

-(void)dealloc {
    NSLOG_DEBUG(@"TPerUserClient::dealloc closing client 0x%016x", self.object);
    if (self.object) {
        kern_return_t ret = CloseUserClient(self.object);
        assert(kIOReturnSuccess == ret || MACH_SEND_INVALID_DEST==ret); // Sometimes it's already closed
#pragma unused(ret)  // avoid warning
    }
}
@end
