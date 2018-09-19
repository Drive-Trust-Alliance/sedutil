/* C:B**************************************************************************
This software is Copyright 2014-2016 Bright Plaza Inc. <drivetrust@drivetrust.com>

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
#pragma once
#include "os.h"
#include "DtaDiskType.h"
/** Device specific implementation of disk access functions. */

typedef struct _SDWA {
	SCSI_PASS_THROUGH_DIRECT scsiDetails;
	WORD filler;
	char sensebytes[32];
} SDWA;


//CScsiCmdInquiry_StandardData NVME_INQUIRY_DATA;


typedef struct _CScsiCmdInquiry_StandardData {
	uint8_t        m_PeripheralDeviceType : 5;        //  0
	uint8_t        m_PeripheralQualifier : 3;
	uint8_t        m_Reserved_1 : 6;        //  1
	uint8_t        m_LUCong : 1;
	uint8_t        m_RMB : 1;
	uint8_t         m_Version;                              //  2
	uint8_t        m_ResponseDataFormat : 4;        //  3
	uint8_t        m_HiSup : 1;
	uint8_t        m_NormACA : 1;
	uint8_t        m_Reserved_2 : 1;
	uint8_t        m_Reserved_3 : 1;
	uint8_t         m_AdditionalLength;                     //  4
	uint8_t        m_Protect : 1;        //  5
	uint8_t        m_Reserved_4 : 2;
	uint8_t        m_3PC : 1;
	uint8_t        m_TPGS : 2;
	uint8_t        m_ACC : 1;
	uint8_t        m_SCCS : 1;
	uint8_t        m_ADDR16 : 1;        //  6
	uint8_t        m_Reserved_5 : 2;
	uint8_t        m_Obsolete_1 : 1;
	uint8_t        m_MultiP : 1;
	uint8_t        m_VS1 : 1;
	uint8_t        m_EncServ : 1;
	uint8_t        m_Obsolete_2 : 1;
	uint8_t        m_VS2 : 1;        //  7
	uint8_t        m_CmdQue : 1;
	uint8_t        m_Reserved_6 : 1;
	uint8_t        m_Obsolete_3 : 1;
	uint8_t        m_Sync : 1;
	uint8_t        m_WBus16 : 1;
	uint8_t        m_Reserved_7 : 1;
	uint8_t        m_Obsolete_4 : 1;
	uint8_t         m_T10VendorId[8];                       //  8
	uint8_t         m_ProductId[16];                        // 16
	uint8_t         m_ProductRevisionLevel[4];              // 32 4->40
} CScsiCmdInquiry_StandardData;                                  // 36

/*
typedef struct _NVME_INQUIRY_DATA {
	uint8_t fill1[20];
	char ProductSerial[20];
	uint8_t fill2[6];
	char ProductRev[8];
	char ProductID[40];
} NVME_INQUIRY_DATA;
*/

class DtaDiskNVME : public DtaDiskType {
public:
	DtaDiskNVME();
	~DtaDiskNVME();
	/** device specific initialization.
	* This function should perform the necessary authority and environment checking
	* to allow proper functioning of the program, open the device, perform an 
	* identify, add the fields from the identify response to the disk info structure
	* and if the device is an NVME device perform a call to Discovery0() to complete
	* the disk_info structure
	* @param devref character representation of the device is standard OS lexicon
	*/
	void init(const char * devref);
	/** OS specific method to send an NVME command to the device
	* @param cmd command to be sent to the device
	* @param protocol security protocol to be used in the command
	* @param comID communications ID to be used
	* @param buffer input/output buffer
	* @param bufferlen length of the input/output buffer
	*/
	uint8_t	sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID, void * buffer, uint32_t bufferlen);
	/** OS specific routine to send an NVME identify to the device */
	void identify(OPAL_DiskInfo& disk_info);
private:
	void * scsiPointer; /**< pointer to SDWB structure */
	HANDLE hDev; /**< Windows device handle */
	uint8_t isOpen = FALSE;
};
