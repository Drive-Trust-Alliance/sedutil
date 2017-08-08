/* C:B**************************************************************************
This software is Copyright 2014-2017 Bright Plaza Inc. <drivetrust@drivetrust.com>

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
    uint8_t reserved01 : 1;
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
} Discovery0OPALV200;
/** Union of features used to parse the discovery 0 response */
union Discovery0Features {
    Discovery0TPerFeatures TPer;
    Discovery0LockingFeatures locking;
    Discovery0GeometryFeatures geometry;
    Discovery0EnterpriseSSC enterpriseSSC;
    Discovery0SingleUserMode singleUserMode;
    Discovery0OPALV200 opalv200;
	Discovery0OpalV100 opalv100;
    Discovery0DatastoreTable datastore;
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
    uint16_t OPAL20_numAdmins;
    uint16_t OPAL20_numUsers;
    uint8_t OPAL20_rangeCrossing;
    // IDENTIFY information
    DTA_DEVICE_TYPE devType;
    uint8_t serialNum[20];
	uint8_t null0;  // make sn a cstring
    uint8_t firmwareRev[8];
	uint8_t null1;  // make firmware rev a cstring
    uint8_t modelNum[40];
	uint8_t null2;  // make model number a cstring
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
    unsigned        m_EVPD                  : 1;            //  1
    unsigned        m_Reserved_1            : 7;
    uint8_t         m_PageCode;                             //  2
    uint16_t        m_AllocationLength;                     //  3
    uint8_t         m_Control;                              //  5
} ;                                  //  6

////////////////////////////////////////////////////////////////////////////////
class CScsiCmdInquiry_StandardData
////////////////////////////////////////////////////////////////////////////////
{
public:
    unsigned        m_PeripheralDeviceType      : 5;        //  0
    unsigned        m_PeripheralQualifier       : 3;
    unsigned        m_Reserved_1                : 6;        //  1
    unsigned        m_LUCong                    : 1;
    unsigned        m_RMB                       : 1;
    uint8_t         m_Version;                              //  2
    unsigned        m_ResponseDataFormat        : 4;        //  3
    unsigned        m_HiSup                     : 1;
    unsigned        m_NormACA                   : 1;
    unsigned        m_Reserved_2                : 1;
    unsigned        m_Reserved_3                : 1;
    uint8_t         m_AdditionalLength;                     //  4
    unsigned        m_Protect                   : 1;        //  5
    unsigned        m_Reserved_4                : 2;
    unsigned        m_3PC                       : 1;
    unsigned        m_TPGS                      : 2;
    unsigned        m_ACC                       : 1;
    unsigned        m_SCCS                      : 1;
    unsigned        m_ADDR16                    : 1;        //  6
    unsigned        m_Reserved_5                : 2;
    unsigned        m_Obsolete_1                : 1;
    unsigned        m_MultiP                    : 1;
    unsigned        m_VS1                       : 1;
    unsigned        m_EncServ                   : 1;
    unsigned        m_Obsolete_2                : 1;
    unsigned        m_VS2                       : 1;        //  7
    unsigned        m_CmdQue                    : 1;
    unsigned        m_Reserved_6                : 1;
    unsigned        m_Obsolete_3                : 1;
    unsigned        m_Sync                      : 1;
    unsigned        m_WBus16                    : 1;
    unsigned        m_Reserved_7                : 1;
    unsigned        m_Obsolete_4                : 1;
    uint8_t         m_T10VendorId[8];                       //  8
    uint8_t         m_ProductId[16];                        // 16
    uint8_t         m_ProductRevisionLevel[4];              // 32
};                                  // 36

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
    unsigned        m_Reserved_1    : 7;        //  4
    unsigned        m_INC_512       : 1;
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
    unsigned        m_Reserved_1    : 7;        //  4
    unsigned        m_INC_512       : 1;
    uint8_t         m_Reserved_2;               //  5
    uint32_t        m_TransferLength;           //  6
    uint8_t         m_Reserved_3[1];            // 10
    uint8_t         m_Control;                  // 11
};                      // 12

#pragma pack(pop)