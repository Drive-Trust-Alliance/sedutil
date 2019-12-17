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
#include "DtaStructures.h"
#include "DtaLexicon.h"
#include <vector>
#include "DtaOptions.h"
#include "DtaResponse.h"
class DtaCommand;
class DtaSession;

using namespace std;
/** Base class for a disk device.
 * This is a virtual base class defining the minimum functionality of device
 * object.  The methods defined here are called by other parts of the program 
 * so must be present in all devices
 */
class DtaDev {
public:
	/** Default constructor, does nothing */
	DtaDev();
	/** Default destructor, does nothing*/
	virtual ~DtaDev();
	/** Does the device conform to the Pyrite 2.0 SSC */
	uint8_t isPyrite2();
	/** Does the device conform to the Pyrite 1.0 SSC */
	uint8_t isPyrite1();
	/** Does the device conform to the Opalite SSC */
	uint8_t isOpalite();
	/** Does the device conform to the OPAL 2.0 SSC */
	uint8_t isOpal2();
	/** Does the device conform to the OPAL 1.0 SSC */
	uint8_t isOpal1();
	/** Does the device conform to the OPAL Enterprise SSC */
	uint8_t isEprise();
	/** Does the device conform to ANY TCG storage SSC */
	uint8_t isAnySSC();
	/** Is the MBREnabled flag set */
	uint8_t MBREnabled();
	/** Is the MBRDone flag set */
	uint8_t MBRDone();
	/** Is the MBRAbsent flag set */
	uint8_t MBRAbsent();
	/** Is the Locked flag set */
	uint8_t Locked();
	/** Is the Locking SP enabled */
	uint8_t LockingEnabled();
	/** Is there an OS disk represented by this object */
	uint8_t isPresent();
	/** Returns the Firmware revision reported by the identify command */
	char *getFirmwareRev();
	/** Returns the Model Number reported by the Identify command */
	char *getModelNum();
	/** Returns the Serial Number reported by the Identify command */
	char *getSerialNum();
	/* What type of disk attachment is used */
	DTA_DEVICE_TYPE getDevType();
	/** displays the information returned by the Discovery 0 reply */
	virtual void puke();

	/** Decode the Discovery 0 response. Scans the D0 response and creates a structure
	 * that can be queried later as required.This code also takes care of
	 * the endianess conversions either via a bitswap in the structure or executing
	 * a macro when the input buffer is read.
	 */
	void discovery0();

	/*
	 * virtual methods required in the OS specific
	 * device class
	 */
	/** OS specific initialization.
	 * This function should perform the necessary authority and environment checking
	 * to allow proper functioning of the program, open the device, perform an ATA
	 * identify, add the fields from the identify response to the disk info structure
	 * and if the device is an ATA device perform a call to Discovery0() to complete
	 * the disk_info structure
	 * @param devref character representation of the device is standard OS lexicon
	 */
	virtual void init(const char * devref) = 0;
	/** OS specific method to send an ATA command to the device
	 * @param cmd ATA command to be sent to the device
	 * @param protocol security protocol to be used in the command
	 * @param comID communications ID to be used
	 * @param buffer input/output buffer
	 * @param bufferlen length of the input/output buffer
	 */
	virtual uint8_t sendCmd(ATACOMMAND cmd, uint8_t protocol, uint16_t comID,
		void * buffer, uint32_t bufferlen) = 0;
	/** OS specific command to Wait for specified number of milliseconds
	 * @param milliseconds  number of milliseconds to wait
	 */
	virtual void osmsSleep(uint32_t milliseconds) = 0;
	/** OS specific routine to send an ATA identify to the device */
	virtual void identify(OPAL_DiskInfo& disk_info) = 0;
	/** OS specific routine to get size of the device */
	virtual unsigned long long getSize() = 0;
	/*
	 * virtual functions required to be implemented
	 * because they are called by sedutil.cpp
	 */
	/** User command to prepare the device for management by sedutil.
	 * Specific to the SSC that the device supports
	 * @param password the password that is to be assigned to the SSC master entities
	 */
	virtual uint8_t initialSetup(char * password) = 0;
	/** User command to prepare the drive for Single User Mode and rekey a SUM locking range.
	 * @param lockingrange locking range number to enable
	 * @param start LBA to start locking range
	 * @param length length (in blocks) for locking range
	 * @param Admin1Password admin1 password for TPer
	 * @param password User password to set for locking range
	 */
	virtual uint8_t setup_SUM(uint8_t lockingrange, uint64_t start, uint64_t length, char *Admin1Password, char * password) = 0;
	/** Set the SID password.
	 * Requires special handling because password is not always hashed.
	 * @param oldpassword  current SID password
	 * @param newpassword  value password is to be changed to
	 * @param hasholdpwd  is the old password to be hashed before being added to the bytestream
	 * @param hashnewpwd  is the new password to be hashed before being added to the bytestream
	 */
	virtual uint8_t setSIDPassword(char * oldpassword, char * newpassword,
		uint8_t hasholdpwd = 1, uint8_t hashnewpwd = 1) = 0;
	/** Set the password of a locking SP user.
	 * @param password  current password
	 * @param userid the userid whose password is to be changed
	 * @param newpassword  value password is to be changed to
	 */
	virtual uint8_t setPassword(char * password, char * userid, char * newpassword) = 0;
	/** Set the password of a locking SP user in Single User Mode.
         * @param password  current user password
         * @param userid the userid whose password is to be changed
         * @param newpassword  value password is to be changed to
         */
	virtual uint8_t setNewPassword_SUM(char * password, char * userid, char * newpassword) = 0;
	/** Loads a disk image file to the shadow MBR table.
	 * @param password the password for the administrative authority with access to the table
	 * @param filename the filename of the disk image
	 */
	virtual uint8_t loadPBA(char * password, char * filename) = 0;
	/** Change the locking state of a locking range
	 * @param lockingrange The number of the locking range (0 = global)
	 * @param lockingstate  the locking state to set
	 * @param Admin1Password password of administrative authority for locking range
	 */
	virtual uint8_t setLockingRange(uint8_t lockingrange, uint8_t lockingstate,
		char * Admin1Password) = 0;
	/** Change the locking state of a locking range in Single User Mode
         * @param lockingrange The number of the locking range (0 = global)
         * @param lockingstate  the locking state to set
         * @param password password of user authority for the locking range
         */
	virtual uint8_t setLockingRange_SUM(uint8_t lockingrange, uint8_t lockingstate,
		char * password) = 0;
	/** Change the active state of a locking range
	 * @param lockingrange The number of the locking range (0 = global)
	 * @param enabled  enable (true) or disable (false) the lockingrange
	 * @param password Password of administrative authority for locking range
	 */
	virtual uint8_t configureLockingRange(uint8_t lockingrange, uint8_t enabled,
		char * password) = 0;
	/** Setup a locking range.  Initialize a locking range, set it's start
	 *  LBA and length, initialize it as unlocked with locking disabled.
	 *  @paran lockingrange The Locking Range to be setup
	 *  @param start  Starting LBA
	 *  @param length Number of blocks
	 *  @param password Password of administrator
	 */
	virtual uint8_t setupLockingRange(uint8_t lockingrange, uint64_t start,
		uint64_t length, char * password) = 0;
	/** Setup a locking range in Single User Mode.  Initialize a locking range,
	 *  set it's start LBA and length, initialize it as unlocked with locking enabled.
         *  @paran lockingrange The Locking Range to be setup
         *  @param start  Starting LBA
         *  @param length Number of blocks
         *  @param password Password of administrator
         */
	virtual uint8_t setupLockingRange_SUM(uint8_t lockingrange, uint64_t start,
		uint64_t length, char * password) = 0;
	/** List status of locking ranges.  
	*  @param password Password of administrator
	*/
	virtual uint8_t listLockingRanges(char * password, int16_t rangeid) = 0;
	/** Generate a new encryption key for a locking range.
	* @param lockingrange locking range number
	* @param password password of the locking administrative authority
	*/
	virtual uint8_t rekeyLockingRange(uint8_t lockingrange, char * password) = 0;
	/** Enable bands using MSID.
	* @param lockingrange locking range number
	*/
	virtual uint8_t setBandsEnabled(int16_t rangeid, char * password) = 0;
	/** Primitive to set the MBRDone flag.
	 * @param state 0 or 1
	 * @param Admin1Password Locking SP authority with access to flag
	 */
	virtual uint8_t setMBRDone(uint8_t state, char * Admin1Password) = 0;
	/** Primitive to set the MBREnable flag.
	 * @param state 0 or 1
	 * @param Admin1Password Locking SP authority with access to flag
	 */
	virtual uint8_t setMBREnable(uint8_t state, char * Admin1Password) = 0;
	/** enable a locking sp user.
	 * @param password password of locking sp administrative authority
	 * @param userid  the user to be enabled
	 */
	virtual uint8_t enableUser(char * password, char * userid, OPAL_TOKEN status = OPAL_TOKEN::OPAL_TRUE) = 0;
	/** Enable locking on the device
	 * @param password password of the admin sp SID authority
	 */
	virtual uint8_t activateLockingSP(char * password) = 0;
	/** Enable locking on the device in Single User Mode
	* @param lockingrange the locking range number to activate in SUM
	* @param password password of the admin sp SID authority
	*/
	virtual uint8_t activateLockingSP_SUM(uint8_t lockingrange, char * password) = 0;
	/** Erase a Single User Mode locking range by calling the drive's erase method
	 * @param lockingrange The Locking Range to erase
	 * @param password The administrator password for the drive
	 */
	virtual uint8_t eraseLockingRange_SUM(uint8_t lockingrange, char * password) = 0;
	/** Change the SID password from it's MSID default
	 * @param newpassword  new password for SID and locking SP admins
	 */
	virtual uint8_t takeOwnership(char * newpassword) = 0;
	/** Reset the Locking SP to its factory default condition
	 * ERASES ALL DATA!
	 * @param password of Administrative user
	 * @param keep true false for noerase function NOT WWORKING
	 */
	virtual uint8_t revertLockingSP(char * password, uint8_t keep = 0) = 0;
	/** Reset the TPER to its factory condition
	 * ERASES ALL DATA!
	 * @param password password of authority (SID or PSID)
	 * @param PSID true or false is the authority the PSID
	 *   */
	virtual uint8_t revertTPer(char * password, uint8_t PSID = 0, uint8_t AdminSP = 0 ) = 0;
	/** Erase a locking range
	 * @param lockingrange The number of the locking range (0 = global)
	 * @param password Password of administrative authority for locking range
	 */
	virtual uint8_t eraseLockingRange(uint8_t lockingrange, char * password) = 0;
	/** Dumps an object for diagnostic purposes
	 * @param sp index into the OPALUID table for the SP the object is in
	 * @param auth the authority to use for the dump
	 * @param pass the password for the authority
	 * @param objID the UID of the object to dump
	 *  */
	virtual uint8_t objDump(char *sp, char * auth, char *pass,
		char * objID) = 0;
	/** Issue any command to the drive for diagnostic purposes
	 * @param sp index into the OPALUID table for the SP the object is in
	 * @param auth the authority to use for the dump
	 * @param pass the password for the authority
	 * @param invoker caller of the method
	 * @param method the method to call
	 * @param plist  the parameter list for the command
	 *
	 */
	virtual uint8_t rawCmd(char *sp, char * auth, char *pass,
		char *invoker, char *method, char *plist) = 0;
	/** Read MSID
	 */
	virtual uint8_t printDefaultPassword() = 0;
	/*
	* virtual functions required to be implemented
	* because they are called by DtaSession.cpp
	*/
	/** Send a command to the device and wait for the response
	 * @param cmd the MswdCommand object containing the command
	 * @param response the DtaResonse object containing the response
	 * @param protocol The security protocol number to use for the command
	 */
	virtual uint8_t exec(DtaCommand * cmd, DtaResponse & resp, uint8_t protocol = 0x01) = 0;
	/** return the communications ID to be used for sessions to this device */
	virtual uint16_t comID() = 0;
	bool no_hash_passwords; /** disables hashing of passwords */
	sedutiloutput output_format; /** standard, readable, JSON */
protected:
	const char * dev;   /**< character string representing the device in the OS lexicon */
	uint8_t isOpen = FALSE;  /**< The device has been opened */
	OPAL_DiskInfo disk_info;  /**< Structure containing info from identify and discovery 0 */
	DtaResponse response;   /**< shared response object */
	DtaResponse propertiesResponse;  /**< response fron properties exchange */
	DtaSession *session;  /**< shared session object pointer */
	uint8_t discovery0buffer[MIN_BUFFER_LENGTH + IO_BUFFER_ALIGNMENT];
	uint32_t tperMaxPacket = 2048;
	uint32_t tperMaxToken = 1950;
};
