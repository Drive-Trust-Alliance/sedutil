/* C:B**************************************************************************
This software is Copyright 2014-2017 Bright Plaza Inc. <drivetrust@drivetrust.com>

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
#include "os.h"
#include <stdio.h>
#include <iostream>
#pragma warning(push)
#pragma warning(disable : 4091)
#include <Ntddscsi.h>
#pragma warning(pop)
#include <vector>
#include "DtaDiskATA.h"
#include "DtaEndianFixup.h"
#include "DtaStructures.h"
#include "DtaHexDump.h"

using namespace std;
DtaDiskATA::DtaDiskATA() {};
void DtaDiskATA::init(const char * devref)
{
    LOG(D1) << "Creating DtaDiskATA::DtaDiskATA() " << devref;
    ATA_PASS_THROUGH_DIRECT * ata =
            (ATA_PASS_THROUGH_DIRECT *) _aligned_malloc(sizeof (ATA_PASS_THROUGH_DIRECT), 8);
    ataPointer = (void *) ata;
     hDev = CreateFile(devref,
                      GENERIC_WRITE | GENERIC_READ,
                      FILE_SHARE_WRITE | FILE_SHARE_READ,
                      NULL,
                      OPEN_EXISTING,
                      0,
                      NULL);
    if (INVALID_HANDLE_VALUE == hDev) 
		return;
    else 
        isOpen = TRUE;
}
uint8_t DtaDiskATA::sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                        void * buffer, uint32_t bufferlen)
{
    LOG(D1) << "Entering DtaDiskATA::sendCmd";
    DWORD bytesReturned = 0; // data returned
    if (!isOpen) {
        LOG(D1) << "Device open failed";
		return DTAERROR_OPEN_ERR; //disk open failed so this will too
    }
    /*
     * Initialize the ATA_PASS_THROUGH_DIRECT structures
     * per windows DOC with the special sauce from the
     * ATA Command set reference (protocol and comID)
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
    if (IDENTIFY != cmd) {
        ata->CurrentTaskFile[0] = protocol; // Protocol
        ata->CurrentTaskFile[1] = uint8_t(bufferlen / 512); // Payload in number of 512 blocks
        // Damn self inflicted endian bugs
        // The comID is passed in host endian format in the taskfile
        // don't know why?? Translated later?
        ata->CurrentTaskFile[3] = (comID & 0x00ff); // Commid LSB
        ata->CurrentTaskFile[4] = ((comID & 0xff00) >> 8); // Commid MSB
    }
    ata->CurrentTaskFile[6] = (uint8_t) cmd; // ata Command
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

/** adds the IDENTIFY information to the disk_info structure */

void DtaDiskATA::identify(OPAL_DiskInfo& disk_info)
{
    LOG(D1) << "Entering DtaDiskATA::identify()";
	vector<uint8_t> nullz(512, 0x00);
    void * identifyResp = NULL;
	identifyResp = _aligned_malloc(MIN_BUFFER_LENGTH, IO_BUFFER_ALIGNMENT);
    if (NULL == identifyResp) return;
    memset(identifyResp, 0, MIN_BUFFER_LENGTH);
    uint8_t iorc = sendCmd(IDENTIFY, 0x00, 0x0000, identifyResp, MIN_BUFFER_LENGTH);
    // TODO: figure out why iorc = 4
    if ((0x00 != iorc) && (0x04 != iorc)) {
        LOG(D) << "IDENTIFY Failed " << (uint16_t) iorc;
        //ALIGNED_FREE(identifyResp);
        //return;
    }
	if (!(memcmp(identifyResp, nullz.data(), 512))) {
		disk_info.devType = DEVICE_TYPE_OTHER;
		return;
	}
    IDENTIFY_RESPONSE * id = (IDENTIFY_RESPONSE *) identifyResp;
    disk_info.devType = DEVICE_TYPE_ATA;
    for (int i = 0; i < sizeof (disk_info.serialNum); i += 2) {
        disk_info.serialNum[i] = id->serialNum[i + 1];
        disk_info.serialNum[i + 1] = id->serialNum[i];
    }
    for (int i = 0; i < sizeof (disk_info.firmwareRev); i += 2) {
        disk_info.firmwareRev[i] = id->firmwareRev[i + 1];
        disk_info.firmwareRev[i + 1] = id->firmwareRev[i];
    }
    for (int i = 0; i < sizeof (disk_info.modelNum); i += 2) {
        disk_info.modelNum[i] = id->modelNum[i + 1];
        disk_info.modelNum[i + 1] = id->modelNum[i];
    }
	_aligned_free(identifyResp);
    return;
}

/** Close the filehandle so this object can be delete. */
DtaDiskATA::~DtaDiskATA()
{
    LOG(D1) << "Destroying DtaDiskATA";
    CloseHandle(hDev);
    _aligned_free(ataPointer);
}
