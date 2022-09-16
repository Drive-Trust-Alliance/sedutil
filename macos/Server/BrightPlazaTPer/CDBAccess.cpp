//
//  CDBAccess.cpp
//  SedUserClient
//
//  Created by Jackie Marks on 10/16/15.
//
//

//#pragma clang diagnostic push
//#pragma clang diagnostic ignored "-Wsign-conversion"
//#include <libkern/libkern.h>
//#pragma clang diagnostic pop


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wdocumentation-html"
#include <IOKit/scsi/SCSITask.h>
#include <IOKit/IOUserClient.h>
#pragma clang diagnostic pop

#include "CDBAccess.hpp"
#include "ATAProtocolDirections.h"

#define CDB_BLOCK_SIZE 512


IODirection getATAMemoryDescriptorDir(SCSICommandDescriptorBlock cdb) {
    ATA_PROTOCOL_TYPE transferProtocol = (ATA_PROTOCOL_TYPE) ((cdb[1] >> 1) & 0x0f);
    return  ATADirectionConstants[transferProtocol].memoryDescriptorDir;
}

UInt8 getATATaskCommandDir(SCSICommandDescriptorBlock cdb) {
    ATA_PROTOCOL_TYPE transferProtocol = (ATA_PROTOCOL_TYPE) ((cdb[1] >> 1) & 0x0f);
    return ATADirectionConstants[transferProtocol].taskCommandDir ;
}

uint64_t getATATransferSize(SCSICommandDescriptorBlock cdb)
{
    // TODO check whether always true
    return cdb[4] * CDB_BLOCK_SIZE;
}
