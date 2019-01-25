/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* win32窗口设备
\*************************************************************************/
#ifndef __GL_WIN32_DEVICE_H__
#define __GL_WIN32_DEVICE_H__

#ifdef _WIN32

#include "GLSupport.h"
#include "Vayo3dDevice.h"

class GLWin32Device : public Device
{
	DISALLOW_COPY_AND_ASSIGN(GLWin32Device)
public:
	GLWin32Device(int deviceID, const Attrib& attrib);
	~GLWin32Device();
	bool    init();
	void*   getWndHandle() const;
	bool    handleEvents(bool& idle);
	void    sleep(unsigned int milliSeconds, bool pauseTimer);
	void    setWndCaption(const wstring& wndCaption);

	// 事件注入
	void    injectMouseDown(EMouseKeys mouseKey, int x, int y);
	void    injectMouseUp(EMouseKeys mouseKey, int x, int y);
	void    injectMouseMove(int x, int y);
	void    injectMouseWheel(float wheel);
	void    injectKeyboard(unsigned int keyCode, unsigned int scanCode, bool keyDown);
	void    injectPaint();
	void    injectDestroy();
	void    injectActivate();
	void    injectInactive();
	void    injectInputLanguageChange();

	LRESULT windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	bool _externalWindow;
	PROPERTY_R(HWND, _wndHandle, Win32WndHandle)
};

#endif // _WIN32
#endif // __GL_WIN32_DEVICE_H__
