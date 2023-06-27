set -xv
# build hash.so for python GUI

cp -pv ../Common/{DtaHashPwd,DtaOptions,PyDtaHashPassword,includePython}.* ./

echo "#ifndef PYEXTHASH" > pyexthash.h
echo "#define PYEXTHASH"  >> pyexthash.h
echo "#endif" >>  pyexthash.h

#echo "void * __gxx_personality_v0;" >> pyexthash.h

CC=g++ python3 setup.py build

os="$(/usr/bin/uname -s)"
case "${os}" in
    linux*)
	cp -pv ./build/lib.linux*/PyExtHash.so ../PyExtHash.so
	cp -pv ./build/lib.linux*/PyExtHash.so ./PyExtHash.so
	;;

    Darwin)
	cp -pv ./build/lib.macos*/PyExtHash*.so ../py/PyExtHash.so
	cp -pv ./build/lib.macos*/PyExtHash*.so ./PyExtHash.so
	;;

    *)
	1>&2 echo "Don't know how to handle OS '${os}'"
	;;
esac

# build x86_64^H^H^H^H^H^Huniversal PyExtHash.so
