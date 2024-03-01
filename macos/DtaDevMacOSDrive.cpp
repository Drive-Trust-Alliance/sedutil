/* C:B**************************************************************************
   This software is Copyright (c) 2014-2024 Bright Plaza Inc. <drivetrust@drivetrust.com>

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

#include <cstdint>
#include <cstring>
#include "os.h"
#include "log.h"
#include "dirent.h"
#include "DtaEndianFixup.h"
#include "DtaHexDump.h"
#include "DtaDevMacOSDrive.h"


int DtaDevMacOSDrive::fdopen(const char * devref)
{
  if (access(devref, R_OK | W_OK)) {
    LOG(E) << "You do not have permission to access the raw device in write mode";
    LOG(E) << "Perhaps you might try sudo to run as root";
  }

  int fd = open(devref, O_RDWR);

  if (fd < 0) {
    LOG(E) << "Error opening device " << devref << " " << (int32_t) fd;
    //        if (-EPERM == fd) {
    //            LOG(E) << "You do not have permission to access the raw disk in write mode";
    //            LOG(E) << "Perhaps you might try sudo to run as root";
    //        }
  }
  return fd;
}

void DtaDevMacOSDrive::fdclose()
{
  if (0 <= fd) {
    LOG(D4) << "Closing device file handle " << (int32_t) fd;
    close(fd);
  }
}

using namespace std;
vector<string> DtaDevMacOSDrive::enumerateDtaDevMacOSDriveDevRefs()
{
  vector<string> devices;

  DIR *dir = opendir("/dev");
  if (dir==NULL) {
    LOG(E) << "Can't read /dev ?!";
    return devices;
  }

  struct dirent *dirent;
  while (NULL != (dirent=readdir(dir))) {
    char devref[261];
    snprintf(devref,sizeof(devref),"/dev/%s",dirent->d_name);
    if (isDtaDevOSDriveDevRef(devref))
      devices.push_back(string(devref));
  }

  closedir(dir);

  std::sort(devices.begin(),devices.end());

  return devices;
}
