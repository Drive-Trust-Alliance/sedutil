#!/bin/bash
# echo Original source for this script was /Users/scott/Drive Trust Alliance/DTA/submodules/FH/sedutil/macos/sedutil project/SED ToolBox/DTA/macOS/.Utilities/99_unload_driver.sh

# Source Utility_functions.sh from the same directory as this script
dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
. "${dir}/Utility_functions.sh"

beroot

local -i must_reboot

must_reboot="no"

if ! ( >/dev/null 2>&1  kmutil unload -b com.brightplaza.BrightPlazaTPer )
then
    must_reboot="yes"
fi

if ! ( >/dev/null 2>&1 kmutil check )
then
    must_reboot="yes"
fi


if [ "$must_reboot"=="yes" ]
then
    reboot
else
    echo Apparently no need to reboot '¯\_(ツ)_/'
    exit 0
fi
