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

// This routine should be compiled with the main routine sedutil-cli and the
// appropriate Common/Customizations folder, and not with the sedutil library.

#include "os.h"
#include "log.h"
#include "DtaOptions.h"
#include "DtaLexicon.h"
#include "DtaUsage.h"
#include "Version.h"


#define LOCKINGRANGEARG(lockingrange) \
TESTARG(0, lockingrange, 0)            \
TESTARG(1, lockingrange, 1)            \
TESTARG(2, lockingrange, 2)            \
TESTARG(3, lockingrange, 3)            \
TESTARG(4, lockingrange, 4)            \
TESTARG(5, lockingrange, 5)            \
TESTARG(6, lockingrange, 6)            \
TESTARG(7, lockingrange, 7)            \
TESTARG(8, lockingrange, 8)            \
TESTARG(9, lockingrange, 9)            \
TESTARG(10, lockingrange, 10)          \
TESTARG(11, lockingrange, 11)          \
TESTARG(12, lockingrange, 12)          \
TESTARG(13, lockingrange, 13)          \
TESTARG(14, lockingrange, 14)          \
TESTARG(15, lockingrange, 15)          \
TESTFAIL("Invalid Locking Range (0-15)")

#define MBRSTATEARG(arg,mbrstate) \
TESTARG(ON, mbrstate, 1)      \
TESTARG(on, mbrstate, 1)      \
TESTARG(Y, mbrstate, 1)       \
TESTARG(y, mbrstate, 1)       \
TESTARG(n, mbrstate, 0)       \
TESTARG(N, mbrstate, 0)       \
TESTARG(off, mbrstate, 0)     \
TESTARG(OFF, mbrstate, 0)     \
TESTFAIL("Invalid " #arg "argument not <ON|on|Y|y|OFF|off|N|n>")

#define LOCKINGSTATEARG(lockingstate) \
TESTARG(RW, lockingstate, OPAL_LOCKINGSTATE::READWRITE) \
TESTARG(rw, lockingstate, OPAL_LOCKINGSTATE::READWRITE) \
TESTARG(RO, lockingstate, OPAL_LOCKINGSTATE::READONLY)  \
TESTARG(ro, lockingstate, OPAL_LOCKINGSTATE::READONLY)  \
TESTARG(LK, lockingstate, OPAL_LOCKINGSTATE::LOCKED)    \
TESTARG(lk, lockingstate, OPAL_LOCKINGSTATE::LOCKED)    \
TESTFAIL("Invalid locking state <RW|rw|RO|ro|LK|lk>")

#define TCGRESETTYPEARG(resettype) \
TESTARG(0, resettype, 0) \
TESTARG(1, resettype, 1) \
TESTARG(2, resettype, 2) \
TESTARG(3, resettype, 3) \
TESTFAIL("Invalid TCGreset argument not <0|1|2|3>")

uint8_t DtaOptions(int argc, char * argv[], DTA_OPTIONS * opts)
{
    memset(opts, 0, sizeof (DTA_OPTIONS));
    opts->output_format = DEFAULT_OUTPUT_FORMAT;
    uint16_t loggingLevel = DEFAULT_LOGGING_LEVEL;
    uint8_t baseOptions = 2; // program and option
    CLogLevel = CLog::FromInt(loggingLevel);
    RCLogLevel = RCLog::FromInt(loggingLevel);
    if (2 > argc) {
        usage();
        return DTAERROR_INVALID_COMMAND;
    }
    for (uint8_t i = 1; i < argc; i++) {
        if (!(strcmp("-h", argv[i])) || !(strcmp("--help", argv[i]))) {
            usage();
            return DTAERROR_INVALID_COMMAND;
        }

        if ('v' == argv[i][1]) {
            // logging level set to length of any arg staring with 'v'
          baseOptions += 1;
          loggingLevel += (uint16_t)(strlen(argv[i]) - 1);
          if (loggingLevel > MAX_LOGGING_LEVEL) loggingLevel = MAX_LOGGING_LEVEL;
          CLogLevel = CLog::FromInt(loggingLevel);
          RCLogLevel = RCLog::FromInt(loggingLevel);
          LOG(D) << "Log level set to " << CLog::ToString(CLog::FromInt(loggingLevel));
          LOG(D) << "sedutil version : " << GIT_VERSION;
        } else if (!(strcmp("-a", argv[i]))) {
          baseOptions += 1;
          opts->skip_activate = true;
          LOG(D) << "Do not activate LockingSP";
        } else if (!(strcmp("-u", argv[i]))) {
          baseOptions += 1;
          opts->usermode = true;
          LOG(D) << "user mode ON";
        } else if (!(strcmp("-t", argv[i]))) {
          baseOptions += 1;
          opts->translate_req = true;
          LOG(D) << "translate hashed string to data";
        } else if (!(strcmp("-n", argv[i]))) {
          baseOptions += 1;
          opts->no_hash_passwords = true;
          LOG(D) << "Password hashing is disabled";
        } else if (!strcmp("-l", argv[i])) {
          baseOptions += 1;
          opts->output_format = sedutilNormal;
          outputFormat = sedutilNormal;
        } else if (!(('-' == argv[i][0]) && ('-' == argv[i][1])) &&
                                            (0 == opts->action)) {
          LOG(E) << "Argument " << (uint16_t) i << " (" << argv[i] << ") should be a command";
          return DTAERROR_INVALID_COMMAND;
        }

#include "DtaOptions.inc"

#include "Customizations/DtaExtensionOptions.inc"

        else {
            LOG(E) << "Invalid command line argument " << argv[i];
			return DTAERROR_INVALID_COMMAND;
        }
    }
    return DTAERROR_SUCCESS;
}
