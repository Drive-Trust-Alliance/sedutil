#!/bin/bash

# set -o verbose
# set -o xtrace

# Source Utility_functions.sh from the same directory as this script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
. "${DIR}/Utility_functions.sh"



beroot

cp -r "${DIR}/../Products/files/usr/local/lib/libKernInterface.dylib" /usr/local/lib/libKernInterface.dylib
cp -r "${DIR}/../Products/files/usr/local/lib/libmsed.dylib" /usr/local/lib/libmsed.dylib
cp -r "${DIR}/../Products/files/usr/local/libexec/sedd" /usr/local/libexec/sedd


cp -r "${DIR}/../Products/files/Library/LaunchDaemons/com.brightplaza.sedd.plist" \
      /Library/LaunchDaemons/com.brightplaza.sedd.plist
launchctl load /Library/LaunchDaemons/com.brightplaza.sedd.plist
