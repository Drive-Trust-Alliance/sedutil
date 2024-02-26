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
#include "DtaDev.h"
#include "DtaDevOSDrive.h"

/** Linux specific implementation of DtaDevOS.
 */
class DtaDevOS : public DtaDev {
public:
  /** Destructor */
  ~DtaDevOS();


  /** Factory method to produce instance of appropriate subclass
   *   Note that all of DtaDevGeneric, DtaDevEnterprise, DtaDevOpal, ... derive from DtaDevOS
   * @param devref             name of the device in the OS lexicon
   * @param dev                reference into which to store the address of the new instance
   * @param genericIfNotTPer   if true, store an instance of DtaDevGeneric for non-TPers;
   *                          if false, store NULL for non-TPers
   */
  static uint8_t getDtaDevOS(const char * devref, DtaDevOS * & dev,
                             bool genericIfNotTPer=false);




  /** OS specific method to send an ATA command to the device
   * @param cmd ATA command to be sent to the device
   * @param protocol security protocol to be used in the command
   * @param comID communications ID to be used
   * @param buffer input/output buffer
   * @param bufferlen length of the input/output buffer
   */
  uint8_t sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                  void * buffer, size_t bufferlen);

  /** A static function to scan for supported drives */
  static int diskScan();

  /** Short-circuit routine re-uses initialized drive and disk_info */
  DtaDevOS(const char * devref, DtaDevOSDrive * drv, DTA_DEVICE_INFO & di)
    : drive(drv)
  {dev=devref; disk_info=di; isOpen=(drv!=NULL && drv->isOpen());};

protected:
  /** Minimal internal type-switching routine
   *
   *  Checks that the drive responds to discovery0.
   *  Assumes all the identify/discovery0 information is thus in di,
   *  and that drive has a file descriptor open on devref.
   *  Just tests di SSC bits and picks DtaDevOS subclass accordingly.
   *  Otherwise return NULL, or
   *  a DtaDevGeneric instance if genericIfNotTper is true.
   */
  static DtaDevOS* getDtaDevOS(const char * devref,
                               DtaDevOSDrive * drive,
                               DTA_DEVICE_INFO & di,
                               bool genericIfNotTPer=false);

  /** OS specific command to Wait for specified number of milliseconds
   * @param ms  number of milliseconds to wait
   */
  void osmsSleep(uint32_t ms);

  /** OS specific routine to send an ATA identify to the device */
  bool identify(DTA_DEVICE_INFO& disk_info);

  /** return drive size in bytes */
  const unsigned long long getSize();

private:
  /* Protocol-specific subclass instance -- Nvme, Scsi, Sata, ... */
  DtaDevOSDrive *drive;

  // /** Default constructor */
  // DtaDevOS()
  //   : drive(NULL)
  // { dev=NULL;
  //   isOpen=FALSE;
  //   bzero(&disk_info, sizeof(disk_info));
  //   assert(FALSE);  // ***TODO*** this is never used
  // };
};
