//
//  main.cpp
//  oui-test
//
//  Created by Scott Marks on 09/20/2017.
//
//

#include "oui_vendor.hpp"
#include <iostream>

//
// Per oui_vendor, but with descriptive output to std::cout and std::err
//

static const char * vendorID_for_oui_canonically_if_necessary__INSTRUMENTED(const char *oui) {
    CString_Lookup_Table::const_iterator pManufacturer = manufacturer_for_oui.find(oui);
    if (pManufacturer == manufacturer_for_oui.end()) {
        std::cerr << oui << " *** not found ***"  << std::endl;
        return NULL;
    }
    
    const char * manufacturer(pManufacturer->second);
    std::cout << oui << " => " << manufacturer << std::endl;
    
    const char * vendorID = NULL ;

    {
        CString_Lookup_Table::const_iterator pVendorID = vendorID_for_vendor.find(manufacturer);
        if (pVendorID != vendorID_for_vendor.end()) {
            vendorID =  pVendorID->second ;
            std::cout << manufacturer << " => " << vendorID << std::endl;
            return vendorID;
        }
    }
    
    std::cerr << manufacturer << " *** not found exactly ***"  << std::endl;
    
    {
        CString_Canonical_Lookup_Table::const_iterator pVendorID = vendorID_for_vendor_canonically.find(manufacturer);
        if (pVendorID != vendorID_for_vendor_canonically.end()) {
            vendorID = pVendorID->second;
            std::cout << manufacturer << " ≈> " << vendorID << std::endl;
            return vendorID;
        }
    }
    
    std::cerr << manufacturer << " *** not found canonically ***"  << std::endl;

    return NULL;
}


static const char * oui_for_vendorID_canonically_if_necessary__INSTRUMENTED(const char *vendorID) {
    CString_Lookup_Table::const_iterator pVendor = vendor_for_vendorID.find(vendorID);
    if (pVendor == vendor_for_vendorID.end()) {
        std::cerr << vendorID << " *** not found ***"  << std::endl;
        return NULL;
    }
    
    const char * vendor(pVendor->second);
    std::cout << vendorID << " ==> " << vendor << std::endl;
    
    const char * oui = NULL ;

    {
        CString_Lookup_Table::const_iterator poui = oui_for_manufacturer.find(vendor);
        if (poui != oui_for_manufacturer.end()) {
            oui =  poui->second ;
            std::cout << vendor << " ==> " << oui << std::endl;
            return oui;
        }
    }
    
    std::cerr << vendorID << " *** not found exactly ***"  << std::endl;
    
    {
        CString_Canonical_Lookup_Table::const_iterator poui = oui_for_manufacturer_canonically.find(vendor);
        if (poui != oui_for_manufacturer_canonically.end()) {
            oui =  poui->second ;
            std::cout << vendor << " ≈≈> " << oui << std::endl;
            return oui;
        }
    }
    
    std::cerr << vendorID << " *** not found canonically ***"  << std::endl;

    return NULL;
}


int main(int argc, const char * argv[]) {
    bool thereHaveBeenErrors = false;
    for (int i=1; i<argc; std::cout << std::endl, i++) {
        const char * vendorID = vendorID_for_oui_canonically_if_necessary__INSTRUMENTED(argv[i]);
        if (vendorID != NULL) {
            const char * oui = oui_for_vendorID_canonically_if_necessary__INSTRUMENTED(vendorID);
            if (oui != NULL) {
                assert( 0 == strcmp(oui, oui_for_vendorID_canonically_if_necessary(vendorID)) );
                continue;
            }
        }
        thereHaveBeenErrors = true;
    }
    return thereHaveBeenErrors ? 1 : 0;
}
