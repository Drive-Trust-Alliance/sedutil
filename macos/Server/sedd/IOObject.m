//
//  IOObject.m
//  sedagent
//
//  Created by Scott Marks on 04/10/2016.
//  Copyright © 2016 Bright Plaza Inc. All rights reserved.
//

#import "IOObject.h"
//#undef DEBUG //// <-------------  HACK!!! undo!!!  TODO
#include "debug.h"

#define DEBUG_IOOBJECTS 0
#if DEBUG_IOOBJECTS
#define IOOBJECT_DEBUG NSLOG_DEBUG
#else
#define IOOBJECT_DEBUG(...) do ; while(0)
#endif

@interface IOObject(){
    io_object_t _object;
}
@end

@implementation IOObject
-(id) initWithObject:(io_object_t)object {
    if (0==object) {
        self=nil;
    }
    else if ((self=[super init])) {
        self->_object=object;
        IOOBJECT_DEBUG(@"IOObject::initWithObject self=0x%016lx object=%d",
              (size_t)(__bridge void *)self, self->_object);
    }
    return self;
}
-(void) dealloc{
    IOOBJECT_DEBUG(@"IOObject::dealloc releasing self=0x%016lx object=%d",
          (size_t)(__bridge void *)self, self->_object);
    IOObjectRelease(self->_object);
    self->_object=IO_OBJECT_NULL;
}
-(io_object_t) object{return _object;}

-(NSString *)registryClassName { // could be className, but Objective-C already uses that
    CFStringRef objectClassNameRef=IOObjectCopyClass(self.object);
    NSString * className=[NSString stringWithString:(__bridge_transfer NSString *)objectClassNameRef];
    return className;
}
@end

@interface IORegistryEntry()
-(id)initWithEntry:(io_registry_entry_t)entry;
-(io_registry_entry_t)entry;
@end

@implementation IORegistryEntry
-(id)initWithEntry:(io_registry_entry_t)entry {
    self=[super initWithObject:entry];
    return self;
}
-(io_registry_entry_t)entry { return (io_registry_entry_t)(self.object); }

-(NSObject *)searchForProperty: (NSString *) property searchingParents: (BOOL) searchParents {
    IOOptionBits opts = searchParents
                            ? kIORegistryIterateRecursively | kIORegistryIterateParents
                            : kIORegistryIterateRecursively;
    return CFBridgingRelease(IORegistryEntrySearchCFProperty(self.object, kIOServicePlane,
                                                             (__bridge CFStringRef)property,
                                                             kCFAllocatorDefault, opts));
}


-(IORegistryEntry *) searchForRegistryEntryMatching: (NSString *)matchingString
                                   searchingParents: (BOOL)searchParents {
    IOIterator * iterator = searchParents ? [self ancestorIterator] : [self descendantIterator];
    
    if (!iterator)
        return 0;
    
    CFDictionaryRef matching=IOServiceMatching(matchingString.UTF8String);
    
    
    boolean_t matches = 0;
    IORegistryEntry *service;
    while ( (service = [iterator next]) ) {
        IOServiceMatchPropertyTable(service.object,
                                    matching,
                                    &matches);
        if (matches){
            break;
        }
    }
    
    CFRelease( matching );
    return matches ? service : nil;
}

-(IORegistryEntry *)parent {
    io_registry_entry_t parent=IO_OBJECT_NULL;
    kern_return_t kernResult = IORegistryEntryGetParentEntry(self.entry, kIOServicePlane, &parent );
    return (KERN_SUCCESS == kernResult ) ? [[[self class] alloc] initWithEntry:parent] : nil;
}

-(IORegistryEntry *)child {
    io_registry_entry_t child=IO_OBJECT_NULL;
    kern_return_t kernResult = IORegistryEntryGetChildEntry(self.entry, kIOServicePlane, &child );
    return (KERN_SUCCESS == kernResult ) ? [[[self class] alloc] initWithEntry:child] : nil;
}

-(NSDictionary *)properties {
    CFMutableDictionaryRef props;
    kern_return_t kern_return= IORegistryEntryCreateCFProperties(self.object, &props, kCFAllocatorDefault, 0);
    return kIOReturnSuccess==kern_return ? (__bridge NSMutableDictionary *)props : nil ;
}

-(id)propertyForKey:(NSString *)key {
    return CFBridgingRelease(IORegistryEntryCreateCFProperty(self.entry, (__bridge CFStringRef)key, kCFAllocatorDefault, 0));
}

-(NSDictionary *)dictPropertyForKey:(NSString *)key {
    return (NSDictionary *)[self propertyForKey:key];
}

-(NSString *)stringPropertyForKey:(NSString *)key {
    return (NSString *)[self propertyForKey:key];
}

-(NSData *)dataPropertyForKey:(NSString *)key {
    return (NSData *)[self propertyForKey:key];
}

-(uint64_t)entryID {
    uint64_t entryID=0;
    kern_return_t ret=IORegistryEntryGetRegistryEntryID(self.object, &entryID);
    return (kIOReturnSuccess==ret) ? entryID : 0 ;
}

+(instancetype)entryFromObject:(IOObject *)object
{
    if (!object) return nil;
    io_object_t obj=object.object;
    kern_return_t kern_return=IOObjectRetain(obj);
    return  (kIOReturnSuccess==kern_return) ? [[self alloc] initWithEntry:(io_registry_entry_t)(object.object)] : nil;
}

+(instancetype)entryFromEntry:(IORegistryEntry *)entry
{
    if (!entry) return nil;
    kern_return_t kern_return=IOObjectRetain(entry.entry);
    return  (kIOReturnSuccess==kern_return) ? [[self alloc] initWithEntry:entry.entry] : nil;
}

+(instancetype)entryForBSDName:(NSString *)bsdName {
    if (!bsdName) return nil;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    io_service_t service=
        IOServiceGetMatchingService(kIOMasterPortDefault,
                                    IOBSDNameMatching(kIOMasterPortDefault,
                                                      0,
                                                      bsdName.UTF8String));
#pragma clang diagnostic pop
    return service ? [[self alloc] initWithEntry:service] : nil ;
}

+(instancetype)entryForEntryID:(uint64_t) entryID {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    io_registry_entry_t entry = IOServiceGetMatchingService(kIOMasterPortDefault,
                                                            IORegistryEntryIDMatching(entryID));
#pragma clang diagnostic pop
    return [[self alloc] initWithEntry:entry];
}
@end

@interface IOIterator()
-(id) initWithIterator:(io_iterator_t)iterator;
-(io_iterator_t)iterator;
+(instancetype) ioiteratorWithIterator:(io_iterator_t) iterator;
@end

@implementation IOIterator
-(id) initWithIterator:(io_iterator_t)iterator {
    self=[super initWithObject:iterator];
    return self;
}

+(instancetype) ioiteratorWithIterator:(io_iterator_t) iterator
{
    return iterator!=IO_OBJECT_NULL ? [[self alloc]initWithIterator:iterator] : nil;
}

-(io_iterator_t)iterator { return (io_iterator_t)(self.object); }

// WARNING: the method below calls IOServiceGetMatchingServices,
// which releases one reference to its matchingDict parameter via CF_RELEASES_ARGUMENT
// See the declaration of IOServiceGetMatchingServices

static io_iterator_t getMatchingServicesIterator(CFDictionaryRef matchingDict CF_RELEASES_ARGUMENT)  {
    io_iterator_t iterator = IO_OBJECT_NULL;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    IOServiceGetMatchingServices(kIOMasterPortDefault, matchingDict, &iterator);
#pragma clang diagnostic pop
    return iterator;
}

+(instancetype) iteratorMatchingServiceClass:(NSString *)className {
    return [self ioiteratorWithIterator:getMatchingServicesIterator(IOServiceMatching(className.UTF8String))];
}

+(instancetype) iteratorMatchingServiceName:(NSString *)serviceName {
    return [self ioiteratorWithIterator:getMatchingServicesIterator(IOServiceNameMatching(serviceName.UTF8String))];
}

+(instancetype) iteratorOfAncestorsOf:(IORegistryEntry *)entry {
    io_iterator_t iterator;
    kern_return_t kernResult =IORegistryEntryCreateIterator(entry.object,
                                                            kIOServicePlane,
                                                            kIORegistryIterateRecursively | kIORegistryIterateParents,
                                                            &iterator);
    return kIOReturnSuccess == kernResult ? [[self alloc] initWithIterator:iterator] : nil;
}

+(instancetype) iteratorOfDescendentsOf:(IORegistryEntry *)entry {
    io_iterator_t iterator;
    kern_return_t kernResult =IORegistryEntryCreateIterator(entry.object,
                                                            kIOServicePlane,
                                                            kIORegistryIterateRecursively,
                                                            &iterator);
    return kIOReturnSuccess == kernResult ? [[self alloc] initWithIterator:iterator] : nil;
}

-(IORegistryEntry *)next {
    io_object_t object=IOIteratorNext(self.iterator);
    return (object != IO_OBJECT_NULL) ? [[IORegistryEntry alloc] initWithObject:object] : nil;
}

@end

@implementation IORegistryEntry(Iteration)
-(IOIterator *) ancestorIterator { return [IOIterator iteratorOfAncestorsOf: self];}

-(IOIterator *) parentIterator {
    io_iterator_t iterator;
    kern_return_t kernResult =
    IORegistryEntryGetParentIterator(self.object, kIOServicePlane, &iterator );
    if (KERN_SUCCESS != kernResult ) return nil;

    return [[IOIterator alloc]initWithIterator:iterator];
}

-(IOIterator *) childIterator {
    io_iterator_t iterator;
    kern_return_t kernResult = IORegistryEntryGetChildIterator(self.object, kIOServicePlane, &iterator );
    if (KERN_SUCCESS != kernResult ) return nil;

    return [[IOIterator alloc]initWithIterator:iterator];
}

-(IOIterator *) descendantIterator { return [IOIterator iteratorOfDescendentsOf: self];}

-(IORegistryEntry *)findParentWithClassName:(NSString *)className {
    @autoreleasepool {
        IOIterator * ancestorIterator = [ self ancestorIterator] ;
        IOObject * parent;
        while ( ( parent = (IORegistryEntry *)ancestorIterator.next )) {
            NSString * registryClassName = parent.registryClassName;
            if ( !registryClassName )
                return nil;
            if ([registryClassName isEqualToString: className]) {
                return [IORegistryEntry entryFromObject:parent];
            }
        }
        return nil;
    }
}

-(NSString *)findBSDNameOfDescendant
{
    return ( __bridge_transfer NSString *)IORegistryEntrySearchCFProperty(self.object,
                                                                          kIOServicePlane,
                                                                          CFSTR(kIOBSDNameKey),
                                                                          kCFAllocatorDefault,
                                                                          kIORegistryIterateRecursively);
}

@end

@implementation IOConnection
-(id)initWithConnection:(io_connect_t)connect {
    self=[super initWithObject:connect];
    return self;
}
+(instancetype)connectionFromObject:(IOObject *)object
{
    io_object_t obj=object.object;
    kern_return_t kern_return=IOObjectRetain(obj);
    return kIOReturnSuccess==kern_return ? [[self alloc] initWithConnection:(io_connect_t)(object.object)] : nil;
}
@end

@implementation IOService:IOObject
@end
