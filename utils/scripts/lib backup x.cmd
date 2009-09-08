pushd ..
md old
popd
rar a -rr -r -ep1 -apmxsrclib -x*.obj -x*.tds -x*.~* -x*.il? -x*.bak -x*.*~ -x*.dcu -x~W*.tmp -x~$*.* "-ag yyyymmdd hhmmss" "..\old\mxsrclib.rar" .
