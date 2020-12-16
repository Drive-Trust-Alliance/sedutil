@echo off
@REM this batch file must execute from one level lower which is CLI directory

cd ..
echo #ifdef PYEXTLIC > ..\License\pyextlic.h
echo #undef PYEXTLIC >> ..\License\pyextlic.h
echo #endif >> ..\License\pyextlic.h

echo #ifdef PYEXTOB > ..\Common\pyextob.h
echo #undef PYEXTOB >> ..\Common\pyextob.h
echo #endif >> ..\Common\pyextob.h


echo #ifdef PYEXTHASH > ..\Common\pyexthash.h
echo #undef PYEXTHASH >> ..\Common\pyexthash.h
echo #endif >>  ..\Common\pyexthash.h
cd CLI
