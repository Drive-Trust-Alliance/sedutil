
#include "ob.h"
#include <stdio.h>

////////////////////////////////////////////////////////////////////
//Use Compile-Time as seed
//#define Seed ((__TIME__[7] - '0') * 1  + (__TIME__[6] - '0') * 10  + \
//              (__TIME__[4] - '0') * 60   + (__TIME__[3] - '0') * 600 + \
//              (__TIME__[1] - '0') * 3600 + (__TIME__[0] - '0') * 36000)
////////////////////////////////////////////////////////////////////

obfs::obfs()
{

}
obfs::~obfs()
{
}


void obfs::rstor(char * st1, char * sf)
{
	char st2[17] = "FidelityPRO5    "; // array size is 17-byte but actual content is 16-byte
	if (0) {
	printf("supposed text in hex : ");
	for (int i = 0; i < 16; i++) {
		printf("%02X", st2[i] & 0xff);
	}
	
	printf("\n");
	}
	// restore to original text
	for (int i = 0; i < 16; i++) {
		st1[i] = ((sf[i] & 0xff) ^ (Seed + i));
	}
	if (0) {
		printf("restore text in hex : ");
		for (int i = 0; i < 16; i++) {
			printf("%02X", st1[i] & 0xff);
		}
		printf("\n");
		printf("restore text in char : \"");

		for (int i = 0; i < 16; i++) {
			printf("%C", st1[i] & 0xff);
		}
		printf("\"\n");
	}

}


void obfs::lic(int lev, char * lic_level)
{
	switch (lev)
	{
	case 1:
		//memcpy(lic_level, "FidelityFree    ", 16);
		rstor(lic_level, sfree);
		//memcpy(lic_level, st1, 16);
		break;
	case 2:
		//memcpy(lic_level, "FidelityStandard", 16);
		//memcpy(lic_level, sstd, 16);
		rstor(lic_level, sstd);
		//memcpy(lic_level, st1, 16);
		break;
	case 4:
		//memcpy(lic_level, "FidelityPRO5    ", 16);
		//memcpy(lic_level, s5, 16);
		rstor(lic_level, s5);
		//memcpy(lic_level, st1, 16);
		break;
	case 16:
		//memcpy(lic_level, "FidelityPRO25   ", 16);
		//memcpy(lic_level, s25, 16);
		rstor(lic_level, s25);
		//memcpy(lic_level, st1, 16);
		break;
	case 32:
		//memcpy(lic_level, "FidelityPRO100  ", 16);
		//memcpy(lic_level, s100, 16);
		rstor(lic_level, s100);
		//memcpy(lic_level, st1, 16);
		break;
	case 64:
		//memcpy(lic_level, "FidelityPROUnlimt", 16);
		//memcpy(lic_level, sunlmt, 16);
		rstor(lic_level, sunlmt);
		//memcpy(lic_level, st1, 16);
		break;
	default:
		//memcpy(lic_level, "                ", 16);
		//memcpy(lic_level, sbnk, 16);
		for (int i=0;i<16; i++)
			lic_level[i] = ' ';
		break;
	}
}


//uint32_t obfs::getseed()
//{
//	return (Seed & 0xff);
//}
