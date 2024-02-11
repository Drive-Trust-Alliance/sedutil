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


static DtaDevLinuxDrive* getDtaDevLinuxDriveSubclassInstance(const char * devref) {
 if (!strncmp(devref, "/dev/nvme", 9))
    {
      return new DtaDevLinuxNvme();
    }
  else if (!strncmp(devref, "/dev/s", 6))
    {
      return new DtaDevLinuxSata();
    }
  else
    {
      return NULL;
    }
}

static DtaDevOS* getDtaDevOSSubclassInstance(DtaDevLinuxDrive * d,
                                             const char * devref,
                                             bool genericIfNotTPer) {
    if (t->isOpal2())     return new DtaDevOpal2(devref);
    if (t->isOpal1())     return new DtaDevOpal1(devref);
    if (t->isEprise())    return new DtaDevEnterprise(devref);
//  if (t->isRuby()) ...  etc.

    if (genericIfNotTPer) return new DtaDevGeneric(devref);

    LOG(E) << "Unknown OPAL SSC ";
    return NULL;
}


/** Factory method to produce instance of appropriate subclass
 *   Note that all of DtaDevGeneric, DtaDevEnterprise, DtaDevOpal, ... derive from DtaDevOS
 * @param devref             name of the device in the OS lexicon
 * @param dev                reference into which to store the address of the new instance
 * @param genericIfNotTPer   if true, store an instance of DtaDevGeneric for non-TPers;
 *                          if false, store NULL for non-TPers
 */
// static
uint8_t DtaDevOS::getDtaDevOS(const char * devref, DtaDevOS * & dev,
                              bool genericIfNotTPer)
{
  DtaDevLinuxDrive * drive;

  drive = getDtaDevLinuxDriveSubclassInstance(devref);
  if (!drive) {
    dev = NULL;
    LOG(E) << "Invalid or unsupported device " << devref;
    return DTAERROR_COMMAND_ERROR;;
  }

  if (!(drive->init(devref))) {
    delete drive;
    dev = NULL;
    LOG(E) << "Invalid or unsupported device " << devref;
    return DTAERROR_COMMAND_ERROR;;
  }


  drive->identify(d->disk_info);


  dev =  getDtaDevOSSubclassInstance(DtaDevLinuxDrive drive, devref, genericIfNotTPer) ;

  delete drive ;

  if (!dev) {
    LOG(E) << "Invalid or unsupported device " << devref;
    return DTAERROR_COMMAND_ERROR;;
  }

  return DTAERROR_SUCCESS;
}


uint8_t DtaDev::getDtaDev(const char * devref, DtaDev * & device, bool genericIfNotTPer)
{
    DtaDevOS * d;
    uint8_t result = DtaDevOS::getDtaDevOS(devref, d, genericIfNotTPer);
    if (result == DTAERROR_SUCCESS) {
        device = static_cast<DtaDev *>(d);
    }
    return result;
}

#endif
#endif // defined(__APPLE__) && defined(__MACH__)
