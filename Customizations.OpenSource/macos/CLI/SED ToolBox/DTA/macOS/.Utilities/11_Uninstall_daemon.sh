#!/bin/bash

# Source Utility_functions.sh from the same directory as this script
dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
. "${dir}/Utility_functions.sh"

beroot

launch_daemon_dirs="/Library/LaunchDaemons /System/Library/LaunchDaemons ${HOME}/Library/LaunchDaemons"
daemon_executable_dirs="/usr/libexec /usr/local/libexec"
old_app_names="seddaemon"
new_app_names="sedd"


for app_name in ${old_app_names} ${new_app_names}
do
    for dir in ${launch_daemon_dirs}
    do
        plist=${dir}/com.brightplaza.${app_name}.plist
        2>/dev/null launchctl unload ${plist}
        rm -f ${plist}
    done

    PID="$(ps aux | grep ${app_name} | grep -v grep | awk '{print $2}')"
    if [ -n "${PID}" ]
    then
        kill -9 "${PID}"
    fi

    for dir in ${daemon_executable_dirs}
    do
        rm -rf ${dir}/${app_name}
    done
done

exit 0
