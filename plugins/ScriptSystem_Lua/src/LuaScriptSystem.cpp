#include "LuaScriptSystem.h"
#include "LuaScriptStack.h"

EScriptType LuaScriptSystem::getScriptType() const
{
	return EST_LUA;
}

LuaScriptSystem::LuaScriptSystem()
{
}

LuaScriptSystem::~LuaScriptSystem()
{
}

ScriptProtocol* LuaScriptSystem::createScript(const wstring& name /*= L""*/)
{
	LuaScriptStack* luaStack = new LuaScriptStack(name, this);
	if (!luaStack)
		return NULL;

	ScriptProtocol* pFinded = findScript(luaStack->getScriptName());
	if (pFinded)
	{
		SAFE_DELETE(pFinded);
	}

	_scripts[luaStack->getScriptName()] = luaStack;
	return luaStack;
}
