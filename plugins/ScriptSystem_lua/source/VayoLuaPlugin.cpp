#include "VayoLuaPlugin.h"
#include "VayoLuaScriptSystem.h"
#include "VayoRoot.h"

NS_VAYO_BEGIN

static const wstring s_pluginName = L"ScriptSystem_lua";

LuaPlugin::LuaPlugin()
	: _scriptSys(NULL)
{
}

const wstring& LuaPlugin::getName() const
{
	return s_pluginName;
}

void LuaPlugin::install()
{
	_scriptSys = new LuaScriptSystem();
	Root::getSingleton().setScriptSystem(_scriptSys);
}

void LuaPlugin::initialise()
{
	// nothing to do.
}

void LuaPlugin::shutdown()
{
	// nothing to do.
}

void LuaPlugin::uninstall()
{
	SAFE_DELETE(_scriptSys);
	Root::getSingleton().setScriptSystem(NULL);
}

NS_VAYO_END
