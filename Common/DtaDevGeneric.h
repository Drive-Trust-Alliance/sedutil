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
class DtaCommand;
class DtaSession;
#include "os.h"
#include "DtaDev.h"
#include "DtaDevOS.h"
#include "DtaStructures.h"   

#include <vector>

using namespace std;
/** Device Class representing an unknown type of disk device.
 * This device is used in determining if a disk supports a TCG Storage SSC.
 * Most of the functions in this class are implemented to return an error as
 * it is not known if the device supports a SSC
 *
*/
class DtaDevGeneric : public DtaDevOS {
public:
    /** Constructor using an OS specific device descriptor.
     * @param devref reference to device is OS specific lexicon 
     *  */
    DtaDevGeneric(const char * devref);
    /** Default constructor */
	~DtaDevGeneric();
        /** OS specific initialization.
         * This function should perform the necessary authority and environment checking
         * to allow proper functioning of the program, open the device, perform an ATA
         * identify, add the fields from the identify response to the disk info structure
         * and if the device is an ATA device perform a call to Discovery0() to complete
         * the disk_info structure
         * @param devref character representation of the device is standard OS lexicon
         */
	 void init(const char * devref) ;
	/* sedutil.cpp */
         /** User command to prepare the device for management by sedutil. 
         * Specific to the SSC that the device supports
         * @param password the password that is to be assigned to the SSC master entities 
         */
	 uint8_t initialSetup(char * password, bool securemode) ;
	/** User command to prepare the drive for Single User Mode and rekey a SUM locking range.
         * @param lockingrange locking range number to enable
         * @param start LBA to start locking range
         * @param length length (in blocks) for locking range
         * @param Admin1Password admin1 password for TPer
         * @param password User password to set for locking range
         * @param securemode is the new password shoulb be interactively asked
         */
         uint8_t setup_SUM(uint8_t lockingrange, uint64_t start, uint64_t length, char *Admin1Password, char * password, bool securemode = false);
          /** Set the SID password.
         * Requires special handling because password is not always hashed.
         * @param oldpassword  current SID password
         * @param newpassword  value password is to be changed to
         * @param hasholdpwd  is the old password to be hashed before being added to the bytestream
         * @param hashnewpwd  is the new password to be hashed before being added to the bytestream
         * @param securemode is the new password shoulb be interactively asked
         */ 
	 uint8_t setSIDPassword(char * oldpassword, char * newpassword,
		uint8_t hasholdpwd = 1, uint8_t hashnewpwd = 1, bool securemode = false) ;
        /** Verify the SID pasword.
         * Requires special handling because password is not always hashed.
         * @param password      SID password to be tested
         * @param hashpwdd      Should the password be hashed. See comments in function Impl.
         * @param securemode    Should the password be interactively obtained.
         */
    uint8_t verifySIDPassword(char const * const password, uint8_t hashpwd, bool securemode);
         /** Set the password of a locking SP user.
          * @param password  current password
         * @param userid the userid whose password is to be changed 
         * @param newpassword  value password is to be changed to
         * @param securemode is the new password should be interactively asked
         */
	 uint8_t setPassword(char * password, char * userid, char * newpassword, bool securemode = false) ;
	 /** Set the password of a locking SP user in Single User Mode.
         * @param password  current user password
         * @param userid the userid whose password is to be changed
         * @param newpassword  value password is to be changed to
         * @param securemode is the new password should be interactively asked
         */
	 uint8_t setNewPassword_SUM(char * password, char * userid, char * newpassword, bool securemode = false) ;
          /** Loads a disk image file to the shadow MBR table.
         * @param password the password for the administrative authority with access to the table
         * @param filename the filename of the disk image
         */
	 uint8_t loadPBA(char * password, char * filename) ;
         /** Change the locking state of a locking range 
         * @param lockingrange The number of the locking range (0 = global)
         * @param lockingstate  the locking state to set
         * @param Admin1Password password of administrative authority for locking range 
         */
	 uint8_t setLockingRange(uint8_t lockingrange, uint8_t lockingstate,
		char * Admin1Password) ;
	 /** Change the locking state of a locking range in Single User Mode
         * @param lockingrange The number of the locking range (0 = global)
         * @param lockingstate  the locking state to set
         * @param password password of user authority for the locking range
         */
	 uint8_t setLockingRange_SUM(uint8_t lockingrange, uint8_t lockingstate,
		 char * password);
         /** Change the active state of a locking range 
         * @param lockingrange The number of the locking range (0 = global)
         * @param enabled  enable (true) or disable (false) the lockingrange
         * @param password password of administrative authority for locking range 
         */
	 uint8_t configureLockingRange(uint8_t lockingrange, uint8_t enabled,
		char * password) ;
	 /** Setup a locking range.  Initialize a locking range, set it's start
	 *  LBA and length, initialize it as unlocked with locking disabled.
	 *  @paran lockingrange The Locking Range to be setup
	 *  @param start  Starting LBA
	 *  @param length Number of blocks
	 *  @paran password Password of administrator
	 */
	 uint8_t setupLockingRange(uint8_t lockingrange, uint64_t start,
		 uint64_t length, char * password);
	 /** Setup a locking range in Single User Mode.  Initialize a locking range,
	 *  set it's start LBA and length, initialize it as unlocked with locking enabled.
         *  @paran lockingrange The Locking Range to be setup
         *  @param start  Starting LBA
         *  @param length Number of blocks
         *  @paran password Password of administrator
         */
	 uint8_t setupLockingRange_SUM(uint8_t lockingrange, uint64_t start,
		 uint64_t length, char * password);
         /** Primitive to set the MBRDone flag.
         * @param state 0 or 1  
         * @param Admin1Password Locking SP authority with access to flag
         */
	 /** List status of locking ranges.
	 *  @param password Password of administrator
	 */
	 uint8_t listLockingRanges(char * password, int16_t rangeid);
	 /** Generate a new encryption key for a locking range.
	 * @param lockingrange locking range number
	 * @param password password of the locking administrative authority
	 */
	 uint8_t rekeyLockingRange(uint8_t lockingrange, char * password);
	 /** Enable bands using MSID.
	 * @param lockingrange locking range number
	 */
	 uint8_t setBandsEnabled(int16_t rangeid, char * password);
	 uint8_t setMBRDone(uint8_t state, char * Admin1Password) ;
         /** Primitive to set the MBREnable flag.
         * @param state 0 or 1  
         * @param Admin1Password Locking SP authority with access to flag
         */
	 uint8_t setMBREnable(uint8_t state, char * Admin1Password) ;
         /** enable a locking sp user.  
         * @param password password of locking sp administrative authority
         * @param userid  the user to be enabled
         */
	 uint8_t enableUser(char * password, char * userid, OPAL_TOKEN status = OPAL_TOKEN::OPAL_TRUE) ;
          /** Enable locking on the device
         * @param password password of the admin sp SID authority
         */
	 uint8_t activateLockingSP(char * password) ;
	/** Enable locking on the device in Single User Mode
	 * @param lockingrange locking range to activate in SUM
	 * @param password password of the admin sp SID authority
	 */
	 uint8_t activateLockingSP_SUM(uint8_t lockingrange, char * password);
	/** Erase a Single User Mode locking range by calling the drive's erase method
         * @param lockingrange The Locking Range to erase
         * @param password The administrator password for the drive
         */
        uint8_t eraseLockingRange_SUM(uint8_t lockingrange, char * password);
        /** Change the SID password from it's MSID default 
         * @param newpassword  new password for SID and locking SP admins
         * @param securemode is the new password should be interactively asked
         */ 
	 uint8_t takeOwnership(char * newpassword, bool securemode = false) ;
         /** Reset the Locking SP to its factory default condition
         * ERASES ALL DATA!
         * @param password of Administrative user
         * @param keep true false for noerase function NOT WWORKING
         */
	 uint8_t revertLockingSP(char * password, uint8_t keep ) ;
         /** Reset the TPER to its factory condition   
         * ERASES ALL DATA!
         * @param password password of authority (SID or PSID)
         * @param PSID true or false is the authority the PSID
         *   */
	 uint8_t revertTPer(char * password, uint8_t PSID, uint8_t AdminSP ) ;
	    /** Erase a locking range
	    * @param lockingrange The number of the locking range (0 = global)
	    * @param password Password of administrative authority for locking range
	    */
	virtual uint8_t eraseLockingRange(uint8_t lockingrange, char * password);
         /** Dumps an object for diagnostic purposes
         * @param sp index into the OPALUID table for the SP the object is in
         * @param auth the authority ti use for the dump
         * @param pass the password for the suthority
         * @param objID the UID of the object to dump
         */ 
	 uint8_t objDump(char *sp, char * auth, char *pass,
		char * objID) ;
         /** Issue any command to the drive for diagnostic purposes
         * @param sp index into the OPALUID table for the SP the object is in
         * @param auth the authority ti use for the dump
         * @param pass the password for the suthority
         * @param invoker caller of the method
         * @param method the method to call
         * @param plist  the parameter list for the command
         * 
         */
	 uint8_t rawCmd(char *sp, char * auth, char *pass,
		char *invoker, char *method, char *plist) ;
	/** Read MSID
	 */
	uint8_t printDefaultPassword();
	/* DtaSession.cpp 	*/
        /** Send a command to the device and wait for the response
         * @param cmd the MswdCommand object containg the command
         * @param response the DtaResonse object containing the response
         * @param protocol The security protocol number to use for the command
         */
	 uint8_t exec(DtaCommand * cmd, DtaResponse & resp, uint8_t protocol = 1) ;
         /** return the communications ID to be used for sessions to this device */
	 uint16_t comID() ;
};
