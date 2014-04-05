/* C:B**************************************************************************
This software is Copyright (c) 2014 Michael Romeo <r0m30@r0m30.com>

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
#include "os.h"
#include "options.h"

void usage()
{
    printf("msed v%s Copyright (c) 2014 Michael Romeo <r0m30@r0m30.com>\n", MSED_VERSION);
    printf("\n");
    printf("msed is a utility to manage self encrypting drives\n");
    printf("that conform to the Trusted Computing Group OPAL 2.0 SSC\n");
    printf("specification \n");
    printf("Usage:\n");
    printf("msed <action> <options> <device>\n\n");
    printf("actions \n");
    printf("-s, --scan \n");
    printf("                                scans the devices on the system \n");
    printf("                                identifying Opal 2.0 compliant devices \n");
    printf("-t, --takeownership \n");
    printf("                                change the SID password of the device\n");
    printf("                                password(-p) option required to specify the\n");
    printf("                                new password\n");
    printf("-T, --revertTPer \n");
    printf("                                set the device back to factory defaults \n");
    printf("                                password(-p) option required to specify the SID password\n");
    printf("                                see TCG documentation for optional Vendor unique\n");
    printf("                                exceptions\n");
    printf("options\n");
    printf("-p --password <password>     \n");
    printf("                                 the password for the action \n");
    printf("-v	                             increase verbosity, can be repeated multiple times\n");
    printf("-q, --quiet                      suppress all but ERROR level messages\n");
    printf("-h, --help                       displays this message and ignores all other actions and options\n");
    printf("\n");
    printf("Examples \n");
    printf("msed --scan \n");
    printf("msed --takeownership --password newSIDpassword \\\\.\\PhysicalDisk2 \n");
    printf("msed --revertTPer --password theSIDPassword /dev/sg0 \n");
    return;
}

/*
 * This would probably be easier with getopts but
 * I'm not ready to commit to GPL
 */
uint8_t options(int argc, char * argv[], MSED_OPTIONS * opts)
{
    memset(opts, 0, sizeof (MSED_OPTIONS));
    uint8_t loggingLevel = 6;
    CLog::Level() = CLog::FromInt(loggingLevel);

    if (2 > argc) {
        usage();
        return 1;
    }
    for (uint8_t i = 1; i < argc; i++) {
        if (!(strcmp("-h", argv[i])) || !(strcmp("--help", argv[i]))) {
            usage();
            return 1;
        }
        else if (!(strcmp("-t", argv[i])) || !(strcmp("--takeownership", argv[i])))
            opts->action = 't';
        else if (!(strcmp("-T", argv[i])) || !(strcmp("--revertTPer", argv[i])))
            opts->action = 'T';
        else if (!(strcmp("-s", argv[i])) || !(strcmp("--scan", argv[i])))
            opts->action = 's';
        else if (!(strcmp("-p", argv[i])) || !(strcmp("-password", argv[i])))
            opts->password = ++i;
        else if ('v' == argv[i][2]) {
            loggingLevel += strnlen(argv[i], 10);
            if (loggingLevel > 7) loggingLevel = 7;
            LOG(D) << "Setting log level to " << loggingLevel;
            CLog::Level() = CLog::FromInt(loggingLevel);
        }
        else if (i != argc - 1) {
            LOG(E) << "Invalid command line argument " << argv[i];
            return 1;
        }
    }
    //if (i == argc - 1){
    //	LOG(E) << "Cannt Find the <device> paramater ";
    //	usage();
    //	return 1;
    //}
    // some basic sanity checks
    if ('s' != opts->action) {
        if (argc < 3) {
            LOG(E) << "To few command line options";
            return -1;
        }
        else {
            if ('-' == argv[argc - 1][1]) {
                LOG(E) << "The last argument must be the device ";
                return -1;
            }
        }
    }
    // TODO: check for multiple actions specified
    //if (1 != opts->takeOwnership + opts->scanSystem + opts->revertSP) {
    //	LOG(E) << "only one action can be performed at a time";
    //	usage();
    //	return 1;
    //}
    return 0;
}