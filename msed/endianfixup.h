/* C:B**************************************************************************
This software is Copyright © 2014 Michael Romeo <r0m30@r0m30.com>

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
/* These are a few macros to fixup the endianess of the data
 * returned by the drive as specified in the TCG OPAL SSC standards
 * Since this is a low use utility porgram it shouldn't be to
 * ugly that these are macros
 */

/*
 * Windows provides system call in the network stack to do this but
 * I've never had much luck when the winsock headers get in the mix
 */
//
//TODO: add a test on the endianess of the system and define
//  empty macros if the system is big endian
#pragma once
#ifdef __gnu_linux__
#include <endian.h>
#if __BYTE_ORDER != __LITTLE_ENDIAN
#error This code does not support big endian architectures
#endif
#endif
#define SWAP16(x) ((uint16_t) ((x & 0x00ff) << 8) | ((x & 0xff00) >> 8))
#define SWAP32(x) ((uint32_t) ((x & 0x000000ff) << 24) | ((x & 0x0000ff00) << 8) | ((x & 0x00ff0000) >> 8) | ((x & 0xff000000) >> 24))