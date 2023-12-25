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

#include <signal.h>

#include "DtaDevOpal.h"
#include "DtaCommand.h"
#include "DtaSession.h"

using namespace std;

uint8_t DtaDevOpal::loadPBA(char * password, char * filename) {
	LOG(D) << "Entering DtaDevOpal::loadPBAimage()" << filename << " " << dev;
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
