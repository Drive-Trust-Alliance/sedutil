//
//  InterfaceDeviceID.h
//  BrightPlazaTPer
//
//  Created by Scott Marks on 9/7/22
//  Copyright © 2022 Bright Plaza Inc. All rights reserved.
//


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wdocumentation-html"
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#include <IOKit/scsi/IOSCSIPeripheralDeviceNub.h>  // TODO: tell Apple about the documentation bugs in these files
#include <IOKit/scsi/IOSCSIPeripheralDeviceType00.h>
#include <IOKit/scsi/IOSCSIBlockCommandsDevice.h>
#include <IOKit/scsi/IOBlockStorageServices.h>
#pragma clang diagnostic pop
#include "UserKernelShared.h"
#include "DtaStructures.h"

#pragma once
typedef unsigned char InterfaceDeviceID[kINQUIRY_VENDOR_IDENTIFICATION_Length +
                                        kINQUIRY_PRODUCT_IDENTIFICATION_Length +
                                        kINQUIRY_PRODUCT_REVISION_LEVEL_Length];

typedef enum TPerOverrideAction {
    // SCSI hacks
    reverseInquiryPage80SerialNumber    =  1,

    
    // SAT (SCSI-passthrough) hacks
    tryUnjustifiedLevel0Discovery       =  6,
    splitVendorNameFromModelNumber      =  7,

    
    noSpecialAction                     =  0,
} TPerOverrideAction;

typedef uint16_t TPerOverrideActions;  // set of TPerOverrideAction bits

typedef struct tperOverrideEntry {
    InterfaceDeviceID value;
    InterfaceDeviceID mask;
    TPerOverrideActions actions;
} tperOverrideEntry;

extern tperOverrideEntry tperOverrides[];
extern const size_t nTperOverrides;
//bool idMatches(const InterfaceDeviceID id,
//               const InterfaceDeviceID value,
//               const InterfaceDeviceID mask);
// TPerOverrideActions actionsForID(const InterfaceDeviceID id);
bool deviceNeedsSpecialAction(const InterfaceDeviceID interfaceDeviceIdentification,
                              TPerOverrideAction action);
