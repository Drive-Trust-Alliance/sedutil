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
#if defined(__APPLE__) && defined(__MACH__)
    /* Apple OSX and iOS (Darwin). ------------------------------ */
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR == 1
    /* iOS in Xcode simulator */

#elif TARGET_OS_IPHONE == 1
    /* iOS on iPhone, iPad, etc. */

#elif TARGET_OS_MAC == 1
    /* OSX */
#include "DtaDevMacOSTPer.h"
#endif
#endif  // defined(__APPLE__) && defined(__MACH__)

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
    
#if defined(__APPLE__) && defined(__MACH__)
    /* Apple OSX and iOS (Darwin). ------------------------------ */
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR == 1
    /* iOS in Xcode simulator */

#elif TARGET_OS_IPHONE == 1
    /* iOS on iPhone, iPad, etc. */

#elif TARGET_OS_MAC == 1
    /* OSX */

    /** OS specific method to initialize an object to a pre-existing connection
     *  @param devref the name of the device in the OS lexicon
     *  @param driverService  the I/O Registry entry of the device
     *  @param connect the connection returned by Open
     */
    void init(const char * devref,
              io_registry_entry_t driverService,
              io_connect_t connect);

#endif
#endif  // defined(__APPLE__) && defined(__MACH__)
    
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
    
    virtual void puke() {
        identify();
        DtaDev::puke();
    }
    

#if defined(__APPLE__) && defined(__MACH__)
    /* Apple OSX and iOS (Darwin). ------------------------------ */
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR == 1
    /* iOS in Xcode simulator */

#elif TARGET_OS_IPHONE == 1
    /* iOS on iPhone, iPad, etc. */

#elif TARGET_OS_MAC == 1
    /* OSX */

    const char * vendorID() { return blockStorageDevice->getVendorID();}
    const char * serialNumber() { return blockStorageDevice->getSerialNum();}
    const char * bsdName() { return blockStorageDevice->getBSDName();}
    const char * modelNumber() { return blockStorageDevice->getModelNum();}
    const char * firmwareRevision() { return blockStorageDevice->getFirmwareRev();}
    const vector <uint8_t> worldWideName() { return blockStorageDevice->getWorldWideName();}
    vector<uint8_t> passwordSalt() { return tPer->getPasswordSalt();}
    /** return drive size in bytes */
    const unsigned long long getSize () {
        return blockStorageDevice->getSize();
    }

    const char * getPhysicalInterconnect()
    {
        return blockStorageDevice->getPhysicalInterconnect();
    }
    const char * getPhysicalInterconnectLocation()
    {
        return blockStorageDevice->getPhysicalInterconnectLocation();
    }

    // TODO: private with accessors?
    DtaDevMacOSBlockStorageDevice * blockStorageDevice;
    DtaDevMacOSTPer *tPer;

#endif
#endif  // defined(__APPLE__) && defined(__MACH__)


protected:
    /** OS specific command to Wait for specified number of milliseconds
     * @param ms  number of milliseconds to wait
     */
    void osmsSleep(uint32_t ms);

private:
    bool __init(const char *devref);
    bool __init(const char *devref, DTA_DEVICE_INFO &di);
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
