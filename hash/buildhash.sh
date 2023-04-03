set -xv
# build hash.so for python GUI

cp -pv ../Common/{DtaHashPwd,DtaOptions}.* ./

echo "#ifndef PYEXTHASH" > pyexthash.h
echo "#define PYEXTHASH"  >> pyexthash.h
echo "#endif" >>  pyexthash.h

#echo "void * __gxx_personality_v0;" >> pyexthash.h

CC=g++ python setup.py build

os="$(/usr/bin/uname -s)"
case "${os}" in
    linux*)
	cp -pv ./build/lib.linux*/PyExtHash.so ../py/
	cp -pv ./build/lib.linux*/PyExtHash.so .
	;;

    Darwin)
	cp -pv ./build/lib.macos*/PyExtHash.so ../py/
	cp -pv ./build/lib.macos*/PyExtHash.so .
	;;

    *)
	1>&2 echo "Don't know how to handle OS '${os}'"
	;;
esac

# build x86_64 hash.so
