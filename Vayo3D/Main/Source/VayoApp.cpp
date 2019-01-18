#include "VayoApp.h"
#include "VayoLog.h"
#include "VayoDevice.h"

NS_VAYO_BEGIN

void App::run()
{
	Root& root = Root::getSingleton();
	if (!root.IsLaunched())
		return;

	if (!startup())
	{
		cleanup();
		return;
	}

	Device* pDevice = root.getActiveDevice();
	root.getTimer().reset();
	bool idle;
	while (pDevice->handleEvents(idle))
	{
		if (idle)
		{
			if (pDevice->getAppPaused())
				pDevice->sleep(0, false);
			else
				root.renderOneFrame(pDevice);
		}
	}

	cleanup();
}

App::App(const Root::Config& config)
{
	Log::wprint(ELL_DEBUG, L"Vayo Engine version 1.0");
	Root* pRoot = new Root();
	if (pRoot)
	{
		if (!pRoot->launch(config))
			return;
		enableTouch(true);
		enableKeypad(true);
	}
}

App::~App()
{
	Root* pRoot = Root::getSingletonPtr();
	if (pRoot)
	{
		enableTouch(false);
		enableKeypad(false);
		delete pRoot;
	}
}

NS_VAYO_END
