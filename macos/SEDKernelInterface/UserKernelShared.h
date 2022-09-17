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
#define IOInquiryDeviceResponseKey      "Inquiry Response"
#define IOInquiryPage00ResponseKey      "Inquiry Page00 Response"
#define IOInquiryPage80ResponseKey      "Inquiry Page80 Response"
#define IOInquiryPage83ResponseKey      "Inquiry Page83 Response"
#define IOInquiryPage86ResponseKey      "Inquiry Page86 Response"
#define IOInquiryPage89ResponseKey      "Inquiry Page89 Response"
#define IOInquiryPageB0ResponseKey      "Inquiry PageB0 Response"
#define IOInquiryPageB1ResponseKey      "Inquiry PageB1 Response"
#define IOInquiryPageB2ResponseKey      "Inquiry PageB2 Response"
#define IOInquiryCharacteristicsKey           "Inquiry Characteristics"
#define IOInquiryPage00CharacteristicsKey     "Inquiry Page 00 Characteristics"
#define IOInquiryPage80CharacteristicsKey     "Inquiry Page 80 Characteristics"
#define IOInquiryPage83CharacteristicsKey     "Inquiry Page 83 Characteristics"
#define IOInquiryPage86CharacteristicsKey     "Inquiry Page 86 Characteristics"
#define IOInquiryPage89CharacteristicsKey     "Inquiry Page 89 Characteristics"
#define IOInquiryPageB0CharacteristicsKey     "Inquiry Page B0 Characteristics"
#define IOInquiryPageB1CharacteristicsKey     "Inquiry Page B1 Characteristics"
#define IOInquiryPageB2CharacteristicsKey     "Inquiry Page B2 Characteristics"
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
#define IOVendorNameKey                 "Vendor Name"
#define IOInterfaceTypeKey              "TPer Interface Type"
#define IOInterfaceTypeSAT              "SAT"
#define IOInterfaceTypeSCSI             "SCSI"
#define IODtaDeviceInfoKey              "DTA Device Info"


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
