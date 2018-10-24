# build ob.so for python GUI

cp -pv ../Common/ob.cpp .
cp -pv ../Common/ob.h .
cp -pv ../License/include.h .

echo "#ifndef PYEXTOB" > pyextob.h
echo "#define PYEXTOB 1"  >> pyextob.h
echo "#endif" >>  pyextob.h

echo "void * __gxx_personality_v0;" >> pyextob.h

CC=g++ python setup.py build

cp -pv ./build/lib.linux-x86_64-2.7/PyExtOb.so ../py/

# build x86_64 ob.so

