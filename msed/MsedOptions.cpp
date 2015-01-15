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
#include "os.h"
#include "MsedOptions.h"
#include "Version.h"
void usage()
{
    printf("msed v%s Copyright 2014,2015 Michael Romeo <r0m30@r0m30.com>\n", GIT_VERSION);
    printf("a utility to manage self encrypting drives that conform\n");
    printf("to the Trusted Computing Group OPAL 2.0 SSC specification\n");
    printf("General Usage:                     (see readme for extended commandset)\n");
    printf("msed <-v> <action> <options> <device>\n");
	printf("-v (optional)	                increase verbosity, one to five v's\n");
    printf("actions \n");
    printf("--scan \n");
    printf("                                Scans the devices on the system \n");
    printf("                                identifying Opal compliant devices \n");
    printf("--query <device>\n");
    printf("                                Display the Discovery 0 response of a device\n");
	printf("--initialsetup <SIDpassword> <device>\n");
	printf("                                Setup the device for use with msed\n");
	printf("                                <SIDpassword> is new SID and Admin1 password\n");
	printf("--setSIDPwd <SIDpassword> <newSIDpassword> <device> \n");
	printf("                                Change the SID password\n");
	printf("--setAdmin1Pwd <Admin1password> <newAdmin1password> <device> \n");
	printf("                                Change the Admin1 password\n");
	printf("--setLockingRange <0...n> <RW|RO|LK> <Admin1password> <device> \n");
	printf("                                Set the status of a Locking Range\n");
	printf("                                0 = GLobal 1..n  = LRn \n");
	printf("--enableLockingRange <0...n> <Admin1password> <device> \n");
	printf("                                Enable a Locking Range\n");
	printf("                                0 = GLobal 1..n  = LRn \n");
	printf("--disableLockingRange <0...n> <Admin1password> <device> \n");
	printf("                                Disable a Locking Range\n");
	printf("                                0 = GLobal 1..n  = LRn \n");
	printf("--setMBREnable <on|off> <Admin1password> <device> \n");
	printf("                                Enable|Disable MBR shadowing \n");
	printf("--setMBRDone <on|off> <Admin1password> <device> \n");
	printf("                                set|unset MBRDone\n");
	printf("--loadPBAimage <Admin1password> <file> <device> \n");
	printf("                                Write <file> to MBR Shadow area\n");
    printf("--reverttper <SIDpassword> <device>\n");
    printf("                                set the device back to factory defaults \n");
	printf("                                This **ERASES ALL DATA** \n");
    printf("--yesIreallywanttoERASEALLmydatausingthePSID <PSID> <device>\n");
    printf("                                revert the device using the PSID *ERASING* *ALL* the data \n");
    printf("\n");
    printf("Examples \n");
    printf("msed --scan \n");
	printf("msed --query %s \n", DEVICEEXAMPLE);
	printf("msed --yesIreallywanttoERASEALLmydatausingthePSID <PSIDALLCAPSNODASHED> %s \n", DEVICEEXAMPLE);
	printf("msed --initialsetup <newSIDpassword> %s \n", DEVICEEXAMPLE);
    return;
}

uint8_t MsedOptions(int argc, char * argv[], MSED_OPTIONS * opts)
{
    memset(opts, 0, sizeof (MSED_OPTIONS));
    uint16_t loggingLevel = 2;
	uint8_t baseOptions = 2; // program and option
    CLog::Level() = CLog::FromInt(loggingLevel);
    if (2 > argc) {
        usage();
        return 1;
    }
	for (uint8_t i = 1; i < argc; i++) {
		if (!(strcmp("-h", argv[i])) || !(strcmp("--help", argv[i]))) {
			usage();
			return 1;
		}
		else if ('v' == argv[i][1])
		{
			baseOptions += 1;
			loggingLevel += (uint16_t)(strlen(argv[i]) - 1);
			if (loggingLevel > 7) loggingLevel = 7;
			CLog::Level() = CLog::FromInt(loggingLevel);
			LOG(D) << "Log level set to " << CLog::ToString(CLog::FromInt(loggingLevel));
			LOG(D) << "msed version : " << GIT_VERSION;
		}
		else if (!(('-' == argv[i][0]) && ('-' == argv[i][1])) && 
			(0 == opts->action))
		{
			LOG(E) << "Argument " << (uint16_t) i << " (" << argv[i] << ") should be a command";
			return 1;
		}
		/* ****************************************************************
		 * General Usage commands should be the only ones used by a
		 * typical end user
		 *******************************************************************/
		BEGIN_OPTION(initialsetup, 2) OPTION_IS(password) OPTION_IS(device) END_OPTION
		BEGIN_OPTION(setSIDPwd, 3) OPTION_IS(password) OPTION_IS(newpassword) OPTION_IS(device) END_OPTION
		BEGIN_OPTION(setAdmin1Pwd, 3) OPTION_IS(password) OPTION_IS(newpassword) OPTION_IS(device) END_OPTION
		BEGIN_OPTION(loadPBAimage, 3) OPTION_IS(password) OPTION_IS(pbafile) OPTION_IS(device) END_OPTION
		BEGIN_OPTION(reverttper, 2) OPTION_IS(password) OPTION_IS(device) END_OPTION
		BEGIN_OPTION(PSIDrevert, 2) OPTION_IS(password) OPTION_IS(device) END_OPTION
		BEGIN_OPTION(yesIreallywanttoERASEALLmydatausingthePSID, 2) OPTION_IS(password) OPTION_IS(device) END_OPTION
		BEGIN_OPTION(enableuser, 2) OPTION_IS(password) OPTION_IS(userid) OPTION_IS(device) END_OPTION
		BEGIN_OPTION(activateLockingSP, 2) OPTION_IS(password) OPTION_IS(device) END_OPTION
		BEGIN_OPTION(query, 1) OPTION_IS(device) END_OPTION
		BEGIN_OPTION(scan,0)  END_OPTION
		BEGIN_OPTION(takeownership, 2) OPTION_IS(password) OPTION_IS(device) END_OPTION
		BEGIN_OPTION(revertLockingSP, 2) OPTION_IS(password) OPTION_IS(device) END_OPTION
		BEGIN_OPTION(setPassword, 4) OPTION_IS(password) OPTION_IS(userid) 
						OPTION_IS(newpassword) OPTION_IS(device) END_OPTION
		BEGIN_OPTION(validatePBKDF2,0) END_OPTION
		BEGIN_OPTION(setMBREnable, 3) 
			TESTARG(ON, mbrstate, 1)
			TESTARG(on, mbrstate, 1)
			TESTARG(off, mbrstate, 0)
			TESTARG(OFF, mbrstate, 0)
			TESTFAIL("Invalid setMBREnable argument not <on|off>")
			OPTION_IS(password)
			OPTION_IS(device) 
		END_OPTION
		BEGIN_OPTION(setMBRDone, 3)
			TESTARG(ON, mbrstate, 1)
			TESTARG(on, mbrstate, 1)
			TESTARG(off, mbrstate, 0)
			TESTARG(OFF, mbrstate, 0)
			TESTFAIL("Invalid setMBRDone argument not <on|off>")
			OPTION_IS(password)
			OPTION_IS(device) 
		END_OPTION
		BEGIN_OPTION(setLockingRange, 4)
			TESTARG(0, lockingrange, 0)
			TESTFAIL("Only the Global Locking Range (0) is currently supported")
			TESTARG(RW, lockingstate, 1)
			TESTARG(rw, lockingstate, 1)
			TESTARG(RO, lockingstate, 2)
			TESTARG(ro, lockingstate, 2)
			TESTARG(LK, lockingstate, 3)
			TESTARG(lk, lockingstate, 3)
			TESTFAIL("Invalid locking state <ro|rw|lk>")
			OPTION_IS(password)
			OPTION_IS(device) 
		END_OPTION
		BEGIN_OPTION(enableLockingRange, 3)
			TESTARG(0, lockingrange, 0)
			TESTFAIL("Only the Global Locking Range (0) is currently supported")
			OPTION_IS(password)
			OPTION_IS(device) 
		END_OPTION
		BEGIN_OPTION(disableLockingRange, 3)
			TESTARG(0, lockingrange, 0)
			TESTFAIL("Only the Global Locking Range (0) is currently supported")
			OPTION_IS(password)
			OPTION_IS(device) 
		END_OPTION

		else if (i != argc - 1) {
            LOG(E) << "Invalid command line argument " << argv[i];
            return 1;
        }
    }
    return 0;
}