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
#pragma pack(push)
#pragma pack(1)
/*
 * These structures are here While I think about how to 
 * code a parser that isn't going to be a nightmare
 */
typedef struct _SSResponse {
	TCGHeader h;
	uint8_t call;
	uint8_t invoker[9];
	uint8_t method[9];
	uint8_t sl0;
	uint8_t smallatom01;
	uint32_t HostSessionNumber;
	uint8_t smallatom02;
	uint32_t TPerSessionNumber;
} SSResponse;
typedef struct _GenericResponse {
	TCGHeader h;
	uint8_t payload[500];
} GenericResponse;
#pragma pack(pop)