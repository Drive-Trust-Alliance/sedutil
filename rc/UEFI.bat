@REM Author : Jerry Hwang
@REM Organization : Fidelity Height
@REM Date : 8/9/2018
@REM Revision History :
@REM 1.0 : 01/02/2018 initial version
@REM 1.1 : 08/09/2018 generate sedutil-cli.exe if %1 is not all 

@echo off
@REM build Release Debug x86 x64 
@REM UEFI %1 %2 
@REM %1 is x86 or x64
@REM %2 is release or Debug
@REM if %1 is all then build all configuration 

if "%1"=="" (
    set P1=x64
	set P2=Debug
	call :exec64
	goto end
)

if "%1"=="all" (
	set P1=x64
	set P2=Debug
	call :exec64
	set P2=Release
	call :exec64
	set P1=x86
	set P2=Debug
	set P3=Win32
	call :exec86
	set P2=Release
	call :exec86
	goto end
)

:L2
if "%2"=="Release" (
	set P2=Release
	goto L3
)
if "%2"=="Debug" (
	set P2=Debug
	goto L3
) else (
	goto er2
)

:L3


if "%1"=="x86" (
	@echo %1 
	set P1=x86
	set P3=Win32
	goto exec86
)
if "%1"=="x64" (
	@echo %1
	set P1="%1"
	goto exec64
) else (
	goto er1
)



:L2

set P2=%2
goto exec


:er0
	@echo "No command parameter error, use UEFI x86 release ......"
	goto end

:er1
	@echo "unknown command parameter 1 error, use UEFI x86 release ......"
	goto end
	
:er2
	@echo "unknown command parameter 2 error, use UEFI x86 release ......"
	goto end
	



:exec64
@copy ..\windows\CLI\%P1%\%P2%\sedutil-cli.exe sedutil_%P1%_%P2%.exe
@copy /b  sedutil_%P1%_%P2%.exe + UEFI64.zip sedutil-cli_%P1%_%P2%.exe
if "%1"=="all" ( goto end ) 
if "%1"=="" (
@copy sedutil-cli_%P1%_%P2%.exe sedutil-cli.exe
@copy sedutil_%P1%_%P2%.exe sedutil.exe
goto end
)
if "%1"=="x64" (
@copy sedutil-cli_%P1%_%P2%.exe sedutil-cli.exe
@copy sedutil_%P1%_%P2%.exe sedutil.exe
goto end
)
goto end

:exec86
@echo %P1% %P2% %P3%
@copy ..\windows\CLI\%P3%\%P2%\sedutil-cli.exe sedutil_%P1%_%P2%.exe
@copy /b  sedutil_%P1%_%P2%.exe + UEFI64.zip sedutil-cli_%P1%_%P2%.exe
if "%1"=="x86" (
@copy sedutil-cli_%P1%_%P2%.exe sedutil-cli.exe
@copy sedutil_%P1%_%P2%.exe sedutil.exe
goto end
)
goto end
:exit 




:end
@REM end of build UEFI 
@echo ********************************************************
@echo * end of build UEFI.BAT platform config                *
@echo ********************************************************


