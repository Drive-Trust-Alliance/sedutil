//
//  kernel_debug.h
//  SedUserClient
//
//  Created by Jackie Marks on 2/23/16.
//
//

#ifndef kernel_debug_h
#define kernel_debug_h
#include <IOKit/IOLib.h>
#if DEBUG
#define IOLOG_DEBUG IOLog
#else
#define IOLOG_DEBUG(...) do ; while(0)
#endif

#endif /* kernel_debug_h */
