/* C:B**************************************************************************
This software is © 2014 Bright Plaza Inc. <drivetrust@drivetrust.com>

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

// Operating specific or customizing option implementations can be included in the
// body of the switch statement processing options by entering them here like this:
//
//  case sedutiloption::catchFireAndBurn:
//      LOG(D) << "Run in circles! Scream and shout!";
//      return 127;
//      break;

#if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__)
#define OSNAME "linux"
#elif defined(APPLE) || defined(_APPLE) || defined(__APPLE__)
#define OSNAME "macOS"
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define OSNAME "windows"
#else
#define OSNAME "unknownOS";
#endif


case sedutiloption::version:
    LOG(D) << "print version number ";
    printf("sedutil-cli : %s.%s\n", OSNAME, GIT_VERSION);
    return 0;
    break;
