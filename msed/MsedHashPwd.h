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
#include <vector>
class MsedBaseDev;

using namespace std;
/** Hash the password using the drive serialnumber as salt.
 * This is an intermediary pass through so that the real hash
 * function (MsedHashPassword) can be tested and verified.
 * This is far from ideal but it's better that a single salt as
 * it should prevent attacking the password with a prebuilt table
 */
void MsedHashPwd(vector<uint8_t> &hash, char * password, MsedBaseDev * device);
void MsedHashPassword(vector<uint8_t> &hash, char * password, vector<uint8_t> salt,
        unsigned int iter = 75000, uint8_t hashsize = 32);
int MsedTestPBDKF2();
