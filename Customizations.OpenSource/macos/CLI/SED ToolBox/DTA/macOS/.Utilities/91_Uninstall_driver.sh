#!/bin/bash

# Source Utility_functions.sh from the same directory as this script
dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
. "${dir}/Utility_functions.sh"

beroot

old_ext_names="SCSIPassThrough SCSIPassThroughDriver"
new_ext_names="BrightPlazaTPer"

must_reboot="no"

for ext_name in ${old_ext_names} ${new_ext_names}
do
    if ! ( >/dev/null 2>&1  kmutil unload -b com.brightplaza.${ext_name} )
    then
        must_reboot="yes"
    fi
done


if ! ( >/dev/null 2>&1 kmutil check )
then
    must_reboot="yes"
fi


extension_dirs="/Library/Extensions /System/Library/Extensions"

for dir in ${extension_dirs}
do
    for ext_name in ${old_ext_names} ${new_ext_names}
    do
        ext=${dir}/${ext_name}.kext
        rm -rf ${ext}
    done
done


if [ "${must_reboot}"=="yes" ]
then
    reboot
else
    echo Apparently no need to reboot '¯\_(ツ)_/'
    exit 0
fi
