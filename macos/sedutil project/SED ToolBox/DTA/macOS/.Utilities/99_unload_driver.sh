#!/bin/bash
echo /Users/scott/Drive Trust Alliance/DTA/submodules/FH/sedutil/macos/sedutil project/SED ToolBox/DTA/macOS/.Utilities/99_unload_driver.sh

# Source Utility_functions.sh from the same directory as this script
dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
. "${dir}/Utility_functions.sh"

beroot

if ! ( >/dev/null 2>&1  kmutil unload -b com.brightplaza.BrightPlazaTPer )
then
    must_reboot="yes"
else
    must_reboot="no"
fi

>/dev/null 2>&1 kmutil check

if [ "$must_reboot"=="yes" ]
then
    # reboot
    echo SHOULD REBOOT, but ... no
    exit 0
else
    exit 0
fi
