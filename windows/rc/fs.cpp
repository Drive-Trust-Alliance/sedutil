// write file size to sedsize.h
// how to compile
// CL /EHsc fs.cpp 
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;
int main (int argc, char * argv[] ) {

    char buf[64] = {' '};
	char exename[64] = { ' ' };
    const char * fname[] = {"sedutil.exe","..\\Common\\sedsize.h",""};
    ifstream pbafile;

	switch(argc) {
		case 1 : 
			strncpy(exename, fname[0], strlen(argv[1]));
			break;
		case 2: 
			strncpy(exename, argv[1], strlen(argv[1]));
			break;
		default:
			strncpy(exename, fname[0], strlen(argv[1]));
			break;

	}
	cout << exename << endl;

    pbafile.open(exename, ios::in | ios::binary); // get sedutil-cli.exe size
    pbafile.seekg(0, pbafile.end); // seek to the end of sedutil.exe
    long int pos_beg = pbafile.tellg(); 
    pbafile.close();
    
    ofstream pba;
    pba.open(fname[1],ios::out );
    sprintf(buf,"#define sedsize %ld",pos_beg);
    cout << fname[1] << endl;
    cout << buf << endl ;;
    pba.write(buf,sizeof(buf));
    pba.close();
}
    
    
    
    