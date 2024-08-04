//
//  InterfaceDeviceID.h
//  BrightPlazaTPer
//
//  Created by Scott Marks on 9/7/22
//  Copyright © 2022-2024 Bright Plaza Inc. All rights reserved.
//

#include <stdint.h>
#include <stddef.h>

#pragma once
// Copied from SCSICmds_INQUIRY_Definitions.h:71
/*!
  @enum INQUIRY field sizes
  @discussion
  Sizes for some of the inquiry data fields.
  @constant INQUIRY_VENDOR_IDENTIFICATION_Length
  Size of VENDOR_IDENTIFICATION field.
  @constant INQUIRY_PRODUCT_IDENTIFICATION_Length
  Size of PRODUCT_IDENTIFICATION field.
  @constant INQUIRY_PRODUCT_REVISION_LEVEL_Length
  Size of PRODUCT_REVISION_LEVEL field.
*/
enum
{
    INQUIRY_VENDOR_IDENTIFICATION_Length = 8,
    INQUIRY_PRODUCT_IDENTIFICATION_Length = 16,
    INQUIRY_PRODUCT_REVISION_LEVEL_Length = 4
};
/*!
  @enum InterfaceDeviceID size
  @discussion
  Sized large enough to hold all the INQUIRY fields aboc
  @constant InterfaceDeviceID_Length
  Size of InterfaceDeviceID
*/
enum
{
    InterfaceDeviceID_Length=INQUIRY_VENDOR_IDENTIFICATION_Length +
                             INQUIRY_PRODUCT_IDENTIFICATION_Length +
                             INQUIRY_PRODUCT_REVISION_LEVEL_Length
};
typedef unsigned char InterfaceDeviceID[InterfaceDeviceID_Length];

typedef enum TPerOverrideAction {
    // SCSI Pseudodevice hacks
    acceptPseudoDeviceImmediately     = 1,

    // SCSI (SAS) hacks
    avoidSlowSATATimeout             = 11,
    reverseInquiryPage80SerialNumber = 12,

    // SAT (SCSI ATA passthrough) hacks
    avoidSlowSASTimeout              = 21,
    tryUnjustifiedLevel0Discovery    = 22,
    splitVendorNameFromModelNumber   = 23,

    noSpecialAction                   = 0,
} TPerOverrideAction;

static __inline
unsigned int single_action(const unsigned int action) {
    return action == noSpecialAction ? 0 : (unsigned int)(1 << (action - 1));
}

typedef uint32_t TPerOverrideActions;  // set of TPerOverrideAction bits

typedef struct tperOverrideEntry {
    InterfaceDeviceID value;
    InterfaceDeviceID mask;
    TPerOverrideActions actions;
} tperOverrideEntry;

extern tperOverrideEntry tperOverrides[];
extern const size_t nTperOverrides;

#if defined(__cplusplus)
bool idMatches(const InterfaceDeviceID& id,
    const InterfaceDeviceID& value,
    const InterfaceDeviceID& mask);

TPerOverrideActions actionsForID(const InterfaceDeviceID& interfaceDeviceIdentification);
bool deviceNeedsSpecialAction(const InterfaceDeviceID& interfaceDeviceIdentification,
    TPerOverrideAction action);
#endif // defined(__cplusplus)
