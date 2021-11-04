@echo OFF 
@REM Fidelity Height LLC code copyright 2018,2019,2020, 2021 signing script file
@REM date 4/30/2020
@REM sign exe file with password certificate
@REM 12/24/2020
@REM replace comodoca timestamp server with symantec 
@REM verify if file has been signed , if so skip signing , otherwise get error for double signing
@REM verify signed exe
@sign-32bit\signtool verify /q /pa %1  > nul
@if %errorlevel% == 0 (
	@REM @echo "code has been signed previously, Remove the signature and resign again "
	@sign-32bit\signtool remove /q /s %1 > nul
	@goto end
)
@REM @echo "sign the code"
@REM /fd sha256 /tr http://timestamp.comodoca.com/?td=sha256 /td sha256 
@REM signtool sign -f "C:\Dropbox\1Note\codesigning\cert\FidelityHeightLLC-nopass.p12" /fd sha256 /tr http://timestamp.comodoca.com/authenticode?td=sha256 /td sha256 %1
@REM sign-32bit\signtool.exe sign -f "C:\Dropbox\1SED\codesigning\cert\FidelityHeightLLC2019.p12"  /fd sha256 /tr http://timestamp.comodoca.com/authenticode?td=sha256 /td sha256 /p Opal2019Tool %1
@set CERT="\\Dropbox\\1SED\\codesigning\\cert\\FidelityHeightLLC2020.p12"
@for %%F in (C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z) do ( 
@REM echo %%F 
@REM echo %%F:%CERT%
@if exist %%F:%CERT% (
@REM @echo Certificate file exist %%F:%CERT% 
@set DRIVE="%%F:%CERT%"
@REM @echo %DRIVE%
@goto next 
)
)
:nocertificat
@if "%Drive%" == "" (
@echo No certificate found, set default to C: drive"
@set Drive="C:%FILE%"
@echo %Drive%
)
:next
REM original sign-32bit\signtool.exe sign -f %DRIVE% /fd sha256 /tr http://timestamp.comodoca.com/authenticode?td=sha256 /td sha256 /p Opal2019Tool %1
REM http://sha256timestamp.ws.symantec.com/sha256/timestamp
REM http://timestamp.verisign.com/scripts/timstamp.dll
REM comodo timestamp server seem problematic.  use symantec time stamp seems OK
@sign-32bit\signtool.exe sign /q -f %DRIVE% /fd sha256 /tr http://sha256timestamp.ws.symantec.com/sha256/timestamp/authenticode?td=sha256 /td sha256 /p Opal2019Tool %1 > nul
@if %errorlevel% == 0 (
	@REM @echo "Code signing OK, verify if it really ok"
	@goto L1
)
@echo "Code signing error. exit" 
@goto end
:L1
@REM verify signed exe
@sign-32bit\signtool verify /q /pa %1  > nul
:end