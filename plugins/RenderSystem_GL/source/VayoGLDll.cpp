#include "VayoGLSupport.h"
#include "VayoGLPlugin.h"
#include "VayoRoot.h"

NS_VAYO_BEGIN

static GLPlugin* s_plugin;

extern "C" void _VayoGLExport dllStartPlugin(void)
{
	s_plugin = new GLPlugin();
	Root::getSingleton().installPlugin(s_plugin);
}

extern "C" void _VayoGLExport dllStopPlugin(void)
{
	Root::getSingleton().uninstallPlugin(s_plugin);
	SAFE_DELETE(s_plugin);
}

NS_VAYO_END
