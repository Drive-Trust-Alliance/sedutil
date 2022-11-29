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
    for (int i=1; i<argc; i++) {
        const char * oui(argv[i]);
        CString_Lookup_Table::const_iterator pManufacturer = manufacturer_for_oui.find(oui);
        CString_Lookup_Table::const_iterator moEnd         = manufacturer_for_oui.end();
        if (pManufacturer != moEnd) {
            std::cout << oui << " => " << pManufacturer->second << std::endl;


            const char * vendor(pManufacturer->second);
            CString_Lookup_Table::const_iterator pVendorID = vendorID_for_vendor.find(vendor);
            CString_Lookup_Table::const_iterator vIvEnd    = vendorID_for_vendor.end();
            if (pVendorID != vIvEnd) {
                std::cout << vendor << " => " << pVendorID->second << std::endl;
            } else {
                std::cerr << vendor << " *** not found ***"  << std::endl;
                thereHaveBeenErrors = true;
            }


        } else {
            std::cerr << oui << " *** not found ***"  << std::endl;
            thereHaveBeenErrors = true;
        }
        

    }
    return thereHaveBeenErrors ? 1 : 0;
}
