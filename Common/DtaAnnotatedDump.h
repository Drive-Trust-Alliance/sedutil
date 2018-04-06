/* C:B**************************************************************************
This software is Copyright 2014-2017 Bright Plaza Inc. <drivetrust@drivetrust.com>

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

#pragma pack(push,1)

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
    } TinyAtom;

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
    } TinyAtom_sign;

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
    } ShortAtom;

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
    } MediumAtom;

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
    } LongAtom;

} CAtomHeader;
#pragma pack(pop)
////////////////////////////////////////////////////////////////////////////////
class DtaToken
////////////////////////////////////////////////////////////////////////////////
{
public:
    DtaToken(void);
    ~DtaToken(void);

    void parse(uint8_t * buf, uint32_t buflen);

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
extern uint8_t DtaAnnotatedDump(ATACOMMAND cmd, void * buffer, uint32_t bufferlen);
////////////////////////////////////////////////////////////////////////////////
