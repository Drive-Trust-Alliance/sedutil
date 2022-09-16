//
//  UserKernelShared.h
//  SedUserClient
//
//  Created by Jackie Marks on 9/21/15.
//  Copyright © 2015 Bright Plaza Inc. All rights reserved.
//

#ifndef UserKernelShared_h
#define UserKernelShared_h


#define DriverClass		com_brightplaza_BrightPlazaTPer
#define kBrightPlazaDriverClass	"com_brightplaza_BrightPlazaTPer"
#define ProviderClass   IOSCSIPeripheralDeviceNub
#define kProviderClass  "IOSCSIPeripheralDeviceNub"
#define UserClientClass com_brightplaza_BrightPlazaTPerUserClient

#define kIOBlockStorageDriver   "IOBlockStorageDriver"

#define IOIdentifyDeviceResponseKey     "Identify Response"
#define IODiscovery0ResponseKey         "D0 Response"
#define IOIdentifyCharacteristicsKey    "Identify Characteristics"
#define IOTPerLockingFeatureKey         "TPer Locking Feature"
#define IOTCGPropertiesKey              "TCG Properties"
#define IODeviceCharacteristicsKey      "Device Characteristics"
#define IOProtocolCharacteristicsKey    "Protocol Characteristics"
#define IOLockingEnabledKey             "Locking Enabled"
#define IOLockedKey                     "Locked"
#define IOMBREnabledKey                 "MBR Enabled"
#define IOMBRDoneKey                    "MBR Done"
#define IOTCGOptionsKey                 "TCG Options"
#define IODeviceTypeKey                 "Device Type"
#define IOSerialNumberKey               "Serial Number"
#define IOModelNumberKey                "Model Number"
#define IOFirmwareRevisionKey           "Firmware Revision"
#define IOProductNameKey                "Product Name"
#define IOWorldWideNameKey              "World Wide Name"


#include <AssertMacros.h>

#define SED_TIMEOUT 10000
#define IDENTIFY_RESPONSE_SIZE 512
#define DISCOVERY0_RESPONSE_SIZE 2048


#include <IOKit/scsi/SCSITask.h>

typedef struct _tagSCSICommandDescriptorBlockAsTwoQuads
{
    uint64_t quad0;
    uint64_t quad1;
} SCSICommandDescriptorBlockAsTwoQuads;

#define __TPerDriverMethod__(index, method, nScalarInputs, nStructInputs, nScalarOutputs, nStructOutputs) index,
enum {
#include "TPerDriverDispatchTable.inc"
    kSedUserClientNumberOfMethods // Must be last
};
#undef __TPerDriverMethod__


#endif  // #ifndef UserKernelShared_h
