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
#include <winioctl.h>
#include <vector>
#include "DtaDiskNVMe.h"
#include "DtaEndianFixup.h"
#include "DtaStructures.h"
#include "DtaHexDump.h"

using namespace std;

// Missing stuff pulled from MSDN
//

// STORAGE PROPERTY ID is defined but these #define values are missing
#define StorageAdapterProtocolSpecificProperty (STORAGE_PROPERTY_ID) 49

#define NVME_MAX_LOG_SIZE 4096  // value from random internet search
#if 0
typedef enum _STORAGE_PROTOCOL_TYPE {
    ProtocolTypeUnknown = 0x00,
    ProtocolTypeScsi,
    ProtocolTypeAta,
    ProtocolTypeNvme,
    ProtocolTypeSd,
    ProtocolTypeProprietary = 0x7E,
    ProtocolTypeMaxReserved = 0x7F
} STORAGE_PROTOCOL_TYPE, *PSTORAGE_PROTOCOL_TYPE;
typedef struct _STORAGE_PROTOCOL_SPECIFIC_DATA {
    STORAGE_PROTOCOL_TYPE ProtocolType;
    DWORD                 DataType;
    DWORD                 ProtocolDataRequestValue;
    DWORD                 ProtocolDataRequestSubValue;
    DWORD                 ProtocolDataOffset;
    DWORD                 ProtocolDataLength;
    DWORD                 FixedProtocolReturnData;
    DWORD                 Reserved[3];
} STORAGE_PROTOCOL_SPECIFIC_DATA, *PSTORAGE_PROTOCOL_SPECIFIC_DATA;

typedef enum _STORAGE_PROTOCOL_NVME_DATA_TYPE {
    NVMeDataTypeUnknown = 0,
    NVMeDataTypeIdentify,
    NVMeDataTypeLogPage,
    NVMeDataTypeFeature
} STORAGE_PROTOCOL_NVME_DATA_TYPE, *PSTORAGE_PROTOCOL_NVME_DATA_TYPE;
typedef struct _STORAGE_PROTOCOL_DATA_DESCRIPTOR {
    DWORD                          Version;
    DWORD                          Size;
    STORAGE_PROTOCOL_SPECIFIC_DATA ProtocolSpecificData;
} STORAGE_PROTOCOL_DATA_DESCRIPTOR, *PSTORAGE_PROTOCOL_DATA_DESCRIPTOR;
#endif
// End of missing stuff



DtaDiskNVMe::DtaDiskNVMe() {};
void DtaDiskNVMe::init(const char * devref)
{
    LOG(D1) << "Creating DtaDiskNVMe::DtaDiskNVMe() " << devref;

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

uint8_t DtaDiskNVMe::sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
                        void * buffer, uint16_t bufferlen)
{
    UNREFERENCED_PARAMETER(cmd);
    UNREFERENCED_PARAMETER(protocol);
    UNREFERENCED_PARAMETER(comID);
    UNREFERENCED_PARAMETER(buffer);
    UNREFERENCED_PARAMETER(bufferlen);
    LOG(D1) << "Entering DtaDiskNVMe::sendCmd";
    LOG(D1) << "DtaDiskNVMe::sendCmd Not yet implemented ";
    return DTAERROR_COMMAND_ERROR;
}

/** adds the IDENTIFY information to the disk_info structure */

void DtaDiskNVMe::identify(OPAL_DiskInfo& disk_info)
{
    LOG(D1) << "Entering DtaDiskNVMe::identify()";
    PVOID   buffer = NULL;
    UINT8   *results = NULL;
    ULONG   bufferLength = 0;
    DWORD dwReturned = 0;
    BOOL iorc = 0;

    PSTORAGE_PROPERTY_QUERY query = NULL;
    PSTORAGE_PROTOCOL_SPECIFIC_DATA protocolData = NULL;
    PSTORAGE_PROTOCOL_DATA_DESCRIPTOR protocolDataDescr = NULL;
    //  This buffer allocation is needed because the STORAGE_PROPERTY_QUERY has additional data
    // that the nvme driver doesn't use ???????????????????
    /* ****************************************************************************************
    !!DANGER WILL ROBINSON!! !!DANGER WILL ROBINSON!! !!DANGER WILL ROBINSON!!
    This buffer definition causes the STORAGE_PROTOCOL_SPECIFIC_DATA to OVERLAY the
    STORAGE_PROPERTY_QUERY.AdditionalParameters field
    * **************************************************************************************** */
    bufferLength = FIELD_OFFSET(STORAGE_PROPERTY_QUERY, AdditionalParameters)
        + sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA) + NVME_MAX_LOG_SIZE;
    buffer = malloc(bufferLength);
    /* */
    if (buffer == NULL) {
        LOG(E) << "DeviceNVMeQueryProtocolDataTest: allocate buffer failed, exit.\n";
        return;
    }

    //
    // Initialize query data structure to get Identify Data.
    //
    ZeroMemory(buffer, bufferLength);

    query = (PSTORAGE_PROPERTY_QUERY)buffer;
    /* ****************************************************************************************
    !!DANGER WILL ROBINSON!! !!DANGER WILL ROBINSON!! !!DANGER WILL ROBINSON!!
    This buffer definition causes the STORAGE_PROTOCOL_SPECIFIC_DATA to OVERLAY the
    STORAGE_PROPERTY_QUERY.AdditionalParameters field
    * **************************************************************************************** */
    protocolDataDescr = (PSTORAGE_PROTOCOL_DATA_DESCRIPTOR)buffer;
    protocolData = (PSTORAGE_PROTOCOL_SPECIFIC_DATA)query->AdditionalParameters;
    /* */
    query->PropertyId = StorageAdapterProtocolSpecificProperty;
    query->QueryType = PropertyStandardQuery;

    protocolData->ProtocolType = ProtocolTypeNvme;
    protocolData->DataType = NVMeDataTypeIdentify;
    //  protocolData->ProtocolDataRequestValue = NVME_IDENTIFY_CNS_CONTROLLER;
    protocolData->ProtocolDataRequestSubValue = 0;
    protocolData->ProtocolDataOffset = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
    protocolData->ProtocolDataLength = NVME_MAX_LOG_SIZE;

    iorc = DeviceIoControl(hDev, IOCTL_STORAGE_QUERY_PROPERTY,
        buffer, bufferLength, buffer, bufferLength, &dwReturned, NULL);

//
//
//
    disk_info.devType = DEVICE_TYPE_NVME;
    results = (UINT8 *)buffer + FIELD_OFFSET(STORAGE_PROPERTY_QUERY, AdditionalParameters)
        + sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
    results += 4;
    memcpy(disk_info.serialNum, results, sizeof(disk_info.serialNum));
    results += sizeof(disk_info.serialNum);
    memcpy(disk_info.modelNum, results, sizeof(disk_info.modelNum));
    results += sizeof(disk_info.modelNum);
    memcpy(disk_info.firmwareRev, results, sizeof(disk_info.firmwareRev));
    

    return;
}

/** Close the filehandle so this object can be delete. */
DtaDiskNVMe::~DtaDiskNVMe()
{
    LOG(D1) << "Destroying DtaDiskNVMe";
    CloseHandle(hDev);

}
