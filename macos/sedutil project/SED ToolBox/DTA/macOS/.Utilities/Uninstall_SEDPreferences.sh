#!/bin/bash

# set -o verbose
# set -o xtrace

# Source Utility_functions.sh from the same directory as this script
. "$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )/Utility_functions.sh"

>/dev/null 2>&1 rm -rf "~/Library/PreferencePanes/Self-Encrypting Drives.prefPane"

beroot

>/dev/null 2>&1 rm -rf '/Library/PreferencePanes/Self-Encrypting Drives.prefPane'

pkill -i "System Preferences"

exit 0
