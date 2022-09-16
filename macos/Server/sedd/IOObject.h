//
//  IOObject.h
//  sedagent
//
//  Created by Scott Marks on 04/10/2016.
//  Copyright © 2016 Bright Plaza Inc. All rights reserved.
//
#import <Foundation/Foundation.h>

@interface IOObject:NSObject
-(id) initWithObject:(io_object_t)object;
@property(nonatomic,readonly)io_object_t object;
-(NSString *)registryClassName;
@end

@interface IORegistryEntry:IOObject
-(IORegistryEntry *)parent;
-(IORegistryEntry *)child;
-(NSDictionary *)properties;
-(id)propertyForKey:(NSString *)key;
-(NSDictionary *)dictPropertyForKey:(NSString *)key;
-(NSString *)stringPropertyForKey:(NSString *)key;
-(NSData *)dataPropertyForKey:(NSString *)key;
-(uint64_t)entryID;
-(IORegistryEntry *) searchForRegistryEntryMatching: (NSString *)matchingString
                                   searchingParents: (BOOL)searchParents;
-(NSObject *)searchForProperty: (NSString *) property searchingParents: (BOOL) searchParents;
+(instancetype)entryFromObject:(IOObject *)object;
+(instancetype)entryFromEntry:(IORegistryEntry *)entry;
+(instancetype)entryForBSDName:(NSString *)bsdName;
+(instancetype)entryForEntryID:(uint64_t) entryID;
@end

@interface IOIterator:IOObject
+(instancetype) iteratorMatchingServiceClass:(NSString *)className;
+(instancetype) iteratorMatchingServiceName:(NSString *)serviceName;
-(IORegistryEntry *)next;
@end

@interface IORegistryEntry(Iteration)
-(IOIterator *) ancestorIterator;
-(IOIterator *) parentIterator;
-(IOIterator *) childIterator;
-(IOIterator *) descendantIterator;
-(IORegistryEntry *)findParentWithClassName:(NSString *)className;
-(NSString *)findBSDNameOfDescendant;
@end

@interface IOConnection:IOObject
+(instancetype)connectionFromObject:(IOObject *)object;
@end

@interface IOService:IOObject
@end
