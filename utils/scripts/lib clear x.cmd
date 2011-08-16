call utils\scripts\delx.bat
:del_arm
cd arch\arm
if errorlevel 1 goto del_avr
call ..\..\utils\scripts\delx.bat
cd ..\..
:del_avr
cd arch\avr
if errorlevel 1 goto del_cbuilder
call ..\..\utils\scripts\delx.bat
cd ..\..
:del_cbuilder
cd arch\cbuilder
if errorlevel 1 goto del_dos32
call ..\..\utils\scripts\delx.bat
cd ..\..
:del_dos32
cd arch\dos32
if errorlevel 1 goto del_linux
call ..\..\utils\scripts\delx.bat
cd ..\..
:del_linux
cd arch\linux
if errorlevel 1 goto del_qt
call ..\..\utils\scripts\delx.bat
cd ..\..
:del_qt
cd arch\qt
if errorlevel 1 goto del_visual
call ..\..\utils\scripts\delx.bat
cd ..\..
:del_visual
cd arch\visualcpp
if errorlevel 1 goto script_exit
call ..\..\utils\scripts\delx.bat
cd ..\..
:script_exit
