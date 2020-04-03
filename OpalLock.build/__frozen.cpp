// This provides the frozen (compiled bytecode) files that are included if
// any.
#include <Python.h>

#include "nuitka/constants_blob.h"

// Blob from which modules are unstreamed.
#define stream_data constant_bin

// These modules should be loaded as bytecode. They may e.g. have to be loadable
// during "Py_Initialize" already, or for irrelevance, they are only included
// in this un-optimized form. These are not compiled by Nuitka, and therefore
// are not accelerated at all, merely bundled with the binary or module, so
// that CPython library can start out finding them.

struct frozen_desc {
    char const *name;
    ssize_t start;
    int size;
};

void copyFrozenModulesTo( struct _frozen *destination )
{
    struct frozen_desc frozen_modules[] = {
        {"base64", 6456583, 10790},
        {"codecs", 6467373, 36231},
        {"copy_reg", 6503604, 5050},
        {"encodings", 6508654, -4350},
        {"encodings.aliases", 6513004, 8757},
        {"encodings.ascii", 6521761, 2223},
        {"encodings.base64_codec", 6523984, 3755},
        {"encodings.big5", 6527739, 1727},
        {"encodings.big5hkscs", 6529466, 1767},
        {"encodings.bz2_codec", 6531233, 4635},
        {"encodings.charmap", 6535868, 3420},
        {"encodings.cp037", 6539288, 2796},
        {"encodings.cp1006", 6542084, 2882},
        {"encodings.cp1026", 6544966, 2810},
        {"encodings.cp1140", 6547776, 2796},
        {"encodings.cp1250", 6550572, 2833},
        {"encodings.cp1251", 6553405, 2830},
        {"encodings.cp1252", 6556235, 2833},
        {"encodings.cp1253", 6559068, 2846},
        {"encodings.cp1254", 6561914, 2835},
        {"encodings.cp1255", 6564749, 2854},
        {"encodings.cp1256", 6567603, 2832},
        {"encodings.cp1257", 6570435, 2840},
        {"encodings.cp1258", 6573275, 2838},
        {"encodings.cp424", 6576113, 2826},
        {"encodings.cp437", 6578939, 8031},
        {"encodings.cp500", 6586970, 2796},
        {"encodings.cp720", 6589766, 2893},
        {"encodings.cp737", 6592659, 8259},
        {"encodings.cp775", 6600918, 8045},
        {"encodings.cp850", 6608963, 7778},
        {"encodings.cp852", 6616741, 8047},
        {"encodings.cp855", 6624788, 8228},
        {"encodings.cp856", 6633016, 2858},
        {"encodings.cp857", 6635874, 7768},
        {"encodings.cp858", 6643642, 7748},
        {"encodings.cp860", 6651390, 8014},
        {"encodings.cp861", 6659404, 8025},
        {"encodings.cp862", 6667429, 8160},
        {"encodings.cp863", 6675589, 8025},
        {"encodings.cp864", 6683614, 8156},
        {"encodings.cp865", 6691770, 8025},
        {"encodings.cp866", 6699795, 8260},
        {"encodings.cp869", 6708055, 8072},
        {"encodings.cp874", 6716127, 2924},
        {"encodings.cp875", 6719051, 2793},
        {"encodings.cp932", 6721844, 1735},
        {"encodings.cp949", 6723579, 1735},
        {"encodings.cp950", 6725314, 1735},
        {"encodings.euc_jis_2004", 6727049, 1791},
        {"encodings.euc_jisx0213", 6728840, 1791},
        {"encodings.euc_jp", 6730631, 1743},
        {"encodings.euc_kr", 6732374, 1743},
        {"encodings.gb18030", 6734117, 1751},
        {"encodings.gb2312", 6735868, 1743},
        {"encodings.gbk", 6737611, 1719},
        {"encodings.hex_codec", 6739330, 3707},
        {"encodings.hp_roman8", 6743037, 4079},
        {"encodings.hz", 6747116, 1711},
        {"encodings.idna", 6748827, 6326},
        {"encodings.iso2022_jp", 6755153, 1780},
        {"encodings.iso2022_jp_1", 6756933, 1796},
        {"encodings.iso2022_jp_2", 6758729, 1796},
        {"encodings.iso2022_jp_2004", 6760525, 1820},
        {"encodings.iso2022_jp_3", 6762345, 1796},
        {"encodings.iso2022_jp_ext", 6764141, 1812},
        {"encodings.iso2022_kr", 6765953, 1780},
        {"encodings.iso8859_1", 6767733, 2835},
        {"encodings.iso8859_10", 6770568, 2850},
        {"encodings.iso8859_11", 6773418, 2944},
        {"encodings.iso8859_13", 6776362, 2853},
        {"encodings.iso8859_14", 6779215, 2871},
        {"encodings.iso8859_15", 6782086, 2850},
        {"encodings.iso8859_16", 6784936, 2852},
        {"encodings.iso8859_2", 6787788, 2835},
        {"encodings.iso8859_3", 6790623, 2842},
        {"encodings.iso8859_4", 6793465, 2835},
        {"encodings.iso8859_5", 6796300, 2836},
        {"encodings.iso8859_6", 6799136, 2880},
        {"encodings.iso8859_7", 6802016, 2843},
        {"encodings.iso8859_8", 6804859, 2874},
        {"encodings.iso8859_9", 6807733, 2835},
        {"encodings.johab", 6810568, 1735},
        {"encodings.koi8_r", 6812303, 2857},
        {"encodings.koi8_u", 6815160, 2843},
        {"encodings.latin_1", 6818003, 2253},
        {"encodings.mac_arabic", 6820256, 7981},
        {"encodings.mac_centeuro", 6828237, 2904},
        {"encodings.mac_croatian", 6831141, 2912},
        {"encodings.mac_cyrillic", 6834053, 2902},
        {"encodings.mac_farsi", 6836955, 2816},
        {"encodings.mac_greek", 6839771, 2856},
        {"encodings.mac_iceland", 6842627, 2895},
        {"encodings.mac_latin2", 6845522, 4874},
        {"encodings.mac_roman", 6850396, 2873},
        {"encodings.mac_romanian", 6853269, 2913},
        {"encodings.mac_turkish", 6856182, 2896},
        {"encodings.mbcs", 6859078, 1995},
        {"encodings.palmos", 6861073, 3033},
        {"encodings.ptcp154", 6864106, 4857},
        {"encodings.punycode", 6868963, 7911},
        {"encodings.quopri_codec", 6876874, 3552},
        {"encodings.raw_unicode_escape", 6880426, 2175},
        {"encodings.rot_13", 6882601, 3585},
        {"encodings.shift_jis", 6886186, 1767},
        {"encodings.shift_jis_2004", 6887953, 1807},
        {"encodings.shift_jisx0213", 6889760, 1807},
        {"encodings.string_escape", 6891567, 2034},
        {"encodings.tis_620", 6893601, 2905},
        {"encodings.undefined", 6896506, 2556},
        {"encodings.unicode_escape", 6899062, 2123},
        {"encodings.unicode_internal", 6901185, 2149},
        {"encodings.utf_16", 6903334, 5100},
        {"encodings.utf_16_be", 6908434, 1966},
        {"encodings.utf_16_le", 6910400, 1966},
        {"encodings.utf_32", 6912366, 5658},
        {"encodings.utf_32_be", 6918024, 1859},
        {"encodings.utf_32_le", 6919883, 1859},
        {"encodings.utf_7", 6921742, 1859},
        {"encodings.utf_8", 6923601, 1918},
        {"encodings.utf_8_sig", 6925519, 4917},
        {"encodings.uu_codec", 6930436, 4835},
        {"encodings.zlib_codec", 6935271, 4555},
        {"functools", 6939826, 5951},
        {"locale", 6945777, 54920},
        {"quopri", 7000697, 6511},
        {"re", 242314, 13300},
        {"sre_compile", 7007208, 12476},
        {"sre_constants", 7019684, 6162},
        {"sre_parse", 7025846, 19829},
        {"stringprep", 7045675, 14381},
        {"struct", 7060056, 226},
        {"types", 359678, 2682},
        {NULL, 0, 0}
    };

    struct frozen_desc *current = frozen_modules;

    for(;;)
    {
        destination->name = (char *)current->name;
        destination->code = (unsigned char *)&constant_bin[ current->start ];
        destination->size = current->size;

        if (destination->name == NULL) break;

        current += 1;
        destination += 1;
    };
}
