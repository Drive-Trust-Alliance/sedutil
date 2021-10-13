# build py.so for python GUI

#cp ../Common/ob.cpp .
#cp ../Common/ob.h .
#cp ../License/include.h .

echo "#ifndef PYEXTOB" > pyextlic.h
echo "#define PYEXTOB 1"  >> pyextlic.h
echo "#endif" >>  pyextlic.h

echo "void * __gxx_personality_v0;" >> pyextlic.h

CC=g++ python setup.py build

cp -pv ./build/lib.linux-x86_64-2.7/PyExtLic.so ../py/

# build x86_64 ob.so
