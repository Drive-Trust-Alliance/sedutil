/* C:B**************************************************************************
Copyright 2017, Alex Badics

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
#include "os.h"
#include <sys/ioctl.h>
#include <linux/sed-opal.h>
#include "DtaDevLinuxDrive.h"

using namespace std;

uint8_t DtaDevLinuxDrive::prepareForS3Sleep(uint8_t lockingrange, const vector<uint8_t> &password_hash)
{
    LOG(D1) << "Entering DtaDevLinuxDrive::prepareForS3Sleep";

    opal_lock_unlock opal_ioctl_data={};
    opal_ioctl_data.l_state = OPAL_RW;
    opal_ioctl_data.session.who = OPAL_ADMIN1;
    opal_ioctl_data.session.opal_key.lr = 0;

    size_t hash_len=min(password_hash.size(), sizeof(opal_ioctl_data.session.opal_key.key));
    LOG(D2) << "Setting a hash of length" << hash_len;

    memcpy(opal_ioctl_data.session.opal_key.key, &password_hash[0], hash_len);
    opal_ioctl_data.session.opal_key.key_len = hash_len;

    int err = ioctl(fd, IOC_OPAL_SAVE, &opal_ioctl_data);
    if (err < 0)
        return errno;
    return 0;
}
