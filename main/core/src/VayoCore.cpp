#include "VayoCore.h"
#include "VayoLog.h"
#include "VayoLanguage.h"
#include "VayoConfigManager.h"
#include "VayoDatabaseCSV.h"
#include "VayoWin32Device.h"

NS_VAYO_BEGIN

Core* s_core = NULL;
Core::Core()
	: _isLaunched(false)
	, _mainDevice(NULL)
	, _activeDevice(NULL)
	, _frameCnt(0)
	, _msOneFrame(0.0f)
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
	s_core = NULL;
}

Core& Core::getCore()
{
	if (!s_core)
	{
		VAYO_ASSERT(s_core);
	}
	return *s_core;
}

bool Core::launch(Config* config)
{
	do 
	{
		IF_FALSE_BREAK(config);
		IF_FALSE_BREAK(ConfigManager::getSingleton().init(config->RootDirectory));
		IF_FALSE_BREAK(DatabaseCSV::getSingleton().init());
		IF_FALSE_BREAK(Language::getSingleton().init());
		_activeDevice = _mainDevice = new Win32Device(_maxSupportDevCnt, config->MainDeviceAttrib);
		IF_FALSE_BREAK(_mainDevice && _mainDevice->init());
		_isLaunched = true;
		return true;
	} while (0);
	_isLaunched = false;
	return false;
}

Device* Core::createDevice(void* wndHandle /*= NULL*/, bool wndQuit /*= true*/,
	bool wndPaint /*= false*/, wstring wndCaption /*= L"Vayo Engine"*/,
	bool turnOnUI /*= true*/, bool fullScreen /*= false*/,
	unsigned bgClearColor /*= 0xff000000*/, Dimension2di screenSize /*= Dimension2di(1280, 720)*/)
{
	if (!_isLaunched)
		return NULL;

	for (int i = 0; i < _maxSupportDevCnt; ++i)
	{
		if (NULL == _multiDevices[i])
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

			Win32Device* dev = new Win32Device(i, devAttrib);
			if (NULL == dev || !dev->init())
			{
				SAFE_DELETE(dev);
				return NULL;
			}

			if (!setPixelFormat(dev))
			{
				SAFE_DELETE(dev);
				return NULL;
			}

			if (dev->getAttrib().TurnOnUI)
				openUI(dev);

			_multiDevices[i] = dev;
			return _multiDevices[i];
		}
	}

	Log::wprint(ELL_WARNING, L"达到支持设备数量的最大上限值[%d]，无法创建新的窗口设备！", _maxSupportDevCnt);
	return NULL;
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
		_activeDevice = NULL;
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
			_activeDevice = NULL;
	}
}

void Core::destroyAllDevices()
{
	if (_activeDevice != _mainDevice)
		_activeDevice = NULL;
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
	static float timeElapsed = 0.0f;
	static int curFrameCnt = 0;
	curFrameCnt++;
	if ((_timer.totalTime() - timeElapsed) >= 1.0f)
	{
		_msOneFrame = 1000.0f / (float)curFrameCnt;
		_frameCnt = curFrameCnt;
		curFrameCnt = 0;
		timeElapsed += 1.0f;
	}
}

NS_VAYO_END
