strnum () {
    local n="${1}"
    local sign='[+-]?'   
    local zero='0'
    local hex='0[Xx][[:xdigit:]]+'
    local oct='0[0-7]+'
    local dec='[1-9][[:digit:]]*'
    local bdigs='[[:alnum:]@_]+'
    local -i base=0;
    [[ ":${n}:" =~ ^:($sign)(($zero)|($hex)|($oct)|($dec)):$ ]] || ( [[ ":${n}:" =~ ^:($sign)($dec)[#]($bdigs)($sentinel):$ ]] && base=$(( ${BASH_REMATCH[2]} )) && (( 2 <= $base && $base <= 64 )) ) && echo $(( ${n} )) 2> /dev/null
}
        
call_site_info () {  
    local file='<stdin>' 
    local -i line=0 
    local func='<top-level>' 
    local fail_or_die_seen=false 
    local -i i 
    local -i N="${#FUNCNAME[@]}" 
    (( "${N}" == "${#BASH_LINENO[@]}" )) || { echo Internal error. 1>&2 ; exit 127 ; }        
    for ((i=1 ; i<N ; i++))
    do
        local this_file="${BASH_SOURCE[$i+1]}" 
        if [[ -n "${this_file}" ]]
        then
            file="${this_file}" 
            line="${BASH_LINENO[$i]}" 
        fi 
        func="${FUNCNAME[$i]}";
        [[ "${func}" =~ (fail|die) ]] || break;
        [[ "${func}" =~ source ]] && func="$(/usr/bin/basename "${BASH_SOURCE[$i]}")" 
    done 
    [[ -n "${file}" ]] || file='<stdin>' 
    [[ -n "${func}" ]] || func='<top-level>' 
    local -a info=("${file}" "${line}" "${func}") 
    echo "${info[@]}"
}

fail () {
    local -i return_value
    return_value=$(strnum "$1") && shift || return_value=1
    local -a info=($(set -f; call_site_info))
    local file="${info[0]}"
    local line="${info[1]}"
    local func="${info[2]}"
    [[ -n "${line}" ]] && (( ${line} !=0 )) && line=":${line}" || line=''
    if (( $return_value != 0 ))
    then
        local functag=''
        [[ -n "${func}" ]] && functag="::${func}"
        echo "${file}${line}${functag}: $*" 1>&2
        [ -n "$usage" ] && echo "Usage: ${func} ${usage}" 1>&2
    else
        echo "$*" 1>&2
    fi
    return $return_value
}

isroot () { [[ $EUID -eq 0 ]] ; }

notroot () { ! isroot ; }

can_sudo () { (sudo -n true 2>/dev/null) ; }

enable_sudo ()
{
    if ( notroot ) && ( ! ( can_sudo ) )
    then
        echo You are about to be prompted for your password so that this script can execute some commands as \'root\'.
        echo You can type control+c to bail out of this script with nothing changed.
    fi
    sudo true
}

beroot ()
{
    if notroot # not executing as root
    then
        if ! can_sudo
        then
	    echo You are about to be prompted for your password so that this script can execute some commands as \'root\'.
            echo You can type control+c to bail out of this script with nothing changed.
        fi
#         exec sudo "$0" "$@" # restart this script as root
        exec sudo "$0" "$@" 1>>/tmp/beroot.out 2>>/tmp/beroot.err # TODO: debugging
    fi
}

reboot ()
{
    echo "This script needs to reboot to complete its work."
    echo "Type control-c within ten seconds to avoid rebooting."
    # trap ctrl-c and call ctrl_c()
    trap ctrl_c INT

    function ctrl_c()
    {
        echo -e "\nWill not reboot now -- be sure to reboot later."
        exit 1
    }

    for i in `seq 10 -1 1`
    do
        echo -ne "$i "
        sleep 1
    done
    echo  "0 ... rebooting ..."
    /sbin/reboot
}
## /Users/scott/Drive Trust Alliance/DTA/submodules/FH/sedutil/macos/SED ToolBox/DTA/macOS/.Utilities/Utility_functions.sh
