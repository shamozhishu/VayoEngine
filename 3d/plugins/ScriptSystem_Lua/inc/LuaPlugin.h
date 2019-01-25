/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* Lua插件
\*************************************************************************/
#ifndef __LUA_PLUGIN_H__
#define __LUA_PLUGIN_H__

#include "LuaSupport.h"
#include "VayoPlugin.h"

class LuaPlugin : public Plugin
{
public:
	LuaPlugin();
	const wstring& getName() const;
	void install();
	void initialise();
	void shutdown();
	void uninstall();

protected:
	LuaScriptSystem* _scriptSys;
};

#endif // __LUA_PLUGIN_H__
