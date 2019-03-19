#include "VayoPluginSystem.h"
#include "VayoConfigManager.h"
#include "VayoDynLibManager.h"
#include "VayoDynLib.h"
#include "VayoPlugin.h"
#include "VayoLog.h"

NS_VAYO_BEGIN

typedef void(*DLL_START_PLUGIN)(PluginSystem* pPluginSystem);
typedef void(*DLL_STOP_PLUGIN)(PluginSystem* pPluginSystem);
const vector<Plugin*>& PluginSystem::getInstalledPlugins() const
{
	return _plugins;
}

PluginSystem::PluginSystem()
{
	_dynLibManager = new DynLibManager();
}

PluginSystem::~PluginSystem()
{
	SAFE_DELETE(_dynLibManager);
}

void PluginSystem::loadPlugins(EDimension dimension /*= _3D*/)
{
	wstring pluginName;
	vector<wstring> pluginList;
	if (dimension == _2D)
		pluginList = ConfigManager::getSingleton().getConfig()._2d.plugins;
	else
		pluginList = ConfigManager::getSingleton().getConfig()._3d.plugins;

	vector<wstring>::const_iterator it = pluginList.cbegin();
	for (; it != pluginList.cend(); ++it)
	{
		pluginName = *it;
#ifdef _DEBUG
		pluginName += L"_d";
#endif
		loadPlugin(pluginName);
	}
}

void PluginSystem::initialisePlugins()
{
	vector<Plugin*>::reverse_iterator it = _plugins.rbegin();
	for (; it != _plugins.rend(); ++it)
	{
		(*it)->initialise();
	}
}

void PluginSystem::shutdownPlugins()
{
	vector<Plugin*>::reverse_iterator it = _plugins.rbegin();
	for (; it != _plugins.rend(); ++it)
	{
		(*it)->shutdown();
	}
}

void PluginSystem::unloadPlugins()
{
	for (vector<DynLib*>::reverse_iterator it = _pluginLibs.rbegin(); it != _pluginLibs.rend(); ++it)
	{
		DLL_STOP_PLUGIN pFunc = (DLL_STOP_PLUGIN)(*it)->getSymbol("dllStopPlugin");
		pFunc(this);
		_dynLibManager->unload(*it);
	}
	_pluginLibs.clear();

	for (vector<Plugin*>::reverse_iterator it = _plugins.rbegin(); it != _plugins.rend(); ++it)
	{
		(*it)->uninstall();
	}
	_plugins.clear();
}

void PluginSystem::loadPlugin(const wstring& pluginName)
{
	DynLib* lib = _dynLibManager->load(pluginName);
	if (lib)
	{
		_pluginLibs.push_back(lib);
		DLL_START_PLUGIN pFunc = (DLL_START_PLUGIN)lib->getSymbol("dllStartPlugin");
		if (pFunc)
			pFunc(this);
		else
			Log::wprint(ELL_ERROR, L"Cannot find symbol dllStartPlugin in library %s(Root::loadPlugin)", pluginName.c_str());
	}
}

void PluginSystem::unloadPlugin(const wstring& pluginName)
{
	vector<DynLib*>::iterator it = _pluginLibs.begin();
	for (; it != _pluginLibs.end(); ++it)
	{
		if ((*it)->getName() == pluginName)
		{
			DLL_STOP_PLUGIN pFunc = (DLL_STOP_PLUGIN)(*it)->getSymbol("dllStopPlugin");
			pFunc(this);
			_dynLibManager->unload(*it);
			_pluginLibs.erase(it);
			return;
		}
	}
}

void PluginSystem::installPlugin(Plugin* plugin)
{
	Log::wprint(ELL_DEBUG, L"Installing plugin: %s", plugin->getName().c_str());
	_plugins.push_back(plugin);
	plugin->install();
	plugin->initialise();
	Log::wprint(ELL_DEBUG, L"Plugin successfully installed");
}

void PluginSystem::uninstallPlugin(Plugin* plugin)
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

NS_VAYO_END
