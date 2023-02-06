#!/bin/bash
verbose=1
DISK_IMAGE_READONLY=1
ISED_AD_HOC_DISTRIBUTION=1

spew () { (( verbose )) && echo "spew: $@" ; }
spew "verbose=${verbose}, spewing"


# Certificates from the same directory as this script
cur=$( cd "$( dirname "${0}" )" && pwd )
spew "cur=${cur}"


spew "${cur}/build_packages.sh"
. "${cur}/build_packages.sh"

spew "${cur}/build_disk_image.sh"
. "${cur}/build_disk_image.sh"
