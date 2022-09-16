//
//  RegistryUtilities.h
//  sedagent
//
//  Created by Jackie Marks on 4/8/16.
//  Copyright © 2016 Bright Plaza Inc. All rights reserved.
//

#ifndef RegistryUtilities_h
#define RegistryUtilities_h

#include <IOKit/IOKitLib.h>
#import <Availability.h>



#if defined(__cplusplus)
extern "C" {
#endif // defined(__cplusplus)

mach_port_t default_port ();

extern io_registry_entry_t findBrightPlazaDriverInParents(io_registry_entry_t service);
extern io_iterator_t createRecursiveIORegistryIterator(io_registry_entry_t service);
extern io_registry_entry_t findServiceForClassInChildrenBelowIterator(io_iterator_t iterator, const char * driverClassName);
extern io_registry_entry_t findBrightPlazaDriverInChildrenBelowIterator(io_iterator_t iterator);
extern io_registry_entry_t findServiceForClassInChildren(io_registry_entry_t service, const char * driverClassName);
extern io_registry_entry_t findBrightPlazaDriverInChildren(io_registry_entry_t service);

extern io_registry_entry_t findBSDName(const char *bsdName);
extern io_registry_entry_t findParent(io_registry_entry_t service);
extern CFMutableDictionaryRef copyProperties(io_registry_entry_t service);
extern io_iterator_t findMatchingServices(const char * className) ;
extern void GetName(io_registry_entry_t service, char * nameBuffer);

#if defined(__cplusplus)
}
#endif //defined(__cplusplus)


#endif /* RegistryUtilities_h */
