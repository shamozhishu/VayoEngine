@echo off
set source_path=D:\______svn-code\vayo-engine-master\
set target_path=D:\_____git-code\VayoEngine\
echo copy[%source_path%]to[%target_path%]:
pause

echo ----------------------------------------

if not exist %target_path% md %target_path%
copy %source_path%VayoEngine.sln %target_path%VayoEngine.sln

echo ----------------------------------------2d catalog

xcopy /y %source_path%2d\main\*.* %target_path%2d\main\*.*
xcopy /y /s /e %source_path%2d\main\inc\*.* %target_path%2d\main\inc\
xcopy /y /s /e %source_path%2d\main\src\*.* %target_path%2d\main\src\

echo ----------------------------------------3d catalog

xcopy /y %source_path%3d\main\*.* %target_path%3d\main\*.*
xcopy /y /s /e %source_path%3d\main\inc\*.* %target_path%3d\main\inc\
xcopy /y /s /e %source_path%3d\main\src\*.* %target_path%3d\main\src\

xcopy /y %source_path%3d\plugins\RenderSystem_GL\*.* %target_path%3d\plugins\RenderSystem_GL\*.*
xcopy /y /s /e %source_path%3d\plugins\RenderSystem_GL\inc\*.* %target_path%3d\plugins\RenderSystem_GL\inc\
xcopy /y /s /e %source_path%3d\plugins\RenderSystem_GL\src\*.* %target_path%3d\plugins\RenderSystem_GL\src\

xcopy /y %source_path%3d\plugins\ScriptSystem_Lua\*.* %target_path%3d\plugins\ScriptSystem_Lua\*.*
xcopy /y /s /e %source_path%3d\plugins\ScriptSystem_Lua\inc\*.* %target_path%3d\plugins\ScriptSystem_Lua\inc\
xcopy /y /s /e %source_path%3d\plugins\ScriptSystem_Lua\src\*.* %target_path%3d\plugins\ScriptSystem_Lua\src\

xcopy /y %source_path%3d\samples\HelloWorld\*.* %target_path%3d\samples\HelloWorld\*.*
xcopy /y %source_path%3d\samples\TessgridBuilder\*.h %target_path%3d\samples\TessgridBuilder\*.h
xcopy /y %source_path%3d\samples\TessgridBuilder\*.cpp %target_path%3d\samples\TessgridBuilder\*.cpp
xcopy /y %source_path%3d\samples\TessgridBuilder\*.rc %target_path%3d\samples\TessgridBuilder\*.rc
xcopy /y /s /e %source_path%3d\samples\TessgridBuilder\res\*.* %target_path%3d\samples\TessgridBuilder\res\
copy %source_path%3d\samples\TessgridBuilder\TessgridBuilder.vcxproj %target_path%3d\samples\TessgridBuilder\TessgridBuilder.vcxproj
copy %source_path%3d\samples\TessgridBuilder\TessgridBuilder.vcxproj.filters %target_path%3d\samples\TessgridBuilder\TessgridBuilder.vcxproj.filters
copy %source_path%3d\samples\TessgridBuilder\TessgridBuilder.vcxproj.user %target_path%3d\samples\TessgridBuilder\TessgridBuilder.vcxproj.user

xcopy /y /s /e %source_path%3d\utilities\*.* %target_path%3d\utilities\

echo ----------------------------------------core catalog

xcopy /y %source_path%core\*.* %target_path%core\*.*
xcopy /y /s /e %source_path%core\inc\*.* %target_path%core\inc\
xcopy /y /s /e %source_path%core\src\*.* %target_path%core\src\

echo ----------------------------------------media catalog

xcopy /y /s /e %source_path%media\*.* %target_path%media\

echo ----------------------------------------thirdparty catalog

xcopy /y %source_path%thirdparty\rc5\*.* %target_path%thirdparty\rc5\*.*
xcopy /y %source_path%thirdparty\zlib\*.* %target_path%thirdparty\zlib\*.*
xcopy /y %source_path%thirdparty\opengl\*.* %target_path%thirdparty\opengl\*.*
xcopy /y %source_path%thirdparty\tinyxml2\*.* %target_path%thirdparty\tinyxml2\*.*
xcopy /y %source_path%thirdparty\stb_image\*.* %target_path%thirdparty\stb_image\*.*
xcopy /y %source_path%thirdparty\freetype-2.9.1\*.* %target_path%thirdparty\freetype-2.9.1\*.*
xcopy /y /s /e %source_path%thirdparty\freetype-2.9.1\include\*.* %target_path%thirdparty\freetype-2.9.1\include\
xcopy /y /s /e %source_path%thirdparty\freetype-2.9.1\src\*.* %target_path%thirdparty\freetype-2.9.1\src\
xcopy /y /s /e %source_path%thirdparty\lua-5.2.4\src\*.* %target_path%thirdparty\lua-5.2.4\src\
xcopy /y %source_path%thirdparty\lua-5.2.4\lua5.2.4\*.* %target_path%thirdparty\lua-5.2.4\lua5.2.4\*.*
xcopy /y %source_path%thirdparty\lua-5.2.4\lua\*.* %target_path%thirdparty\lua-5.2.4\lua\*.*
xcopy /y %source_path%thirdparty\lua-5.2.4\luac\*.* %target_path%thirdparty\lua-5.2.4\luac\*.*
xcopy /y %source_path%thirdparty\lua-5.2.4\tolua++\*.* %target_path%thirdparty\lua-5.2.4\tolua++\*.*

echo ----------------------------------------

pause