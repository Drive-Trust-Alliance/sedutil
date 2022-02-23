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
#pragma pack(push)
#pragma pack(1)

#define FC_TPER		  0x0001
#define FC_LOCKING    0x0002
#define FC_GEOMETRY   0x0003
#define FC_ENTERPRISE 0x0100
#define FC_DATASTORE  0x0202
#define FC_SINGLEUSER 0x0201
#define FC_OPALV100   0x0200
#define FC_OPALV200   0x0203
#define FC_OPALITE    0x0301
#define FC_PYRITE     0x0302
#define FC_PYRITE2    0x0303
#define FC_RUBY       0x0304
#define FC_BlockSID   0x0402
#define FC_NSLocking 0x0403 // Mandatory 2018 TCG feature set  1.32
#define FC_DataRemoval 0x0404
/** The Discovery 0 Header. As defined in
* Opal SSC Documentation
*/
typedef struct _Discovery0Header {
    uint32_t length; /**< the lenght of the header 48 in 2.00.100*/
    uint32_t revision; /**< revision of the header 1 in 2.00.100 */
    uint32_t reserved01;
    uint32_t reserved02;
    // the remainder of the structure is vendor specific and will not be addressed now
} Discovery0Header;

/** TPer Feature Descriptor. Contains flags indicating support for the
 * TPer features described in the OPAL specification. The names match the
 * OPAL terminology
 */
typedef struct _Discovery0TPerFeatures {
    uint16_t featureCode; /* 0x0001 in 2.00.100 */
    uint8_t reserved_v : 4;
    uint8_t version : 4;
    uint8_t length;
    /* big endian
    uint8_t reserved01 : 1;
    uint8_t comIDManagement : 1;
    uint8_t reserved02 : 1;
    uint8_t streaming : 1;
    uint8_t bufferManagement : 1;
    uint8_t acknack : 1;
    uint8_t async : 1;
    uint8_t sync : 1;
     */
    uint8_t sync : 1;
    uint8_t async : 1;
    uint8_t acknack : 1;
    uint8_t bufferManagement : 1;
    uint8_t streaming : 1;
    uint8_t reserved02 : 1;
    uint8_t comIDManagement : 1;
    uint8_t reserved01 : 1;

    uint32_t reserved03;
    uint32_t reserved04;
    uint32_t reserved05;
} Discovery0TPerFeatures;

/** Locking Feature Descriptor. Contains flags indicating support for the
 * locking features described in the OPAL specification. The names match the
 * OPAL terminology
 */
typedef struct _Discovery0LockingFeatures {
    uint16_t featureCode; /* 0x0002 in 2.00.100 */
    uint8_t reserved_v : 4;
    uint8_t version : 4;
    uint8_t length;
    /* Big endian
    uint8_t reserved01 : 1;
    uint8_t reserved02 : 1;
    uint8_t MBRDone : 1;
    uint8_t MBREnabled : 1;
    uint8_t mediaEncryption : 1;
    uint8_t locked : 1;
    uint8_t lockingEnabled : 1;
    uint8_t lockingSupported : 1;
     */
    uint8_t lockingSupported : 1;
    uint8_t lockingEnabled : 1;
    uint8_t locked : 1;
    uint8_t mediaEncryption : 1;
    uint8_t MBREnabled : 1;
    uint8_t MBRDone : 1;
    uint8_t MBRshadowingNotSupported : 1;
    uint8_t reserved02 : 1;

    uint32_t reserved03;
    uint32_t reserved04;
    uint32_t reserved05;
} Discovery0LockingFeatures;

/** Geometry Feature Descriptor. Contains flags indicating support for the
 * geometry features described in the OPAL specification. The names match the
 * OPAL terminology
 */
typedef struct _Discovery0GeometryFeatures {
    uint16_t featureCode; /* 0x0003 in 2.00.100 */
    uint8_t reserved_v : 4;
    uint8_t version : 4;
    uint8_t length;
    /* big Endian
    uint8_t reserved01 : 7;
    uint8_t align : 1;
     */
    uint8_t align : 1;
    uint8_t reserved01 : 7;
    uint8_t reserved02[7];
    uint32_t logicalBlockSize;
    uint64_t alignmentGranularity;
    uint64_t lowestAlighedLBA;
} Discovery0GeometryFeatures;

/** Enterprise SSC Feature 
 */
typedef struct _Discovery0EnterpriseSSC {
    uint16_t featureCode; /* 0x0100 */
    uint8_t reserved_v : 4;
    uint8_t version : 4;
    uint8_t length;
    uint16_t baseComID;
    uint16_t numberComIDs;
    /* big endian
    uint8_t reserved01 : 7;
    uint8_t rangeCrossing : 1;
     */
    uint8_t rangeCrossing : 1;
    uint8_t reserved01 : 7;

    uint8_t reserved02;
    uint16_t reserved03;
    uint32_t reserved04;
    uint32_t reserved05;
} Discovery0EnterpriseSSC;

/** Opal V1 feature 
 */
typedef struct _Discovery0OpalV100 {
	uint16_t featureCode; /* 0x0200 */
	uint8_t reserved_v : 4;
	uint8_t version : 4;
	uint8_t length;
	uint16_t baseComID;
	uint16_t numberComIDs;
} Discovery0OpalV100;
/** Single User Mode feature 
 */
typedef struct _Discovery0SingleUserMode {
    uint16_t featureCode; /* 0x0201 */
    uint8_t reserved_v : 4;
    uint8_t version : 4;
    uint8_t length;
    uint32_t numberLockingObjects;
    /* big endian
    uint8_t reserved01 : 5;
    uint8_t policy : 1;
    uint8_t all : 1;
    uint8_t any : 1;
     */
    uint8_t any : 1;
    uint8_t all : 1;
    uint8_t policy : 1;
    uint8_t reserved01 : 5;

    uint8_t reserved02;
    uint16_t reserved03;
    uint32_t reserved04;
} Discovery0SingleUserMode;

/** Additonal Datastores feature .
 */
typedef struct _Discovery0DatastoreTable {
    uint16_t featureCode; /* 0x0203 */
    uint8_t reserved_v : 4;
    uint8_t version : 4;
    uint8_t length;
    uint16_t reserved01;
    uint16_t maxTables;
    uint32_t maxSizeTables;
    uint32_t tableSizeAlignment;
} Discovery0DatastoreTable;

/** OPAL 2.0 feature 
 */
typedef struct _Discovery0OPALV200 {
    uint16_t featureCode; /* 0x0203 */
	uint8_t minor_v : 4; // minor version low bibble first
    uint8_t version : 4; 

    uint8_t length;
    uint16_t baseCommID;
    uint16_t numCommIDs;
    /* big endian
    uint8_t reserved01 : 7;
    uint8_t rangeCrossing : 1;
     */
    uint8_t rangeCrossing : 1;
    uint8_t reserved01 : 7;

    uint16_t numlockingAdminAuth;
    uint16_t numlockingUserAuth;
    uint8_t initialPIN;
    uint8_t revertedPIN;
    uint8_t reserved02;
    uint32_t reserved03;
} Discovery0OPALV200;
typedef struct _Discovery0PYRITE {
	uint16_t featureCode; /* 0x0302 */
	uint8_t reserved_v : 4;
	uint8_t version : 4;
	uint8_t length;
	uint16_t baseCommID;
	uint16_t numCommIDs;
	/* big endian
	uint8_t reserved01 : 7;
	uint8_t rangeCrossing : 1;
	*/
	uint8_t rangeCrossing : 1;
	uint8_t reserved01 : 7;

	uint16_t numlockingAdminAuth;
	uint16_t numlockingUserAuth;
	uint8_t initialPIN;
	uint8_t revertedPIN;
	uint8_t reserved02;
	uint32_t reserved03;
} Discovery0PYRITE;
typedef struct _Discovery0PYRITE2 {
	uint16_t featureCode; /* 0x0303 */
	uint8_t reserved_v : 4;
	uint8_t version : 4;
	uint8_t length;
	uint16_t baseCommID;
	uint16_t numCommIDs;
	/* big endian
	uint8_t reserved01 : 7;
	uint8_t rangeCrossing : 1;
	*/
	uint8_t rangeCrossing : 1;
	uint8_t reserved01 : 7;

	uint16_t numlockingAdminAuth;
	uint16_t numlockingUserAuth;
	uint8_t initialPIN;
	uint8_t revertedPIN;
	uint8_t reserved02;
	uint32_t reserved03;
} Discovery0PYRITE2;
typedef struct _Discovery0OPALITE {
	uint16_t featureCode; /* 0x0301 */
	uint8_t reserved_v : 4;
	uint8_t version : 4;
	uint8_t length;
	uint16_t baseCommID;
	uint16_t numCommIDs;
	/* big endian
	uint8_t reserved01 : 7;
	uint8_t rangeCrossing : 1;
	*/
	uint8_t rangeCrossing : 1;
	uint8_t reserved01 : 7;

	uint16_t numlockingAdminAuth;
	uint16_t numlockingUserAuth;
	uint8_t initialPIN;
	uint8_t revertedPIN;
	uint8_t reserved02;
	uint32_t reserved03;
} Discovery0OPALITE;
typedef struct _Discovery0RUBY {
	uint16_t featureCode; /* 0x0304 */
	uint8_t reserved_v : 4;
	uint8_t version : 4;
	uint8_t length;
	uint16_t baseCommID;
	uint16_t numCommIDs;
	/* big endian
	uint8_t reserved01 : 7;
	uint8_t rangeCrossing : 1;
	*/
	uint8_t rangeCrossing : 1;
	uint8_t reserved01 : 7;

	uint16_t numlockingAdminAuth;
	uint16_t numlockingUserAuth;
	uint8_t initialPIN;
	uint8_t revertedPIN;
	uint8_t reserved02;
	uint32_t reserved03;
} Discovery0RUBY;
typedef struct _Discovery0BlockSIDFeatures {
	uint16_t featureCode; /* 0x0402 in 2.00.100 */
	uint8_t reserved_v : 4;
	uint8_t version : 4;
	uint8_t length;
	uint8_t SIDvalueState : 1;
	uint8_t BlockSIDState : 1;
	uint8_t reserved07 : 6;
	uint8_t HardReset : 1;
	uint8_t reserved06 : 7;
	uint16_t reserved05;
	uint32_t reserved04;
	uint32_t reserved03;
} Discovery0BlockSIDFeatures;
//Supported Data RemovalMechanism Feature (Feature Code = 0x0404)
typedef struct _Discovery0DataRemovalMechanismFeatures {
	uint16_t featureCode; /* 0x0404 in Pyrite 2 */
	uint8_t reserved_v : 4;
	uint8_t version : 4;
	uint8_t length;
	uint8_t reserved04; 
// bit defined from LSB to MSB on intel x86 x64 
	uint8_t DataRemoval_OperationProcessing : 1;
	uint8_t reserved05 : 7;
	uint8_t DataRemoval_Mechanism; 
	uint8_t DataRemoval_TimeFormat_Bit0 : 1;
	uint8_t DataRemoval_TimeFormat_Bit1 : 1;
	uint8_t DataRemoval_TimeFormat_Bit2 : 1;
	uint8_t DataRemoval_TimeFormat_Bit3 : 1;
	uint8_t DataRemoval_TimeFormat_Bit4 : 1;
	uint8_t DataRemoval_TimeFormat_Bit5 : 1;
	uint8_t DataRemoval_TimeFormat_reserved : 2;
	uint16_t DataRemoval_Time_Bit0; // byte 8-9
	uint16_t DataRemoval_Time_Bit1;
	uint16_t DataRemoval_Time_Bit2;
	uint16_t DataRemoval_Time_Bit3;
	uint16_t DataRemoval_Time_Bit4;
	uint16_t DataRemoval_Time_Bit5; // byte 18-19
	uint8_t reserved16[16]; // byte 20 - 35
} Discovery0DataRemovalMechanismFeatures;

//=====
/** Configuable Namespace Locking features 
*/
typedef struct _Discovery0Configurable_Namespace_LockingFeature {
	uint16_t featureCode; /* 0x0403 */
	uint8_t reserved_v : 4;
	uint8_t version : 4;
	uint8_t reserved; 
	uint32_t Max_Key_Count;
	uint32_t Unused_Key_Count;
	uint32_t Max_Range_Per_NS;
} Discovery0Configurable_Namespace_LockingFeature;

// =====

/** Union of features used to parse the discovery 0 response */
union Discovery0Features {
    Discovery0TPerFeatures TPer;
    Discovery0LockingFeatures locking;
    Discovery0GeometryFeatures geometry;
    Discovery0EnterpriseSSC enterpriseSSC;
    Discovery0SingleUserMode singleUserMode;
    Discovery0OPALV200 opalv200;
	Discovery0PYRITE pyritev100;
	Discovery0PYRITE2 pyritev200;
	Discovery0OPALITE opalitev100;
	Discovery0RUBY rubyv100;
	Discovery0OpalV100 opalv100;
    Discovery0DatastoreTable datastore;
	Discovery0BlockSIDFeatures blocksidauth;
	Discovery0DataRemovalMechanismFeatures dataremoval;
	Discovery0Configurable_Namespace_LockingFeature  Configurable_Namespace_LockingFeature;
};

/** ComPacket (header) for transmissions. */

typedef struct _OPALComPacket {
    uint32_t reserved0;
    uint8_t extendedComID[4];
    uint32_t outstandingData;
    uint32_t minTransfer;
    uint32_t length;
} OPALComPacket;

/** Packet structure. */
typedef struct _OPALPacket {
    uint32_t TSN;
    uint32_t HSN;
    uint32_t seqNumber;
    uint16_t reserved0;
    uint16_t ackType;
    uint32_t acknowledgement;
    uint32_t length;
} OPALPacket;

/** Data sub packet header */
typedef struct _OPALDataSubPacket {
    uint8_t reserved0[6];
    uint16_t kind;
    uint32_t length;
} OPALDataSubPacket;
/** header of a response */
typedef struct _OPALHeader {
    OPALComPacket cp;
    OPALPacket pkt;
    OPALDataSubPacket subpkt;
} OPALHeader;
/** ATA commands needed for TCG storage communication */
typedef enum _ATACOMMAND {
    IF_RECV = 0x5c,
    IF_SEND = 0x5e,
    IDENTIFY = 0xec,
} ATACOMMAND;

typedef enum _NVMECOMMAND {
    NVME_RECV = 0x82,
    NVME_SEND = 0x81,
    NVME_IDENTIFY = 0x06,
} NVMECOMMAND;

typedef enum _DTA_DEVICE_TYPE {
    DEVICE_TYPE_ATA,
    DEVICE_TYPE_SAS,
    DEVICE_TYPE_NVME,
	DEVICE_TYPE_USB,
    DEVICE_TYPE_OTHER,
} DTA_DEVICE_TYPE;

/** structure to store Disk information. */
typedef struct _OPAL_DiskInfo {
    // parsed the Function block?
	uint8_t Unknown;
	uint8_t VendorSpecific;
    uint8_t TPer : 1;
    uint8_t Locking : 1;
    uint8_t Geometry : 1;
    uint8_t Enterprise : 1;
    uint8_t SingleUser : 1;
    uint8_t DataStore : 1;
    uint8_t OPAL20 : 1;
	uint8_t OPAL10 : 1;
	uint8_t Properties : 1;
	uint8_t ANY_OPAL_SSC : 1;
    uint8_t OPALITE : 1;
    uint8_t PYRITE : 1;
	uint8_t PYRITE2 : 1;
	uint8_t RUBY : 1;
	uint8_t BlockSID : 1;
	uint8_t DataRemoval : 1; 
	uint8_t NSLocking : 1;
	uint8_t FIPS : 1;

    // values ONLY VALID IF FUNCTION ABOVE IS TRUE!!!!!
    uint8_t TPer_ACKNACK : 1;
    uint8_t TPer_async : 1;
    uint8_t TPer_bufferMgt : 1;
    uint8_t TPer_comIDMgt : 1;
    uint8_t TPer_streaming : 1;
    uint8_t TPer_sync : 1;
    uint8_t Locking_locked : 1;
    uint8_t Locking_lockingEnabled : 1;
    uint8_t Locking_lockingSupported : 1;
	uint8_t Locking_MBRshadowingNotSupported : 1;
    uint8_t Locking_MBRDone : 1;
    uint8_t Locking_MBREnabled : 1;
    uint8_t Locking_mediaEncrypt : 1;
    uint8_t Geometry_align : 1;
    uint64_t Geometry_alignmentGranularity;
    uint32_t Geometry_logicalBlockSize;
    uint64_t Geometry_lowestAlignedLBA;
    uint8_t Enterprise_rangeCrossing : 1;
    uint16_t Enterprise_basecomID;
    uint16_t Enterprise_numcomID;
    uint8_t SingleUser_any : 1;
    uint8_t SingleUser_all : 1;
    uint8_t SingleUser_policy : 1;
    uint32_t SingleUser_lockingObjects;
    uint16_t DataStore_maxTables;
    uint32_t DataStore_maxTableSize;
    uint32_t DataStore_alignment;
	uint16_t OPAL10_basecomID;
	uint16_t OPAL10_numcomIDs;
    uint16_t OPAL20_basecomID;
    uint16_t OPAL20_numcomIDs;
    uint8_t OPAL20_initialPIN;
    uint8_t OPAL20_revertedPIN;
	uint8_t OPAL20_version;
	uint8_t OPAL20_minor_v;
    uint16_t OPAL20_numAdmins;
    uint16_t OPAL20_numUsers;
    uint8_t OPAL20_rangeCrossing;

	uint8_t OPALITE_version;
    uint16_t OPALITE_basecomID;
    uint16_t OPALITE_numcomIDs;
    uint8_t OPALITE_initialPIN;
    uint8_t OPALITE_revertedPIN;

	uint8_t PYRITE_version;
    uint16_t PYRITE_basecomID;
    uint16_t PYRITE_numcomIDs;
    uint8_t PYRITE_initialPIN;
    uint8_t PYRITE_revertedPIN;

	uint8_t PYRITE2_version;
	uint16_t PYRITE2_basecomID;
	uint16_t PYRITE2_numcomIDs;
	uint8_t PYRITE2_initialPIN;
	uint8_t PYRITE2_revertedPIN;
	//
	uint8_t RUBY_version;
	uint16_t RUBY_basecomID;
	uint16_t RUBY_numcomIDs;
	uint16_t RUBY_numAdmins;
	uint16_t RUBY_numUsers;
	uint8_t RUBY_initialPIN;
	uint8_t RUBY_revertedPIN;
	//
	uint8_t BlockSID_BlockSIDState : 1;
	uint8_t BlockSID_SIDvalueState : 1;
	uint8_t BlockSID_HardReset : 1;
	// FC 403

	uint8_t DataRemoval_version;
	uint8_t DataRemoval_OperationProcessing;
	uint8_t DataRemoval_Mechanism;
	uint8_t DataRemoval_TimeFormat_Bit5;
	uint16_t DataRemoval_Time_Bit5;
	uint8_t DataRemoval_TimeFormat_Bit4;
	uint16_t DataRemoval_Time_Bit4;
	uint8_t DataRemoval_TimeFormat_Bit3;
	uint16_t DataRemoval_Time_Bit3;
	uint8_t DataRemoval_TimeFormat_Bit2;
	uint16_t DataRemoval_Time_Bit2;
	uint8_t DataRemoval_TimeFormat_Bit1;
	uint16_t DataRemoval_Time_Bit1;
	uint8_t DataRemoval_TimeFormat_Bit0;
	uint16_t DataRemoval_Time_Bit0;
	// NSLocking 
	uint8_t NSLocking_version;
	uint8_t range_C : 1;
	uint8_t range_P : 1;
	uint8_t Max_Key_Count;
	uint8_t Unused_Key_Count;
	uint8_t Max_Range_Per_NS;


    // IDENTIFY information
    DTA_DEVICE_TYPE devType;
    uint8_t serialNum[20];
	uint8_t null0;  // make sn a cstring
    uint8_t firmwareRev[8];
	uint8_t null1;  // make firmware rev a cstring
    uint8_t modelNum[40];
	uint8_t null2;  // make model number a cstring
	uint8_t fips; // FIPS Approval mode
} OPAL_DiskInfo;
/** Response returned by ATA Identify */
typedef struct _IDENTIFY_RESPONSE {
    uint8_t reserved0;
    uint8_t reserved1 : 7;
    uint8_t devType : 1;
    uint8_t reserved2[18];
    uint8_t serialNum[20];
    uint8_t reserved3[6];
    uint8_t firmwareRev[8];
    uint8_t modelNum[40];
} IDENTIFY_RESPONSE;



////////////////////////////////////////////////////////////////////////////////
class CScsiCmdInquiry
////////////////////////////////////////////////////////////////////////////////
{
public:
    enum
    {
        OPCODE  = 0x12,
    };
    uint8_t         m_Opcode;                               //  0
    uint8_t        m_EVPD                  : 1;            //  1
    uint8_t        m_Reserved_1            : 7;
    uint8_t         m_PageCode;                             //  2
    uint16_t        m_AllocationLength;                     //  3
    uint8_t         m_Control;                              //  5
} ;                                  //  6

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#else
////////////////////////////////////////////////////////////////////////////////
class CScsiCmdInquiry_StandardData
////////////////////////////////////////////////////////////////////////////////
{
public:
    uint8_t        m_PeripheralDeviceType      : 5;        //  0
    uint8_t        m_PeripheralQualifier       : 3;
    uint8_t        m_Reserved_1                : 6;        //  1
    uint8_t        m_LUCong                    : 1;
    uint8_t        m_RMB                       : 1;
    uint8_t         m_Version;                              //  2
    uint8_t        m_ResponseDataFormat        : 4;        //  3
    uint8_t        m_HiSup                     : 1;
    uint8_t        m_NormACA                   : 1;
    uint8_t        m_Reserved_2                : 1;
    uint8_t        m_Reserved_3                : 1;
    uint8_t         m_AdditionalLength;                     //  4
    uint8_t        m_Protect                   : 1;        //  5
    uint8_t        m_Reserved_4                : 2;
    uint8_t        m_3PC                       : 1;
    uint8_t        m_TPGS                      : 2;
    uint8_t        m_ACC                       : 1;
    uint8_t        m_SCCS                      : 1;
    uint8_t        m_ADDR16                    : 1;        //  6
    uint8_t        m_Reserved_5                : 2;
    uint8_t        m_Obsolete_1                : 1;
    uint8_t        m_MultiP                    : 1;
    uint8_t        m_VS1                       : 1;
    uint8_t        m_EncServ                   : 1;
    uint8_t        m_Obsolete_2                : 1;
    uint8_t        m_VS2                       : 1;        //  7
    uint8_t        m_CmdQue                    : 1;
    uint8_t        m_Reserved_6                : 1;
    uint8_t        m_Obsolete_3                : 1;
    uint8_t        m_Sync                      : 1;
    uint8_t        m_WBus16                    : 1;
    uint8_t        m_Reserved_7                : 1;
    uint8_t        m_Obsolete_4                : 1;
    uint8_t         m_T10VendorId[8];                       //  8
    uint8_t         m_ProductId[16];                        // 16
    uint8_t         m_ProductRevisionLevel[4];              // 32
};                                  // 36
#endif

////////////////////////////////////////////////////////////////////////////////
class CScsiCmdSecurityProtocolIn
////////////////////////////////////////////////////////////////////////////////
{
public:
    enum
    {
        OPCODE  = 0XA2,
    };
    uint8_t         m_Opcode;                   //  0
    uint8_t         m_SecurityProtocol;         //  1
    uint16_t        m_SecurityProtocolSpecific; //  2
    uint8_t        m_Reserved_1    : 7;        //  4
    uint8_t        m_INC_512       : 1;
    uint8_t         m_Reserved_2;               //  5
    uint32_t        m_AllocationLength;         //  6
    uint8_t         m_Reserved_3[1];            // 10
    uint8_t         m_Control;                  // 11
};                      // 12


////////////////////////////////////////////////////////////////////////////////
class CScsiCmdSecurityProtocolOut
////////////////////////////////////////////////////////////////////////////////
{
public:
    enum
    {
        OPCODE  = 0XB5,
    };
    uint8_t         m_Opcode;                   //  0
    uint8_t         m_SecurityProtocol;         //  1
    uint16_t        m_SecurityProtocolSpecific; //  2
    uint8_t        m_Reserved_1    : 7;        //  4
    uint8_t        m_INC_512       : 1;
    uint8_t         m_Reserved_2;               //  5
    uint32_t        m_TransferLength;           //  6
    uint8_t         m_Reserved_3[1];            // 10
    uint8_t         m_Control;                  // 11
};                      // 12

#pragma pack(pop)
