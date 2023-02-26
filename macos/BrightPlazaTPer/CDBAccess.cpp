//
//  CDBAccess.cpp
//  SedUserClient
//
//  Created by Jackie Marks on 10/16/15.
//
//

#include <IOKit/scsi/SCSITask.h>
#include <IOKit/IOUserClient.h>

#include "CDBAccess.hpp"
#include "ATAProtocolDirections.h"
#include "kernel_debug.h"

#define CDB_BLOCK_SIZE 512

//#define DEBUG_CDBACCESS 1
#undef DEBUG_CDBACCESS
#if defined(DEBUG_CDBACCESS)
#define CDBA_IOLOG_DEBUG IOLOG_DEBUG
#define CDBA_IOLOGBUFFER_DEBUG IOLOGBUFFER_DEBUG
#else // !defined(DEBUG_CDBACCESS)
#define CDBA_IOLOG_DEBUG(...) do ; while (0)
#define CDBA_IOLOGBUFFER_DEBUG(...)  do ; while (0)
#endif // defined(DEBUG_CDBACCESS)

IODirection getATAMemoryDescriptorDir(SCSICommandDescriptorBlock cdb) {
    CDBA_IOLOG_DEBUG("getATAMemoryDescriptorDir: cdb[1]=0x%02X", cdb[1]);
    ATA_PROTOCOL_TYPE transferProtocol = (ATA_PROTOCOL_TYPE) ((cdb[1] >> 1) & 0x0f);
    CDBA_IOLOG_DEBUG("getATAMemoryDescriptorDir: transferProtocol=0x%02X", transferProtocol);
    IODirection direction = ATADirectionConstants[transferProtocol].memoryDescriptorDir ;
    CDBA_IOLOG_DEBUG("getATAMemoryDescriptorDir: direction=0x%02X", direction);
    return direction;
}

UInt8 getATATaskCommandDir(SCSICommandDescriptorBlock cdb) {
    CDBA_IOLOG_DEBUG("getATATaskCommandDir: cdb[1]=0x%02X", cdb[1]);
    ATA_PROTOCOL_TYPE transferProtocol = (ATA_PROTOCOL_TYPE) ((cdb[1] >> 1) & 0x0f);
    CDBA_IOLOG_DEBUG("getATATaskCommandDir: transferProtocol=0x%02X", transferProtocol);
    UInt8 direction = ATADirectionConstants[transferProtocol].taskCommandDir ;
    CDBA_IOLOG_DEBUG("getATATaskCommandDir: direction=0x%02X", direction);
    return direction;
}

uint64_t getATATransferSize(SCSICommandDescriptorBlock cdb)
{
    // TODO check whether always true
    return cdb[4] * CDB_BLOCK_SIZE;
}
