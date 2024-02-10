/* C:B**************************************************************************
This software is Copyright 2014-2017 Bright Plaza Inc. <drivetrust@drivetrust.com>

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

#ifndef _DTAOPTIONS_H
#define	_DTAOPTIONS_H
#include <cstdint>

/** Output modes */
typedef enum _sedutiloutput {
	sedutilNormal,
	sedutilReadable,
	sedutilJSON
} sedutiloutput;

/** Structure representing the command line issued to the program */
typedef struct _DTA_OPTIONS {
    uint8_t password;   /**< password supplied */
	uint8_t userid;   /**< userid supplied */
	uint8_t newpassword;   /**< new password for password change */
	uint8_t pbafile;   /**< file name for loadPBAimage command */
    uint8_t device;   /**< device name  */
    uint8_t action;   /**< option requested */
	uint8_t mbrstate;   /**< mbrstate for set mbr commands */
	uint8_t lockingrange;  /**< locking range to be manipulated */
	uint8_t lockingstate;  /**< locking state to set a lockingrange to */
	uint8_t lrstart;		/** the starting block of a lockingrange */
	uint8_t lrlength;		/** the length in blocks of a lockingrange */

	bool no_hash_passwords; /** global parameter, disables hashing of passwords */
    bool hex_passwords; /** global parameter, all incoming passwords are treated as hex-encoded */
	sedutiloutput output_format;
} DTA_OPTIONS;
/** Print a usage message */
void usage();
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
	enableuser,
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
    printPasswordHash,
    prepareForS3Sleep,
	rawCmd,

} sedutiloption;
/** verify the number of arguments passed */
#define CHECKARGS(x) \
if((x+baseOptions) != argc) { \
	LOG(E) << "Incorrect number of paramaters for " << argv[i] << " command"; \
	return 100; \
	}
/** Test the command input for a recognized argument */
#define BEGIN_OPTION(cmdstring,args) \
				else if (!(strcasecmp(#cmdstring, &argv[i][2]))) { \
				CHECKARGS(args) \
				opts->action = sedutiloption::cmdstring; \

/** end of an OPTION */
#define END_OPTION }
/** test an argument for a value */
#define TESTARG(literal,structfield,value) \
				if (!(strcasecmp(#literal, argv[i + 1]))) \
					{opts->structfield = value;} else
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

#endif /* _DTAOPTIONS_H */
