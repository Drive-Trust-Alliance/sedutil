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
#if !defined(__ATASTRUCTURES_H_INCLUDED__)
#define __ATASTRUCTURES_H_INCLUDED__

/** ATA commands needed for TCG storage communication */
typedef enum _ATACOMMAND {
    IF_RECV  = 0x5c,
    IF_SEND  = 0x5e,
    IDENTIFY = 0xec,
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

#endif //!defined(__ATASTRUCTURES_H_INCLUDED__)
