#!/bin/bash

# Source Utility_functions.sh from the same directory as this script
dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
. "${dir}/Utility_functions.sh"

beroot


library_names="KernInterface msed SedKernel"
library_dirs="/usr/lib /usr/local/lib"


for lib in ${library_names}
do
    for dir in ${library_dirs}
    do
        rm -rf ${dir}/lib${lib}.dylib
    done
done


exit 0
