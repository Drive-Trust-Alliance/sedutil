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
#pragma once

#include "DtaDev.h"
#include "DtaDevMacOSTPer.h"

/** MacOS specific implementation of DtaDevOS.
 */
class DtaDevOS : public DtaDev {
public:
    /** Default constructor */
    DtaDevOS();
    /** Destructor */
    ~DtaDevOS();
    
    /** Factory method to produce instance of appropriate subclass
     *   Note that all of DtaDevGeneric, DtaDevEnterprise, DtaDevOpal, ... derive from DtaDevOS
     * @param devref                         name of the device in the OS lexicon
     * @param dev                                reference into which to store the address of the new instance
     * @param genericIfNotTPer   if true, store an instance of DtaDevGeneric for non-TPers;
     *                          if false, store NULL for non-TPers
     */
    static uint8_t getDtaDevOS(const char * devref, DtaDevOS * & dev,
                               bool genericIfNotTPer=false);


    /** OS specific initialization.
     * This function should perform the necessary authority and environment checking
     * to allow proper functioning of the program, open the device, perform an ATA
     * identify, add the fields from the identify response to the disk info structure
     * and if the device is an ATA device perform a call to Discovery0() to complete
     * the disk_info structure
     * @param devref character representation of the device is standard OS lexicon
     */
    void init(const char * devref);
    
    /** OS specific method to initialize an object to a pre-existing connection
     *  @param devref the name of the device in the OS lexicon
     *  @param driverService  the I/O Registry entry of the device
     *  @param connect the connection returned by Open
     */
    void init(const char * devref,
              io_registry_entry_t driverService,
              io_connect_t connect);
    
    /** OS specific method to initialize an object to a pre-existing connection
     *  @param di  reference to already-initialized DTA_DEVICE_INFO
     */
    void init(const char * devref,
              DTA_DEVICE_INFO &di);

    

    /** OS specific routine to identify the device and fill out the device information struct
     */
    void identify();
    

        /** OS specific method to send an ATA command to the device
         * @param cmd ATA command to be sent to the device
         * @param protocol security protocol to be used in the command
         * @param comID communications ID to be used
         * @param buffer input/output buffer
         * @param bufferlen length of the input/output buffer
         */
    uint8_t sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                    void * buffer, size_t bufferlen);
    /** A static class to scan for supported drives */
    static int diskScan();
    
    const char * serialNumber() { return tPer->getSerialNum();}
    const char * bsdName() { return tPer->getBSDName();}
    const char * modelNumber() { return tPer->getModelNum();}
    const char * firmwareRevision() { return tPer->getFirmwareRev();}
    vector<uint8_t> passwordSalt() { return tPer->getPasswordSalt();}
    
    virtual void puke() {
        identify();
        DtaDev::puke();
    }

protected:
    
    /** return drive size in bytes */
    const unsigned long long getSize ();
    
    /** OS specific command to Wait for specified number of milliseconds
     * @param ms  number of milliseconds to wait
     */
    void osmsSleep(uint32_t ms);

private:
    bool __init(const char *devref);
    bool __init(const char *devref, DTA_DEVICE_INFO &di);
    DtaDevMacOSBlockStorageDevice * blockStorageDevice;
    DtaDevMacOSTPer *tPer;
};


////////////////////////////////////////////////////////////////////////////////
static inline void set8(vector<uint8_t> & v, const uint8_t value[8])
////////////////////////////////////////////////////////////////////////////////
{
    v.clear();
    v.push_back(OPAL_SHORT_ATOM::BYTESTRING8);
    for (int i = 0; i < 8; i++)
    {
        v.push_back(value[i]);
    }
}

static inline vector<uint8_t> vUID(OPAL_UID uid) {
    vector<uint8_t> v(9);
    set8(v,OPALUID[uid]);
    return v;
}

static __inline bool __is_not_all_NULs(const uint8_t * b, const size_t n) {
    for(const uint8_t * e = b + n; b<e; b++) {
        if (0!=*b)
            return true;
    }
    return false;
}


static __inline bool __is_not_all_zeroes(const uint8_t * b, const size_t n) {
    for(const uint8_t * e = b + n; b<e; b++) {
        if (0!=*b && '0'!=*b)
            return true;
    }
    return false;
}
