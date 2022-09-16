//
//  ATAProtocolDirections.h
//  SedUserClient
//
//  Created by Jackie Marks on 10/16/15.
//
//

#ifndef ATAProtocolDirections_h
#define ATAProtocolDirections_h
#include "ATAProtocolTypes.h"

#if defined(__cplusplus)
extern "C" {
#endif // defined(__cplusplus)
    


typedef struct tagATA_DIRECTION_CONSTANTS
{
    unsigned int memoryDescriptorDir;
    unsigned char taskCommandDir;
} ATA_DIRECTION_CONSTANTS;


extern ATA_DIRECTION_CONSTANTS ATADirectionConstants[16];

#if defined(__cplusplus)
}
#endif //defined(__cplusplus)



#endif /* ATAProtocolDirections_h */
