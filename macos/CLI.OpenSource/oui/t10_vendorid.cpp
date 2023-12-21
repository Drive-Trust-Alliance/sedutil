//
//  t10_vendorid.cpp
//
//  T10 registered vendor
//  see https://www.t10.org/vendorid.txt
//
//  Created by Scott Marks on 10/10/2022.
//
//

#include "t10_vendorid.hpp"

static CStringKeyValuePair data[]={
#include "t10_vendorid.inc"
};
static const size_t nData = sizeof(data)/sizeof(data[0]);

#if defined(__clang__)
[[clang::no_destroy]]
#elif defined(__GNUC__)
[[no_destroy]]
#endif
const CString_Lookup_Table vendor_for_vendorID_table =
    make_table<CString_Lookup_Table>(data,nData);

#if defined(__clang__)
[[clang::no_destroy]]
#elif defined(__GNUC__)
[[no_destroy]]
#endif
const CString_Canonical_Lookup_Table vendor_for_vendorID_canonically_table =
    make_table<CString_Canonical_Lookup_Table>(data,nData);

#if defined(__clang__)
[[clang::no_destroy]]
#elif defined(__GNUC__)
[[no_destroy]]
#endif
const CString_Lookup_Table vendorID_for_vendor_table =
    make_inverse_table<CString_Lookup_Table>(data,nData);

#if defined(__clang__)
[[clang::no_destroy]]
#elif defined(__GNUC__)
[[no_destroy]]
#endif
const CString_Canonical_Lookup_Table vendorID_for_vendor_canonically_table =
    make_inverse_table<CString_Canonical_Lookup_Table>(data,nData);

