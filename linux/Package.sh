#!/bin/bash
rm sedutil_LINUX.tgz
strip --strip-debug ./CLI/dist/Release_i686/GNU-Linux-x86/sedutil-cli
strip --strip-unneeded ./CLI/dist/Release_i686/GNU-Linux-x86/sedutil-cli
strip --strip-debug ./CLI/dist/Release_x86_64/GNU-Linux-x86/sedutil-cli
strip --strip-unneeded ./CLI/dist/Release_x86_64/GNU-Linux-x86/sedutil-cli
tar --xform 's,^,sedutil/,' -czf sedutil_LINUX.tgz *.txt TestSuite.sh ./*.txt  -C ./CLI/dist Release_i686/GNU-Linux-x86/sedutil-cli Release_x86_64/GNU-Linux-x86/sedutil-cli