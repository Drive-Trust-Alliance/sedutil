//
//  PrintBuffer.c
//  SedUserClient
//
//  Created by Jackie Marks on 10/5/15.
//
//

//#include <ctype.h>
#include "PrintBuffer.h"

// Used instead of isprint to avoid dynamic linking to libc.dylib -- drivers don't like dynamic linking
static inline int printable(unsigned char c) { return (' '<=c && c<='~'); }

void _internalPrintBuffer( printfn pf, const char * tagstring, const void * buffer, size_t bufferSize )
{
    const unsigned char * b = (unsigned char *)buffer;
    // Print out a buffer in canonical Ascii style (very similar to hexdump -C).
    // We are plagued by long buffers with mostly zeros.
    // Start by finding the last non-zero value in the buffer,
    // but keep at least one row,
    size_t effectiveBufferSize = bufferSize;
    while (16<--effectiveBufferSize && 0==b[effectiveBufferSize]) continue;
    // round up to an even number of rows,
    effectiveBufferSize = ((effectiveBufferSize + 15)/16)*16;
    // don't round up past the actual bufferSize.
    if (bufferSize<effectiveBufferSize) effectiveBufferSize=bufferSize;

    size_t i=0;
    for (; i < effectiveBufferSize; i+=16 ) {
        if ( tagstring!=NULL ) pf("%s: ", tagstring);
        pf("%08x ", i);
        for (size_t j = i ; j < i+16 ; j++ ) {
            if ( j < effectiveBufferSize) {
                unsigned char c = b[j];
                pf( " %02x", c ) ;
            } else {
                pf( "   " );
            }
            if ( j == i + 7 ) {
                pf( " " ) ;
            }
        }
        pf( "  |" ) ;
        for (size_t j = i ; j < i+16 ; j++ ) {
            if ( j < effectiveBufferSize ) {
                unsigned char c = b[j];
                pf( "%c", printable(c)? c : '.' );
            }
        }
        pf( "|\n" );
    }
    // Print an indication of suppressed rows
    if (effectiveBufferSize + 16 <= bufferSize) pf("*\n");
    // Finally print actual bufferSize
    pf("%08x\n", bufferSize);
}
