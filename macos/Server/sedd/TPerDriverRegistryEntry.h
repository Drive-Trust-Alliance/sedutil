//
//  TPerDriverRegistryEntry.h
//  seddaemonandclient
//
//  Created by Jackie Marks on 9/4/16.
//  Copyright © 2016 MagnoliaHeights. All rights reserved.
//

#import "TPerUserClient.h"

@interface TPerDriverRegistryEntry : IORegistryEntry

@property (nonatomic, readonly) NSString * mediaSerialNumber;
@property (nonatomic, readonly) NSData * worldWideName;


+(instancetype)forMedia: (IORegistryEntry *) media;
+(instancetype)forDriveNamed: (NSString *) bsdName;



-(void) updateLockingPropertiesInIORegistry;
-(void) writeLockingPropertiesViaUserClient: (TPerUserClient *) userClient;
-(bool) isLocked;
@end



@interface DriverIterator:IOIterator

+(instancetype) iterator;
-(TPerDriverRegistryEntry *)next;

@end
