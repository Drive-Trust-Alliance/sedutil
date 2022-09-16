//
//  ATAProtocolDirections.cpp
//  SedUserClient
//
//  Created by Jackie Marks on 10/16/15.
//
//
//#include <IOKit/IOLib.h>

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

#include "ATAProtocolDirections.h"


ATA_DIRECTION_CONSTANTS ATADirectionConstants[16] = {
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { kIODirectionIn,  kSCSIDataTransfer_FromTargetToInitiator },           //  4   userland "read", security protocol in
    { kIODirectionOut, kSCSIDataTransfer_FromInitiatorToTarget },           //  5   userland "write", security protocol out
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 }
};
