//
//  debug.h
//  SedUserClient
//
//  Created by Jackie Marks on 2/23/16.
//
//

#ifndef debug_h
#define debug_h

#if DEBUG
#define IOLOG_DEBUG IOLog
#define NSLOG_DEBUG NSLog
#define loggingLevel 0
#else
#define IOLOG_DEBUG(...) do ; while(0)
#define NSLOG_DEBUG(...) do ; while(0)
#define loggingLevel 0
#endif

//#if DEBUG
//static inline void debugIO(format, fn_name, ...)
//{
//    char tagstring[256];
//    int header_length;
//    va_list a_list;
//    va_start(a_list, format);
//    header_length=snprintf( tagstring, sizeof(tagstring), "%s[%p]::%s] ", getName(), this, __FUNCTION__);
//    vsnprintf(&tagstring[header_length], sizeof(tagstring)-header_length, format, a_list);
//    va_end(a_list);
//    IOLog("%s", tagstring);
//}
//#endif



#endif /* debug_h */
