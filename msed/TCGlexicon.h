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
	// tables
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
    // tables
    TCG_C_PIN_MSID_TABLE,
    TCG_C_PIN_SID_TABLE,
	TCG_C_PIN_ADMIN1_TABLE,
    // omitted optional parameter
    TCG_UID_HEXFF,
} TCG_UID;

/** TCG Methods.
 * Links to TCGMETHOD in TCGCommand
 */
//#define TCGMETHOD_SIZE 3
static uint8_t TCGMETHOD[][8]{
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x01 }, // Properties
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x02 }, //STARTSESSION
    { 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x02, 0x02 }, // Revert
	{ 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x02, 0x03 }, // Activate
	{ 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x11 }, // revertSP
	{ 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x16 }, // Get
	{ 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x17 }, // Set
};

typedef enum _TCG_METHOD {
    PROPERTIES,
    STARTSESSION,
    REVERT,
	ACTIVATE,
	REVERTSP,
	GET,
	SET,
} TCG_METHOD;

/** TCG TOKENS
 * Single byte non atom tokens used in TCG psuedo code
 */
typedef enum _TCG_TOKEN {
    // cellblocks
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