@echo off
set source_path=D:\______svn-code\vayo-engine-master\
set target_path=D:\___WORK\vayo-engine-debug\
echo copy[%source_path%]to[%target_path%]:
pause

if not exist %target_path% md %target_path%

echo ----------------------------------------DebugĿ¼

if not exist %target_path%Debug\ md %target_path%Debug\
copy %source_path%Bin\Debug\VayoMain_d.dll %target_path%Debug\VayoMain_d.dll
copy %source_path%Bin\Debug\VayoMain_d.pdb %target_path%Debug\VayoMain_d.pdb
copy %source_path%Bin\Debug\RenderSystem_GL_d.dll %target_path%Debug\RenderSystem_GL_d.dll
copy %source_path%Bin\Debug\RenderSystem_GL_d.pdb %target_path%Debug\RenderSystem_GL_d.pdb
copy %source_path%Bin\Debug\CableDiagram.dll %target_path%Debug\CableDiagram.dll
copy %source_path%Bin\Debug\CableDiagram.pdb %target_path%Debug\CableDiagram.pdb

if not exist %target_path%x64\Debug\ md %target_path%x64\Debug\
copy %source_path%Bin\x64\Debug\VayoMain_d.dll %target_path%x64\Debug\VayoMain_d.dll
copy %source_path%Bin\x64\Debug\VayoMain_d.pdb %target_path%x64\Debug\VayoMain_d.pdb
copy %source_path%Bin\x64\Debug\RenderSystem_GL_d.dll %target_path%x64\Debug\RenderSystem_GL_d.dll
copy %source_path%Bin\x64\Debug\RenderSystem_GL_d.pdb %target_path%x64\Debug\RenderSystem_GL_d.pdb
copy %source_path%Bin\x64\Debug\CableDiagram.dll %target_path%x64\Debug\CableDiagram.dll
copy %source_path%Bin\x64\Debug\CableDiagram.pdb %target_path%x64\Debug\CableDiagram.pdb

echo ----------------------------------------ReleaseĿ¼

if not exist %target_path%Release\ md %target_path%Release\
copy %source_path%Bin\Release\VayoMain.dll %target_path%Release\VayoMain.dll
copy %source_path%Bin\Release\VayoMain.pdb %target_path%Release\VayoMain.pdb
copy %source_path%Bin\Release\RenderSystem_GL.dll %target_path%Release\RenderSystem_GL.dll
copy %source_path%Bin\Release\RenderSystem_GL.pdb %target_path%Release\RenderSystem_GL.pdb
copy %source_path%Bin\Release\CableDiagram.dll %target_path%Release\CableDiagram.dll
copy %source_path%Bin\Release\CableDiagram.pdb %target_path%Release\CableDiagram.pdb

if not exist %target_path%x64\Release\ md %target_path%x64\Release\
copy %source_path%Bin\x64\Release\VayoMain.dll %target_path%x64\Release\VayoMain.dll
copy %source_path%Bin\x64\Release\VayoMain.pdb %target_path%x64\Release\VayoMain.pdb
copy %source_path%Bin\x64\Release\RenderSystem_GL.dll %target_path%x64\Release\RenderSystem_GL.dll
copy %source_path%Bin\x64\Release\RenderSystem_GL.pdb %target_path%x64\Release\RenderSystem_GL.pdb
copy %source_path%Bin\x64\Release\CableDiagram.dll %target_path%x64\Release\CableDiagram.dll
copy %source_path%Bin\x64\Release\CableDiagram.pdb %target_path%x64\Release\CableDiagram.pdb

echo ----------------------------------------GLRendererĿ¼

xcopy /y %source_path%Plugins\RenderSystem_GL\Include\*.* %target_path%GLRenderer\Include\
xcopy /y %source_path%Plugins\RenderSystem_GL\Source\*.* %target_path%GLRenderer\Source\
xcopy /y %source_path%Plugins\RenderSystem_GL\GL\*.* %target_path%GLRenderer\GL\

echo ----------------------------------------SamplesĿ¼

xcopy /y %source_path%Samples\CableDiagram\*.h %target_path%Samples\CableDiagram\*.h
xcopy /y %source_path%Samples\CableDiagram\*.cpp %target_path%Samples\CableDiagram\*.cpp

echo ----------------------------------------VayoMainĿ¼

xcopy /y /s /e %source_path%VayoMain\Include\*.* %target_path%VayoMain\Include\
xcopy /y /s /e %source_path%VayoMain\Source\*.* %target_path%VayoMain\Source\

echo ----------------------------------------

pause