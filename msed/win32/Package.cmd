@echo Making new Windows distribution zip
@echo off
DEL msed_WIN.zip
"c:\Program Files\7-Zip\7z.exe" a msed_WIN.zip .\*.txt
"c:\Program Files\7-Zip\7z.exe" a msed_WIN.zip .\*.cmd
"c:\Program Files\7-Zip\7z.exe" a msed_WIN.zip .\..\*.txt
"c:\Program Files\7-Zip\7z.exe" a msed_WIN.zip .\..\..\Release\msed.exe
 