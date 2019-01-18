/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* Lua脚本支持
\*************************************************************************/
#ifndef __VAYO_LUA_SUPPORT_H__
#define __VAYO_LUA_SUPPORT_H__

#include "VayoSupport.h"
#include "VayoScriptSystem.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#ifdef VAYO_LUAPLUGIN_EXPORTS
#define _VayoLuaExport __declspec(dllexport)
#else
#if defined(__MINGW32__)
#define _VayoLuaExport
#else
#define _VayoLuaExport __declspec(dllimport)
#endif
#endif

namespace Vayo
{
	class LuaScriptSystem;
}

#endif // __VAYO_LUA_SUPPORT_H__
