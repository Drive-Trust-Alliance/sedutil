/* C:B**************************************************************************
This software is Copyright 2014,2015 Michael Romeo <r0m30@r0m30.com>

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

typedef union
{
    // four bytes in big endian (network) byte order
    uint8_t     all[4];

    // TINY ATOM
    class CTinyAtom
    {
    public:
        enum
        {
            INDICATOR   = 0,    // 0b
            HEADER_SIZE = 1,
        };
        unsigned    value           : 6;
        unsigned    sign            : 1;
        unsigned    indicator       : 1;
    } __attribute__((packed)) TinyAtom;

    // TINY ATOM (sign)
    class CTinyAtom_sign
    {
    public:
        enum
        {
            INDICATOR   = 0,    // 0b
            HEADER_SIZE = 1,
        };
        signed      value           : 6;
        unsigned    sign            : 1;
        unsigned    indicator       : 1;
    } __attribute__((packed)) TinyAtom_sign;

    // SHORT ATOM
    class CShortAtom
    {
    public:
        enum
        {
            INDICATOR   = 2,    // 10b
            HEADER_SIZE = 1,
        };
        unsigned    length          : 4;
        unsigned    sign            : 1;
        unsigned    byte            : 1;
        unsigned    indicator       : 2;
    } __attribute__((packed)) ShortAtom;

    // MEDIUM ATOM
    class CMediumAtom
    {
    public:
        enum
        {
            INDICATOR   = 6,    // 110b
            HEADER_SIZE = 2,
        };
        unsigned    length_msb      : 3;
        unsigned    sign            : 1;
        unsigned    byte            : 1;
        unsigned    indicator       : 3;
        uint8_t     length_lsb;
    } __attribute__((packed)) MediumAtom;

    // LONG ATOM
    class CLongAtom
    {
    public:
        enum
        {
            INDICATOR   = 14,   // 1110b
            HEADER_SIZE = 4,
        };
        unsigned    sign            : 1;
        unsigned    byte            : 1;
        unsigned    reserved        : 2;
        unsigned    indicator       : 4;
        uint8_t     length[3];
    } __attribute__((packed)) LongAtom;

} CAtomHeader;

////////////////////////////////////////////////////////////////////////////////
class CMsedToken
////////////////////////////////////////////////////////////////////////////////
{
public:
    CMsedToken(void);
    ~CMsedToken(void);

    int parse(uint8_t * buf, uint32_t buflen);

    int print(FILE *stream, uint32_t buflen);

    typedef enum _TokenType
    {
        NONE,
        TINY_ATOM,
        SHORT_ATOM,
        MEDIUM_ATOM,
        LONG_ATOM,
        TOKEN,
    } TokenType;

    TokenType       m_TokenType;
    uint32_t        m_HeaderLength;
    uint32_t        m_DataLength;
    uint32_t        m_TokenLength;
    uint8_t *       m_token;
    uint8_t *       m_data;
    uint64_t        m_value;
    bool            m_sign;

private:
    void str2int(uint8_t * buf, bool byte);
    int printTokenType(FILE *stream, uint8_t token);
    int printAscii(FILE *stream, uint8_t * buf, uint32_t buflen);
    int printUID(FILE *stream, uint8_t buf[8]);
};

////////////////////////////////////////////////////////////////////////////////
extern uint8_t MsedAnnotatedDump(ATACOMMAND cmd, void * buffer, uint32_t bufferlen);
////////////////////////////////////////////////////////////////////////////////
