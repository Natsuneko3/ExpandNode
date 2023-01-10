
D:
cd D:\Epic Games\UE_4.26\Engine\Build\BatchFiles
call RunUAT.bat BuildPlugin -Plugin="C:\Users\Neko\OneDrive\UePlugins\ExpandNode\ExpandNode.uplugin" -Package="C:\Users\Neko\OneDrive\UePlugins\ExpandNode\Package\UE4.26\ExpandNode" -Rocket
c:
cd C:\Users\Neko\OneDrive\UePlugins\ExpandNode\Package\UE4.26\ExpandNode
rd /s /q Binaries
rd /s /q Intermediate
cd ..\
IF EXIST "Ue4.26Plugins.zip" del "Ue4.26Plugins.zip"
7z.exe a -r ..\Ue4.26Plugins.zip
cd ..\ 
rd /s /q "UE4.26"
pause