# oui
Access IEEE OUI data and the T10 Vendor ID data

C++ class for accessing the IEEE OUI database and the T10 Vendor IT data

Data are kept in flat files which can be refreshed by simple scripts, included.

A "canonical" form for vendor/manufacturer information is defined to allow inexact
matching.  The canonical form is defined by mapping each alphanum character to its
corresponding uppercase character, mapping each non-empty sequence of non-alphanum
characters to a single blank character, and by trimming any leading or trailing blank.

Two vendor values are determined to match canonically iff their canonical
forms match.
