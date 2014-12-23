#!/bin/bash 
GITVER=`git describe --dirty`
echo "#define GIT_VERSION " \"$GITVER\"
