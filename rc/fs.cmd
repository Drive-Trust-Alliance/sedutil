@REM Author : Jerry Hwang
@REM Organization : Fidelity Height
@REM Date : 12/15/2020
@REM Revision History :
@REM 1.0 : 12/15/2020 initial version
@REM 1.1 : 

@echo off
@REM set sedutil-cli.exe size of Release Debug x86 x64 of 
@REM fs.exe %1 %2 
@REM %1 is win32 or x64
@REM %2 is release or Debug
@REM if %1 is all then build all configuration 

if "%1"=="/h" (
	goto L1
)
if "%1"=="/help" (
	goto L1
)
if "%1"=="-h" (
	goto L1
)
if "%1"=="--help" (
	goto L1
)
if "%1"=="" (
	goto L1
)

if "%1"=="win32" (
	if "%2"=="Release" (
		goto L2
	)
	if "%2"=="Debug" (
		goto L2
	)
	goto L1
)
if "%1"=="x64" (
	if "%2"=="Release" (
		goto L2
	)
	if "%2"=="Debug" (
		goto L2
	)
	@echo *******************************
	@echo config is not Release or Debug
	@echo *******************************
	goto L1
)
@echo *******************************
@echo Platform is not x64 or win32
@echo *******************************
goto L1



goto L2

:L1
	@echo usage : %0 x64/win32 Release/Debug
	@echo example : %0 x64 Release
	@echo           %0 win32 Release
	@echo           %0 x64 Debug
	@echo           %0 win32 Debug
	goto end

:L2


set P1=%1
set P2=%2
goto exec

:exec
fs.exe ..\windows\CLI\%P1%\%P2%\sedutil-cli.exe 
dir ..\windows\CLI\%P1%\%P2%\sedutil-cli.exe
type ..\Common\sedsize.h
:end
@REM end of fs.exe UEFI 
@echo ********************************************************
@echo * end of build fs.cmd platform config                  *
@echo ********************************************************


