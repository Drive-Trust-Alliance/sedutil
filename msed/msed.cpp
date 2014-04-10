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

#include "TCGtasks.h"
#include "options.h"

int main(int argc, char * argv[])
{
	MSED_OPTIONS opts;
	if (options(argc, argv, &opts)) {
		//LOG(E) << "Invalid command line options ";
		return 1;
	}
	switch (opts.action){
		case 's':
			LOG(D) << "Performing diskScan() ";
			diskScan();
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
			LOG(D) << "Activating the LockingSP on" << argv[argc-1] << " with password " << argv[opts.password];
			return activateLockingSP(argv[argc - 1], argv[opts.password]);
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
		case 'Y':
			if (0 == opts.password) {
				LOG(E) << "PSID Revert requires a password (-p)";
				return 1;
			}
			LOG(I) << "Performing a PSID Revert on " << argv[argc - 1] << " with password " << argv[opts.password];
			return revertTPer(argv[argc - 1], argv[opts.password],1);
			break;
		case 'Z':
			if (0 == opts.password) {
				LOG(E) << "Reverting the Locking SP requires a password (-p)";
				return 1;
			}
			LOG(D) << "Performing revertLockingSP (KeepGlobalRangeKey) on " << argv[argc - 1] << " with password " << argv[opts.password];
			return revertLockingSP(argv[argc - 1], argv[opts.password],1);
			break;
		case 'S':
			LOG(D) << "Performing setpassword for user " << argv[opts.userid];
			LOG(D4) <<"new password is " << argv[opts.newpassword] <<
				" using " << argv[opts.password] << " as the LockingSP ADMIN1 password" <<
				" on device " << argv[argc - 1];
			return setNewPassword(argv[opts.password],argv[opts.userid],
							argv[opts.newpassword], argv[argc-1]);
			break;
		case 'e':
			LOG(D) << "Performing enable user for user " << argv[opts.userid];
			return enableUser(argv[opts.password], argv[opts.userid], argv[argc - 1]);
			break;
		default:
			LOG(E) << "Uable to determing what you want to do ";
			usage();
	}
	return 1;
}