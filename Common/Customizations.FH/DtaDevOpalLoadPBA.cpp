/* C:B**************************************************************************
This software is Copyright 2014, 2022 Bright Plaza Inc. <drivetrust@drivetrust.com>

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
/** Device class for Opal 2.0 SSC
 * also supports the Opal 1.0 SSC
 */

#if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__) || defined(__APPLE__)
#else // Windows
#pragma warning(disable: 4224) //C2224: conversion from int to char , possible loss of data
#pragma warning(disable: 4244) //C4244: 'argument' : conversion from 'uint16_t' to 'uint8_t', possible loss of data
#pragma warning(disable: 4996)
#pragma comment(lib, "rpcrt4.lib")  // UuidCreate - Minimum supported OS Win 2000
#endif

#include "os.h"
#include <log/log.h>

#if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__) || defined(__APPLE__)
#else // Windows
#include <Windows.h>
#include "compressapi-8.1.h"
#include "sedsize.h" 
#endif

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <iomanip>

#include "DtaDevOpal.h"
#include "DtaHashPwd.h"
#include "DtaEndianFixup.h"
#include "DtaStructures.h"
#include "DtaCommand.h"
#include "DtaResponse.h"
#include "DtaSession.h"
#include "DtaHexDump.h"
#include <signal.h>


#include "ob.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include "uuid.h"
#endif

using namespace std;

////////////////////////////////////////////////////////////////////

class DtaDevOpalLock :: DevDevOpal {
    uint8_t loadPBA(char * password, char * filename) ;
}

uint8_t DtaDevOpal::loadPBA(char * password, char * filename) {

	if (!skip_activate)
		return (static_cast<DtaDevOpalLock,self>)->loadPBA_M(password, filename));

    
    // from original
	LOG(D) << "Entering DtaDevOpal::loadPBAimage_O()" << filename << " " << dev;
	uint8_t lastRC;
	uint32_t blockSize = BLOCKSIZE_HI;
	uint32_t filepos = 0;
	uint32_t eofpos;
	ifstream pbafile;

	if (disk_info.enclosure) {
		// do not change host property for enclosure
		adj_host = 0;
		blockSize = 1950;
		//printf("disk_info.enclosure=%d Do not change host property at all ; blockSize=%d\n", disk_info.enclosure, blockSize);
	}
	else
	if (Tper_sz_MaxComPacketSize > IO_BUFFER_LENGTH_HI) adj_host = 1; else adj_host = 2;
	if (!disk_info.enclosure) { // only if not enclosure need recovery of property
		lastRC = properties();
		if (lastRC != 0) {
			LOG(E) << "adjust host property fail ; go back to MINIMUM packet size";
			// improve later on with MINIMUM packet size 
			adj_host = 0;
			properties();
		}
		else {
			fill_prop(false);
			blockSize = (adj_host == 1) ? BLOCKSIZE_HI : Tper_sz_MaxIndTokenSize - 110; // 60;
		}
	}
	vector <uint8_t> buffer, lengthtoken; 

	if (!disk_info.enclosure) { // only if not enclosure need packet size change 
		uint32_t tperMaxPacket = Tper_sz_MaxComPacketSize;
		uint32_t tperMaxToken = Tper_sz_MaxIndTokenSize;
        
        blockSize = (MAX_BUFFER_LENGTH > tperMaxPacket) ? tperMaxPacket : MAX_BUFFER_LENGTH;
		if (blockSize > (tperMaxToken - 4)) blockSize = tperMaxToken - 4;
		//printf("tperMaxPacket=%ld  tperMaxToken=%ld before blockSize=%ld\n", tperMaxPacket, tperMaxToken, blockSize);
		//vector <uint8_t> buffer, lengthtoken;
		blockSize -= sizeof(DTA_Header) + 50;  // packet overhead
        printf("tperMaxPacket=%u  tperMaxToken=%u After blockSize=%u\n", tperMaxPacket, tperMaxToken, blockSize);
	}
 
	buffer.resize(blockSize);
	pbafile.open(filename, ios::in | ios::binary);
	if (!pbafile) {
		LOG(E) << "Unable to open PBA image file " << filename;
		return DTAERROR_COMMAND_ERROR;
	}
	pbafile.seekg(0, pbafile.end);
	eofpos = (uint32_t)pbafile.tellg();
	pbafile.seekg(0, pbafile.beg);
    printf("eofpos(size of pbaimage file)=%u\n", eofpos);
	DtaCommand *cmd = new DtaCommand();
	if (NULL == cmd) {
		LOG(E) << "Unable to create command object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
	}

	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object ";
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete cmd;
		delete session;
		pbafile.close();
		return lastRC;
	}
	LOG(D) << "Writing PBA to " << dev;

	while (!pbafile.eof()) {
		if (eofpos == filepos) break;
		if ((eofpos - filepos) < blockSize) {
			blockSize = eofpos - filepos; // handle a short last block
			buffer.resize(blockSize);
		}
		//printf("before pba write filepos=%ld blockSize=%ld\n", filepos, blockSize);
		lengthtoken.clear();
		lengthtoken.push_back(0xe2);
		lengthtoken.push_back((uint8_t)((blockSize >> 16) & 0x000000ff));
		lengthtoken.push_back((uint8_t)((blockSize >> 8) & 0x000000ff));
		lengthtoken.push_back((uint8_t)(blockSize & 0x000000ff));
		pbafile.read((char *)buffer.data(), blockSize);
		cmd->reset(OPAL_UID::OPAL_MBR, OPAL_METHOD::SET);
		cmd->addToken(OPAL_TOKEN::STARTLIST);
		cmd->addToken(OPAL_TOKEN::STARTNAME);
		cmd->addToken(OPAL_TOKEN::WHERE);
		cmd->addToken(filepos);
		cmd->addToken(OPAL_TOKEN::ENDNAME);
		cmd->addToken(OPAL_TOKEN::STARTNAME);
		cmd->addToken(OPAL_TOKEN::VALUES);
		cmd->addToken(lengthtoken);
		cmd->addToken(buffer);
		cmd->addToken(OPAL_TOKEN::ENDNAME);
		cmd->addToken(OPAL_TOKEN::ENDLIST);
		cmd->complete();
		if ((lastRC = session->sendCommand(cmd, response)) != 0) {
			delete cmd;
			delete session;
			pbafile.close();
			return lastRC;
		}
		filepos += blockSize;
		cout << "Writing PBA " << filepos << " of " << eofpos << " " << (uint16_t)(((float)filepos / (float)eofpos) * 100) << "% blkSz=" << blockSize << " \r";
		// LOG(D) << "Writing PBA " << filepos << " of " << eofpos << " " << (uint16_t)(((float)filepos / (float)eofpos) * 100) << "% blkSz=" << blockSize << " \n";
		//printf("\rWriting PBA %ld (%ld) %d  blkSz=%ld %s", filepos, eofpos, "%", (uint16_t)((filepos * 100) / eofpos),blockSize, dev);
	}
	cout << "\n";
	delete cmd;
	delete session;
	pbafile.close();
	LOG(D) << "PBA image  " << filename << " written to " << dev;
	LOG(D) << "Exiting DtaDevOpal::loadPBAimage_O()";
	return 0;
}

///////////////////////////////////////////////////////////////////


uint8_t DtaDevOpalLock::loadPBA(char * password, char * filename) {
#if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__) || defined(__APPLE__) || (WINDOWS7)
#pragma unused(password)
#pragma unused(filename)
        LOG(D1) << "DtaDevOpalLock::loadPBA() isn't supported in Linux";
	return 0;
#else
	LOG(D) << "Entering DtaDevOpalLock::loadPBA()" << filename << " " << dev;
	if (disk_info.Locking_MBRshadowingNotSupported) {
		LOG(E) << "SSC device does not support shadow MBR";
		return DTAERROR_INVALID_COMMAND;
	}

	if (disk_info.enclosure) {
		LOG(E) << "Does Not Support Large PBA write to Enclosure Drive"; 
		return NOT_SUPPORT_LARGE_PBA_WRITE_TO_ENCLOSURE_DRIVE; 
	}

	uint8_t embed = 1;
	uint8_t lastRC;
	uint64_t fivepercent = 0;
	uint64_t imgsize;
	int complete = 4;
	typedef struct { uint8_t  i : 2; } spinnertik;
	spinnertik spinnertick;
	spinnertick.i = 0;
	char star[] = "*";
	char spinner[] = "|/-\\";
	char progress_bar[] = "   [                     ]";
	uint32_t blockSize = 1950; // = 16384; // 57344; // 57344=512*112=E000h 1950=0x79E;  16384=512*32=0x4000
	uint32_t filepos = 0;
	uint64_t sz;
	ifstream pbafile;
	ofstream progfile;
	char progbuf[50];
	// for decompression
	PBYTE DecompressedBuffer = NULL;
	//uint64_t DecompressedBufferSize = NULL;
	SIZE_T DecompressedBufferSize = NULL;
	PBYTE CompressedBuffer = NULL;
	uint64_t CompressedBufferSize = 0;
	DECOMPRESSOR_HANDLE Decompressor = NULL;
	DecompressedBuffer = NULL;
	BOOL Success;
	SIZE_T  DecompressedDataSize;
	//void * somebuf = NULL;
	uint8_t oper = 0; 
	vector <uint8_t> buffer; // 0 buffer  (57344, 0x00),
	vector <uint8_t> lengthtoken;

	if (embed == 0) {
		pbafile.open(filename, ios::in | ios::binary);
		if (!pbafile) {
			adj_host_prop(0); // reset host properties to smaller size
			LOG(E) << "Unable to open PBA image file " << filename;
			return DTAERROR_OPEN_ERR;
		}
		pbafile.seekg(0, pbafile.end);
		fivepercent = (uint64_t)((pbafile.tellg() / 20) / blockSize) * blockSize;
	}
	else {
		const char * fname[] = { "sedutil-cli.exe" , "prog"}; // , "..\\rc\\sedutil.exe", "..\\rc\\UEFI.img"	};
		#include "sedsize.h" 
		pbafile.open(fname[0], ios::in | ios::binary);
		pbafile.seekg(0, pbafile.end);
		imgsize = pbafile.tellg();
		pbafile.seekg(0, pbafile.beg);
		pbafile.seekg(sedsize);
		//LOG(D) << "read pointer=" << pbafile.tellg();
		int comprss = 1;
		if (comprss) { 
			CompressedBufferSize = imgsize - sedsize;
			CompressedBuffer = (PBYTE)_aligned_malloc(CompressedBufferSize, IO_BUFFER_ALIGNMENT);
			if (!CompressedBuffer)
			{
				adj_host_prop(0); // reset host properties to smaller size
				LOG(E) << "Cannot allocate memory for compressed buffer.";
				return DTAERROR_OPEN_ERR;
			}
			pbafile.read((char *)CompressedBuffer, CompressedBufferSize); // read all img data
            
			Success = CreateDecompressor( // nozero = OK; 0 : NG
				COMPRESS_ALGORITHM_XPRESS_HUFF, //  Compression Algorithm
				NULL,                           //  Optional allocation routine
				&Decompressor);                 //  Handle
			if (!Success)
			{
				
				LOG(E) << "Cannot create a decompressor: " << GetLastError();
				goto done;
			}
			//  Query decompressed buffer size.
			Success = Decompress(
				Decompressor,                //  Compressor Handle
				CompressedBuffer,            //  Compressed data
				CompressedBufferSize,        //  Compressed data size
				NULL,                        //  Buffer set to NULL
				0,                           //  Buffer size set to 0
				&DecompressedBufferSize);    //  Decompressed Data size
											 // Allocate memory for decompressed buffer.
			if (!Success)
			{
				DWORD ErrorCode = GetLastError();
				// Note that the original size returned by the function is extracted 
				// from the buffer itself and should be treated as untrusted and tested
				// against reasonable limits.
				if (ErrorCode != ERROR_INSUFFICIENT_BUFFER)
				{
					LOG(E) << "Cannot query decompress data: " << ErrorCode ;
					//printf("DecompressedBufferSize=%I64d \n", DecompressedBufferSize);
					goto done;
				}
			}
				DecompressedBuffer = (PBYTE)_aligned_malloc(DecompressedBufferSize + (1024 * IO_BUFFER_ALIGNMENT), IO_BUFFER_ALIGNMENT);
				if (!DecompressedBuffer)
				{
					LOG(E) << "Cannot allocate memory for decompressed buffer";
					goto done;
				}
				//somebuf = malloc(DecompressedBufferSize);
				//if (!somebuf) {
				//	LOG(E) << "Cannot allocate memory for somebuf buffer";
				//	goto done;
				//} 
				memset(DecompressedBuffer, 0, DecompressedBufferSize);

			//  Decompress data 
			Success = Decompress(
				Decompressor,               //  Decompressor handle
				CompressedBuffer,           //  Compressed data
				CompressedBufferSize,       //  Compressed data size
				DecompressedBuffer,         //  Decompressed buffer
				DecompressedBufferSize,     //  Decompressed buffer size
				&DecompressedDataSize);     //  Decompressed data size
			if (!Success)
			{
				LOG(E) << "Cannot really decompress data: " << GetLastError();
				//LOG(D) << ("DecompressedBufferSize=%I64d DecompressedDataSize=%I64d\n", DecompressedBufferSize, DecompressedDataSize);
				goto done;
			}
				
			/*
			printf("CompressedBuffer size: %I64d; DecompressedBufferSize:%I64d; DecompressedDataSize: %I64d\n",
				CompressedBufferSize,
				DecompressedBufferSize,
				DecompressedDataSize);
			printf("File decompressed.\n");
			*/
		done:
			// house keeping buffer and file handler
			if (Decompressor != NULL)
			{
				LOG(D1) << "free Decompressor" << endl;
				CloseDecompressor(Decompressor);
			}
			if (CompressedBuffer)
			{
				LOG(D1) << "free CompressBuffer" << endl;
				_aligned_free(CompressedBuffer);
			}
		} // end cmprss
		// printf("\nend of decompression Tper_sz_MaxComPacketSize = %d\n", Tper_sz_MaxComPacketSize);

		if (!Success)
		{
			adj_host_prop(0); // reset host properties to smaller size
			return DTAERROR_OPEN_ERR;
		}

		//fivepercent = (uint64_t)((DecompressedBufferSize / 20) / blockSize) * blockSize;
	}
	// change FAT uuid and disk label
	UUID uuid;
	uint8_t struuid[64];
	vector <uint8_t> uu = ugenv(uuid, struuid);
	for (uint8_t i = 0; i < 4; i++) { DecompressedBuffer[0x100027 + i] = uu.at(i); DecompressedBuffer[0x27 + i] = uu.at(i);
		DecompressedBuffer[0x1b8 + i] = uu.at(i);
	}
	for (uint8_t i = 0; i < 11; i++) { DecompressedBuffer[0x10002b + i] = struuid[i]; DecompressedBuffer[0x2b + i] = struuid[i];
	}

	// embedded info to MBR
	bool saved_flag = no_hash_passwords;
	no_hash_passwords = false;
	char * model = getModelNum();
	char * firmware = getFirmwareRev();
	char * sernum = getSerialNum();
	vector<uint8_t> hash;

	IFLOG(D1) printf("model : %s ", model);
	IFLOG(D1) printf("firmware : %s ", firmware);
	IFLOG(D1) printf("serial : %s\n", sernum);
	hash.clear();
	LOG(D1) << "start hashing";
	DtaHashPwd(hash, sernum, this,1000);
	LOG(D1) << "end hashing";
	//////IFLOG(D1) printf("hashed size = %zd\n", hash.size());
	//////IFLOG(D1) printf("hashed serial number is ");
	//////IFLOG(D1) // should never expose the hashed series , need to comment out when release
	//////for (uint8_t i = 0; i < hash.size(); i++)
	//////{
	//////	printf("%02X", hash.at(i));
	//////}
	//////printf("\n"); // end of IFLOG(D1)
	// try dump decompressed buffer of sector 0 , 1 
	//DtaHexDump(DecompressedBuffer + 512, 512);
	// write 32-byte date into buffer 
	for (uint8_t i = 2; i < hash.size(); i++)
	{
		DecompressedBuffer[512 + 64 + i - 2] = hash.at(i);
	}

	hash.clear();
	LOG(D1) << "start hashing";
	char mbrstr[16] = { 'F','i','d','e','l','i','t','y','L','o','c','k','M', 'B', 'R', }; // "FidelityLockMBR";
	DtaHashPwd(hash, mbrstr, this,1000); // why IFLOG(D4)
	for (uint8_t i = 2; i < hash.size(); i++)
	{
		DecompressedBuffer[512 + 96 + i - 2] = hash.at(i);
	}

	// write license level 
	hash.clear();
	//LOG(D1) << "start hashing license level";
	//uint8_t idx[16];
	//char st1[16];

	char sbnk[16] = { ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ', ' ', ' ', ' ', };
	char lic_level[18];
	memset(lic_level, 0, 18);

	if (!memcmp("0:", LicenseLevel, 2)) { // correct feature set
		setlic(lic_level, LicenseLevel);
	}
	else {
		memcpy(lic_level, sbnk, 16);
		printf("no license = %s\n", lic_level);
	}

	//IFLOG(D4) 
	//	for (uint8_t i = 0; i < 16; i++) { printf("%02X", lic_level[i]); };
	hash.clear();
	DtaHashPwd(hash, lic_level, this,1000);
	for (uint8_t i = 2; i < hash.size(); i++)
	{
		DecompressedBuffer[512 + 128 + i - 2] = hash.at(i);
	}
	IFLOG(D4)  // remove hexdump when release 
		DtaHexDump(DecompressedBuffer + 512, 512);

	no_hash_passwords = saved_flag;

	if (0 == fivepercent) fivepercent++;
	if (embed ==0) 
		pbafile.seekg(0, pbafile.beg);

	// decompressed file size is established at this moment
	if ((DecompressedBufferSize > 1950) && (Tper_sz_MaxComPacketSize > 2048)) {
		// adjust host property 
	//	printf("\nwrite MBR lengtn = %ld Tper_sz_MaxComPacketSize = %d\n", DecompressedBufferSize, Tper_sz_MaxComPacketSize);
		if (disk_info.enclosure) { // do not change host property for enclosure
			adj_host = 0;
			blockSize = 1950 ;
			lastRC = 0;
			buffer.resize(blockSize);
			//printf("disk_info.enclosure=%d Do not change host property at all ; blockSize=%d\n", disk_info.enclosure, blockSize);
		}
		else if (Tper_sz_MaxComPacketSize > IO_BUFFER_LENGTH_HI) {
			adj_host = 1;
			lastRC = properties();
		}
		else if ((Tper_sz_MaxComPacketSize > 2048) && (Tper_sz_MaxComPacketSize > IO_BUFFER_LENGTH_MI) && disk_info.asmedia) // make sure it is T7
		{
			adj_host = 3; // force to minimum 3;
			lastRC = properties();
			printf("set adj_host to 3\n"); 
		} 
		else {
			printf("set adj_host to 2\n");
			adj_host = 2;
			lastRC = properties();
		}
		if (disk_info.enclosure == 0) {
			if (lastRC != 0) {
				LOG(E) << "adjust host property fail ; go back to MINIMUM packet size";
				// improve later on with MINIMUM packet size 
				adj_host = 0;
				lastRC = properties();
				blockSize = 1950; // Minimum block size; 
				if (lastRC != 0) LOG(E) << "unable to adjust host property back to original MINIMUM setting";
			}
			else if ((adj_host == 3) && disk_info.asmedia) { // make sure it is T7 
				fill_prop(false);
				blockSize = BLOCKSIZE_MI; //  110; // 60; - 512, 1024 NG
				if (DecompressedBufferSize < blockSize) {
					blockSize = DecompressedBufferSize; // truncate blockSize to len 
				}
				buffer.resize(blockSize);
			}
			else {
				// adj_io_buffer_length = Tper_sz_MaxComPacketSize - adjust_more_t7
				fill_prop(false);
				blockSize = (adj_host == 1) ? BLOCKSIZE_HI : Tper_sz_MaxIndTokenSize - 110; //  110; // 60; - 512, 1024 NG
				if (DecompressedBufferSize < blockSize) {
					blockSize = DecompressedBufferSize; // truncate blockSize to len 
				}
				buffer.resize(blockSize);
			}
		}
	}
	else { // len <= 1950
		if (DecompressedBufferSize < blockSize) { // len < 1950 very unlikely but it may happen
			blockSize = DecompressedBufferSize;
			buffer.resize(blockSize); // need to resize buffer due to truncated size of data length by user 
		}
	}
	
	//printf("loadPBAimage : After Tper size exchange , blockSize=%d len=%d sizeof(SIZE_T)=%d adj_io_buffer_length=%d \
	//	adj_host=%d disk_info.asmedia=%d disk_info.enclosure=%d\n", 
	//	blockSize,  DecompressedBufferSize, sizeof(SIZE_T), adj_io_buffer_length,
	//	adj_host, disk_info.asmedia, disk_info.enclosure
	//	);

	DtaCommand *cmd = new DtaCommand();
	if (NULL == cmd) {
		LOG(E) << "Unable to create command object " << dev;
		adj_host_prop(0); // reset host properties to smaller size
		return DTAERROR_OBJECT_CREATE_FAILED;
	}

	session = new DtaSession(this);
	if (NULL == session) {
		LOG(E) << "Unable to create session object " << dev;
		adj_host_prop(0); // reset host properties to smaller size
		return DTAERROR_OBJECT_CREATE_FAILED;
	}
	if ((lastRC = session->start(OPAL_UID::OPAL_LOCKINGSP_UID, password, OPAL_UID::OPAL_ADMIN1_UID)) != 0) {
		delete cmd;
		delete session;
		pbafile.close();
		adj_host_prop(0); // reset host properties to smaller size
		return lastRC;
	}
	/* 
	LOG(D1) << "Start transaction";
	cmd->reset();
	cmd->addToken(OPAL_TOKEN::STARTTRANSACTON);
	cmd->addToken(OPAL_TOKEN::WHERE);
	cmd->complete();
	if ((lastRC = session->sendCommand(cmd, response)) != 0) {
		delete cmd;
		delete session;
		pbafile.close();
		return lastRC;
	} */
	
	LOG(D) << "Writing PBA to " << dev;
	uint32_t startpos;
	startpos = 0; // no effect of startpos 34020000;  // add startpos for debug purpose, delete after done debug
	while ( (filepos + startpos < DecompressedBufferSize)) {
		if (embed == 0) {
			pbafile.read((char *)buffer.data(), blockSize);
		}
		else {
			
				signal(SIGSEGV, SignalHandler);
				try { 
					//memcpy(&buffer[0], &(DecompressedBuffer[filepos]), ((filepos + blockSize) <= DecompressedBufferSize) ? blockSize: DecompressedBufferSize - filepos);
					//buffer.erase(buffer.begin(), buffer.end());
					
					sz = (((filepos + blockSize) <= DecompressedBufferSize) ? blockSize : DecompressedBufferSize - filepos);
					//buffer.clear();
					//buffer.insert(buffer.begin(), DecompressedBuffer + filepos, DecompressedBuffer +filepos + sz); // could pointer be a problem 
					if (sz < blockSize) {
							buffer.resize(sz);
					}
					//buffer.resize(sz); // always use the same buffer
					memcpy_s( buffer.data(), sz, DecompressedBuffer + filepos, sz); // better way to copy buf to vector, no mem reallocation 
					//printf("buffer.size()=%zd ; DecompressedBuffer+filepos=%I64X; DecompressedBuffer+filepos+sz=%I64X; filepos=%ld; sz=%I64d; blkSz=%ld; \n", buffer.size(), (uint64_t)DecompressedBuffer + filepos, (uint64_t)DecompressedBuffer + filepos + sz, filepos, sz, blockSize );
				}
				catch (char *e) {
					LOG(E) << "Exception Caught: " << e;
					break;
				}
		}


		if (!(filepos % (blockSize * 5))) {
#if 1 // temp take out spinner 
		//	progress_bar[1] = spinner[spinnertick.i++];
			//printf("\r%s %i(%I64d) %s", progress_bar,filepos, DecompressedBufferSize, dev);
			//

			if (sizeof(SIZE_T) == 8)
				printf("\r%c %i(%I64d) %d%% BklSz=%ld %s ", spinner[spinnertick.i++],  filepos, (DecompressedBufferSize), (uint16_t)(((float)filepos / (float)DecompressedBufferSize) * 100), blockSize , dev);
			else
				printf("\r%c %i(%ld) %d%% BklSz=%ld %s ", spinner[spinnertick.i++], filepos, (DecompressedBufferSize), (uint16_t)(((float)filepos / (float)DecompressedBufferSize) * 100), blockSize, dev);
			fflush(stdout);
#endif
			// open progress output file at slower rate
			if (!(filepos % (blockSize * 50))) {
				progfile.open(sernum, ios::out);
				memset(progbuf, 0, 50);
				if (sizeof(SIZE_T) == 8)
					sprintf(progbuf, "\r%i(%I64d)", filepos, DecompressedBufferSize);
				else
					sprintf(progbuf, "\r%i(%ld)", filepos, DecompressedBufferSize);
				progfile.write(progbuf, strlen(progbuf));
				progfile.close();
			}
		}

		lengthtoken.clear();
		lengthtoken.push_back(0xe2); // E2 is byte string which mean the followind data is byte-stream, but for read, there is no byte string so it should be E0
		lengthtoken.push_back(0x00);
		lengthtoken.push_back((uint8_t)(sz >> 8)); // (0xE0); // 
		lengthtoken.push_back((uint8_t)(sz & 0xFF));// (0x00); // 
		cmd->reset(OPAL_UID::OPAL_MBR, OPAL_METHOD::SET);
		cmd->addToken(OPAL_TOKEN::STARTLIST);
		cmd->addToken(OPAL_TOKEN::STARTNAME);
		cmd->addToken(OPAL_TOKEN::WHERE);
		cmd->addToken(filepos + startpos ); // remove startpos after done debug
		cmd->addToken(OPAL_TOKEN::ENDNAME);
		cmd->addToken(OPAL_TOKEN::STARTNAME);
		cmd->addToken(OPAL_TOKEN::VALUES);
		cmd->addToken(lengthtoken);
		cmd->addToken(buffer);
		cmd->addToken(OPAL_TOKEN::ENDNAME);
		cmd->addToken(OPAL_TOKEN::ENDLIST);
		cmd->complete();
		if ((lastRC = session->sendCommand(cmd, response,oper = 1)) != 0) {
			delete cmd;
			delete session;
			pbafile.close();
            if (DecompressedBuffer) _aligned_free(DecompressedBuffer);
			adj_host_prop(0); // reset host properties to smaller size
			return lastRC;
		}

		filepos += blockSize;
		//printf("filepos=%ld blockSize=%ld sz=%ld DecompressedBufferSize=%I64d \
		//	startpos=%ld filepos + startpos =%ld\n",
		//	filepos , blockSize, sz, DecompressedBufferSize, startpos, filepos + startpos);

	} // end of while 

	//printf("\r%s %i(%I64d) bytes written \n", progress_bar, filepos, DecompressedBufferSize);
	if (sizeof(SIZE_T) == 8)
		printf("\n%Id(%I64d) bytes written to %s \n", (int32_t)(filepos - blockSize + sz), DecompressedBufferSize, dev);
	else
		printf("\n%Id(%ld) bytes written to %s \n", (int32_t)(filepos - blockSize + sz), DecompressedBufferSize, dev);
	// open progress output file
	progfile.open(sernum, ios::out);
	memset(progbuf, 0, 50);
	if (sizeof(SIZE_T) == 8)
		sprintf(progbuf, "\r%i(%I64d) Complete PBA write", filepos, DecompressedBufferSize);
	else
		sprintf(progbuf, "\r%i(%ld) Complete PBA write", filepos, DecompressedBufferSize);
	progfile.write(progbuf, strlen(progbuf));
	progfile.close();

/*	LOG(D1) << "end transaction";
	cmd->reset();
	cmd->addToken(OPAL_TOKEN::ENDTRANSACTON);
	cmd->addToken(OPAL_TOKEN::WHERE);
	cmd->complete(); 
	if ((lastRC = session->sendCommand(cmd, response)) != 0) {
		delete cmd;
		delete session;
		pbafile.close();
		adj_host_prop(0); // reset host properties to smaller size
		return lastRC;
	} */
	delete cmd;
	delete session;
	pbafile.close();
	if (embed ==0)
		LOG(D) << "PBA image  " << filename << " written to " << dev;
	else {
		LOG(D) << "PBA image written to " << dev;
	}
	
    _aligned_free(DecompressedBuffer);
	adj_host_prop(0); // reset host properties to smaller size
	LOG(D) << "Exiting DtaDevOpalLock::loadPBA() " << dev;

	return 0;
#endif
}
