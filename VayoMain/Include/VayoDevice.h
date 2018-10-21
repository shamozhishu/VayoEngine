/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* 窗口设备
\*************************************************************************/
#ifndef __VAYO_DEVICE_H__
#define __VAYO_DEVICE_H__

#include "VayoSupport.h"
#include "VayoBitState.h"
#include "VayoVector2d.h"
#include "VayoDimension2d.h"
#include "VayoRectangle.h"
NS_VAYO_BEGIN

class Device
{
public:
	static Device* create();

public:
	Device();
	virtual ~Device() {}
	virtual void* getMainWnd() const = 0;
	virtual float aspectRatio() const;
	virtual bool  init() = 0;
	virtual bool  handleEvents(bool& idle) = 0;
	virtual void  onResize();
	virtual void  onSleep(unsigned int milliSeconds = 0) = 0;
	virtual void  onDestroy() = 0;
	virtual void  setMainWndCaption(const wstring& wndCaption);
	virtual void  setScreenSize(const Dimension2di& screenSize);

	// 事件注入
	virtual void  injectMouseDown(unsigned int btnState, int x, int y);
	virtual void  injectMouseMove(unsigned int btnState, int x, int y);
	virtual void  injectMouseUp(unsigned int btnState, int x, int y);
	virtual void  injectMouseWheel(float wheel);
	virtual void  injectKeyboard(unsigned int keyCode, unsigned int scanCode, bool keyDown);
	virtual void  injectSizeMinimized(int w, int h);
	virtual void  injectSizeMaximized(int w, int h);
	virtual void  injectSizeRestored(int w, int h);
	virtual void  injectActivate();
	virtual void  injectInactive();
	virtual void  injectEnterSizeMove();
	virtual void  injectExitSizeMove();

protected:
	PROPERTY_R(bool,              _appPaused,      AppPaused)
	PROPERTY_R(bool,              _minimized,      Minimized)
	PROPERTY_R(bool,              _maximized,      Maximized)
	PROPERTY_R(bool,              _resizing,       Resizing)
	PROPERTY_R(BitState,          _mouseIsDown,    MouseIsDown)
	PROPERTY_R_REF(wstring,       _mainWndCaption, MainWndCaption)
	PROPERTY_R_REF(Dimension2di,  _screenSize,     ScreenSize)
};

NS_VAYO_END

#endif // __VAYO_DEVICE_H__
