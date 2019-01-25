#include "LuaSupport.h"
#include "LuaPlugin.h"
#include "Vayo3dRoot.h"

static LuaPlugin* s_plugin;

extern "C" void __declspec(dllexport) dllStartPlugin(void)
{
	s_plugin = new LuaPlugin();
	_3d::Root::getSingleton().installPlugin(s_plugin);
}

extern "C" void __declspec(dllexport) dllStopPlugin(void)
{
	_3d::Root::getSingleton().uninstallPlugin(s_plugin);
	SAFE_DELETE(s_plugin);
}
