#!/bin/bash 
# set -x

if which -s git
then
    GITVER=`git describe --dirty`
else
    GITVER=tarball
fi

configuration="$( echo "${BUILT_PRODUCTS_DIR}" | cut -c "$(expr ${#BUILD_ROOT} + 2)-${#BUILT_PRODUCTS_DIR}" | awk '{print $1}' )"
if [ "${configuration}" == "Debug" ]
then
    GITVER="${GITVER} (Debug build)"
fi

echo "#define GIT_VERSION \"${GITVER}\""
