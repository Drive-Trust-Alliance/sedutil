/* C:B**************************************************************************
This software is Copyright � 2014 Michael Romeo <r0m30@r0m30.com>

THIS SOFTWARE IS PROVIDED BY THE AUTHORS ''AS IS'' AND ANY EXPRESS
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 * C:E********************************************************************** */
#pragma once
#pragma pack(push)
#pragma pack(1)
/** The Discovery 0 Header. As defined in
 * TCG Documentation
 */

#define FC_TPER		  0x0001
#define FC_LOCKING    0x0002
#define FC_GEOMETRY   0x0003
#define FC_ENTERPRISE 0x0100
#define FC_DATASTORE  0x0202
#define FC_SINGLEUSER 0x0201
#define FC_OPALV200   0x0203

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
    uint8_t Async : 1;
    uint8_t Sync : 1;
     */
    uint8_t Sync : 1;
    uint8_t Async : 1;
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
    uint8_t LockingSupported : 1;
     */
    uint8_t LockingSupported : 1;
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

/** Locking Feature Descriptor. Contains flags indicating support for the
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
    uint8_t align : 1;		// Tied to the Locking info table somehow??
     */
    uint8_t align : 1; /// Tied to the Locking info table somehow??
    uint8_t reserved01 : 7;

    uint8_t reserved02;
    uint16_t reserved03;
    uint32_t reserved04;
    uint32_t logicalBlockSize;
    uint32_t alignmentGranularity;
    uint32_t lowestAlighedLBA;
} Discovery0GeometryFeatures;

/** Support for the Enterprise SSC Spec.
 * I doubt I'll ever care, if you do you can read all
 * about it in the TCG spec.
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

/** Support for Single User Mode.
 * This might be interesting but who has the time?
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

/** Support for Additonal Datasotres.
 * This is probably usefull for enterprises but I doubt
 * it will be necessary for individuals
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

/** Support for OPALV2.
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
    uint8_t initialPin;
    uint8_t revertedPin;
    uint8_t reserved02;
    uint32_t reserved03;
} Discovery0OPALV200;

union Discovery0Features {
    Discovery0TPerFeatures TPer;
    Discovery0LockingFeatures Locking;
    Discovery0GeometryFeatures Geometry;
    Discovery0EnterpriseSSC EnterpriseSSC;
    Discovery0SingleUserMode SingleUserMode;
    Discovery0OPALV200 OPALv200;
    Discovery0DatastoreTable Datastore;
};

/** Defines the ComPacket (header) for TCG transmissions. */

typedef struct _TCGComPacket {
    uint32_t reserved0;
    uint8_t ExtendedComID[4];
    uint32_t OutstandingData;
    uint32_t MinTransfer;
    uint32_t Length;
} TCGComPacket;

/** Defines the TCG Packet structure. */
typedef struct _TCGPacket {
    uint32_t TSN;
    uint32_t HSN;
    uint32_t SeqNumber;
    uint16_t reserved0;
    uint16_t AckType;
    uint32_t Aknowledgement;
    uint32_t Length;
} TCGPacket;

/** Define the TCG Data sub header. */
typedef struct _TCGDataSubPacket {
    uint8_t reserved0[6];
    uint16_t Kind;
    uint32_t Length;
} TCGDataSubPacket;

typedef struct _TCGHeader {
    TCGComPacket cp;
    TCGPacket pkt;
    TCGDataSubPacket subpkt;
} TCGHeader;


typedef enum _ATACOMMAND{
	IF_RECV = 0x5c,
	IF_SEND = 0x5e
} ATACOMMAND;

/** structure to store D0 information. */
typedef struct _TCG_DiskInfo {
	// parsed the Function block?
	uint8_t TPer : 1;
	uint8_t Locking : 1;
	uint8_t Geometry : 1;
	uint8_t Enterprise : 1;
	uint8_t SingleUser : 1;
	uint8_t DataStore : 1;
	uint8_t OPAL20 : 1;
	uint8_t Unknown;
	// values ONLY VALID IF FUNCTION ABOVE IS TRUE!!!!!
	uint8_t TPer_ACKNACK : 1;
	uint8_t TPer_ASYNC : 1;
	uint8_t TPer_BufferMgt : 1;
	uint8_t TPer_comIDMgt : 1;
	uint8_t TPer_Streaming : 1;
	uint8_t TPer_SYNC : 1;
	uint8_t Locking_Locked : 1;
	uint8_t Locking_LockingEnabled : 1;
	uint8_t Locking_LockingSupported : 1;
	uint8_t Locking_MBRDone : 1;
	uint8_t Locking_MBREnabled : 1;
	uint8_t Locking_MediaEncrypt : 1;
	uint8_t Geometry_Align : 1;
	uint32_t Geometry_AlignmentGranularity;
	uint32_t Geometry_LogicalBlockSize;
	uint32_t Geometry_LowestAlignedLBA;
	uint8_t Enterprise_RangeCrossing : 1;
	uint16_t Enterprise_BasecomID;
	uint16_t Enterprise_NumcomID;
	uint8_t SingleUser_ANY : 1;
	uint8_t SingleUser_ALL : 1;
	uint8_t SingleUser_Policy : 1;
	uint32_t SingleUser_LockingObjects;
	uint16_t DataStore_MaxTables;
	uint32_t DataStore_MaxTableSize;
	uint32_t DataStore_Alignment;
	uint16_t OPAL20_BasecomID;
	uint16_t OPAL20_NumcomIDs;
	uint8_t OPAL20_InitialPIN;
	uint8_t OPAL20_RevertedPIN;
	uint16_t OPAL20_NumAdmins;
	uint16_t OPAL20_NumUsers;
	uint8_t OPAL20_RangeCrossing;
} TCG_DiskInfo;


#pragma pack(pop)