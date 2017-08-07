/*
 * This software is Copyright 2007 Petru Marginean <petru.marginean@gmail.com>
 *
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org/>
 *
 *
 * simple logging class discussed in Dr Dobs journal
 * http://www.drdobbs.com/cpp/logging-in-c/201804215
 *
 * changed the naming a little, converted to safe printf's
 * and added IFLOG() macro to support calling procedure only
 * if we are at/above a specific log level
 */

#ifndef __LOG_H__
#define __LOG_H__

#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "DtaOptions.h"

inline std::string NowTime();

enum TLogLevel {
    E, W, I, D, D1, D2, D3, D4
};

template <typename T>
class Log {
public:
    Log();
    virtual ~Log();
    std::ostringstream& Get(TLogLevel level = I);
public:
    static TLogLevel& Level();
    static std::string ToString(TLogLevel level);
    static TLogLevel FromString(const std::string& level);
    static TLogLevel FromInt(const int level);
protected:
    std::ostringstream os;
private:
    Log(const Log&);
    Log& operator =(const Log&);
};

template <typename T>
Log<T>::Log() {
}

template <typename T>
std::ostringstream& Log<T>::Get(TLogLevel level) {
    os << "- " << NowTime();
    os << " " << ToString(level) << ": ";
    //	os << std::string(level > D ? level - D : 0, '\t');
    return os;
}

template <typename T>
Log<T>::~Log() {
    os << std::endl;
    T::Output(os.str());
}

template <typename T>
TLogLevel& Log<T>::Level() {
    static TLogLevel Level = D4;
    return Level;
}

template <typename T>
std::string Log<T>::ToString(TLogLevel level) {
    static const char* const buffer[] = {"ERR ", "WARN", "INFO", "DBG ", "DBG1", "DBG2", "DBG3", "DBG4"};
    return buffer[level];
}

template <typename T>
TLogLevel Log<T>::FromString(const std::string& level) {
    if (level == "DEBUG4")
        return D4;
    if (level == "DEBUG3")
        return D3;
    if (level == "DEBUG2")
        return D2;
    if (level == "DEBUG1")
        return D1;
    if (level == "DEBUG")
        return D;
    if (level == "INFO")
        return I;
    if (level == "WARN")
        return W;
    if (level == "ERROR")
        return E;
    Log<T>().Get(W) << "Unknown logging level '" << level << "'. Using INFO level as default.";
    return I;
}

template <typename T>
TLogLevel Log<T>::FromInt(const int level) {
    if (level == 7)
        return D4;
    if (level == 6)
        return D3;
    if (level == 5)
        return D2;
    if (level == 4)
        return D1;
    if (level == 3)
        return D;
    if (level == 2)
        return I;
    if (level == 1)
        return W;
    if (level == 0)
        return E;
    Log<T>().Get(W) << "Unknown logging level '" << level << "'. Using INFO level as default.";
    return I;
}

template <typename T>
class RLog {
public:
    RLog();
    virtual ~RLog();
    std::ostringstream& Get(TLogLevel level = I, sedutiloutput format = sedutilReadable);
public:
    static TLogLevel& Level();
    static std::string ToString(TLogLevel level);
    static TLogLevel FromString(const std::string& level);
    static TLogLevel FromInt(const int level);
protected:
    std::ostringstream os;
private:
    RLog(const RLog&);
    RLog& operator =(const RLog&);
    TLogLevel curlevel;
    sedutiloutput outputformat;
};

template <typename T>
RLog<T>::RLog() {
}

template <typename T>
std::ostringstream& RLog<T>::Get(TLogLevel level, sedutiloutput output_format) {
	curlevel = level;
	outputformat = output_format;
	if (output_format == sedutilNormal) {
		os << "- " << NowTime();
		os << " " << ToString(level) << ": ";
	}
    //	os << std::string(level > D ? level - D : 0, '\t');
    return os;
}

template <typename T>
RLog<T>::~RLog() {
    os << std::endl;
	if ((curlevel == I) && (outputformat != sedutilNormal))
		T::Output(os.str());
	else
		T::OutputErr(os.str());
}

template <typename T>
TLogLevel& RLog<T>::Level() {
    static TLogLevel Level = D4;
    return Level;
}

template <typename T>
std::string RLog<T>::ToString(TLogLevel level) {
    static const char* const buffer[] = {"ERR ", "WARN", "INFO", "DBG ", "DBG1", "DBG2", "DBG3", "DBG4"};
    return buffer[level];
}

template <typename T>
TLogLevel RLog<T>::FromString(const std::string& level) {
    if (level == "DEBUG4")
        return D4;
    if (level == "DEBUG3")
        return D3;
    if (level == "DEBUG2")
        return D2;
    if (level == "DEBUG1")
        return D1;
    if (level == "DEBUG")
        return D;
    if (level == "INFO")
        return I;
    if (level == "WARN")
        return W;
    if (level == "ERROR")
        return E;
    RLog<T>().Get(W, sedutilNormal) << "Unknown logging level '" << level << "'. Using INFO level as default.";
    return I;
}

template <typename T>
TLogLevel RLog<T>::FromInt(const int level) {
    if (level == 7)
        return D4;
    if (level == 6)
        return D3;
    if (level == 5)
        return D2;
    if (level == 4)
        return D1;
    if (level == 3)
        return D;
    if (level == 2)
        return I;
    if (level == 1)
        return W;
    if (level == 0)
        return E;
    RLog<T>().Get(W, sedutilNormal) << "Unknown logging level '" << level << "'. Using INFO level as default.";
    return I;
}


class Output2FILE {
public:
    static FILE*& Stream();
    static FILE*& StreamStdout();
    static void Output(const std::string& msg);
    static void OutputErr(const std::string& msg);
};

inline FILE*& Output2FILE::StreamStdout() {
    static FILE* pStream = stdout;
    return pStream;
}

inline FILE*& Output2FILE::Stream() {
    static FILE* pStream = stderr;
    return pStream;
}

inline void Output2FILE::OutputErr(const std::string& msg) {
    FILE* pStream = Stream();
    if (!pStream)
        return;
    fprintf(pStream, "%s", msg.c_str());
    fflush(pStream);
}

inline void Output2FILE::Output(const std::string& msg) {
    FILE* pStream = StreamStdout();
    if (!pStream)
        return;
    fprintf(pStream, "%s", msg.c_str());
    fflush(pStream);
}

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#if defined (BUILDING_FILELOG_DLL)
#define FILELOG_DECLSPEC   __declspec (dllexport)
#elif defined (USING_FILELOG_DLL)
#define FILELOG_DECLSPEC   __declspec (dllimport)
#else
#define FILELOG_DECLSPEC
#endif // BUILDING_DBSIMPLE_DLL
#else
#define FILELOG_DECLSPEC
#endif // _WIN32

class FILELOG_DECLSPEC CLog : public Log<Output2FILE> {
};
//typedef Log<Output2FILE> FILELog;

class FILELOG_DECLSPEC RCLog : public RLog<Output2FILE> {
};

#ifndef CLOG_MAX_LEVEL
#define CLOG_MAX_LEVEL D4
#endif

#if 0
#define LOG(level) \
	if (level > CLOG_MAX_LEVEL) ;\
	else if (level > CLog::Level() || !Output2FILE::Stream()) ; \
	else CLog().Get(level)
#endif

#define IFLOG(level) \
	if (level > CLOG_MAX_LEVEL) ;\
	else if (level > CLog::Level() || !Output2FILE::Stream()) ; \
	else

extern sedutiloutput outputFormat;

#define	LOGX(level) \
	if (level > CLOG_MAX_LEVEL) ;\
	else if (level > RCLog::Level() || !Output2FILE::Stream()) ; \
	else RCLog().Get(level, outputFormat)
#define	LOG LOGX

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)

#include <windows.h>
#include <cstdlib>

inline std::string NowTime() {
    const int MAX_LEN = 200;
    char buffer[MAX_LEN];
    if (GetTimeFormatA(LOCALE_USER_DEFAULT, 0, 0,
            "HH':'mm':'ss", buffer, MAX_LEN) == 0)
        return "Error in NowTime()";

    char result[100] = {0};
    static DWORD first = GetTickCount();
    sprintf_s(result, 99, "%s.%03ld", buffer, (long) (GetTickCount() - first) % 1000);
    return result;
}

#else

#include <sys/time.h>

inline std::string NowTime() {
    char buffer[11];
    time_t t;
    time(&t);
    tm r = {0};
    strftime(buffer, sizeof (buffer), "%X", localtime_r(&t, &r));
    struct timeval tv;
    gettimeofday(&tv, 0);
    char result[100] = {0};
    snprintf(result, 95, "%s.%03ld", buffer, (long) tv.tv_usec / 1000);
    return result;
}

#endif //WIN32

#endif //__LOG_H__
