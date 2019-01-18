@echo off
set source_path=D:\______svn-code\vayo-engine-master\Vayo3D\
set target_path=D:\_____git-code\VayoEngine\Vayo3D\
echo copy[%source_path%]to[%target_path%]:
pause

echo ----------------------------------------

if not exist %target_path% md %target_path%
copy %source_path%..\VayoEngine.sln %target_path%..\VayoEngine.sln

echo ----------------------------------------BinĿ¼

if not exist %target_path%Bin\Win32\Debug\ md %target_path%Bin\Win32\Debug\
copy %source_path%Bin\Win32\Debug\engine_config.xml %target_path%Bin\Win32\Debug\engine_config.xml

if not exist %target_path%Bin\Win32\Release\ md %target_path%Bin\Win32\Release\
copy %source_path%Bin\Win32\Release\engine_config.xml %target_path%Bin\Win32\Release\engine_config.xml

if not exist %target_path%Bin\x64\Debug\ md %target_path%Bin\x64\Debug\
copy %source_path%Bin\x64\Debug\engine_config.xml %target_path%Bin\x64\Debug\engine_config.xml

if not exist %target_path%Bin\x64\Release\ md %target_path%Bin\x64\Release\
copy %source_path%Bin\x64\Release\engine_config.xml %target_path%Bin\x64\Release\engine_config.xml

echo ----------------------------------------LibĿ¼

if not exist %target_path%Lib\Win32\Debug\ md %target_path%Lib\Win32\Debug\

if not exist %target_path%Lib\Win32\Release\ md %target_path%Lib\Win32\Release\

if not exist %target_path%Lib\x64\Debug\ md %target_path%Lib\x64\Debug\

if not exist %target_path%Lib\x64\Release\ md %target_path%Lib\x64\Release\

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
xcopy /y %source_path%Plugins\ScriptSystem_Lua\Lua-5.2.4\*.* %target_path%Plugins\ScriptSystem_Lua\Lua-5.2.4\
xcopy /y %source_path%Plugins\ScriptSystem_Lua\lua5.2.4\*.* %target_path%Plugins\ScriptSystem_Lua\lua5.2.4\
xcopy /y %source_path%Plugins\ScriptSystem_Lua\lua\*.* %target_path%Plugins\ScriptSystem_Lua\lua\
xcopy /y %source_path%Plugins\ScriptSystem_Lua\luac\*.* %target_path%Plugins\ScriptSystem_Lua\luac\

echo ----------------------------------------SamplesĿ¼

xcopy /y %source_path%Samples\HelloWorld\*.* %target_path%Samples\HelloWorld\*.*
xcopy /y %source_path%Samples\TessgridBuilder\*.h %target_path%Samples\TessgridBuilder\*.h
xcopy /y %source_path%Samples\TessgridBuilder\*.cpp %target_path%Samples\TessgridBuilder\*.cpp
xcopy /y %source_path%Samples\TessgridBuilder\*.rc %target_path%Samples\TessgridBuilder\*.rc
xcopy /y %source_path%Samples\TessgridBuilder\res\*.* %target_path%Samples\TessgridBuilder\res\*.*
copy %source_path%Samples\TessgridBuilder\TessgridBuilder.vcxproj %target_path%Samples\TessgridBuilder\TessgridBuilder.vcxproj
copy %source_path%Samples\TessgridBuilder\TessgridBuilder.vcxproj.filters %target_path%Samples\TessgridBuilder\TessgridBuilder.vcxproj.filters
copy %source_path%Samples\TessgridBuilder\TessgridBuilder.vcxproj.user %target_path%Samples\TessgridBuilder\TessgridBuilder.vcxproj.user

echo ----------------------------------------UtilitiesĿ¼
xcopy /y /s /e %source_path%Utilities\*.* %target_path%Utilities\

echo ----------------------------------------MainĿ¼

xcopy /y %source_path%Main\*.* %target_path%Main\*.*
xcopy /y /s /e %source_path%Main\Include\*.* %target_path%Main\Include\
xcopy /y /s /e %source_path%Main\Source\*.* %target_path%Main\Source\
xcopy /y %source_path%Main\ThirdParty\*.cpp %target_path%Main\ThirdParty\*.cpp
xcopy /y /s /e %source_path%Main\ThirdParty\rc5\*.* %target_path%Main\ThirdParty\rc5\
xcopy /y /s /e %source_path%Main\ThirdParty\zlib\*.* %target_path%Main\ThirdParty\zlib\
xcopy /y /s /e %source_path%Main\ThirdParty\stb_image\*.* %target_path%Main\ThirdParty\stb_image\
xcopy /y %source_path%Main\ThirdParty\freetype-2.9.1\*.* %target_path%Main\ThirdParty\freetype-2.9.1\*.*
xcopy /y /s /e %source_path%Main\ThirdParty\freetype-2.9.1\include\*.* %target_path%Main\ThirdParty\freetype-2.9.1\include\
xcopy /y /s /e %source_path%Main\ThirdParty\freetype-2.9.1\src\*.* %target_path%Main\ThirdParty\freetype-2.9.1\src\

echo ----------------------------------------

pause