#!/bin/bash 
# set -x

if which -s git
then
    GITVER=`git describe --dirty`
else
    GITVER=tarball
fi

configuration="$(${mac_root}/extract_configuration_from_Xcode_build_environment)"
if [ "${configuration}" == "Debug" ]
then
    GITVER="${GITVER} (Debug build)"
fi

echo "#define GIT_VERSION \"${GITVER}\""
