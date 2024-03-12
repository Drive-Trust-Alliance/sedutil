#!/bin/bash
# verbose=0
verbose=1
DISK_IMAGE_READONLY=1
ISED_AD_HOC_DISTRIBUTION=0

spew () { (( verbose )) && echo "spew: $@" ; }

(( verbose )) && spew "verbose=${verbose}, spewing" || \
    echo "verbose=${verbose}, not spewing"

spew "${BASH_SOURCE[0]}"

# Other scripts from the same directory as this script
export build_sh_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
spew "build_sh_dir=${build_sh_dir}"

export mac_root="$( cd "${build_sh_dir}/.." && pwd )"
spew "mac_root=${mac_root}"

spew "${build_sh_dir}/build_packages.sh"
. "${build_sh_dir}/build_packages.sh"

spew "${build_sh_dir}/build_disk_image.sh"
. "${build_sh_dir}/build_disk_image.sh"
