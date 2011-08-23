set progname=prog

if "%~1"=="" (set lib=..\old) else (set lib=%~1)

rar a -rr -r -ep1 -ap%progname% -x*.obj -x*.tds -x*.~* -x*.il? -x*.bak -x*.*~^
  -x*.dcu -x~W*.tmp -x~$*.* -x*.map -x*.pch "-ag yyyymmdd hhmmss"^
  "%lib%\%progname%.rar" .

if "%~1"=="" pause
