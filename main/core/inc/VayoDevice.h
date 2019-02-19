/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 窗口设备
\*************************************************************************/
#ifndef __VAYO_DEVICE_H__
#define __VAYO_DEVICE_H__

#include "VayoSupport.h"
#include "VayoInput.h"
#include "VayoBitState.h"
#include "math/VayoVector2d.h"
#include "math/VayoRectangle.h"
NS_VAYO_BEGIN

enum EDeviceID
{
	EDID_AUX_DEVICE0 = 0,
	EDID_AUX_DEVICE1,
	EDID_AUX_DEVICE2,
	EDID_AUX_DEVICE3,
	EDID_AUX_DEVICE4,
	EDID_AUX_DEVICE5,
	EDID_AUX_DEVICE6,
	EDID_AUX_DEVICE7,
	EDID_MAIN_DEVICE,
	EDID_DEVICE_COUNT
};

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
		unsigned int  BgClearColor;
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
	Device(EDeviceID deviceID, const Attrib& attrib);
	virtual ~Device();
	virtual bool  init() = 0;
	virtual void* getWndHandle() const = 0;
	virtual float getAspectRatio() const;
	virtual bool  handleEvents(bool& idle) = 0;
	virtual void  onResize();
	virtual void  sleep(unsigned int milliSeconds, bool pauseTimer) = 0;
	virtual void  setWndCaption(const wstring& wndCaption);
	virtual void  setScreenSize(const Dimension2di& screenSize);
	virtual void  setBgClearColor(unsigned int bgClr);
	virtual wstring getDeviceCode() const;
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
	PROPERTY_R(EDeviceID,  _deviceID,    DeviceID)
	PROPERTY_R(bool,       _appPaused,   AppPaused)
	PROPERTY_R(bool,       _minimized,   Minimized)
	PROPERTY_R(bool,       _maximized,   Maximized)
	PROPERTY_R(bool,       _resizing,    Resizing)
	PROPERTY_R(BitState,   _mouseIsDown, MouseIsDown)
	PROPERTY_R_REF(Attrib, _attribute,   Attrib)
};

NS_VAYO_END

#endif // __VAYO_DEVICE_H__
