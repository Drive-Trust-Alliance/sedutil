/* C:B**************************************************************************
This software is Copyright 2014 Michael Romeo <r0m30@r0m30.com>

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

void usage()
{
    printf("msed v%s Copyright 2014 Michael Romeo <r0m30@r0m30.com>\n", MSED_VERSION);
    printf("a utility to manage self encrypting drives that conform\n");
    printf("to the Trusted Computing Group OPAL 2.0 SSC specification\n");
    printf("General Usage:                     (see readme for extended commandset)\n");
    printf("msed <-v> <action> <options> <device>\n");
	printf("-v (optional)	                increase verbosity, one to five v's\n");
    printf("actions \n");
    printf("--scan \n");
    printf("                                Scans the devices on the system \n");
    printf("                                identifying Opal 2.0 compliant devices \n");
    printf("--query <device>\n");
    printf("                                Display the Discovery 0 response of a device\n");
	printf("--initialsetup <SIDpassword> <device>\n");
	printf("                                Setup the device for use with msed\n");
	printf("                                <SIDpassword> is new SID and Admin1 password\n");
	printf("--setSIDPwd <SIDpassword> <newSIDpassword> <device> \n");
	printf("                                Change the SID password\n");
	printf("--setAdmin1Pwd <Admin1password> <newAdmin1password> <device> \n");
	printf("                                Change the Admin1 password\n");
	printf("--setLR <0...n> <RW|RO|LK> <Admin1password> <device> \n");
	printf("                                Set the status of a Locking Range\n");
	printf("                                0 = GLobal 1..n  = LRn \n");
	printf("--setMBREnable <on|off> <Admin1password> <device> \n");
	printf("                                Enable|Disable MBR shadowing \n");
	printf("--setMBRDone <on|off> <Admin1password> <device> \n");
	printf("                                set|unset MBRDone\n");
	printf("--loadPBAimage <SIDpassword> <file> <device> \n");
	printf("                                Write <file> to MBR Shadow area\n");
    printf("--revert <SIDpassword> <device>\n");
    printf("                                set the device back to factory defaults \n");
	printf("                                This **ERASES ALL DATA** \n");
    //printf("--revertnoerase <SIDpassword> <Admin1password> <device> \n");
    //printf("                                revert the drive without erasing the data \n");
    printf("--PSIDrevert <PSID> <device>\n");
    printf("                                revert the device using the PSID *ERASING* *ALL* the data \n");
    printf("\n");
    printf("Examples \n");
    printf("msed --scan \n");
    printf("msed --initialize newSIDpassword \\\\.\\PhysicalDisk2 \n");
    printf("msed --revert SIDPassword /dev/sg0 \n");
    return;
}

uint8_t MsedOptions(int argc, char * argv[], MSED_OPTIONS * opts)
{
    memset(opts, 0, sizeof (MSED_OPTIONS));
    uint16_t loggingLevel = 2;
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
		/* ****************************************************************
		 * General Usage commands should be the only ones uded by a
		 * typical end user
		 *******************************************************************/
		else if (!(strcmp("--initialsetup", argv[i]))) {
			opts->action = 0x01;
			opts->password = ++i;
		}
		else if (!(strcmp("--setSIDPwd", argv[i]))) {
			opts->action = 0x02;
			opts->password = ++i;
			opts->newpassword = ++i;
		}
		else if (!(strcmp("--setAdmin1Pwd", argv[i]))) {
			opts->password = ++i;
			opts->newpassword = ++i;
			opts->action = 0x03;
		}
		else if (!(strcmp("--setMBREnable", argv[i]))) {
			if ((!(strcmp("ON", argv[i + 1]))) || (!(strcmp("on", argv[i + 1])))) {
				opts->action = 0x90;
			}
			else if ((!(strcmp("OFF", argv[i + 1]))) || (!(strcmp("off", argv[i + 1])))) {
				opts->action = 0x8e;
			}
			else {
				LOG(E) << "Invalid MBREnable argument <on|off> " << argv[i];
				return 1;
			}
			i++;
			opts->password = ++i;
		}
		else if (!(strcmp("--setMBRDone", argv[i]))) {
			if ((!(strcmp("ON", argv[i + 1]))) || (!(strcmp("on", argv[i + 1])))) {
				opts->action = 0x91;
			}
			else if ((!(strcmp("OFF", argv[i + 1]))) || (!(strcmp("off", argv[i + 1])))) {
				opts->action = 0x8f;
			}
			else {
				LOG(E) << "Invalid MBRDone argument <on|off> " << argv[i];
				return 1;
			}
			i++;
			opts->password = ++i;
		}
		else if (!(strcmp("--setLR", argv[i]))) {
			opts->action = 0x06;
			opts->lockingrange = (uint8_t) atoi(argv[++i]);
			if (opts->lockingrange) {
				LOG(E) << "Only the Global Locking Range is currently supported";
				return 1;
			}
			if ((!(strcmp("rw", argv[i + 1]))) || (!(strcmp("RW", argv[i + 1])))) {
				opts->lockingstate = 0x01;
			}
			else if ((!(strcmp("ro", argv[i + 1]))) || (!(strcmp("RO", argv[i + 1])))) {
				opts->lockingstate = 0x02;
			}
			else if ((!(strcmp("lk", argv[i + 1]))) || (!(strcmp("LK", argv[i + 1])))) {
				opts->lockingstate = 0x03;
			}
			else {
				LOG(E) << "Invalid setLR argument <ro|rw|lk> " << argv[i+1];
				return 1;
			}
			i++;
			opts->password = ++i;
		}
		else if (!(strcmp("--loadPBAimage", argv[i]))) {
			opts->action = 0x04;
			opts->password = ++i;
			opts->pbafile = ++i;
		}
		else if (!(strcmp("--revert", argv[i]))) {
			opts->action = 0x95;
			opts->password = ++i;
		}
		//else if (!(strcmp("--revertnoerase", argv[i]))) {
		//	opts->action = 0x05;
		//	opts->password = ++i;
		//	opts->newpassword = ++i;
		//}
		else if (!(strcmp("--PSIDrevert", argv[i]))) {
			opts->action = 0x97;
			opts->password = ++i;
		}
		/* ****************************************************************
		 * discrete manupulation of individual settings in the TPer
		 */
		else if (!(strcmp("--enableReadLockingGlobal", argv[i])))
			opts->action = 0x80;
		else if (!(strcmp("--enableWriteLockingGlobal", argv[i])))
			opts->action = 0x81;
		else if (!(strcmp("--disableReadLockingGlobal", argv[i])))
			opts->action = 0x82;
		else if (!(strcmp("--disableWriteLockingGlobal", argv[i])))
			opts->action = 0x83;
		else if (!(strcmp("--enableuser", argv[i])))
			opts->action = 0x85;
		else if (!(strcmp("--setReadLockedGlobal", argv[i])))
			opts->action = 0x86;
		else if (!(strcmp("--setWriteLockedGlobal", argv[i])))
			opts->action = 0x87;
		else if (!(strcmp("--unsetReadLockedGlobal", argv[i])))
			opts->action = 0x88;
		else if (!(strcmp("--unsetWriteLockedGlobal", argv[i])))
			opts->action = 0x89;
		
        else if (!(strcmp("--activateLockingSP", argv[i])))
            opts->action = 0x8a;  
		
		else if (!(strcmp("--query", argv[i])))
			opts->action = 0x8b;
		else if (!(strcmp("--scan", argv[i])))
			opts->action = 0x8c;
		else if (!(strcmp("--takeownership", argv[i])))
			opts->action = 0x8d;
		//else if (!(strcmp("--unsetMBREnable", argv[i])))
		//	opts->action = 0x8e;
		//else if (!(strcmp("--unsetMBRDone", argv[i])))
		//	opts->action = 0x8f;
		//else if (!(strcmp("--setMBREnable", argv[i])))
		//	opts->action = 0x90;
		//else if (!(strcmp("--setMBRDone", argv[i])))
		//	opts->action = 0x91;
		else if (!(strcmp("--dumpTable", argv[i])))
			opts->action = 0x92;
		else if (!(strcmp("--revertLockingSP", argv[i])))
			opts->action = 0x93;
		else if (!(strcmp("--setPassword", argv[i])))
			opts->action = 0x94;
        else if (!(strcmp("--revertTPer", argv[i])))
			opts->action = 0x95;  
		else if (!(strcmp("--ValidatePBKDF2", argv[i])))
			opts->action = 0x96;
		else if (!(strcmp("--PSIDRevert", argv[i])) ||
			!(strcmp("--yesIreallywanttoERASE*ALL*mydatausingthePSID", argv[i]))) {
			opts->action = 0x97;
			opts->password = ++i;
		}
        else if (!(strcmp("--revertLockingSPnoerase", argv[i])))
            opts->action = 0x98;
		else if ('v' == argv[i][1])
		{
			loggingLevel += (uint16_t)(strlen(argv[i]) - 1);
			if (loggingLevel > 7) loggingLevel = 7;
			CLog::Level() = CLog::FromInt(loggingLevel);
			LOG(D) << "Log level set to " << CLog::ToString(CLog::FromInt(loggingLevel));
		}
		else if (!(strcmp("--user", argv[i])))
			opts->userid = ++i;
		else if (!(strcmp("--newpassword", argv[i])))
			opts->newpassword = ++i;
		else if (!(strcmp("--password", argv[i])))
			opts->password = ++i;
       
        else if (i != argc - 1) {
            LOG(E) << "Invalid command line argument " << argv[i];
            return 1;
        }
    }
    //if (i == argc - 1){
    //	LOG(E) << "Cannt Find the <device> paramater ";
    //	usage();
    //	return 1;
    //}
    // some basic sanity checks
	// scan and validatePBKDF only require the action
    if ((0x8c != opts->action) && (0x96 != opts->action)) {
        if (argc < 3) {
            LOG(E) << "To few command line options";
            return 1;
        }
        else {
            if ('-' == argv[argc - 1][1]) {
                LOG(E) << "The last argument must be the device ";
                return 1;
            }
        }
    }
    if (0x94 == opts->action) {
        if (0 == opts->newpassword) {
            LOG(E) << "--newpassword(-n) required to set a newpassword";
            return 1;
        }
        if (0 == opts->userid) {
            LOG(E) << "--user(-u) required to set a newpassword";
            return 1;
        }
    }
    if (0x85 == opts->action) {
        if (0 == opts->password) {
            LOG(E) << "--password(-p) of the lockingSP ADMIN1 password is required to enable a user";
            return 1;
        }
        if (0 == opts->userid) {
            LOG(E) << "--user(-u) required to enable a user";
            return 1;
        }
    }
    // TODO: check for multiple actions specified
    //if (1 != opts->takeOwnership + opts->scanSystem + opts->revertSP) {
    //	LOG(E) << "only one action can be performed at a time";
    //	usage();
    //	return 1;
    //}
    return 0;
}