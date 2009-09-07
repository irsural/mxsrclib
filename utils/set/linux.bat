move /y ..\..\arch ..\..\arch_old > NUL
del /f /s /q ..\..\arch\cur > NUL
xcopy /y /i ..\..\arch_old\linux ..\..\arch\cur > NUL
