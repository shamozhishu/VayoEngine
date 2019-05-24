/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 应用程序基类
\*************************************************************************/
#ifndef __VAYO_APP_H__
#define __VAYO_APP_H__

#include "VayoLog.h"
#include "VayoCore.h"
#include "VayoInput.h"

NS_VAYO_BEGIN

class App : public TouchDelegate, public KeypadDelegate
{
public:
	App()
	{
		Log::wprint(ELL_DEBUG, L"Vayo Engine version 1.0");
	}

	virtual ~App()
	{
		shutdown();
	}

	template<typename T>
	bool init(Core::Config* config)
	{
		T* pSubclassObj = new T();
		Core* pCore = dynamic_cast<Core*>(pSubclassObj);
		if (nullptr == pCore)
		{
			SAFE_DELETE(pSubclassObj);
			return false;
		}

		if (!pCore->launch(config))
			return false;

		enableTouch(true);
		enableKeypad(true);
		return true;
	}

	virtual void shutdown()
	{
		Core* pCore = Core::getCorePtr();
		if (pCore)
		{
			enableTouch(false);
			enableKeypad(false);
			delete pCore;
		}
	}

	virtual void run()
	{
		if (!Core::getCore().isLaunched())
			return;

		if (!startup())
		{
			Log::wprint(ELL_ERROR, L"App startup failed!");
			cleanup();
			return;
		}

		mainLoop();
		cleanup();
	}

	virtual void mainLoop()
	{
		Core& core = Core::getCore();
		if (!core.isLaunched())
			return;

		Device* pDevice = core.getActiveDevice();
		core.resetFrameStats();
		bool idle;
		while (pDevice->handleEvents(idle))
		{
			if (idle)
			{
				if (pDevice->getAppPaused())
					pDevice->sleep(0, false);
				else if (!pDevice->getAttrib().WndPaint)
					core.renderOneFrame(pDevice);
			}
		}
	}

	virtual bool startup() { return false; }
	virtual void cleanup() {}
};

NS_VAYO_END

#endif // __VAYO_APP_H__
