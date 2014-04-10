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
#pragma pack(push)
#pragma pack(1)
/*
 * These structures are here While I think about how to 
 * code a parser that isn't going to be a nightmare
 */
typedef struct _SSResponse {
	TCGHeader h;
	uint8_t call;
	uint8_t invoker[9];
	uint8_t method[9];
	uint8_t sl0;
	uint8_t smallatom01;
	uint32_t HostSessionNumber;
	uint8_t smallatom02;
	uint32_t TPerSessionNumber;
} SSResponse;
typedef struct _GenericResponse {
	TCGHeader h;
	uint8_t payload[500];
} GenericResponse;
#pragma pack(pop)