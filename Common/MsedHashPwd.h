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
class MsedDev;

using namespace std;
/** Hash the password using the drive serial number as salt.
 * This is far from ideal but it's better that a single salt as
 * it should prevent attacking the password with a prebuilt table
 * 
 * This is an intermediary pass through so that the real hash
 * function (MsedHashPassword) can be tested and verified.
 * @param  hash The field whare the hash is to be placed
 * @param password The password to be hashed
 * @param device the device where the password is to be used
 */
void MsedHashPwd(vector<uint8_t> &hash, char * password, MsedDev * device);
/** Hash a passwor using the PBDKF2<SHA1> function 
 *
 * @param hash Field where hash returned
 * @param password password to be hashed
 * @param salt salt to be used in the hash
 * @param iter number of iterations to be preformed 
 * @param hashsize size of hash to be returned
 */
void MsedHashPassword(vector<uint8_t> &hash, char * password, vector<uint8_t> salt,
        unsigned int iter = 75000, uint8_t hashsize = 32);
/** Test the hshing function using publicly available test cased and report */
int MsedTestPBDKF2();
