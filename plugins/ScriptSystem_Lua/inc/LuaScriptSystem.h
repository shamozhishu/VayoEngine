/*************************************************************************\
* ��������1.0
* Copyright (c) 2018-2019 authored by ��Ӻ�
* Lua�ű�ϵͳ
\*************************************************************************/
#ifndef __LUA_SCRIPT_SYSTEM_H__
#define __LUA_SCRIPT_SYSTEM_H__

#include "LuaSupport.h"

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

#endif // __LUA_SCRIPT_SYSTEM_H__
