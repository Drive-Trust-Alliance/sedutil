copy ..\windows\CLI\x64\Debug\sedutil-cli.exe sedutil.exe
copy /b  sedutil.exe + UEFI64.zip sedutil-cli.exe
REM C:\util-1207\util\rc\sedutil-cli.exe --loadpbaimage password UEFI \\.\physicaldrive1
REM C:\util-1207\util\rc\sedutil-cli.exe --loadpbaimage 7WTWDD6Q2VF13A35711PQ3MA9ZJXXVT4 UEFI \\.\physicaldrive1
