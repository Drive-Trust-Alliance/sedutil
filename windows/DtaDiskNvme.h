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

// Intel RST data structure 

#pragma pack()

//////////////////////////////////////////////////////////////////
// for Intel RST NVMe
//////////////////////////////////////////////////////////////////
// #pragma pack() : mean no pack
// #pragma pack(1) : mean pack member align at byte level
// 
#pragma pack(push, 1)

typedef union
{
	struct
	{
		ULONG Opcode : 8;
		ULONG FUSE : 2;
		ULONG _Rsvd : 4;
		ULONG PSDT : 2;
		ULONG CID : 16;
	} DUMMYSTRUCTNAME;
	ULONG AsDWord;
} NVME_CDW0, *PNVME_CDW0;

// NVMe Command Format
// See NVMe specification 1.3c Section 4.2, Figure 10
typedef union
{
	struct
	{
		ULONG   CNS : 2;
		ULONG   _Rsvd : 30;
	} DUMMYSTRUCTNAME;
	ULONG AsDWord;
} NVME_IDENTIFY_CDW10, *PNVME_IDENTIFY_CDW10;

// NVMe Specification < 1.3
typedef union
{
	struct
	{
		ULONG   LID : 8;
		ULONG   _Rsvd1 : 8;
		ULONG   NUMD : 12;
		ULONG   _Rsvd2 : 4;
	} DUMMYSTRUCTNAME;
	ULONG   AsDWord;
} NVME_GET_LOG_PAGE_CDW10, *PNVME_GET_LOG_PAGE_CDW10;

// NVMe Specification >= 1.3
typedef union
{
	struct
	{
		ULONG   LID : 8;
		ULONG   LSP : 4;
		ULONG   Reserved0 : 3;
		ULONG   RAE : 1;
		ULONG   NUMDL : 16;
	} DUMMYSTRUCTNAME;
	ULONG   AsDWord;
} NVME_GET_LOG_PAGE_CDW10_V13, *PNVME_GET_LOG_PAGE_CDW10_V13;

typedef struct
{
	// Common fields for all commands
	NVME_CDW0           CDW0;

	ULONG               NSID;
	ULONG               _Rsvd[2];
	ULONGLONG           MPTR;
	ULONGLONG           PRP1;
	ULONGLONG           PRP2;

	// Command independent fields from CDW10 to CDW15
	union
	{
		// Admin Command: Identify (6)
		struct
		{
			NVME_IDENTIFY_CDW10 CDW10;
			ULONG   CDW11;
			ULONG   CDW12;
			ULONG   CDW13;
			ULONG   CDW14;
			ULONG   CDW15;
		} IDENTIFY;

		// Admin Command: Get Log Page (2)
		struct
		{
			NVME_GET_LOG_PAGE_CDW10 CDW10;
			//NVME_GET_LOG_PAGE_CDW10_V13 CDW10;
			ULONG   CDW11;
			ULONG   CDW12;
			ULONG   CDW13;
			ULONG   CDW14;
			ULONG   CDW15;
		} GET_LOG_PAGE;
	} u;
} NVME_CMD, *PNVME_CMD;

typedef struct _INTEL_NVME_PAYLOAD
{
	BYTE    Version;        // 0x001C
	BYTE    PathId;         // 0x001D
	BYTE    TargetID;       // 0x001E
	BYTE    Lun;            // 0x001F
	NVME_CMD Cmd;           // 0x0020 ~ 0x005F
	DWORD   CplEntry[4];    // 0x0060 ~ 0x006F
	DWORD   QueueId;        // 0x0070 ~ 0x0073
	DWORD   ParamBufLen;    // 0x0074
	DWORD   ReturnBufferLen;// 0x0078
	BYTE    __rsvd2[0x28];  // 0x007C ~ 0xA3
} INTEL_NVME_PAYLOAD, *PINTEL_NVME_PAYLOAD;

typedef struct _INTEL_NVME_PASS_THROUGH
{
	SRB_IO_CONTROL SRB;     // 0x0000 ~ 0x001B
	INTEL_NVME_PAYLOAD Payload;
	BYTE DataBuffer[0x1000];
} INTEL_NVME_PASS_THROUGH, *PINTEL_NVME_PASS_THROUGH;
#pragma pack(pop)

#define IOCTL_INTEL_NVME_PASS_THROUGH CTL_CODE(0xf000, 0xA02, METHOD_BUFFERED, FILE_ANY_ACCESS);

#pragma pack(push,1)

typedef	struct _IDENTIFY_DEVICE_OUTDATA
{
	SENDCMDOUTPARAMS	SendCmdOutParam;
	BYTE				Data[IDENTIFY_BUFFER_SIZE - 1];
} IDENTIFY_DEVICE_OUTDATA, *PIDENTIFY_DEVICE_OUTDATA;

struct BIN_IDENTIFY_DEVICE
{
	BYTE		Bin[512];
};

struct NVME_IDENTIFY_DEVICE
{
	CHAR		Reserved1[4];
	CHAR		SerialNumber[20];
	CHAR		Model[40];
	CHAR		FirmwareRev[8];
	CHAR		Reserved2[9];
	CHAR		MinorVersion;
	SHORT		MajorVersion;
	CHAR		Reserved3[428];
};

struct ATA_IDENTIFY_DEVICE
{
	WORD		GeneralConfiguration;					//0
	WORD		LogicalCylinders;						//1	Obsolete
	WORD		SpecificConfiguration;					//2
	WORD		LogicalHeads;							//3 Obsolete
	WORD		Retired1[2];							//4-5
	WORD		LogicalSectors;							//6 Obsolete
	DWORD		ReservedForCompactFlash;				//7-8
	WORD		Retired2;								//9
	CHAR		SerialNumber[20];						//10-19
	WORD		Retired3;								//20
	WORD		BufferSize;								//21 Obsolete
	WORD		Obsolute4;								//22
	CHAR		FirmwareRev[8];							//23-26
	CHAR		Model[40];								//27-46
	WORD		MaxNumPerInterupt;						//47
	WORD		Reserved1;								//48
	WORD		Capabilities1;							//49
	WORD		Capabilities2;							//50
	DWORD		Obsolute5;								//51-52
	WORD		Field88and7064;							//53
	WORD		Obsolute6[5];							//54-58
	WORD		MultSectorStuff;						//59
	DWORD		TotalAddressableSectors;				//60-61
	WORD		Obsolute7;								//62
	WORD		MultiWordDma;							//63
	WORD		PioMode;								//64
	WORD		MinMultiwordDmaCycleTime;				//65
	WORD		RecommendedMultiwordDmaCycleTime;		//66
	WORD		MinPioCycleTimewoFlowCtrl;				//67
	WORD		MinPioCycleTimeWithFlowCtrl;			//68
	WORD		Reserved2[6];							//69-74
	WORD		QueueDepth;								//75
	WORD		SerialAtaCapabilities;					//76
	WORD		SerialAtaAdditionalCapabilities;		//77
	WORD		SerialAtaFeaturesSupported;				//78
	WORD		SerialAtaFeaturesEnabled;				//79
	WORD		MajorVersion;							//80
	WORD		MinorVersion;							//81
	WORD		CommandSetSupported1;					//82
	WORD		CommandSetSupported2;					//83
	WORD		CommandSetSupported3;					//84
	WORD		CommandSetEnabled1;						//85
	WORD		CommandSetEnabled2;						//86
	WORD		CommandSetDefault;						//87
	WORD		UltraDmaMode;							//88
	WORD		TimeReqForSecurityErase;				//89
	WORD		TimeReqForEnhancedSecure;				//90
	WORD		CurrentPowerManagement;					//91
	WORD		MasterPasswordRevision;					//92
	WORD		HardwareResetResult;					//93
	WORD		AcoustricManagement;					//94
	WORD		StreamMinRequestSize;					//95
	WORD		StreamingTimeDma;						//96
	WORD		StreamingAccessLatency;					//97
	DWORD		StreamingPerformance;					//98-99
	ULONGLONG	MaxUserLba;								//100-103
	WORD		StremingTimePio;						//104
	WORD		Reserved3;								//105
	WORD		SectorSize;								//106
	WORD		InterSeekDelay;							//107
	WORD		IeeeOui;								//108
	WORD		UniqueId3;								//109
	WORD		UniqueId2;								//110
	WORD		UniqueId1;								//111
	WORD		Reserved4[4];							//112-115
	WORD		Reserved5;								//116
	DWORD		WordsPerLogicalSector;					//117-118
	WORD		Reserved6[8];							//119-126
	WORD		RemovableMediaStatus;					//127
	WORD		SecurityStatus;							//128
	WORD		VendorSpecific[31];						//129-159
	WORD		CfaPowerMode1;							//160
	WORD		ReservedForCompactFlashAssociation[7];	//161-167
	WORD		DeviceNominalFormFactor;				//168
	WORD		DataSetManagement;						//169
	WORD		AdditionalProductIdentifier[4];			//170-173
	WORD		Reserved7[2];							//174-175
	CHAR		CurrentMediaSerialNo[60];				//176-205
	WORD		SctCommandTransport;					//206
	WORD		ReservedForCeAta1[2];					//207-208
	WORD		AlignmentOfLogicalBlocks;				//209
	DWORD		WriteReadVerifySectorCountMode3;		//210-211
	DWORD		WriteReadVerifySectorCountMode2;		//212-213
	WORD		NvCacheCapabilities;					//214
	DWORD		NvCacheSizeLogicalBlocks;				//215-216
	WORD		NominalMediaRotationRate;				//217
	WORD		Reserved8;								//218
	WORD		NvCacheOptions1;						//219
	WORD		NvCacheOptions2;						//220
	WORD		Reserved9;								//221
	WORD		TransportMajorVersionNumber;			//222
	WORD		TransportMinorVersionNumber;			//223
	WORD		ReservedForCeAta2[10];					//224-233
	WORD		MinimumBlocksPerDownloadMicrocode;		//234
	WORD		MaximumBlocksPerDownloadMicrocode;		//235
	WORD		Reserved10[19];							//236-254
	WORD		IntegrityWord;							//255
};

union IDENTIFY_DEVICE
{
	ATA_IDENTIFY_DEVICE	 A;
	NVME_IDENTIFY_DEVICE N;
	BIN_IDENTIFY_DEVICE	 B;
};
#pragma	pack(pop)

///// above Intel RST specific ////////////////////////////////////////////////////


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
public:
	BYTE physicalDriveId = 0;
	BYTE scsiTargetId;
	BYTE scsiPort;
	//BYTE lun;
	//BYTE scsiPathId;
};
