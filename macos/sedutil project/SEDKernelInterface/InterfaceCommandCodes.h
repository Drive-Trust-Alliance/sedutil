//
//  InterfaceCommandCodes.h
//  SEDKernelInterface
//
//  Created by Scott Marks on 2022 Sep 05.
//  Copyright © 2022 Bright Plaza, Inc. All rights reserved.
//
#pragma once

typedef enum ATA_COMMAND {
    kATACmd_TRUSTED_RECEIVE_PIO = 0x5C,
    kATACmd_TRUSTED_RECEIVE_DMA = 0x5D,
    kATACmd_TRUSTED_SEND_PIO    = 0x5E,
    kATACmd_TRUSTED_SEND_DMA    = 0x5F,
    kATACmd_IDENTIFY_DEVICE     = 0xEC,
} ATA_COMMAND;

typedef enum ATA_PIO_DIRECTIONS {
    PIO_DataIn  = 4,
    PIO_DataOut = 5,
} ATA_PIO_DIRECTIONS;


#include <IOKit/scsi/SCSICommandOperationCodes.h>
typedef enum SCSI_COMMAND {
//    kSCSICmd_INQUIRY               = 0x12,
    kSCSICmd_ATA_PASS_THROUGH      = 0xA1,
    kSCSICmd_SECURITY_PROTOCOL_IN  = 0xA2,
    kSCSICmd_SECURITY_PROTOCOL_OUT = 0xB5,
} SCSI_COMMAND;

