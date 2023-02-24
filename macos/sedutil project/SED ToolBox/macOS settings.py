# -*- coding: utf-8 -*-
from __future__ import unicode_literals

import os.path

#
#
build = defines.get('build', 'build/SED.build/Debug Bonjour Multistart/SED ToolBox.build/DTA')


static = defines.get('static', 'SED ToolBox/DTA')

macOS = os.path.join(build, 'macOS')

format = 'UDZO'

# Volume size
size = defines.get('size', None)

pkg = os.path.join(macOS, 'DTA.pkg')
pkg_name = os.path.basename(pkg)

certificates = os.path.join(build, 'Certificates')
certs_name = os.path.basename(certificates)

utilities = os.path.join(macOS, '.Utilities')
ut_name = os.path.basename(utilities)

resources = os.path.join(macOS, '.resources')
res_name = os.path.basename(resources)

uninstall = os.path.join(macOS, 'Uninstall')
unin_name = os.path.basename(uninstall)


# Files to include
files = [ pkg, uninstall, certificates, utilities, resources ]
images = defines.get('images', os.path.join(resources, 'images'))
icon = os.path.join(images, 'DTA.VolumeIcon.icns')
_icon_name = '.VolumeIcon.icns'
#badge_icon = None



window_rect = ((100, 100), (740, 440))
background = os.path.join(images, 'DTA_folder_background.png')
_bg_name = '.background.png'


arrange_by = None
icon_size = 48
icon_locations = {
    pkg_name   : (600,   0),
    unin_name  : (600, 100),
    certs_name : (600, 200),

    ut_name    : (820, 100),
    res_name   : (820, 200),

    _bg_name   : (999, 100),
    _icon_name : (999, 200)
    }
hide = [
    certs_name,
    ut_name,
    res_name,
    _bg_name,
    _icon_name
]

show_status_bar = False
show_tab_view = False
show_toolbar = False
show_pathbar = False
show_sidebar = False
sidebar_width = 180

# Select the default view; must be one of
#
#    'icon-view'
#    'list-view'
#    'column-view'
#    'coverflow'
#
default_view = 'icon-view'

# General view configuration
show_icon_preview = False

# Set these to True to force inclusion of icon/list view settings (otherwise
# we only include settings for the default view)
include_icon_view_settings = 'auto'
include_list_view_settings = 'auto'

# .. Icon view configuration ...................................................

# arrange_by = None
grid_offset = (0, 0)
grid_spacing = 100
scroll_position = (0, 0)
label_pos = 'bottom' # or 'right'
text_size = 16

# .. List view configuration ...................................................

# Column names are as follows:
#
#   name
#   date-modified
#   date-created
#   date-added
#   date-last-opened
#   size
#   kind
#   label
#   version
#   comments
#
list_icon_size = 16
list_text_size = 12
list_scroll_position = (0, 0)
list_sort_by = 'name'
list_use_relative_dates = True
list_calculate_all_sizes = False,
list_columns = ('name', 'date-modified', 'size', 'kind', 'date-added')
list_column_widths = {
    'name': 300,
    'date-modified': 181,
    'date-created': 181,
    'date-added': 181,
    'date-last-opened': 181,
    'size': 97,
    'kind': 115,
    'label': 100,
    'version': 75,
    'comments': 300,
    }
list_column_sort_directions = {
    'name': 'ascending',
    'date-modified': 'descending',
    'date-created': 'descending',
    'date-added': 'descending',
    'date-last-opened': 'descending',
    'size': 'descending',
    'kind': 'ascending',
    'label': 'ascending',
    'version': 'ascending',
    'comments': 'ascending',
    }

# .. License configuration .....................................................

# Text in the license configuration is stored in the resources, which means
# it gets stored in a legacy Mac encoding according to the language.  dmgbuild
# will *try* to convert Unicode strings to the appropriate encoding, *but*
# you should be aware that Python doesn't support all of the necessary encodings;
# in many cases you will need to encode the text yourself and use byte strings
# instead here.

# Recognized language names are:
#
#  af_ZA, ar, be_BY, bg_BG, bn, bo, br, ca_ES, cs_CZ, cy, da_DK, de_AT, de_CH,
#  de_DE, dz_BT, el_CY, el_GR, en_AU, en_CA, en_GB, en_IE, en_SG, en_US, eo,
#  es_419, es_ES, et_EE, fa_IR, fi_FI, fo_FO, fr_001, fr_BE, fr_CA, fr_CH,
#  fr_FR, ga-Latg_IE, ga_IE, gd, grc, gu_IN, gv, he_IL, hi_IN, hr_HR, hu_HU,
#  hy_AM, is_IS, it_CH, it_IT, iu_CA, ja_JP, ka_GE, kl, ko_KR, lt_LT, lv_LV,
#  mk_MK, mr_IN, mt_MT, nb_NO, ne_NP, nl_BE, nl_NL, nn_NO, pa, pl_PL, pt_BR,
#  pt_PT, ro_RO, ru_RU, se, sk_SK, sl_SI, sr_RS, sv_SE, th_TH, to_TO, tr_TR,
#  uk_UA, ur_IN, ur_PK, uz_UZ, vi_VN, zh_CN, zh_TW

# license = {
#     'default-language': 'en_US',
#     'licenses': {
#         # For each language, the text of the license.  This can be plain text,
#         # RTF (in which case it must start "{\rtf1"), or a path to a file
#         # containing the license text.  If you're using RTF,
#         # watch out for Python escaping (or read it from a file).
#         'English': b'''{\\rtf1\\ansi\\ansicpg1252\\cocoartf1504\\cocoasubrtf820
# {\\fonttbl\\f0\\fnil\\fcharset0 Helvetica-Bold;\\f1\\fnil\\fcharset0 Helvetica;}
# {\\colortbl;\\red255\\green255\\blue255;\\red0\\green0\\blue0;}
# {\\*\\expandedcolortbl;;\\cssrgb\\c0\\c0\\c0;}
# \\paperw11905\\paperh16837\\margl1133\\margr1133\\margb1133\\margt1133
# \\deftab720
# \\pard\\pardeftab720\\sa160\\partightenfactor0

# \\f0\\b\\fs60 \\cf2 \\expnd0\\expndtw0\\kerning0
# \\up0 \\nosupersub \\ulnone \\outl0\\strokewidth0 \\strokec2 Test License\\
# \\pard\\pardeftab720\\sa160\\partightenfactor0

# \\fs36 \\cf2 \\strokec2 What is this?\\
# \\pard\\pardeftab720\\sa160\\partightenfactor0

# \\f1\\b0\\fs22 \\cf2 \\strokec2 This is the English license. It says what you are allowed to do with this software.\\
# \\
# }''',
#     },
#     'buttons': {
#         # For each language, text for the buttons on the licensing window.
#         #
#         # Default buttons and text are built-in for the following languages:
#         #
#         #   English (en_US), German (de_DE), Spanish (es_ES), French (fr_FR),
#         #   Italian (it_IT), Japanese (ja_JP), Dutch (nl_NL), Swedish (sv_SE),
#         #   Brazilian Portuguese (pt_BR), Simplified Chinese (zh_CN),
#         #   Traditional Chinese (zh_TW), Danish (da_DK), Finnish (fi_FI),
#         #   Korean (ko_KR), Norwegian (nb_NO)
#         #
#         # You don't need to specify them for those languages; if you fail to
#         # specify them for some other language, English will be used instead.

#         'en_US': (
#             b'English',
#             b'Agree',
#             b'Disagree',
#             b'Print',
#             b'Save',
#             b'If you agree with the terms of this license, press "Agree" to '
#             b'install the software.  If you do not agree, press "Disagree".'
#         ),
#     },
# }
