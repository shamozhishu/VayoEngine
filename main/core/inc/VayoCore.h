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

	virtual void resize(Device* dev = nullptr) = 0;
	virtual void activate(Device* dev = nullptr) = 0;
	virtual bool renderOneFrame(Device* renderWnd = nullptr);
	virtual bool fireFrameStarted();
	virtual bool fireFrameRendering(Device* renderWnd = nullptr);
	virtual bool fireFrameEnded();
	virtual void addFrameListener(FrameListener* newListener);
	virtual void removeFrameListener(FrameListener* oldListener);
	virtual void bootFrame(Device* dev, const wstring& scenename = L"", const wstring& userEvtID = L"") = 0;
	virtual bool openUI(Device* dev = nullptr) = 0;
	virtual void closeUI(Device* dev = nullptr) = 0;
	virtual bool configDevice(Device* dev = nullptr) = 0;

	virtual Device* createDevice(void* wndHandle = nullptr, bool wndQuit = true, bool wndPaint = false,
		wstring wndCaption = L"Vayo Engine", bool turnOnUI = true, bool fullScreen = false,
		unsigned bgClearColor = 0xff000000, Dimension2di screenSize = Dimension2di(1280, 720));
	virtual Device* findDevice(unsigned int idx);
	virtual void    destroyDevice(unsigned int idx);
	virtual void    destroyDevice(Device* dev);
	virtual void    destroyAllDevices();
	static int      getMaxSupportDevCnt();

protected:
	void updateFrameStats();
	void syncAddedRemovedFrameListeners();
	PROPERTY_R(Device*, _mainDevice,   MainDevice)
	PROPERTY_R(Device*, _activeDevice, ActiveDevice)

protected:
	Timer               _timer;
	int                 _frameCnt;
	float               _msOneFrame;
	bool                _isLaunched;
	set<FrameListener*> _frameListeners;
	set<FrameListener*> _addedFrameListeners;
	set<FrameListener*> _removedFrameListeners;
	static const int    _maxSupportDevCnt = 8;
	Device*             _multiDevices[_maxSupportDevCnt];
};

NS_VAYO_END

#endif // __VAYO_CORE_H__
