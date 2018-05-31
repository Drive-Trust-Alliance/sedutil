/* C:B**************************************************************************
This software is Copyright 2014-2016 Bright Plaza Inc. <drivetrust@drivetrust.com>

This file is part of sedutil and not free, authorisation must be granded.

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

#include <sys/ioctl.h>
#include <sys/types.h>

#include <net/if.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/if_link.h>


#include <ncurses.h>
#include <stdio.h>

#include "os.h"
#include "Network.h"
#include <string>
using namespace std;

string GetMACAddress(void){
	int s;
	struct ifreq buffer;
	char MAC[14];

	GetFirstInterface();

	s = socket(PF_INET, SOCK_DGRAM, 0);

	memset(&buffer, 0x00, sizeof(buffer));
	
	string PHY = GetFirstInterface();
	strcpy(buffer.ifr_name, PHY.c_str());

	ioctl(s, SIOCGIFHWADDR, &buffer);

	close(s);
	
	for( s = 0; s < 6; s++ )
	{
		mvprintw(8,s*2, "%.2X ", (unsigned char)buffer.ifr_hwaddr.sa_data[s]);
	}

	sprintf(MAC, "%02x%02x%02x%02x%02x%02x\n",
		 (unsigned char)buffer.ifr_hwaddr.sa_data[0], 
		 (unsigned char)buffer.ifr_hwaddr.sa_data[1], 
		 (unsigned char)buffer.ifr_hwaddr.sa_data[2], 
		 (unsigned char)buffer.ifr_hwaddr.sa_data[3], 
		 (unsigned char)buffer.ifr_hwaddr.sa_data[4], 
		 (unsigned char)buffer.ifr_hwaddr.sa_data[5]);


	mvprintw(9,2, "%s ", MAC);

	return( string(MAC) );

}

string GetFirstInterface(void){
	struct ifaddrs *ifaddr, *ifa;
	int family, s, n;
	char host[NI_MAXHOST];
	char PHY[14];

	if (getifaddrs(&ifaddr) == -1) {
		mvprintw(15,2, "getifaddrs");
		exit(EXIT_FAILURE);
	}

	/* Walk through linked list, maintaining head pointer so we
	can free list later */

	for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++) {
		if (ifa->ifa_addr == NULL)
			continue;

		family = ifa->ifa_addr->sa_family;

		/* Display interface name and family (including symbolic
		form of the latter for the common families) */

		mvprintw(15,2,"GetInterface Output: %-8s %s (%d)\n",
		ifa->ifa_name,
		(family == AF_PACKET) ? "AF_PACKET" :
		(family == AF_INET) ? "AF_INET" :
		(family == AF_INET6) ? "AF_INET6" : "???",
		family);

		sprintf(PHY, "%s", ifa->ifa_name);

		mvprintw(20,20,"SPrint Conv: %s HI",PHY);


	}

	freeifaddrs(ifaddr);
	// exit(EXIT_SUCCESS);
	return( string(PHY) );

}