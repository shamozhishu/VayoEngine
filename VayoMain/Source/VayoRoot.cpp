#include "VayoRoot.h"
#include "VayoRenderSystem.h"
#include "VayoScriptSystem.h"
#include "VayoConfigManager.h"
#include "VayoDatabaseCSV.h"
#include "VayoLanguage.h"
#include "VayoSceneManager.h"
#include "VayoLog.h"
#include "VayoApp.h"
#include "VayoSceneNode.h"
#include "VayoCamera.h"
#include "VayoCoreGUI.h"
#include "VayoTextureManager.h"
#include "VayoMaterialManager.h"
#include "VayoMeshManager.h"
#include "VayoDynLibManager.h"
#include "VayoDynLib.h"
#include "VayoPlugin.h"

NS_VAYO_BEGIN

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
	_dynLibManager = new DynLibManager();
	_configManager = new ConfigManager();
	_csvDatabase = new DatabaseCSV();
	_language = new Language();
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
	SAFE_DELETE(_language);
	SAFE_DELETE(_csvDatabase);
	SAFE_DELETE(_configManager);
	destroyAllDevices();
	SAFE_DELETE(_mainDevice);
	unloadPlugins();
	SAFE_DELETE(_dynLibManager);
}

bool Root::launch(const Config& config)
{
	do
	{
		IF_FALSE_BREAK(_configManager && _configManager->init(config.RootDirectory));
		IF_FALSE_BREAK(_csvDatabase && _csvDatabase->init());
		IF_FALSE_BREAK(_language && _language->init());
		loadPlugins();
		map<wstring, RenderSystem*>::iterator it = _renderers.find(config.RendererName);
		if (it != _renderers.end())
			_activeRenderer = it->second;
		IF_FALSE_BREAK(_activeRenderer);
		_activeDevice = _mainDevice = _activeRenderer->createDevice(config.MainDeviceAttrib);
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
	map<wstring, SceneManager*>::iterator it = _sceneMgrPool.find(sceneName);
	if (it != _sceneMgrPool.end())
	{
		delete it->second;
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
	_sceneMgrPool.clear();
}

Device* Root::createDevice(void* wndHandle /*= NULL*/, bool wndQuit /*= true*/, bool wndPaint /*= false*/,
	wstring wndCaption /*= L"Vayo Engine"*/, bool turnOnUI /*= true*/, bool fullScreen /*= false*/,
	Colour bgClearColor /*= 0xff000000*/, Dimension2di screenSize /*= Dimension2di(1280, 720)*/)
{
	if (_activeRenderer)
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

		Device* dev = _activeRenderer->createDevice(devAttrib);
		for (int i = 0; i < _maxSupportDevCnt; ++i)
		{
			if (NULL == _multiDevices[i])
			{
				_multiDevices[i] = dev;
				return dev;
			}
		}

		Log::wprint(ELL_WARNING, L"达到支持设备数量的最大上限值[%d]，无法创建新的窗口设备！", _maxSupportDevCnt);
		SAFE_DELETE(dev);
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
	SAFE_DELETE(_multiDevices[idx]);
}

void Root::destroyAllDevices()
{
	for (int i = 0; i < _maxSupportDevCnt; ++i)
	{
		SAFE_DELETE(_multiDevices[i]);
	}
}

int Root::getMaxSupportDevCnt()
{
	return _maxSupportDevCnt;
}

UIManager* Root::getUIManager() const
{
	if (_activeDevice)
		return _activeDevice->getUIManager();
	return _mainDevice->getUIManager();
}

TouchDispatcher* Root::getTouchDispatcher() const
{
	if (_activeDevice)
		return _activeDevice->getTouchDispatcher();
	return _mainDevice->getTouchDispatcher();
}

KeypadDispatcher* Root::getKeypadDispatcher() const
{
	if (_activeDevice)
		return _activeDevice->getKeypadDispatcher();
	return _mainDevice->getKeypadDispatcher();
}

void Root::loadPlugins()
{
	const vector<tagPluginConfig>& pluginList = _configManager->getPluginConfig();
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

NS_VAYO_END
