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
#pragma once
/*
 * Manage the session to a TPer.
 */
#include "TCGlexicon.h"
class TCGcommand;
class TCGdev;

class TCGdev;

class TCGsession {
public:
    TCGsession(TCGdev * device);
    ~TCGsession();
    uint8_t start(TCG_UID SP,
            char * HostChallenge = NULL,
            TCG_UID SignAuthority = TCG_UID::TCG_UID_HEXFF);
    void setProtocol(uint8_t value);
    void expectAbort();
    uint8_t sendCommand(TCGcommand * cmd);
private:
    TCGsession();
    uint8_t SEND(TCGcommand * cmd);
    uint8_t RECV(void * resp);
    char * methodStatus(uint8_t status);
    TCGdev * d;
    uint32_t bufferpos = 0;
    /* The session numbers should be taken from the
     * syncsession response so there will be no
     * issues with endianess
     */
    uint32_t TSN = 0;
    uint32_t HSN = 0;
    uint8_t willAbort = 0;
    uint8_t TCGProtocol = 0x01;
};

