#include "VayoApp.h"
#include "VayoLog.h"
#include "VayoDevice.h"

NS_VAYO_BEGIN

void App::run()
{
	Root& root = Root::getSingleton();
	if (!root.launch())
		return;

	if (!startup())
	{
		cleanup();
		return;
	}

	Device* pDevice = root.getDevice();
	root.getTimer().reset();
	bool idle;
	while (pDevice->handleEvents(idle))
	{
		if (idle)
		{
			if (pDevice->getAppPaused())
				pDevice->sleep(0, false);
			else
				root.renderOneFrame();
		}
	}

	cleanup();
}

App::App(const Root::Config& config)
{
	Log::wprint(ELL_DEBUG, L"Vayo Engine version 1.0");
	Root* pRoot = new Root(config);
	if (pRoot)
	{
		pRoot->getTouchDispatcher()->addTouchDelegate(this);
		pRoot->getKeypadDispatcher()->addKeypadDelegate(this);
	}
}

App::~App()
{
	Root* pRoot = Root::getSingletonPtr();
	if (pRoot)
	{
		pRoot->getTouchDispatcher()->removeTouchDelegate(this);
		pRoot->getKeypadDispatcher()->removeKeypadDelegate(this);
		delete pRoot;
	}
}

NS_VAYO_END
