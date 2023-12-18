#!/bin/bash 

#set -xv

function possibly_eject {
    local target="${1}"
    [ -d "${target}" ] && diskutil eject "${target}"
}
export -f possibly_eject


function maybe_attach {
    [ -f "${1}" ]  &&  hdiutil attach -quiet "${@}"
}
export -f maybe_attach


function persistently_require {
    local target="${1}"
    shift
 
   [ -d "${target}" ] && fail 1 "${target} already exists, could not move aside."

    maybe_attach "$@" || fail 2 "Could not find ${target} and could not attach ${@}"

   [ -d "${target}" ] || fail 3 "Attached $@ but still could not find ${target}"
}
export -f persistently_require
