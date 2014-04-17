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
typedef struct _MSED_OPTIONS {
    uint8_t password;
	uint8_t userid;
	uint8_t newpassword;
    uint8_t deviceref;
    uint8_t action;
} MSED_OPTIONS;
void usage();
uint8_t MsedOptions(int argc, char * argv[], MSED_OPTIONS * opts);

