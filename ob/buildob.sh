# build ob.so for python GUI

cp ../Common/ob.cpp .
cp ../Common/ob.h .
cp ../License/include.h .

echo "#ifndef PYEXTOB" > pyextob.h
echo "#define PYEXTOB 1"  >> pyextob.h
echo "#endif" >>  pyextob.h

python setup.py build

# build x86_64 ob.so

