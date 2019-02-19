#include "D2DSupport.h"
#include "D2DPlugin.h"
#include "VayoPluginSystem.h"

static D2DPlugin* s_plugin;

extern "C" __declspec(dllexport) void dllStartPlugin(PluginSystem* pPluginSystem)
{
	s_plugin = new D2DPlugin();
	pPluginSystem->installPlugin(s_plugin);
}

extern "C" __declspec(dllexport) void dllStopPlugin(PluginSystem* pPluginSystem)
{
	pPluginSystem->uninstallPlugin(s_plugin);
	SAFE_DELETE(s_plugin);
}
