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
#include <IOKit/IOKitLib.h>
#include "DtaDevOSDrive.h"

/** virtual implementation for a disk interface-generic disk drive
 */
class DtaDevMacOSDrive : public DtaDevOSDrive {
    using DtaDevOSDrive::DtaDevOSDrive;
    
public:

    DtaDevMacOSDrive(io_registry_entry_t dS, io_connect_t c)
    : DtaDevOSDrive::DtaDevOSDrive()
    , driverService (dS)
    , connection (c)
    {};

  /** Factory function to look at the devref to filter out whether it could be a DtaDevMacOSDrive
   *
   * @param devref OS device reference e.g. "/dev/sda"
   */

  static bool isDtaDevMacOSDriveDevRef(const char * devref);

    /** Factory function to enumerate all the devrefs that pass the above filter
     *
     */
    static
    std::vector<std::string> enumerateDtaDevMacOSDriveDevRefs(void);

    /** Factory function to look at the devref and create an instance of the appropriate subclass of
   *  DtaDevMacOSDrive
   *
   * @param devref OS device reference e.g. "/dev/sda"
   * @param disk_info reference to DTA_DEVICE_INFO structure filled out during device identification
   */
  static DtaDevMacOSDrive * getDtaDevMacOSDrive(const char * devref,
                                                DTA_DEVICE_INFO & disk_info);


    bool isOpen(void) { return ( driverService != IO_OBJECT_NULL && connection != IO_OBJECT_NULL ) ;}


  virtual ~DtaDevMacOSDrive() {fdclose();}


protected:

  static io_connect_t fdopen(const char * devref, io_registry_entry_t & dS);

  void fdclose(void);

    /** Close the device reference so this object can be delete. */
    void ClearIOObjects(void);
    /** Close the device reference so this object can be delete. */
    void ReleaseIOObjects(void);
    
    io_registry_entry_t driverService;
    io_connect_t connection;
//    bool ownDriverService;
//    bool ownConnection;

};
