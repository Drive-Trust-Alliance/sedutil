//
//  oui.cpp
//
//  IEEE Organizationally unique identifiers
//  see https://en.wikipedia.org/wiki/Organizationally_unique_identifier
//
//  Created by Scott Marks on 09/10/2017.
//
//

#include "oui.hpp"

static CStringKeyValuePair data[]={
#define __hex__(oui) #oui
#include "oui.inc"
#undef __hex__
};
static const size_t nData=sizeof(data)/sizeof(data[0]);


#if defined(__clang__)
[[clang::no_destroy]]
#elif defined(__GNUC__)
[[no_destroy]]
#endif
const CString_Lookup_Table manufacturer_for_oui_table =
    make_table<CString_Lookup_Table>(data,nData);


#if defined(__clang__)
[[clang::no_destroy]]
#elif defined(__GNUC__)
[[no_destroy]]
#endif
const CString_Canonical_Lookup_Table manufacturer_for_oui_canonically_table =
    make_table<CString_Canonical_Lookup_Table>(data,nData);

#if defined(__clang__)
[[clang::no_destroy]]
#elif defined(__GNUC__)
[[no_destroy]]
#endif
const CString_Lookup_Table oui_for_manufacturer_table =
    make_inverse_table<CString_Lookup_Table>(data,nData);

#if defined(__clang__)
[[clang::no_destroy]]
#elif defined(__GNUC__)
[[no_destroy]]
#endif
const CString_Canonical_Lookup_Table oui_for_manufacturer_canonically_table =
    make_inverse_table<CString_Canonical_Lookup_Table>(data,nData);


const CString private_oui = "1000000";
