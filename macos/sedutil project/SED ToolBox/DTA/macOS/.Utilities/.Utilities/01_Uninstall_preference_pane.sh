#!/bin/bash

# Source Utility_functions.sh from the same directory as this script
dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
. "${dir}/Utility_functions.sh"

beroot

pkill -i "System Preferences"
pkill -i "System Settings"

preference_pane_names=("Self-Encrypting Drives")
preference_pane_dirs=("/Library/PreferencePanes /System/Library/PreferencePanes ${HOME}/Library/PreferencePanes")


for pane in ${preference_pane_names[@]}
do
    for dir in ${preference_pane_dirs[@]}
    do
        rm -rf "${dir}/${pane}.prefPane"
    done
done


exit 0

