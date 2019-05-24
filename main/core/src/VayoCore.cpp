#include "VayoCore.h"
#include "VayoLog.h"
#include "VayoLanguage.h"
#include "VayoConfigManager.h"
#include "VayoFrameListener.h"
#include "VayoDatabaseCSV.h"
#include "VayoWin32Device.h"

NS_VAYO_BEGIN

static Core* s_core = nullptr;
Core::Core()
	: _isLaunched(false)
	, _mainDevice(nullptr)
	, _activeDevice(nullptr)
	, _frameCnt(0u)
	, _msOneFrame(0.0f)
	, _timeElapsed(0.0f)
	, _curFrameCnt(0u)
{
	s_core = this;
	memset(_multiDevices, 0, sizeof(_multiDevices));
	new Language();
	new DatabaseCSV();
	new ConfigManager();
	new TouchDispatcher();
	new KeypadDispatcher();
}

Core::~Core()
{
	destroyAllDevices();
	SAFE_DELETE(_mainDevice);
	delete Language::getSingletonPtr();
	delete DatabaseCSV::getSingletonPtr();
	delete ConfigManager::getSingletonPtr();
	delete TouchDispatcher::getSingletonPtr();
	delete KeypadDispatcher::getSingletonPtr();
	s_core = nullptr;
}

Core& Core::getCore()
{
	if (!s_core)
	{
		VAYO_ASSERT(s_core);
	}
	return *s_core;
}

Core* Core::getCorePtr()
{
	return s_core;
}

bool Core::launch(Config* config)
{
	do 
	{
		IF_FALSE_BREAK(config);
		IF_FALSE_BREAK(DatabaseCSV::getSingleton().init());
		IF_FALSE_BREAK(Language::getSingleton().init());
		_activeDevice = _mainDevice = new Win32Device(EDID_MAIN_DEVICE, config->MainDeviceAttrib);
		IF_FALSE_BREAK(_mainDevice && _mainDevice->init());
		_isLaunched = true;
		return true;
	} while (0);
	_isLaunched = false;
	return false;
}

void Core::resetFrameStats()
{
	_timer.reset();
	_frameCnt = 0u;
	_msOneFrame = 0.0f;
	_timeElapsed = 0.0f;
	_curFrameCnt = 0u;
}

void Core::resize(Device* dev /*= nullptr*/)
{
}

void Core::activate(Device* dev /*= nullptr*/)
{
}

bool Core::renderOneFrame(Device* renderWnd /*= nullptr*/)
{
	if (!fireFrameStarted())
		return false;
	if (!fireFrameRendering(renderWnd))
		return false;
	return fireFrameEnded();
}

bool Core::fireFrameStarted()
{
	_timer.tick();
	updateFrameStats();
	syncAddedRemovedFrameListeners();
	for (set<FrameListener*>::iterator it = _frameListeners.begin(); it != _frameListeners.end(); ++it)
		if (!(*it)->frameStarted(_timer.deltaTime()))
			return false;
	return true;
}

bool Core::fireFrameRendering(Device* renderWnd /*= nullptr*/)
{
	syncAddedRemovedFrameListeners();
	for (set<FrameListener*>::iterator it = _frameListeners.begin(); it != _frameListeners.end(); ++it)
		if (!(*it)->frameRendering(_timer.deltaTime()))
			return false;
	return true;
}

bool Core::fireFrameEnded()
{
	syncAddedRemovedFrameListeners();
	for (set<FrameListener*>::iterator it = _frameListeners.begin(); it != _frameListeners.end(); ++it)
		if (!(*it)->frameEnded(_timer.deltaTime()))
			return false;
	return true;
}

void Core::addFrameListener(FrameListener* newListener)
{
	_removedFrameListeners.erase(newListener);
	_addedFrameListeners.insert(newListener);
}

void Core::removeFrameListener(FrameListener* oldListener)
{
	_addedFrameListeners.erase(oldListener);
	_removedFrameListeners.insert(oldListener);
}

bool Core::openUI(Device* dev /*= nullptr*/)
{
	return false;
}

void Core::closeUI(Device* dev /*= nullptr*/)
{
}

bool Core::configDevice(Device* dev /*= nullptr*/)
{
	return true;
}

Device* Core::createDevice(void* wndHandle /*= nullptr*/, bool wndQuit /*= true*/,
	bool wndPaint /*= false*/, wstring wndCaption /*= L"Vayo Engine"*/,
	bool turnOnUI /*= true*/, bool fullScreen /*= false*/,
	unsigned bgClearColor /*= 0xff000000*/, Dimension2di screenSize /*= Dimension2di(1280, 720)*/)
{
	if (!_isLaunched)
		return nullptr;

	for (int i = 0; i < _maxSupportDevCnt; ++i)
	{
		if (nullptr == _multiDevices[i])
		{
			Device::Attrib devAttrib;
			devAttrib.WndHandle = wndHandle;
			devAttrib.WndQuit = wndQuit;
			devAttrib.WndPaint = wndPaint;
			devAttrib.WndCaption = wndCaption;
			devAttrib.TurnOnUI = turnOnUI;
			devAttrib.FullScreen = fullScreen;
			devAttrib.BgClearColor = bgClearColor;
			devAttrib.ScreenSize = screenSize;

			Win32Device* dev = new Win32Device((EDeviceID)i, devAttrib);
			if (nullptr == dev || !dev->init())
			{
				SAFE_DELETE(dev);
				return nullptr;
			}

			if (!configDevice(dev))
			{
				SAFE_DELETE(dev);
				return nullptr;
			}

			if (dev->getAttrib().TurnOnUI)
				openUI(dev);

			_multiDevices[i] = dev;
			return _multiDevices[i];
		}
	}

	Log::wprint(ELL_WARNING, L"达到支持设备数量的最大上限值[%d]，无法创建新的窗口设备！", _maxSupportDevCnt);
	return nullptr;
}

Device* Core::findDevice(unsigned int idx)
{
	if (idx >= (unsigned int)_maxSupportDevCnt)
		return _mainDevice;
	return _multiDevices[idx];
}

void Core::destroyDevice(unsigned int idx)
{
	if (idx >= (unsigned int)_maxSupportDevCnt)
		return;
	if (_multiDevices[idx] == _activeDevice)
		_activeDevice = nullptr;
	SAFE_DELETE(_multiDevices[idx]);
}

void Core::destroyDevice(Device* dev)
{
	if (dev)
	{
		for (int i = 0; i < _maxSupportDevCnt; ++i)
		{
			if (_multiDevices[i] == dev)
			{
				SAFE_DELETE(_multiDevices[i]);
				break;
			}
		}

		if (_activeDevice == dev && _activeDevice != _mainDevice)
			_activeDevice = nullptr;
	}
}

void Core::destroyAllDevices()
{
	if (_activeDevice != _mainDevice)
		_activeDevice = nullptr;
	for (int i = 0; i < _maxSupportDevCnt; ++i)
	{
		SAFE_DELETE(_multiDevices[i]);
	}
}

int Core::getMaxSupportDevCnt()
{
	return _maxSupportDevCnt;
}

void Core::updateFrameStats()
{
	++_curFrameCnt;
	if ((_timer.totalTime() - _timeElapsed) >= 1.0f)
	{
		_msOneFrame = 1000.0f / (float)_curFrameCnt;
		_frameCnt = _curFrameCnt;
		_curFrameCnt = 0;
		_timeElapsed += 1.0f;
	}
}

void Core::syncAddedRemovedFrameListeners()
{
	set<FrameListener*>::iterator it = _removedFrameListeners.begin();
	for (; it != _removedFrameListeners.end(); ++it)
		_frameListeners.erase(*it);
	_removedFrameListeners.clear();

	it = _addedFrameListeners.begin();
	for (; it != _addedFrameListeners.end(); ++it)
		_frameListeners.insert(*it);
	_addedFrameListeners.clear();
}

NS_VAYO_END
