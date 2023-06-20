#!/bin/bash

# Source Utility_functions.sh from the same directory as this script
dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
. "${dir}/Utility_functions.sh"

beroot

framework_names="            \
log                          \
cifra                        \
oui                          \
SEDKernelInterface           \
sedutil                      \
SedClientServerCommunication \
Python                       \
Python3                      \
"
framework_dirs="\
/Library/Frameworks        \
/System/Library/Frameworks \
${HOME}/Library/Frameworks \
"


for framework in ${framework_names}
do
    for dir in ${framework_dirs}
    do
        rm -rf ${dir}/${framework}.framework
    done
done


exit 0
