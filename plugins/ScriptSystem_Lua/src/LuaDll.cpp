#include "LuaSupport.h"
#include "LuaPlugin.h"
#include "VayoPluginSystem.h"

static LuaPlugin* s_plugin;

extern "C" void __declspec(dllexport) dllStartPlugin(PluginSystem* pPluginSystem)
{
	s_plugin = new LuaPlugin();
	pPluginSystem->installPlugin(s_plugin);
}

extern "C" void __declspec(dllexport) dllStopPlugin(PluginSystem* pPluginSystem)
{
	pPluginSystem->uninstallPlugin(s_plugin);
	SAFE_DELETE(s_plugin);
}
