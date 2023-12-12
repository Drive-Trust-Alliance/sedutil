//
//  main.cpp
//  IORegistryTest
//
//  Created by Scott Marks on 5/30/22.
//

// IOServiceMatchPropertyTable seems to match everything with IOServiceMatching
// That's unfortunate.

#include <iostream>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOKitKeys.h>
#include <IOKit/storage/IOBlockStorageDevice.h>
#include <IOKit/storage/IOMedia.h>
#include "Availability.h"


mach_port_t default_port ()
{
#if defined (MAC_OS_VERSION_12_0) && MAC_OS_VERSION_12_0 <= MAC_OS_X_VERSION_MAX_ALLOWED
    return kIOMainPortDefault;
#else
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    return kIOMasterPortDefault;
#pragma clang diagnostic pop
#endif
}


int main(int argc, const char * argv[]) {
    
    kern_return_t kernRet;

    io_iterator_t blockStorageDeviceEntryIterator = IO_OBJECT_NULL;
    kernRet =
        IOServiceGetMatchingServices(default_port(),
                                     IOServiceMatching(kIOBlockStorageDeviceClass),
                                     & blockStorageDeviceEntryIterator );
    if (KERN_SUCCESS != kernRet) {
        return -1 ; // We don't have _any_ block storage devices?!
    }
    std::cout << "Got an iterator for " << kIOBlockStorageDeviceClass
              << " with id " << blockStorageDeviceEntryIterator << std::endl;
    
    // We would normally loop here, but this is just an example, so the first one is enough ...
    io_registry_entry_t blockStorageDeviceEntry = IOIteratorNext( blockStorageDeviceEntryIterator ) ;
    if ( IO_OBJECT_NULL == blockStorageDeviceEntry ) {
        return -2 ; // We don't have _any_ block storage devices?!
    }
    std::cout << "Found an entry for " << kIOBlockStorageDeviceClass
              << " with id " << blockStorageDeviceEntry << std::endl;
    
        
    // This first one is surely going to be for the device with BSD name "disk0".
    // Just to check, let's get its IOMedia entry ..
    io_iterator_t blockStorageDeviceChildrenIterator;
    kernRet = IORegistryEntryCreateIterator(blockStorageDeviceEntry,
                                            kIOServicePlane,
                                            kIORegistryIterateRecursively,
                                            & blockStorageDeviceChildrenIterator);
    if (KERN_SUCCESS != kernRet) {
        return -3 ; // We don't have _any_ children of our block storage device?!
    }
    std::cout << "Got an iterator for the children of " << blockStorageDeviceEntry
              << " with id " << blockStorageDeviceChildrenIterator << std::endl;


    CFMutableDictionaryRef mediaClassMatching = IOServiceMatching(kIOMediaClass);
    std::cout << "Created the IOServiceMatching dictionary for " << kIOMediaClass << std::endl;

    io_registry_entry_t blockStorageDeviceChildEntry;
    while (IO_OBJECT_NULL != (blockStorageDeviceChildEntry = IOIteratorNext(blockStorageDeviceChildrenIterator) ) ) {
        std::cout << "Does child with id " << blockStorageDeviceChildEntry << " match \"" << kIOMediaClass << "\"?" << std::endl;
        io_name_t childClassName;
        IOObjectGetClass(blockStorageDeviceChildEntry, childClassName);
        std::cout << "Its class name is reported as \"" << (const char *)childClassName << "\"" << std::endl;
        
        boolean_t matchesMediaClass;
        kern_return_t kernRet = IOServiceMatchPropertyTable(blockStorageDeviceChildEntry,
                                                            mediaClassMatching,
                                                            &matchesMediaClass);
        if ( KERN_SUCCESS == kernRet && matchesMediaClass ) {
            std::cout << "It matches!" << std::endl;
        } else {
            std::cout << "It does not match." << std::endl;
        }
        
        CFStringRef childClassNameRef = IOObjectCopyClass(blockStorageDeviceChildEntry);
        if ( CFEqual( childClassNameRef, CFSTR(kIOMediaClass))) {
            std::cout << "It matches (as a string)!" << std::endl;
        } else {
            std::cout << "It does not match (as a string)." << std::endl;
        }
        CFRelease(childClassNameRef);
        
        IOObjectRelease( blockStorageDeviceChildEntry);
    }
}
