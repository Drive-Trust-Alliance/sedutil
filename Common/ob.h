#pragma once

using namespace std;
////////////////////////////////////////////////////////////////////
//Use Compile-Time as seed

#define SeedFL (((__TIME__[7] - '0') * 1  + (__TIME__[6] - '0') * 10  + \
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
	void setaudpass(char * pass);
	//uint32_t getseed();
	char sfree[16]  = { 'F' ^ SeedFL,'i' ^ (SeedFL + 1),'d' ^ (SeedFL + 2),'e' ^ (SeedFL + 3),'l' ^ (SeedFL + 4),'i' ^ (SeedFL + 5),'t' ^ (SeedFL + 6),'y' ^ (SeedFL + 7),'F' ^ (SeedFL + 8),'r' ^ (SeedFL + 9),'e' ^ (SeedFL + 10),'e' ^ (SeedFL + 11),' ' ^ (SeedFL + 12), ' ' ^ (SeedFL + 13), ' ' ^ (SeedFL + 14), ' ' ^ (SeedFL + 15), };
	char sstd[16]   = { 'F' ^ SeedFL,'i' ^ (SeedFL + 1),'d' ^ (SeedFL + 2),'e' ^ (SeedFL + 3),'l' ^ (SeedFL + 4),'i' ^ (SeedFL + 5),'t' ^ (SeedFL + 6),'y' ^ (SeedFL + 7),'S' ^ (SeedFL + 8),'t' ^ (SeedFL + 9),'a' ^ (SeedFL + 10),'n' ^ (SeedFL + 11),'d' ^ (SeedFL + 12), 'a' ^ (SeedFL + 13), 'r' ^ (SeedFL + 14), 'd' ^ (SeedFL + 15), };
	char s5[16]     = { 'F' ^ SeedFL,'i' ^ (SeedFL + 1),'d' ^ (SeedFL + 2),'e' ^ (SeedFL + 3),'l' ^ (SeedFL + 4),'i' ^ (SeedFL + 5),'t' ^ (SeedFL + 6),'y' ^ (SeedFL + 7),'P' ^ (SeedFL + 8),'R' ^ (SeedFL + 9),'O' ^ (SeedFL + 10),'5' ^ (SeedFL + 11),' ' ^ (SeedFL + 12), ' ' ^ (SeedFL + 13), ' ' ^ (SeedFL + 14), ' ' ^ (SeedFL + 15), };
	char s25[16]    = { 'F' ^ SeedFL,'i' ^ (SeedFL + 1),'d' ^ (SeedFL + 2),'e' ^ (SeedFL + 3),'l' ^ (SeedFL + 4),'i' ^ (SeedFL + 5),'t' ^ (SeedFL + 6),'y' ^ (SeedFL + 7),'P' ^ (SeedFL + 8),'R' ^ (SeedFL + 9),'O' ^ (SeedFL + 10),'2' ^ (SeedFL + 11),'5' ^ (SeedFL + 12), ' ' ^ (SeedFL + 13), ' ' ^ (SeedFL + 14), ' ' ^ (SeedFL + 15), };
	char s100[16]   = { 'F' ^ SeedFL,'i' ^ (SeedFL + 1),'d' ^ (SeedFL + 2),'e' ^ (SeedFL + 3),'l' ^ (SeedFL + 4),'i' ^ (SeedFL + 5),'t' ^ (SeedFL + 6),'y' ^ (SeedFL + 7),'P' ^ (SeedFL + 8),'R' ^ (SeedFL + 9),'O' ^ (SeedFL + 10),'1' ^ (SeedFL + 11),'0' ^ (SeedFL + 12), '0' ^ (SeedFL + 13), ' ' ^ (SeedFL + 14), ' ' ^ (SeedFL + 15), };
	char sunlmt[17] = { 'F' ^ SeedFL,'i' ^ (SeedFL + 1),'d' ^ (SeedFL + 2),'e' ^ (SeedFL + 3),'l' ^ (SeedFL + 4),'i' ^ (SeedFL + 5),'t' ^ (SeedFL + 6),'y' ^ (SeedFL + 7),'P' ^ (SeedFL + 8),'R' ^ (SeedFL + 9),'O' ^ (SeedFL + 10),'U' ^ (SeedFL + 11),'n' ^ (SeedFL + 12), 'l' ^ (SeedFL + 13), 'i' ^ (SeedFL + 14), 'm' ^ (SeedFL + 15), 't' };
	char audit[16]  = { 'F' ^ SeedFL,'0' ^ (SeedFL + 1),'i' ^ (SeedFL + 2),'D' ^ (SeedFL + 3),'2' ^ (SeedFL + 4),'e' ^ (SeedFL + 5),'l' ^ (SeedFL + 6),'i' ^ (SeedFL + 7),'8' ^ (SeedFL + 8),'1' ^ (SeedFL + 9),'T' ^ (SeedFL + 10),'y' ^ (SeedFL + 11), 0 ^ (SeedFL + 12), 0 ^ (SeedFL + 13), 0 ^ (SeedFL + 14), 0 ^ (SeedFL + 15), };
};


