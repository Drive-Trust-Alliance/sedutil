//
//  log.cpp
//  SED
//
//  Created by Scott Marks on 09/20/2017.
//
//

#include "../Common/log.h"

#define DEBUG_LOGGING_LEVEL 0
//#define DEBUG_LOGGING_LEVEL 4
//#define DEBUG_LOGGING_LEVEL 0
#define RELEASE_LOGGING_LEVEL 0


template<> TLogLevel& Log<Output2FILE>::Level() {
    static TLogLevel Level = D4;
    return Level;
}

void SetLoggingLevel(const int loggingLevel)
{
    CLog::Level() = CLog::FromInt(loggingLevel);
    RCLog::Level() = RCLog::FromInt(loggingLevel);
}

void turnOffLogging(void) {
#if DEBUG
    SetLoggingLevel(DEBUG_LOGGING_LEVEL);
#else // DEBUG
    SetLoggingLevel(RELEASE_LOGGING_LEVEL);
#endif // DEBUG
}

