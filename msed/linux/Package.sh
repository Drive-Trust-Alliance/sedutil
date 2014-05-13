#!/bin/bash
rm msed_LINUX.tgz
strip --strip-debug ../../dist/Release/GNU-Linux-x86/msed
strip --strip-unneeded ../../dist/Release/GNU-Linux-x86/msed
tar --xform 's,^,msed/,' -czf msed_LINUX.tgz *.txt *.sh ../*.txt  -C ../../dist/Release/GNU-Linux-x86/ msed