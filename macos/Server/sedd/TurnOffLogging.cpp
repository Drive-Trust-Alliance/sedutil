//
//  TurnOffLogging.cpp
//  msed
//
//  Created by Jackie Marks on 6/28/16.
//  Copyright © 2016 Bright Plaza Inc. All rights reserved.
//

#include "TurnOffLogging.hpp"
#include <sedutil/sedutil.h>

#define DEBUG_LOGGING_LEVEL 1
//#define DEBUG_LOGGING_LEVEL 4
//#define DEBUG_LOGGING_LEVEL 0
#define RELEASE_LOGGING_LEVEL 0

void SEDSetLoggingLevel(int loggingLevel)
{
    CLog::Level() = CLog::FromInt(loggingLevel);
}

void turnOffLogging(void) {
    
#if DEBUG
    DtaSetLoggingLevel(DEBUG_LOGGING_LEVEL);
    SEDSetLoggingLevel(DEBUG_LOGGING_LEVEL);
#else // DEBUG
    DtaSetLoggingLevel(RELEASE_LOGGING_LEVEL);
    SEDSetLoggingLevel(RELEASE_LOGGING_LEVEL);
#endif // DEBUG
    
    
    
}
