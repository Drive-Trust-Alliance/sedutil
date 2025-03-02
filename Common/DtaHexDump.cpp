/* C:B**************************************************************************
This software is © 2014 Bright Plaza Inc. <drivetrust@drivetrust.com>

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
#include "DtaHexDump.h"

#if !NOHEXDUMP
#include <cstring>
#include <stdint.h>
#include <stdio.h>
#include "log.h"


void __DtaHexDump(const char * tag, void * address, unsigned int length) {
	uint8_t display[17];
	uint8_t * cpos = (uint8_t *)address;
	uint8_t * epos = cpos + length;
        LOG(D4) << tag << "=" << std::hex << address <<" (length=0x" << std::hex << length << "):";
	int rpos = 0;
	int dpos = 0;
	fprintf( stderr,"%04x ",rpos);
	while (cpos < epos){
		fprintf( stderr,"%02x", cpos[0]);
		if (!((++rpos) % 4))fprintf( stderr," ");
		display[dpos++] = (isprint(cpos[0]) ? cpos[0] : 0x2e );
		cpos += 1;
		if (16 == dpos) {
			dpos = 0;
			display[16] = 0x00;
			fprintf( stderr," %s \n", display);
			if(cpos < epos) fprintf( stderr,"%04x ", rpos);
			memset(&display,0,sizeof(display));
		}
	}
	if (dpos != 0) {
		if (dpos % 4) fprintf( stderr," ");
			fprintf( stderr,"  ");
		for (int i = dpos ; i < 15; i++) {
			if (!(i % 4)) fprintf( stderr," ");
			fprintf( stderr,"  ");
		}
		display[dpos] = 0x00;
		fprintf( stderr," %s\n", display);
	}
}
#endif
