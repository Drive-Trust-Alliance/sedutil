//
//  oui_vendor.hpp
//  oui
//
//  Created by Scott Marks on 11/30/22.
//

#ifndef oui_vendor_hpp
#define oui_vendor_hpp

#include <oui/oui.hpp>
#include <oui/t10_vendorid.hpp>

// OUI from IEEE
extern "C" CString manufacturer_for_oui(CString oui);
extern "C" CString manufacturer_for_oui_canonically(CString oui);
extern "C" CString manufacturer_for_oui_canonically_if_necessary(CString oui);

// Vendor ID from T10
extern "C" CString vendor_for_vendorID(CString vendorID);
extern "C" CString vendor_for_vendorID_canonically(CString vendorID);
extern "C" CString vendor_for_vendorID_canonically_if_necessary(CString vendorID);

extern "C" CString vendorID_for_vendor(CString vendor);
extern "C" CString vendorID_for_vendor_canonically(CString vendor) ;
extern "C" CString vendorID_for_vendor_canonically_if_necessary(CString vendor);

// Combined
extern "C" CString vendorID_for_oui(CString oui);
extern "C" CString vendorID_for_oui_canonically(CString oui);
extern "C" CString vendorID_for_oui_canonically_if_necessary(CString oui);

extern "C" CString oui_for_vendor(CString vendor) ;
extern "C" CString oui_for_vendor_canonically(CString vendor) ;
extern "C" CString oui_for_vendor_canonically_if_necessary(CString vendor) ;

extern "C" CString oui_for_vendorID(CString vendorID) ;
extern "C" CString oui_for_vendorID_canonically(CString vendorID) ;
extern "C" CString oui_for_vendorID_canonically_if_necessary(CString vendorID) ;

#endif /* oui_vendor_hpp */
