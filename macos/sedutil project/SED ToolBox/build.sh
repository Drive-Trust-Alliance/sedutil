#!/bin/bash
verbose=1
DISK_IMAGE_READONLY=1
ISED_AD_HOC_DISTRIBUTION=1

spew () { (( verbose )) && echo "spew: $@" ; }
spew "verbose=${verbose}, spewing"

# Other scripts from the same directory as this script
export build_sh_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
spew "build_sh_dir=${build_sh_dir}"

export mac_root="$( cd "${build_sh_dir}/.." && pwd )"
spew "mac_root=${mac_root}"

spew "${build_sh_dir}/build_packages.sh"
. "${build_sh_dir}/build_packages.sh"

spew "${build_sh_dir}/build_disk_image.sh"
. "${build_sh_dir}/build_disk_image.sh"
