//
//  CStringMap.hpp
//
//  IEEE Organizationally unique identifiers
//  see https://en.wikipedia.org/wiki/Organizationally_unique_identifier
//
//  Created by Scott Marks on 09/10/2017.
//
//

#ifndef CStringMap_hpp
#define CStringMap_hpp

#include <unordered_map>



typedef const char * CString;

struct CStringKeyValuePair {
    CString _Nonnull key;
    CString _Nonnull value;
};


struct equal_to_CString
{
    typedef bool __result_type;  // used by valarray
    bool operator()(const CString _Nonnull & __x, const CString _Nonnull & __y) const
        {
            return  0 == strcmp(__x, __y) ;
        }
};

struct hash_CString
{
    typedef size_t __result_type;  // used by valarray
    size_t operator()(const CString _Nonnull & __x) const
        {
            size_t x_length = strlen(__x);
            // define and use a max length so the buffer size is known at compile time
            size_t result = std::__murmur2_or_cityhash<size_t>()(__x, x_length);
            return result;
        }
};


typedef std::unordered_map< CString,
                            CString,
                            hash_CString,
                            equal_to_CString>
    CString_Lookup_Table;



static size_t canonicalize(const _Nonnull CString & raw, size_t raw_length, char result[_Nonnull] ){
    result[0]=0;
//    size_t result_length=0;
    char * q = result ;
    for (const char * p = raw, *pend = raw + raw_length;  p < pend;  p++) {
        char c = *p;
        if (isalnum(c)) {
            *(q++) = static_cast<char>(toupper(c));     // Alphanumerics are upper-cased
        } else if (result<q && q[-1] != ' ') {              // No leading blank
            *(q++) = ' ';                               // All others collapsed to single blank
        }
    }
    // Insert trailing NUL, possibly trimming trailing blank
    *(q[-1] == ' ' ? --q : q) = 0;
    return static_cast<size_t>(q-result);
}

struct canonically_equal_to_CString
{
    typedef bool __result_type;  // used by valarray
    bool operator()(const CString _Nonnull & __x, const CString _Nonnull & __y) const
        {
            size_t x_length = strlen(__x);
            size_t y_length = strlen(__y);
            // define and use a max length so the buffer size is known at compile time
#define MAX_LENGTH 63
            if (MAX_LENGTH<x_length) x_length = MAX_LENGTH;
            if (MAX_LENGTH<y_length) y_length = MAX_LENGTH;
            char cx[1+MAX_LENGTH];
            char cy[1+MAX_LENGTH];
#undef MAX_LENGTH
            size_t cx_length = canonicalize(__x, x_length, cx);
            size_t cy_length = canonicalize(__y, y_length, cy);
            return cx_length == cy_length && 0 == memcmp(cx, cy, cx_length) ;
        }
};

struct canonically_hash_CString
{
    typedef size_t __result_type;  // used by valarray
    size_t operator()(const CString _Nonnull & __x) const
        {
            size_t x_length = strlen(__x);
            // define and use a max length so the buffer size is known at compile time
#define MAX_LENGTH 63
            if (MAX_LENGTH<x_length) x_length = MAX_LENGTH;
            char cx[1+MAX_LENGTH];
#undef MAX_LENGTH
            size_t cx_length = canonicalize(__x, x_length, cx);
            size_t result = std::__murmur2_or_cityhash<size_t>()(cx, cx_length);
            return result;
        }
};

typedef std::unordered_map< CString,
                            CString,
                            canonically_hash_CString,
                            canonically_equal_to_CString>
    CString_Canonical_Lookup_Table;



template <typename _Table>
_Table make_table( CStringKeyValuePair data[_Nonnull], size_t nData) {
    _Table t;
    for (auto *p=&data[0], *pend=&data[nData]; p!=pend; p++)
        t[p->key]=p->value;
    t.reserve( t.size() );
    return t;
}

template <typename _Table>
_Table make_inverse_table( CStringKeyValuePair data[_Nonnull], size_t nData) {
    _Table t;
    for (auto *p=&data[0], *pend=&data[nData]; p!=pend; p++)
        t[p->value]=p->key;   // roles reversed, collisions expected
    t.reserve( t.size() );
    return t;
}


template<typename _Table, typename _KeyType=/*_Table::key_type*/CString, typename _ValueType=CString>
_ValueType __nullable lookup(_Table table, _KeyType key) {
    auto pValue=table.find(key);
    return pValue == table.end() ? NULL : pValue->second;
}



#endif /* CStringMap_hpp */
