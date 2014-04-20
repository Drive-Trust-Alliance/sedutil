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

#include "MsedTasks.h"
#include "MsedHashPwd.h"
#include "MsedOptions.h"
#include "MsedLexicon.h"

int main(int argc, char * argv[])
{
    ///* *****************************************************************
    // * *****************************************************************
    // */
    //dumpTable();
    //if (argc > 0) return 0;
    ///* ******************************************************************
    // * ******************************************************************
    // */

    MSED_OPTIONS opts;
	vector<uint8_t> opalTRUE, opalFALSE;
	opalTRUE.push_back(0x01);
	opalFALSE.push_back(0x00);

    if (MsedOptions(argc, argv, &opts)) {
        //LOG(E) << "Invalid command line options ";
        return 1;
    }
    switch (opts.action) {
    case 's':
        LOG(D) << "Performing diskScan() ";
        diskScan();
        break;
    case 'V':
        LOG(D) << "Performing PBKDF2 validation ";
        MsedTestPBDKF2();
        break;
    case 'q':
        LOG(D) << "Performing diskquery() on " << argv[argc - 1];
        diskQuery(argv[argc - 1]);
        return 0;
        break;
    case 't':
        if (0 == opts.password) {
            LOG(E) << "Taking ownwership requires a *NEW* SID password (-p)";
            return 1;
        }
        LOG(D) << "Taking Ownership of the drive at" << argv[argc - 1] << " with password " << argv[opts.password];
        return takeOwnership(argv[argc - 1], argv[opts.password]);
        break;
    case 'l':
        if (0 == opts.password) {
            LOG(E) << "Activating the Locking SP required the SID password (-p)";
            return 1;
        }
        LOG(D) << "Activating the LockingSP on" << argv[argc - 1] << " with password " << argv[opts.password];
        return activateLockingSP(argv[argc - 1], argv[opts.password]);
        break;
	case 'a':
		if (0 == opts.password) {
			LOG(E) << "Enabling read Locking on the global range " << 
				"requires the Admin1 password (-p)";
			return 1;
		}
		LOG(I) << "Enabling read Locking on the global range " << argv[argc - 1];
		return(MsedSetLSP(OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL,
			OPAL_TOKEN::READLOCKENABLED, opalTRUE, 
			argv[opts.password], argv[argc - 1]));
		break;
	case 'b':
		if (0 == opts.password) {
			LOG(E) << "Enabling write Locking on the global range " <<
				"requires the Admin1 password (-p)";
			return 1;
		}
		LOG(I) << "Enabling write Locking on the global range " << argv[argc - 1];
		return(MsedSetLSP(OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL,
			OPAL_TOKEN::WRITELOCKENABLED, opalTRUE, argv[opts.password], argv[argc - 1]));
		break;
	case 'c':
		if (0 == opts.password) {
			LOG(E) << "Disabling read Locking on the global range " <<
				"requires the Admin1 password (-p)";
			return 1;
		}
		LOG(I) << "Disabling read Locking on the global range " << argv[argc - 1];
		return(MsedSetLSP(OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL,
			OPAL_TOKEN::READLOCKENABLED, opalFALSE,
			argv[opts.password], argv[argc - 1]));
		break;
	case 'd':
		if (0 == opts.password) {
			LOG(E) << "Disabling write Locking on the global range " <<
				"requires the Admin1 password (-p)";
			return 1;
		}
		LOG(I) << "Disabling write Locking on the global range " << argv[argc - 1];
		return(MsedSetLSP(OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL,
			OPAL_TOKEN::WRITELOCKENABLED, opalFALSE, 
			argv[opts.password], argv[argc - 1]));
		break;
	case 'f':
		if (0 == opts.password) {
			LOG(E) << "Setting readLocked on the global range " <<
				"requires the Admin1 password (-p)";
			return 1;
		}
		LOG(I) << "Setting readLocked on the global range " << argv[argc - 1];
		return(MsedSetLSP(OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL,
			OPAL_TOKEN::READLOCKED , opalTRUE,
			argv[opts.password], argv[argc - 1]));
		break;
	case 'g':
		if (0 == opts.password) {
			LOG(E) << "Setting writeLocked on the global range " <<
				"requires the Admin1 password (-p)";
			return 1;
		}
		LOG(I) << "Setting writeLocked on the global range " << argv[argc - 1];
		return(MsedSetLSP(OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL,
			OPAL_TOKEN::WRITELOCKED, opalTRUE,
			argv[opts.password], argv[argc - 1]));
		break;
	case 'i':
		if (0 == opts.password) {
			LOG(E) << "Unsetting readLocked on the global range " <<
				"requires the Admin1 password (-p)";
			return 1;
		}
		LOG(I) << "Unsetting readLocking on the global range " << argv[argc - 1];
		return(MsedSetLSP(OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL,
			OPAL_TOKEN::READLOCKED, opalFALSE,
			argv[opts.password], argv[argc - 1]));
		break;
	case 'j':
		if (0 == opts.password) {
			LOG(E) << "Unsetting writeLocked on the global range " <<
				"requires the Admin1 password (-p)";
			return 1;
		}
		LOG(I) << "Unsetting writeLocked on the global range " << argv[argc - 1];
		return(MsedSetLSP(OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL,
			OPAL_TOKEN::WRITELOCKED, opalFALSE,
			argv[opts.password], argv[argc - 1]));
		break;
	case 'w':
		if (0 == opts.password) {
			LOG(E) << "Unsetting MBREnable " <<
				"requires the Admin1 password (-p)";
			return 1;
		}
		LOG(I) << "Unsetting MBREnable " << argv[argc - 1];
		return(MsedSetLSP(OPAL_UID::OPAL_MBRCONTROL, OPAL_TOKEN::MBRENABLE, opalFALSE,
			argv[opts.password], argv[argc - 1]));
		break;
	case 'x':
		if (0 == opts.password) {
			LOG(E) << "Unsetting MBRDone " <<
				"requires the Admin1 password (-p)";
			return 1;
		}
		LOG(I) << "Unsetting MBRDone " << argv[argc - 1];
		return(MsedSetLSP(OPAL_UID::OPAL_MBRCONTROL, OPAL_TOKEN::MBRDONE, opalFALSE,
			argv[opts.password], argv[argc - 1]));
		break;
	case 'y':
		if (0 == opts.password) {
			LOG(E) << "Setting MBREnable " <<	"requires the Admin1 password (-p)";
			return 1;
		}
		LOG(I) << "Setting MBREnable " << argv[argc - 1];
		return(MsedSetLSP(OPAL_UID::OPAL_MBRCONTROL, OPAL_TOKEN::MBRENABLE, opalTRUE,
			argv[opts.password], argv[argc - 1]));
		break;
	case 'z':
		if (0 == opts.password) {
			LOG(E) << "Setting MBRDone " << "requires the Admin1 password (-p)";
			return 1;
		}
		LOG(I) << "Setting MBRDone " << argv[argc - 1];
		return(MsedSetLSP(OPAL_UID::OPAL_MBRCONTROL, OPAL_TOKEN::MBRDONE, opalTRUE,
			argv[opts.password], argv[argc - 1]));
		break;
	case 'T':
        if (0 == opts.password) {
            LOG(E) << "Reverting the TPer requires a the SID password (-p)";
            return 1;
        }
        LOG(D) << "Performing revertTPer on " << argv[argc - 1] << " with password " << argv[opts.password];
        return revertTPer(argv[argc - 1], argv[opts.password]);
        break;
    case 'L':
        if (0 == opts.password) {
            LOG(E) << "Reverting the Locking SP requires a password (-p)";
            return 1;
        }
        LOG(D) << "Performing revertLockingSP on " << argv[argc - 1] << " with password " << argv[opts.password];
        return revertLockingSP(argv[argc - 1], argv[opts.password]);
        break;
    case 'D':
        if (0 == opts.password) {
            LOG(E) << "tableDump requires a password (-p)";
            return 1;
        }
        LOG(I) << "Performing dumpTable() ";
        return dumpTable();
        break;
    case 'Y':
        if (0 == opts.password) {
            LOG(E) << "PSID Revert requires a password (-p)";
            return 1;
        }
        LOG(I) << "Performing a PSID Revert on " << argv[argc - 1] << " with password " << argv[opts.password];
        return revertTPer(argv[argc - 1], argv[opts.password], 1);
        break;
    case 'Z':
        if (0 == opts.password) {
            LOG(E) << "Reverting the Locking SP requires a password (-p)";
            return 1;
        }
        LOG(D) << "Performing revertLockingSP (KeepGlobalRangeKey) on " << argv[argc - 1] << " with password " << argv[opts.password];
        return revertLockingSP(argv[argc - 1], argv[opts.password], 1);
        break;
    case 'S':
        LOG(D) << "Performing setpassword for user " << argv[opts.userid];
        LOG(D4) << "new password is " << argv[opts.newpassword] <<
                " using " << argv[opts.password] << " as the LockingSP ADMIN1 password" <<
                " on device " << argv[argc - 1];
        return setNewPassword(argv[opts.password], argv[opts.userid],
                              argv[opts.newpassword], argv[argc - 1]);
        break;
    case 'e':
        LOG(D) << "Performing enable user for user " << argv[opts.userid];
        return enableUser(argv[opts.password], argv[opts.userid], argv[argc - 1]);
        break;
    default:
        LOG(E) << "Uable to determine what you want to do ";
        usage();
    }
    return 1;
}