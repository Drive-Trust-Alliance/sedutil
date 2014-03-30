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
	uint32_t length;				/**< the lenght of the header 48 in 2.00.100*/
	uint32_t revision;			/**< revision of the header 1 in 2.00.100 */
	uint32_t reserved01;
	uint32_t reserved02;
	// the remainder of the structure is vendor specific and will not be addressed now
} Discovery0Header;
/** TPer Feature Descriptor. Contains flags indicating support for the
* TPer features described in the OPAL specification. The names match the
* OPAL terminology
*/
typedef struct _Discovery0TPerFeatures {
	uint16_t featureCode;				/* 0x0001 in 2.00.100 */
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
	uint16_t featureCode;				/* 0x0002 in 2.00.100 */
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
	uint16_t featureCode;	   /* 0x0003 in 2.00.100 */
	uint8_t reserved_v: 4;
	uint8_t version : 4;
	uint8_t length;
	/* big Endian 
	uint8_t reserved01 : 7;
	uint8_t align : 1;		/// Tied to the Locking info table somehow?? 
	*/
	uint8_t align : 1;		/// Tied to the Locking info table somehow?? 
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
	uint16_t featureCode;				/* 0x0100 */
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
	uint16_t featureCode;				/* 0x0201 */
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
	uint16_t featureCode;				/* 0x0203 */
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
	uint16_t featureCode;				/* 0x0203 */
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
union Discovery0Features{
	Discovery0TPerFeatures TPer;
	Discovery0LockingFeatures Locking;
	Discovery0GeometryFeatures Geometry;
	Discovery0EnterpriseSSC EnterpriseSSC;
	Discovery0SingleUserMode SingleUserMode;
	Discovery0OPALV200 OPALv200;
	Discovery0DatastoreTable Datastore;
};
#pragma pack(pop)