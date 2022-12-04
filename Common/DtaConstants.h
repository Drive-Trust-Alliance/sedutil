/* C:B**************************************************************************
This software is Copyright 2014, 2022 Bright Plaza Inc. <drivetrust@drivetrust.com>

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
 /** MAX Length of input the IO buffers used */
#define MAX_BUFFER_LENGTH 61440
/** Length of input the IO buffers used */
#define MIN_BUFFER_LENGTH 2048
#define IO_BUFFER_LENGTH 12288 // 15360 // 17408
#define IO_BUFFER_LENGTH_HI 61440
#define IO_BUFFER_LENGTH_MI 28672 // 28K 32768 // 33280 // T7 is 33280
#define IO_BUFFER_LENGTH_LO 12288 // 15360 // 17408
#define BLOCKSIZE_HI 57344  // 56K
#define BLOCKSIZE_MI 28672  // 30K->28K(28672 7000h) -> 24K(24576 6000h) NG
#define BLOCKSIZE_LO 10240 //  10K
/** Alignment of the IO buffers.
* generic align on 1k boundary probably not needed
* but when things weren't working this was one of the
* things I tried to make it work.
*/
// #define IO_BUFFER_ALIGNMENT 1024
#define IO_BUFFER_ALIGNMENT 16384  // ARM systems use 16K memory page size
/** maximum number of disks to be scanned */
#define MAX_DISKS 99
/** iomanip commands to hexdump a field */
#define HEXON(x) "0x" << std::hex << std::setw(x) << std::setfill('0')
/** iomanip command to return to standard ascii output */
#define HEXOFF std::dec << std::setw(0) << std::setfill(' ')
/** Return Codes */
#define DTAERROR_UNSUPORTED_LOCKING_RANGE	0x81
#define DTAERROR_OBJECT_CREATE_FAILED		0x82
#define DTAERROR_INVALID_PARAMETER			0x83
#define DTAERROR_OPEN_ERR					0x84
#define DTAERROR_INVALID_LIFECYCLE			0x85
#define DTAERROR_INVALID_COMMAND			0x86
#define DTAERROR_AUTH_FAILED 				0x87
#define DTAERROR_COMMAND_ERROR				0x88
#define DTAERROR_NO_METHOD_STATUS			0x89
#define DTAERROR_NO_LOCKING_INFO			0x8a
#define DTAERROR_CREATE_USB				0x8b
#define LICENSE_NOT_ACIVATED			0x90
#define NOT_SUPPORT_LARGE_PBA_WRITE_TO_ENCLOSURE_DRIVE 0x91
/** Locking Range Configurations */
#define DTA_DISABLELOCKING		0x00
#define DTA_READLOCKINGENABLED		0x01
#define DTA_WRITELOCKINGENABLED	0x02
