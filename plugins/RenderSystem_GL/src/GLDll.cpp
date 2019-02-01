#include "GLSupport.h"
#include "GLPlugin.h"
#include "VayoPluginSystem.h"

static GLPlugin* s_plugin;

extern "C" __declspec(dllexport) void dllStartPlugin(PluginSystem* pPluginSystem)
{
	s_plugin = new GLPlugin();
	pPluginSystem->installPlugin(s_plugin);
}

extern "C" __declspec(dllexport) void dllStopPlugin(PluginSystem* pPluginSystem)
{
	pPluginSystem->uninstallPlugin(s_plugin);
	SAFE_DELETE(s_plugin);
}
