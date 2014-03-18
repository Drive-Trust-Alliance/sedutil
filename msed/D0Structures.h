/* C:B**************************************************************************
This software is Copyright © 2014 Michael Romeo <r0m30@r0m30.com>

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
	UINT32 length;				/**< the lenght of the header 48 in 2.00.100*/
	UINT32 revision;			/**< revision of the header 1 in 2.00.100 */
	UINT32 reserved01;
	UINT32 reserved02;
	// the remainder of the structure is vendor specific and will not be addressed now
} Discovery0Header;
/** TPer Feature Descriptor. Contains flags indicating support for the
* TPer features described in the OPAL specification. The names match the
* OPAL terminology
*/
typedef struct _Discovery0TPerFeatures {
	UINT16 featureCode;				/* 0x0001 in 2.00.100 */
	UINT8 reserved_v : 4;
	UINT8 version : 4;
	UINT8 length;
	/* big endian
	UINT8 reserved01 : 1;
	UINT8 comIDManagement : 1;
	UINT8 reserved02 : 1;
	UINT8 streaming : 1;
	UINT8 bufferManagement : 1;
	UINT8 acknack : 1;
	UINT8 Async : 1;
	UINT8 Sync : 1;
	*/
	UINT8 Sync : 1;
	UINT8 Async : 1;
	UINT8 acknack : 1;
	UINT8 bufferManagement : 1;
	UINT8 streaming : 1;
	UINT8 reserved02 : 1;
	UINT8 comIDManagement : 1;
	UINT8 reserved01 : 1;

	UINT32 reserved03;
	UINT32 reserved04;
	UINT32 reserved05;
} Discovery0TPerFeatures;
/** Locking Feature Descriptor. Contains flags indicating support for the
* locking features described in the OPAL specification. The names match the
* OPAL terminology
*/
typedef struct _Discovery0LockingFeatures {
	UINT16 featureCode;				/* 0x0002 in 2.00.100 */
	UINT8 reserved_v : 4;
	UINT8 version : 4;
	UINT8 length;
	/* Big endian
	UINT8 reserved01 : 1;
	UINT8 reserved02 : 1;
	UINT8 MBRDone : 1;
	UINT8 MBREnabled : 1;
	UINT8 mediaEncryption : 1;		
	UINT8 locked : 1;
	UINT8 lockingEnabled : 1;
	UINT8 LockingSupported : 1;
	*/
	UINT8 LockingSupported : 1;
	UINT8 lockingEnabled : 1;
	UINT8 locked : 1; 
	UINT8 mediaEncryption : 1;
	UINT8 MBREnabled : 1;
	UINT8 MBRDone : 1;
	UINT8 reserved01 : 1;
	UINT8 reserved02 : 1;

	UINT32 reserved03;
	UINT32 reserved04;
	UINT32 reserved05;
} Discovery0LockingFeatures;
/** Locking Feature Descriptor. Contains flags indicating support for the
* geometry features described in the OPAL specification. The names match the
* OPAL terminology
*/
typedef struct _Discovery0GeometryFeatures {
	UINT16 featureCode;	   /* 0x0003 in 2.00.100 */
	UINT8 reserved_v: 4;
	UINT8 version : 4;
	UINT8 length;
	/* big Endian 
	UINT8 reserved01 : 7;
	UINT8 align : 1;		/// Tied to the Locking info table somehow?? 
	*/
	UINT8 align : 1;		/// Tied to the Locking info table somehow?? 
	UINT8 reserved01 : 7;
	
	UINT8 reserved02;
	UINT16 reserved03;
	UINT32 reserved04;
	UINT32 logicalBlockSize;
	UINT32 alignmentGranularity;
	UINT32 lowestAlighedLBA;
} Discovery0GeometryFeatures;
/** Support for the Enterprise SSC Spec.
* I doubt I'll ever care, if you do you can read all
* about it in the TCG spec.
*/
typedef struct _Discovery0EnterpriseSSC {
	UINT16 featureCode;				/* 0x0100 */
	UINT8 reserved_v : 4;
	UINT8 version : 4;
	UINT8 length;
	UINT16 baseComID;
	UINT16 numberComIDs;
	/* big endian 
	UINT8 reserved01 : 7;
	UINT8 rangeCrossing : 1;
	*/
	UINT8 rangeCrossing : 1;
	UINT8 reserved01 : 7;

	UINT8 reserved02;
	UINT16 reserved03;
	UINT32 reserved04;
	UINT32 reserved05;
} Discovery0EnterpriseSSC;
/** Support for Single User Mode.
* This might be interesting but who has the time?
*/
typedef struct _Discovery0SingleUserMode {
	UINT16 featureCode;				/* 0x0201 */
	UINT8 reserved_v : 4;
	UINT8 version : 4;
	UINT8 length;
	UINT32 numberLockingObjects;
	/* big endian 
	UINT8 reserved01 : 5;
	UINT8 policy : 1;
	UINT8 all : 1;
	UINT8 any : 1;
	*/
	UINT8 any : 1;
	UINT8 all : 1;
	UINT8 policy : 1;
	UINT8 reserved01 : 5;

	UINT8 reserved02;
	UINT16 reserved03;
	UINT32 reserved04;
} Discovery0SingleUserMode;
/** Support for Additonal Datasotres.
* This is probably usefull for enterprises but I doubt
* it will be necessary for individuals
*/
typedef struct _Discovery0DatastoreTable {
	UINT16 featureCode;				/* 0x0203 */
	UINT8 reserved_v : 4;
	UINT8 version : 4;
	UINT8 length;
	UINT16 reserved01;
	UINT16 maxTables;
	UINT32 maxSizeTables;
	UINT32 tableSizeAlignment;
} Discovery0DatastoreTable;
/** Support for OPALV2.
*/
typedef struct _Discovery0OPALV200 {
	UINT16 featureCode;				/* 0x0203 */
	UINT8 reserved_v : 4;
	UINT8 version : 4;
	UINT8 length;
	UINT16 baseCommID;
	UINT16 numCommIDs;
	/* big endian 
	UINT8 reserved01 : 7;
	UINT8 rangeCrossing : 1;
	*/
	UINT8 rangeCrossing : 1;
	UINT8 reserved01 : 7;

	UINT16 numlockingAdminAuth;
	UINT16 numlockingUserAuth;
	UINT8 initialPin;
	UINT8 revertedPin;
	UINT8 reserved02;
	UINT32 reserved03;
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