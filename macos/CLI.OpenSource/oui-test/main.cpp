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
    const char * manufacturer = manufacturer_for_oui(oui);
    if (manufacturer == NULL) {
        std::cerr << oui << " *** not found ***"  << std::endl;
        return NULL;
    }
    std::cout << oui << " => " << manufacturer << std::endl;
    
    const char * vendorID;
    if (NULL != (vendorID = vendorID_for_vendor(manufacturer))) {
        std::cout << manufacturer << " => " << vendorID << std::endl;
        return vendorID;
    }

    std::cerr << manufacturer << " *** not found exactly ***"  << std::endl;
    
    if (NULL != (vendorID = vendorID_for_vendor_canonically(manufacturer))) {
        std::cout << manufacturer << " ≈> " << vendorID << std::endl;
        return vendorID;
    }
    
    std::cerr << manufacturer << " *** not found canonically ***"  << std::endl;

    return NULL;
}

static const char * oui_for_vendor_canonically_if_necessary__INSTRUMENTED(const char * vendor) {
    const char * oui;
    if (NULL != (oui = oui_for_vendor(vendor))) {
        std::cout << vendor << " ==> " << oui << std::endl;
        return oui;
    }
    if (NULL != (oui = oui_for_vendor_canonically(vendor))) {
        std::cout << vendor << " ≈≈> " << oui << std::endl;
        return oui;
    }
    return NULL;
}

static const char * oui_for_vendorID_canonically_if_necessary__INSTRUMENTED(const char *vendorID) {
    const char * vendor=vendor_for_vendorID(vendorID);
    if (vendor == NULL) {
        std::cerr << vendorID << " *** not found ***"  << std::endl;
        return NULL;
    }
    
    std::cout << vendorID << " ==> " << vendor << std::endl;
    
    return oui_for_vendor_canonically_if_necessary__INSTRUMENTED(vendor);
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
