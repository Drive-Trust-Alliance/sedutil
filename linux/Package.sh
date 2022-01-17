#!/bin/bash
rm sedutil_LINUX.tgz
cd ..
mkdir -p ./linux/CLI/dist/Release_i686/
mkdir -p ./linux/CLI/dist/Release_x86_64/
autoreconf -i
./configure --enable-silent-rules
make CFLAGS='-m32 -O2' CXXFLAGS='-m32 -O2' all
cp sedutil-cli ./linux/CLI/dist/Release_i686/
make clean
make CFLAGS='-m64 -O2' CXXFLAGS='-m64 -O2' all
cp sedutil-cli ./linux/CLI/dist/Release_x86_64/
strip --strip-debug ./linux/CLI/dist/Release_i686/sedutil-cli
strip --strip-unneeded ./linux/CLI/dist/Release_i686/sedutil-cli
strip --strip-debug ./linux/CLI/dist/Release_x86_64/sedutil-cli
strip --strip-unneeded ./linux/CLI/dist/Release_x86_64/sedutil-cli
cd linux
tar --xform 's,^,sedutil/,' -czf sedutil_LINUX.tgz *.txt TestSuite.sh ./*.txt ../docs/* -C ./CLI/dist Release_i686/sedutil-cli Release_x86_64/sedutil-cli
