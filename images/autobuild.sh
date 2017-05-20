#!/bin/bash

# Echo commands and abort on error
set -ex

# Optimal MAKEFLAGS argument if not already defined
if [ -z ${MAKEFLAGS+x} ]; then
    # Add 1 assuming disk IO will block processes from time to time.
    export MAKEFLAGS=$((1 + $(grep processor /proc/cpuinfo | wc -l)))
fi

# Run everything from the path of this script despite how invoked
cd "$( dirname "${BASH_SOURCE[0]}" )"

source conf

# Build buildroot before other tools as it provides the toolchain for linuxpba
# and sedutil-cli.
./getresources
./buildpbaroot

pushd ../LinuxPBA
rm -rf dist build
make CONF=Debug
make CONF=Debug_x86_64
make CONF=Release
make CONF=Release_x86_64
popd

pushd ../linux/CLI
rm -rf dist build
make CONF=Debug_i686
make CONF=Debug_x86_64
make CONF=Release_i686
make CONF=Release_x86_64
popd

# Rescue build
./buildrescue

# Build BIOS images (untested, probably subtly broken)
#./buildbiospba Release
#./buildbiospba Debug

# Build UEFI images
./buildUEFI64 Release
./buildUEFI64 Debug
