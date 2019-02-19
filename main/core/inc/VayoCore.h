/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 引擎入口
\*************************************************************************/
#ifndef __VAYO_CORE_H__
#define __VAYO_CORE_H__

#include "VayoSupport.h"
#include "VayoTimer.h"
#include "VayoDevice.h"
NS_VAYO_BEGIN

class _VayoExport Core
{
public:
	typedef struct tagConfig
	{
		wstring        RendererName;
		wstring        RootDirectory;
		Device::Attrib MainDeviceAttrib;
		virtual ~tagConfig() {}
	} Config;

public:
	Core();
	virtual ~Core();
	static Core& getCore();

	virtual bool launch(Config* config);
	Timer&       getTimer() { return _timer; }
	const Timer& getTimer() const { return _timer; }
	int          getFrameCnt() const { return _frameCnt; }
	float        getMsOneFrame() const { return _msOneFrame; }
	bool         isLaunched() const { return _isLaunched; }

	virtual void resize(Device* dev = NULL) = 0;
	virtual void activate(Device* dev = NULL) = 0;
	virtual bool renderOneFrame(Device* renderWnd = NULL) = 0;
	virtual void bootFrame(Device* dev, const wstring& scenename = L"", const wstring& userEvtID = L"") = 0;
	virtual bool openUI(Device* dev = NULL) = 0;
	virtual void closeUI(Device* dev = NULL) = 0;
	virtual bool configDevice(Device* dev = NULL) = 0;

	virtual Device* createDevice(void* wndHandle = NULL, bool wndQuit = true, bool wndPaint = false,
		wstring wndCaption = L"Vayo Engine", bool turnOnUI = true, bool fullScreen = false,
		unsigned bgClearColor = 0xff000000, Dimension2di screenSize = Dimension2di(1280, 720));
	virtual Device* findDevice(unsigned int idx);
	virtual void    destroyDevice(unsigned int idx);
	virtual void    destroyDevice(Device* dev);
	virtual void    destroyAllDevices();
	static int      getMaxSupportDevCnt();

protected:
	void updateFrameStats();
	PROPERTY_R(Device*, _mainDevice,   MainDevice)
	PROPERTY_R(Device*, _activeDevice, ActiveDevice)

protected:
	Timer            _timer;
	int              _frameCnt;
	float            _msOneFrame;
	bool             _isLaunched;
	static const int _maxSupportDevCnt = 8;
	Device*          _multiDevices[_maxSupportDevCnt];
};

NS_VAYO_END

#endif // __VAYO_CORE_H__
