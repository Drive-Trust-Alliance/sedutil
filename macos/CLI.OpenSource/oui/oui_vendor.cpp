//
//  oui_vendor.cpp
//  oui
//
//  Created by Scott Marks on 11/30/22.
//

#include "oui_vendor.hpp"

CString manufacturer_for_oui(CString oui) {
    return lookup(manufacturer_for_oui_table, oui);
}

CString manufacturer_for_oui_canonically(CString oui) {
    return lookup(manufacturer_for_oui_canonically_table, oui);
}


CString manufacturer_for_oui_canonically_if_necessary(CString oui) {
    CString manufacturer;
    if (NULL != (manufacturer = manufacturer_for_oui(oui)))
        return manufacturer;
    if (NULL != (manufacturer = manufacturer_for_oui_canonically(oui)))
        return manufacturer;
    return NULL;
}

CString vendorID_for_vendor(CString vendor) {
    return lookup(vendorID_for_vendor_table, vendor);
}


CString vendorID_for_vendor_canonically(CString vendor) {
    return lookup(vendorID_for_vendor_canonically_table, vendor);
}

CString vendorID_for_vendor_canonically_if_necessary(CString vendor) {
    CString vendorID;
    if (NULL != (vendorID = vendorID_for_vendor(vendor)))
        return vendorID;
    if (NULL != (vendorID = vendorID_for_vendor_canonically(vendor)))
        return vendorID;
    return NULL;
}

CString vendor_for_vendorID(CString vendorID) {
    return lookup(vendor_for_vendorID_table, vendorID);
}

CString vendor_for_vendorID_canonically(CString vendorID) {
    return lookup(vendor_for_vendorID_canonically_table, vendorID);
}

CString vendor_for_vendorID_canonically_if_necessary(CString vendorID) {
    CString vendor;
    if (NULL != (vendor = vendor_for_vendorID(vendorID)))
        return vendor;
    if (NULL != (vendor = vendor_for_vendorID_canonically(vendorID)))
        return vendor;
    return NULL;
}

CString vendorID_for_oui(CString oui) {
    CString manufacturer = manufacturer_for_oui(oui);
    return manufacturer == NULL ? NULL : vendorID_for_vendor(manufacturer);
}

CString vendorID_for_oui_canonically(CString oui) {
    CString manufacturer = manufacturer_for_oui(oui);
    return manufacturer == NULL ? NULL : vendorID_for_vendor_canonically(manufacturer);
}

CString vendorID_for_oui_canonically_if_necessary(CString oui) {
    CString manufacturer = manufacturer_for_oui(oui);
    return manufacturer == NULL ? NULL : vendorID_for_vendor_canonically_if_necessary(manufacturer);
}

CString oui_for_vendor(CString vendor) {
    return lookup(oui_for_manufacturer_table, vendor);
}

CString oui_for_vendor_canonically(CString vendor) {
    return lookup(oui_for_manufacturer_canonically_table, vendor);
}

CString oui_for_vendor_canonically_if_necessary(CString vendor) {
    CString oui;
    if (NULL != (oui = oui_for_vendor(vendor)))
        return oui;
    if (NULL != (oui = oui_for_vendor_canonically(vendor)))
        return oui;
    return NULL;
}

CString oui_for_vendorID(CString vendorID) {
    CString vendor=vendor_for_vendorID(vendorID);
    return vendor == NULL ? NULL : oui_for_vendor(vendor);
}

CString oui_for_vendorID_canonically(CString vendorID) {
    CString vendor=vendor_for_vendorID_canonically(vendorID);
    return vendor == NULL ? NULL : oui_for_vendor_canonically(vendor);
}

CString oui_for_vendorID_canonically_if_necessary(CString vendorID) {
    CString vendor=vendor_for_vendorID_canonically_if_necessary(vendorID);
    return vendor == NULL ? NULL : oui_for_vendor_canonically_if_necessary(vendor);
}

