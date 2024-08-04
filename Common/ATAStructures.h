/* C:B**************************************************************************
 This software is Copyright (c) 2014-2024 Bright Plaza Inc. <drivetrust@drivetrust.com>
 
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
#if !defined(__ATASTRUCTURES_H_INCLUDED__)
#define __ATASTRUCTURES_H_INCLUDED__
#include "stdint.h"

/** ATA commands needed for TCG storage communication */
typedef enum _ATACOMMAND {
    TRUSTED_RECEIVE  = 0x5c,
    TRUSTED_SEND  = 0x5e,
    IDENTIFY_DEVICE = 0xec,
} ATACOMMAND;

typedef enum _ATAPROTOCOL {
    // Per e.g. https://www.t10.org/ftp/t10/document.04/04-262r8.pdf
    HARD_RESET                  =  0,
    SRST                        =  1,
    // 2 Reserved
    NON_DATA                    =  3,
    PIO_DATA_IN                 =  4,
    PIO_DATA_OUT                =  5,
    DMA                         =  6,
    DMA_QUEUED                  =  7,
    DEVICE_DIAGNOSTIC           =  8,
    DEVICE_RESET                =  9,
    UDMA_DATA_IN                = 10,
    UDMA_DATA_OUT               = 11,
    FPDMA                       = 12,
    // 13, 14 Reserved
    RETURN_RESPONSE_INFORMATION = 15
} ATAPROTOCOL;

#pragma pack(push)
#pragma pack(1)
/** Response returned by ATA Identify Device */
typedef struct _ATA_IDENTIFY_DEVICE_RESPONSE {
    uint8_t ignore1;             //word 0
    uint8_t ignore2 : 7;         //word 0
    uint8_t devType : 1;         //word 0
    
    uint8_t ignore3[18];         //words 1-9
    uint8_t serialNumber[20];    //words 10-19
    uint8_t ignore4[6];          //words 20-22
    uint8_t firmwareRevision[8]; //words 23-26
    uint8_t modelNum[40];        //words 27-46
    uint8_t readMultFlags[2];    //word 47
    uint8_t TCGOptions[2];       //word 48
    uint8_t ignore5[102];        //words 49-99
    uint8_t maxLBA[8];           //words 100-103
    uint8_t ignore6[8];          //words 104-107
    uint8_t worldWideName[8];    //words 108-111
    uint8_t ignore7[32];         //words 112-127
    uint8_t securityStatus[2];   //word 128
    uint8_t vendorSpecific[62];  //words 129-159
    uint8_t ignored8[32];        //words 160-175
    uint8_t mediaSerialNum[60];  //words 176-205
    uint8_t ignored9[98];        //words 206-254
    uint8_t integrityWord[2];    //word 255
} ATA_IDENTIFY_DEVICE_RESPONSE;
#pragma pack(pop)
#define ATA_IDENTIFY_DEVICE_RESPONSE_SIZE sizeof(ATA_IDENTIFY_DEVICE_RESPONSE)
#endif //!defined(__ATASTRUCTURES_H_INCLUDED__)
