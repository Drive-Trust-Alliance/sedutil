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
protected:
    /** Constructor using an OS specific device descriptor.
     * @param devref reference to device is OS specific lexicon
     *  */
    DtaDevGeneric(const char * devref);

private:
    /** Default destructor */
	~DtaDevGeneric();

public:
    /** OS specific initialization.
         * This function should perform the necessary authority and environment checking
         * to allow proper functioning of the program, open the device, perform an ATA
         * identify, add the fields from the identify response to the disk info structure
         * and if the device is an ATA device perform a call to Discovery0() to complete
         * the disk_info structure
         * @param devref character representation of the device is standard OS lexicon
         */
	void init(const char * devref) ;


   /** User command to prepare the device for management by sedutil.
         * Specific to the SSC that the device supports
         * @param password the password that is to be assigned to the SSC master entities
         */
	uint8_t initialSetup(char * password) ;

    /** User command to prepare the device for management by sedutil.
     * Specific to the SSC that the device supports
     * @param HostChallenge the HostChallenge that is to be assigned to the SSC master entities
     */
    uint8_t initialSetup(vector<uint8_t>HostChallenge);

    /** User command to prepare the drive for Single User Mode and rekey a SUM locking range.
         * @param lockingrange locking range number to enable
         * @param start LBA to start locking range
         * @param length length (in blocks) for locking range
         * @param Admin1Password admin1 password for TPer
         * @param password User password to set for locking range
         */
    uint8_t setup_SUM(uint8_t lockingrange, uint64_t start, uint64_t length, char *Admin1Password, char * password);

    /** Set the SID password.
         * Requires special handling because password is not always hashed.
         * @param oldpassword  current SID password
         * @param newpassword  value password is to be changed to
         * @param hasholdpwd  is the old password to be hashed before being added to the bytestream
         * @param hashnewpwd  is the new password to be hashed before being added to the bytestream
         */
	uint8_t setSIDPassword(char * oldpassword, char * newpassword,
                           uint8_t hasholdpwd = 1, uint8_t hashnewpwd = 1) ;

    /** Set the SID host challenge.
     * @param oldHostChallenge  current SID host challenge
     * @param newHostChallenge  value host challenge is to be changed to
     * @note neither value is hashed
     */
    uint8_t setSIDHostChallenge(vector<uint8_t> oldHostChallenge,
                                vector<uint8_t> newHostChallenge);

    /** Set the password of a locking SP user.
         * @param password  current password
         * @param userid the userid whose password is to be changed
         * @param newpassword  value password is to be changed to
         */
    uint8_t setPassword(char * password, char * userid, char * newpassword) ;

    /** Set the host challenge of a locking SP user.
         * @param oldHostChallenge  current host challenge
         * @param userid the userid whose host challenge  is to be changed
         * @param newHostChallenge  value host challenge  is to be changed to
         */
    uint8_t setHostChallenge(vector<uint8_t> oldHostChallenge, char * userid,
                             vector<uint8_t> newHostChallenge);

    /** Set the password of a locking SP user in Single User Mode.
         * @param password  current user password
         * @param userid the userid whose password is to be changed
         * @param newpassword  value password is to be changed to
         */
	uint8_t setNewPassword_SUM(char * password, char * userid, char * newpassword) ;
    uint8_t activate(char * password);
    uint8_t getmfgstate(void);
    uint8_t getMBRsize(char * password);

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
	uint8_t setLockingRange(uint8_t lockingrange, uint8_t lockingstate, char * Admin1Password) ;
    /** User command to manipulate the state of a locking range.
     * RW|RO|LK are the supported states @see OPAL_LOCKINGSTATE
     * @param lockingrange locking range number
     * @param lockingstate desired locking state (see above)
     * @param Admin1HostChallenge  host challenge -- unsalted password of the locking administrative authority
     */
    uint8_t setLockingRange(uint8_t lockingrange, uint8_t lockingstate, vector<uint8_t> Admin1HostChallenge);

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

    /** User command to enable/disable a locking range.
     * RW|RO|LK are the supported states @see OPAL_LOCKINGSTATE
     * @param lockingrange locking range number
     * @param enabled boolean true = enabled, false = disabled
     * @param HostChallenge HostChallenge of the locking administrative authority
     */
uint8_t configureLockingRange(uint8_t lockingrange, uint8_t enabled, vector<uint8_t> HostChallenge);

    /** Setup a locking range.  Initialize a locking range, set its start
         *  LBA and length, initialize it as unlocked with locking disabled.
	     *  @paran lockingrange The Locking Range to be setup
	     *  @param start  Starting LBA
	     *  @param length Number of blocks
	     *  @paran password Password of administrator
	     */
	uint8_t setupLockingRange(uint8_t lockingrange, uint64_t start,
                              uint64_t length, char * password);

	/** Setup a locking range in Single User Mode.  Initialize a locking range,
         *  set its start LBA and length, initialize it as unlocked with locking enabled.
         *  @paran lockingrange The Locking Range to be setup
         *  @param start  Starting LBA
         *  @param length Number of blocks
         *  @paran password Password of administrator
         */
	 uint8_t setupLockingRange_SUM(uint8_t lockingrange, uint64_t start,
                                   uint64_t length, char * password);

    /** List status of locking ranges.
	     *  @param password Password of administrator
	     */
	 uint8_t listLockingRanges(char * password, int16_t rangeid);

    /** Generate a new encryption key for a locking range.
         * @param lockingrange locking range number
         * @param password password of the locking administrative authority
         */
	uint8_t rekeyLockingRange(uint8_t lockingrange, char * password);

    /** Enable bands using password, or MSID if password is NULL (usual case).
        * @param rangeid locking range number
        * @param password password or NULL to use MSID
	 */
    uint8_t setBandsEnabled(int16_t rangeid, char * password);

    /** Primitive to set the MBRDone flag.
         * @param state 0 or 1
         * @param Admin1Password password of the locking sp administrative authority
         */
    uint8_t setMBRDone(uint8_t state, char * Admin1Password) ;

    /** Primitive to set the MBRDone flag.
         * @param state 0 or 1
         * @param Admin1HostChallenge host challenge of the locking sp administrative authority
         */
    uint8_t setMBRDone(uint8_t state, vector<uint8_t> Admin1HostChallenge) ;



    uint8_t TCGreset(uint8_t state);



    /** Primitive to set the MBREnable flag.
         * @param state 0 or 1
         * @param Admin1Password Locking SP authority with access to flag
         */
    uint8_t setMBREnable(uint8_t state, char * Admin1Password) ;

    /** Primitive to set the MBREnable flag.
     * @param state 0 or 1
     * @param Admin1HostChallenge  host challenge -- unsalted password of the locking administrative authority
     */
    uint8_t setMBREnable(uint8_t state, vector<uint8_t> Admin1HostChallenge);


//    /** enable a locking sp user.
//         * @param password password of locking sp administrative authority
//         * @param userid  the user to be enabled
//         */
//    uint8_t enableUser(char * password, char * userid, OPAL_TOKEN status = OPAL_TOKEN::OPAL_TRUE) ;
    uint8_t enableUser(uint8_t state, char * password, char * userid) ;
    uint8_t enableUserRead(uint8_t state, char * password, char * userid);

    /** enable a locking sp user.
     * @param state 0 or 1
     * @param HostChallenge HostChallenge of locking sp administrative authority
     * @param userid  the user to be enabled
     */
    uint8_t enableUser(uint8_t state, vector<uint8_t> HostChallenge, char * userid);

    /** Enable locking on the device
     * @param state 0 or 1
     * @param HostChallenge HostChallenge of the admin sp SID authority
     */
    uint8_t enableUserRead(uint8_t state, vector<uint8_t> HostChallenge, char * userid);

    /** Enable locking on the device
         * @param password password of the admin sp SID authority
         */
     uint8_t activateLockingSP(char * password) ;

    /** Enable locking on the device
         * @param HostChallenge HostChallenge of the admin sp SID authority
         */
     uint8_t activateLockingSP(vector<uint8_t>HostChallenge);

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

    /** Change the SID password from its MSID default
         * @param newpassword  new password for SID and locking SP admins
         */
    uint8_t takeOwnership(char * newpassword) ;

    /** Change the SID HostChallenge from its MSID default
         * @param HostChallenge  new HostChallenge for SID and locking SP admins
         */
    uint8_t takeOwnership(vector<uint8_t> HostChallenge);

    /** Reset the Locking SP to its factory default condition
         * ERASES ALL DATA!
         * @param password of Administrative user
         * @param keep true false for noerase function NOT WWORKING
         */
	 uint8_t revertLockingSP(char * password, uint8_t keep ) ;

    /** Reset the Locking SP to its factory default condition
     * ERASES ALL DATA!
     * @param HostChallenge of Administrative user
     * @param keep true false for noerase function NOT WWORKING
     */
    uint8_t revertLockingSP(vector<uint8_t> HostChallenge, uint8_t keep = 0);


    /** Reset the TPER to its factory condition
         * ERASES ALL DATA!
         * @param password password of authority (SID or PSID)
         * @param PSID true or false is the authority the PSID
         *   */
	uint8_t revertTPer(char * password, uint8_t PSID, uint8_t AdminSP ) ;

    /** Reset the TPER to its factory condition
     * @param HostChallenge HostChallenge of authority (SID or PSID)
     * @param PSID true or false is the authority the PSID
     * @param AdminSP true or false is the SP the AdminSP or ThisSP (Enterprise Only)
     */
    uint8_t revertTPer(vector<uint8_t> HostChallenge, uint8_t PSID = 0, uint8_t AdminSP = 0 );


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
	uint8_t objDump(char *sp, char * auth, char *pass, char * objID) ;

    /** Issue any command to the drive for diagnostic purposes
         * @param sp index into the OPALUID table for the SP the object is in
         * @param auth the authority ti use for the dump
         * @param pass the password for the suthority
         * @param invoker caller of the method
         * @param method the method to call
         * @param plist  the parameter list for the command
         *
         */
    uint8_t rawCmd(char *sp, char * auth, char *pass, char *invoker, char *method, char *plist) ;


    /** Primitive to extract the MSID into a std::string
     * @param MSID the string to receive the MSID
     */
    uint8_t getMSID(string& MSID);

    /** Read MSID
	 */
	uint8_t printDefaultPassword();


    /** Send a command to the device and wait for the response
         * @param cmd the MswdCommand object containg the command
         * @param resp the DtaResonse object containing the response
         * @param protocol The security protocol number to use for the command
         */
	 uint8_t exec(DtaCommand * cmd, DtaResponse & resp, uint8_t protocol = 1) ;

  /** return the communications ID to be used for sessions to this device */
	 uint16_t comID() ;
};
