/* C:B**************************************************************************
This software is Copyright 2014,2015 Michael Romeo <r0m30@r0m30.com>

This file is part of msed.

msed is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

msed is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with msed.  If not, see <http://www.gnu.org/licenses/>.

* C:E********************************************************************** */
/** Structure representing the command line issued to the program */
typedef struct _MSED_OPTIONS {
    uint8_t password;   /**< password supplied */
	uint8_t userid;   /**< userid supplied */
	uint8_t newpassword;   /**< new password for password change */
	uint8_t pbafile;   /**< file name for loadPBAimage command */
    uint8_t device;   /**< device name  */
    uint8_t action;   /**< msedoption requested */
	uint8_t mbrstate;   /**< mbrstate for set mbr commands */
	uint8_t lockingrange;  /**< locking range to be manipulated */
	uint8_t lockingstate;  /**< locking state to set a lockingrange to */
} MSED_OPTIONS;
/** Print a usage message */
void usage();
/** Parse the command line and return a structure that describes the action desired
 * @param argc program argc parameter 
 * @param argv program argv paramater
 * @param opts pointer to options structure to be filled out
 */
uint8_t MsedOptions(int argc, char * argv[], MSED_OPTIONS * opts);
/** Command line options implemented in msed */
typedef enum _msedoption {
	deadbeef,    // 0 should indicate no action specified
	initialsetup,
	setSIDPwd,
	setAdmin1Pwd,
	setPassword,
	loadPBAimage,
	setLockingRange,
	reverttper,
	revertLockingSP,
	PSIDrevert,
	yesIreallywanttoERASEALLmydatausingthePSID,
	enableLockingRange,
	disableLockingRange,
	setMBREnable,
	setMBRDone,
	enableuser,
	activateLockingSP,
	query,
	scan,
	takeownership,
	validatePBKDF2,
	objDump,
	rawCmd,

} msedoption;
/** verify the number of arguments passed */
#define CHECKARGS(x) \
if((x+baseOptions) != argc) { \
	LOG(E) << "Incorrect number of paramaters for " << argv[i] << " command"; \
	return 100; \
	}
/** Test the command input for a recognized argument */
#define BEGIN_OPTION(cmdstring,args) \
				else if (!(strcmp(#cmdstring, &argv[i][2]))) { \
				CHECKARGS(args) \
				opts->action = msedoption::cmdstring; \

/** end of an OPTION */
#define END_OPTION }
/** test an argument for a value */
#define TESTARG(literal,structfield,value) \
				if (!(strcmp(#literal, argv[i + 1]))) \
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
