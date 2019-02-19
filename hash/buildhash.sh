# build hash.so for python GUI

cp -pv ../Common/DtaHashPwd.cpp .
cp -pv ../Common/DtaHashPwd.h .
cp -pv ../Common/DtaConstants.h .
cp -pv ../Common/DtaDev.h .
cp -pv ../Common/DtaLexicon.h .
cp -pv ../Common/DtaStructures.h .
cp -pv ../Common/DtaResponse.h .
cp -pv ../Common/pbkdf2/*   .
cp -pv ../Common/pyexthash.h   .
#cp -pv ../Common/   .
#cp -pv ../Common/   .
#cp -pv ../Common/   .
#cp -pv ../Common/   .
#cp -pv ../Common/   .
#cp -pv ../Common/   .
#cp -pv ../Common/   .
cp -pv ../Common/log.h  .
cp -pv ../linux/os.h .
cp -pv ../License/include.h .

echo "#ifndef PYEXTHASH" > pyexthash.h
echo "#define PYEXTHASH 1"  >> pyexthash.h
echo "#endif" >>  pyexthash.h

echo "void * __gxx_personality_v0;" >> pyexthash.h

CC=g++ python setup.py build

cp -pv ./build/lib.linux-x86_64-2.7/PyExtHash.so ../py/

# build x86_64 hash.so

