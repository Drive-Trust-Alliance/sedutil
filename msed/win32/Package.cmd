@echo Making new Windows distribution zip
@echo off
DEL msed_WIN.zip
"c:\Program Files\7-Zip\7z.exe" a msed_WIN.zip .\*.txt
"c:\Program Files\7-Zip\7z.exe" a msed_WIN.zip .\*.cmd
"c:\Program Files\7-Zip\7z.exe" a msed_WIN.zip .\..\*.txt
"c:\Program Files\7-Zip\7z.exe" a msed_WIN.zip .\..\..\Win32\Release\msed.exe
COPY .\..\..\x64\Release\msed.exe .\..\..\x64\Release\msed64.exe
"c:\Program Files\7-Zip\7z.exe" a msed_WIN.zip .\..\..\x64\Release\msed64.exe
DEL .\..\..\x64\Release\msed64.exe
PAUSE