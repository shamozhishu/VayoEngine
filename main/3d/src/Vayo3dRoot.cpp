#include "Vayo3dRoot.h"
#include "Vayo3dRenderSystem.h"
#include "Vayo3dSceneManager.h"
#include "Vayo3dSceneNode.h"
#include "Vayo3dCamera.h"
#include "Vayo3dGUI.h"
#include "Vayo3dTextureManager.h"
#include "Vayo3dMaterialManager.h"
#include "Vayo3dMeshManager.h"
#include "VayoConfigManager.h"

NS_VAYO3D_BEGIN

void Root::addRenderSystem(RenderSystem* newRenderer)
{
	if (newRenderer)
		_renderers[newRenderer->getName()] = newRenderer;
}

Root::Root()
	: _curSceneMgr(NULL)
	, _activeRenderer(NULL)
{
	memset(_uiManager, 0, sizeof(_uiManager));
	_textureManager = new TextureManager();
	_materialManager = new MaterialManager();
	_meshManager = new MeshManager();
}

Root::~Root()
{
	for (int i = 0; i <= _maxSupportDevCnt; ++i)
	{
		SAFE_DELETE(_uiManager[i]);
	}

	destroyAllSceneMgrs();
	SAFE_DELETE(_meshManager);
	SAFE_DELETE(_materialManager);
	SAFE_DELETE(_textureManager);
	unloadPlugins();
}

bool Root::launch(Core::Config* config)
{
	do
	{
		IF_FALSE_BREAK(config);
		IF_FALSE_BREAK(ConfigManager::getSingleton().init(config->RootDirectory, ConfigManager::_3D));
		IF_FALSE_BREAK(Core::launch(config));
		Root::Config* rootConfig = dynamic_cast<Root::Config*>(config);
		IF_FALSE_BREAK(rootConfig);
		loadPlugins();
		map<wstring, RenderSystem*>::iterator it = _renderers.find(config->RendererName);
		if (it != _renderers.end())
			_activeRenderer = it->second;
		IF_FALSE_BREAK(_activeRenderer);
		IF_FALSE_BREAK(_activeRenderer->init(rootConfig->AntiAliasFactor, rootConfig->HandleSRGB));
		IF_FALSE_BREAK(_textureManager && _textureManager->init());
		IF_FALSE_BREAK(_materialManager && _materialManager->init());
		IF_FALSE_BREAK(_meshManager && _meshManager->init());
		if (config->MainDeviceAttrib.TurnOnUI)
			openUI(_mainDevice);
		_isLaunched = true;
		return true;
	} while (0);
	_isLaunched = false;
	return false;
}

void Root::resize(Device* dev /*= NULL*/)
{
	if (!dev)
		dev = _activeDevice;
	if (!dev)
		dev = _mainDevice;

	if (_curSceneMgr && _activeRenderer && _activeRenderer->isActive())
	{
		_activeRenderer->setViewpot(Recti(0, 0, dev->getAttrib().ScreenSize._width, dev->getAttrib().ScreenSize._height));
		Camera* pActiveCamera = _curSceneMgr->getActiveCamera();
		if (pActiveCamera)
		{
			float w = dev->getAttrib().ScreenSize._width;
			float h = dev->getAttrib().ScreenSize._height;
			if (0 == dev->getAttrib().ScreenSize._height)
				h = 1.0f;

			if (pActiveCamera->isOrthogonal())
				pActiveCamera->setLens(w, h, pActiveCamera->getNearZ(), pActiveCamera->getFarZ());
			else
				pActiveCamera->setLens(pActiveCamera->getFovY(), w / h, pActiveCamera->getNearZ(), pActiveCamera->getFarZ());
		}
	}
}

void Root::activate(Device* dev /*= NULL*/)
{
	if (_activeRenderer)
		_activeRenderer->restoreContext();
}

bool Root::fireFrameRendering(Device* renderWnd /*= NULL*/)
{
	if (!renderWnd)
		renderWnd = _activeDevice;

	if (!renderWnd)
		return false;

	if (!_activeRenderer->beginScene(true, true, true, renderWnd))
		return false;

	if (_curSceneMgr && _curSceneMgr->getActiveCamera())
	{
		_activeRenderer->removeAllDynamicLights();
		_curSceneMgr->getActiveCamera()->refresh();
		_curSceneMgr->getRootSceneNode()->visit(_timer.deltaTime());
		_curSceneMgr->updateRenderQueue();
	}

	if (!Core::fireFrameRendering(renderWnd))
		return false;

	UIManager* uiMgr = getUIManager(renderWnd);
	if (uiMgr)
		uiMgr->render();

	return _activeRenderer->endScene();
}

void Root::bootFrame(Device* dev, const wstring& scenename /*= L""*/, const wstring& userEvtID /*= L""*/)
{
	SceneManager* sceneMgr = NULL;
	if (!scenename.empty())
		sceneMgr = findSceneMgr(scenename);

	bool isDispatchEvts = false;
	if (sceneMgr && _curSceneMgr != sceneMgr)
	{
		isDispatchEvts = true;
		_curSceneMgr = sceneMgr;
	}

	if (dev && _activeDevice != dev)
	{
		isDispatchEvts = true;
		_activeDevice = dev;
	}

	if (!userEvtID.empty())
		isDispatchEvts = true;

	if (isDispatchEvts)
	{
		vector<wstring> evtIds;
		if (_activeDevice)
			evtIds.push_back(_activeDevice->getDeviceCode());
		if (_curSceneMgr)
			evtIds.push_back(_curSceneMgr->getName());
		if (!userEvtID.empty())
			evtIds.push_back(userEvtID);
		TouchDispatcher::getSingleton().dispatchTouchEvts(evtIds);
		KeypadDispatcher::getSingleton().dispatchKeypadEvts(evtIds);
	}
}

bool Root::openUI(Device* dev /*= NULL*/)
{
	if (!dev)
		dev = _activeDevice;
	if (!dev)
		dev = _mainDevice;

	const_cast<Device::Attrib&>(dev->getAttrib()).TurnOnUI = true;
	int deviceId = dev->getDeviceID();
	if (_uiManager[deviceId])
		return true;

	_uiManager[deviceId] = new UIManager(dev);
	if (!_uiManager[deviceId] || !_uiManager[deviceId]->init())
	{
		const_cast<Device::Attrib&>(dev->getAttrib()).TurnOnUI = false;
		SAFE_DELETE(_uiManager[deviceId]);
		return false;
	}

	return true;
}

void Root::closeUI(Device* dev /*= NULL*/)
{
	if (!dev)
		dev = _activeDevice;
	if (!dev)
		dev = _mainDevice;
	const_cast<Device::Attrib&>(dev->getAttrib()).TurnOnUI = false;
	SAFE_DELETE(_uiManager[dev->getDeviceID()]);
}

UIManager* Root::getUIManager(Device* dev /*= NULL*/) const
{
	if (!dev)
		dev = _activeDevice;
	if (!dev)
		dev = _mainDevice;
	return _uiManager[dev->getDeviceID()];
}

bool Root::configDevice(Device* dev /*= NULL*/)
{
	if (!dev)
		dev = _activeDevice;
	if (!dev)
		dev = _mainDevice;
	if (_activeRenderer)
		return _activeRenderer->setWndPixelFormat(dev);
	return false;
}

SceneManager* Root::createSceneMgr(const wstring& sceneName /*= L""*/)
{
	SceneManager* ret = new SceneManager(sceneName);
	if (!ret)
		return NULL;

	SceneManager* pFindedSceneMgr = findSceneMgr(ret->getName());
	if (pFindedSceneMgr)
		destroySceneMgr(pFindedSceneMgr);
	
	_sceneMgrPool[ret->getName()] = ret;
	return ret;
}

SceneManager* Root::findSceneMgr(const wstring& sceneName)
{
	map<wstring, SceneManager*>::iterator it = _sceneMgrPool.find(sceneName);
	if (it != _sceneMgrPool.end())
		return it->second;
	return NULL;
}

void Root::destroySceneMgr(const wstring& sceneName)
{
	SceneManager* pSceneMgr;
	map<wstring, SceneManager*>::iterator it = _sceneMgrPool.find(sceneName);
	if (it != _sceneMgrPool.end())
	{
		pSceneMgr = it->second;
		if (pSceneMgr == _curSceneMgr)
			_curSceneMgr = NULL;
		delete pSceneMgr;
		_sceneMgrPool.erase(it);
	}
}

void Root::destroySceneMgr(SceneManager* pSceneMgr)
{
	if (pSceneMgr)
		destroySceneMgr(pSceneMgr->getName());
}

void Root::destroyAllSceneMgrs()
{
	map<wstring, SceneManager*>::iterator it = _sceneMgrPool.begin();
	for (; it != _sceneMgrPool.end(); ++it)
		delete it->second;
	_curSceneMgr = NULL;
	_sceneMgrPool.clear();
}

void Root::setNullCurSceneMgr()
{
	_curSceneMgr = NULL;
}

NS_VAYO3D_END
