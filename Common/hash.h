#pragma once

//using namespace std;
////////////////////////////////////////////////////////////////////

// window linux all need it 
class hashpwd {
public:
	hashpwd();
	~hashpwd();
	void DtaHashPassword(vector<uint8_t> &hash, char * password, vector<uint8_t> salt, unsigned int iter, uint8_t hashsize);
};
