@echo off
set source_path=D:\______svn-code\vayo-engine-master\
set target_path=D:\_____git-code\VayoEngine\
echo copy[%source_path%]to[%target_path%]:
pause

echo ----------------------------------------

if not exist %target_path% md %target_path%
copy %source_path%VayoEngine.sln %target_path%VayoEngine.sln

echo ----------------------------------------main catalog

xcopy /y %source_path%main\2d\*.* %target_path%main\2d\*.*
xcopy /y /s /e %source_path%main\2d\inc\*.* %target_path%main\2d\inc\
xcopy /y /s /e %source_path%main\2d\src\*.* %target_path%main\2d\src\

xcopy /y %source_path%main\3d\*.* %target_path%main\3d\*.*
xcopy /y /s /e %source_path%main\3d\inc\*.* %target_path%main\3d\inc\
xcopy /y /s /e %source_path%main\3d\src\*.* %target_path%main\3d\src\

xcopy /y %source_path%main\core\*.* %target_path%main\core\*.*
xcopy /y /s /e %source_path%main\core\inc\*.* %target_path%main\core\inc\
xcopy /y /s /e %source_path%main\core\src\*.* %target_path%main\core\src\

echo ----------------------------------------media catalog

xcopy /y /s /e %source_path%media\*.* %target_path%media\

echo ----------------------------------------plugins catalog

xcopy /y %source_path%plugins\Renderer_D2D\*.* %target_path%plugins\Renderer_D2D\*.*
xcopy /y /s /e %source_path%plugins\Renderer_D2D\inc\*.* %target_path%plugins\Renderer_D2D\inc\
xcopy /y /s /e %source_path%plugins\Renderer_D2D\src\*.* %target_path%plugins\Renderer_D2D\src\

xcopy /y %source_path%plugins\RenderSystem_GL\*.* %target_path%plugins\RenderSystem_GL\*.*
xcopy /y /s /e %source_path%plugins\RenderSystem_GL\inc\*.* %target_path%plugins\RenderSystem_GL\inc\
xcopy /y /s /e %source_path%plugins\RenderSystem_GL\src\*.* %target_path%plugins\RenderSystem_GL\src\

xcopy /y %source_path%plugins\ScriptSystem_Lua\*.* %target_path%plugins\ScriptSystem_Lua\*.*
xcopy /y /s /e %source_path%plugins\ScriptSystem_Lua\inc\*.* %target_path%plugins\ScriptSystem_Lua\inc\
xcopy /y /s /e %source_path%plugins\ScriptSystem_Lua\src\*.* %target_path%plugins\ScriptSystem_Lua\src\

echo ----------------------------------------samples catalog

xcopy /y %source_path%samples\HelloWorld\*.* %target_path%samples\HelloWorld\*.*
xcopy /y %source_path%samples\TessgridBuilder\*.h %target_path%samples\TessgridBuilder\*.h
xcopy /y %source_path%samples\TessgridBuilder\*.cpp %target_path%samples\TessgridBuilder\*.cpp
xcopy /y %source_path%samples\TessgridBuilder\*.rc %target_path%samples\TessgridBuilder\*.rc
xcopy /y /s /e %source_path%samples\TessgridBuilder\res\*.* %target_path%samples\TessgridBuilder\res\
copy %source_path%samples\TessgridBuilder\TessgridBuilder.vcxproj %target_path%samples\TessgridBuilder\TessgridBuilder.vcxproj
copy %source_path%samples\TessgridBuilder\TessgridBuilder.vcxproj.filters %target_path%samples\TessgridBuilder\TessgridBuilder.vcxproj.filters
copy %source_path%samples\TessgridBuilder\TessgridBuilder.vcxproj.user %target_path%samples\TessgridBuilder\TessgridBuilder.vcxproj.user

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
xcopy /y %source_path%thirdparty\lua-5.2.4\liblua\*.* %target_path%thirdparty\lua-5.2.4\liblua\*.*
xcopy /y %source_path%thirdparty\lua-5.2.4\lua\*.* %target_path%thirdparty\lua-5.2.4\lua\*.*
xcopy /y %source_path%thirdparty\lua-5.2.4\luac\*.* %target_path%thirdparty\lua-5.2.4\luac\*.*
xcopy /y %source_path%thirdparty\lua-5.2.4\tolua++\*.* %target_path%thirdparty\lua-5.2.4\tolua++\*.*

echo ----------------------------------------utilities catalog

xcopy /y /s /e %source_path%utilities\*.* %target_path%utilities\

echo ----------------------------------------

pause