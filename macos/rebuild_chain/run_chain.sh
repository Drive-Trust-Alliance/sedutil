#!/bin/bash
set -xv
export CHAIN_LINKS_LIB="${CHAIN_LINKS_LIB-/Users/scott/bin/chain_links}" # TODO: Explicit username because this is run by root effectively as sudo -s $USER so HOME is still /var/root


authenticate


export CHAIN_DIR="$(realpath "$(dirname "$(realpath "${BASH_SOURCE-$0}")")")"
export CHAIN_AUTOSTARTS=no
export CHAIN_AUTORUNS=yes
export CHAIN_IS_CIRCULAR=no

source "${CHAIN_LINKS_LIB}/utils"

tracer "Entering ${CHAIN_DIR}/run_chain.sh"

# The loop idea below won't work because all the individual `run_script` invocations terminate
# with success as soon as `osascript` fires off finishes its `tell app "Terminal"`
# while step_chain ; do tracer "Step_chain result=$?" ; done

# Hence we assume that each step must be able to invoke the next with e.g. `exec`
step_chain ; tracer "Step_chain result=$?"

tracer "Exiting ${CHAIN_DIR}/run_chain.sh"
