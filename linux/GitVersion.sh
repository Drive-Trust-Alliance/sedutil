#!/bin/sh
#set -x
echo "#define GIT_VERSION \"$(command -v git >/dev/null && git describe --dirty 2>/dev/null || echo tarball)\""
