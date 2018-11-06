@echo off 
@REM sign exe file with no password certificate

@REM verify if file has been signed , if so skip signing , otherwise get error for double signing
@REM verify signed exe
signtool verify /pa %1
if %errorlevel% == 0 (
	@echo "code has been signed, no double signing, skip "
	@goto end
)
@echo "sign the code"
@REM /fd sha256 /tr http://timestamp.comodoca.com/?td=sha256 /td sha256 
signtool sign -f "C:\Dropbox\1Note\codesigning\cert\FidelityHeightLLC-nopass.p12" /fd sha256 /tr http://timestamp.comodoca.com/authenticode?td=sha256 /td sha256 %1

if %errorlevel% == 0 (
	@echo "Code signing OK, verify if it really ok"
	@goto L1
)
@echo "Code signing error. exit" 
@goto end
 
:L1
@REM verify signed exe
signtool verify /pa %1





:end
