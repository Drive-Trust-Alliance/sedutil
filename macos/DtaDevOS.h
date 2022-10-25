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

    

    /** OS specific routine to identify the device and fill out the device information struct
     */
    void identify(void);
    

        /** OS specific method to send an ATA command to the device
         * @param cmd ATA command to be sent to the device
         * @param protocol security protocol to be used in the command
         * @param comID communications ID to be used
         * @param buffer input/output buffer
         * @param bufferlen length of the input/output buffer
         */
    uint8_t sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                    void * buffer, uint32_t bufferlen);
    /** A static class to scan for supported drives */
    static int diskScan();
    
    const char * serialNumber() { return tPer->getSerialNum();}
    const char * bsdName() { return tPer->getBSDName();}
    const char * modelNumber() { return tPer->getModelNum();}
    const char * firmwareRevision() { return tPer->getFirmwareRev();}

protected:
    /** OS specific command to Wait for specified number of milliseconds 
     * @param ms  number of milliseconds to wait
     */
    void osmsSleep(uint32_t ms);
    /** return drive size in bytes */
    const unsigned long long getSize();

private:
    bool __init(const char *devref);
    
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
