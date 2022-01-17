#!/bin/bash

set -euxo pipefail
WORKSPACE=$(pwd)

WORKDIRS=("${WORKSPACE}/LinuxPBA" "${WORKSPACE}/linux/CLI")
VERSION="Release_x86_64"
OPAL_UNIT=sed-opal.service
EXEDIR=/usr/sbin
MAN_PAGE=sedutil-cli.8
MAN_DIR=/usr/share/man/man8
USR_SYSTEMD=/etc/systemd/system
PKGDIR=debian

export LDFLAGS=-static

COMMIT=$(git rev-parse --short HEAD)

PKG_VERSION=$(git tag --sort=taggerdate | tail -n1)+g"$COMMIT"
#FIXME Cleanup
for WRKDIR in "${WORKDIRS[@]}"
do
	make -j8 -C "$WRKDIR" CONF="${VERSION}" clean
done

rm -rf "*.deb"
sudo rm -rf "${PKGDIR}"

mkdir "${PKGDIR}"
mkdir -p "${PKGDIR}/${EXEDIR}"
mkdir -p "${PKGDIR}/${USR_SYSTEMD}"
mkdir -p "${PKGDIR}/${MAN_DIR}"

cp -rp DEBIAN "${PKGDIR}"
cat > "$PKGDIR/DEBIAN/control" <<EOF
Package: sedutil
Section: utils
Version: $PKG_VERSION
Architecture: amd64
Maintainer: ops-team@kentik.com
Homepage: https://github.com/kentik/sedutil
Description: Forked and locally-modified DTA sedutil-cli package. Statically compiled, should be good for all modern distros. Built from commit $COMMIT.
EOF

for DIR in "${WORKDIRS[@]}"
do
  ( make -j8 -C "$DIR" CONF="${VERSION}" clean ; make -j8 -C "$DIR" CONF="${VERSION}" ) 2>&1 | tee logfile.txt
done

for DIR in "${WORKDIRS[@]}"
do
  cp "${DIR}/dist/${VERSION}/GNU-Linux/"* "${PKGDIR}/${EXEDIR}"
done

cp docs/"${MAN_PAGE}" "${PKGDIR}/${MAN_DIR}"
gzip -9 "${PKGDIR}/${MAN_DIR}/${MAN_PAGE}"
cp "Contrib/${OPAL_UNIT}" "${PKGDIR}/${USR_SYSTEMD}"
chmod 0644 "${PKGDIR}/${USR_SYSTEMD}/${OPAL_UNIT}"

sudo find "${PKGDIR}" -execdir chown 0:0 '{}' +;

dpkg-deb --build "${PKGDIR}"

pkg_ver_arch=$(dpkg-deb --info debian.deb  \
		   | grep -E 'Package|Version|Architecture' \
		   | tr -d ' ' \
		   | tr ':' '=')
for var in "${pkg_ver_arch[@]}"
do
  eval "$var"
done

mv debian.deb "${Package}_${Version}_${Architecture}.deb"
