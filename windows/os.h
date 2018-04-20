/* C:B**************************************************************************
This software is Copyright 2014 - 2016 Bright Plaza Inc. <drivetrust@drivetrust.com>

This file is part of sedutil.

sedutil is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

sedutil is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sedutil.If not, see <http://www.gnu.org/licenses/>.

*C : E********************************************************************** */

#pragma once
#define WINDOWS7 0 // 0 if for windows 8.1 or above ; 1 only for windows 7

#include <WinSDKVer.h>
#define _WIN32_WINNT _WIN32_WINNT_WINXP
#include <SDKDDKVer.h>
#include <Windows.h>
#include <assert.h>

#include "DtaConstants.h"
#pragma warning(disable: 4127) //C4127: conditional expression is constant
#include "log.h"
/** OS specific implementation of the "safe" snprintf function */
#define SNPRINTF sprintf_s
#define strcasecmp _stricmp 
/** OS specific device iterator */
#define DEVICEMASK sprintf_s(devname, 23, "\\\\.\\PhysicalDrive%i", i)
/** OS specific example device to be used in help output*/
#define DEVICEEXAMPLE "\\\\.\\PhysicalDrive3"
// match types
typedef UINT8 uint8_t;
typedef UINT16 uint16_t;
typedef UINT32 uint32_t;
typedef UINT64 uint64_t;
typedef INT8 int8_t;
typedef INT16 int16_t;
typedef INT32 int32_t;
typedef INT64 int64_t;