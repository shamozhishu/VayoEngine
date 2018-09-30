/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* Lua脚本系统
\*************************************************************************/
#ifndef __VAYO_LUA_SCRIPT_SYSTEM_H__
#define __VAYO_LUA_SCRIPT_SYSTEM_H__

#include "VayoLuaSupport.h"
NS_VAYO_BEGIN

class LuaScriptSystem : public ScriptSystem
{
public:
	LuaScriptSystem();
	~LuaScriptSystem();
	EScriptType getScriptType() const;
	ScriptProtocol* createScript(const wstring& name = L"");

private:
	friend class LuaScriptStack;
	std::map<wstring, string> _scriptFileName2ContentCache;
};

NS_VAYO_END

#endif // __VAYO_LUA_SCRIPT_SYSTEM_H__
