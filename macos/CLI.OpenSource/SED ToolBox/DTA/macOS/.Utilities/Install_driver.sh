#!/bin/bash

# set -o verbose
# set -o xtrace

# Source Utility_functions.sh from the same directory as this script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

. "$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )/Utility_functions.sh"

beroot

cp -r "${DIR}/../Products/bundles/Library/Extensions/BrightPlazaTPer.kext" \
      /Library/Extensions/BrightPlazaTPer.kext

reboot
