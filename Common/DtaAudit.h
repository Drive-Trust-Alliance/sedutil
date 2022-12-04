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

#include "DtaDev.h"
#include "DtaDevOS.h"
#include "DtaStructures.h"
#include "DtaLexicon.h"
#include "DtaResponse.h"
#include <vector>
#include <wchar.h>

using namespace std;

#define MAX_ENTRY_NUM 1000
#define ERRCHKSUM 1
#define SIGNATURE "Fidelity Lock Signature"

/*
Event IDList:
1.	Activate
2.	AdminLogin
3.	User Login
4.	RevertSP
5.	Revert
6.	Revert PSID
7.	GenKey
8.	Cryptographic Erase
9.	A large number of failed authentication attempts ()???
10.	Password Changed for SID
11.	Password changedfor Admins
12.	Password changed for Users
*/
typedef enum _evt{
	evt_Activate = 1,
	evt_AdminLogin,
	evt_UserLogin,
	evt_RevertSP,
	evt_Revert,
	evt_RevertPSID,
	evt_GenKey,
	evt_CryptographicErase,
	evt_AuthFail,
	evt_PasswordChangedSID,
	evt_PasswordChangedAdmin,
	evt_PasswordChangedUser,
	evt_SetLockingRange,
	evt_lastID,
}evt;

typedef struct _entry_t {
	uint8_t event;
	uint8_t yy;
	uint8_t mm;
	uint8_t dd;
	uint8_t hh;
	uint8_t min;
	uint8_t sec;
	uint8_t reserved;
	//uint16_t reserved;
} entry_t;

typedef struct _audit_hdr {
	char hdr[32] = "Fidelity Lock Audit Log";
	uint8_t ver_major = 1;
	uint8_t ver_minor = 0;
	entry_t date_created;
	uint16_t head = 0; // 0 to 1000
	uint16_t tail = 0; // 0 to 1000
	uint16_t num_entry = 0;
	char signature[32] = "Fidelity Lock Signature"; // password + salt + entry
	uint16_t chksum;
} audit_hdr;

typedef struct _audit_t
{
	audit_hdr header;
	uint8_t buffer[MAX_ENTRY_NUM * 8];
}audit_t;


//audit_t auditL;
//audit_t * auditP;
//vector <entry_t> entryA;

//uint8_t auditlogwr(uint8_t ID); // add event ID and write audit log to Data Store

uint16_t genchksum(char * buffer);
uint16_t getchksum(char * buffer);
void wrtchksum(char * buffer, uint16_t sum);

//audit_t * init();
//uint8_t auditlogrd(uint8_t * buffer); // read audit log to Data Store
uint16_t gethdrsize();
//audit_t * getauditPtr();
//uint16_t getnumentry();
//uint8_t auditRec(char * password);

/*
class DtaAudit { //public DtaDevOS {
	audit_t auditL;
	audit_t * auditP;
	vector <entry_t> entryA;
public:
	DtaAudit();
	~DtaAudit();
	uint8_t auditlog(uint8_t ID); // add event ID and write audit log to Data Store

	audit_t * init();
	uint8_t auditlog(uint8_t * buffer); // read audit log to Data Store
	uint16_t gethdrsize();
	audit_t * getauditPtr();
	uint16_t getnumentry();
};
*/

