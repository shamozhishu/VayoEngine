/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* OpenGL第三方支持
\*************************************************************************/
#ifndef __GL_SUPPORT_H__
#define __GL_SUPPORT_H__

#include "Vayo3dSupport.h"
#include <Windows.h>
#include <WindowsX.h>
#include <glad.h>
#include <gl/GLU.h>

using namespace Vayo;
using namespace Vayo::_3d;
class GLRenderSystem;
class GLWin32Device;
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

#endif // __GL_SUPPORT_H__
