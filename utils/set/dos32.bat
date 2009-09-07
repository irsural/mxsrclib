move /y ..\..\arch ..\..\arch_old > NUL
del /f /s /q ..\..\arch\cur > NUL
xcopy /y /i ..\..\arch_old\dos32 ..\..\arch\cur > NUL
