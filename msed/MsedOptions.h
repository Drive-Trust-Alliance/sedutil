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
typedef struct _MSED_OPTIONS {
    uint8_t password;
	uint8_t userid;
	uint8_t newpassword;
	uint8_t pbafile;
    uint8_t device;
    uint8_t action;
	uint8_t mbrstate;
	uint8_t lockingrange;
	uint8_t lockingstate;
} MSED_OPTIONS;
void usage();
uint8_t MsedOptions(int argc, char * argv[], MSED_OPTIONS * opts);
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
#define CHECKARGS(x) \
if((x+baseOptions) != argc) { \
	LOG(E) << "Incorrect number of paramaters for " << argv[i] << " command"; \
	return 100; \
	}
#define BEGIN_OPTION(cmdstring,args) \
				else if (!(strcmp(#cmdstring, &argv[i][2]))) { \
				CHECKARGS(args) \
				opts->action = msedoption::cmdstring; \

#define END_OPTION }
#define TESTARG(literal,structfield,value) \
				if (!(strcmp(#literal, argv[i + 1]))) \
					{opts->structfield = value;} else

#define TESTFAIL(msg) \
	{ \
	LOG(E) << msg << " " << argv[i+1]; \
	return 1;\
	} \
i++;

#define OPTION_IS(option_field) \
				opts->option_field = ++i; 
