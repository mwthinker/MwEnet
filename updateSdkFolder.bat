@echo off
:: Copies the latest MwExtra to the SDK folder.

set destFolder="..\SDK\MwEnet"

del %destFolder%\include\mw\enet\ /Q
del %destFolder%\source\ /Q

del %destFolder%\lib-r\ /Q
del %destFolder%\lib-d\ /Q

xcopy mw\enet\*.h %destFolder%\include\mw\enet\
xcopy mw\enet\*.cpp %destFolder%\source\

xcopy Release\*.pdb %destFolder%\lib-r\
xcopy Release\MwEnet.lib %destFolder%\lib-r\

xcopy Debug\*.pdb %destFolder%\lib-d\
xcopy Debug\*.idb %destFolder%\lib-d\
xcopy Debug\MwEnet.lib %destFolder%\lib-d\
