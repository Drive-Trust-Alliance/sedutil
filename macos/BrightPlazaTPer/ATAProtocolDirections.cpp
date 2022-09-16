//
//  ATAProtocolDirections.cpp
//  SedUserClient
//
//  Created by Jackie Marks on 10/16/15.
//
//
#include <IOKit/scsi/SCSITask.h>
#include <IOKit/IOUserClient.h>

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
