//
//  TPerDriverRegistryEntry.m
//  seddaemonandclient
//
//  Created by Jackie Marks on 9/4/16.
//  Copyright © 2016 MagnoliaHeights. All rights reserved.
//

#import <SEDKernelInterface/SEDKernelInterface.h>
#import "TPerDriverRegistryEntry.h"
#import "AvailabilityMacros.h"


@implementation TPerDriverRegistryEntry

-(NSString *)mediaSerialNumber {
    @autoreleasepool {
        NSDictionary * dict = [self dictPropertyForKey: @IOIdentifyCharacteristicsKey];
        return dict[@IOSerialNumberKey];
    }
}

-(NSData *)worldWideName {
    @autoreleasepool {
        NSDictionary * dict = [self dictPropertyForKey: @IOIdentifyCharacteristicsKey];
        return dict[@IOWorldWideNameKey];
    }
}

+(instancetype)forMedia: (IORegistryEntry *) media {
    IORegistryEntry * driver = media ? [media findParentWithClassName:@kBrightPlazaDriverClass] : nil;
    return [self entryFromEntry: driver];
}

+(instancetype)forDriveNamed: (NSString *) bsdName {
    IORegistryEntry * media = [IORegistryEntry entryForBSDName:bsdName];
    return [self forMedia: media];
}

-(void) updateLockingPropertiesInIORegistry {
    @autoreleasepool {
        kern_return_t kernRet = kIOReturnError;
        
        TPerUserClient * userClient = [TPerUserClient connectionToDriver: self];
        
        kernRet = updateLockingPropertiesInIORegistry(userClient.object);
        
        (void)kernRet;
    }
}

-(void) writeLockingPropertiesViaUserClient: (TPerUserClient *) userClient {
    @autoreleasepool {
        kern_return_t kernRet = kIOReturnError;
        kernRet = updateLockingPropertiesInIORegistry(userClient.object);
        (void) kernRet;
    }
}

-(bool) isLocked {
    @autoreleasepool {
        NSDictionary * dict = [self dictPropertyForKey: @IOTPerLockingFeatureKey];
        
        bool lockingEnabled = [dict[@IOLockingEnabledKey] boolValue];
        bool locked = [dict[@IOLockedKey] boolValue];
        bool mbrEnabled = [dict[@IOMBREnabledKey] boolValue];
        bool mbrDone = [dict[@IOMBRDoneKey] boolValue];
        
        return (lockingEnabled && locked) || (mbrEnabled && !mbrDone);
    }
}

@end



@implementation DriverIterator

+(instancetype) iterator {
    CFDictionaryRef matchingDict = IOServiceMatching( kBrightPlazaDriverClass);
    io_iterator_t iterator = IO_OBJECT_NULL;
    IOServiceGetMatchingServices(
#if defined(MAC_OS_X_VERSION_MIN_REQUIRED) && MAC_OS_VERSION_12_0 <= MAC_OS_X_VERSION_MIN_REQUIRED
                                 kIOMainPortDefault
#else
                                 kIOMasterPortDefault
#endif
                                 , matchingDict, &iterator);
    return iterator!=IO_OBJECT_NULL ? [[self alloc] initWithIterator:iterator] : nil;;
}

-(id) initWithIterator:(io_iterator_t)iterator {
    self=[super initWithObject:iterator];
    return self;
}

-(TPerDriverRegistryEntry *)next {
    io_object_t object=IOIteratorNext(self.object);
    return (object != IO_OBJECT_NULL) ? [[TPerDriverRegistryEntry alloc] initWithObject:object] : nil;
}

@end

