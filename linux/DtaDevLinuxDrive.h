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
#pragma once

#include "DtaDevOSDrive.h"
#include <errno.h>
#include <fcntl.h>

/** virtual implementation for a disk interface-generic disk drive
 */
class DtaDevLinuxDrive : public DtaDevOSDrive {
public:

  /** Factory function to look at the devref to filter out whether it could be a DtaDevLinuxDrive
   *
   * @param devref OS device reference e.g. "/dev/sda"
   */

  static bool isDtaDevLinuxDriveDevRef(const char * devref);

  /** Factory function to look at the devref and create an instance of the appropriate subclass of
   *  DtaDevLinuxDrive
   *
   * @param devref OS device reference e.g. "/dev/sda"
   * @param pdisk_info weak reference to DTA_DEVICE_INFO structure filled out during device identification
   */
  static DtaDevLinuxDrive * getDtaDevLinuxDrive(const char * devref,
                                          DTA_DEVICE_INFO & disk_info);


  bool isOpen(void) {return 0<fd && (fcntl(fd, F_GETFL) != -1 || errno != EBADF);}

  DtaDevLinuxDrive(int _fd) :fd(_fd) {}

  virtual ~DtaDevLinuxDrive() {fdclose();}


protected:

  static int fdopen(const char * devref);
  void fdclose(void);

public:  // *** TODO *** DEBUGGING *** this should just be protected
  int fd=0; /**< Linux handle for the device  */

};
