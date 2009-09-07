call delx.bat
cd arch\cur
if errorlevel 1 goto del_avr
call delx.bat
cd ..\..
:del_avr
cd arch\avr
if errorlevel 1 goto del_cbuilder
call delx.bat
cd ..\..
:del_cbuilder
cd arch\cbuilder
if errorlevel 1 goto del_dos32
call delx.bat
cd ..\..
:del_dos32
cd arch\dos32
if errorlevel 1 goto del_linux
call delx.bat
cd ..\..
:del_linux
cd arch\linux
if errorlevel 1 goto del_qt
call delx.bat
cd ..\..
:del_qt
cd arch\qt
if errorlevel 1 goto del_visual
call delx.bat
cd ..\..
:del_visual
cd arch\visualcpp
if errorlevel 1 goto script_exit
call delx.bat
cd ..\..
:script_exit
