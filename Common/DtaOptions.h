/* C:B**************************************************************************
This software is © 2014 Bright Plaza Inc. <drivetrust@drivetrust.com>

This file is part of sedutil.

sedutil is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

sedutil is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sedutil.  If not, see <http://www.gnu.org/licenses/>.

* C:E********************************************************************** */

#if !(defined(DTAOPTIONS_INCLUDED_FOR_CODE) || defined(DTAOPTIONS_INCLUDED_FOR_USAGE))
#define DTAOPTIONS_INCLUDED_FOR_CODE  // assume legacy code
#endif

#if defined(DTAOPTIONS_INCLUDED_FOR_CODE)
#ifndef _DTAOPTIONS_H
#define	_DTAOPTIONS_H


#include <cstdint>

#include "log.h"

#if DEBUG && defined(AUTOMATICALLY_BUMP_LOGGING_LEVEL_IN_DEBUG_BUILDS)
#define DEFAULT_LOGGING_LEVEL 3
#else // !DEBUG
#define DEFAULT_LOGGING_LEVEL 2
#endif // DEBUG
#define MAX_LOGGING_LEVEL 7


/** Structure representing the command line issued to the program */
typedef struct _DTA_OPTIONS {
    uint8_t password;             /**< password supplied */
    uint8_t userid;               /**< userid supplied */
    uint8_t newpassword;          /**< new password for password change */
    uint8_t pbafile;              /**< file name for loadPBAimage command */
    uint8_t device;               /**< device name  */
    uint8_t action;               /**< option requested */
    uint8_t mbrstate;             /**< mbrstate for set mbr commands */
    uint8_t lockingrange;         /**< locking range to be manipulated */
    uint8_t lockingstate;         /**< locking state to set a lockingrange to */
    uint8_t lrstart;	          /**< the starting block of a lockingrange */
    uint8_t lrlength;	          /**< the length in blocks of a lockingrange */
    uint8_t resettype;            /**< type of TCG reset */
    bool no_hash_passwords;       /**< global parameter, disables hashing of passwords */
    bool usermode;                /**< true : start session with UserN UID, otherwise with AdminN UID */
    uint8_t devusb;               /**< usb devname */
    bool    translate_req;        /**< global parameter, require to translate the hashed password string into hex data */
    bool    skip_activate;        /**< skip activation LockingSP during initial setup */
    sedutiloutput output_format;  /**< global parameter, output format */

    uint8_t eventid;	          /**< audit log event ID */
    uint8_t dsnum;	          /**< which data store to read write*/
    uint32_t startpos;	          /**< data store start position  */
    uint32_t len;	          /**< data store length */
} DTA_OPTIONS;

/** Print a usage message */
extern void usage();
/** Parse the command line and return a structure that describes the action desired
 * @param argc program argc parameter
 * @param argv program argv paramater
 * @param opts pointer to options structure to be filled out
 */
uint8_t DtaOptions(int argc, char * argv[], DTA_OPTIONS * opts);
/** Command line options implemented in sedutil */
typedef enum _sedutiloption {
    deadbeef,    // 0 should indicate no action specified
    initialSetup,
    setSIDPassword,
    setup_SUM,
    setAdmin1Pwd,
    setPassword,
    setPassword_SUM,
    activate,
    getmfgstate,
    loadPBAimage,
    setLockingRange,
    revertTPer,
    revertNoErase,
    setLockingRange_SUM,
    revertLockingSP,
    PSIDrevert,
    PSIDrevertAdminSP,
    yesIreallywanttoERASEALLmydatausingthePSID,
    enableLockingRange,
    disableLockingRange,
    readonlyLockingRange,
    setupLockingRange,
    setupLockingRange_SUM,
    listLockingRanges,
    listLockingRange,
    rekeyLockingRange,
    setBandsEnabled,
    setBandEnabled,
    setMBREnable,
    setMBRDone,
    TCGreset,
    enableuser,
    enableuserread,
    activateLockingSP,
    activateLockingSP_SUM,
    eraseLockingRange_SUM,
    query,
    scan,
    isValidSED,
    eraseLockingRange,
    takeOwnership,
    validatePBKDF2,
    objDump,
    printDefaultPassword,
    rawCmd,
    version,
    hashvalidation,

    // Predefined slots in this enumeration allow common compilation
    // linked with later customization.
    extensionOption01,
    extensionOption02,
    extensionOption03,
    extensionOption04,
    extensionOption05,
    extensionOption06,
    extensionOption07,
    extensionOption08,
    extensionOption09,
    extensionOption10,

    extensionOption11,
    extensionOption12,
    extensionOption13,
    extensionOption14,
    extensionOption15,
    extensionOption16,
    extensionOption17,
    extensionOption18,
    extensionOption19,
    extensionOption20,

} sedutiloption;

/** verify the number of arguments passed */
#define CHECKARGS(x) \
            if((x+baseOptions) != argc) { \
                LOG(E) << "Incorrect number of parameters for " << argv[i] << " command"; \
            return 100; \
        }

#define CHECKMAXARGS(x) \
            if((x+baseOptions) < argc) { \
                LOG(E) << "Too many parameters for " << argv[i] << " command"; \
            return 100; \
        }

/** Test the command input for a recognized argument */
#define BEGIN_OPTION(cmdstring,args) \
        else if (!(strcasecmp(#cmdstring, &argv[i][2]))) { \
            CHECKARGS(args) \
            opts->action = sedutiloption::cmdstring; \

#define BEGIN_OPTION_MAX(cmdstring,args) \
        else if (!(strcasecmp(#cmdstring, &argv[i][2]))) { \
            CHECKMAXARGS(args) \
            opts->action = sedutiloption::cmdstring; \

/** end of an OPTION */
#define END_OPTION }
/** test an argument for a value */
#define TESTARG(literal,structfield,value) \
    if (!(strcasecmp(#literal, argv[i + 1]))) \
            {opts->structfield = value;} \
        else
/** test an argument for a value */
#define TESTOPTIONALARG(literal,structfield,value) \
    if (i+1<argc && !(strcasecmp(#literal, argv[i + 1]))) \
            {opts->structfield = value; i++;} \
        else
#define ENDOPTIONALARGS ;

/** if all testargs fail then do this */
#define TESTFAIL(msg) \
            { \
	        LOG(E) << msg << " " << argv[i+1]; \
	        return 1;\
	    } \
        i++;

/** set the argc value for this parameter in the options structure */
#define OPTION_IS(option_field) \
	opts->option_field = ++i;

#define SKIP_FIELDS(n) i += n;

#define USAGE_IS(u)

#endif /* _DTAOPTIONS_H */

#elif defined(DTAOPTIONS_INCLUDED_FOR_USAGE)
#define CHECKARGS(x)
#define CHECKMAXARGS(x)
#define BEGIN_OPTION(cmdstring,args)
#define BEGIN_OPTION_MAX(cmdstring,args)
#define END_OPTION
#define TESTARG(literal,structfield,value)
#define TESTOPTIONALARG(literal,structfield,value)
#define ENDOPTIONALARGS
#define TESTFAIL(msg)
#define OPTION_IS(option_field)
#define SKIP_FIELDS(n)

#define USAGE_IS(u) u,


#else
#error DtaOptions.h must be #included with either #define DTAOPTIONS_INCLUDED_FOR_CODE or #define DTAOPTIONS_INCLUDED_FOR_USAGE
#endif  // configuration
