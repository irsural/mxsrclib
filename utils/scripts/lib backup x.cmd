if "%~1"=="" (set lib=..\old) else (set lib=%~1)

pushd ..
md old
popd

set archive_file_name=%lib%
call datetime_name date_time_str
set archive_file_name=%archive_file_name%\%date_time_str% mxsrclib.7z
call utils arc7z_excluded arc7z_excluded_var
call 7zarc a %arc7z_excluded_var% "%archive_file_name%" ..\mxsrclib

goto no_rar
rar a -rr -r -ep1 -apmxsrclib -x*.obj -x*.tds -x*.~* -x*.il? -x*.bak -x*.*~^
  -x*.dcu -x~W*.tmp -x~$*.* -x*.map -x*.pch "-ag yyyymmdd hhmmss"^
  "%lib%\mxsrclib.rar" .
:no_rar
