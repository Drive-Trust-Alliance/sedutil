#!/bin/bash

# Source Utility_functions.sh from the same directory as this script
. "$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )/Utility_functions.sh"

beroot

app_dirs="/Applications /Applications/Utilities ${HOME}/Applications ${HOME}/Applications/Utilities"

old_app_names="SEDDaemonClient"
new_app_names="SEDDaemonTest SEDPreferencesTest"

for app_name in ${old_app_names} ${new_app_names}
do
    for dir in ${app_dirs}
    do
        rm -rf ${dir}/${app_name}.app
    done
done    

    
exit 0    
