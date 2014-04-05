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

#include "TCGtasks.h"
#include "options.h"

int main(int argc, char * argv[])
{
	MSED_OPTIONS opts;
	if (options(argc, argv, &opts)) {
		//LOG(E) << "Invalid command line options ";
		return 1;
	}
	switch (opts.action){
		case 's':
			LOG(D) << "Performing diskScan() ";
			diskScan();
			break;
		case 'q':
			LOG(D) << "Performing diskquery() on " << argv[argc - 1];
			diskQuery(argv[argc - 1]);
			break;
		case 't':
			if (0 == opts.password) {
				LOG(E) << "Taking ownwership requires a *NEW* SID password (-p)";
				break;
			}
			LOG(D) << "Performing takeOwnership of " << argv[argc-1] << " with password " << argv[opts.password];
			return takeOwnership(argv[argc - 1], argv[opts.password]);
			break;
		case 'T':
			if (0 == opts.password) {
				LOG(E) << "Reverting the TPer requires a the SID password (-p)";
				break;
			}
			LOG(D) << "Performing revertTPer on " << argv[argc - 1] << " with password " << argv[opts.password];
			return revertTPer(argv[argc - 1], argv[opts.password]);
			break;
		default:
			LOG(E) << "Uable to determing what you want to do ";
			usage();
	}
	return 1;
}