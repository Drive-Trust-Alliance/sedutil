@echo Making new Windows distribution zip
@echo off
DEL sedutil_WIN.zip
"c:\Program Files\7-Zip\7z.exe" a sedutil_WIN.zip .\PSIDRevert_WINDOWS.txt
"c:\Program Files\7-Zip\7z.exe" a sedutil_WIN.zip .\..\*.txt
"c:\Program Files\7-Zip\7z.exe" a sedutil_WIN.zip .\CLI\*\Release\*.exe
PAUSE