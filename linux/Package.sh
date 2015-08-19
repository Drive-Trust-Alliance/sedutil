#!/bin/bash
rm msed_LINUX.tgz
strip --strip-debug ../../dist/Release_i686/msed
strip --strip-unneeded ../../dist/Release_i686/msed
strip --strip-debug ../../dist/Release_x86_64/msed
strip --strip-unneeded ../../dist/Release_x86_64/msed
tar --xform 's,^,msed/,' -czf msed_LINUX.tgz *.txt *.sh ../*.txt  -C ../../dist Release_i686/msed Release_x86_64/msed