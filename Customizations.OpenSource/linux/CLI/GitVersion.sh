#!/bin/bash
# set -x

GIT="$(which git)"

if [ -n "${GIT}" ]
then
    GITVER="$("${GIT}" describe --dirty)"
else
    GITVER=tarball
fi

echo "#define GIT_VERSION \"${GITVER}\""
