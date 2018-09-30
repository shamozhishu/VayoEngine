#include "VayoLuaSupport.h"
#include "VayoLuaPlugin.h"
#include "VayoRoot.h"

NS_VAYO_BEGIN

static LuaPlugin* s_plugin;

extern "C" void _VayoLuaExport dllStartPlugin(void)
{
	s_plugin = new LuaPlugin();
	Root::getSingleton().installPlugin(s_plugin);
}

extern "C" void _VayoLuaExport dllStopPlugin(void)
{
	Root::getSingleton().uninstallPlugin(s_plugin);
	SAFE_DELETE(s_plugin);
}

NS_VAYO_END
