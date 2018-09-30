#include "VayoApp.h"
#include "VayoLog.h"
#include "VayoDevice.h"

NS_VAYO_BEGIN

void App::run()
{
	Root& root = Root::getSingleton();
	if (!root.launch())
		return;

	root.getTouchDispatcher()->addTouchDelegate(this);
	root.getKeypadDispatcher()->addKeypadDelegate(this);

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
			root.getTimer().tick();

			if (pDevice->getAppPaused())
				pDevice->onSleep();
			else
				root.renderOneFrame();
		}
	}

	cleanup();
}

App::App(const Root::InitConfig& initConfig)
{
	Log::wprint(ELL_INFORMATION, L"Vayo Engine version 1.0");
	new Root(initConfig);
}

App::~App()
{
	Root* pRoot = Root::getSingletonPtr();
	pRoot->getTouchDispatcher()->removeTouchDelegate(this);
	pRoot->getKeypadDispatcher()->removeKeypadDelegate(this);
	delete pRoot;
}

NS_VAYO_END
