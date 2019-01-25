#include "GLSupport.h"
#include "GLPlugin.h"
#include "Vayo3dRoot.h"

static GLPlugin* s_plugin;

extern "C" __declspec(dllexport) void dllStartPlugin(void)
{
	s_plugin = new GLPlugin();
	Root::getSingleton().installPlugin(s_plugin);
}

extern "C" __declspec(dllexport) void dllStopPlugin(void)
{
	Root::getSingleton().uninstallPlugin(s_plugin);
	SAFE_DELETE(s_plugin);
}
