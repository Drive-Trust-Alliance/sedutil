//
//  ParseATIdentify.h
//
//  Created by Scott Marks on 8 Sept 2022.
//  Copyright © 2022 Bright Plaza Inc. All rights reserved.
//

#pragma once

#include "stdint.h"
#include "DtaStructures.h"

void parseATIdentifyResponse( const IDENTIFY_RESPONSE & resp, DTA_DEVICE_INFO & di);
