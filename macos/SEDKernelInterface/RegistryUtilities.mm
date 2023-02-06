//
//  RegistryUtilities.mm
//  sedagent
//
//  Created by Jackie Marks on 4/8/16.
//  Copyright © 2016 Bright Plaza Inc. All rights reserved.
//


#import "UserKernelShared.h"
#import "RegistryUtilities.h"


mach_port_t default_port ()
{
#if defined (MAC_OS_VERSION_12_0) && MAC_OS_VERSION_12_0 <= MAC_OS_X_VERSION_MAX_ALLOWED
    if (@available (macOS 12.0, *))
    {
        return kIOMainPortDefault;
    }
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif
    return kIOMasterPortDefault;
#if defined (MAC_OS_VERSION_12_0) && MAC_OS_VERSION_12_0 <= MAC_OS_X_VERSION_MAX_ALLOWED
#pragma clang diagnostic pop
#endif
}



// note - result is retained, should be released by caller
io_registry_entry_t findBrightPlazaDriverInParents(io_registry_entry_t service)
{
    // NSLOG_DEBUG(@"RegistryUtilities:: findBrightPlazaDriverInParents\n");
    kern_return_t kernRet;
    io_registry_entry_t driverService = IO_OBJECT_NULL;
    boolean_t matchesDriverProperty;
    io_iterator_t iterator;
    
    kernRet = IORegistryEntryCreateIterator( service,
                                            kIOServicePlane,
                                            kIORegistryIterateRecursively | kIORegistryIterateParents,
                                            &iterator);
    if (KERN_SUCCESS != kernRet || iterator == IO_OBJECT_NULL)
        return IO_OBJECT_NULL;           // error - BUG should only be here if does match.
    // but user may unplug or something
    CFDictionaryRef matching=IOServiceMatching(kDriverClass);
    while ( ( driverService = IOIteratorNext(iterator) ) ) {
        kernRet = IOServiceMatchPropertyTable( driverService,
                                              matching,
                                              &matchesDriverProperty);
        if (KERN_SUCCESS != kernRet  )
        {
            CFRelease(matching);
            IOObjectRelease(iterator);
            return IO_OBJECT_NULL;
        }
        if ( matchesDriverProperty ) {
            // NSLOG_DEBUG(@"findBrightPlazaDriverInParents leaving iterator loop driver is %d\n", driverService);
            break;
        }
        else {
            IOObjectRelease(driverService);
            // NSLOG_DEBUG(@"findBrightPlazaDriverInParents releasing driverservice and continuing loop\n");
        }
    }
    CFRelease(matching);
    IOObjectRelease(iterator);
    return driverService;
}



io_iterator_t createRecursiveIORegistryIterator(io_registry_entry_t service) {
    
    io_iterator_t iterator;
    kern_return_t kernRet;
    if (service == IO_OBJECT_NULL || service == default_port())
        kernRet = IORegistryCreateIterator(default_port(),
                                           kIOServicePlane,
                                           kIORegistryIterateRecursively,
                                           &iterator);
    else
        kernRet = IORegistryEntryCreateIterator(service,
                                                kIOServicePlane,
                                                kIORegistryIterateRecursively,
                                                &iterator);
    
    if (KERN_SUCCESS != kernRet  || iterator == IO_OBJECT_NULL)
        return IO_OBJECT_NULL;           // error - BUG should only be here if does match.
    return iterator;
}

io_registry_entry_t findServiceForClassInChildrenBelowIterator(io_iterator_t iterator, const char * className) {
    // NSLOG_DEBUG(@"findServiceForClassInChildrenBelowIterator\n");

    io_registry_entry_t entry;
    while (IO_OBJECT_NULL != (entry = IOIteratorNext(iterator) ) ) {
        if (IOObjectConformsTo(entry, className)) {
            return entry;
        }
        IOObjectRelease(entry);
    }
    return IO_OBJECT_NULL;
}

io_registry_entry_t findBrightPlazaDriverInChildrenBelowIterator(io_iterator_t iterator) {
    return findServiceForClassInChildrenBelowIterator(iterator, kDriverClass);
}

io_registry_entry_t findServiceForClassInChildren(io_registry_entry_t entry, const char * className)
{
    // NSLOG_DEBUG(@"findServiceForClassInChildren\n");
    io_iterator_t iterator = createRecursiveIORegistryIterator(entry);
    io_registry_entry_t service = findServiceForClassInChildrenBelowIterator(iterator, className);
    IOObjectRelease(iterator);
    return service;
}

io_registry_entry_t findBrightPlazaDriverInChildren(io_registry_entry_t service)
{
    // NSLOG_DEBUG(@"findBrightPlazaDriverInChildren\n");
    return findServiceForClassInChildren(service, kDriverClass);
}


io_registry_entry_t findBSDName(const char *bsdName)
{
    return IOServiceGetMatchingService(default_port(),
                                       IOBSDNameMatching(default_port(), 0, bsdName));
}

io_registry_entry_t findParent(io_registry_entry_t service)
{
    io_registry_entry_t parent;
    if (IO_OBJECT_NULL == service ||
        KERN_SUCCESS != IORegistryEntryGetParentEntry(service, kIOServicePlane, &parent)) {
        return IO_OBJECT_NULL;
    }
    return parent;
}

io_iterator_t findMatchingServices(const char * className) {
    io_iterator_t existing = IO_OBJECT_NULL;
    if (KERN_SUCCESS != IOServiceGetMatchingServices(default_port(),
                                                     IOServiceMatching(className),
                                                     &existing)) {
        return IO_OBJECT_NULL ;
    }
    return existing;
}

void GetName(io_registry_entry_t service, char * nameBuffer) {
    io_name_t name="*** UNKNOWN ***";
    IORegistryEntryGetName(service, name);
    strncpy(nameBuffer, (const char *)name, sizeof(name));
}
