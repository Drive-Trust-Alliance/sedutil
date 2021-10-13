# build hash.so for python GUI

cp -pv ../Common/DtaHashPwd.cpp .
cp -pv ../Common/DtaHashPwd.h .
#cp -pv ../Common/DtaConstants.h .
#cp -pv ../Common/DtaDev.h .
#cp -pv ../Common/DtaLexicon.h .
#cp -pv ../Common/DtaStructures.h .
#cp -pv ../Common/DtaResponse.h .
#cp -pv ../Common/pbkdf2/*   .
#cp -pv ../Common/pyexthash.h   .
cp -pv ../Common/hash.h   .
#cp -pv ../Common/   .
#cp -pv ../Common/   .
#cp -pv ../Common/   .
#cp -pv ../Common/   .
#cp -pv ../Common/   .
#cp -pv ../Common/   .
#cp -pv ../Common/log.h  .
#cp -pv ../linux/os.h .
cp -pv ../License/include.h .

rm -f *.o
# for x86_64
cp -pv ../linux/CLI/build/Release_x86_64/GNU-Linux/_ext/cdbdd37b/chash.o .
cp -pv ../linux/CLI/build/Release_x86_64/GNU-Linux/_ext/cdbdd37b/blockwise.o . 
cp -pv ../linux/CLI/build/Release_x86_64/GNU-Linux/_ext/cdbdd37b/hmac.o .
cp -pv ../linux/CLI/build/Release_x86_64/GNU-Linux/_ext/cdbdd37b/pbkdf2.o . 
cp -pv ../linux/CLI/build/Release_x86_64/GNU-Linux/_ext/cdbdd37b/sha1.o .

# /usr/bin/ld: i386 architecture of input file `chash.o' is incompatible with i386:x86-64 output
#cp -pv ../linux/CLI/build/Release_i686/GNU-Linux/_ext/cdbdd37b/chash.o .
#cp -pv ../linux/CLI/build/Release_i686/GNU-Linux/_ext/cdbdd37b/blockwise.o . 
#cp -pv ../linux/CLI/build/Release_i686/GNU-Linux/_ext/cdbdd37b/hmac.o .
#cp -pv ../linux/CLI/build/Release_i686/GNU-Linux/_ext/cdbdd37b/pbkdf2.o . 
#cp -pv ../linux/CLI/build/Release_i686/GNU-Linux/_ext/cdbdd37b/sha1.o .

#cp -pv ../linux/CLI/build/Release_x86_64/GNU-Linux/_ext/cdbdd37b/chash.o .
#cp -pv ../linux/CLI/build/Release_x86_64/GNU-Linux/_ext/cdbdd37b/blockwise.o . 
#cp -pv ../linux/CLI/build/Release_x86_64/GNU-Linux/_ext/cdbdd37b/hmac.o .
#cp -pv ../linux/CLI/build/Release_x86_64/GNU-Linux/_ext/cdbdd37b/pbkdf2.h . 
#cp -pv ../linux/CLI/build/Release_x86_64/GNU-Linux/_ext/cdbdd37b/sha1.o .

#./build/Debug_i686/GNU-Linux/_ext/cdbdd37b/chash.o.d
#./build/Debug_i686/GNU-Linux/_ext/cdbdd37b/chash.o
#./build/Release_i686/GNU-Linux/_ext/cdbdd37b/chash.o.d
#./build/Release_i686/GNU-Linux/_ext/cdbdd37b/chash.o
#./build/Debug_x86_64/GNU-Linux/_ext/cdbdd37b/chash.o.d
#./build/Debug_x86_64/GNU-Linux/_ext/cdbdd37b/chash.o
#./build/Release_x86_64/GNU-Linux/_ext/cdbdd37b/chash.o.d
#./build/Release_x86_64/GNU-Linux/_ext/cdbdd37b/chash.o


#	${OBJECTDIR}/_ext/cdbdd37b/blockwise.o \
#	${OBJECTDIR}/_ext/cdbdd37b/chash.o \
#	${OBJECTDIR}/_ext/cdbdd37b/hmac.o \
#	${OBJECTDIR}/_ext/cdbdd37b/pbkdf2.o \
#	${OBJECTDIR}/_ext/cdbdd37b/sha1.o \

echo "#ifndef PYEXTHASH" > pyexthash.h
echo "#define PYEXTHASH 1"  >> pyexthash.h
echo "#endif" >>  pyexthash.h

#echo "void * __gxx_personality_v0;" >> pyexthash.h

CC=g++ python setup.py build

cp -pv ./build/lib.linux-x86_64-2.7/PyExtHash.so ../py/
cp -pv ./build/lib.linux-x86_64-2.7/PyExtHash.so .


# build x86_64 hash.so

