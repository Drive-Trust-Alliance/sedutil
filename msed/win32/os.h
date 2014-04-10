/* C:B**************************************************************************
This software is Copyright 2014 Michael Romeo <r0m30@r0m30.com>

This file is part of msed.

msed is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

msed is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with msed.  If not, see <http://www.gnu.org/licenses/>.

* C:E********************************************************************** */
#pragma once

#include <WinSDKVer.h>
#define _WIN32_WINNT _WIN32_WINNT_WINXP
#include <SDKDDKVer.h>
#include <Windows.h>

#include "msedConstants.h"
#pragma warning(disable: 4127) //C4127: conditional expression is constant
#include "log.h"
#define ALIGNED_ALLOC(alignment, length) _aligned_malloc(length,alignment)
#define SNPRINTF sprintf_s
#define ALIGNED_FREE _aligned_free
typedef UINT8 uint8_t;
typedef UINT16 uint16_t;
typedef UINT32 uint32_t;
typedef UINT64 uint64_t;
typedef INT8 int8_t;
typedef INT16 int16_t;
typedef INT32 int32_t;