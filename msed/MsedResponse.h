/* C:B**************************************************************************
This software is Copyright 2014,2015 Michael Romeo <r0m30@r0m30.com>

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
#include <vector>
#include <string>
#include "MsedStructures.h"
#include "MsedLexicon.h"


/** Object containing the parsed tokens.
 * a vector of vector<uint8_T> that contains each token
 * returned in the TCG response
 */
class MsedResponse {
public:
    MsedResponse();
    /** constructor
     * @param buffer the response returned by a TCG command */
    MsedResponse(void * buffer);
    ~MsedResponse();
    /** (re)initialize the object using a new buffer
     * @param buffer the response returned by a TCG command */
    void init(void * buffer);
    /** return the type of token 
     * @param tokenNum the 0 based number of the token*/
    OPAL_TOKEN tokenIs(uint32_t tokenNum);
    /** return the length of a token
    * @param tokenNum the 0 based number of the token*/
    uint32_t getLength(uint32_t tokenNum);
    /** return an unsigned 64bit integer
    * @param tokenNum the 0 based number of the token*/
    uint64_t getUint64(uint32_t tokenNum);
    /** return an unsigned 32bit integer
    * @param tokenNum the 0 based number of the token*/
    uint32_t getUint32(uint32_t tokenNum);
    /** return an unsigned 16bit integer
    * @param tokenNum the 0 based number of the token*/
    uint16_t getUint16(uint32_t tokenNum);
    /** return an unsigned 8bit integer
    * @param tokenNum the 0 based number of the token*/
    uint8_t getUint8(uint32_t tokenNum);
    /** return the number of tokens in the response */
    uint32_t getTokenCount();
    /** return a string of the token 
     * @param tokenNum the 0 based number of the token*/
    std::string getString(uint32_t tokenNum);
    /** return the entire token including TCG token overhead 
    * @param tokenNum the 0 based number of the token*/
    std::vector<uint8_t> getRawToken(uint32_t tokenNum);
    /** return the token in an array of uint8_t  
    * @param tokenNum the 0 based number of the token
    * @param bytearray pointer to array for return data */
    void getBytes(uint32_t tokenNum, uint8_t bytearray[]);
    
    OPALHeader h; /**< TCG Header fields of the response */

private:

    std::vector<std::vector<uint8_t>> response;   /**< tokenized resonse  */
};



