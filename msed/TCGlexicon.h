/* C:B**************************************************************************
This software is Copyright 2014 Michael Romeo <r0m30@r0m30.com>

This file is part of msed.

msed is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

msed is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with msed.  If not, see <http://www.gnu.org/licenses/>.

* C:E********************************************************************** */
#pragma once
/*
 * Define the structures and enums needed to map the TCG
 * Pseudo code to procedures.
 *
 * no effort has been made to be complete, these are the ones
 * that are required for the basic functionalitly provided in this
 * program.
 */

/* ******************* BS ALERT **************************
 * VS2013 gives an error when I try and initialize these
 * as private variables in the header declaration
 * so they are defined here as static
 * ******************* BS ALERT ************************* */
//#define TCGUID_SIZE 6
//static uint8_t TCGUID[TCGUID_SIZE][8]{
static uint8_t TCGUID[][8]{
	// users
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff }, // session management
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 }, // special "thisSP" syntax
	{ 0x00, 0x00, 0x02, 0x05, 0x00, 0x00, 0x00, 0x01 }, // Administrative SP
	{ 0x00, 0x00, 0x02, 0x05, 0x00, 0x00, 0x00, 0x02 },  // Locking SP
	{ 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x01 }, //anybody
	{ 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x06 }, // SID
	{ 0x00, 0x00, 0x00, 0x09, 0x00, 0x01, 0x00, 0x01 }, // ADMIN1
	{ 0x00, 0x00, 0x00, 0x09, 0x00, 0x03, 0x00, 0x01 }, // USER1
	{ 0x00, 0x00, 0x00, 0x09, 0x00, 0x01, 0xff, 0x01 }, // PSID user
	// tables
	{ 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00 }, // AUTHORITY_TABLE
	{ 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x00 }, // C_PIN_TABLE
	//C_PIN_TABLE object ID's
	{ 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x84, 0x02 }, // C_PIN_MSID
	{ 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x01 }, // C_PIN_SID
	{ 0x00, 0x00, 0x00, 0x0B, 0x00, 0x01, 0x00, 0x01 }, // C_PIN_ADMIN1
    // special value for ommited optional parameter
    { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}, // HEXFF for omitted
};

typedef enum _TCG_UID {
    // users
    TCG_SMUID_UID,
    TCG_THISSP_UID,
    TCG_ADMINSP_UID,
	TCG_LOCKINGSP_UID,
    TCG_ANYBODY_UID,
    TCG_SID_UID,
	TCG_ADMIN1_UID,
	TCG_USER1_UID,
	TCG_PSID_UID,
    // tables
	TCG_AUTHORITY_TABLE,
	TCG_C_PIN_TABLE,
	//C_PIN_TABLE object ID's
    TCG_C_PIN_MSID,
    TCG_C_PIN_SID,
	TCG_C_PIN_ADMIN1,
    // omitted optional parameter
    TCG_UID_HEXFF,
} TCG_UID;

/** TCG Methods.
 */
static uint8_t TCGMETHOD[][8]{
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x01 }, // Properties
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x02 }, //STARTSESSION
    { 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x02, 0x02 }, // Revert
	{ 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x02, 0x03 }, // Activate
	{ 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x08 }, // NEXT
	{ 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x0d }, // GetACL
	{ 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x10 }, // GenKey
	{ 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x11 }, // revertSP
	{ 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x16 }, // Get
	{ 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x17 }, // Set
	{ 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x1c }, // Authenticate
	{ 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x06, 0x01 }, // Random
};

typedef enum _TCG_METHOD {
    PROPERTIES,
    STARTSESSION,
    REVERT,
	ACTIVATE,
	NEXT,
	GETACL,
	GENKEY,
	REVERTSP,
	GET,
	SET,
	AUTHENTICATE,
	RANDOM,
} TCG_METHOD;

/** TCG TOKENS
 * Single byte non atom tokens used in TCG psuedo code
 */
typedef enum _TCG_TOKEN {
    //Boolean
	TCG_TRUE = 0x00,
	TCG_FALSE = 0x01,
	// cellblocks
	TABLE =0x00,
    STARTROW = 0x01,
    ENDROW = 0x02,
    STARTCOLUMN = 0x03,
    ENDCOLUMN = 0x04,

    // atoms
    STARTLIST = 0xf0,
    ENDLIST = 0xf1,
    STARTNAME = 0xf2,
    ENDNAME = 0xf3,
    CALL = 0xf8,
    ENDOFDATA = 0xf9,
    ENDOFSESSION = 0xfa,
    STARTTRANSACTON = 0xfb,
    ENDTRANSACTON = 0xfC,
} TCG_TOKEN;

/** Useful tiny atoms.
 * Useful for table columns etc
 */
typedef enum _TCG_TINY_ATOM {
    UINT_00 = 0x00,
    UINT_01 = 0x01,
    UINT_02 = 0x02,
    UINT_03 = 0x03,
    UINT_04 = 0x04,
    UINT_05 = 0x05,
    UINT_06 = 0x06,
    UINT_07 = 0x07,
    UINT_08 = 0x08,
    UINT_09 = 0x09,
	UINT_10 = 0x0a,
} TCG_TINY_ATOM;

/** Useful short atoms.
 */
typedef enum _TCG_SHORT_ATOM {
    BYTESTRING8 = 0xa8,
} TCG_SHORT_ATOM;
/*
 * Structures to build and decode the TCG messages
 * fields that are NOT really numeric are defined as uint8_t[] to
 * help reduce the endianess issues
 */

/** method status codes returned in ATA command blodk.
 */
typedef enum _TCGSTATUSCODE {
    SUCCESS = 0x00,
    NOT_AUTHORIZED = 0x01,
    //	OBSOLETE = 0x02,
    SP_BUSY = 0x03,
    SP_FAILED = 0x04,
    SP_DISABLED = 0x05,
    SP_FROZEN = 0x06,
    NO_SESSIONS_AVAILABLE = 0x07,
    UNIQUENESS_CONFLICT = 0x08,
    INSUFFICIENT_SPACE = 0x09,
    INSUFFICIENT_ROWS = 0x0A,
    INVALID_PARAMETER = 0x0C,
    //	OBSOLETE = 0x0D,
    //	OBSOLETE = 0x0E,
    TPER_MALFUNCTION = 0x0F,
    TRANSACTION_FAILURE = 0x10,
    RESPONSE_OVERFLOW = 0x11,
    AUTHORITY_LOCKED_OUT = 0x12,
    FAIL = 0x3f,
} TCGSTATUSCODE;