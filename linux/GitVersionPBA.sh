#!/bin/bash 
GITVERPBA=`git describe --dirty=-derive`
echo "#define GIT_VERSION_PBA " \"$GITVERPBA\"
