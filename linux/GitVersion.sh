#!/bin/sh
#set -x
if command -v git >/dev/null; then
	echo "#define GIT_VERSION  \"$(git describe --dirty)\""
else
	echo '#define GIT_VERSION "tarball"'
fi
