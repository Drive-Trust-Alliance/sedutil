#!/bin/bash

# Source Utility_functions.sh from the same directory as this script
dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
. "${dir}/Utility_functions.sh"

beroot


extension_dirs="/Library/Extensions /System/Library/Extensions"

old_ext_names="SCSIPassThrough SCSIPassThroughDriver"
new_ext_names="BrightPlazaTPer"

for ext_name in ${old_ext_names} ${new_ext_names}
do
    for dir in ${extension_dirs}
    do
        2>/dev/null kmutil unload --class-name com.brightplaza.${ext_name}
        ext=${dir}/${ext_name}.kext
        rm -rf ${ext}
    done
done
