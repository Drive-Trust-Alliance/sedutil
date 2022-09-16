/* C:B**************************************************************************
This software is Copyright 2014-2017 Bright Plaza Inc. <drivetrust@drivetrust.com>

This file is part of sedutil.

sedutil is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

sedutil is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sedutil.  If not, see <http://www.gnu.org/licenses/>.

* C:E********************************************************************** */
/* These are a few macros to fixup the endianess of the data
 * returned by the drive as specified in the TCG OPAL SSC standards
 * Since this is a low use utility program it shouldn't be to
 * ugly that these are macros
 */

/*
 * Windows provides system call in the network stack to do this but
 * I've never had much luck when the winsock headers get in the mix
 */
//
//TODO: add a test on the endianess of the system and define
//  empty macros if the system is big endian
#pragma once
#ifdef __gnu_linux__
#include <endian.h>
#if __BYTE_ORDER != __LITTLE_ENDIAN
#error This code does not support big endian architectures
#endif
#endif
/** change the "endianess" of a 16bit field */
#define SWAP16(x) ((uint16_t) ((x & 0x00ff) << 8) | ((x & 0xff00) >> 8))
/** change the "endianess" of a 32bit field */
#define SWAP32(x) ((uint32_t) (((x) & 0x000000ff) << 24) | (((x) & 0x0000ff00) << 8) \
	                        | (((x) & 0x00ff0000) >> 8) | (((x) & 0xff000000) >> 24))
/** change the "endianess" of a 64bit field */
#define SWAP64(x) (uint64_t) \
	((uint64_t) (SWAP32((x & 0x00000000ffffffff)) << 32) | \
	((uint64_t) (SWAP32((x >> 32))) )    \
	)
