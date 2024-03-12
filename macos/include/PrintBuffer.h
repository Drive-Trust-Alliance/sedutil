//
//  PrintBuffer.h
//  SedUserClient
//
//  Created by Jackie Marks on 10/5/15.
//
//

#ifndef PrintBuffer_h
#define PrintBuffer_h

#if defined(__cplusplus)
extern "C" {
#endif // defined(__cplusplus)


#if SED_KERNEL
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-conversion"
#include <libkern/libkern.h>
#pragma clang diagnostic pop
#include <IOKit/IOLib.h>
    typedef void (*printfn)(const char * __restrict, ...) ;
    extern void _internalPrintBuffer( printfn pf, const char * tagstring, const void * buffer, size_t bufferSize );
#else  // !SED_KERNEL
#include <stdio.h>
    typedef int	(*printfn)(const char * __restrict, ...) __printflike(1, 2);
    extern void _internalPrintBuffer( printfn pf, const char * tagstring, const void * buffer, size_t bufferSize );
#endif // SED_KERNEL


#if DEBUG

#if SED_KERNEL
#define IOLogBuffer(tagstring,buffer,bufferSize) _internalPrintBuffer(IOLog,tagstring,buffer,bufferSize)
#else
#define printBuffer(buffer,bufferSize) _internalPrintBuffer(printf,NULL,buffer,bufferSize)
#endif

#else // !DEBUG

#if SED_KERNEL
#define IOLogBuffer(tagstring,buffer,bufferSize) do ; while(0) ;
#else
#define printBuffer(buffer,bufferSize) do ; while(0) ;
#endif

#endif // DEBUG



#if defined(__cplusplus)
}
#endif //defined(__cplusplus)

#endif /* PrintBuffer_h */
