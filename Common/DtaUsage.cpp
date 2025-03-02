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

// This routine should be compiled with the main routine sedutil-cli and the
// appropriate Common/Customizations folder, and not with the sedutil library.

#include "os.h"
#include "log.h"
#define DTAOPTIONS_INCLUDED_FOR_USAGE
#include "DtaOptions.h"
// #include "DtaLexicon.h"
#include "DtaUsage.h"
#include "Version.h"

#define LOCKINGRANGEARG(lockingrange)
#define MBRSTATEARG(arg,mbrstate)
#define LOCKINGSTATEARG(lockingstate)
#define TCGRESETTYPEARG(resettype)
void usage()
{
    printf("sedutil-cli v%s © 2014 Bright Plaza Inc. <drivetrust@drivetrust.com>\n", GIT_VERSION);
    printf("a utility to manage self encrypting drives that conform\n");
    printf("to the Trusted Computing Group Storage Architecture Core Specification\n");
    printf("and to one of the Security Subsystem Class specifications such as OPAL 2.0\n");
    printf("General Usage:                     (see readme for extended commandset)\n");
    printf("sedutil-cli [OPTION]... <ACTION> [OPTION]... <DEVICE>\n");
    printf("-v (optional)                       increase verbosity, one to five v's\n");
    printf("-a (optional)                       do not activate LockingSP\n");
    printf("-u (optional)                       user mode ON\n");
    printf("-n (optional)                       no password hashing. Passwords will be sent in clear text!\n");
    printf("-t (optional)                       translate hex password strings (only when not hashing, requires -n)\n");
    printf("-l (optional)                       log style output to stderr only\n");
    printf("\n");
    printf("actions \n");

    static const char * usages[]={
#include "DtaOptions.inc"
    };
    for (size_t i=0 ; i < sizeof(usages)/sizeof(usages[0]); i++) fputs(usages[i], stdout);


    // Customization commands
    printf("--version \n");
    printf("                                print sedutil-cli version, including build tag\n");

    printf("\n");
    printf("Examples \n");
    printf("sedutil-cli --scan <skipdevice>\n");
    printf("sedutil-cli --query %s \n", DEVICEEXAMPLE);
    printf("sedutil-cli --yesIreallywanttoERASEALLmydatausingthePSID <PSIDALLCAPSNODASHES> %s \n", DEVICEEXAMPLE);
    printf("sedutil-cli --initialSetup <newSIDpassword> %s \n", DEVICEEXAMPLE);

    // Customization examples
    printf("sedutil-cli --version \n");
    //    printf("--createUSB <file> <device1> <device2\n");
    //    printf("                                Write image file to USB\n");

    return;
}
