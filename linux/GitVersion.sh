#!/bin/bash 
#set -x
GIT=`which git`
if [ "x"${GIT} == "x" ]; then
	echo "#define GIT_VERSION \"tarball\""
else
	GITVER=`git describe --dirty`
	echo "#define GIT_VERSION " \"$GITVER\"
fi
