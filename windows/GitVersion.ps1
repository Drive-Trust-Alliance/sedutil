Param ([String]$filename = "..\Version.h")
# Build a version header using the git describe command
set-alias git "C:\Program Files\Git\cmd\git.exe"
$filename = Join-Path $pwd $filename
$define = "#define GIT_VERSION "
$gitdesc = git describe --dirty='-derive'
echo $define`"$gitdesc`" | Out-File -FilePath $filename
