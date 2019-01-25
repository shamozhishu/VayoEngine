#include "Vayo3dRoot.h"
#include "Vayo3dRenderSystem.h"
#include "VayoScriptSystem.h"
#include "VayoConfigManager.h"
#include "VayoDatabaseCSV.h"
#include "VayoLanguage.h"
#include "Vayo3dSceneManager.h"
#include "VayoLog.h"
#include "Vayo3dApp.h"
#include "Vayo3dSceneNode.h"
#include "Vayo3dCamera.h"
#include "Vayo3dCoreGUI.h"
#include "Vayo3dTextureManager.h"
#include "Vayo3dMaterialManager.h"
#include "Vayo3dMeshManager.h"
#include "VayoDynLibManager.h"
#include "VayoDynLib.h"
#include "VayoPlugin.h"

NS_VAYO3D_BEGIN

typedef void(*DLL_START_PLUGIN)(void);
typedef void(*DLL_STOP_PLUGIN)(void);
//-------------------------------------------------------------------------
// Root class.
//-------------------------------------------------------------------------
Root::Root()
	: _isLaunched(false)
	, _curSceneMgr(NULL)
	, _scriptSystem(NULL)
	, _mainDevice(NULL)
	, _activeDevice(NULL)
	, _activeRenderer(NULL)
	, _frameCnt(0)
	, _msOneFrame(0.0f)
{
	memset(_multiDevices, 0, sizeof(_multiDevices));

	new Language();
	new DatabaseCSV();
	new ConfigManager();
	new TouchDispatcher();
	new KeypadDispatcher();

	_dynLibManager = new DynLibManager();
	_textureManager = new TextureManager();
	_materialManager = new MaterialManager();
	_meshManager = new MeshManager();
}

Root::~Root()
{
	destroyAllSceneMgrs();
	SAFE_DELETE(_meshManager);
	SAFE_DELETE(_materialManager);
	SAFE_DELETE(_textureManager);

	destroyAllDevices();
	SAFE_DELETE(_mainDevice);
	unloadPlugins();
	SAFE_DELETE(_dynLibManager);

	delete Language::getSingletonPtr();
	delete DatabaseCSV::getSingletonPtr();
	delete ConfigManager::getSingletonPtr();
	delete TouchDispatcher::getSingletonPtr();
	delete KeypadDispatcher::getSingletonPtr();
}

bool Root::launch(const Config& config)
{
	do
	{
		IF_FALSE_BREAK(ConfigManager::getSingleton().init(config.RootDirectory));
		IF_FALSE_BREAK(DatabaseCSV::getSingleton().init());
		IF_FALSE_BREAK(Language::getSingleton().init());
		loadPlugins();
		map<wstring, RenderSystem*>::iterator it = _renderers.find(config.RendererName);
		if (it != _renderers.end())
			_activeRenderer = it->second;
		IF_FALSE_BREAK(_activeRenderer);
		_activeDevice = _mainDevice = _activeRenderer->createDevice(-1, config.MainDeviceAttrib);
		IF_FALSE_BREAK(_activeDevice);
		IF_FALSE_BREAK(_activeRenderer->init(config.AntiAliasFactor, config.HandleSRGB));
		IF_FALSE_BREAK(_textureManager && _textureManager->init());
		IF_FALSE_BREAK(_materialManager && _materialManager->init());
		IF_FALSE_BREAK(_meshManager && _meshManager->init());
		if (config.MainDeviceAttrib.TurnOnUI)
			_mainDevice->openUI();
		_isLaunched = true;
		return true;

	} while (0);

	return false;
}

bool Root::renderOneFrame(Device* renderWnd /*= NULL*/)
{
	_timer.tick();
	updateFrameStats();

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

	UIManager* uiMgr = renderWnd->getUIManager();
	if (uiMgr)
		uiMgr->render();

	return _activeRenderer->endScene();
}

void Root::bootFrame(SceneManager* scene, Device* dev /*= NULL*/, const wstring& userEvtID /*= L""*/)
{
	bool isDispatchEvts = false;
	if (scene && _curSceneMgr != scene)
	{
		isDispatchEvts = true;
		_curSceneMgr = scene;
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

UIManager* Root::getUIManager() const
{
	if (_activeDevice)
		return _activeDevice->getUIManager();
	return _mainDevice->getUIManager();
}

void Root::addRenderSystem(RenderSystem* newRenderer)
{
	if (newRenderer)
		_renderers[newRenderer->getName()] = newRenderer;
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

Device* Root::createDevice(void* wndHandle /*= NULL*/, bool wndQuit /*= true*/, bool wndPaint /*= false*/,
	wstring wndCaption /*= L"Vayo Engine"*/, bool turnOnUI /*= true*/, bool fullScreen /*= false*/,
	unsigned bgClearColor /*= 0xff000000*/, Dimension2di screenSize /*= Dimension2di(1280, 720)*/)
{
	if (_activeRenderer)
	{
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
				_multiDevices[i] = _activeRenderer->createDevice(i, devAttrib);
				return _multiDevices[i];
			}
		}

		Log::wprint(ELL_WARNING, L"达到支持设备数量的最大上限值[%d]，无法创建新的窗口设备！", _maxSupportDevCnt);
	}

	return NULL;
}

Device* Root::findDevice(unsigned int idx)
{
	if (idx >= _maxSupportDevCnt)
		return _mainDevice;
	return _multiDevices[idx];
}

void Root::destroyDevice(unsigned int idx)
{
	if (idx >= _maxSupportDevCnt)
		return;
	if (_multiDevices[idx] == _activeDevice)
		_activeDevice = NULL;
	SAFE_DELETE(_multiDevices[idx]);
}

void Root::destroyDevice(Device* dev)
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

		if (dev == _activeDevice)
			_activeDevice = NULL;
	}
}

void Root::destroyAllDevices()
{
	for (int i = 0; i < _maxSupportDevCnt; ++i)
	{
		SAFE_DELETE(_multiDevices[i]);
	}

	if (_mainDevice != _activeDevice)
		_activeDevice = NULL;
}

int Root::getMaxSupportDevCnt()
{
	return _maxSupportDevCnt;
}

void Root::loadPlugins()
{
	const vector<tagPluginConfig>& pluginList = ConfigManager::getSingleton().getPluginConfig();
	wstring pluginName;
	vector<tagPluginConfig>::const_iterator it = pluginList.cbegin();
	for (; it != pluginList.cend(); ++it)
	{
		pluginName = (*it).PluginName;
#ifdef _DEBUG
		pluginName += L"_d";
#endif
		loadPlugin(pluginName);
	}
}

void Root::initialisePlugins()
{
	vector<Plugin*>::reverse_iterator it = _plugins.rbegin();
	for (; it != _plugins.rend(); ++it)
	{
		(*it)->initialise();
	}
}

void Root::shutdownPlugins()
{
	vector<Plugin*>::reverse_iterator it = _plugins.rbegin();
	for (; it != _plugins.rend(); ++it)
	{
		(*it)->shutdown();
	}
}

void Root::unloadPlugins()
{
	for (vector<DynLib*>::reverse_iterator it = _pluginLibs.rbegin(); it != _pluginLibs.rend(); ++it)
	{
		DLL_STOP_PLUGIN pFunc = (DLL_STOP_PLUGIN)(*it)->getSymbol("dllStopPlugin");
		pFunc();
		_dynLibManager->unload(*it);
	}
	_pluginLibs.clear();

	for (vector<Plugin*>::reverse_iterator it = _plugins.rbegin(); it != _plugins.rend(); ++it)
	{
		(*it)->uninstall();
	}
	_plugins.clear();
}

void Root::loadPlugin(const wstring& pluginName)
{
	DynLib* lib = _dynLibManager->load(pluginName);
	if (lib)
	{
		_pluginLibs.push_back(lib);
		DLL_START_PLUGIN pFunc = (DLL_START_PLUGIN)lib->getSymbol("dllStartPlugin");
		if (pFunc)
			pFunc();
		else
			Log::wprint(ELL_ERROR, L"Cannot find symbol dllStartPlugin in library %s(Root::loadPlugin)", pluginName.c_str());
	}
}

void Root::unloadPlugin(const wstring& pluginName)
{
	vector<DynLib*>::iterator it = _pluginLibs.begin();
	for (; it != _pluginLibs.end(); ++it)
	{
		if ((*it)->getName() == pluginName)
		{
			DLL_STOP_PLUGIN pFunc = (DLL_STOP_PLUGIN)(*it)->getSymbol("dllStopPlugin");
			pFunc();
			_dynLibManager->unload(*it);
			_pluginLibs.erase(it);
			return;
		}
	}
}

void Root::installPlugin(Plugin* plugin)
{
	Log::wprint(ELL_DEBUG, L"Installing plugin: %s", plugin->getName().c_str());
	_plugins.push_back(plugin);
	plugin->install();
	plugin->initialise();
	Log::wprint(ELL_DEBUG, L"Plugin successfully installed");
}

void Root::uninstallPlugin(Plugin* plugin)
{
	Log::wprint(ELL_DEBUG, L"Uninstalling plugin: %s", plugin->getName().c_str());
	vector<Plugin*>::iterator it = std::find(_plugins.begin(), _plugins.end(), plugin);
	if (it != _plugins.end())
	{
		plugin->shutdown();
		plugin->uninstall();
		_plugins.erase(it);
	}
	Log::wprint(ELL_DEBUG, L"Plugin successfully uninstalled");
}

const vector<Plugin*>& Root::getInstalledPlugins() const
{
	return _plugins;
}

void Root::updateFrameStats()
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

NS_VAYO3D_END
