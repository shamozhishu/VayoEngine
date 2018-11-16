/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* 窗口设备
\*************************************************************************/
#ifndef __VAYO_DEVICE_H__
#define __VAYO_DEVICE_H__

#include "VayoSupport.h"
#include "VayoInput.h"
#include "VayoColour.h"
#include "VayoBitState.h"
#include "VayoVector2d.h"
#include "VayoRectangle.h"
NS_VAYO_BEGIN

class _VayoExport Device
{
public:
	typedef struct tagAttrib
	{
		void*         WndHandle;
		bool          WndQuit;
		bool          WndPaint;
		bool          TurnOnUI;
		bool          FullScreen;
		Colour        BgClearColor;
		Dimension2di  ScreenSize;
		wstring       WndCaption;
		tagAttrib()
			: WndHandle(NULL)
			, WndQuit(true)
			, WndPaint(false)
			, TurnOnUI(true)
			, FullScreen(false)
			, BgClearColor(0)
			, ScreenSize(1280, 720)
			, WndCaption(L"Vayo Engine") {}

	} Attrib;

public:
	Device(const Attrib& attrib);
	virtual ~Device();
	virtual bool  init() = 0;
	virtual bool  openUI();
	virtual void  closeUI();
	virtual void* getWndHandle() const = 0;
	virtual float getAspectRatio() const;
	virtual bool  handleEvents(bool& idle) = 0;
	virtual void  onResize();
	virtual void  sleep(unsigned int milliSeconds, bool pauseTimer) = 0;
	virtual void  setWndCaption(const wstring& wndCaption);
	virtual void  setScreenSize(const Dimension2di& screenSize);
	virtual const wstring& getWndCaption() const;
	virtual const Dimension2di& getScreenSize() const;

	// 事件注入
	virtual void  injectMouseDown(EMouseKeys mouseKey, int x, int y);
	virtual void  injectMouseUp(EMouseKeys mouseKey, int x, int y);
	virtual void  injectMouseMove(int x, int y);
	virtual void  injectMouseWheel(float wheel);
	virtual void  injectKeyboard(unsigned int keyCode, unsigned int scanCode, bool keyDown);
	virtual void  injectPaint();
	virtual void  injectDestroy();
	virtual void  injectActivate();
	virtual void  injectInactive();
	virtual void  injectSizeMinimized(int w, int h);
	virtual void  injectSizeMaximized(int w, int h);
	virtual void  injectSizeRestored(int w, int h);
	virtual void  injectEnterSizeMove();
	virtual void  injectExitSizeMove();
	virtual void  injectInputLanguageChange();

protected:
	PROPERTY_R(bool,     _appPaused,   AppPaused)
	PROPERTY_R(bool,     _minimized,   Minimized)
	PROPERTY_R(bool,     _maximized,   Maximized)
	PROPERTY_R(bool,     _resizing,    Resizing)
	PROPERTY_R(BitState, _mouseIsDown, MouseIsDown)
	PROPERTY_R_REF(Attrib,        _attribute,        Attrib)
	PROPERTY_R(UIManager*,        _uiManager,        UIManager)
	PROPERTY_R(TouchDispatcher*,  _touchDispatcher,  TouchDispatcher)
	PROPERTY_R(KeypadDispatcher*, _keypadDispatcher, KeypadDispatcher)
};

NS_VAYO_END

#endif // __VAYO_DEVICE_H__
