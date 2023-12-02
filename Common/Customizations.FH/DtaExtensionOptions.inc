/* C:B**************************************************************************
 This software is Copyright 2014, 2022 Bright Plaza Inc. <drivetrust@drivetrust.com>
 This software is Copyright 2023 Fidelity Height LLC <drivetrust@drivetrust.com>  TODO: fix URL

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

// Operating specific or customizing options can be included in the options list
// by entering them here like this:
//
//  BEGIN_OPTION(catchFireAndBurn, 2)
//      OPTION_IS(password)
//      OPTION_IS(device)
//  END_OPTION

BEGIN_OPTION(MBRRead, 5)
    OPTION_IS(password)
    OPTION_IS(pbafile)
    OPTION_IS(startpos)
    OPTION_IS(len)
    OPTION_IS(device)
END_OPTION
BEGIN_OPTION(createUSB, 3) OPTION_IS(pbafile) OPTION_IS(device) OPTION_IS(devusb) END_OPTION
BEGIN_OPTION(pbaValid, 2) OPTION_IS(password) OPTION_IS(device) END_OPTION
