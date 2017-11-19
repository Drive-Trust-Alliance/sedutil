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
#include <vector>
class DtaDev;

using namespace std;
/** Hash the password using the drive serial number as salt.
 * This is far from ideal but it's better that a single salt as
 * it should prevent attacking the password with a prebuilt table
 * 
 * This is an intermediary pass through so that the real hash
 * function (DtaHashPassword) can be tested and verified.
 * @param  hash The field whare the hash is to be placed
 * @param password The password to be hashed
 * @param device the device where the password is to be used
 */
void DtaHashPwd(vector<uint8_t> &hash, char * password, DtaDev * device);
/** Hash a passwor using the PBDKF2<SHA1> function 
 *
 * @param hash Field where hash returned
 * @param password password to be hashed
 * @param salt salt to be used in the hash
 * @param iter number of iterations to be preformed 
 * @param hashsize size of hash to be returned
 */
void DtaHashPassword(vector<uint8_t> &hash, const vector<uint8_t> &password,
        const vector<uint8_t> &salt, unsigned int iter = 75000, uint8_t hashsize = 32);
/** Test the hshing function using publicly available test cased and report */
int TestPBKDF2();
