#!/bin/bash 
GITVER=`git describe --dirty=-derive`
echo "#define GIT_VERSION " \"$GITVER\"

