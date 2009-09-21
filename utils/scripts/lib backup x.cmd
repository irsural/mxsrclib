if "%~1"=="" (set lib=..\old) else (set lib=%~1)

pushd ..
md old
popd
rar a -rr -r -ep1 -apmxsrclib -x*.obj -x*.tds -x*.~* -x*.il? -x*.bak -x*.*~ -x*.dcu -x~W*.tmp -x~$*.* -x*.map -x*.pch "-ag yyyymmdd hhmmss" "%lib%\mxsrclib.rar" .
