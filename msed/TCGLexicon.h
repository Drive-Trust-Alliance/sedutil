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
/*
 * Define the structures and enums needed to map the TCG
 * Pseudo code to procedures.
 *
 * no effort has been made to be complete, these are the ones
 * that are required for the basic functionalitly provided in this
 * program.
 */
#include "os.h"
/** TCG User IDs.
 * Links to TCGUSER in TCGCommand
 */
#define TCGUID_SIZE 6

typedef enum _TCG_UID {
    // users
    SMUID,
    THISSP,
    ADMINSP,
    ANYBODY,
    SID,
    // tables
    C_PIN_MSID,
} TCG_UID;

/** TCG Methods.
 * Links to TCGMETHOD in TCGCommand
 */
#define TCGMETHOD_SIZE 3

typedef enum _TCG_METHOD {
    PROPERTIES,
    STARTSESSION,
    GET,
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

/** Encoded Names.
 * Where are these documented?  I only found
 * this one in the Aplication Note :-(
 */
typedef enum _TCG_NAME {
    HOSTPROPERTIES = 0x00,
} TCG_NAME;

/** Useful tiny atoms.
 * I am only declaring frequently used
 *
 * start with lowercase u or s to avoid confusion with system types
 */
typedef enum _TCG_TINY_ATOM {
    uINT00 = 0x00,
    uINT01 = 0x01,
    uINT02 = 0x02,
    uINT03 = 0x03,
    uINT04 = 0x04,
} TCG_TINY_ATOM;

/** Useful short atoms.
 * I am only declaring the ones frequently used
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
    USER_ERROR_CODE = 0xff /* I use this code */
} TCGSTATUSCODE;