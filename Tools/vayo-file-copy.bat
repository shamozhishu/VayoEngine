@echo off
set source_path=D:\______svn-code\vayo-engine-master\
set target_path=D:\_____git-code\VayoEngine\
echo copy[%source_path%]to[%target_path%]:
pause

echo ----------------------------------------

if not exist %target_path% md %target_path%
copy %source_path%VayoEngine.sln %target_path%VayoEngine.sln

echo ----------------------------------------BinĿ¼

if not exist %target_path%Bin\Debug\ md %target_path%Bin\Debug\
copy %source_path%Bin\Debug\engine_config.xml %target_path%Bin\Debug\engine_config.xml
copy %source_path%Bin\Debug\lua.exe %target_path%Bin\Debug\lua.exe
copy %source_path%Bin\Debug\luac.exe %target_path%Bin\Debug\luac.exe
copy %source_path%Bin\Debug\tolua*.exe %target_path%Bin\Debug\tolua*.exe

if not exist %target_path%Bin\Release\ md %target_path%Bin\Release\
copy %source_path%Bin\Release\engine_config.xml %target_path%Bin\Release\engine_config.xml
copy %source_path%Bin\Release\lua.exe %target_path%Bin\Release\lua.exe
copy %source_path%Bin\Release\luac.exe %target_path%Bin\Release\luac.exe
copy %source_path%Bin\Release\tolua*.exe %target_path%Bin\Release\tolua*.exe

if not exist %target_path%Bin\x64\Debug\ md %target_path%Bin\x64\Debug\
copy %source_path%Bin\x64\Debug\engine_config.xml %target_path%Bin\x64\Debug\engine_config.xml
copy %source_path%Bin\x64\Debug\lua.exe %target_path%Bin\x64\Debug\lua.exe
copy %source_path%Bin\x64\Debug\luac.exe %target_path%Bin\x64\Debug\luac.exe
copy %source_path%Bin\x64\Debug\tolua*.exe %target_path%Bin\x64\Debug\tolua*.exe

if not exist %target_path%Bin\x64\Release\ md %target_path%Bin\x64\Release\
copy %source_path%Bin\x64\Release\engine_config.xml %target_path%Bin\x64\Release\engine_config.xml
copy %source_path%Bin\x64\Release\lua.exe %target_path%Bin\x64\Release\lua.exe
copy %source_path%Bin\x64\Release\luac.exe %target_path%Bin\x64\Release\luac.exe
copy %source_path%Bin\x64\Release\tolua*.exe %target_path%Bin\x64\Release\tolua*.exe

echo ----------------------------------------LibĿ¼

if not exist %target_path%Lib\Debug\ md %target_path%Lib\Debug\
copy %source_path%Lib\Debug\freetype_d.lib %target_path%Lib\Debug\freetype_d.lib
copy %source_path%Lib\Debug\lua5.2.4_d.lib %target_path%Lib\Debug\lua5.2.4_d.lib

if not exist %target_path%Lib\Release\ md %target_path%Lib\Release\
copy %source_path%Lib\Release\freetype.lib %target_path%Lib\Release\freetype.lib
copy %source_path%Lib\Release\lua5.2.4.lib %target_path%Lib\Release\lua5.2.4.lib

if not exist %target_path%Lib\x64\Debug\ md %target_path%Lib\x64\Debug\
copy %source_path%Lib\x64\Debug\freetype_d.lib %target_path%Lib\x64\Debug\freetype_d.lib
copy %source_path%Lib\x64\Debug\lua5.2.4_d.lib %target_path%Lib\x64\Debug\lua5.2.4_d.lib

if not exist %target_path%Lib\x64\Release\ md %target_path%Lib\x64\Release\
copy %source_path%Lib\x64\Release\freetype.lib %target_path%Lib\x64\Release\freetype.lib
copy %source_path%Lib\x64\Release\lua5.2.4.lib %target_path%Lib\x64\Release\lua5.2.4.lib

echo ----------------------------------------MediaĿ¼

xcopy /y /s /e %source_path%Media\*.* %target_path%Media\

echo ----------------------------------------PluginsĿ¼

xcopy /y %source_path%Plugins\RenderSystem_GL\*.* %target_path%Plugins\RenderSystem_GL\*.*
xcopy /y %source_path%Plugins\RenderSystem_GL\Include\*.* %target_path%Plugins\RenderSystem_GL\Include\
xcopy /y %source_path%Plugins\RenderSystem_GL\Source\*.* %target_path%Plugins\RenderSystem_GL\Source\
xcopy /y %source_path%Plugins\RenderSystem_GL\GL\*.* %target_path%Plugins\RenderSystem_GL\GL\

xcopy /y %source_path%Plugins\ScriptSystem_Lua\*.* %target_path%Plugins\ScriptSystem_Lua\*.*
xcopy /y %source_path%Plugins\ScriptSystem_Lua\Include\*.* %target_path%Plugins\ScriptSystem_Lua\Include\
xcopy /y %source_path%Plugins\ScriptSystem_Lua\Source\*.* %target_path%Plugins\ScriptSystem_Lua\Source\
xcopy /y %source_path%Plugins\ScriptSystem_Lua\Tolua\*.* %target_path%Plugins\ScriptSystem_Lua\Tolua\
xcopy /y %source_path%Plugins\ScriptSystem_Lua\Lua\*.* %target_path%Plugins\ScriptSystem_Lua\Lua\

echo ----------------------------------------SamplesĿ¼

xcopy /y %source_path%Samples\HideAndSeek\*.* %target_path%Samples\HideAndSeek\*.*

echo ----------------------------------------ToolsĿ¼
xcopy /y /s /e %source_path%Tools\*.* %target_path%Tools\

echo ----------------------------------------VayoMainĿ¼

xcopy /y %source_path%VayoMain\*.* %target_path%VayoMain\*.*
xcopy /y %source_path%VayoMain\Include\*.* %target_path%VayoMain\Include\
xcopy /y %source_path%VayoMain\Source\*.* %target_path%VayoMain\Source\
xcopy /y /s /e %source_path%VayoMain\ThirdParty\*.* %target_path%VayoMain\ThirdParty\

echo ----------------------------------------

pause