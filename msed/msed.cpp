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
     MSED_OPTIONS opts;
	 vector<uint8_t> opalTRUE(1,0x01), opalFALSE(1,0x00);
	//opalTRUE.push_back(0x01);
	//opalFALSE.push_back(0x00);

    if (MsedOptions(argc, argv, &opts)) {
        //LOG(E) << "Invalid command line options ";
        return 1;
    }
    switch (opts.action) {
		/* ****************************************************************
		* General Usage commands should be the only ones uded by a
		* typical end user
		* *******************************************************************/
	case 0x01:
		if (0 == opts.password) {
			LOG(E) << "Initial setup requires a new SID password";
			return 1;
		}
		LOG(D) << "Performing initial setup to use msed on drive " << argv[argc - 1];
		return (initialsetup(argv[opts.password], argv[argc - 1]));
	case 0x02:
		if ((0 == opts.password) || (0 == opts.newpassword)) {
			LOG(E) << "setSIDPwd requires both the old SID password and a new SID password";
			return 1;
		}
		LOG(D) << "Performing setSIDPwd ";
		return (setSIDPassword(argv[opts.password], argv[opts.newpassword], argv[argc - 1]));
	case 0x03:
		LOG(D) << "Performing setPAdmin1Pwd ";
		return setNewPassword(argv[opts.password], "Admin1",
			argv[opts.newpassword], argv[argc - 1]);
	case 0x04:
		LOG(E) << "PBA image code is not implemented";
		return 1;
	case 0x05:
		if (0 == opts.password) {
			LOG(E) << "revertnoerase requires the SID password";
			return 1;
		}
		LOG(D) << "Performing revertnoerase on " << argv[argc - 1];
		return (revertnoerase(argv[opts.password], argv[opts.newpassword], argv[argc - 1]));
	case 0x06:
		if (0 == opts.password) {
			LOG(E) << "setLR requires the Admin1 password";
			return 1;
		}
		LOG(D) << "Setting Locking Range " << (uint16_t)opts.lockingrange << " " << (uint16_t)opts.lockingstate;
		return (setLockingRange(opts.lockingrange, opts.lockingstate, argv[opts.password],
			argv[argc - 1]));
		/* ********************************************************************
		 *   Discrete functions should not be used by typical user 
		 * *******************************************************************/
	case 0x80:
		if (0 == opts.password) {
			LOG(E) << "Enabling read Locking on the global range " << 
				"requires the Admin1 password (-p)";
			return 1;
		}
		LOG(D) << "Enabling read Locking on the global range " << argv[argc - 1];
		return(MsedSetLSP(OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL,
			OPAL_TOKEN::READLOCKENABLED, opalTRUE, 
			argv[opts.password], argv[argc - 1]));
		break;
	case 0x81:
		if (0 == opts.password) {
			LOG(E) << "Enabling write Locking on the global range " <<
				"requires the Admin1 password (-p)";
			return 1;
		}
		LOG(D) << "Enabling write Locking on the global range " << argv[argc - 1];
		return(MsedSetLSP(OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL,
			OPAL_TOKEN::WRITELOCKENABLED, opalTRUE, argv[opts.password], argv[argc - 1]));
		break;
	case 0x82:
		if (0 == opts.password) {
			LOG(E) << "Disabling read Locking on the global range " <<
				"requires the Admin1 password (-p)";
			return 1;
		}
		LOG(D) << "Disabling read Locking on the global range " << argv[argc - 1];
		return(MsedSetLSP(OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL,
			OPAL_TOKEN::READLOCKENABLED, opalFALSE,
			argv[opts.password], argv[argc - 1]));
		break;
	case 0x83:
		if (0 == opts.password) {
			LOG(E) << "Disabling write Locking on the global range " <<
				"requires the Admin1 password (-p)";
			return 1;
		}
		LOG(D) << "Disabling write Locking on the global range " << argv[argc - 1];
		return(MsedSetLSP(OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL,
			OPAL_TOKEN::WRITELOCKENABLED, opalFALSE,
			argv[opts.password], argv[argc - 1]));
		break;
	case 0x85:
		LOG(D) << "Performing enable user for user " << argv[opts.userid];
		return enableUser(argv[opts.password], argv[opts.userid], argv[argc - 1]);
		break;
	case 0x86:
		if (0 == opts.password) {
			LOG(E) << "Setting readLocked on the global range " <<
				"requires the Admin1 password";
			return 1;
		}
		LOG(D) << "Setting readLocked on the global range " << argv[argc - 1];
		return(MsedSetLSP(OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL,
			OPAL_TOKEN::READLOCKED , opalTRUE,
			argv[opts.password], argv[argc - 1]));
		break;
	case 0x87:
		if (0 == opts.password) {
			LOG(E) << "Setting writeLocked on the global range " <<
				"requires the Admin1 password";
			return 1;
		}
		LOG(D) << "Setting writeLocked on the global range " << argv[argc - 1];
		return(MsedSetLSP(OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL,
			OPAL_TOKEN::WRITELOCKED, opalTRUE,
			argv[opts.password], argv[argc - 1]));
		break;
	case 0x88:
		if (0 == opts.password) {
			LOG(E) << "Unsetting readLocked on the global range " <<
				"requires the Admin1 password";
			return 1;
		}
		LOG(D) << "Unsetting readLocking on the global range " << argv[argc - 1];
		return(MsedSetLSP(OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL,
			OPAL_TOKEN::READLOCKED, opalFALSE,
			argv[opts.password], argv[argc - 1]));
		break;
	case 0x89:
		if (0 == opts.password) {
			LOG(E) << "Unsetting writeLocked on the global range " <<
				"requires the Admin1 password";
			return 1;
		}
		LOG(D) << "Unsetting writeLocked on the global range " << argv[argc - 1];
		return(MsedSetLSP(OPAL_UID::OPAL_LOCKINGRANGE_GLOBAL,
			OPAL_TOKEN::WRITELOCKED, opalFALSE,
			argv[opts.password], argv[argc - 1]));
		break;
	case 0x8a:
		if (0 == opts.password) {
			LOG(E) << "Activating the Locking SP required the SID password ";
			return 1;
		}
		LOG(D) << "Activating the LockingSP on" << argv[argc - 1];
		return activateLockingSP(argv[argc - 1], argv[opts.password]);
		break;
	case 0x8b:
		LOG(D) << "Performing diskquery() on " << argv[argc - 1];
		diskQuery(argv[argc - 1]);
		return 0;
		break;
	case 0x8c:
		LOG(D) << "Performing diskScan() ";
		diskScan();
		break;
	case 0x8d:
		if (0 == opts.password) {
			LOG(E) << "Taking ownwership requires a *NEW* SID password ";
			return 1;
		}
		LOG(D) << "Taking Ownership of the drive at" << argv[argc - 1];
		return takeOwnership(argv[argc - 1], argv[opts.password]);
		break;
	case 0x8e:
		if (0 == opts.password) {
			LOG(E) << "Unsetting MBREnable " <<
				"requires the Admin1 password";
			return 1;
		}
		LOG(D) << "Unsetting MBREnable " << argv[argc - 1];
		return(MsedSetLSP(OPAL_UID::OPAL_MBRCONTROL, OPAL_TOKEN::MBRENABLE, opalFALSE,
			argv[opts.password], argv[argc - 1]));
		break;
	case 0x8f:
		if (0 == opts.password) {
			LOG(E) << "Unsetting MBRDone " <<
				"requires the Admin1 password";
			return 1;
		}
		LOG(D) << "Unsetting MBRDone " << argv[argc - 1];
		return(MsedSetLSP(OPAL_UID::OPAL_MBRCONTROL, OPAL_TOKEN::MBRDONE, opalFALSE,
			argv[opts.password], argv[argc - 1]));
		break;
	case 0x90:
		if (0 == opts.password) {
			LOG(E) << "Setting MBREnable " << "requires the Admin1 password ";
			return 1;
		}
		LOG(D) << "Setting MBREnable " << argv[argc - 1];
		return(MsedSetLSP(OPAL_UID::OPAL_MBRCONTROL, OPAL_TOKEN::MBRENABLE, opalTRUE,
			argv[opts.password], argv[argc - 1]));
		break;
	case 0x91:
		if (0 == opts.password) {
			LOG(E) << "Setting MBRDone " << "requires the Admin1 password";
			return 1;
		}
		LOG(D) << "Setting MBRDone " << argv[argc - 1];
		return(MsedSetLSP(OPAL_UID::OPAL_MBRCONTROL, OPAL_TOKEN::MBRDONE, opalTRUE,
			argv[opts.password], argv[argc - 1]));
		break;
	case 0x92:
		if (0 == opts.password) {
			LOG(E) << "tableDump requires a password";
			return 1;
		}
		LOG(D) << "Performing dumpTable() ";
		return dumpTable(argv[opts.password], argv[argc - 1]);
		break;
	case 0x93:
		if (0 == opts.password) {
			LOG(E) << "Reverting the Locking SP requires a password ";
			return 1;
		}
		LOG(D) << "Performing revertLockingSP on " << argv[argc - 1];
		return revertLockingSP(argv[argc - 1], argv[opts.password]);
		break;
	case 0x94:
		LOG(D) << "Performing setPassword for user " << argv[opts.userid];
		return setNewPassword(argv[opts.password], argv[opts.userid],
			argv[opts.newpassword], argv[argc - 1]);
		break;
	case 0x95:
		if (0 == opts.password) {
			LOG(E) << "Reverting the TPer requires a the SID password ";
			return 1;
		}
		LOG(D) << "Performing revertTPer on " << argv[argc - 1];
		return revertTPer(argv[argc - 1], argv[opts.password]);
		break;
	case 0x96:
		LOG(D) << "Performing PBKDF2 validation ";
		MsedTestPBDKF2();
		break;
	case 0x97:
		if (0 == opts.password) {
			LOG(E) << "PSID Revert requires a password ";
			return 1;
		}
		LOG(D) << "Performing a PSID Revert on " << argv[argc - 1] << " with password " << argv[opts.password];
		return revertTPer(argv[argc - 1], argv[opts.password], 1);
		break;
	case 0x98:
		if (0 == opts.password) {
			LOG(E) << "Reverting the Locking SP requires a password ";
			return 1;
		}
		LOG(D) << "Performing revertLockingSP (KeepGlobalRangeKey) on " << argv[argc - 1];
		return revertLockingSP(argv[argc - 1], argv[opts.password], 1);
		break;

    default:
        LOG(E) << "Uable to determine what you want to do ";
        usage();
    }
    return 1;
}