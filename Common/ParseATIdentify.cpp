//
//  ParseATIdentify.cpp
//
//  Created by Scott Marks on 8 Sept 2022.
//  Copyright © 2022 Bright Plaza Inc. All rights reserved.
//


#include "log.h"

#include "ParseATIdentify.h"

// Steal P_16_COPY, a very useful internal macro from db.h
#define __DBINTERFACE_PRIVATE
#include <db.h>
#undef __DBINTERFACE_PRIVATE
#include <string.h>
#include <algorithm>

void parseATIdentifyResponse( const ATA_IDENTIFY_DEVICE_RESPONSE * presp, DTA_DEVICE_INFO * pdi)
{
  const ATA_IDENTIFY_DEVICE_RESPONSE & resp = *presp;
  DTA_DEVICE_INFO & di = *pdi;
  LOG(D4) << "Entering parseATIdentifyResponse";

#define P_16_COPY_RESP_TO_DI(respFieldName,diFieldName)                 \
  LOG(D4) << "ParseATIdentifyResponse:copying Response field " << #respFieldName << " to Device Info field " << #diFieldName ; \
  for (size_t i = 0; i < sizeof(resp.respFieldName); i+=2*sizeof(resp.respFieldName[0])) { \
    P_16_COPY(resp.respFieldName[i], di.diFieldName[i]);                \
  }
#define respFieldnotAllZeros(respFieldName)                             \
  (!std::all_of(&resp.respFieldName, &resp.respFieldName+sizeof(resp.respFieldName), [](const unsigned char *b) { return *b==0; }))

#define P_16_COPY_NONZERO_RESP_TO_DI(respFieldName,diFieldName) \
  if (respFieldnotAllZeros(respFieldName)) {                    \
    P_16_COPY_RESP_TO_DI(respFieldName,diFieldName)             \
      }
#define P_16_COPY_NONZERO_RESP_STRING_TO_DI(respFieldName,diFieldName)  \
  if (respFieldnotAllZeros(respFieldName)) {                            \
    P_16_COPY_RESP_TO_DI(respFieldName,diFieldName)                     \
      di.diFieldName##Null = 0;                                         \
  }

  memset(&di.vendorID, 0, sizeof(di.vendorID));

  P_16_COPY_RESP_TO_DI(serialNumber    , passwordSalt )  // save a copy before polishing

  P_16_COPY_NONZERO_RESP_STRING_TO_DI(serialNumber    , serialNum    )
  P_16_COPY_NONZERO_RESP_STRING_TO_DI(firmwareRevision, firmwareRev  )
  P_16_COPY_NONZERO_RESP_STRING_TO_DI(modelNum        , modelNum     )
  // IFLOG(D4) {
  //   LOG(D4) << "WorldWideName:";
  //   for (size_t i=0;i<sizeof(resp.worldWideName)/sizeof(resp.worldWideName[0]);i++)
  //     LOG(D4) << "  " << i << ":" << HEXON(2) << (int)resp.worldWideName[i];
  // }
  P_16_COPY_NONZERO_RESP_TO_DI(worldWideName   , worldWideName);

#undef P_16_COPY_NONZERO_RESP_STRING_TO_DI
#undef P_16_COPY_NONZERO_RESP_TO_DI
#undef respFieldnotAllZeros
#undef P_16_COPY_RESP_TO_DI
    }
