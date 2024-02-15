/* C:B**************************************************************************
   This software is Copyright 2014-2022 Bright Plaza Inc. <drivetrust@drivetrust.com>

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


#include "DtaDevOpal1.h"
#include "DtaDevOpal2.h"
#include "DtaDevEnterprise.h"
#include "DtaDevLinuxNvme.h"
#include "DtaDevLinuxSata.h"
#include "DtaDevGeneric.h"
// TODO: Below Just for Debugging
#include "DtaHexDump.h"



DtaDevOS* DtaDevOS::getDtaDevOS(const char * devref,
                                                DtaDevLinuxDrive * drive,
                                                DTA_DEVICE_INFO & di,
                                                bool genericIfNotTPer) {
  if (di.OPAL20)     return new DtaDevOpal2(devref, drive, di);
  if (di.OPAL10)     return new DtaDevOpal1(devref, drive, di);
  if (di.Enterprise) return new DtaDevEnterprise(devref, drive, di);
  //  if (di.RUBY) ...  etc.

  if (genericIfNotTPer) return new DtaDevGeneric(devref, drive, di);

  LOG(E) << "Unknown OPAL SSC ";
  return NULL;
}


/** Factory method to produce instance of appropriate subclass
 *   Note that all of DtaDevGeneric, DtaDevEnterprise, DtaDevOpal, ... derive from DtaDevOS
 * @param devref             name of the device in the OS lexicon
 * @param dev                reference into which to store the address of the new instance
 * @param genericIfNotTPer   if true, store an instance of DtaDevGeneric for non-TPers;
 *                           if false, store NULL for non-TPers
 */
// static
uint8_t DtaDevOS::getDtaDevOS(const char * devref,
                              DtaDevOS * & dev, bool genericIfNotTPer)
{
  LOG(D1) << "DtaDevOS::getDtaDevOS(devref=\"" << devref << "\")";
  DTA_DEVICE_INFO disk_info;
  bzero(&disk_info, sizeof(disk_info));

  DtaDevLinuxDrive * drive;

  drive = DtaDevLinuxDrive::getDtaDevLinuxDrive(devref, &disk_info);
  if (drive == NULL) {
    dev = NULL;
    LOG(E) << "Invalid or unsupported device " << devref;
    LOG(D1) << "DtaDevOS::getDtaDevOS(devref=\"" << devref << "\") returning DTAERROR_COMMAND_ERROR";
    return DTAERROR_COMMAND_ERROR;
  }

  dev =  getDtaDevOS(devref, drive, disk_info, genericIfNotTPer) ;
  if (dev == NULL) {
    delete drive;
    LOG(E) << "Invalid or unsupported device " << devref;
    LOG(D1) << "DtaDevOS::getDtaDevOS(devref=\"" << devref << "\") returning DTAERROR_COMMAND_ERROR";
    return DTAERROR_COMMAND_ERROR;
  }


  LOG(D1) << "DtaDevOS::getDtaDevOS(devref=\"" << devref << "\") disk_info:";
  DtaHexDump(&disk_info, (int)sizeof(disk_info));
  LOG(D1) << "DtaDevOS::getDtaDevOS(devref=\"" << devref << "\") returning DTAERROR_SUCCESS";
  return DTAERROR_SUCCESS;
}


uint8_t DtaDev::getDtaDev(const char * devref, DtaDev * & device, bool genericIfNotTPer)
{
  DtaDevOS * d;
  uint8_t result = DtaDevOS::getDtaDevOS(devref, d, genericIfNotTPer);
  if (result == DTAERROR_SUCCESS) {
    device = static_cast<DtaDev *>(d);
  } else {
    device = NULL ;
  }
  return result;
}
