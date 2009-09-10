pushd ..
md old
popd
rar a -rr -r -ep1 -apmxsrclib -x*.obj -x*.tds -x*.~* -x*.il? -x*.bak -x*.*~ -x*.dcu -x~W*.tmp -x~$*.* -x*.map -x*.pch "-ag yyyymmdd hhmmss" "..\old\mxsrclib.rar" .
