#include "LuaPlugin.h"
#include "LuaScriptSystem.h"

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
}
