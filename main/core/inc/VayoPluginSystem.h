/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 插件系统
\*************************************************************************/
#ifndef __VAYO_PLUGIN_SYSTEM_H__
#define __VAYO_PLUGIN_SYSTEM_H__

#include "VayoSupport.h"
NS_VAYO_BEGIN

class _VayoExport PluginSystem
{
public:
	PluginSystem();
	virtual ~PluginSystem();
	void loadPlugins(EDimension dimension = _3D);
	void initialisePlugins();
	void shutdownPlugins();
	void unloadPlugins();
	void loadPlugin(const wstring& pluginName);
	void unloadPlugin(const wstring& pluginName);
	void installPlugin(Plugin* plugin);
	void uninstallPlugin(Plugin* plugin);
	const vector<Plugin*>& getInstalledPlugins() const;
	PROPERTY_R(DynLibManager*, _dynLibManager, DynLibManager)

private:
	vector<Plugin*> _plugins;
	vector<DynLib*> _pluginLibs;
};

NS_VAYO_END

#endif // __VAYO_PLUGIN_SYSTEM_H__
