@echo off
set source_path=D:\______svn-code\vayo-engine-master\Vayo3D\
set target_path=D:\___WORK\vayo-engine-debug\Vayo3D\
echo copy[%source_path%]to[%target_path%]:
pause

if not exist %target_path% md %target_path%

echo ----------------------------------------DebugĿ¼

if not exist %target_path%Win32\Debug\ md %target_path%Win32\Debug\
copy %source_path%Bin\Win32\Debug\Vayo3D_d.dll %target_path%Win32\Debug\Vayo3D_d.dll
copy %source_path%Bin\Win32\Debug\Vayo3D_d.pdb %target_path%Win32\Debug\Vayo3D_d.pdb
copy %source_path%Bin\Win32\Debug\RenderSystem_GL_d.dll %target_path%Win32\Debug\RenderSystem_GL_d.dll
copy %source_path%Bin\Win32\Debug\RenderSystem_GL_d.pdb %target_path%Win32\Debug\RenderSystem_GL_d.pdb
copy %source_path%Bin\Win32\Debug\CableDiagram.dll %target_path%Win32\Debug\CableDiagram.dll
copy %source_path%Bin\Win32\Debug\CableDiagram.pdb %target_path%Win32\Debug\CableDiagram.pdb

if not exist %target_path%x64\Debug\ md %target_path%x64\Debug\
copy %source_path%Bin\x64\Debug\Vayo3D_d.dll %target_path%x64\Debug\Vayo3D_d.dll
copy %source_path%Bin\x64\Debug\Vayo3D_d.pdb %target_path%x64\Debug\Vayo3D_d.pdb
copy %source_path%Bin\x64\Debug\RenderSystem_GL_d.dll %target_path%x64\Debug\RenderSystem_GL_d.dll
copy %source_path%Bin\x64\Debug\RenderSystem_GL_d.pdb %target_path%x64\Debug\RenderSystem_GL_d.pdb
copy %source_path%Bin\x64\Debug\CableDiagram.dll %target_path%x64\Debug\CableDiagram.dll
copy %source_path%Bin\x64\Debug\CableDiagram.pdb %target_path%x64\Debug\CableDiagram.pdb

echo ----------------------------------------ReleaseĿ¼

if not exist %target_path%Win32\Release\ md %target_path%Win32\Release\
copy %source_path%Bin\Win32\Release\Vayo3D.dll %target_path%Win32\Release\Vayo3D.dll
copy %source_path%Bin\Win32\Release\Vayo3D.pdb %target_path%Win32\Release\Vayo3D.pdb
copy %source_path%Bin\Win32\Release\RenderSystem_GL.dll %target_path%Win32\Release\RenderSystem_GL.dll
copy %source_path%Bin\Win32\Release\RenderSystem_GL.pdb %target_path%Win32\Release\RenderSystem_GL.pdb
copy %source_path%Bin\Win32\Release\CableDiagram.dll %target_path%Win32\Release\CableDiagram.dll
copy %source_path%Bin\Win32\Release\CableDiagram.pdb %target_path%Win32\Release\CableDiagram.pdb

if not exist %target_path%x64\Release\ md %target_path%x64\Release\
copy %source_path%Bin\x64\Release\Vayo3D.dll %target_path%x64\Release\Vayo3D.dll
copy %source_path%Bin\x64\Release\Vayo3D.pdb %target_path%x64\Release\Vayo3D.pdb
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

echo ----------------------------------------MainĿ¼

xcopy /y /s /e %source_path%Main\Include\*.* %target_path%Main\Include\
xcopy /y /s /e %source_path%Main\Source\*.* %target_path%Main\Source\

echo ----------------------------------------

pause