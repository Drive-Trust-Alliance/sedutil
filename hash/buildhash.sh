# build hash.so for python GUI

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

beroot

cp -pv ../Common/{DtaHashPassword,DtaOptions,PyDtaHashPassword,includePython}.* ./

2>/dev/null pip install .

rm -rf {DtaHashPassword,DtaOptions,PyDtaHashPassword,includePython}.* build

# build x86_64^H^H^H^H^H^Huniversal PyExtHash.so
