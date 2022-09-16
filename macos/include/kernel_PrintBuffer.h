//
//  kernel_PrintBuffer.h
//  SedUserClient
//
//  Created by Jackie Marks on 2/23/16.
//
//

#ifndef kernel_PrintBuffer_h
#define kernel_PrintBuffer_h

#if DEBUG
static inline int printable(unsigned char c) { return (' '<=c && c<='~'); }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
static void PrintBufferToIOLog( const char * tagstring, const void * buffer, size_t bufferSize )
{
    char outputBuf[200];
    char * outputBufPtr = outputBuf;
    const char * outputBufEnd = outputBuf+sizeof(outputBuf);
    
#define pf(...) \
do { \
    int nPrinted = snprintf(outputBufPtr, (size_t)(outputBufEnd-outputBufPtr), __VA_ARGS__) ; \
    if ( nPrinted < 0 ) { \
        flush();\
        nPrinted = snprintf(outputBufPtr, (size_t)(outputBufEnd-outputBufPtr), __VA_ARGS__) ; \
        if ( nPrinted < 0 ) nPrinted = 0 ; \
    } \
    outputBufPtr += nPrinted ; \
} while (0)
#define flush() do { *outputBufPtr=0; IOLog("%s", outputBuf); outputBufPtr = outputBuf; } while (0)
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
    IOSleep(1);
    for (; i < effectiveBufferSize; i+=16 ) {
        if ( tagstring!=NULL ) pf("%s: ", tagstring);
        pf("%08lx ", i);
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
        pf( "|" );
        flush();
        IOSleep(1);
    }
    // Print an indication of suppressed rows
    if (effectiveBufferSize + 16 <= bufferSize) { pf("*"); flush(); }
    // Finally print actual bufferSize
    pf("%08lx", bufferSize);
    flush();
    IOSleep(1);
}
#pragma clang diagnostic pop
#endif // DEBUG

#if DEBUG
#define IOLOGBUFFER_DEBUG PrintBufferToIOLog
#else
#define IOLOGBUFFER_DEBUG(...) do ; while(0)
#endif

#endif /* kernel_PrintBuffer_h */
