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
    printf("msed v%s Copyright (c) 2014 Michael Romeo <r0m30@r0m30.com>\n", MSED_VERSION);
    printf("\n");
    printf("msed is a utility to manage self encrypting drives\n");
    printf("that conform to the Trusted Computing Group OPAL 2.0 SSC\n");
    printf("specification \n");
    printf("Usage:\n");
    printf("msed <action> <options> <device>\n\n");
    printf("actions \n");
    printf("-s, --scan \n");
    printf("                                Scans the devices on the system \n");
    printf("                                identifying Opal 2.0 compliant devices \n");
    printf("-q, --query \n");
    printf("                                Display the Discovery 0 response of a device\n");
    printf("-t, --takeownership \n");
    printf("                                Change the SID password of the device\n");
    printf("                                --password(-p) option required to specify the\n");
    printf("                                new password\n");
    printf("-l, --activatelockingsp \n");
    printf("                                Activate the LockingSP\n");
    printf("                                --password(-p) option required\n");
	printf("--enableReadLockingGlobal \n");
	printf("                                Enable Read Locking on the Global Range\n");
	printf("                                --password(-p) option required\n");
	printf("--enablWriteLockingGlobal \n");
	printf("                                Enable Write Locking on the Global Range\n");
	printf("                                --password(-p) option required\n");
	printf("--disableReadLockingGlobal \n");
	printf("                                Disable Read Locking on the Global Range\n");
	printf("                                --password(-p) option required\n");
	printf("--disablWriteLockingGlobal \n");
	printf("                                Disable Write Locking on the Global Range\n");
	printf("                                --password(-p) option required\n");
	printf("--unsetMBREnable \n");
	printf("                                Disable MBR shadowing \n");
	printf("                                --password(-p) option required\n");
	printf("--unsetMBRDone \n");
	printf("                                 Shadow MBR \n");
	printf("                                --password(-p) option required\n");
	printf("--setMBREnable \n");
	printf("                                Enable MBR shadowing \n");
	printf("                                --password(-p) option required\n");
	printf("--setMBRDone \n");
	printf("                                 Unshadow MBR \n");
	printf("                                --password(-p) option required\n");
    printf("-e, --enableuser \n");
    printf("                                Enable a user in the lockingSP\n");
    printf("                                --password(-p) is the lockingSP Admin1 password\n");
    printf("-S, --setpassword \n");
    printf("                                Change the password of a TPER authority\n");
    printf("                                --password(-p) is the lockingSP Admin1 password\n");
    printf("                                --newpassword(-n) is the new password\n");
    printf("                                Note: when first activated the LockingSPs\n");
    printf("                                      Admin1 password is set to the SID password\n");
    printf("-T, --revertTPer \n");
    printf("                                set the device back to factory defaults \n");
    printf("                                password(-p) option required to specify the SID password\n");
    printf("                                see Opal SSC documentation for optional Vendor unique\n");
    printf("                                exceptions\n");
    printf("-L, --revertLockingSP \n");
    printf("                                revert the LockingSP *ERASING* *ALL* the data \n");
    printf("                                password(-p) option required\n");
    printf("-D, --dumpTable \n");
    printf("                                *DEBUG* table Dump \n");
    printf("                                password(-p) option required\n");
    printf("-V, --ValidatePBKDF2 \n");
    printf("                                Run the PKCS5_PBKDF2_HMAC<SHA1> validation tests \n");
    printf("--revertLockingSPnoerase \n");
    printf("                                revert the LockingSP without erasing the data \n");
    printf("                                password(-p) option required\n");
    printf("--yesIreallywanttoERASE*ALL*mydatausingthePSID \n");
    printf("                                revert the LockingSP using the PSID *ERASING* *ALL* the data \n");
    printf("                                password(-p) option required\n");
    printf("options\n");
    printf("-p --password <password>     \n");
    printf("                                 the password for the action \n");
	printf("-n, --newpassword \n");
	printf("                                new password (only valid for --setpassword(-s)\n");
	printf("-u, --user \n");
	printf("                                Userid action is directed at\n");
    printf("-v	                             increase verbosity, can be repeated multiple times\n");
    printf("-q, --quiet                      suppress all but ERROR level messages\n");
    printf("-h, --help                       displays this message and ignores all other actions and options\n");
    printf("\n");
    printf("Examples \n");
    printf("msed --scan \n");
    printf("msed --takeownership --password newSIDpassword \\\\.\\PhysicalDisk2 \n");
    printf("msed --revertTPer --password theSIDPassword /dev/sg0 \n");
    return;
}

/*
 * This would probably be easier with getopts but
 * I'm not ready to commit to GPL
 */
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
		else if (!(strcmp("-a", argv[i])) || !(strcmp("--enableReadLockingGlobal", argv[i])))
			opts->action = 'a';
		else if (!(strcmp("-b", argv[i])) || !(strcmp("--enableWriteLockingGlobal", argv[i])))
			opts->action = 'b';
		else if (!(strcmp("-c", argv[i])) || !(strcmp("--disableReadLockingGlobal", argv[i])))
			opts->action = 'c';
		else if (!(strcmp("-d", argv[i])) || !(strcmp("--disableWriteLockingGlobal", argv[i])))
			opts->action = 'd';
		else if (!(strcmp("-e", argv[i])) || !(strcmp("--enableuser", argv[i])))
			opts->action = 'e';
		else if (!(strcmp("-f", argv[i])) || !(strcmp("--setReadLockedGlobal", argv[i])))
			opts->action = 'f';
		else if (!(strcmp("-g", argv[i])) || !(strcmp("--setWriteLockedGlobal", argv[i])))
			opts->action = 'g';
		//                 'h' = help
		else if (!(strcmp("-i", argv[i])) || !(strcmp("--unsetReadLockedGlobal", argv[i])))
			opts->action = 'i';
		else if (!(strcmp("-j", argv[i])) || !(strcmp("--unsetWriteLockedGlobal", argv[i])))
			opts->action = 'j';
		
        else if (!(strcmp("-l", argv[i])) || !(strcmp("--activatelockingsp", argv[i])))
            opts->action = 'l';  
		else if (!(strcmp("-n", argv[i])) || !(strcmp("--newpassword", argv[i])))
			opts->newpassword = ++i;
		else if (!(strcmp("-p", argv[i])) || !(strcmp("--password", argv[i])))
			opts->password = ++i;
		else if (!(strcmp("-q", argv[i])) || !(strcmp("--query", argv[i])))
			opts->action = 'q';
		else if (!(strcmp("-s", argv[i])) || !(strcmp("--scan", argv[i])))
			opts->action = 's';
		else if (!(strcmp("-t", argv[i])) || !(strcmp("--takeownership", argv[i])))
			opts->action = 't';
        else if (!(strcmp("-u", argv[i])) || !(strcmp("--user", argv[i])))
            opts->userid = ++i;
		else if ('v' == argv[i][1]) 
		{
			loggingLevel += (uint16_t)(strlen(argv[i]) - 1);
			if (loggingLevel > 7) loggingLevel = 7;
			CLog::Level() = CLog::FromInt(loggingLevel);
			LOG(D) << "Log level set to " << CLog::ToString(CLog::FromInt(loggingLevel));
		}
		else if (!(strcmp("-w", argv[i])) || !(strcmp("--unsetMBREnable", argv[i])))
			opts->action = 'w';
		else if (!(strcmp("-x", argv[i])) || !(strcmp("--unsetMBRDone", argv[i])))
			opts->action = 'x';
		else if (!(strcmp("-y", argv[i])) || !(strcmp("--setMBREnable", argv[i])))
			opts->action = 'y';
		else if (!(strcmp("-z", argv[i])) || !(strcmp("--setMBRDone", argv[i])))
			opts->action = 'z';
		else if (!(strcmp("-D", argv[i])) || !(strcmp("--dumpTable", argv[i])))
			opts->action = 'D';
		else if (!(strcmp("-L", argv[i])) || !(strcmp("--revertLockingSP", argv[i])))
			opts->action = 'L';
		else if (!(strcmp("-S", argv[i])) || !(strcmp("--setpassword", argv[i])))
			opts->action = 'S';
        else if (!(strcmp("-T", argv[i])) || !(strcmp("--revertTPer", argv[i])))
			opts->action = 'T';  
		else if (!(strcmp("-V", argv[i])) || !(strcmp(" --ValidatePBKDF2", argv[i])))
			opts->action = 'V';
        else if (!(strcmp("-Y", argv[i])) ||
                 !(strcmp("--yesIreallywanttoERASE*ALL*mydatausingthePSID", argv[i])))
            opts->action = 'Y';
        else if (!(strcmp("-Z", argv[i])) || !(strcmp("--revertLockingSPnoerase", argv[i])))
            opts->action = 'Z';
       
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
    if (('s' != opts->action) && ('V' != opts->action)) {
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
    if ('S' == opts->action) {
        if (0 == opts->newpassword) {
            LOG(E) << "--newpassword(-n) required to set a newpassword";
            return 1;
        }
        if (0 == opts->userid) {
            LOG(E) << "--user(-u) required to set a newpassword";
            return 1;
        }
    }
    if ('e' == opts->action) {
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