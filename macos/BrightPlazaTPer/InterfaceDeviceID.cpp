//
//  TPerDriver.cpp
//  SedUserClient
//
//  Created by Jackie Marks on 9/21/15.
//  Copyright © 2015 Bright Plaza Inc. All rights reserved.
//


//#pragma clang diagnostic push
//#pragma clang diagnostic ignored "-Wsign-conversion"
//#include <libkern/libkern.h>
//#pragma clang diagnostic pop


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wdocumentation-html"
#include <IOKit/scsi/SCSITask.h>
#include <IOKit/scsi/SCSICmds_INQUIRY_Definitions.h>
#include <IOKit/scsi/SCSICommandOperationCodes.h>
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#include <IOKit/IOUserClient.h>
#include <IOKit/IOBufferMemoryDescriptor.h>
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#include <IOKit/storage/IOBlockStorageDriver.h>
#pragma clang diagnostic pop

#include <SEDKernelInterface/SEDKernelInterface.h>
#include "TPerDriver.h"
#include "CDBAccess.hpp"

#include "kernel_debug.h"
#include "kernel_PrintBuffer.h"
#include "DtaStructures.h"
#include "DtaEndianFixup.h"

#include "InterfaceDeviceID.h"

tperOverrideEntry tperOverrides[] =
{
    {
        { 0x52, 0x65, 0x61, 0x6c, 0x74, 0x65, 0x6b, 0x20, 0x52, 0x54, 0x4c, 0x39, 0x32, 0x31, 0x30, 0x20, //  |Realtek RTL9210 |
          0x4e, 0x56, 0x4d, 0x45, 0x20, 0x20, 0x20, 0x20, 0x31, 0x2e, 0x30, 0x30                          //  |NVME    1.00|
        },  // value
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, //  |XXXXXXXXXXXXXXXX|
          0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00                          //  |XXXX________|
        }   // mask
    },
};

size_t nTperOverrides = sizeof(tperOverrides) / sizeof(tperOverrides[0]);

bool idMatches(const InterfaceDeviceID id, const InterfaceDeviceID value, const InterfaceDeviceID mask) {
    for (const unsigned char * pid = id, * pvalue = value, * pmask = mask, * pend = pid + sizeof(InterfaceDeviceID);
         pid < pend ;
         pid ++ , pvalue ++ , pmask ++ )
        if (0 != (((*pid)^(*pvalue)) & (*pmask)))
            return false;
    return true;
}
