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
#include <iostream>
#include "os.h"
#include "MsedHashPwd.h"
#include "MsedOptions.h"
#include "MsedLexicon.h"
#include "MsedDevGeneric.h"
#include "MsedDevOpal1.h"
#include "MsedDevOpal2.h"
#include "MsedDevEnterprise.h"

using namespace std;

int diskScan()
{
	char devname[25];
	int i = 0;
	MsedDev * d;
	LOG(D1) << "Creating diskList";
	printf("\nScanning for Opal compliant disks\n");
	while (TRUE) {
		DEVICEMASK;
		//snprintf(devname,23,"/dev/sd%c",(char) 0x61+i) Linux
		//sprintf_s(devname, 23, "\\\\.\\PhysicalDrive%i", i)  Windows
		d = new MsedDevGeneric(devname);
		if (d->isPresent()) {
			printf("%s", devname);
			if (d->isAnySSC())
				printf(" %s%s%s ", (d->isOpal1() ? "1" : " "),
				(d->isOpal2() ? "2" : " "), (d->isEprise() ? "E" : " "));
			else
				printf("%s", " No  ");
			cout << d->getModelNum() << " " << d->getFirmwareRev() << std::endl;
			if (MAX_DISKS == i) {
				LOG(I) << MAX_DISKS << " disks, really?";
				delete d;
				return 1;
			}
		}
		else break;
		delete d;
		i += 1;
	}
	delete d;
	printf("No more disks present ending scan\n");
	return 0;
}
int main(int argc, char * argv[])
{
	MSED_OPTIONS opts;
	MsedDev *tempDev = NULL, *d = NULL;
	if (MsedOptions(argc, argv, &opts)) {
		return MSEDERROR_COMMAND_ERROR;
	}

	if ((opts.action != msedoption::scan) && (opts.action != msedoption::validatePBKDF2)) {
		if (opts.device > (argc - 1)) opts.device = 0;
		tempDev = new MsedDevGeneric(argv[opts.device]);
		if (NULL == tempDev) {
			LOG(E) << "Create device object failed";
			return MSEDERROR_OBJECT_CREATE_FAILED;
		}
		if ((!tempDev->isPresent()) || (!tempDev->isAnySSC())) {
			LOG(E) << "Invalid or unsupported disk " << argv[opts.device];
			delete tempDev;
			return MSEDERROR_COMMAND_ERROR;
		}
		if (tempDev->isOpal2())
			d = new MsedDevOpal2(argv[opts.device]);
		else
			if (tempDev->isOpal1())
				d = new MsedDevOpal1(argv[opts.device]);
			else
				if (tempDev->isEprise())
					d = new MsedDevEnterprise(argv[opts.device]);
				else
				{
					LOG(E) << "Unknown OPAL SSC ";
					return MSEDERROR_INVALID_COMMAND;
				}
		delete tempDev;
		if (NULL == d) {
			LOG(E) << "Create device object failed";
			return MSEDERROR_OBJECT_CREATE_FAILED;
		}
	}
	if (opts.no_hash_passwords) {
		d->no_hash_passwords = true;
	}
    switch (opts.action) {
 	case msedoption::initialsetup:
		LOG(D) << "Performing initial setup to use msed on drive " << argv[opts.device];
        return (d->initialsetup(argv[opts.password]));
	case msedoption::setup_SUM:
		LOG(D) << "Performing SUM setup on drive " << argv[opts.device];
		return (d->setup_SUM(opts.lockingrange, atoll(argv[opts.lrstart]),
			atoll(argv[opts.lrlength]), argv[opts.password], argv[opts.newpassword]));
		break;
	case msedoption::setSIDPwd:
        LOG(D) << "Performing setSIDPwd ";
        return d->setSIDPassword(argv[opts.password], argv[opts.newpassword]);
		break;
	case msedoption::setAdmin1Pwd:
        LOG(D) << "Performing setPAdmin1Pwd ";
        return d->setNewPassword(argv[opts.password], (char *) "Admin1",
                            argv[opts.newpassword]);
		break;
	case msedoption::loadPBAimage:
        LOG(D) << "Loading PBA image " << argv[opts.pbafile] << " to " << opts.device;
        return d->loadPBA(argv[opts.password], argv[opts.pbafile]);
		break;
	case msedoption::setLockingRange:
        LOG(D) << "Setting Locking Range " << (uint16_t) opts.lockingrange << " " << (uint16_t) opts.lockingstate;
        return d->setLockingRange(opts.lockingrange, opts.lockingstate, argv[opts.password]);
		break;
	case msedoption::setLockingRange_SUM:
		LOG(D) << "Setting Locking Range " << (uint16_t)opts.lockingrange << " " << (uint16_t)opts.lockingstate << " in Single User Mode";
		return d->setLockingRange_SUM(opts.lockingrange, opts.lockingstate, argv[opts.password]);
		break;
	case msedoption::enableLockingRange:
        LOG(D) << "Enabling Locking Range " << (uint16_t) opts.lockingrange;
        return (d->configureLockingRange(opts.lockingrange,
			(MSED_READLOCKINGENABLED | MSED_WRITELOCKINGENABLED), argv[opts.password]));
        break;
	case msedoption::disableLockingRange:
		LOG(D) << "Disabling Locking Range " << (uint16_t) opts.lockingrange;
		return (d->configureLockingRange(opts.lockingrange, MSED_DISABLELOCKING,
			argv[opts.password]));
		break;
	case msedoption::readonlyLockingRange:
		LOG(D) << "Enabling Locking Range " << (uint16_t)opts.lockingrange;
		return (d->configureLockingRange(opts.lockingrange,
			MSED_WRITELOCKINGENABLED, argv[opts.password]));
		break;
	case msedoption::setupLockingRange:
		LOG(D) << "Setup Locking Range " << (uint16_t)opts.lockingrange;
		return (d->setupLockingRange(opts.lockingrange, atoll(argv[opts.lrstart]),
			atoll(argv[opts.lrlength]), argv[opts.password]));
		break;
	case msedoption::setupLockingRange_SUM:
		LOG(D) << "Setup Locking Range " << (uint16_t)opts.lockingrange << " in Single User Mode";
		return (d->setupLockingRange_SUM(opts.lockingrange, atoll(argv[opts.lrstart]),
			atoll(argv[opts.lrlength]), argv[opts.password]));
		break;
	case msedoption::listLockingRanges:
		LOG(D) << "List Locking Ranges ";
		return (d->listLockingRanges(argv[opts.password]));
		break;
	case msedoption::setMBRDone:
		LOG(D) << "Setting MBRDone " << (uint16_t)opts.mbrstate;
		return (d->setMBRDone(opts.mbrstate, argv[opts.password]));
		break;
	case msedoption::setMBREnable:
		LOG(D) << "Setting MBREnable " << (uint16_t)opts.mbrstate;
		return (d->setMBREnable(opts.mbrstate, argv[opts.password]));
		break;
	case msedoption::enableuser:
        LOG(D) << "Performing enable user for user " << argv[opts.userid];
        return d->enableUser(argv[opts.password], argv[opts.userid]);
        break;
	case msedoption::activateLockingSP:
		LOG(D) << "Activating the LockingSP on" << argv[opts.device];
        return d->activateLockingSP(argv[opts.password]);
        break;
	case msedoption::activateLockingSP_SUM:
		LOG(D) << "Activating the LockingSP on" << argv[opts.device];
		return d->activateLockingSP_SUM(opts.lockingrange, argv[opts.password]);
		break;
	case msedoption::eraseLockingRange_SUM:
		LOG(D) << "Erasing LockingRange " << opts.lockingrange << " on" << argv[opts.device];
		return d->eraseLockingRange_SUM(opts.lockingrange, argv[opts.password]);
		break;
    case msedoption::query:
		LOG(D) << "Performing diskquery() on " << argv[opts.device];
        d->puke();
        return 0;
        break;
	case msedoption::scan:
        LOG(D) << "Performing diskScan() ";
        diskScan();
        break;
	case msedoption::takeownership:
		LOG(D) << "Taking Ownership of the drive at" << argv[opts.device];
        return d->takeOwnership(argv[opts.password]);
        break;
 	case msedoption::revertLockingSP:
		LOG(D) << "Performing revertLockingSP on " << argv[opts.device];
        return d->revertLockingSP(argv[opts.password]);
        break;
	case msedoption::setPassword:
        LOG(D) << "Performing setPassword for user " << argv[opts.userid];
        return d->setNewPassword(argv[opts.password], argv[opts.userid],
                              argv[opts.newpassword]);
        break;
	case msedoption::setPassword_SUM:
		LOG(D) << "Performing setPassword in SUM mode for user " << argv[opts.userid];
		return d->setNewPassword_SUM(argv[opts.password], argv[opts.userid],
			argv[opts.newpassword]);
		break;
	case msedoption::reverttper:
		LOG(D) << "Performing revertTPer on " << argv[opts.device];
        return d->revertTPer(argv[opts.password]);
        break;
	case msedoption::revertnoerase:
		LOG(D) << "Performing revertLockingSP  keep global locking range on " << argv[opts.device];
		return d->revertLockingSP(argv[opts.password], 1);
		break;
	case msedoption::validatePBKDF2:
        LOG(D) << "Performing PBKDF2 validation ";
        MsedTestPBDKF2();
        break;
	case msedoption::yesIreallywanttoERASEALLmydatausingthePSID:
	case msedoption::PSIDrevert:
		LOG(D) << "Performing a PSID Revert on " << argv[opts.device] << " with password " << argv[opts.password];
        return d->revertTPer(argv[opts.password],1);
        break;
	case msedoption::objDump:
		LOG(D) << "Performing objDump " ;
		return d->objDump(argv[argc - 5], argv[argc - 4], argv[argc - 3], argv[argc - 2]);
		break;
	case msedoption::rawCmd:
		LOG(D) << "Performing cmdDump ";
		return d->rawCmd(argv[argc - 7], argv[argc - 6], argv[argc - 5], argv[argc - 4], argv[argc - 3], argv[argc - 2]);
		break;
    default:
        LOG(E) << "Unable to determine what you want to do ";
        usage();
    }
	return MSEDERROR_INVALID_COMMAND;
}
