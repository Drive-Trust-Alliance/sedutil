#pragma once

using namespace std;
////////////////////////////////////////////////////////////////////
//Use Compile-Time as seed

#define Seed (((__TIME__[7] - '0') * 1  + (__TIME__[6] - '0') * 10  + \
              (__TIME__[4] - '0') * 60   + (__TIME__[3] - '0') * 600 + \
              (__TIME__[1] - '0') * 3600 + (__TIME__[0] - '0') * 36000) & 0xff)
////////////////////////////////////////////////////////////////////

// window linux all need it 
class obfs {
public:
	obfs();
	~obfs();
	void rstor(char * st1, char * sf);
	void lic(int lev, char * lic_level);
	//uint32_t getseed();
	char sfree[16]  =  { 'F' ^ Seed,'i'^(Seed+1),'d'^(Seed+2),'e'^(Seed+3),'l'^(Seed+4),'i'^(Seed+5),'t'^(Seed+6),'y'^(Seed+7),'F'^(Seed+8),'r'^(Seed+9),'e'^(Seed+10),'e'^(Seed+11),' '^(Seed+12), ' '^(Seed+13), ' '^(Seed+14), ' '^(Seed+15), };
	char sstd[16]   =  { 'F' ^ Seed,'i' ^ (Seed + 1),'d' ^ (Seed + 2),'e' ^ (Seed + 3),'l' ^ (Seed + 4),'i' ^ (Seed + 5),'t' ^ (Seed + 6),'y' ^ (Seed + 7),'S' ^ (Seed + 8),'t' ^ (Seed + 9),'a' ^ (Seed + 10),'n' ^ (Seed + 11),'d' ^ (Seed + 12), 'a' ^ (Seed + 13), 'r' ^ (Seed + 14), 'd' ^ (Seed + 15), };
	char s5[16]     =  { 'F' ^ Seed,'i' ^ (Seed + 1),'d' ^ (Seed + 2),'e' ^ (Seed + 3),'l' ^ (Seed + 4),'i' ^ (Seed + 5),'t' ^ (Seed + 6),'y' ^ (Seed + 7),'P' ^ (Seed + 8),'R' ^ (Seed + 9),'O' ^ (Seed + 10),'5' ^ (Seed + 11),' ' ^ (Seed + 12), ' ' ^ (Seed + 13), ' ' ^ (Seed + 14), ' ' ^ (Seed + 15), };
	char s25[16]    =  { 'F' ^ Seed,'i' ^ (Seed + 1),'d' ^ (Seed + 2),'e' ^ (Seed + 3),'l' ^ (Seed + 4),'i' ^ (Seed + 5),'t' ^ (Seed + 6),'y' ^ (Seed + 7),'P' ^ (Seed + 8),'R' ^ (Seed + 9),'O' ^ (Seed + 10),'2' ^ (Seed + 11),'5' ^ (Seed + 12), ' ' ^ (Seed + 13), ' ' ^ (Seed + 14), ' ' ^ (Seed + 15), };
	char s100[16]   =  { 'F' ^ Seed,'i' ^ (Seed + 1),'d' ^ (Seed + 2),'e' ^ (Seed + 3),'l' ^ (Seed + 4),'i' ^ (Seed + 5),'t' ^ (Seed + 6),'y' ^ (Seed + 7),'P' ^ (Seed + 8),'R' ^ (Seed + 9),'O' ^ (Seed + 10),'1' ^ (Seed + 11),'0' ^ (Seed + 12), '0' ^ (Seed + 13), ' ' ^ (Seed + 14), ' ' ^ (Seed + 15), };
	char sunlmt[17] =  { 'F' ^ Seed,'i' ^ (Seed + 1),'d' ^ (Seed + 2),'e' ^ (Seed + 3),'l' ^ (Seed + 4),'i' ^ (Seed + 5),'t' ^ (Seed + 6),'y' ^ (Seed + 7),'P' ^ (Seed + 8),'R' ^ (Seed + 9),'O' ^ (Seed + 10),'U' ^ (Seed + 11),'n' ^ (Seed + 12), 'l' ^ (Seed + 13), 'i' ^ (Seed + 14), 'm' ^ (Seed + 15), 't'};
}; 


