#!/usr/bin/env bash
set -ex

#FIXME Cleanup
for WRKDIR in $WORKDIRS
do
	( cd $WRKDIR; make CONF=${VERSION} clean ) || /bin/true
done

rm -rf debian/usr || /bin/true
mkdir -p debian/usr/sbin || /bin/true

sudo apt-get install -y build-essential fakeroot devscripts libc6-dev-i386 gcc-multilib g++-multilib openjdk-8-jdk-headless
