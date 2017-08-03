// write file size to sedsize.h
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;
int main () {
    char buf[64] = {' '};
    const char * fname[] = {"sedutil.exe","..\\Common\\sedsize.h"};
    ifstream pbafile;

    pbafile.open(fname[0], ios::in | ios::binary); // get sedutil-cli.exe size
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
    
    
    
    