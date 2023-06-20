#!/bin/bash

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
    reboot
else
    exit 0
fi
