#!/bin/bash

# set -o verbose
# set -o xtrace

# Source Utility_functions.sh from the same directory as this script
. "$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )/Utility_functions.sh"

beroot

# do something smarter than just expecting SEDAccess.app and com.brightplaza.SEDAccess.plist to be in the current dir
cp -r SEDAccess.app  /usr/local/libexec/SEDAccess.app
cp com.brightplaza.SEDAccess.plist /Library/LaunchAgents/
launchctl load /Library/LaunchAgents/com.brightplaza.SEDAccess.plist

exit 0
