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
