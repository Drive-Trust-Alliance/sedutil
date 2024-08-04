//
//  log.cpp
//  SED
//
//  Created by Scott Marks on 09/20/2017.
//
//

#include "log.h"


/* Default to output that omits timestamps and goes to stdout */
sedutiloutput outputFormat = DEFAULT_OUTPUT_FORMAT;


#define DEBUG_LOGGING_LEVEL 0
#define RELEASE_LOGGING_LEVEL 0

TLogLevel& CLogLevel = CLog::Level();
TLogLevel& RCLogLevel = RCLog::Level();

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

