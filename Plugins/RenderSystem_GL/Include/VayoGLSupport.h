/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* OpenGL第三方支持
\*************************************************************************/
#ifndef __VAYO_GL_SUPPORT_H__
#define __VAYO_GL_SUPPORT_H__

#include "VayoSupport.h"
#include <Windows.h>
#include <WindowsX.h>
#include <glad.h>
#include <gl/GLU.h>

#ifdef VAYO_GLPLUGIN_EXPORTS
#define _VayoGLExport __declspec(dllexport)
#else
#if defined(__MINGW32__)
#define _VayoGLExport
#else
#define _VayoGLExport __declspec(dllimport)
#endif
#endif

namespace Vayo
{
	class GLRenderSystem;
	class Win32Device;
}

#endif // __VAYO_GL_SUPPORT_H__
