#include "VayoScriptSystem.h"

NS_VAYO_BEGIN

EScriptType ScriptSystem::getScriptType() const
{
	return EST_NONE;
}

ScriptProtocol::ScriptProtocol(const wstring& name)
	: _scriptName(name)
{
}

ScriptSystem::ScriptSystem()
{
}

ScriptSystem::~ScriptSystem()
{
	destroyScripts();
}

ScriptProtocol* ScriptSystem::findScript(const wstring& name) const
{
	map<wstring, ScriptProtocol*>::const_iterator cit = _scripts.find(name);
	if (cit != _scripts.cend())
		return cit->second;
	return NULL;
}

void ScriptSystem::destroyScript(ScriptProtocol* script)
{
	if (script)
		destroyScript(script->getScriptName());
}

void ScriptSystem::destroyScript(const wstring& name)
{
	map<wstring, ScriptProtocol*>::iterator it = _scripts.find(name);
	if (it != _scripts.end())
	{
		delete it->second;
		_scripts.erase(it);
	}
}

void ScriptSystem::destroyScripts()
{
	map<wstring, ScriptProtocol*>::iterator it = _scripts.begin();
	for (; it != _scripts.end(); ++it)
		delete it->second;
	_scripts.clear();
}

NS_VAYO_END
