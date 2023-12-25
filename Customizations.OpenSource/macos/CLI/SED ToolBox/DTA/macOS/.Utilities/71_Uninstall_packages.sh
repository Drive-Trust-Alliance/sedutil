#!/bin/bash

#  Uninstall_packages.sh
#  Self-Encrypting Box Package
#
#  Created by Scott Marks on 06/16/2017.
#

# Source Utility_functions.sh from the same directory as this script
dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
. "${dir}/Utility_functions.sh"

beroot


# packages are installed in /
cd /

# remove all files
for p in `pkgutil --pkgs | grep -i bright `
do
    pkgutil --only-files --files $p
done | tr '\n' '\0' | 1>/dev/null 2>/dev/null xargs -n 1 -0 sudo rm -f

# remove all directories that are emptied by the above
for p in `pkgutil --pkgs | grep -i bright `
do
    pkgutil --only-dirs --files $p
done | tr '\n' '\0' | 1>/dev/null 2>/dev/null xargs -n 1 -0 sudo rmdir

# forget the packages
for p in `pkgutil --pkgs | grep -i bright `
do
    1>/dev/null 2>/dev/null sudo pkgutil --forget $p
done
