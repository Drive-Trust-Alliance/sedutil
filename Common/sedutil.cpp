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
#include <iostream>
#include "os.h"
#include "DtaHashPwd.h"
#include "DtaOptions.h"
#include "DtaLexicon.h"
#include "DtaDevGeneric.h"
#include "DtaDevOpal1.h"
#include "DtaDevOpal2.h"
#include "DtaDevEnterprise.h"

using namespace std;

/* Default to output that omits timestamps and goes to stdout */
sedutiloutput outputFormat = sedutilReadable;

int isValidSEDDisk(char *devname)
{
	DtaDev * d;
	d = new DtaDevGeneric(devname);
	if (d->isPresent()) {
		printf("%s", devname);
		if (d->isAnySSC())
			printf(" SED %s%s%s ", (d->isOpal1() ? "1" : "-"),
			(d->isOpal2() ? "2" : "-"), (d->isEprise() ? "E" : "-"));
		else
			printf("%s", " NO --- ");
		cout << d->getModelNum() << " " << d->getFirmwareRev();
		cout << std::endl;
	}
	delete d;
	return 0;
}

int main(int argc, char * argv[])
{
	DTA_OPTIONS opts;
	DtaDev *tempDev = NULL, *d = NULL;
    std::string interactive_password;
	if (DtaOptions(argc, argv, &opts)) {
		return DTAERROR_COMMAND_ERROR;
	}
	
#ifdef __linux__
    if (opts.secure_mode && opts.ask_password) {
        interactive_password = GetPassPhrase("Please enter password ");
    }
#endif //__linux__

	if ((opts.action != sedutiloption::scan) && 
		(opts.action != sedutiloption::validatePBKDF2) &&
		(opts.action != sedutiloption::isValidSED)) {
		if (opts.device > (argc - 1)) opts.device = 0;
		tempDev = new DtaDevGeneric(argv[opts.device]);
		if (NULL == tempDev) {
			LOG(E) << "Create device object failed";
			return DTAERROR_OBJECT_CREATE_FAILED;
		}
		if ((!tempDev->isPresent()) || (!tempDev->isAnySSC())) {
			LOG(E) << "Invalid or unsupported disk " << argv[opts.device];
			delete tempDev;
			return DTAERROR_COMMAND_ERROR;
		}
		if (tempDev->isOpal2())
			d = new DtaDevOpal2(argv[opts.device]);
		else
			if (tempDev->isOpal1())
				d = new DtaDevOpal1(argv[opts.device]);
			else
				if (tempDev->isEprise())
					d = new DtaDevEnterprise(argv[opts.device]);
				else
				{
					LOG(E) << "Unknown OPAL SSC ";
					return DTAERROR_INVALID_COMMAND;
				}
		delete tempDev;
		if (NULL == d) {
			LOG(E) << "Create device object failed";
			return DTAERROR_OBJECT_CREATE_FAILED;
		}
		// make sure DtaDev::no_hash_passwords is initialized
		d->no_hash_passwords = opts.no_hash_passwords;

		d->output_format = opts.output_format;
	}

    switch (opts.action) {
 	case sedutiloption::initialSetup:
		LOG(D) << "Performing initial setup to use sedutil on drive " << argv[opts.device];
        return (d->initialSetup(GET_PASSWORD(), opts.secure_mode));
	case sedutiloption::setup_SUM:
		LOG(D) << "Performing SUM setup on drive " << argv[opts.device];
		return (d->setup_SUM(opts.lockingrange, atoll(argv[opts.lrstart]),
			atoll(argv[opts.lrlength]), GET_PASSWORD(), GET_NEW_PASSWORD(), opts.secure_mode));
		break;
	case sedutiloption::setSIDPassword:
        LOG(D) << "Performing setSIDPassword ";
        return d->setSIDPassword(GET_PASSWORD(), GET_NEW_PASSWORD(), opts.no_hash_passwords? 0 : 1, opts.no_hash_passwords? 0 : 1, opts.secure_mode);
		break;
    case sedutiloption::verifySIDPassword:
        LOG(D) << "Performing verifySIDPassword ";
        return d->verifySIDPassword(GET_PASSWORD(), !opts.no_hash_passwords, opts.secure_mode);
        break;
	case sedutiloption::setAdmin1Pwd:
        LOG(D) << "Performing setPAdmin1Pwd ";
        return d->setPassword(GET_PASSWORD(), (char *) "Admin1",
                            GET_NEW_PASSWORD(), opts.secure_mode);
		break;
	case sedutiloption::loadPBAimage:
        LOG(D) << "Loading PBA image " << argv[opts.pbafile] << " to " << opts.device;
        return d->loadPBA(GET_PASSWORD(), argv[opts.pbafile]);
		break;
	case sedutiloption::setLockingRange:
        LOG(D) << "Setting Locking Range " << (uint16_t) opts.lockingrange << " " << (uint16_t) opts.lockingstate;
        return d->setLockingRange(opts.lockingrange, opts.lockingstate, GET_PASSWORD());
		break;
	case sedutiloption::setLockingRange_SUM:
		LOG(D) << "Setting Locking Range " << (uint16_t)opts.lockingrange << " " << (uint16_t)opts.lockingstate << " in Single User Mode";
		return d->setLockingRange_SUM(opts.lockingrange, opts.lockingstate, GET_PASSWORD());
		break;
	case sedutiloption::enableLockingRange:
        LOG(D) << "Enabling Locking Range " << (uint16_t) opts.lockingrange;
        return (d->configureLockingRange(opts.lockingrange,
			(DTA_READLOCKINGENABLED | DTA_WRITELOCKINGENABLED), GET_PASSWORD()));
        break;
	case sedutiloption::disableLockingRange:
		LOG(D) << "Disabling Locking Range " << (uint16_t) opts.lockingrange;
		return (d->configureLockingRange(opts.lockingrange, DTA_DISABLELOCKING,
			GET_PASSWORD()));
		break;
	case sedutiloption::readonlyLockingRange:
		LOG(D) << "Enabling Locking Range " << (uint16_t)opts.lockingrange;
		return (d->configureLockingRange(opts.lockingrange,
			DTA_WRITELOCKINGENABLED, GET_PASSWORD()));
		break;
	case sedutiloption::setupLockingRange:
		LOG(D) << "Setup Locking Range " << (uint16_t)opts.lockingrange;
		return (d->setupLockingRange(opts.lockingrange, atoll(argv[opts.lrstart]),
			atoll(argv[opts.lrlength]), GET_PASSWORD()));
		break;
	case sedutiloption::setupLockingRange_SUM:
		LOG(D) << "Setup Locking Range " << (uint16_t)opts.lockingrange << " in Single User Mode";
		return (d->setupLockingRange_SUM(opts.lockingrange, atoll(argv[opts.lrstart]),
			atoll(argv[opts.lrlength]), GET_PASSWORD()));
		break;
	case sedutiloption::listLockingRanges:
		LOG(D) << "List Locking Ranges ";
		return (d->listLockingRanges(GET_PASSWORD(), -1));
		break;
	case sedutiloption::listLockingRange:
		LOG(D) << "List Locking Range[" << opts.lockingrange << "]";
		return (d->listLockingRanges(GET_PASSWORD(), opts.lockingrange));
		break;
    case sedutiloption::rekeyLockingRange:
		LOG(D) << "Rekey Locking Range[" << opts.lockingrange << "]";
		return (d->rekeyLockingRange(opts.lockingrange, GET_PASSWORD()));
        break;
    case sedutiloption::setBandsEnabled:
        LOG(D) << "Set bands Enabled";
        return (d->setBandsEnabled(-1, GET_PASSWORD()));
        break;
    case sedutiloption::setBandEnabled:
        LOG(D) << "Set band[" << opts.lockingrange << "] enabled";
        return (d->setBandsEnabled(opts.lockingrange, GET_PASSWORD()));
        break;
	case sedutiloption::setMBRDone:
		LOG(D) << "Setting MBRDone " << (uint16_t)opts.mbrstate;
		return (d->setMBRDone(opts.mbrstate, GET_PASSWORD()));
		break;
	case sedutiloption::setMBREnable:
		LOG(D) << "Setting MBREnable " << (uint16_t)opts.mbrstate;
		return (d->setMBREnable(opts.mbrstate, GET_PASSWORD()));
		break;
	case sedutiloption::enableuser:
        LOG(D) << "Performing enable user for user " << argv[opts.userid];
        return d->enableUser(GET_PASSWORD(), argv[opts.userid]);
        break;
	case sedutiloption::activateLockingSP:
		LOG(D) << "Activating the LockingSP on" << argv[opts.device];
        return d->activateLockingSP(GET_PASSWORD());
        break;
	case sedutiloption::activateLockingSP_SUM:
		LOG(D) << "Activating the LockingSP on" << argv[opts.device];
		return d->activateLockingSP_SUM(opts.lockingrange, GET_PASSWORD());
		break;
	case sedutiloption::eraseLockingRange_SUM:
		LOG(D) << "Erasing LockingRange " << opts.lockingrange << " on" << argv[opts.device];
		return d->eraseLockingRange_SUM(opts.lockingrange, GET_PASSWORD());
		break;
    case sedutiloption::query:
		LOG(D) << "Performing diskquery() on " << argv[opts.device];
        d->puke();
        return 0;
        break;
	case sedutiloption::scan:
        LOG(D) << "Performing diskScan() ";
        return(DtaDevOS::diskScan());
        break;
	case sedutiloption::isValidSED:
		LOG(D) << "Verify whether " << argv[opts.device] << "is valid SED or not";
        return isValidSEDDisk(argv[opts.device]);
        break;
	case sedutiloption::takeOwnership:
		LOG(D) << "Taking Ownership of the drive at" << argv[opts.device];
        return d->takeOwnership(GET_PASSWORD(), opts.secure_mode);
        break;
 	case sedutiloption::revertLockingSP:
		LOG(D) << "Performing revertLockingSP on " << argv[opts.device];
        return d->revertLockingSP(GET_PASSWORD(), 0);
        break;
	case sedutiloption::setPassword:
        LOG(D) << "Performing setPassword for user " << argv[opts.userid];
        return d->setPassword(GET_PASSWORD(), argv[opts.userid],
                              GET_NEW_PASSWORD(), opts.secure_mode);
        break;
	case sedutiloption::setPassword_SUM:
		LOG(D) << "Performing setPassword in SUM mode for user " << argv[opts.userid];
		return d->setNewPassword_SUM(GET_PASSWORD(), argv[opts.userid],
			GET_NEW_PASSWORD(), opts.secure_mode);
		break;
	case sedutiloption::revertTPer:
		LOG(D) << "Performing revertTPer on " << argv[opts.device];
        return d->revertTPer(GET_PASSWORD(), 0, 0);
        break;
	case sedutiloption::revertNoErase:
		LOG(D) << "Performing revertLockingSP  keep global locking range on " << argv[opts.device];
		return d->revertLockingSP(GET_PASSWORD(), 1);
		break;
	case sedutiloption::validatePBKDF2:
        LOG(D) << "Performing PBKDF2 validation ";
        TestPBKDF2();
        break;
	case sedutiloption::yesIreallywanttoERASEALLmydatausingthePSID:
	case sedutiloption::PSIDrevert:
		LOG(D) << "Performing a PSID Revert on " << argv[opts.device] << " with password " << GET_PASSWORD();
        return d->revertTPer(GET_PASSWORD(), 1, 0);
        break;
	case sedutiloption::PSIDrevertAdminSP:
		LOG(D) << "Performing a PSID RevertAdminSP on " << argv[opts.device] << " with password " << GET_PASSWORD();
        return d->revertTPer(GET_PASSWORD(), 1, 1);
        break;
	case sedutiloption::eraseLockingRange:
		LOG(D) << "Erase Locking Range " << (uint16_t)opts.lockingrange;
		return (d->eraseLockingRange(opts.lockingrange, GET_PASSWORD()));
		break;
	case sedutiloption::objDump:
		LOG(D) << "Performing objDump " ;
		return d->objDump(argv[argc - 5], argv[argc - 4], argv[argc - 3], argv[argc - 2]);
		break;
    case sedutiloption::printDefaultPassword:
		LOG(D) << "print default password";
        return d->printDefaultPassword();
        break;
	case sedutiloption::rawCmd:
		LOG(D) << "Performing cmdDump ";
		return d->rawCmd(argv[argc - 7], argv[argc - 6], argv[argc - 5], argv[argc - 4], argv[argc - 3], argv[argc - 2]);
		break;
    default:
        LOG(E) << "Unable to determine what you want to do ";
        usage();
    }
	return DTAERROR_INVALID_COMMAND;
}
