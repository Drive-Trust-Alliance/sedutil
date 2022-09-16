//
//  ATAProtocolTypes.h
//  SedUserClient
//
//  Created by Jackie Marks on 10/20/15.
//
//

#ifndef ATAProtocolTypes_h
#define ATAProtocolTypes_h

typedef enum {
    HARD_RESET,             //  0
    SRST,                   //  1
    RESERVED1,              //  2
    NON_DATA,               //  3
    PIO_DATA_IN,            //  4
    PIO_DATA_OUT,           //  5
    DMA,                    //  6
    DMA_QUEUED,             //  7
    DEVICE_DIAGNOSTIC,      //  8
    DEVICE_RESET,           //  9
    UDMA_DATA_IN,           // 10
    UDMA_DATA_OUT,          // 11
    FPDMA,                  // 12
    RESERVED2,              // 13
    RESERVED3,              // 14
    RETURN_RESPONSE_INFO    // 15
} ATA_PROTOCOL_TYPE;



#endif /* ATAProtocolTypes_h */
