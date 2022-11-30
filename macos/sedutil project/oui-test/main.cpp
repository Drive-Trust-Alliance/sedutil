//
//  main.cpp
//  oui-test
//
//  Created by Scott Marks on 09/20/2017.
//
//

#include "oui.hpp"
#include "t10_vendorid.hpp"
#include <iostream>

int main(int argc, const char * argv[]) {
    bool thereHaveBeenErrors = false;
    for (int i=1; i<argc; std::cout << std::endl, i++) {
        const char * oui(argv[i]);

        CString_Lookup_Table::const_iterator pManufacturer = manufacturer_for_oui.find(oui);
        CString_Lookup_Table::const_iterator moEnd         = manufacturer_for_oui.end();
        if (pManufacturer == moEnd) {
            std::cerr << oui << " *** not found ***"  << std::endl;
            thereHaveBeenErrors = true;
            continue;
        }
        
        const char * vendor(pManufacturer->second);
        std::cout << oui << " => " << vendor << std::endl;

        CString_Lookup_Table::const_iterator pVendorID = vendorID_for_vendor.find(vendor);
        CString_Lookup_Table::const_iterator vIvEnd    = vendorID_for_vendor.end();
        if (pVendorID == vIvEnd) {
            std::cerr << vendor << " *** not found ***"  << std::endl;
            thereHaveBeenErrors = true;
            continue;
        }
        
        const char * vendorID = pVendorID->second ;
        std::cout << vendor << " => " << vendorID << std::endl;
    }
    return thereHaveBeenErrors ? 1 : 0;
}
