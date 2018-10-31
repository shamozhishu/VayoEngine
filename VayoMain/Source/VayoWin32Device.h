/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* win32窗口设备
\*************************************************************************/
#ifndef __VAYO_WIN32_DEVICE_H__
#define __VAYO_WIN32_DEVICE_H__

#ifdef _WIN32

#include "VayoDevice.h"
#include <Windows.h>
#include <WindowsX.h>
NS_VAYO_BEGIN

class Win32Device : public Device
{
	DISALLOW_COPY_AND_ASSIGN(Win32Device)
public:
	Win32Device();
	~Win32Device();
	bool    init();
	void*   getWndHandle() const;
	bool    handleEvents(bool& idle);
	void    sleep(unsigned int milliSeconds, bool pauseTimer);
	void    setWndCaption(const wstring& wndCaption);

	// 事件注入
	void    injectMouseDown(unsigned int btnState, int x, int y);
	void    injectMouseMove(unsigned int btnState, int x, int y);
	void    injectMouseUp(unsigned int btnState, int x, int y);
	void    injectKeyboard(unsigned int keyCode, unsigned int scanCode, bool keyDown);
	void    injectPaint();
	void    injectDestroy();
	void    injectInputLanguageChange();

	LRESULT msgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	HWND _wndHandle;
	bool _externalWindow;
};

NS_VAYO_END

#endif // _WIN32
#endif // __VAYO_WIN32_DEVICE_H__
