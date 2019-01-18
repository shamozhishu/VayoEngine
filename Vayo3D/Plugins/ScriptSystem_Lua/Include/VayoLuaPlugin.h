/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* Lua插件
\*************************************************************************/
#ifndef __VAYO_LUA_PLUGIN_H__
#define __VAYO_LUA_PLUGIN_H__

#include "VayoLuaSupport.h"
#include "VayoPlugin.h"
NS_VAYO_BEGIN

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

NS_VAYO_END

#endif // __VAYO_LUA_PLUGIN_H__
