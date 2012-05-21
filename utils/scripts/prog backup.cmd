echo off

set progname=prog

if "%~1"=="" (set lib=..\old) else (set lib=%~1)

set archive_file_name=%lib%
call datetime_name date_time_str
set archive_file_name=%archive_file_name%\%date_time_str% %progname%.7z
call utils arc7z_excluded arc7z_excluded_var
call 7zarc a %arc7z_excluded_var% "%archive_file_name%" ..\%progname%

goto no_rar
rar a -rr -r -ep1 -ap%progname% -x*.obj -x*.tds -x*.~* -x*.il? -x*.bak -x*.*~^
  -x*.dcu -x~W*.tmp -x~$*.* -x*.map -x*.pch -x*.#?? "-ag yyyymmdd hhmmss"^
  "%lib%\%progname%.rar" .
:no_rar

if "%~1"=="" pause
