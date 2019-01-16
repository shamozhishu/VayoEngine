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
	inline DWORD printLastError(wstring hint = L"")
	{
		DWORD errCode = GetLastError();
		LPVOID lpMsgBuf = NULL;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, errCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
		wchar_t buff[64];
		wsprintf(buff, L"%d", errCode);
		if (!hint.empty())
			hint += L"\n";
		hint += L"错误码";
		hint += buff;
		hint += L"：";
		hint += (LPCTSTR)lpMsgBuf;
		LocalFree(lpMsgBuf);
		MessageBox(0, hint.c_str(), 0, 0);
		return errCode;
	}
}

#endif // __VAYO_GL_SUPPORT_H__
