//
//  ParseATIdentify.h
//
//  Created by Scott Marks on 8 Sept 2022.
//  © 2022 Bright Plaza Inc. All rights reserved.
//

#pragma once

#include "DtaStructures.h"

void parseATIdentifyResponse( const ATA_IDENTIFY_DEVICE_RESPONSE * presp, DTA_DEVICE_INFO * pdi);
