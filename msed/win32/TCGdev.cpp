/* C:B**************************************************************************
This software is Copyright 2014 Michael Romeo <r0m30@r0m30.com>

This file is part of msed.

msed is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

msed is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with msed.  If not, see <http://www.gnu.org/licenses/>.

* C:E********************************************************************** */
#include "os.h"
#include <stdio.h>
#include <iostream>
#include <Ntddscsi.h>
#include "TCGdev.h"
#include "endianfixup.h"
#include "TCGstructures.h"
#include "hexDump.h"

using namespace std;

/** Device Class (win32) represents a single disk device.
 *  This class is stored in a vector so to avoid writing a
 *  copy operator and an assignment operator no custom destructor
 *  is used leading to this unfortunate class method structure
 */
TCGdev::TCGdev(const char * devref)
{
    LOG(D4) << "Creating TCGdev::TCGdev() " << devref;
    ATA_PASS_THROUGH_DIRECT * ata =
            (ATA_PASS_THROUGH_DIRECT *) _aligned_malloc(sizeof (ATA_PASS_THROUGH_DIRECT), 8);
    ataPointer = (void *) ata;

    dev = devref;
    memset(&disk_info, 0, sizeof (TCG_DiskInfo));
    hDev = CreateFile(dev,
                      GENERIC_WRITE | GENERIC_READ,
                      FILE_SHARE_WRITE | FILE_SHARE_READ,
                      NULL,
                      OPEN_EXISTING,
                      0,
                      NULL);
    if (INVALID_HANDLE_VALUE == hDev) {
        DWORD err = GetLastError();
        // This is a D1 because diskscan looks for open fail to end scan
        LOG(D1) << "Error opening device " << dev << " Error " << err;
    }
    else {
        isOpen = TRUE;
        discovery0();
		identify();
    }
}

/** Send an ioctl to the device using pass through. */
uint8_t TCGdev::sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                      void * buffer, uint16_t bufferlen)
{
    LOG(D4) << "Entering TCGdev::sendCmd";
    DWORD bytesReturned = 0; // data returned
    if (!isOpen) {
        LOG(D1) << "Device open failed";
        return 0xff; //disk open failed so this will too
    }
    /*
     * Initialize the ATA_PASS_THROUGH_DIRECT structures
     * per windows DOC with the special sauce from the
     * ATA Command set reference (protocol and commID)
     */
    ATA_PASS_THROUGH_DIRECT * ata = (ATA_PASS_THROUGH_DIRECT *) ataPointer;
    memset(ata, 0, sizeof (ATA_PASS_THROUGH_DIRECT));
    ata->Length = sizeof (ATA_PASS_THROUGH_DIRECT);
	if (IF_RECV == cmd)
		ata->AtaFlags = 0x00 | ATA_FLAGS_DRDY_REQUIRED | ATA_FLAGS_DATA_IN;
	else if (IDENTIFY == cmd)
		ata->AtaFlags = ATA_FLAGS_DATA_IN;
	else
        ata->AtaFlags = 0x00 | ATA_FLAGS_DRDY_REQUIRED | ATA_FLAGS_DATA_OUT;

    ata->DataBuffer = buffer;
    ata->DataTransferLength = bufferlen;
    ata->TimeOutValue = 300;
	/* these were a b**** to find  defined in TCG specs but location is defined
	 * in ATA spec */
	ata->CurrentTaskFile[0] = protocol; // Protocol
	ata->CurrentTaskFile[1] = uint8_t(bufferlen / 512); // Payload in number of 512 blocks
	// Damn self inflicted endian bugs
	// The comID is passed in host endian format in the taskfile
	// don't know why?? Translated later?
	ata->CurrentTaskFile[3] = (comID & 0x00ff); // Commid LSB
	ata->CurrentTaskFile[4] = ((comID & 0xff00) >> 8); // Commid MSB
    ata->CurrentTaskFile[6] = (uint8_t) cmd; // ata Command (0x5e or ox5c)
    //LOG(D4) << "ata before";
    //IFLOG(D4) hexDump(ata, sizeof (ATA_PASS_THROUGH_DIRECT));
    DeviceIoControl(hDev, // device to be queried
                    IOCTL_ATA_PASS_THROUGH_DIRECT, // operation to perform
                    ata, sizeof (ATA_PASS_THROUGH_DIRECT),
                    ata, sizeof (ATA_PASS_THROUGH_DIRECT),
                    &bytesReturned, // # bytes returned
                    (LPOVERLAPPED) NULL); // synchronous I/O
    //LOG(D4) << "ata after";
    //IFLOG(D4) hexDump(ata, sizeof (ATA_PASS_THROUGH_DIRECT));
    return (ata->CurrentTaskFile[0]);
}
void TCGdev::osmsSleep(uint32_t milliseconds) {
	Sleep(milliseconds);
}
/** Close the filehandle so this object can be delete. */
TCGdev::~TCGdev()
{
    LOG(D4) << "Destroying TCGdev";
    CloseHandle(hDev);
    _aligned_free(ataPointer);
}