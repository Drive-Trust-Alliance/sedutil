//
//  InterfaceDeviceID.h
//  BrightPlazaTPer
//
//  Created by Scott Marks on 9/7/22
//  Copyright © 2022-2024 Bright Plaza Inc. All rights reserved.
//



#include <cstdint>

#pragma once
// Copied from SCSICmds_INQUIRY_Definitions.h:71
/*!
@enum INQUIRY field sizes
@discussion
Sizes for some of the inquiry data fields.
@constant kINQUIRY_VENDOR_IDENTIFICATION_Length
Size of VENDOR_IDENTIFICATION field.
@constant kINQUIRY_PRODUCT_IDENTIFICATION_Length
Size of PRODUCT_IDENTIFICATION field.
@constant kINQUIRY_PRODUCT_REVISION_LEVEL_Length
Size of PRODUCT_REVISION_LEVEL field.
*/
enum
{
	kINQUIRY_VENDOR_IDENTIFICATION_Length	= 8,
	kINQUIRY_PRODUCT_IDENTIFICATION_Length	= 16,
	kINQUIRY_PRODUCT_REVISION_LEVEL_Length	= 4
};
typedef unsigned char InterfaceDeviceID[kINQUIRY_VENDOR_IDENTIFICATION_Length +
                                        kINQUIRY_PRODUCT_IDENTIFICATION_Length +
                                        kINQUIRY_PRODUCT_REVISION_LEVEL_Length];

typedef enum TPerOverrideAction {
    // SCSI hacks
    reverseInquiryPage80SerialNumber    =  1,


    // SAT (SCSI-passthrough) hacks
    tryUnjustifiedLevel0Discovery       =  5,
    splitVendorNameFromModelNumber      =  7,


    noSpecialAction                     =  0,
} TPerOverrideAction;

static __inline
int single_action(const int action) {
    return action==noSpecialAction ? 0 : static_cast<int>(1 << (action - 1)) ;
}

typedef uint16_t TPerOverrideActions;  // set of TPerOverrideAction bits

typedef struct tperOverrideEntry {
    InterfaceDeviceID value;
    InterfaceDeviceID mask;
    TPerOverrideActions actions;
} tperOverrideEntry;

extern tperOverrideEntry tperOverrides[];
extern const size_t nTperOverrides;
bool idMatches(const InterfaceDeviceID & id,
               const InterfaceDeviceID & value,
               const InterfaceDeviceID & mask);
TPerOverrideActions actionsForID(const InterfaceDeviceID & interfaceDeviceIdentification);
bool deviceNeedsSpecialAction(const InterfaceDeviceID & interfaceDeviceIdentification,
                              TPerOverrideAction action);
